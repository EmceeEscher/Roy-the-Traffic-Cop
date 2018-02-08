// Header
#include "game_timer.hpp"

// stlib
#include <vector>
#include <algorithm>
#include <sstream>

bool GameTimer::init(float world_scale)
{
	struct tm init_time = {0};
	init_time.tm_mday = DaysAfterUnixDate + 1; //mktime uses 1 based indexing for days
	init_time.tm_year = 70; // mktime starts from 1900 for some reason
	m_current_time = mktime(&init_time);

	return true;
}

string GameTimer::get_current_time_string()
{
	tm* current_time = gmtime(&m_current_time);
	char* month = get_month_from_index(current_time->tm_mon);

	std::stringstream time_string;
	time_string << month << " " << current_time->tm_mday << ", " << current_time->tm_year + 1900 << std::endl;
	string current_time_string = time_string.str();

	return current_time_string;
}

void GameTimer::advance_time(float real_time_seconds_elapsed)
{
	const int SECONDS_IN_DAY = 86400;

	// Hacky workaround for the fact that mktime requires ints to be passed and ints are overflowy.
	// Works for date ranges in reasonable timeframes, so works for us unless Roy is going to live to be thousands of years old
	double current_seconds_elapsed = difftime(m_current_time, 0);
	int current_days = current_seconds_elapsed / SECONDS_IN_DAY;
	int current_seconds = current_seconds_elapsed - (current_days * SECONDS_IN_DAY);
	int seconds_elapsed = real_time_seconds_elapsed * GameToRealSecondsRatio;

	struct tm new_time = {0};
	new_time.tm_sec = seconds_elapsed + current_seconds;
	new_time.tm_mday = current_days + 1;
	new_time.tm_year = 70;

	m_current_time = mktime(&new_time);
}

void GameTimer::draw(const mat3& projection) {
	// TODO
}
