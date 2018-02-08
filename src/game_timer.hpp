#pragma once

#include "common.hpp"
#include <ctime>
using std::string;

class GameTimer : public Renderable
{
public:
	const long int GameToRealSecondsRatio = 172800; // One real second corresponds to two days in game
	const int DaysAfterUnixDate = 17532; // January 1st, 2018: days because seconds lead to overflow

	// Creates instance
	bool init(float world_scale);

	// Returns the current in-game time
	// Format: MONTH dd, yyyy (eg: December 18, 1995)
	// Note: We're probably not going to be printing a string at the end,
	// so this function may need to change to return a struct with the month, day and year
	// Depends on how we want to display the timer
	string get_current_time_string();

	// Advances the current in-game time by the appropriate amount of time for the clock tick
	void advance_time(float real_time_seconds_elapsed);

	// Renders
	void draw(const mat3& projection) override;

private:
	time_t m_current_time; // Current date
};
