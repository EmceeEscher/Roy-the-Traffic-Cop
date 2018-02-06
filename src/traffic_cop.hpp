#pragma once

#include "common.hpp"


using std::string;

class TrafficCop : public Renderable
{
	static Texture cop_texture;
public:
	// Creates all the associated render resources and default transform
	bool init();

	// Releases all associated resources
	void destroy();

	// Renders
	void draw(const mat3& projection)override;

	// Set rotation in radians
	void set_rotation(float radians);

private:
	vec2 m_position; // Window coordinates
	vec2 m_scale; // 1.f in each dimension. 1.f is as big as the associated texture
	float m_rotation; // in radians
};