// Header
#include "game_timer.hpp"

Texture GameTimer::calendar_tex;
TexturedVertex2 vertices[8];
float offset1;
float offset2;
int flip_0;
int flip_1;

float flip_rad;
float flip_offset_actual;
const float m_pi = 3.141592653589793;
float old_offset;
float new_offset;
float num_offset;
int invert_backside;

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

	vertices[0].texcoord = { 0.0f, 0.5f, 0.0f };
	vertices[1].texcoord = { 0.1f, 0.5f, 0.0f };
	vertices[2].texcoord = { 0.1f, 0.0f, 0.0f };
	vertices[3].texcoord = { 0.0f, 0.0f, 0.0f };

	uint16_t indices[] = { 0,3,1,1,3,2 };

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
	flip_0 = 0 ;
	flip_1 = 0;
	flip_rad = 0.0f;
	flip_offset_actual = 0.0f;
	old_offset = 0.0f;
	new_offset = 0.1f;
	invert_backside = 1; 
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
	printf("Month: %d Day: %d Year: %d\n", return_time.month, return_time.day, return_time.year);
	return return_time;
}

void GameTimer::SplitSetDateDigits(int date) {
	num_offset = std::fmodf(date, 10) * 0.1f;
	if (new_offset != num_offset) {
		old_offset = new_offset;
		new_offset = num_offset;
		flip_rad = 0;
	}
}

void GameTimer::advance_time(float real_time_seconds_elapsed)
{	
	const int One_Day_Sec = 86400;
	struct tm * adv_time = localtime(&m_current_time);
	adv_time->tm_sec += One_Day_Sec/150;
	m_current_time = mktime(adv_time);

	SplitSetDateDigits(gmtime(&m_current_time)->tm_mday);
	

	flip_rad += 0.05;
	if (flip_rad >= m_pi) {
		flip_rad = m_pi;
	}
	if (flip_rad < m_pi / 2) {
		flip_offset_actual = old_offset;
		invert_backside = 1;
	}else{
		flip_offset_actual = old_offset+0.1;
		invert_backside = -1;
	}

	//flip_next_num = flip_rad / 2 == 0 ? 1: 0;

}

void GameTimer::draw(const mat3& projection) {
	transform_begin();
	transform_translate(m_position);
	transform_rotate(flip_rad);
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
	GLint flip_offset_actual_uloc = glGetUniformLocation(effect.program, "flip_offset_actual");

	GLint color_uloc = glGetUniformLocation(effect.program, "fcolor");
	GLint flip_0_uloc = glGetUniformLocation(effect.program, "flip_0");
	GLint flip_1_uloc = glGetUniformLocation(effect.program, "flip_1");
	GLint invert_backside_uloc = glGetUniformLocation(effect.program, "invert_backside");


	// Setting uniform values to the currently bound program
	glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&transform);
	glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);
	glUniform1f(flip_offset_actual_uloc , flip_offset_actual);
	glUniform1f(date_0_loc, offset1);
	glUniform1f(date_1_loc, offset2);

	float color[] = { 1.f, 1.f, 1.f };
	glUniform3fv(color_uloc, 1, color);
	glUniform1i(flip_0_uloc, flip_0);
	glUniform1i(flip_0_uloc, flip_1);
	glUniform1i(invert_backside_uloc, invert_backside);


	// Drawing!
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
}
