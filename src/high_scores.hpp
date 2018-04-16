#pragma once

#include <algorithm>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "common.hpp"


class HighScores  : public Renderable
{
	static Texture hs_tex;
public:
	
	bool init();

	// Retrieve the current top 5 scores
	std::vector<int> get_high_scores();

	// Insert the current score into the top 5 if applicable
	void check_score_and_insert(int score, bool game_over);

	void draw(const mat3& projection) override;

	// Set displayed splash screen texture
	void SetHighScoreLocs();

	std::vector<int> HighScores::parseDigits(int hs);

private:
	std::string high_scores_file = "../src/high_scores.txt";
	std::vector<int> m_high_scores = std::vector<int>(5);
	vec2 m_scale;
	vec2 m_position;
	bool is_game_over;
	TexturedVertex vertices[36];

};
