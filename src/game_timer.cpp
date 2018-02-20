// Header
#include "game_timer.hpp"

Texture GameTimer::calendar_tex;
TexturedVertex2 vertices[8];
float m_time;
double m_changetime;
float offset1;
float offset2;
bool GameTimer::init()
{
	struct tm init_time = {0};
	init_time.tm_mday = DaysAfterUnixDate + 1; //mktime uses 1 based indexing for days
	init_time.tm_year = 70; // mktime starts from 1900 for some reason
	m_current_time = mktime(&init_time);

	//load texture
	if (!calendar_tex.is_valid())
	{
		if (!calendar_tex.load_from_file(textures_path("numbers.png")))
		{
			fprintf(stderr, "Failed to load background texture!");
			return false;
		}
	}

	//centre of the texture
	float wr = calendar_tex.width * 0.2f;
	float hr = calendar_tex.height * 0.5f;

	vertices[0].position = { -wr, +hr, 0.f };
	vertices[1].position = { 0, +hr, 0.f };
	vertices[2].position = { 0, -hr, 0.f };
	vertices[3].position = { -wr, -hr, 0.f };
	vertices[4].position = { 0.0f, +hr, 0.f };
	vertices[5].position = { wr, +hr, 0.f };
	vertices[6].position = { wr, -hr, 0.f };
	vertices[7].position = { 0.0f, -hr, 0.f };

	vertices[0].texcoord = { 0.0f, 0.5f, 0.0f };
	vertices[1].texcoord = { 0.1f, 0.5f, 0.0f };
	vertices[2].texcoord = { 0.1f, 0.0f, 0.0f };
	vertices[3].texcoord = { 0.0f, 0.0f, 0.0f };
	vertices[4].texcoord = { 0.0f, 0.5f, 1.0f };
	vertices[5].texcoord = { 0.1f, 0.5f, 1.0f };
	vertices[6].texcoord = { 0.1f, 0.0f, 1.0f };
	vertices[7].texcoord = { 0.0f, 0.0f, 1.0f };

	// counterclockwise as it's the default opengl front winding direction

	uint16_t indices[] = { 0,3,1,1,3,2,4,7,5,5,7,6 };

	// Clearing errors
	gl_flush_errors();

	// Vertex Buffer creation
	glGenBuffers(1, &mesh.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(TexturedVertex2) * 8, vertices, GL_STATIC_DRAW);

	// Index Buffer creation
	glGenBuffers(1, &mesh.ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * 12, indices, GL_STATIC_DRAW);

	// Vertex Array (Container for Vertex + Index buffer)
	glGenVertexArrays(1, &mesh.vao);

	// Loading shaders
	effect.load_from_file(shader_path("gameTimer.vs.glsl"), shader_path("gameTimer.fs.glsl"));

	// Setting initial values, scale is negative to make it face the opposite way
	m_scale.x = 0.25;
	m_scale.y = 0.25;
	m_position = { 500.f, 500.f };
	m_time = 0;
	m_changetime = 0;

	return true;
}

CurrentTime GameTimer::get_current_time()
{
	tm* current_time = gmtime(&m_current_time);
	string month = std::string(get_month_from_index(current_time->tm_mon));
	CurrentTime return_time = {
		current_time->tm_year + 1900,
		current_time->tm_mon,
		current_time->tm_mday
	};
	SplitSetDateDigits(current_time->tm_mday);
	return return_time;
}

void GameTimer::SplitSetDateDigits(int date) {
	offset2 = std::fmodf(date, 10) * 0.1f; 
	offset1 = std::fmodf(date / 10, 10)*0.1f;
}

void GameTimer::advance_time(float real_time_seconds_elapsed)
{
	const int SECONDS_IN_DAY = 86400;


	// Hacky workaround for the fact that mktime requires ints to be passed and ints are overflowy.
	// Works for date ranges in reasonable timeframes, so works for us unless Roy is going to live to be thousands of years old
	double current_seconds_elapsed = difftime(m_current_time, 0);
	int current_days = current_seconds_elapsed / SECONDS_IN_DAY;
	int current_seconds = current_seconds_elapsed - (current_days * SECONDS_IN_DAY);
	int seconds_elapsed = real_time_seconds_elapsed * GameToRealSecondsRatio;
	//printf("seconds_elapsed: %d\n", seconds_elapsed);

	struct tm new_time = {0};
	new_time.tm_sec = seconds_elapsed + current_seconds;
	new_time.tm_mday = current_days + 1;
	new_time.tm_year = 70;

	m_current_time = mktime(&new_time);
}

void GameTimer::update(float ms) {
	//if (m_changetime == 0) {
	//	m_changetime = 2000.0;
	//	offset1 += 0.1;
	//}else{
	//	m_changetime -= 100.0;
	//	if (std::fmod(m_changetime, 1000) == 0) {
	//		offset2 += 0.1;
	//	}
	//	//printf("%f\n", ms);
	//}
	//m_time += 1.0f;
}

void GameTimer::draw(const mat3& projection) {
	transform_begin();
	transform_translate(m_position);
	transform_scale(m_scale);
	transform_end();

	// Setting shaders
	glUseProgram(effect.program);

	// Enabling alpha channel for textures
	glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);

	// Setting vertices and indices
	glBindVertexArray(mesh.vao);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);

	// Input data location as in the vertex buffer
	GLint in_position_loc = glGetAttribLocation(effect.program, "in_position");
	GLint in_texcoord_loc = glGetAttribLocation(effect.program, "in_texcoord");
	glEnableVertexAttribArray(in_position_loc);
	glEnableVertexAttribArray(in_texcoord_loc);
	glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex2), (void*)0);
	glVertexAttribPointer(in_texcoord_loc, 3, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex2), (void*)sizeof(vec3));

	// Enabling and binding texture to slot 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, calendar_tex.id);

	// Getting uniform locations for glUniform* calls
	GLint transform_uloc = glGetUniformLocation(effect.program, "transform");
	GLint projection_uloc = glGetUniformLocation(effect.program, "projection");
	GLint date_0_loc = glGetUniformLocation(effect.program, "date_0_offset");
	GLint date_1_loc = glGetUniformLocation(effect.program, "date_1_offset");

	GLint color_uloc = glGetUniformLocation(effect.program, "fcolor");
	GLint time_uloc = glGetUniformLocation(effect.program, "time");

	// Setting uniform values to the currently bound program
	glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&transform);
	glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);
	glUniform1f(date_0_loc, offset1);
	glUniform1f(date_1_loc, offset2);

	float color[] = { 1.f, 1.f, 1.f };
	glUniform3fv(color_uloc, 1, color);
	glUniform1f(time_uloc, m_time);

	// Drawing!
	glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_SHORT, nullptr);
}
