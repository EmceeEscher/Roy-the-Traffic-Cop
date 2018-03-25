#pragma once
#include "car.hpp"
#include "direction.hpp"
#include "lane.hpp"
#include <vector>

class RemoveIntersection: public Renderable 
{
	static Texture spacebar1_texture;
	static Texture spacebar2_texture;
	static Texture spacebar_texture;
public:
	RemoveIntersection();
	~RemoveIntersection();
	// Creates all the associated render resources and default transform
	bool init();

	// clears symbol on game reset
	void reset();

	void update(float ms, int hit_count);

	// Releases all associated resources
	void destroy();

	// Renders
	void draw(const mat3& projection)override;

	void increment();

	bool show;
	unsigned int m_press;


private:
	float m_prev_time;
	vec2 m_position; // Window coordinates
	vec2 m_scale; // 1.f in each dimension. 1.f is as big as the associated texture
};