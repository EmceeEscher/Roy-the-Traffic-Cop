#pragma once
#include "car.hpp"
#include "direction.hpp"
#include "lane.hpp"
#include <vector>

class Warning : public Renderable
{
	static Texture warning_texture;
public:
	Warning();
	~Warning();
	// Creates all the associated render resources and default transform
	bool init(direction dir);

	// clears symbol on game reset
	void reset();

	void update(float ms);

	// Releases all associated resources
	void destroy();

	void set_position(vec2 position);

	// Renders
	void draw(const mat3& projection)override;

	bool show;

	bool amb_init;
private:
	std::map<direction, vec2> warning_coords;
	unsigned int warning_counter;
	direction m_dir;
	float m_prev_time;
	vec2 m_position; // Window coordinates
	vec2 m_scale; // 1.f in each dimension. 1.f is as big as the associated texture
	float m_rotation; // in radians
	float m_original_rot;

};