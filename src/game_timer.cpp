// Header
#include "game_timer.hpp"

Texture GameTimer::calendar_tex;
TexturedVertex2 timer_vertices[96];

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
int year_d0_flip = 0;
int year_d1_flip = 0;
int year_d2_flip = 0;
int year_d3_flip = 0;

float month_d0_shown_offset;
float month_d1_shown_offset;
float date_d0_shown_offset;
float date_d1_shown_offset;
float year_d0_shown_offset;
float year_d1_shown_offset;
float year_d2_shown_offset;
float year_d3_shown_offset;

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
			fprintf(stderr, "Failed to load game timer texture!");
			return false;
		}
	}

	//centre of the texture
	uv = 100.f/calendar_tex.width; //texture uv
	i_w = calendar_tex.width / 10.f; //individual number width
	s_p = -600.f; //starting position
	float hr = calendar_tex.height * 0.5 ;

	//date digit 0 top -- new
	timer_vertices[0].position = { s_p + i_w	    , 0  , 0.f };
	timer_vertices[1].position = { s_p + 2 * i_w  , 0  , 0.f };
	timer_vertices[2].position = { s_p + 2 * i_w	, -hr, 0.f };
	timer_vertices[3].position = { s_p + i_w		, -hr, 0.f };
	timer_vertices[0].texcoord = { uv * 0.f, 0.5  , 0.1f };
	timer_vertices[1].texcoord = { uv * 1.f, 0.5  , 0.1f };
	timer_vertices[2].texcoord = { uv * 1.f, 0.0f , 0.1f };
	timer_vertices[3].texcoord = { uv * 0.f, 0.0f , 0.1f };

	//date digit 0 bottom -- old
	timer_vertices[4].position = {s_p + i_w		, +hr, 0.f };
	timer_vertices[5].position = {s_p + 2 * i_w   , +hr, 0.f };
	timer_vertices[6].position = {s_p + 2 * i_w	, 0, 0.f };
	timer_vertices[7].position = {s_p + i_w		, 0, 0.f };
	timer_vertices[4].texcoord = { uv * 0.f, 1.0f,  0.11f };
	timer_vertices[5].texcoord = { uv * 1.f, 1.0f,  0.11f };
	timer_vertices[6].texcoord = { uv * 1.f, 0.5f, 0.11f };
	timer_vertices[7].texcoord = { uv * 0.f, 0.5f,  0.11f };

	//date display for date digit 0 -- rotating
	timer_vertices[8 ].position = { s_p + i_w		 , +hr, 0.1f };
	timer_vertices[9 ].position = { s_p + 2 * i_w  , +hr, 0.1f };
	timer_vertices[10].position = { s_p + 2 * i_w	 , -hr, 0.1f };
	timer_vertices[11].position = { s_p + i_w		 , -hr, 0.1f };
	timer_vertices[8 ].texcoord = { uv * 0, 1.0f, 1.0f };
	timer_vertices[9 ].texcoord = { uv * 1, 1.0f, 1.0f };
	timer_vertices[10].texcoord = { uv * 1, 0.0f, 1.0f };
	timer_vertices[11].texcoord = { uv * 0, 0.0f, 1.0f };


	//date digit 1 top -- new
	timer_vertices[12].position = { s_p      , 0  , 0.f };
	timer_vertices[13].position = { s_p + i_w, 0  , 0.f };
	timer_vertices[14].position = { s_p + i_w, -hr, 0.f };
	timer_vertices[15].position = { s_p      , -hr, 0.f };
	timer_vertices[12].texcoord = { uv  * 0.f, 0.5f, 0.2f };
	timer_vertices[13].texcoord = { uv  * 1.f, 0.5f, 0.2f };
	timer_vertices[14].texcoord = { uv  * 1.f, 0.0f, 0.2f };
	timer_vertices[15].texcoord = { uv  * 0.f, 0.0f, 0.2f };

	//date digit 1 bottom -- old
	timer_vertices[16].position = { s_p      , +hr , 0.f };
	timer_vertices[17].position = { s_p + i_w, +hr , 0.f };
	timer_vertices[18].position = { s_p + i_w, 0	 , 0.f };
	timer_vertices[19].position = { s_p      , 0	 , 0.f };
	timer_vertices[16].texcoord = { uv  * 0.f, 1.0f, 0.22f };
	timer_vertices[17].texcoord = { uv  * 1.f, 1.0f, 0.22f };
	timer_vertices[18].texcoord = { uv  * 1.f, 0.5f, 0.22f };
	timer_vertices[19].texcoord = { uv  * 0.f, 0.5f, 0.22f };

	//date display for date digit 1 -- rotating
	timer_vertices[20].position = {s_p 	, +hr, 0.2f };
	timer_vertices[21].position = {s_p + i_w , +hr, 0.2f };
	timer_vertices[22].position = {s_p + i_w	, -hr, 0.2f };
	timer_vertices[23].position = {s_p	, -hr, 0.2f };
	timer_vertices[20].texcoord = { uv * 0, 1.0f, 2.0f };
	timer_vertices[21].texcoord = { uv * 1, 1.0f, 2.0f };
	timer_vertices[22].texcoord = { uv * 1, 0.0f, 2.0f };
	timer_vertices[23].texcoord = { uv * 0, 0.0f, 2.0f };

	//month digit 0 top -- new
	timer_vertices[24].position = { s_p + 3.5f * i_w	, 0  , 0.f };
	timer_vertices[25].position = { s_p + 4.5f * i_w , 0  , 0.f };
	timer_vertices[26].position = { s_p + 4.5f * i_w	, -hr, 0.f };
	timer_vertices[27].position = { s_p + 3.5f * i_w	, -hr, 0.f };
	timer_vertices[24].texcoord = { uv * 0.f, 0.5  , 0.3f };
	timer_vertices[25].texcoord = { uv * 1.f, 0.5  , 0.3f };
	timer_vertices[26].texcoord = { uv * 1.f, 0.0f , 0.3f };
	timer_vertices[27].texcoord = { uv * 0.f, 0.0f , 0.3f };

	//month digit 0 bottom -- old
	timer_vertices[28].position = { s_p + 3.5f * i_w	, +hr, 0.f };
	timer_vertices[29].position = { s_p + 4.5f * i_w , +hr, 0.f };
	timer_vertices[30].position = { s_p + 4.5f * i_w	, 0, 0.f };
	timer_vertices[31].position = { s_p + 3.5f * i_w	, 0, 0.f };
	timer_vertices[28].texcoord = { uv * 0.f, 1.0f,  0.33f };
	timer_vertices[29].texcoord = { uv * 1.f, 1.0f,  0.33f };
	timer_vertices[30].texcoord = { uv * 1.f, 0.5f,  0.33f };
	timer_vertices[31].texcoord = { uv * 0.f, 0.5f,  0.33f };

	//month display for date digit 0 -- rotating
	timer_vertices[32].position = { s_p + 3.5f * i_w	, +hr, 0.3f };
	timer_vertices[33].position = { s_p + 4.5f * i_w , +hr, 0.3f };
	timer_vertices[34].position = { s_p + 4.5f * i_w	, -hr, 0.3f };
	timer_vertices[35].position = { s_p + 3.5f * i_w	, -hr, 0.3f };
	timer_vertices[32].texcoord = { uv * 0, 1.0f, 3.0f };
	timer_vertices[33].texcoord = { uv * 1, 1.0f, 3.0f };
	timer_vertices[34].texcoord = { uv * 1, 0.0f, 3.0f };
	timer_vertices[35].texcoord = { uv * 0, 0.0f, 3.0f };

	//month digit 1 top -- new
	timer_vertices[36].position = { s_p + 2.5f * i_w, 0  , 0.f };
	timer_vertices[37].position = { s_p + 3.5f * i_w, 0  , 0.f };
	timer_vertices[38].position = { s_p + 3.5f * i_w, -hr, 0.f };
	timer_vertices[39].position = { s_p + 2.5f * i_w, -hr, 0.f };
	timer_vertices[36].texcoord = { uv  * 0.f, 0.5f, 0.4f };
	timer_vertices[37].texcoord = { uv  * 1.f, 0.5f, 0.4f };
	timer_vertices[38].texcoord = { uv  * 1.f, 0.0f, 0.4f };
	timer_vertices[39].texcoord = { uv  * 0.f, 0.0f, 0.4f };

	//month digit 1 bottom -- old
	timer_vertices[40].position = { s_p + 2.5f * i_w, +hr , 0.f };
	timer_vertices[41].position = { s_p + 3.5f * i_w, +hr , 0.f };
	timer_vertices[42].position = { s_p + 3.5f * i_w, 0	 , 0.f };
	timer_vertices[43].position = { s_p + 2.5f * i_w, 0	 , 0.f };
	timer_vertices[40].texcoord = { uv  * 0.f, 1.0f, 0.44f };
	timer_vertices[41].texcoord = { uv  * 1.f, 1.0f, 0.44f };
	timer_vertices[42].texcoord = { uv  * 1.f, 0.5f, 0.44f };
	timer_vertices[43].texcoord = { uv  * 0.f, 0.5f, 0.44f };

	//month display for date digit 1 -- rotating
	timer_vertices[44].position = { s_p + 2.5f * i_w, +hr, 0.4f };
	timer_vertices[45].position = { s_p + 3.5f * i_w, +hr, 0.4f };
	timer_vertices[46].position = { s_p + 3.5f * i_w, -hr, 0.4f };
	timer_vertices[47].position = { s_p + 2.5f * i_w, -hr, 0.4f };
	timer_vertices[44].texcoord = { uv * 0, 1.0f, 4.0f };
	timer_vertices[45].texcoord = { uv * 1, 1.0f, 4.0f };
	timer_vertices[46].texcoord = { uv * 1, 0.0f, 4.0f };
	timer_vertices[47].texcoord = { uv * 0, 0.0f, 4.0f };

	//year d0 top -- new
	timer_vertices[48].position = { s_p + 8.f * i_w, 0  , 0.f };
	timer_vertices[49].position = { s_p + 9.f * i_w, 0  , 0.f };
	timer_vertices[50].position = { s_p + 9.f * i_w, -hr, 0.f };
	timer_vertices[51].position = { s_p + 8.f * i_w, -hr, 0.f };
	timer_vertices[48].texcoord = { uv  * 0.f, 0.5f, 0.5f };
	timer_vertices[49].texcoord = { uv  * 1.f, 0.5f, 0.5f };
	timer_vertices[50].texcoord = { uv  * 1.f, 0.0f, 0.5f };
	timer_vertices[51].texcoord = { uv  * 0.f, 0.0f, 0.5f };

	//year d0 bottom -- old
	timer_vertices[52].position = { s_p + 8.f * i_w, +hr , 0.f };
	timer_vertices[53].position = { s_p + 9.f * i_w, +hr , 0.f };
	timer_vertices[54].position = { s_p + 9.f * i_w, 0	 , 0.f };
	timer_vertices[55].position = { s_p + 8.f * i_w, 0	 , 0.f };
	timer_vertices[52].texcoord = { uv  * 0.f, 1.0f, 0.55f };
	timer_vertices[53].texcoord = { uv  * 1.f, 1.0f, 0.55f };
	timer_vertices[54].texcoord = { uv  * 1.f, 0.5f, 0.55f };
	timer_vertices[55].texcoord = { uv  * 0.f, 0.5f, 0.55f };

	//year d0 -- rotating
	timer_vertices[56].position = { s_p + 8.f * i_w, +hr, 0.5f };
	timer_vertices[57].position = { s_p + 9.f * i_w, +hr, 0.5f };
	timer_vertices[58].position = { s_p + 9.f * i_w, -hr, 0.5f };
	timer_vertices[59].position = { s_p + 8.f * i_w, -hr, 0.5f };
	timer_vertices[56].texcoord = { uv * 0, 1.0f, 5.0f };
	timer_vertices[57].texcoord = { uv * 1, 1.0f, 5.0f };
	timer_vertices[58].texcoord = { uv * 1, 0.0f, 5.0f };
	timer_vertices[59].texcoord = { uv * 0, 0.0f, 5.0f };

	//year d1 top -- new
	timer_vertices[60].position = { s_p + 7.f * i_w, 0  , 0.f };
	timer_vertices[61].position = { s_p + 8.f * i_w, 0  , 0.f };
	timer_vertices[62].position = { s_p + 8.f * i_w, -hr, 0.f };
	timer_vertices[63].position = { s_p + 7.f * i_w, -hr, 0.f };
	timer_vertices[60].texcoord = { uv  * 0.f, 0.5f, 0.6f };
	timer_vertices[61].texcoord = { uv  * 1.f, 0.5f, 0.6f };
	timer_vertices[62].texcoord = { uv  * 1.f, 0.0f, 0.6f };
	timer_vertices[63].texcoord = { uv  * 0.f, 0.0f, 0.6f };

	//year d1 bottom -- old
	timer_vertices[64].position = { s_p + 7.f * i_w, +hr , 0.f };
	timer_vertices[65].position = { s_p + 8.f * i_w, +hr , 0.f };
	timer_vertices[66].position = { s_p + 8.f * i_w, 0	 , 0.f };
	timer_vertices[67].position = { s_p + 7.f * i_w, 0	 , 0.f };
	timer_vertices[64].texcoord = { uv  * 0.f, 1.0f, 0.66f };
	timer_vertices[65].texcoord = { uv  * 1.f, 1.0f, 0.66f };
	timer_vertices[66].texcoord = { uv  * 1.f, 0.5f, 0.66f };
	timer_vertices[67].texcoord = { uv  * 0.f, 0.5f, 0.66f };

	//year d1 -- rotating
	timer_vertices[68].position = { s_p + 7.f * i_w, +hr, 0.6f };
	timer_vertices[69].position = { s_p + 8.f * i_w, +hr, 0.6f };
	timer_vertices[70].position = { s_p + 8.f * i_w, -hr, 0.6f };
	timer_vertices[71].position = { s_p + 7.f * i_w, -hr, 0.6f };
	timer_vertices[68].texcoord = { uv * 0, 1.0f, 6.0f };
	timer_vertices[69].texcoord = { uv * 1, 1.0f, 6.0f };
	timer_vertices[70].texcoord = { uv * 1, 0.0f, 6.0f };
	timer_vertices[71].texcoord = { uv * 0, 0.0f, 6.0f };

	//year d2 top -- new
	timer_vertices[72].position = { s_p + 6.f * i_w, 0  , 0.f };
	timer_vertices[73].position = { s_p + 7.f * i_w, 0  , 0.f };
	timer_vertices[74].position = { s_p + 7.f * i_w, -hr, 0.f };
	timer_vertices[75].position = { s_p + 6.f * i_w, -hr, 0.f };
	timer_vertices[72].texcoord = { uv  * 0.f, 0.5f, 0.7f };
	timer_vertices[73].texcoord = { uv  * 1.f, 0.5f, 0.7f };
	timer_vertices[74].texcoord = { uv  * 1.f, 0.0f, 0.7f };
	timer_vertices[75].texcoord = { uv  * 0.f, 0.0f, 0.7f };

	//year d2 bottom -- old
	timer_vertices[76].position = { s_p + 6.f * i_w, +hr , 0.f };
	timer_vertices[77].position = { s_p + 7.f * i_w, +hr , 0.f };
	timer_vertices[78].position = { s_p + 7.f * i_w, 0	 , 0.f };
	timer_vertices[79].position = { s_p + 6.f * i_w, 0	 , 0.f };
	timer_vertices[76].texcoord = { uv  * 0.f, 1.0f, 0.77f };
	timer_vertices[77].texcoord = { uv  * 1.f, 1.0f, 0.77f };
	timer_vertices[78].texcoord = { uv  * 1.f, 0.5f, 0.77f };
	timer_vertices[79].texcoord = { uv  * 0.f, 0.5f, 0.77f };

	//year d2 -- rotating
	timer_vertices[80].position = { s_p + 6.f * i_w, +hr, 0.7f };
	timer_vertices[81].position = { s_p + 7.f * i_w, +hr, 0.7f };
	timer_vertices[82].position = { s_p + 7.f * i_w, -hr, 0.7f };
	timer_vertices[83].position = { s_p + 6.f * i_w, -hr, 0.7f };
	timer_vertices[80].texcoord = { uv * 0, 1.0f, 7.0f };
	timer_vertices[81].texcoord = { uv * 1, 1.0f, 7.0f };
	timer_vertices[82].texcoord = { uv * 1, 0.0f, 7.0f };
	timer_vertices[83].texcoord = { uv * 0, 0.0f, 7.0f };

	//year d3 top -- new
	timer_vertices[84].position = { s_p + 5.f * i_w, 0  , 0.f };
	timer_vertices[85].position = { s_p + 6.f * i_w, 0  , 0.f };
	timer_vertices[86].position = { s_p + 6.f * i_w, -hr, 0.f };
	timer_vertices[87].position = { s_p + 5.f * i_w, -hr, 0.f };
	timer_vertices[84].texcoord = { uv  * 0.f, 0.5f, 0.8f };
	timer_vertices[85].texcoord = { uv  * 1.f, 0.5f, 0.8f };
	timer_vertices[86].texcoord = { uv  * 1.f, 0.0f, 0.8f };
	timer_vertices[87].texcoord = { uv  * 0.f, 0.0f, 0.8f };

	//year d3 bottom -- old
	timer_vertices[88].position = { s_p + 5.f * i_w, +hr , 0.f };
	timer_vertices[89].position = { s_p + 6.f * i_w, +hr , 0.f };
	timer_vertices[90].position = { s_p + 6.f * i_w, 0	 , 0.f };
	timer_vertices[91].position = { s_p + 5.f * i_w, 0	 , 0.f };
	timer_vertices[88].texcoord = { uv  * 0.f, 1.0f, 0.88f };
	timer_vertices[89].texcoord = { uv  * 1.f, 1.0f, 0.88f };
	timer_vertices[90].texcoord = { uv  * 1.f, 0.5f, 0.88f };
	timer_vertices[91].texcoord = { uv  * 0.f, 0.5f, 0.88f };

	//year d3 -- rotating
	timer_vertices[92].position = { s_p + 5.f * i_w, +hr, 0.8f };
	timer_vertices[93].position = { s_p + 6.f * i_w, +hr, 0.8f };
	timer_vertices[94].position = { s_p + 6.f * i_w, -hr, 0.8f };
	timer_vertices[95].position = { s_p + 5.f * i_w, -hr, 0.8f };
	timer_vertices[92].texcoord = { uv * 0, 1.0f, 8.0f };
	timer_vertices[93].texcoord = { uv * 1, 1.0f, 8.0f };
	timer_vertices[94].texcoord = { uv * 1, 0.0f, 8.0f };
	timer_vertices[95].texcoord = { uv * 0, 0.0f, 8.0f };

	uint16_t indices[] = { 0,3,1,1,3,2,4,7,5,5,7,6,8,11,9,9,11,10,
						   12,15,13,13,15,14,16,19,17,17,19,18,20,23,21,21,23,22,
						   24,27,25,25,27,26,28,31,29,29,31,30,32,35,33,33,35,34,
						   36,39,37,37,39,38,40,43,41,41,43,42,44,47,45,45,47,46,
						   48,51,49,49,51,50,52,55,53,53,55,54,56,59,57,57,59,58,
							60,63,61,61,63,62,64,67,65,65,67,66,68,71,69,69,71,70,
							72,75,73,73,75,74,76,79,77,77,79,78,80,83,81,81,83,82,
							84,87,85,85,87,86,88,91,89,89,91,90,92,95,93,93,95,94};

	// Clearing errors
	gl_flush_errors();

	// Vertex Buffer creation
	glGenBuffers(1, &mesh.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(TexturedVertex2) * 96, timer_vertices, GL_STATIC_DRAW);

	// Index Buffer creation
	glGenBuffers(1, &mesh.ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * 144, indices, GL_STATIC_DRAW);

	// Vertex Array (Container for Vertex + Index buffer)
	glGenVertexArrays(1, &mesh.vao);

	// Loading shaders
	effect.load_from_file(shader_path("gameTimer.vs.glsl"), shader_path("gameTimer.fs.glsl"));

	// Setting initial values, scale is negative to make it face the opposite way
	m_scale.x = 0.4;
	m_scale.y = 0.4;
	m_position = { 860.f, 80.f };

	gt_date.digit_0.old_offset = 0.0f;
	gt_date.digit_0.new_offset = uv * 1;
	gt_date.digit_0.flip = -1;
	date_d0_shown_offset = uv * 1;

	gt_date.digit_1.old_offset = 0.0f;
	gt_date.digit_1.new_offset = 0.0f;
	gt_date.digit_1.flip = -1;
	date_d1_shown_offset = 0.f;

	gt_month.digit_0.old_offset = 0.0f;
	gt_month.digit_0.new_offset = uv * 1;
	gt_month.digit_0.flip = -1;
	month_d0_shown_offset = uv * 1;

	gt_month.digit_1.old_offset = 0.0f;
	gt_month.digit_1.new_offset = 0.0f;
	gt_month.digit_1.flip = -1;
	month_d1_shown_offset = 0.f;

	gt_year.digit_0.old_offset = 0.0f;
	gt_year.digit_0.new_offset = uv * 8;
	gt_year.digit_0.flip = -1;
	year_d0_shown_offset = uv * 8;

	gt_year.digit_1.old_offset = 0.0f;
	gt_year.digit_1.new_offset = uv * 1;
	gt_year.digit_1.flip = -1;
	year_d1_shown_offset = uv * 1;

	gt_year.digit_2.old_offset = 0.0f;
	gt_year.digit_2.new_offset = uv * 0;
	gt_year.digit_2.flip = -1;
	year_d2_shown_offset = uv * 0;

	gt_year.digit_3.old_offset = 0.0f;
	gt_year.digit_3.new_offset = uv * 2;
	gt_year.digit_3.flip = -1;
	year_d3_shown_offset = uv * 2;


	return true;
}

