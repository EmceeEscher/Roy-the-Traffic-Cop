#include "high_scores.hpp"

Texture HighScores::hs_tex;
TexturedVertex2 hs_vertices[36];

float uv_hs;
float i_w_hs;
float s_p_hs;

int d0_flip_hs = 0;
int d1_flip_hs = 0;
int d2_flip_hs = 0;
int d3_flip_hs = 0;

gt_tracker gt_final_score;

float d0_shown_offset_hs;
float d1_shown_offset_hs;
float d2_shown_offset_hs;
float d3_shown_offset_hs;

bool HighScores::init()
{
	std::ifstream infile;
	std::string line;

	infile.open(high_scores_file);

	//load texture
	if (!hs_tex.is_valid())
	{
		if (!hs_tex.load_from_file(textures_path("numbers.png")))
		{
			fprintf(stderr, "Failed to load game timer texture!");
			return false;
		}
	}

	//centre of the texture
	uv_hs = 100.f / hs_tex.width; //texture uv
	i_w_hs = hs_tex.width / 10.f; //individual number width
	s_p_hs = -600.f; //starting position
	float hr = hs_tex.height * 0.5;

	//year d0 top -- new
	hs_vertices[0].position = { s_p_hs + 20.f * i_w_hs, 0  , 0.f };
	hs_vertices[1].position = { s_p_hs + 20.f * i_w_hs, 0  , 0.f };
	hs_vertices[2].position = { s_p_hs + 20.f * i_w_hs, 0  , 0.f };
	hs_vertices[3].position = { s_p_hs + 20.f * i_w_hs, 0  , 0.f };
	hs_vertices[0].texcoord = { s_p_hs + 20.f * i_w_hs, 0  , 0.f };
	hs_vertices[1].texcoord  = { s_p_hs + 20.f * i_w_hs, 0  , 0.f };
	hs_vertices[2].texcoord = { s_p_hs + 20.f * i_w_hs, 0  , 0.f };
	hs_vertices[3].texcoord = { s_p_hs + 20.f * i_w_hs, 0  , 0.f };

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
	glBufferData(GL_ARRAY_BUFFER, sizeof(TexturedVertex2) * 36, hs_vertices, GL_STATIC_DRAW);

	// Index Buffer creation
	glGenBuffers(1, &mesh.ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * 54, indices, GL_STATIC_DRAW);

	// Vertex Array (Container for Vertex + Index buffer)
	glGenVertexArrays(1, &mesh.vao);

	// Loading shaders
	effect.load_from_file(shader_path("textured.vs.glsl"), shader_path("textured.fs.glsl"));

	// Setting initial values, scale is negative to make it face the opposite way
	m_scale.x = 0.4;
	m_scale.y = 0.4;
	m_position = { 100.f, 80.f };

	gt_final_score.digit_0.old_offset = 0.0f;
	gt_final_score.digit_0.new_offset = uv_hs * 0;
	gt_final_score.digit_0.flip = -1;
	d0_shown_offset_hs = uv_hs * 0;

	gt_final_score.digit_1.old_offset = 0.0f;
	gt_final_score.digit_1.new_offset = uv_hs * 0;
	gt_final_score.digit_1.flip = -1;
	d1_shown_offset_hs = uv_hs * 0;

	gt_final_score.digit_2.old_offset = 0.0f;
	gt_final_score.digit_2.new_offset = uv_hs * 0;
	gt_final_score.digit_2.flip = -1;
	d2_shown_offset_hs = uv_hs * 0;

	int i = 0;
	while (std::getline(infile, line))
	{
		m_high_scores[i] = stoi(line);
		i++;
	}

	return true;
}

std::vector<int> HighScores::get_high_scores()
{
	return m_high_scores;
}

void HighScores::check_score_and_insert(int score)
{
	if (score > m_high_scores[4]) {
		m_high_scores[4] = score;
		std::sort(m_high_scores.rbegin(), m_high_scores.rend());

		std::ofstream ofs;
		ofs.open(high_scores_file, std::ofstream::out | std::ofstream::trunc);

		for (int i = 0; i < 5; i++) {
			ofs << m_high_scores[i]  << std::endl;
		}

		ofs.close();

	}
}

void HighScores::update_scores(int new_scores)
{
	HighScores::SplitSetScoreDigits(new_scores, &gt_final_score);
}

void HighScores::SplitSetScoreDigits(int score, gt_tracker* gt_final_score) {
	float score_offset_d0 = std::fmodf(score, 10) * uv_hs;
	float score_offset_d1 = std::fmodf(score / 10, 10) * uv_hs;
	float score_offset_d2 = std::fmodf(score / 100, 10) * uv_hs;

	gt_final_score->digit_0.old_offset = score_offset_d0;
	gt_final_score->digit_1.old_offset = score_offset_d1;
	gt_final_score->digit_2.old_offset = score_offset_d2;

}

void HighScores::draw(const mat3& projection) {
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
	glBindTexture(GL_TEXTURE_2D, hs_tex.id);

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

	glUniform4f(d0_uloc, gt_final_score.digit_0.old_offset, gt_final_score.digit_0.new_offset, d0_shown_offset_hs, gt_final_score.digit_0.flip);
	glUniform4f(d1_uloc, gt_final_score.digit_1.old_offset, gt_final_score.digit_1.new_offset, d1_shown_offset_hs, gt_final_score.digit_1.flip);
	glUniform4f(d2_uloc, gt_final_score.digit_2.old_offset, gt_final_score.digit_2.new_offset, d2_shown_offset_hs, gt_final_score.digit_2.flip);
	glUniform4f(d3_uloc, gt_final_score.digit_3.old_offset, gt_final_score.digit_3.new_offset, d3_shown_offset_hs, gt_final_score.digit_3.flip);

	float color[] = { 1.f, 1.f, 1.f };
	glUniform3fv(color_uloc, 1, color);



	// Drawing!
	glDrawElements(GL_TRIANGLES, 54, GL_UNSIGNED_SHORT, nullptr);
}