#include "score_display.hpp"

Texture ScoreDisplay::score_tex;
TexturedVertex2 score_vertices[36];

float uv;
float i_w;
float s_p;

int d0_flip = 0;
int d1_flip = 0;
int d2_flip = 0;

gt_tracker gt_score;

float d0_shown_offset;
float d1_shown_offest;
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
	uv = 100.f / score_tex.width; //texture uv
	i_w = score_tex.width / 10.f; //individual number width
	s_p = -600.f; //starting position
	float hr = score_tex.height * 0.5;

	//digit 0 top -- new
	score_vertices[0].position = { s_p + i_w	    , 0  , 0.f };
	score_vertices[1].position = { s_p + 2 * i_w  , 0  , 0.f };
	score_vertices[2].position = { s_p + 2 * i_w	, -hr, 0.f };
	score_vertices[3].position = { s_p + i_w		, -hr, 0.f };
	score_vertices[0].texcoord = { uv * 0.f, 0.5  , 0.1f };
	score_vertices[1].texcoord = { uv * 1.f, 0.5  , 0.1f };
	score_vertices[2].texcoord = { uv * 1.f, 0.0f , 0.1f };
	score_vertices[3].texcoord = { uv * 0.f, 0.0f , 0.1f };

	//digit 0 bottom -- old
	score_vertices[4].position = { s_p + i_w		, +hr, 0.f };
	score_vertices[5].position = { s_p + 2 * i_w   , +hr, 0.f };
	score_vertices[6].position = { s_p + 2 * i_w	, 0, 0.f };
	score_vertices[7].position = { s_p + i_w		, 0, 0.f };
	score_vertices[4].texcoord = { uv * 0.f, 1.0f,  0.11f };
	score_vertices[5].texcoord = { uv * 1.f, 1.0f,  0.11f };
	score_vertices[6].texcoord = { uv * 1.f, 0.5f, 0.11f };
	score_vertices[7].texcoord = { uv * 0.f, 0.5f,  0.11f };

	//display for digit 0 -- rotating
	score_vertices[8].position = { s_p + i_w		 , +hr, 0.1f };
	score_vertices[9].position = { s_p + 2 * i_w  , +hr, 0.1f };
	score_vertices[10].position = { s_p + 2 * i_w	 , -hr, 0.1f };
	score_vertices[11].position = { s_p + i_w		 , -hr, 0.1f };
	score_vertices[8].texcoord = { uv * 0, 1.0f, 1.0f };
	score_vertices[9].texcoord = { uv * 1, 1.0f, 1.0f };
	score_vertices[10].texcoord = { uv * 1, 0.0f, 1.0f };
	score_vertices[11].texcoord = { uv * 0, 0.0f, 1.0f };


	//digit 1 top -- new
	score_vertices[12].position = { s_p      , 0  , 0.f };
	score_vertices[13].position = { s_p + i_w, 0  , 0.f };
	score_vertices[14].position = { s_p + i_w, -hr, 0.f };
	score_vertices[15].position = { s_p      , -hr, 0.f };
	score_vertices[12].texcoord = { uv  * 0.f, 0.5f, 0.2f };
	score_vertices[13].texcoord = { uv  * 1.f, 0.5f, 0.2f };
	score_vertices[14].texcoord = { uv  * 1.f, 0.0f, 0.2f };
	score_vertices[15].texcoord = { uv  * 0.f, 0.0f, 0.2f };

	//digit 1 bottom -- old
	score_vertices[16].position = { s_p      , +hr , 0.f };
	score_vertices[17].position = { s_p + i_w, +hr , 0.f };
	score_vertices[18].position = { s_p + i_w, 0	 , 0.f };
	score_vertices[19].position = { s_p      , 0	 , 0.f };
	score_vertices[16].texcoord = { uv  * 0.f, 1.0f, 0.22f };
	score_vertices[17].texcoord = { uv  * 1.f, 1.0f, 0.22f };
	score_vertices[18].texcoord = { uv  * 1.f, 0.5f, 0.22f };
	score_vertices[19].texcoord = { uv  * 0.f, 0.5f, 0.22f };

	//display for digit 1 -- rotating
	score_vertices[20].position = { s_p 	, +hr, 0.2f };
	score_vertices[21].position = { s_p + i_w , +hr, 0.2f };
	score_vertices[22].position = { s_p + i_w	, -hr, 0.2f };
	score_vertices[23].position = { s_p	, -hr, 0.2f };
	score_vertices[20].texcoord = { uv * 0, 1.0f, 2.0f };
	score_vertices[21].texcoord = { uv * 1, 1.0f, 2.0f };
	score_vertices[22].texcoord = { uv * 1, 0.0f, 2.0f };
	score_vertices[23].texcoord = { uv * 0, 0.0f, 2.0f };

	//digit 2 top -- new
	score_vertices[24].position = { s_p + 3.5f * i_w	, 0  , 0.f };
	score_vertices[25].position = { s_p + 4.5f * i_w , 0  , 0.f };
	score_vertices[26].position = { s_p + 4.5f * i_w	, -hr, 0.f };
	score_vertices[27].position = { s_p + 3.5f * i_w	, -hr, 0.f };
	score_vertices[24].texcoord = { uv * 0.f, 0.5  , 0.3f };
	score_vertices[25].texcoord = { uv * 1.f, 0.5  , 0.3f };
	score_vertices[26].texcoord = { uv * 1.f, 0.0f , 0.3f };
	score_vertices[27].texcoord = { uv * 0.f, 0.0f , 0.3f };

	//digit 2 bottom -- old
	score_vertices[28].position = { s_p + 3.5f * i_w	, +hr, 0.f };
	score_vertices[29].position = { s_p + 4.5f * i_w , +hr, 0.f };
	score_vertices[30].position = { s_p + 4.5f * i_w	, 0, 0.f };
	score_vertices[31].position = { s_p + 3.5f * i_w	, 0, 0.f };
	score_vertices[28].texcoord = { uv * 0.f, 1.0f,  0.33f };
	score_vertices[29].texcoord = { uv * 1.f, 1.0f,  0.33f };
	score_vertices[30].texcoord = { uv * 1.f, 0.5f,  0.33f };
	score_vertices[31].texcoord = { uv * 0.f, 0.5f,  0.33f };

	//display for digit 2 -- rotating
	score_vertices[32].position = { s_p + 3.5f * i_w	, +hr, 0.3f };
	score_vertices[33].position = { s_p + 4.5f * i_w , +hr, 0.3f };
	score_vertices[34].position = { s_p + 4.5f * i_w	, -hr, 0.3f };
	score_vertices[35].position = { s_p + 3.5f * i_w	, -hr, 0.3f };
	score_vertices[32].texcoord = { uv * 0, 1.0f, 3.0f };
	score_vertices[33].texcoord = { uv * 1, 1.0f, 3.0f };
	score_vertices[34].texcoord = { uv * 1, 0.0f, 3.0f };
	score_vertices[35].texcoord = { uv * 0, 0.0f, 3.0f };


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
	gt_score.digit_0.new_offset = uv * 1;
	gt_score.digit_0.flip = -1;
	score_d0_shown_offset = uv * 1;

	gt_score.digit_1.old_offset = 0.0f;
	gt_score.digit_1.new_offset = 0.0f;
	gt_score.digit_1.flip = -1;
	score_d1_shown_offset = 0.f;

	gt_score.digit_2.old_offset = 0.0f;
	gt_score.digit_2.new_offset = uv * 2;
	gt_score.digit_2.flip = -1;
	score_d2_shown_offset = uv * 2;

	return true;
}

void ScoreDisplay::SplitSetScoreDigits(int score, gt_tracker* gt_points) {
	float score_offset_digit0 = std::fmodf(score, 10) * uv;
	float score_offset_digit1 = std::fmodf(score / 10, 10) * uv;
	float score_offset_digit2 = std::fmodf(score / 100, 10) * uv;

	if (gt_points->digit_0.new_offset != score_offset_digit0) { // need to flip
		// prepare flip
		d0_shown_offset = gt_points->digit_0.old_offset;
		gt_points->digit_0.new_offset = score_offset_digit0;
		gt_points->digit_0.old_offset = score_offset_digit0 - uv;
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
		gt_points->digit_1.old_offset = score_offset_digit1 - uv;
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
		gt_points->digit_2.old_offset = score_offset_digit2 - uv;
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
}