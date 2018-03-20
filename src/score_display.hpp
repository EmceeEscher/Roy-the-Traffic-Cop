#pragma once

#include "common.hpp"

/*
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

*/

#include <ctime>
using std::string;

struct CurrentTime_sd
{
	int year;
	int month;
	int day;
};

class ScoreDisplay : public Renderable
{
	static Texture calendar_tex_sd;
public:
	const long int GameToRealSecondsRatio_sd = 
		1
		; // One real second corresponds to two days in game
	const int DaysAfterUnixDate_sd = 17532; // January 1st, 2018: days because seconds lead to overflow

	// Creates instance
	bool init();

	// Advances the current in-game time by the appropriate amount of time for the clock tick
	void advance_time(float real_time_seconds_elapsed);

	// Split and set Date digits for texture offset
	void SplitSetDateDigits(int day, gt_tracker* gt_day, 
							int mon, gt_tracker* gt_mon,
							int year, gt_tracker* gt_year);

	// Renders
	void draw(const mat3& projection) override;

private:
	time_t m_current_time; // Current date
	vec2 m_scale;
	vec2 m_position;
};