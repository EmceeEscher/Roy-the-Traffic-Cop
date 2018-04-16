#include "high_scores.hpp"

Texture HighScores::hs_tex;

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
			fprintf(stderr, "Failed to load game number texture!");
			return false;
		}
	}

	// The position (0,0) corresponds to the center of the texture
	float wr = hs_tex.width * 0.5;
	float hr = hs_tex.height * 0.5;
	float indiv_splash_width = 100.f;

	vertices[0].position = { -indiv_splash_width / 2, +hr, 0.f };
	vertices[1].position = { +indiv_splash_width / 2, +hr, 0.f };
	vertices[2].position = { +indiv_splash_width / 2, -hr, 0.f };
	vertices[3].position = { -indiv_splash_width / 2, -hr, 0.f };
	vertices[4].position = { -indiv_splash_width / 2 + 100, +hr, 0.f };
	vertices[5].position = { +indiv_splash_width / 2 + 100, +hr, 0.f };
	vertices[6].position = { +indiv_splash_width / 2 + 100, -hr, 0.f };
	vertices[7].position = { -indiv_splash_width / 2 + 100, -hr, 0.f };
	 vertices[8].position = { -indiv_splash_width / 2 + 200, +hr, 0.f };
	 vertices[9].position = { +indiv_splash_width / 2 + 200, +hr, 0.f };
	vertices[10].position = { +indiv_splash_width / 2 + 200, -hr, 0.f };
	vertices[11].position = { -indiv_splash_width / 2 + 200, -hr, 0.f };
	vertices[12].position = { -indiv_splash_width / 2, +hr-120, 0.f };
	vertices[13].position = { +indiv_splash_width / 2, +hr-120, 0.f };
	vertices[14].position = { +indiv_splash_width / 2, -hr-120, 0.f };
	vertices[15].position = { -indiv_splash_width / 2, -hr-120, 0.f };
	vertices[16].position = { -indiv_splash_width / 2 + 100, +hr-120, 0.f };
	vertices[17].position = { +indiv_splash_width / 2 + 100, +hr-120, 0.f };
	vertices[18].position = { +indiv_splash_width / 2 + 100, -hr-120, 0.f };
	vertices[19].position = { -indiv_splash_width / 2 + 100, -hr-120, 0.f };
	vertices[20].position = { -indiv_splash_width / 2 + 200, +hr-120, 0.f };
	vertices[21].position = { +indiv_splash_width / 2 + 200, +hr-120, 0.f };
	vertices[22].position = { +indiv_splash_width / 2 + 200, -hr-120, 0.f };
	vertices[23].position = { -indiv_splash_width / 2 + 200, -hr-120, 0.f };
	vertices[24].position = { -indiv_splash_width / 2, +hr - 240, 0.f };
	vertices[25].position = { +indiv_splash_width / 2, +hr - 240, 0.f };
	vertices[26].position = { +indiv_splash_width / 2, -hr - 240, 0.f };
	vertices[27].position = { -indiv_splash_width / 2, -hr - 240, 0.f };
	vertices[28].position = { -indiv_splash_width / 2 + 100, +hr - 240, 0.f };
	vertices[29].position = { +indiv_splash_width / 2 + 100, +hr - 240, 0.f };
	vertices[30].position = { +indiv_splash_width / 2 + 100, -hr - 240, 0.f };
	vertices[31].position = { -indiv_splash_width / 2 + 100, -hr - 240, 0.f };
	vertices[32].position = { -indiv_splash_width / 2 + 200, +hr - 240, 0.f };
	vertices[33].position = { +indiv_splash_width / 2 + 200, +hr - 240, 0.f };
	vertices[34].position = { +indiv_splash_width / 2 + 200, -hr - 240, 0.f };
	vertices[35].position = { -indiv_splash_width / 2 + 200, -hr - 240, 0.f };

	glGenBuffers(1, &mesh.vbo);
	glGenBuffers(1, &mesh.ibo);

	// Vertex Array (Container for Vertex + Index buffer)
	glGenVertexArrays(1, &mesh.vao);
	if (gl_has_errors())
		return false;

	// Loading shaders
	if (!effect.load_from_file(shader_path("textured.vs.glsl"), shader_path("textured.fs.glsl")))
		return false;

	// Setting initial values, scale is negative to make it face the opposite way
	// 1.0 would be as big as the original texture
	m_scale.x = 0.4;
	m_scale.y = 0.4;
	m_position.x = 460;
	m_position.y = 590;
	return true;


	int i = 0;
	while (std::getline(infile, line))
	{
		m_high_scores[i] = stoi(line);
		i++;
	}

	return true;
}

