#pragma once

#include "common.hpp"

class ScoreDisplay : public Renderable
{
	static Texture score_tex;

public:

	// Creates instance
	bool init();

	// Split and set digits for texture offset
	void SplitSetScoreDigits(int score, gt_tracker* gt_points);

	// Renders the score
	void draw(const mat3& projection) override;
	
	// increments the score by one (display won't cross 999)
	void update_score(int score);

private:
	
	int m_current_score;
	vec2 m_scale;
	vec2 m_position;
};