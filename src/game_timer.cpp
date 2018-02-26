// Header
#include "game_timer.hpp"

Texture GameTimer::calendar_tex;
TexturedVertex2 vertices[48];

float uv;
float i_w;
float s_p;
gt_tracker gt_date;
gt_tracker gt_month;
gt_tracker gt_year;
int date_digit_0_flip = 0;
int date_digit_1_flip = 0;
int month_digit_0_flip = 0;
int month_digit_1_flip = 0;

float month_d0_shown_offset;

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
	uv = 100.f/calendar_tex.width; //texture uv
	i_w = calendar_tex.width / 10.f; //individual number width
	s_p = -400.f; //starting position
	float hr = calendar_tex.height * 0.5 ;
	
	//date digit 0 top -- new 
	vertices[0].position = { s_p + i_w	    , 0  , 0.f };
	vertices[1].position = { s_p + 2 * i_w  , 0  , 0.f };
	vertices[2].position = { s_p + 2 * i_w	, -hr, 0.f };
	vertices[3].position = { s_p + i_w		, -hr, 0.f };
	vertices[0].texcoord = { uv * 0.f, 0.5  , 0.1f };
	vertices[1].texcoord = { uv * 1.f, 0.5  , 0.1f };
	vertices[2].texcoord = { uv * 1.f, 0.0f , 0.1f };
	vertices[3].texcoord = { uv * 0.f, 0.0f , 0.1f };

	//date digit 0 bottom -- old
	vertices[4].position = {s_p + i_w		, +hr, 0.f };
	vertices[5].position = {s_p + 2 * i_w   , +hr, 0.f };
	vertices[6].position = {s_p + 2 * i_w	, 0, 0.f };
	vertices[7].position = {s_p + i_w		, 0, 0.f };
	vertices[4].texcoord = { uv * 0.f, 1.0f,  0.11f };
	vertices[5].texcoord = { uv * 1.f, 1.0f,  0.11f };
	vertices[6].texcoord = { uv * 1.f, 0.5f, 0.11f };
	vertices[7].texcoord = { uv * 0.f, 0.5f,  0.11f };

	//date display for date digit 0 -- rotating
	vertices[8 ].position = { s_p + i_w		 , +hr, 0.1f };
	vertices[9 ].position = { s_p + 2 * i_w  , +hr, 0.1f };
	vertices[10].position = { s_p + 2 * i_w	 , -hr, 0.1f };
	vertices[11].position = { s_p + i_w		 , -hr, 0.1f };
	vertices[8 ].texcoord = { uv * 0, 1.0f, 1.0f };
	vertices[9 ].texcoord = { uv * 1, 1.0f, 1.0f };
	vertices[10].texcoord = { uv * 1, 0.0f, 1.0f };
	vertices[11].texcoord = { uv * 0, 0.0f, 1.0f };


	//date digit 1 top -- new 
	vertices[12].position = { s_p      , 0  , 0.f };
	vertices[13].position = { s_p + i_w, 0  , 0.f };
	vertices[14].position = { s_p + i_w, -hr, 0.f };
	vertices[15].position = { s_p      , -hr, 0.f };
	vertices[12].texcoord = { uv  * 0.f, 0.5f, 0.2f };
	vertices[13].texcoord = { uv  * 1.f, 0.5f, 0.2f };
	vertices[14].texcoord = { uv  * 1.f, 0.0f, 0.2f };
	vertices[15].texcoord = { uv  * 0.f, 0.0f, 0.2f };

	//date digit 1 bottom -- old
	vertices[16].position = { s_p      , +hr , 0.f };
	vertices[17].position = { s_p + i_w, +hr , 0.f };
	vertices[18].position = { s_p + i_w, 0	 , 0.f };
	vertices[19].position = { s_p      , 0	 , 0.f };
	vertices[16].texcoord = { uv  * 0.f, 1.0f, 0.22f };
	vertices[17].texcoord = { uv  * 1.f, 1.0f, 0.22f };
	vertices[18].texcoord = { uv  * 1.f, 0.5f, 0.22f };
	vertices[19].texcoord = { uv  * 0.f, 0.5f, 0.22f };

	//date display for date digit 1 -- rotating
	vertices[20].position = {s_p 	, +hr, 0.2f };
	vertices[21].position = {s_p + i_w , +hr, 0.2f };
	vertices[22].position = {s_p + i_w	, -hr, 0.2f };
	vertices[23].position = {s_p	, -hr, 0.2f };
	vertices[20].texcoord = { uv * 0, 1.0f, 2.0f };
	vertices[21].texcoord = { uv * 1, 1.0f, 2.0f };
	vertices[22].texcoord = { uv * 1, 0.0f, 2.0f };
	vertices[23].texcoord = { uv * 0, 0.0f, 2.0f };

	///////////////////////////////////////////////////////////////////////////
	//month digit 0 top -- new 
	vertices[24].position = { s_p + 4 * i_w	, 0  , 0.f };
	vertices[25].position = { s_p + 5 * i_w , 0  , 0.f };
	vertices[26].position = { s_p + 5 * i_w	, -hr, 0.f };
	vertices[27].position = { s_p + 4 * i_w	, -hr, 0.f };
	vertices[24].texcoord = { uv * 0.f, 0.5  , 0.3f };
	vertices[25].texcoord = { uv * 1.f, 0.5  , 0.3f };
	vertices[26].texcoord = { uv * 1.f, 0.0f , 0.3f };
	vertices[27].texcoord = { uv * 0.f, 0.0f , 0.3f };

	//month digit 0 bottom -- old
	vertices[28].position = { s_p + 4 * i_w	, +hr, 0.f };
	vertices[29].position = { s_p + 5 * i_w , +hr, 0.f };
	vertices[30].position = { s_p + 5 * i_w	, 0, 0.f };
	vertices[31].position = { s_p + 4 * i_w	, 0, 0.f };
	vertices[28].texcoord = { uv * 0.f, 1.0f,  0.33f };
	vertices[29].texcoord = { uv * 1.f, 1.0f,  0.33f };
	vertices[30].texcoord = { uv * 1.f, 0.5f,  0.33f };
	vertices[31].texcoord = { uv * 0.f, 0.5f,  0.33f };

	//month display for date digit 0 -- rotating
	vertices[32].position = { s_p + 5 * i_w	, +hr, 0.3f };
	vertices[33].position = { s_p + 6 * i_w , +hr, 0.3f };
	vertices[34].position = { s_p + 6 * i_w	, -hr, 0.3f };
	vertices[35].position = { s_p + 5 * i_w	, -hr, 0.3f };
	vertices[32].texcoord = { uv * 0, 1.0f, 3.0f };
	vertices[33].texcoord = { uv * 1, 1.0f, 3.0f };
	vertices[34].texcoord = { uv * 1, 0.0f, 3.0f };
	vertices[35].texcoord = { uv * 0, 0.0f, 3.0f };
	///////////////////////////////////////////////////////////////////////////

	//month digit 1 top -- new 
	vertices[36].position = { s_p + 3 * i_w, 0  , 0.f };
	vertices[37].position = { s_p + 4 * i_w, 0  , 0.f };
	vertices[38].position = { s_p + 4 * i_w, -hr, 0.f };
	vertices[39].position = { s_p + 3 * i_w, -hr, 0.f };
	vertices[36].texcoord = { uv  * 0.f, 0.5f, 0.4f };
	vertices[37].texcoord = { uv  * 1.f, 0.5f, 0.4f };
	vertices[38].texcoord = { uv  * 1.f, 0.0f, 0.4f };
	vertices[39].texcoord = { uv  * 0.f, 0.0f, 0.4f };

	//month digit 1 bottom -- old
	vertices[40].position = { s_p + 3 * i_w, +hr , 0.f };
	vertices[41].position = { s_p + 4 * i_w, +hr , 0.f };
	vertices[42].position = { s_p + 4 * i_w, 0	 , 0.f };
	vertices[43].position = { s_p + 3 * i_w, 0	 , 0.f };
	vertices[40].texcoord = { uv  * 0.f, 1.0f, 0.44f };
	vertices[41].texcoord = { uv  * 1.f, 1.0f, 0.44f };
	vertices[42].texcoord = { uv  * 1.f, 0.5f, 0.44f };
	vertices[43].texcoord = { uv  * 0.f, 0.5f, 0.44f };

	//month display for date digit 1 -- rotating
	vertices[44].position = { s_p + 3 * i_w, +hr, 0.4f };
	vertices[45].position = { s_p + 4 * i_w, +hr, 0.4f };
	vertices[46].position = { s_p + 4 * i_w, -hr, 0.4f };
	vertices[47].position = { s_p + 3 * i_w, -hr, 0.4f };
	vertices[44].texcoord = { uv * 0, 1.0f, 4.0f };
	vertices[45].texcoord = { uv * 1, 1.0f, 4.0f };
	vertices[46].texcoord = { uv * 1, 0.0f, 4.0f };
	vertices[47].texcoord = { uv * 0, 0.0f, 4.0f };

	uint16_t indices[] = { 0,3,1,1,3,2,4,7,5,5,7,6,8,11,9,9,11,10,
						   12,15,13,13,15,14,16,19,17,17,19,18,20,23,21,21,23,22,
						   24,27,25,25,27,26,28,31,29,29,31,30,32,35,33,33,35,34,
						   36,39,37,37,39,38,40,43,41,41,43,42,44,47,45,45,47,46};

	// Clearing errors
	gl_flush_errors();

	// Vertex Buffer creation
	glGenBuffers(1, &mesh.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(TexturedVertex2) * 48, vertices, GL_STATIC_DRAW);

	// Index Buffer creation
	glGenBuffers(1, &mesh.ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * 72, indices, GL_STATIC_DRAW);

	// Vertex Array (Container for Vertex + Index buffer)
	glGenVertexArrays(1, &mesh.vao);

	// Loading shaders
	effect.load_from_file(shader_path("gameTimer.vs.glsl"), shader_path("gameTimer.fs.glsl"));

	// Setting initial values, scale is negative to make it face the opposite way
	m_scale.x = 0.5;
	m_scale.y = 0.5;
	m_position = { 500.f, 500.f };

	gt_date.digit_0.old_offset = uv * 1;
	gt_date.digit_0.new_offset = uv * 1;
	gt_date.digit_0.flip = -1;

	gt_date.digit_1.old_offset = 0.0f;
	gt_date.digit_1.new_offset = 0.0f;
	gt_date.digit_1.flip = -1; 

	gt_month.digit_0.old_offset = 0.0f;
	gt_month.digit_0.new_offset = uv * 1;
	gt_month.digit_0.flip = -1;
	/////////////////////////////////////////
	month_d0_shown_offset = uv * 1;
	/////////////////////////////////////////
	
	gt_month.digit_1.old_offset = 0.0f;
	gt_month.digit_1.new_offset = 0.0f;
	gt_month.digit_1.flip = -1;
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

