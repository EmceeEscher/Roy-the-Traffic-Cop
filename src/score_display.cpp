#include "score_display.hpp"

Texture ScoreDisplay::score_tex;
TexturedVertex2 score_vertices[36];

float uv_sd;
float i_w_sd;
float s_p_sd;
gt_tracker gt_curr_score;
int d0_flip_sd = 0;
int d1_flip_sd = 0;
int d2_flip_sd = 0;
int d3_flip_sd = 0;

float d0_shown_offset_sd;
float d1_shown_offset_sd;
float d2_shown_offset_sd;
float d3_shown_offset_sd;

bool ScoreDisplay::init()
{

	//load texture
	if (!score_tex.is_valid())
	{
		if (!score_tex.load_from_file(textures_path("numbers.png")))
		{
			fprintf(stderr, "Failed to load game timer texture!");
			return false;
		}
	}

	//centre of the texture
	uv_sd = 100.f / score_tex.width; //texture uv
	i_w_sd = score_tex.width / 10.f; //individual number width
	s_p_sd = -600.f; //starting position
	float hr = score_tex.height * 0.5;

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

	gt_curr_score.digit_0.old_offset = 0.0f;
	gt_curr_score.digit_0.new_offset = uv_sd * 0;
	gt_curr_score.digit_0.flip = -1;
	d0_shown_offset_sd = uv_sd * 0;

	gt_curr_score.digit_1.old_offset = 0.0f;
	gt_curr_score.digit_1.new_offset = uv_sd * 0;
	gt_curr_score.digit_1.flip = -1;
	d1_shown_offset_sd = uv_sd * 0;

	gt_curr_score.digit_2.old_offset = 0.0f;
	gt_curr_score.digit_2.new_offset = uv_sd * 0;
	gt_curr_score.digit_2.flip = -1;
	d2_shown_offset_sd = uv_sd * 0;

	return true;
}

void ScoreDisplay::destroy() {
	glDeleteBuffers(1, &mesh.vbo);
	glDeleteBuffers(1, &mesh.ibo);
	glDeleteBuffers(1, &mesh.vao);

	glDeleteShader(effect.vertex);
	glDeleteShader(effect.fragment);
	glDeleteShader(effect.program);
}

void ScoreDisplay::SplitSetScoreDigits(int score, gt_tracker* gt_score) {
	float score_offset_d0 = std::fmodf(score, 10) * uv_sd;
	float score_offset_d1 = std::fmodf(score / 10, 10) * uv_sd;
	float score_offset_d2 = std::fmodf(score / 100, 10) * uv_sd;

	if (gt_score->digit_0.new_offset != score_offset_d0) { //need to flip
		d0_shown_offset_sd = gt_score->digit_0.old_offset;
		gt_score->digit_0.new_offset = score_offset_d0;
		gt_score->digit_0.old_offset = score_offset_d0 - uv_sd;
		d0_flip_sd = 1;
		gt_score->digit_0.flip = 1;

	}
	else if (d0_flip_sd == 1) {//middle of flipping
		gt_score->digit_0.flip -= 0.1;
		if (gt_score->digit_0.flip <= 0) {
			d0_shown_offset_sd = gt_score->digit_0.new_offset; //show new_offset once flipped past halfway
		}
		if (gt_score->digit_0.flip <= -1) {
			gt_score->digit_0.flip = -1;
			d0_flip_sd = 0; //indicate done flipping;
		}

	}
	else if (d0_flip_sd == 0) {//done flipping, waiting for new flip
		gt_score->digit_0.old_offset = score_offset_d0;
	}

	if (gt_score->digit_1.new_offset != score_offset_d1) { //need to flip
		d1_shown_offset_sd = gt_score->digit_1.old_offset;
		gt_score->digit_1.new_offset = score_offset_d1;
		gt_score->digit_1.old_offset = score_offset_d1 - uv_sd;
		d1_flip_sd = 1;
		gt_score->digit_1.flip = 1;

	}
	else if (d1_flip_sd == 1) {//middle of flipping
		gt_score->digit_1.flip -= 0.1;
		if (gt_score->digit_1.flip <= 0) {
			d1_shown_offset_sd = gt_score->digit_1.new_offset; //show new_offset once flipped past halfway
		}
		if (gt_score->digit_1.flip <= -1) {
			gt_score->digit_1.flip = -1;
			d1_flip_sd = 0; //indicate done flipping;
		}

	}
	else if (d1_flip_sd == 0) {//done flipping, waiting for new flip
		gt_score->digit_1.old_offset = score_offset_d1;
	}

	if (gt_score->digit_2.new_offset != score_offset_d2) { //need to flip
		d2_shown_offset_sd = gt_score->digit_2.old_offset;
		gt_score->digit_2.new_offset = score_offset_d2;
		gt_score->digit_2.old_offset = score_offset_d2 - uv_sd;
		d2_flip_sd = 1;
		gt_score->digit_2.flip = 1;

	}
	else if (d2_flip_sd == 1) {//middle of flipping
		gt_score->digit_2.flip -= 0.1;
		if (gt_score->digit_2.flip <= 0) {
			d2_shown_offset_sd = gt_score->digit_2.new_offset; //show new_offset once flipped past halfway
		}
		if (gt_score->digit_2.flip <= -1) {
			gt_score->digit_2.flip = -1;
			d2_flip_sd = 0; //indicate done flipping;
		}

	}
	else if (d2_flip_sd == 0) {//done flipping, waiting for new flip
		gt_score->digit_2.old_offset = score_offset_d2;
	}

}

void ScoreDisplay::update_score(int new_score)
{
	SplitSetScoreDigits(new_score, &gt_curr_score);
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
	GLint d0_uloc = glGetUniformLocation(effect.program, "year_d0");
	GLint d1_uloc = glGetUniformLocation(effect.program, "year_d1");
	GLint d2_uloc = glGetUniformLocation(effect.program, "year_d2");
	GLint d3_uloc = glGetUniformLocation(effect.program, "year_d3");

	GLint color_uloc = glGetUniformLocation(effect.program, "fcolor");

	// Setting uniform values to the currently bound program
	glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&transform);
	glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);

	glUniform4f(d0_uloc, gt_curr_score.digit_0.old_offset, gt_curr_score.digit_0.new_offset, d0_shown_offset_sd, gt_curr_score.digit_0.flip);
	glUniform4f(d1_uloc, gt_curr_score.digit_1.old_offset, gt_curr_score.digit_1.new_offset, d1_shown_offset_sd, gt_curr_score.digit_1.flip);
	glUniform4f(d2_uloc, gt_curr_score.digit_2.old_offset, gt_curr_score.digit_2.new_offset, d2_shown_offset_sd, gt_curr_score.digit_2.flip);
	glUniform4f(d3_uloc, gt_curr_score.digit_3.old_offset, gt_curr_score.digit_3.new_offset, d3_shown_offset_sd, gt_curr_score.digit_3.flip);

	float color[] = { 1.f, 1.f, 1.f };
	glUniform3fv(color_uloc, 1, color);



	// Drawing!
	glDrawElements(GL_TRIANGLES, 54, GL_UNSIGNED_SHORT, nullptr);
}