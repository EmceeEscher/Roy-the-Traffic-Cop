#pragma once

#include "common.hpp"
#include "car.hpp"

class Lane : public Renderable
{
public:
	// Creates all the associated render resources and default transform
	bool init();

	// Releases all associated resources
	void destroy();

	// Renders
	void draw(const mat3& projection)override;

	// Returns the current time remaining on the timer
    float get_time_remaining() const;

    // Returns the cars in the lane
    Car* get_cars() const;

private:
	vec2 m_position; // Window coordinates
	vec2 m_scale; // 1.f in each dimension. 1.f is as big as the associated texture
	float m_rotation; // in radians
	size_t m_num_indices; // passed to glDrawElements
    Car* m_cars; // Cars in the lane
    float m_time_remaining; // Time remaining on timer
};