void HighScores::readScores(std::vector<int> scores) {
	printf("%i", m_high_scores[0]);
}

std::vector<int> HighScores::parseDigits(int hs) {
	std::vector<int> digits = std::vector<int>(3);
	digits[0] = hs / 100 % 10;
	digits[1] = hs / 10 % 10;
	digits[2] = hs % 10;

	return digits;
}

void HighScores::SetHighScoreLocs() {
	//TODO: Need to update these with the actual score, need to use a helper function to read in the stream? 
	//d2, d1, d0 = 789 (press N during game play to trigger game over)
	//readScores(m_high_scores);
	//std::vector<int> hs1 = std::vector<int>(3);
	std::vector<int> hs2 = parseDigits(m_high_scores[2]);

	int score_locs_l1_d0 = hs2[2]; //lowest score
	int score_locs_l1_d1 = hs2[1]; //lowest score
	int score_locs_l1_d2 = hs2[0]; //lowest score

	std::vector<int> hs1 = parseDigits(m_high_scores[1]);

	int score_locs_l2_d0 = hs1[2]; //middle score
	int score_locs_l2_d1 = hs1[1]; //middle score
	int score_locs_l2_d2 = hs1[0]; //middle score

	std::vector<int> hs0 = parseDigits(m_high_scores[0]);

	int score_locs_l3_d0 = hs0[2]; //high score
	int score_locs_l3_d1 = hs0[1]; //high score
	int score_locs_l3_d2 = hs0[0]; //high score

	float texture_locs[] = {
		0.f,       
		1.f / 10.f, 
		2.f / 10.f, 
		3.f / 10.f, 
		4.f / 10.f, 
		5.f / 10.f, 
		6.f / 10.f, 
		7.f / 10.f, 
		8.f / 10.f, 
		9.f / 10.f, 
		10.f / 10.f };
	//line 1
	vertices[0].texcoord = { texture_locs[score_locs_l1_d2], 1.f };//top left
	vertices[1].texcoord = { texture_locs[score_locs_l1_d2 + 1], 1.f };//top right
	vertices[2].texcoord = { texture_locs[score_locs_l1_d2 + 1], 0.f };//bottom right
	vertices[3].texcoord = { texture_locs[score_locs_l1_d2], 0.f };//bottom left
	vertices[4].texcoord = { texture_locs[score_locs_l1_d1], 1.f };//top left
	vertices[5].texcoord = { texture_locs[score_locs_l1_d1 + 1], 1.f };//top right
	vertices[6].texcoord = { texture_locs[score_locs_l1_d1 + 1], 0.f };//bottom right
	vertices[7].texcoord = { texture_locs[score_locs_l1_d1], 0.f };//bottom left
	vertices[8].texcoord = { texture_locs[score_locs_l1_d0], 1.f };//top left
	vertices[9].texcoord = { texture_locs[score_locs_l1_d0 + 1], 1.f };//top right
	vertices[10].texcoord = { texture_locs[score_locs_l1_d0 + 1], 0.f };//bottom right
	vertices[11].texcoord = { texture_locs[score_locs_l1_d0], 0.f };//bottom left

	//line 2
	vertices[12].texcoord = { texture_locs[score_locs_l2_d2], 1.f };//top left
	vertices[13].texcoord = { texture_locs[score_locs_l2_d2 + 1], 1.f };//top right
	vertices[14].texcoord = { texture_locs[score_locs_l2_d2 + 1], 0.f };//bottom right
	vertices[15].texcoord = { texture_locs[score_locs_l2_d2], 0.f };//bottom left
	vertices[16].texcoord = { texture_locs[score_locs_l2_d1], 1.f };//top left
	vertices[17].texcoord = { texture_locs[score_locs_l2_d1 + 1], 1.f };//top right
	vertices[18].texcoord = { texture_locs[score_locs_l2_d1 + 1], 0.f };//bottom right
	vertices[19].texcoord = { texture_locs[score_locs_l2_d1], 0.f };//bottom left
	vertices[20].texcoord = { texture_locs[score_locs_l2_d0], 1.f };//top left
	vertices[21].texcoord = { texture_locs[score_locs_l2_d0 + 1], 1.f };//top right
	vertices[22].texcoord = { texture_locs[score_locs_l2_d0 + 1], 0.f };//bottom right
	vertices[23].texcoord = { texture_locs[score_locs_l2_d0], 0.f };//bottom left

	//line 3
	vertices[24].texcoord = { texture_locs[score_locs_l3_d2], 1.f };//top left
	vertices[25].texcoord = { texture_locs[score_locs_l3_d2 + 1], 1.f };//top righ
	vertices[26].texcoord = { texture_locs[score_locs_l3_d2 + 1], 0.f };//bottom r
	vertices[27].texcoord = { texture_locs[score_locs_l3_d2], 0.f };//bottom left
	vertices[28].texcoord = { texture_locs[score_locs_l3_d1], 1.f };//top left
	vertices[29].texcoord = { texture_locs[score_locs_l3_d1 + 1], 1.f };//top righ
	vertices[30].texcoord = { texture_locs[score_locs_l3_d1 + 1], 0.f };//bottom r
	vertices[31].texcoord = { texture_locs[score_locs_l3_d1], 0.f };//bottom left
	vertices[32].texcoord = { texture_locs[score_locs_l3_d0], 1.f };//top left
	vertices[33].texcoord = { texture_locs[score_locs_l3_d0 + 1], 1.f };//top righ
	vertices[34].texcoord = { texture_locs[score_locs_l3_d0 + 1], 0.f };//bottom r
	vertices[35].texcoord = { texture_locs[score_locs_l3_d0], 0.f };//bottom left

	uint16_t indices[] = { 0, 3, 1, 1, 3, 2 , 4,7,5,5,7,6,8,11,9,9,11,10,
	12, 15, 13, 13, 15, 14, 16, 19, 17, 17, 19, 18, 20, 23, 21, 21, 23, 22, 24, 27, 25, 25, 27, 26,
	28, 31, 29, 29, 31, 30, 32, 35, 33, 33, 35, 34};

	// Clearing errors
	gl_flush_errors();

	// Vertex Buffer creation
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(TexturedVertex) * 36, vertices, GL_DYNAMIC_DRAW);

	// Index Buffer creation
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * 54, indices, GL_DYNAMIC_DRAW);
}

std::vector<int> HighScores::get_high_scores()
{
	return m_high_scores;
}

void HighScores::check_score_and_insert(int score, bool game_over)
{
	is_game_over = game_over;
	SetHighScoreLocs();
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

void HighScores::draw(const mat3& projection) {
	if (is_game_over) {
		transform_begin();
		transform_translate(m_position);
		transform_scale(m_scale);
		transform_end();;

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
		glBindTexture(GL_TEXTURE_2D, hs_tex.id);

		// Setting uniform values to the currently bound program
		glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&transform);
		float color[] = { 1.f, 1.f, 1.f };
		glUniform3fv(color_uloc, 1, color);
		glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);

		// Drawing!
		glDrawElements(GL_TRIANGLES, 54, GL_UNSIGNED_SHORT, nullptr);
	}
}