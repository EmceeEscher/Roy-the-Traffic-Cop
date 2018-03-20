#include "score_display.hpp"

Texture ScoreDisplay::calendar_tex_sd;
TexturedVertex2 score_vertices[36];

float uv_sd;
float i_w_sd;
float s_p_sd;
gt_tracker gt_date_sp;
gt_tracker gt_month_sp;
gt_tracker gt_year_sp;
int date_digit_0_flip_sd = 0;
int date_digit_1_flip_sd = 0;
int month_digit_0_flip_sd = 0;
int month_digit_1_flip_sd = 0;
int year_d0_flip_sd = 0;
int year_d1_flip_sd = 0;
int year_d2_flip_sd = 0;
int year_d3_flip_sd = 0;

float month_d0_shown_offset_sd;
float month_d1_shown_offset_sd;
float date_d0_shown_offset_sd;
float date_d1_shown_offset_sd;
float year_d0_shown_offset_sd;
float year_d1_shown_offset_sd;
float year_d2_shown_offset_sd;
float year_d3_shown_offset_sd;

bool ScoreDisplay::init()
{
	struct tm init_time = { 0 };
	init_time.tm_mday = DaysAfterUnixDate_sd + 1; //mktime uses 1 based indexing for days
	init_time.tm_year = 70; // mktime starts from 1900 for some reason
	m_current_time = mktime(&init_time);

	//load texture
	if (!calendar_tex_sd.is_valid())
	{
		if (!calendar_tex_sd.load_from_file(textures_path("numbers.png")))
		{
			fprintf(stderr, "Failed to load game timer texture!");
			return false;
		}
	}

	//centre of the texture
	uv_sd = 100.f / calendar_tex_sd.width; //texture uv
	i_w_sd = calendar_tex_sd.width / 10.f; //individual number width
	s_p_sd = -600.f; //starting position
	float hr = calendar_tex_sd.height * 0.5;

	//year d0 top -- new
	score_vertices[0].position = { s_p_sd + 8.f * i_w_sd, 0  , 0.f };
	score_vertices[1].position = { s_p_sd + 9.f * i_w_sd, 0  , 0.f };
	score_vertices[2].position = { s_p_sd + 9.f * i_w_sd, -hr, 0.f };
	score_vertices[3].position = { s_p_sd + 8.f * i_w_sd, -hr, 0.f };
	score_vertices[0].texcoord = { uv_sd  * 0.f, 0.5f, 0.5f };
	score_vertices[1].texcoord = { uv_sd  * 1.f, 0.5f, 0.5f };
	score_vertices[2].texcoord = { uv_sd  * 1.f, 0.0f, 0.5f };
	score_vertices[3].texcoord = { uv_sd  * 0.f, 0.0f, 0.5f };

	//year d0 bottom -- old
	score_vertices[4].position = { s_p_sd + 8.f * i_w_sd, +hr , 0.f };
	score_vertices[5].position = { s_p_sd + 9.f * i_w_sd, +hr , 0.f };
	score_vertices[6].position = { s_p_sd + 9.f * i_w_sd, 0	 , 0.f };
	score_vertices[7].position = { s_p_sd + 8.f * i_w_sd, 0	 , 0.f };
	score_vertices[4].texcoord = { uv_sd  * 0.f, 1.0f, 0.55f };
	score_vertices[5].texcoord = { uv_sd  * 1.f, 1.0f, 0.55f };
	score_vertices[6].texcoord = { uv_sd  * 1.f, 0.5f, 0.55f };
	score_vertices[7].texcoord = { uv_sd  * 0.f, 0.5f, 0.55f };

	//year d0 -- rotating
	score_vertices[8].position = { s_p_sd + 8.f * i_w_sd, +hr, 0.5f };
	score_vertices[9].position = { s_p_sd + 9.f * i_w_sd, +hr, 0.5f };
	score_vertices[10].position = { s_p_sd + 9.f * i_w_sd, -hr, 0.5f };
	score_vertices[11].position = { s_p_sd + 8.f * i_w_sd, -hr, 0.5f };
	score_vertices[8].texcoord = { uv_sd * 0, 1.0f, 5.0f };
	score_vertices[9].texcoord = { uv_sd * 1, 1.0f, 5.0f };
	score_vertices[10].texcoord = { uv_sd * 1, 0.0f, 5.0f };
	score_vertices[11].texcoord = { uv_sd * 0, 0.0f, 5.0f };

	//year d1 top -- new
	score_vertices[12].position = { s_p_sd + 7.f * i_w_sd, 0  , 0.f };
	score_vertices[13].position = { s_p_sd + 8.f * i_w_sd, 0  , 0.f };
	score_vertices[14].position = { s_p_sd + 8.f * i_w_sd, -hr, 0.f };
	score_vertices[15].position = { s_p_sd + 7.f * i_w_sd, -hr, 0.f };
	score_vertices[12].texcoord = { uv_sd  * 0.f, 0.5f, 0.6f };
	score_vertices[13].texcoord = { uv_sd  * 1.f, 0.5f, 0.6f };
	score_vertices[14].texcoord = { uv_sd  * 1.f, 0.0f, 0.6f };
	score_vertices[15].texcoord = { uv_sd  * 0.f, 0.0f, 0.6f };

	//year d1 bottom -- old
	score_vertices[16].position = { s_p_sd + 7.f * i_w_sd, +hr , 0.f };
	score_vertices[17].position = { s_p_sd + 8.f * i_w_sd, +hr , 0.f };
	score_vertices[18].position = { s_p_sd + 8.f * i_w_sd, 0	 , 0.f };
	score_vertices[19].position = { s_p_sd + 7.f * i_w_sd, 0	 , 0.f };
	score_vertices[16].texcoord = { uv_sd  * 0.f, 1.0f, 0.66f };
	score_vertices[17].texcoord = { uv_sd  * 1.f, 1.0f, 0.66f };
	score_vertices[18].texcoord = { uv_sd  * 1.f, 0.5f, 0.66f };
	score_vertices[19].texcoord = { uv_sd  * 0.f, 0.5f, 0.66f };

	//year d1 -- rotating
	score_vertices[20].position = { s_p_sd + 7.f * i_w_sd, +hr, 0.6f };
	score_vertices[21].position = { s_p_sd + 8.f * i_w_sd, +hr, 0.6f };
	score_vertices[22].position = { s_p_sd + 8.f * i_w_sd, -hr, 0.6f };
	score_vertices[23].position = { s_p_sd + 7.f * i_w_sd, -hr, 0.6f };
	score_vertices[20].texcoord = { uv_sd * 0, 1.0f, 6.0f };
	score_vertices[21].texcoord = { uv_sd * 1, 1.0f, 6.0f };
	score_vertices[22].texcoord = { uv_sd * 1, 0.0f, 6.0f };
	score_vertices[23].texcoord = { uv_sd * 0, 0.0f, 6.0f };

	//year d2 top -- new
	score_vertices[24].position = { s_p_sd + 6.f * i_w_sd, 0  , 0.f };
	score_vertices[25].position = { s_p_sd + 7.f * i_w_sd, 0  , 0.f };
	score_vertices[26].position = { s_p_sd + 7.f * i_w_sd, -hr, 0.f };
	score_vertices[27].position = { s_p_sd + 6.f * i_w_sd, -hr, 0.f };
	score_vertices[24].texcoord = { uv_sd  * 0.f, 0.5f, 0.7f };
	score_vertices[25].texcoord = { uv_sd  * 1.f, 0.5f, 0.7f };
	score_vertices[26].texcoord = { uv_sd  * 1.f, 0.0f, 0.7f };
	score_vertices[27].texcoord = { uv_sd  * 0.f, 0.0f, 0.7f };

	//year d2 bottom -- old
	score_vertices[28].position = { s_p_sd + 6.f * i_w_sd, +hr , 0.f };
	score_vertices[29].position = { s_p_sd + 7.f * i_w_sd, +hr , 0.f };
	score_vertices[30].position = { s_p_sd + 7.f * i_w_sd, 0	 , 0.f };
	score_vertices[31].position = { s_p_sd + 6.f * i_w_sd, 0	 , 0.f };
	score_vertices[28].texcoord = { uv_sd  * 0.f, 1.0f, 0.77f };
	score_vertices[29].texcoord = { uv_sd  * 1.f, 1.0f, 0.77f };
	score_vertices[30].texcoord = { uv_sd  * 1.f, 0.5f, 0.77f };
	score_vertices[31].texcoord = { uv_sd  * 0.f, 0.5f, 0.77f };

	//year d2 -- rotating
	score_vertices[32].position = { s_p_sd + 6.f * i_w_sd, +hr, 0.7f };
	score_vertices[33].position = { s_p_sd + 7.f * i_w_sd, +hr, 0.7f };
	score_vertices[34].position = { s_p_sd + 7.f * i_w_sd, -hr, 0.7f };
	score_vertices[35].position = { s_p_sd + 6.f * i_w_sd, -hr, 0.7f };
	score_vertices[32].texcoord = { uv_sd * 0, 1.0f, 7.0f };
	score_vertices[33].texcoord = { uv_sd * 1, 1.0f, 7.0f };
	score_vertices[34].texcoord = { uv_sd * 1, 0.0f, 7.0f };
	score_vertices[35].texcoord = { uv_sd * 0, 0.0f, 7.0f };

	uint16_t indices[] = { 
		0,3,1,1,3,2,4,7,5,5,7,6,8,11,9,9,11,10,
		12,15,13,13,15,14,16,19,17,17,19,18,20,23,21,21,23,22,
		24,27,25,25,27,26,28,31,29,29,30,30,32,35,33,33,35,34
	};

	// Clearing errors
	gl_flush_errors();

	// Vertex Buffer creation
	glGenBuffers(1, &mesh.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(TexturedVertex2) * 36, score_vertices, GL_STATIC_DRAW);

	// Index Buffer creation
	glGenBuffers(1, &mesh.ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * 54, indices, GL_STATIC_DRAW);

	// Vertex Array (Container for Vertex + Index buffer)
	glGenVertexArrays(1, &mesh.vao);

	// Loading shaders
	effect.load_from_file(shader_path("gameTimer.vs.glsl"), shader_path("gameTimer.fs.glsl"));

	// Setting initial values, scale is negative to make it face the opposite way
	m_scale.x = 0.4;
	m_scale.y = 0.4;
	m_position = { 100.f, 80.f };

	gt_date_sp.digit_0.old_offset = 0.0f;
	gt_date_sp.digit_0.new_offset = uv_sd * 1;
	gt_date_sp.digit_0.flip = -1;
	date_d0_shown_offset_sd = uv_sd * 1;

	gt_date_sp.digit_1.old_offset = 0.0f;
	gt_date_sp.digit_1.new_offset = 0.0f;
	gt_date_sp.digit_1.flip = -1;
	date_d1_shown_offset_sd = 0.f;

	gt_month_sp.digit_0.old_offset = 0.0f;
	gt_month_sp.digit_0.new_offset = uv_sd * 1;
	gt_month_sp.digit_0.flip = -1;
	month_d0_shown_offset_sd = uv_sd * 1;

	gt_month_sp.digit_1.old_offset = 0.0f;
	gt_month_sp.digit_1.new_offset = 0.0f;
	gt_month_sp.digit_1.flip = -1;
	month_d1_shown_offset_sd = 0.f;

	gt_year_sp.digit_0.old_offset = 0.0f;
	gt_year_sp.digit_0.new_offset = uv_sd * 8;
	gt_year_sp.digit_0.flip = -1;
	year_d0_shown_offset_sd = uv_sd * 8;

	gt_year_sp.digit_1.old_offset = 0.0f;
	gt_year_sp.digit_1.new_offset = uv_sd * 1;
	gt_year_sp.digit_1.flip = -1;
	year_d1_shown_offset_sd = uv_sd * 1;

	gt_year_sp.digit_2.old_offset = 0.0f;
	gt_year_sp.digit_2.new_offset = uv_sd * 0;
	gt_year_sp.digit_2.flip = -1;
	year_d2_shown_offset_sd = uv_sd * 0;

	gt_year_sp.digit_3.old_offset = 0.0f;
	gt_year_sp.digit_3.new_offset = uv_sd * 2;
	gt_year_sp.digit_3.flip = -1;
	year_d3_shown_offset_sd = uv_sd * 2;


	return true;
}

