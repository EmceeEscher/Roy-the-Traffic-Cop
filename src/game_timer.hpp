#pragma once

#include "common.hpp"
#include <ctime>
using std::string;

struct CurrentTime
{
	int year;
	int month;
	int day;
};

class GameTimer : public Renderable
{
	static Texture calendar_tex;
public:
	const long int GameToRealSecondsRatio = 
		1
		; // One real second corresponds to two days in game
	const int DaysAfterUnixDate = 17532; // January 1st, 2018: days because seconds lead to overflow

	// Creates instance
	bool init();

	void reset();

	// Returns the current in-game time
	// Format: MONTH dd, yyyy (eg: December 18, 1995)
	// Note: We're probably not going to be printing a string at the end,
	// so this function may need to change to return a struct with the month, day and year
	// Depends on how we want to display the timer
	CurrentTime get_current_time();

	// Advances the current in-game time by the appropriate amount of time for the clock tick
	void advance_time(float real_time_seconds_elapsed, int level);

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
