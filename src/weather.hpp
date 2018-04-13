#pragma once

#include "common.hpp"
#include "weather.hpp"

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
	void update(float ms);

	// Set displayed splash screen texture
	void SetWeatherTexLocs(int weather_loc);

	// Reset display screen values;
	void reset();

private:
	TexturedVertex vertices[4];
	vec2 m_position; // Window coordinates
	vec2 m_scale; // 1.f in each dimension. 1.f is as big as the associated texture
};
