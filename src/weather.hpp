#pragma once

#include "common.hpp"
#include "weather.hpp"
#include "game_timer.hpp"

using std::string;

class Weather : public Renderable
{
	static Texture weather_screens;

public:
	// Creates all the associated render resources and default transform
	bool init();

	// Releases all associated resources
	void destroy();

	// Renders
	void draw(const mat3& projection)override;

	// update
	void update(float ms, int game_level, CurrentTime game_time);

	// Set displayed splash screen texture
	void SetWeatherTexLocs(int weather_loc);

	// Reset display screen values;
	void reset();

	// which sky should it be? dusk/dawn/night etc
	void determine_sky();

	// which shader effect to use? heat/snow/rain
	void determine_condition(string condition);

	// sets weather conditions for debugging
	void set_condition(string condition);

private:
	TexturedVertex vertices[4];
	vec2 m_position; // Window coordinates
	vec2 m_scale; // 1.f in each dimension. 1.f is as big as the associated texture


	int game_month;
	float weather_timer;
	int curr_weather_loc;
};
