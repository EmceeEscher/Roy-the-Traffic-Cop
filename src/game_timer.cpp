// Header
#include "game_timer.hpp"

bool GameTimer::init()
{
	struct tm init_time = {0};
	init_time.tm_mday = DaysAfterUnixDate + 1; //mktime uses 1 based indexing for days
	init_time.tm_year = 70; // mktime starts from 1900 for some reason
	m_current_time = mktime(&init_time);
	return true;
}

CurrentTime GameTimer::get_current_time()
{
	tm* current_time = gmtime(&m_current_time);
	string month = std::string(get_month_from_index(current_time->tm_mon));
	CurrentTime return_time = {
		current_time->tm_year + 1900,
		current_time->tm_mon,
		current_time->tm_mday
	};

	printf("Month: %d Day: %d Year: %d\n", return_time.month, return_time.day, return_time.year);

	return return_time;
}

void GameTimer::advance_time(float real_time_seconds_elapsed)
{	
	const int One_Day = 86400;

	//In one second, how many days do you need to go by? X * One_Day, ie 2*, 1 second = 2 days in game. 
	struct tm * adv_time = localtime(&m_current_time);
	adv_time->tm_sec += real_time_seconds_elapsed * 2*One_Day;
	m_current_time = mktime(adv_time);
}

void GameTimer::draw(const mat3& projection) {
	// TODO
}