//will we need this for anything else?
CurrentTime_sd ScoreDisplay::get_current_time()
{
	tm* current_time = gmtime(&m_current_time);
	string month = std::string(get_month_from_index(current_time->tm_mon));
	CurrentTime_sd return_time = {
		current_time->tm_year + 1900,
		current_time->tm_mon,
		current_time->tm_mday
	};
	return return_time;
}

void ScoreDisplay::SplitSetDateDigits(int day, gt_tracker* gt_day, int mon, gt_tracker* gt_mon, int year, gt_tracker* gt_year_sp) {
	float date_offset_digit0 = std::fmodf(day, 10) * uv_sd;
	float date_offset_digit1 = std::fmodf(day / 10, 10) * uv_sd;
	float month_offset_digit0 = std::fmodf(mon, 10) * uv_sd;
	float month_offset_digit1 = std::fmodf(mon / 10, 10) * uv_sd;
	float year_offset_d0 = std::fmodf(year, 10) * uv_sd;
	float year_offset_d1 = std::fmodf(year / 10, 10) * uv_sd;
	float year_offset_d2 = std::fmodf(year / 100, 10) * uv_sd;
	float year_offset_d3 = std::fmodf(year / 1000, 10) * uv_sd;

	if (gt_day->digit_0.new_offset != date_offset_digit0) { //need to flip
															//prepare flip
		date_d0_shown_offset_sd = gt_day->digit_0.old_offset;
		gt_day->digit_0.new_offset = date_offset_digit0;
		gt_day->digit_0.old_offset = date_offset_digit0 - uv_sd;
		//flip
		date_digit_0_flip_sd = 1;
		gt_day->digit_0.flip = 1;

	}
	else if (date_digit_0_flip_sd == 1) {//middle of flipping
		gt_day->digit_0.flip -= 0.1;
		if (gt_day->digit_0.flip <= 0) {
			date_d0_shown_offset_sd = gt_day->digit_0.new_offset; //show new_offset once flipped past halfway
		}
		if (gt_day->digit_0.flip <= -1) {
			gt_day->digit_0.flip = -1;
			date_digit_0_flip_sd = 0; //indicate done flipping;
		}

	}
	else if (date_digit_0_flip_sd == 0) {//done flipping, waiting for new flip
		gt_day->digit_0.old_offset = date_offset_digit0;
	}

	if (gt_day->digit_1.new_offset != date_offset_digit1) { //need to flip
		date_d1_shown_offset_sd = gt_day->digit_1.old_offset;
		gt_day->digit_1.new_offset = date_offset_digit1;
		gt_day->digit_1.old_offset = date_offset_digit1 - uv_sd;
		date_digit_1_flip_sd = 1;
		gt_day->digit_1.flip = 1;

	}
	else if (date_digit_1_flip_sd == 1) {//middle of flipping
		gt_day->digit_1.flip -= 0.1;
		if (gt_day->digit_1.flip <= 0) {
			date_d1_shown_offset_sd = gt_day->digit_1.new_offset; //show new_offset once flipped past halfway
		}
		if (gt_day->digit_1.flip <= -1) {
			gt_day->digit_1.flip = -1;
			date_digit_1_flip_sd = 0; //indicate done flipping;
		}

	}
	else if (date_digit_1_flip_sd == 0) {//done flipping, waiting for new flip
		gt_day->digit_1.old_offset = date_offset_digit1;
	}

	if (gt_mon->digit_0.new_offset != month_offset_digit0) { //need to flip
		month_d0_shown_offset_sd = gt_mon->digit_0.old_offset;
		gt_mon->digit_0.new_offset = month_offset_digit0;
		gt_mon->digit_0.old_offset = month_offset_digit0 - uv_sd;
		month_digit_0_flip_sd = 1;
		gt_mon->digit_0.flip = 1;

	}
	else if (month_digit_0_flip_sd == 1) {//middle of flipping
		gt_mon->digit_0.flip -= 0.1;
		if (gt_mon->digit_0.flip <= 0) {
			month_d0_shown_offset_sd = gt_mon->digit_0.new_offset; //show new_offset once flipped past halfway
		}
		if (gt_mon->digit_0.flip <= -1) {
			gt_mon->digit_0.flip = -1;
			month_digit_0_flip_sd = 0; //indicate done flipping;
		}

	}
	else if (month_digit_0_flip_sd == 0) {//done flipping, waiting for new flip
		gt_mon->digit_0.old_offset = month_offset_digit0;
	}

	if (gt_mon->digit_1.new_offset != month_offset_digit1) { //need to flip
		month_d1_shown_offset_sd = gt_mon->digit_1.old_offset;
		gt_mon->digit_1.new_offset = month_offset_digit1;
		gt_mon->digit_1.old_offset = month_offset_digit1 - uv_sd;
		month_digit_1_flip_sd = 1;
		gt_mon->digit_1.flip = 1;

	}
	else if (month_digit_1_flip_sd == 1) {//middle of flipping
		gt_mon->digit_1.flip -= 0.1;
		if (gt_mon->digit_1.flip <= 0) {
			month_d1_shown_offset_sd = gt_mon->digit_1.new_offset; //show new_offset once flipped past halfway
		}
		if (gt_mon->digit_1.flip <= -1) {
			gt_mon->digit_1.flip = -1;
			month_digit_1_flip_sd = 0; //indicate done flipping;
		}

	}
	else if (month_digit_1_flip_sd == 0) {//done flipping, waiting for new flip
		gt_mon->digit_1.old_offset = month_offset_digit1;
	}

	if (gt_year_sp->digit_0.new_offset != year_offset_d0) { //need to flip
		year_d0_shown_offset_sd = gt_year_sp->digit_0.old_offset;
		gt_year_sp->digit_0.new_offset = year_offset_d0;
		gt_year_sp->digit_0.old_offset = year_offset_d0 - uv_sd;
		year_d0_flip_sd = 1;
		gt_year_sp->digit_0.flip = 1;

	}
	else if (year_d0_flip_sd == 1) {//middle of flipping
		gt_year_sp->digit_0.flip -= 0.1;
		if (gt_year_sp->digit_0.flip <= 0) {
			year_d0_shown_offset_sd = gt_year_sp->digit_0.new_offset; //show new_offset once flipped past halfway
		}
		if (gt_year_sp->digit_0.flip <= -1) {
			gt_year_sp->digit_0.flip = -1;
			year_d0_flip_sd = 0; //indicate done flipping;
		}

	}
	else if (year_d0_flip_sd == 0) {//done flipping, waiting for new flip
		gt_year_sp->digit_0.old_offset = year_offset_d0;
	}

	if (gt_year_sp->digit_1.new_offset != year_offset_d1) { //need to flip
		year_d1_shown_offset_sd = gt_year_sp->digit_1.old_offset;
		gt_year_sp->digit_1.new_offset = year_offset_d1;
		gt_year_sp->digit_1.old_offset = year_offset_d1 - uv_sd;
		year_d1_flip_sd = 1;
		gt_year_sp->digit_1.flip = 1;

	}
	else if (year_d1_flip_sd == 1) {//middle of flipping
		gt_year_sp->digit_1.flip -= 0.1;
		if (gt_year_sp->digit_1.flip <= 0) {
			year_d1_shown_offset_sd = gt_year_sp->digit_1.new_offset; //show new_offset once flipped past halfway
		}
		if (gt_year_sp->digit_1.flip <= -1) {
			gt_year_sp->digit_1.flip = -1;
			year_d1_flip_sd = 0; //indicate done flipping;
		}

	}
	else if (year_d1_flip_sd == 0) {//done flipping, waiting for new flip
		gt_year_sp->digit_1.old_offset = year_offset_d1;
	}

	if (gt_year_sp->digit_2.new_offset != year_offset_d2) { //need to flip
		year_d2_shown_offset_sd = gt_year_sp->digit_2.old_offset;
		gt_year_sp->digit_2.new_offset = year_offset_d2;
		gt_year_sp->digit_2.old_offset = year_offset_d2 - uv_sd;
		year_d2_flip_sd = 1;
		gt_year_sp->digit_2.flip = 1;

	}
	else if (year_d2_flip_sd == 1) {//middle of flipping
		gt_year_sp->digit_2.flip -= 0.1;
		if (gt_year_sp->digit_2.flip <= 0) {
			year_d2_shown_offset_sd = gt_year_sp->digit_2.new_offset; //show new_offset once flipped past halfway
		}
		if (gt_year_sp->digit_2.flip <= -1) {
			gt_year_sp->digit_2.flip = -1;
			year_d2_flip_sd = 0; //indicate done flipping;
		}

	}
	else if (year_d2_flip_sd == 0) {//done flipping, waiting for new flip
		gt_year_sp->digit_2.old_offset = year_offset_d2;
	}

	if (gt_year_sp->digit_3.new_offset != year_offset_d3) { //need to flip
		year_d3_shown_offset_sd = gt_year_sp->digit_3.old_offset;
		gt_year_sp->digit_3.new_offset = year_offset_d3;
		gt_year_sp->digit_3.old_offset = year_offset_d3 - uv_sd;
		year_d3_flip_sd = 1;
		gt_year_sp->digit_3.flip = 1;

	}
	else if (year_d3_flip_sd == 1) {//middle of flipping
		gt_year_sp->digit_3.flip -= 0.1;
		if (gt_year_sp->digit_3.flip <= 0) {
			year_d3_shown_offset_sd = gt_year_sp->digit_3.new_offset; //show new_offset once flipped past halfway
		}
		if (gt_year_sp->digit_3.flip <= -1) {
			gt_year_sp->digit_3.flip = -1;
			year_d3_flip_sd = 0; //indicate done flipping;
		}

	}
	else if (year_d3_flip_sd == 0) {//done flipping, waiting for new flip
		gt_year_sp->digit_3.old_offset = year_offset_d3;
	}
}

