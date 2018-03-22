#pragma once

#include "common.hpp"
#include "game_timer.hpp"


using std::string;

class DisplayScreen : public Renderable
{
	static Texture start_splash_tex;
public:
	// Creates all the associated render resources and default transform
	bool init();

	// Releases all associated resources
	void destroy();

	// Renders
	void draw(const mat3& projection)override;

	// Set rotation in radians
	void update(bool paused, bool start_start, bool game_over, CurrentTime game_time);

private:
	vec2 m_position; // Window coordinates
	vec2 m_scale; // 1.f in each dimension. 1.f is as big as the associated texture

};