void GameTimer::SplitSetDateDigits(int day, gt_tracker* gt_day, int mon, gt_tracker* gt_mon) {
	float date_offset_digit0 = std::fmodf(day, 10) * uv;
	float date_offset_digit1 = std::fmodf(day / 10, 10) * uv;
	float month_offset_digit0 = std::fmodf(mon, 10) * uv;
	float month_offset_digit1 = std::fmodf(mon / 10, 10) * uv;

	if (gt_day->digit_0.new_offset != date_offset_digit0) {
		gt_day->digit_0.old_offset = date_offset_digit0-uv;
		gt_day->digit_0.new_offset = date_offset_digit0;
		gt_day->digit_0.flip = 1;
		date_digit_0_flip = 1; 
	}
	else if (date_digit_0_flip == 1){
		gt_day->digit_0.flip -= 0.05;
		if (gt_day->digit_0.flip <= -1) {
			gt_day->digit_0.flip = -1;
			date_digit_0_flip = 0;
		}
	}

	if (gt_day->digit_1.new_offset != date_offset_digit1) {
		gt_day->digit_1.old_offset = date_offset_digit1-uv;
		gt_day->digit_1.new_offset = date_offset_digit1;
		gt_day->digit_1.flip = 1;
		date_digit_1_flip = 1;
	}
	else  if (date_digit_1_flip == 1) {
		gt_day->digit_1.flip -= 0.05;
		if (gt_day->digit_1.flip <= -1) {
			gt_day->digit_1.flip = -1;
			date_digit_1_flip = 0;
		}
	}
	///////////////////////////////////////////////////////////////////////////
	if (gt_mon->digit_0.new_offset != month_offset_digit0) { //need to flip
		//prepare flip
		month_d0_shown_offset = gt_mon->digit_0.old_offset;
		gt_mon->digit_0.new_offset = month_offset_digit0;
		gt_mon->digit_0.old_offset = month_offset_digit0 - uv;
		//flip
		month_digit_0_flip = 1;
		gt_mon->digit_0.flip = 1;

	}else if (month_digit_0_flip == 1) {//middle of flipping
		gt_mon->digit_0.flip -= 0.1;
		if (gt_mon->digit_0.flip <= 0) {
			month_d0_shown_offset = gt_mon->digit_0.new_offset; //show new_offset once flipped past halfway
		}
		if (gt_mon->digit_0.flip <= -1) {
			gt_mon->digit_0.flip = -1;
			month_digit_0_flip = 0; //indicate done flipping;
		}
		
	}else if (month_digit_0_flip == 0) {//done flipping, waiting for new flip
		gt_mon->digit_0.old_offset = month_offset_digit0;
	}
	///////////////////////////////////////////////////////////////////////////

	if (gt_mon->digit_1.new_offset != month_offset_digit1) {
		gt_mon->digit_1.old_offset = month_offset_digit1 - uv;
		gt_mon->digit_1.new_offset = month_offset_digit1;
		gt_mon->digit_1.flip = 1;
		month_digit_0_flip = 1;
	}
	else if (month_digit_0_flip == 1) {
		gt_mon->digit_1.flip -= 0.05;
		if (gt_mon->digit_1.flip <= -1) {
			gt_mon->digit_1.flip = -1;
			month_digit_1_flip = 0;
		}
	}
	
}