void ScoreDisplay::advance_time(float real_time_seconds_elapsed)
{
	const int game_sec_per_ms = 332; //sec_in_year/music_length/1000ms
	struct tm * adv_time = localtime(&m_current_time);
	adv_time->tm_sec += (int)(real_time_seconds_elapsed * game_sec_per_ms);
	m_current_time = mktime(adv_time);

	SplitSetDateDigits(gmtime(&m_current_time)->tm_mday, &gt_date_sp,
		gmtime(&m_current_time)->tm_mon + 1, &gt_month_sp,
		gmtime(&m_current_time)->tm_year + 1900, &gt_year_sp);
}

void ScoreDisplay::draw(const mat3& projection) {
	transform_begin();
	transform_translate(m_position);
	transform_scale(m_scale);
	transform_end();

	// Setting shaders
	glUseProgram(effect.program);

	// Enabling alpha channel for textures
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);

	// Setting score_vertices and indices
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
	glBindTexture(GL_TEXTURE_2D, calendar_tex_sd.id);

	// Getting uniform locations for glUniform* calls
	GLint transform_uloc = glGetUniformLocation(effect.program, "transform");
	GLint projection_uloc = glGetUniformLocation(effect.program, "projection");
	GLint date_digit_0_uloc = glGetUniformLocation(effect.program, "date_digit_0");
	GLint date_digit_1_uloc = glGetUniformLocation(effect.program, "date_digit_1");
	GLint month_digit_0_uloc = glGetUniformLocation(effect.program, "month_digit_0");
	GLint month_digit_1_uloc = glGetUniformLocation(effect.program, "month_digit_1");
	GLint year_d0_uloc = glGetUniformLocation(effect.program, "year_d0");
	GLint year_d1_uloc = glGetUniformLocation(effect.program, "year_d1");
	GLint year_d2_uloc = glGetUniformLocation(effect.program, "year_d2");
	GLint year_d3_uloc = glGetUniformLocation(effect.program, "year_d3");

	GLint color_uloc = glGetUniformLocation(effect.program, "fcolor");

	// Setting uniform values to the currently bound program
	glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&transform);
	glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);
	glUniform4f(date_digit_0_uloc, gt_date_sp.digit_0.old_offset, gt_date_sp.digit_0.new_offset, date_d0_shown_offset_sd, gt_date_sp.digit_0.flip);
	glUniform4f(date_digit_1_uloc, gt_date_sp.digit_1.old_offset, gt_date_sp.digit_1.new_offset, date_d1_shown_offset_sd, gt_date_sp.digit_1.flip);
	glUniform4f(month_digit_0_uloc, gt_month_sp.digit_0.old_offset, gt_month_sp.digit_0.new_offset, month_d0_shown_offset_sd, gt_month_sp.digit_0.flip);
	glUniform4f(month_digit_1_uloc, gt_month_sp.digit_1.old_offset, gt_month_sp.digit_1.new_offset, month_d1_shown_offset_sd, gt_month_sp.digit_1.flip);

	glUniform4f(year_d0_uloc, gt_year_sp.digit_0.old_offset, gt_year_sp.digit_0.new_offset, year_d0_shown_offset_sd, gt_year_sp.digit_0.flip);
	glUniform4f(year_d1_uloc, gt_year_sp.digit_1.old_offset, gt_year_sp.digit_1.new_offset, year_d1_shown_offset_sd, gt_year_sp.digit_1.flip);
	glUniform4f(year_d2_uloc, gt_year_sp.digit_2.old_offset, gt_year_sp.digit_2.new_offset, year_d2_shown_offset_sd, gt_year_sp.digit_2.flip);
	glUniform4f(year_d3_uloc, gt_year_sp.digit_3.old_offset, gt_year_sp.digit_3.new_offset, year_d3_shown_offset_sd, gt_year_sp.digit_3.flip);

	float color[] = { 1.f, 1.f, 1.f };
	glUniform3fv(color_uloc, 1, color);



	// Drawing!
	glDrawElements(GL_TRIANGLES, 144, GL_UNSIGNED_SHORT, nullptr);
}

