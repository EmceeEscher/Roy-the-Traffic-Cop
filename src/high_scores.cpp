#include "high_scores.hpp"

bool HighScores::init()
{
	std::ifstream infile;
	std::string line;

	infile.open(high_scores_file);

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