#pragma once

#include "common.hpp"


using std::string;

class Coin : public Renderable
{
	static Texture coin_texture;
public:
	// Creates all the associated render resources and default transform
	bool init();

	// Releases all associated resources
	void destroy();

	// Renders
	void draw(const mat3& projection)override;

	void update(float elapsed_ms);

	// Tells which part of the sprite sheet to look at for the animation
	void set_vertices(int coin_frame);

private:
	float m_prev_time;
	float m_fps;
	vec2 m_position; // Window coordinates
	vec2 m_scale; // 1.f in each dimension. 1.f is as big as the associated texture
	int curr_frame;
};