/*
Texture ScoreDisplay::score_tex;
TexturedVertex2 score_vertices[36];

float score_uv;
float score_i_w;
float score_s_p;

int d0_flip = 0;
int d1_flip = 0;
int d2_flip = 0;

gt_tracker gt_score;

float d0_shown_offset;
float d1_shown_offset;
float d2_shown_offset;

bool ScoreDisplay::init()
{
	m_current_score = 0;

	//load texture
	if (!score_tex.is_valid())
	{
		if (!score_tex.load_from_file(textures_path("numbers.png")))
		{
			fprintf(stderr, "Failed to load score display texture!");
			return false;
		}
	}

	//centre of the texture
	score_uv = 100.f / score_tex.width; //texture score_uv
	score_i_w = score_tex.width / 10.f; //individual number width
	score_s_p = -600.f; //starting position
	float hr = score_tex.height * 0.5;

	//digit 0 top -- new
	score_vertices[0].position = { score_s_p + score_i_w	    , 0  , 0.f };
	score_vertices[1].position = { score_s_p + 2 * score_i_w  , 0  , 0.f };
	score_vertices[2].position = { score_s_p + 2 * score_i_w	, -hr, 0.f };
	score_vertices[3].position = { score_s_p + score_i_w		, -hr, 0.f };
	score_vertices[0].texcoord = { score_uv * 0.f, 0.5  , 0.1f };
	score_vertices[1].texcoord = { score_uv * 1.f, 0.5  , 0.1f };
	score_vertices[2].texcoord = { score_uv * 1.f, 0.0f , 0.1f };
	score_vertices[3].texcoord = { score_uv * 0.f, 0.0f , 0.1f };

	//digit 0 bottom -- old
	score_vertices[4].position = { score_s_p + score_i_w		, +hr, 0.f };
	score_vertices[5].position = { score_s_p + 2 * score_i_w   , +hr, 0.f };
	score_vertices[6].position = { score_s_p + 2 * score_i_w	, 0, 0.f };
	score_vertices[7].position = { score_s_p + score_i_w		, 0, 0.f };
	score_vertices[4].texcoord = { score_uv * 0.f, 1.0f,  0.11f };
	score_vertices[5].texcoord = { score_uv * 1.f, 1.0f,  0.11f };
	score_vertices[6].texcoord = { score_uv * 1.f, 0.5f, 0.11f };
	score_vertices[7].texcoord = { score_uv * 0.f, 0.5f,  0.11f };

	//display for digit 0 -- rotating
	score_vertices[8].position = { score_s_p + score_i_w		 , +hr, 0.1f };
	score_vertices[9].position = { score_s_p + 2 * score_i_w  , +hr, 0.1f };
	score_vertices[10].position = { score_s_p + 2 * score_i_w	 , -hr, 0.1f };
	score_vertices[11].position = { score_s_p + score_i_w		 , -hr, 0.1f };
	score_vertices[8].texcoord = { score_uv * 0, 1.0f, 1.0f };
	score_vertices[9].texcoord = { score_uv * 1, 1.0f, 1.0f };
	score_vertices[10].texcoord = { score_uv * 1, 0.0f, 1.0f };
	score_vertices[11].texcoord = { score_uv * 0, 0.0f, 1.0f };


	//digit 1 top -- new
	score_vertices[12].position = { score_s_p      , 0  , 0.f };
	score_vertices[13].position = { score_s_p + score_i_w, 0  , 0.f };
	score_vertices[14].position = { score_s_p + score_i_w, -hr, 0.f };
	score_vertices[15].position = { score_s_p      , -hr, 0.f };
	score_vertices[12].texcoord = { score_uv  * 0.f, 0.5f, 0.2f };
	score_vertices[13].texcoord = { score_uv  * 1.f, 0.5f, 0.2f };
	score_vertices[14].texcoord = { score_uv  * 1.f, 0.0f, 0.2f };
	score_vertices[15].texcoord = { score_uv  * 0.f, 0.0f, 0.2f };

	//digit 1 bottom -- old
	score_vertices[16].position = { score_s_p      , +hr , 0.f };
	score_vertices[17].position = { score_s_p + score_i_w, +hr , 0.f };
	score_vertices[18].position = { score_s_p + score_i_w, 0	 , 0.f };
	score_vertices[19].position = { score_s_p      , 0	 , 0.f };
	score_vertices[16].texcoord = { score_uv  * 0.f, 1.0f, 0.22f };
	score_vertices[17].texcoord = { score_uv  * 1.f, 1.0f, 0.22f };
	score_vertices[18].texcoord = { score_uv  * 1.f, 0.5f, 0.22f };
	score_vertices[19].texcoord = { score_uv  * 0.f, 0.5f, 0.22f };

	//display for digit 1 -- rotating
	score_vertices[20].position = { score_s_p 	, +hr, 0.2f };
	score_vertices[21].position = { score_s_p + score_i_w , +hr, 0.2f };
	score_vertices[22].position = { score_s_p + score_i_w	, -hr, 0.2f };
	score_vertices[23].position = { score_s_p	, -hr, 0.2f };
	score_vertices[20].texcoord = { score_uv * 0, 1.0f, 2.0f };
	score_vertices[21].texcoord = { score_uv * 1, 1.0f, 2.0f };
	score_vertices[22].texcoord = { score_uv * 1, 0.0f, 2.0f };
	score_vertices[23].texcoord = { score_uv * 0, 0.0f, 2.0f };

	//digit 2 top -- new
	score_vertices[24].position = { score_s_p + 3.5f * score_i_w	, 0  , 0.f };
	score_vertices[25].position = { score_s_p + 4.5f * score_i_w , 0  , 0.f };
	score_vertices[26].position = { score_s_p + 4.5f * score_i_w	, -hr, 0.f };
	score_vertices[27].position = { score_s_p + 3.5f * score_i_w	, -hr, 0.f };
	score_vertices[24].texcoord = { score_uv * 0.f, 0.5  , 0.3f };
	score_vertices[25].texcoord = { score_uv * 1.f, 0.5  , 0.3f };
	score_vertices[26].texcoord = { score_uv * 1.f, 0.0f , 0.3f };
	score_vertices[27].texcoord = { score_uv * 0.f, 0.0f , 0.3f };

	//digit 2 bottom -- old
	score_vertices[28].position = { score_s_p + 3.5f * score_i_w	, +hr, 0.f };
	score_vertices[29].position = { score_s_p + 4.5f * score_i_w , +hr, 0.f };
	score_vertices[30].position = { score_s_p + 4.5f * score_i_w	, 0, 0.f };
	score_vertices[31].position = { score_s_p + 3.5f * score_i_w	, 0, 0.f };
	score_vertices[28].texcoord = { score_uv * 0.f, 1.0f,  0.33f };
	score_vertices[29].texcoord = { score_uv * 1.f, 1.0f,  0.33f };
	score_vertices[30].texcoord = { score_uv * 1.f, 0.5f,  0.33f };
	score_vertices[31].texcoord = { score_uv * 0.f, 0.5f,  0.33f };

	//display for digit 2 -- rotating
	score_vertices[32].position = { score_s_p + 3.5f * score_i_w	, +hr, 0.3f };
	score_vertices[33].position = { score_s_p + 4.5f * score_i_w , +hr, 0.3f };
	score_vertices[34].position = { score_s_p + 4.5f * score_i_w	, -hr, 0.3f };
	score_vertices[35].position = { score_s_p + 3.5f * score_i_w	, -hr, 0.3f };
	score_vertices[32].texcoord = { score_uv * 0, 1.0f, 3.0f };
	score_vertices[33].texcoord = { score_uv * 1, 1.0f, 3.0f };
	score_vertices[34].texcoord = { score_uv * 1, 0.0f, 3.0f };
	score_vertices[35].texcoord = { score_uv * 0, 0.0f, 3.0f };


	uint16_t indices[] = { 0,3,1,1,3,2,4,7,5,5,7,6,8,11,9,9,11,10,
		12,15,13,13,15,14,16,19,17,17,19,18,20,23,21,21,23,22,
		24,27,25,25,27,26,28,31,29,29,31,30,32,35,33,33,35,34};

	// Clearing errors
	gl_flush_errors();

	// Vertex Buffer creation
	glGenBuffers(1, &mesh.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(TexturedVertex2) * 36, score_vertices, GL_STATIC_DRAW);

	// Index Buffer creation
	glGenBuffers(1, &mesh.ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * 54, indices, GL_STATIC_DRAW);
	
	// Loading shaders
	effect.load_from_file(shader_path("scoreDisplay.vs.glsl"), shader_path("scoreDisplay.fs.glsl"));

	// Setting initial values
	m_scale.x = 0.4;
	m_scale.y = 0.4;
	m_position = { 215.f, 80.f };

	gt_score.digit_0.old_offset = 0.0f;
	gt_score.digit_0.new_offset = score_uv * 1;
	gt_score.digit_0.flip = -1;
	d0_shown_offset = score_uv * 1;

	gt_score.digit_1.old_offset = 0.0f;
	gt_score.digit_1.new_offset = 0.0f;
	gt_score.digit_1.flip = -1;
	d1_shown_offset = 0.f;

	gt_score.digit_2.old_offset = 0.0f;
	gt_score.digit_2.new_offset = score_uv * 2;
	gt_score.digit_2.flip = -1;
	d2_shown_offset = score_uv * 2;

	return true;
}

void ScoreDisplay::SplitSetScoreDigits(int score, gt_tracker* gt_points) {
	float score_offset_digit0 = std::fmodf(score, 10) * score_uv;
	float score_offset_digit1 = std::fmodf(score / 10, 10) * score_uv;
	float score_offset_digit2 = std::fmodf(score / 100, 10) * score_uv;

	if (gt_points->digit_0.new_offset != score_offset_digit0) { // need to flip
		// prepare flip
		d0_shown_offset = gt_points->digit_0.old_offset;
		gt_points->digit_0.new_offset = score_offset_digit0;
		gt_points->digit_0.old_offset = score_offset_digit0 - score_uv;
		// flip
		d0_flip - 1;
		gt_points->digit_0.flip = 1;
	}
	else if (d0_flip == 1) { //middle of flipping
		gt_points->digit_0.flip -= 0.1;
		if (gt_points->digit_0.flip <= 0) {
			d0_shown_offset = gt_points->digit_0.new_offset;//show new_offset once flipped past halfway
		}
		if (gt_points->digit_0.flip <= -1) {
			gt_points->digit_0.flip = -1;
			d0_flip = 0; // indicate done flipping;
		}
	}
	else if (d0_flip == 0) { //done flipping, waiting for new flip
		gt_points->digit_0.old_offset = score_offset_digit0;
	}

	if (gt_points->digit_1.new_offset != score_offset_digit1) { // need to flip
																// prepare flip
		d1_shown_offset = gt_points->digit_1.old_offset;
		gt_points->digit_1.new_offset = score_offset_digit1;
		gt_points->digit_1.old_offset = score_offset_digit1 - score_uv;
		// flip
		d1_flip - 1;
		gt_points->digit_1.flip = 1;
	}
	else if (d1_flip == 1) { //middle of flipping
		gt_points->digit_1.flip -= 0.1;
		if (gt_points->digit_1.flip <= 0) {
			d1_shown_offset = gt_points->digit_1.new_offset;//show new_offset once flipped past halfway
		}
		if (gt_points->digit_1.flip <= -1) {
			gt_points->digit_1.flip = -1;
			d1_flip = 0; // indicate done flipping;
		}
	}
	else if (d1_flip == 0) { //done flipping, waiting for new flip
		gt_points->digit_1.old_offset = score_offset_digit1;
	}

	if (gt_points->digit_2.new_offset != score_offset_digit2) { // need to flip
																// prepare flip
		d2_shown_offset = gt_points->digit_2.old_offset;
		gt_points->digit_2.new_offset = score_offset_digit2;
		gt_points->digit_2.old_offset = score_offset_digit2 - score_uv;
		// flip
		d2_flip - 1;
		gt_points->digit_2.flip = 1;
	}
	else if (d2_flip == 1) { //middle of flipping
		gt_points->digit_2.flip -= 0.1;
		if (gt_points->digit_2.flip <= 0) {
			d2_shown_offset = gt_points->digit_2.new_offset;//show new_offset once flipped past halfway
		}
		if (gt_points->digit_2.flip <= -1) {
			gt_points->digit_2.flip = -1;
			d2_flip = 0; // indicate done flipping;
		}
	}
	else if (d2_flip == 0) { //done flipping, waiting for new flip
		gt_points->digit_2.old_offset = score_offset_digit2;
	}
}

void ScoreDisplay::draw(const mat3& projection) {
	transform_begin();
	transform_translate(m_position);
	transform_scale(m_scale);
	transform_end();

	// Setting shaders
	glUseProgram(effect.program);

	// Enabling alpha channel for textures
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);

	// Setting score_vertices and indices
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
	glBindTexture(GL_TEXTURE_2D, score_tex.id);

	// Getting uniform locations for glUniform* calls
	GLint transform_uloc = glGetUniformLocation(effect.program, "transform");
	GLint projection_uloc = glGetUniformLocation(effect.program, "projection");
	GLint digit_0_uloc = glGetUniformLocation(effect.program, "digit_0");
	GLint digit_1_uloc = glGetUniformLocation(effect.program, "digit_1");
	GLint digit_2_uloc = glGetUniformLocation(effect.program, "digit_2");

	GLint color_uloc = glGetUniformLocation(effect.program, "fcolor");

	// Setting uniform values to the currently bound program
	glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&transform);
	glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);
	glUniform4f(digit_0_uloc, gt_score.digit_0.old_offset, gt_score.digit_0.new_offset, d0_shown_offset, gt_score.digit_0.flip);
	glUniform4f(digit_1_uloc, gt_score.digit_1.old_offset, gt_score.digit_1.new_offset, d1_shown_offset, gt_score.digit_1.flip);
	glUniform4f(digit_2_uloc, gt_score.digit_2.old_offset, gt_score.digit_2.new_offset, d2_shown_offset, gt_score.digit_2.flip);

	float color[] = { 1.f, 1.f, 1.f };
	glUniform3fv(color_uloc, 1, color);


	// Drawing!
	glDrawElements(GL_TRIANGLES, 54, GL_UNSIGNED_SHORT, nullptr);
}

void ScoreDisplay::update_score(int score)
{
	if (score < 1000) {
		m_current_score = score;
	}
	else {
		m_current_score = 999;
	}

	SplitSetScoreDigits(m_current_score, &gt_score);
}

*/