#include "remove_intersection.hpp"

// stlib
#include <vector>
#include <algorithm>


Texture RemoveIntersection::spacebar1_texture;
Texture RemoveIntersection::spacebar2_texture;
Texture RemoveIntersection::spacebar_texture;
RemoveIntersection::RemoveIntersection() :
	m_press(0)
{

}

RemoveIntersection::~RemoveIntersection()
{

}

bool RemoveIntersection::init()
{
	show = false;
	m_prev_time = 0.f;
	spacebar1_texture.load_from_file(textures_path("spacebar1.png"));
	spacebar2_texture.load_from_file(textures_path("spacebar2.png"));



	// Load shared texture
	if (!spacebar_texture.is_valid())
	{
		if (!spacebar_texture.load_from_file(textures_path("spacebar1.png")))
		{
			fprintf(stderr, "Failed to load cop texture!");
			return false;
		}
	}

	// The position (0,0) corresponds to the center of the texture
	float wr = spacebar_texture.width * 0.5;
	float hr = spacebar_texture.height * 0.5;

	TexturedVertex vertices[4];
	vertices[0].position = { -wr, +hr, 0.f };
	vertices[0].texcoord = { 0.f, 1.f };//top left
	vertices[1].position = { +wr, +hr, 0.f };
	vertices[1].texcoord = { 1.f, 1.f };//top right
	vertices[2].position = { +wr, -hr, 0.f };
	vertices[2].texcoord = { 1.f, 0.f };//bottom right
	vertices[3].position = { -wr, -hr, 0.f };
	vertices[3].texcoord = { 0.f, 0.f };//bottom left

										// counterclockwise as it's the default opengl front winding direction
	uint16_t indices[] = { 0, 3, 1, 1, 3, 2 };

	// Clearing errors
	gl_flush_errors();

	// Vertex Buffer creation
	glGenBuffers(1, &mesh.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(TexturedVertex) * 4, vertices, GL_STATIC_DRAW);

	// Index Buffer creation
	glGenBuffers(1, &mesh.ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * 6, indices, GL_STATIC_DRAW);

	// Vertex Array (Container for Vertex + Index buffer)
	glGenVertexArrays(1, &mesh.vao);
	if (gl_has_errors())
		return false;

	// Loading shaders
	if (!effect.load_from_file(shader_path("textured.vs.glsl"), shader_path("textured.fs.glsl")))
		return false;

	// Setting initial values, scale is negative to make it face the opposite way
	// 1.0 would be as big as the original texture
	m_scale.x = 0.5;
	m_scale.y = 0.5;
	m_position = { 1000.f, 850.f };
	//m_rotation = 0.f;

	return true;
}

void RemoveIntersection::reset() {
	show = false;
	m_press = 0;
}

void RemoveIntersection::update(float ms, int hit_count, int game_level)
{
	m_prev_time += ms;
	if (m_prev_time >=0 && m_prev_time < 150) {
		spacebar_texture = spacebar1_texture;
	}
	else if(m_prev_time >= 150 && m_prev_time < 300){
		spacebar_texture = spacebar2_texture;
	}
	else {
		m_prev_time = 0;
	}

	if (hit_count > 0) {
		show = true;
	}
	if (m_press == game_level) {
		show = false;
		m_press = 0;
	}

}

void RemoveIntersection::destroy()
{
	glDeleteBuffers(1, &mesh.vbo);
	glDeleteBuffers(1, &mesh.ibo);
	glDeleteVertexArrays(1, &mesh.vao);

	glDeleteShader(effect.vertex);
	glDeleteShader(effect.fragment);
	glDeleteShader(effect.program);
}

void RemoveIntersection::draw(const mat3& projection)
{
	transform_begin();
	transform_scale(m_scale);
	transform_translate(m_position);
	transform_end();

	if (show) {
		// Setting shaders
		glUseProgram(effect.program);

		// Enabling alpha channel for textures
		glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_DEPTH_TEST);

		// Getting uniform locations for glUniform* calls
		GLint transform_uloc = glGetUniformLocation(effect.program, "transform");
		GLint color_uloc = glGetUniformLocation(effect.program, "fcolor");
		GLint projection_uloc = glGetUniformLocation(effect.program, "projection");

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
		glBindTexture(GL_TEXTURE_2D, spacebar_texture.id);

		// Setting uniform values to the currently bound program
		glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&transform);
		float color[] = { 1.f, 1.f, 1.f };
		glUniform3fv(color_uloc, 1, color);
		glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);

		// Drawing!
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
	}
}

void RemoveIntersection::increment() {
	m_press++;
}
