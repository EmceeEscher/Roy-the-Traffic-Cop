#pragma once

#include "common.hpp"

// Background
class Background : public Renderable
{
	static Texture background_texture;

public:
	// Creates all the associated render resources and default transform
	bool init(float world_scale);

	// Releases all the associated resources
	void destroy();

	// Renders the background
	// projection is the 2D orthographic projection matrix
	void draw(const mat3& projection)override;

	// Returns the background' bounding box for collision detection, called by collides_with()
	vec2 get_bounding_box()const;

private:
	vec2 m_position; // Window coordinates
	vec2 m_scale; // 1.f in each dimension. 1.f is as big as the associated texture
};