void GameTimer::advance_time(float real_time_seconds_elapsed)
{	
	const int One_Day_Sec = 86400;
	struct tm * adv_time = localtime(&m_current_time);
	adv_time->tm_sec += One_Day_Sec/10;
	m_current_time = mktime(adv_time);

	SplitSetDateDigits(gmtime(&m_current_time)->tm_mday, &gt_date, gmtime(&m_current_time)->tm_mon + 1, &gt_month);
}

void GameTimer::draw(const mat3& projection) {
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
	GLint date_digit_0_uloc = glGetUniformLocation(effect.program, "date_digit_0");
	GLint date_digit_1_uloc = glGetUniformLocation(effect.program, "date_digit_1");
	GLint month_digit_0_uloc = glGetUniformLocation(effect.program, "month_digit_0");
	GLint month_digit_1_uloc = glGetUniformLocation(effect.program, "month_digit_1");

	GLint color_uloc = glGetUniformLocation(effect.program, "fcolor");
	
	// Setting uniform values to the currently bound program
	glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&transform);
	glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);
	glUniform3f(date_digit_0_uloc, gt_date.digit_0.old_offset, gt_date.digit_0.new_offset, gt_date.digit_0.flip);
	glUniform3f(date_digit_1_uloc, gt_date.digit_1.old_offset, gt_date.digit_1.new_offset, gt_date.digit_1.flip);

	glUniform4f(month_digit_0_uloc, gt_month.digit_0.old_offset, gt_month.digit_0.new_offset, month_d0_shown_offset, gt_month.digit_0.flip);

	glUniform3f(month_digit_1_uloc, gt_month.digit_1.old_offset, gt_month.digit_1.new_offset, gt_month.digit_1.flip);;

	float color[] = { 1.f, 1.f, 1.f };
	glUniform3fv(color_uloc, 1, color);



	// Drawing!
	glDrawElements(GL_TRIANGLES, 72, GL_UNSIGNED_SHORT, nullptr);
}
