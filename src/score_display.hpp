#pragma once

#include "common.hpp"

class ScoreDisplay : public Renderable
{
	static Texture score_tex;
public:
	// Creates instance
	bool init();

	void destroy();

	// Advances the current in-game time by the appropriate amount of time for the clock tick
	void update_score(int new_score);

	// Split and set Date digits for texture offset
	void SplitSetScoreDigits(int score, gt_tracker* gt_score);

	// Renders
	void draw(const mat3& projection) override;

private:
	vec2 m_scale;
	vec2 m_position;
};