void GameTimer::reset() {
	struct tm init_time = { 0 };
	init_time.tm_mday = DaysAfterUnixDate + 1; //mktime uses 1 based indexing for days
	init_time.tm_year = 70; // mktime starts from 1900 for some reason
	m_current_time = mktime(&init_time);
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
	return return_time;
}

void GameTimer::SplitSetDateDigits(int day, gt_tracker* gt_day, int mon, gt_tracker* gt_mon, int year, gt_tracker* gt_year) {
	float date_offset_digit0 = std::fmodf(day, 10) * uv;
	float date_offset_digit1 = std::fmodf(day / 10, 10) * uv;
	float month_offset_digit0 = std::fmodf(mon, 10) * uv;
	float month_offset_digit1 = std::fmodf(mon / 10, 10) * uv;
	float year_offset_d0 = std::fmodf(year, 10) * uv;;
	float year_offset_d1 = std::fmodf(year /10, 10) * uv;;
	float year_offset_d2 = std::fmodf(year /100, 10) * uv;;
	float year_offset_d3 = std::fmodf(year /1000, 10) * uv;;

	if (gt_day->digit_0.new_offset != date_offset_digit0) { //need to flip
		//prepare flip
		date_d0_shown_offset = gt_day->digit_0.old_offset;
		gt_day->digit_0.new_offset = date_offset_digit0;
		gt_day->digit_0.old_offset = date_offset_digit0 - uv;
		//flip
		date_digit_0_flip = 1;
		gt_day->digit_0.flip = 1;

	}
	else if (date_digit_0_flip == 1) {//middle of flipping
		gt_day->digit_0.flip -= 0.1;
		if (gt_day->digit_0.flip <= 0) {
			date_d0_shown_offset = gt_day->digit_0.new_offset; //show new_offset once flipped past halfway
		}
		if (gt_day->digit_0.flip <= -1) {
			gt_day->digit_0.flip = -1;
			date_digit_0_flip = 0; //indicate done flipping;
		}

	}
	else if (date_digit_0_flip == 0) {//done flipping, waiting for new flip
		gt_day->digit_0.old_offset = date_offset_digit0;
	}

	if (gt_day->digit_1.new_offset != date_offset_digit1) { //need to flip
		date_d1_shown_offset = gt_day->digit_1.old_offset;
		gt_day->digit_1.new_offset = date_offset_digit1;
		gt_day->digit_1.old_offset = date_offset_digit1 - uv;
		date_digit_1_flip = 1;
		gt_day->digit_1.flip = 1;

	}
	else if (date_digit_1_flip == 1) {//middle of flipping
		gt_day->digit_1.flip -= 0.1;
		if (gt_day->digit_1.flip <= 0) {
			date_d1_shown_offset = gt_day->digit_1.new_offset; //show new_offset once flipped past halfway
		}
		if (gt_day->digit_1.flip <= -1) {
			gt_day->digit_1.flip = -1;
			date_digit_1_flip = 0; //indicate done flipping;
		}

	}
	else if (date_digit_1_flip == 0) {//done flipping, waiting for new flip
		gt_day->digit_1.old_offset = date_offset_digit1;
	}

	if (gt_mon->digit_0.new_offset != month_offset_digit0) { //need to flip
		month_d0_shown_offset = gt_mon->digit_0.old_offset;
		gt_mon->digit_0.new_offset = month_offset_digit0;
		gt_mon->digit_0.old_offset = month_offset_digit0 - uv;
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

	if (gt_mon->digit_1.new_offset != month_offset_digit1) { //need to flip
		month_d1_shown_offset = gt_mon->digit_1.old_offset;
		gt_mon->digit_1.new_offset = month_offset_digit1;
		gt_mon->digit_1.old_offset = month_offset_digit1 - uv;
		month_digit_1_flip = 1;
		gt_mon->digit_1.flip = 1;

	}
	else if (month_digit_1_flip == 1) {//middle of flipping
		gt_mon->digit_1.flip -= 0.1;
		if (gt_mon->digit_1.flip <= 0) {
			month_d1_shown_offset = gt_mon->digit_1.new_offset; //show new_offset once flipped past halfway
		}
		if (gt_mon->digit_1.flip <= -1) {
			gt_mon->digit_1.flip = -1;
			month_digit_1_flip = 0; //indicate done flipping;
		}

	}
	else if (month_digit_1_flip == 0) {//done flipping, waiting for new flip
		gt_mon->digit_1.old_offset = month_offset_digit1;
	}

	if (gt_year->digit_0.new_offset != year_offset_d0) { //need to flip
		year_d0_shown_offset = gt_year->digit_0.old_offset;
		gt_year->digit_0.new_offset = year_offset_d0;
		gt_year->digit_0.old_offset = year_offset_d0 - uv;
		year_d0_flip = 1;
		gt_year->digit_0.flip = 1;

	}
	else if (year_d0_flip == 1) {//middle of flipping
		gt_year->digit_0.flip -= 0.1;
		if (gt_year->digit_0.flip <= 0) {
			year_d0_shown_offset = gt_year->digit_0.new_offset; //show new_offset once flipped past halfway
		}
		if (gt_year->digit_0.flip <= -1) {
			gt_year->digit_0.flip = -1;
			year_d0_flip = 0; //indicate done flipping;
		}

	}
	else if (year_d0_flip == 0) {//done flipping, waiting for new flip
		gt_year->digit_0.old_offset = year_offset_d0;
	}

	if (gt_year->digit_1.new_offset != year_offset_d1) { //need to flip
		year_d1_shown_offset = gt_year->digit_1.old_offset;
		gt_year->digit_1.new_offset = year_offset_d1;
		gt_year->digit_1.old_offset = year_offset_d1 - uv;
		year_d1_flip = 1;
		gt_year->digit_1.flip = 1;

	}
	else if (year_d1_flip == 1) {//middle of flipping
		gt_year->digit_1.flip -= 0.1;
		if (gt_year->digit_1.flip <= 0) {
			year_d1_shown_offset = gt_year->digit_1.new_offset; //show new_offset once flipped past halfway
		}
		if (gt_year->digit_1.flip <= -1) {
			gt_year->digit_1.flip = -1;
			year_d1_flip = 0; //indicate done flipping;
		}

	}
	else if (year_d1_flip == 0) {//done flipping, waiting for new flip
		gt_year->digit_1.old_offset = year_offset_d1;
	}

	if (gt_year->digit_2.new_offset != year_offset_d2) { //need to flip
		year_d2_shown_offset = gt_year->digit_2.old_offset;
		gt_year->digit_2.new_offset = year_offset_d2;
		gt_year->digit_2.old_offset = year_offset_d2 - uv;
		year_d2_flip = 1;
		gt_year->digit_2.flip = 1;

	}
	else if (year_d2_flip == 1) {//middle of flipping
		gt_year->digit_2.flip -= 0.1;
		if (gt_year->digit_2.flip <= 0) {
			year_d2_shown_offset = gt_year->digit_2.new_offset; //show new_offset once flipped past halfway
		}
		if (gt_year->digit_2.flip <= -1) {
			gt_year->digit_2.flip = -1;
			year_d2_flip = 0; //indicate done flipping;
		}

	}
	else if (year_d2_flip == 0) {//done flipping, waiting for new flip
		gt_year->digit_2.old_offset = year_offset_d2;
	}

	if (gt_year->digit_3.new_offset != year_offset_d3) { //need to flip
		year_d3_shown_offset = gt_year->digit_3.old_offset;
		gt_year->digit_3.new_offset = year_offset_d3;
		gt_year->digit_3.old_offset = year_offset_d3 - uv;
		year_d3_flip = 1;
		gt_year->digit_3.flip = 1;

	}
	else if (year_d3_flip == 1) {//middle of flipping
		gt_year->digit_3.flip -= 0.1;
		if (gt_year->digit_3.flip <= 0) {
			year_d3_shown_offset = gt_year->digit_3.new_offset; //show new_offset once flipped past halfway
		}
		if (gt_year->digit_3.flip <= -1) {
			gt_year->digit_3.flip = -1;
			year_d3_flip = 0; //indicate done flipping;
		}

	}
	else if (year_d3_flip == 0) {//done flipping, waiting for new flip
		gt_year->digit_3.old_offset = year_offset_d3;
	}
}

void GameTimer::advance_time(float real_time_seconds_elapsed)
{
	const int game_sec_per_ms = 322; //sec_in_year/music_length/1000ms
	struct tm * adv_time = localtime(&m_current_time);
	adv_time->tm_sec += (int)(real_time_seconds_elapsed * game_sec_per_ms); 
	m_current_time = mktime(adv_time);

	SplitSetDateDigits(gmtime(&m_current_time)->tm_mday, &gt_date,
						gmtime(&m_current_time)->tm_mon + 1, &gt_month,
						gmtime(&m_current_time)->tm_year + 1900, &gt_year);
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

	// Setting timer_vertices and indices
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
	GLint year_d0_uloc = glGetUniformLocation(effect.program, "year_d0");
	GLint year_d1_uloc = glGetUniformLocation(effect.program, "year_d1");
	GLint year_d2_uloc = glGetUniformLocation(effect.program, "year_d2");
	GLint year_d3_uloc = glGetUniformLocation(effect.program, "year_d3");

	GLint color_uloc = glGetUniformLocation(effect.program, "fcolor");

	// Setting uniform values to the currently bound program
	glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&transform);
	glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);
	glUniform4f(date_digit_0_uloc, gt_date.digit_0.old_offset, gt_date.digit_0.new_offset, date_d0_shown_offset, gt_date.digit_0.flip);
	glUniform4f(date_digit_1_uloc, gt_date.digit_1.old_offset, gt_date.digit_1.new_offset, date_d1_shown_offset, gt_date.digit_1.flip);
	glUniform4f(month_digit_0_uloc, gt_month.digit_0.old_offset, gt_month.digit_0.new_offset, month_d0_shown_offset, gt_month.digit_0.flip);
	glUniform4f(month_digit_1_uloc, gt_month.digit_1.old_offset, gt_month.digit_1.new_offset, month_d1_shown_offset, gt_month.digit_1.flip);

	glUniform4f(year_d0_uloc, gt_year.digit_0.old_offset, gt_year.digit_0.new_offset, year_d0_shown_offset, gt_year.digit_0.flip);
	glUniform4f(year_d1_uloc, gt_year.digit_1.old_offset, gt_year.digit_1.new_offset, year_d1_shown_offset, gt_year.digit_1.flip);
	glUniform4f(year_d2_uloc, gt_year.digit_2.old_offset, gt_year.digit_2.new_offset, year_d2_shown_offset, gt_year.digit_2.flip);
	glUniform4f(year_d3_uloc, gt_year.digit_3.old_offset, gt_year.digit_3.new_offset, year_d3_shown_offset, gt_year.digit_3.flip);

	float color[] = { 1.f, 1.f, 1.f };
	glUniform3fv(color_uloc, 1, color);



	// Drawing!
	glDrawElements(GL_TRIANGLES, 144, GL_UNSIGNED_SHORT, nullptr);
}
