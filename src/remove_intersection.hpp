#pragma once
#include "car.hpp"
#include "direction.hpp"
#include "lane.hpp"
#include <vector>

class RemoveIntersection: public Renderable 
{
	static Texture spacebar_texture;
public:
	// Creates all the associated render resources and default transform
	bool init();

	// Releases all associated resources
	void destroy();

	// Renders
	void draw(const mat3& projection)override;

private:
	vec2 m_position; // Window coordinates
	vec2 m_scale; // 1.f in each dimension. 1.f is as big as the associated texture
};