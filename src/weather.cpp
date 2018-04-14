// Header
#include "weather.hpp"
#include "time.h"


// stlib
#include <vector>
#include <algorithm>

Texture Weather::weather_screens;

bool Weather::init()
{
	// Load shared texture
	weather_screens.load_from_file(textures_path("WeatherSplash.png"));

	// The position (0,0) corresponds to the center of the texture
	float wr = weather_screens.width * 0.5;
	float hr = weather_screens.height * 0.5;
	float indiv_splash_width = 1000.f;

	vertices[0].position = { -indiv_splash_width / 2, +hr, 0.f };
	vertices[1].position = { +indiv_splash_width / 2, +hr, 0.f };
	vertices[2].position = { +indiv_splash_width / 2, -hr, 0.f };
	vertices[3].position = { -indiv_splash_width / 2, -hr, 0.f };

	glGenBuffers(1, &mesh.vbo);
	glGenBuffers(1, &mesh.ibo);

	// Vertex Array (Container for Vertex + Index buffer)
	glGenVertexArrays(1, &mesh.vao);
	if (gl_has_errors())
		return false;

	// Loading shaders
	if (!effect.load_from_file(shader_path("textured.vs.glsl"), shader_path("weather.fs.glsl")))
		return false;

	// Setting initial values, scale is negative to make it face the opposite way
	// 1.0 would be as big as the original texture
	m_scale.x = 1.5;
	m_scale.y = 1.5;
	m_position.x = 500;
	m_position.y = 500;
	weather_timer = 0;
	srand(time(NULL));
	game_month = 0; 
	check_month = 0;
	curr_weather_loc = 0;
	return true;
}

// Releases all graphics resources
void Weather::destroy()
{
	glDeleteBuffers(1, &mesh.vbo);
	glDeleteBuffers(1, &mesh.ibo);
	glDeleteVertexArrays(1, &mesh.vao);

	glDeleteShader(effect.vertex);
	glDeleteShader(effect.fragment);
	glDeleteShader(effect.program);
}

void Weather::update(float ms, int level, CurrentTime game_time) {
	if (level >= 3) {
		weather_timer += ms / 1000.f;
		if (fmod(check_month, 12) == game_time.month) {
			game_month = game_time.month;
			determine_sky();
			check_month = game_time.month + 4;
		}
	}
	else {
		SetWeatherTexLocs(0);
	}
}

void Weather::determine_sky() {
	//10% chance night time regardless of month
	if (rand() % 10 == 0) {
		if (curr_weather_loc != 4) SetWeatherTexLocs(4);
	}
	else if (game_month >= 3 && game_month <= 8) {
		//50% summer 
		if (rand() % 10 > 4) {
			if (curr_weather_loc != 1) SetWeatherTexLocs(1);
		}
		//40% fall 
		else if (rand() % 10 <= 3) {
			if (curr_weather_loc != 5) SetWeatherTexLocs(5);
		}
		// deafult clouds
		else {
			if (curr_weather_loc != 6) SetWeatherTexLocs(6);
		}
	}
	else if (game_month > 8 && game_month < 3) {
		//50% winter 
		if (rand() % 10 > 4) {
			if (curr_weather_loc != 3) SetWeatherTexLocs(3);
		}
		//40% spring 
		else if (rand() % 10 <= 3) {
			if (curr_weather_loc != 2) SetWeatherTexLocs(2);
		}
		// default clouds
		else {
			if (curr_weather_loc != 6) SetWeatherTexLocs(6);
		}
	}
}

void Weather::SetWeatherTexLocs(int weather_locs) {
	curr_weather_loc = weather_locs;
	weather_timer = 0;
	// weather_locs: 
	float texture_locs[] = { 
		0.f,       //0, no weather
		1.f / 8.f, //1, summer
		2.f / 8.f, //2, overcast/spring
		3.f / 8.f, //3, winter
		4.f / 8.f, //4, night
		5.f / 8.f, //5, fall
		6.f / 8.f, //6, white clouds part 1
		7.f / 8.f, //7, white clouds part 2, unused
		8.f / 8.f, }; 

	vertices[0].texcoord = { texture_locs[weather_locs], 1.f };//top left
	vertices[1].texcoord = { texture_locs[weather_locs + 1], 1.f };//top right
	vertices[2].texcoord = { texture_locs[weather_locs + 1], 0.f };//bottom right
	vertices[3].texcoord = { texture_locs[weather_locs], 0.f };//bottom left

	// counterclockwise as it's the default opengl front winding direction
	uint16_t indices[] = { 0, 3, 1, 1, 3, 2 };

	// Clearing errors
	gl_flush_errors();

	// Vertex Buffer creation
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(TexturedVertex) * 4, vertices, GL_DYNAMIC_DRAW);

	// Index Buffer creation
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * 6, indices, GL_DYNAMIC_DRAW);
}

void Weather::draw(const mat3& projection)
{
	transform_begin();
	transform_scale(m_scale);
	transform_translate(m_position);
	transform_end();

	// Setting shaders
	glUseProgram(effect.program);

	// Enabling alpha channel for textures
	glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);

	// Getting uniform locations for glUniform* calls
	GLint transform_uloc = glGetUniformLocation(effect.program, "transform");
	GLint color_uloc = glGetUniformLocation(effect.program, "fcolor");
	GLint projection_uloc = glGetUniformLocation(effect.program, "projection");
	GLint weather_time_loc = glGetUniformLocation(effect.program, "weather_time");

	// Setting vertices and indices
	glBindVertexArray(mesh.vao);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);

	// Input data location as in the vertex buffer
	GLint in_position_loc = glGetAttribLocation(effect.program, "in_position");
	GLint in_texcoord_loc = glGetAttribLocation(effect.program, "in_texcoord");

	glEnableVertexAttribArray(in_position_loc);
	glEnableVertexAttribArray(in_texcoord_loc);
	glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void*)0);
	glVertexAttribPointer(in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void*)sizeof(vec3));

	// Enabling and binding texture to slot 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, weather_screens.id);

	// Setting uniform values to the currently bound program
	glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&transform);
	float color[] = { 1.f, 1.f, 1.f };
	glUniform3fv(color_uloc, 1, color);
	glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);
	glUniform1f(weather_time_loc, weather_timer);

	// Drawing!
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);		
}

void Weather::reset() {
	weather_timer = 0;
	game_month = 0;
	curr_weather_loc = 0;
	check_month = 0;
}