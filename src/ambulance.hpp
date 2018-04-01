#pragma once
#include "car.hpp"
#include "direction.hpp"
#include "lane.hpp"
#include <vector>

class Ambulance : public Renderable
{
	static Texture ambulance_texture;
public:
	// Creates all the associated render resources and default transform
	bool init();

	// clears symbol on game reset
	void reset();

	void update(float ms);

	// Releases all associated resources
	void destroy();

	void set_position(vec2 position);

	void set_lane(direction dir);

	// Renders
	void draw(const mat3& projection)override;

	bool show;

private:
	direction m_dir;
	float m_prev_time;
	vec2 m_position; // Window coordinates
	vec2 m_scale; // 1.f in each dimension. 1.f is as big as the associated texture
	float m_rotation; // in radians
	float m_original_rot;
};