#pragma once

#include "common.hpp"
#include "game_timer.hpp"

using std::string;

class DisplayScreen : public Renderable
{
	static Texture splash_screens;

public:
	// Creates all the associated render resources and default transform
	bool init();

	// Releases all associated resources
	void destroy();

	// Renders
	void draw(const mat3& projection)override;

	// Set rotation in radians
	void update(bool paused, bool show_start, bool game_over, int level, float ms);

	// Set displayed splash screen texture
	void SetSplashTexLocs(int splash_screen);

	// Reset display screen values;
	void reset();

private:
	TexturedVertex vertices[4];
	vec2 m_position; // Window coordinates
	vec2 m_scale; // 1.f in each dimension. 1.f is as big as the associated texture
	bool draw_splash; //if false, draw nothing
	float display_duration; //display splash duration
	bool start_countdown;
	int prev_level;

};
