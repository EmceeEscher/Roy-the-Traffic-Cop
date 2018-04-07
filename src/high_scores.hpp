#pragma once

#include <algorithm>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

class HighScores
{
public:

	bool init();

	// Retrieve the current top 5 scores
	std::vector<int> get_high_scores();

	// Insert the current score into the top 5 if applicable
	void check_score_and_insert(int score);

private:
	std::string high_scores_file = "../src/high_scores.txt";
	std::vector<int> m_high_scores = std::vector<int>(5);

};
