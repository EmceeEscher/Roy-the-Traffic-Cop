#pragma once

#include "common.hpp"
#include <string>

using std::string;

enum class direction : int
{
	NORTH,
	EAST,
	SOUTH,
	WEST,
};

class Car : public Renderable
{
	static Texture car_texture;
public:
	// Creates all the associated render resources and default transform
	bool init(float world_scale);

	// Releases all associated resources
	void destroy();

	// Update position based on direction
	// ms represents the number of milliseconds elapsed from the previous update() call
	void update(float ms);

	// Renders
	void draw(const mat3& projection)override;

	// Returns the current position
	vec2 get_position()const;

	// Returns the desired direction of car
	direction get_desired_direction()const;

	// Moves the position by the specified offset
	void move(vec2 off);

	// Returns the turtle' bounding box for collision detection, called by collides_with()
	vec2 get_bounding_box()const;

	// Set rotation in radians
	void set_rotation(float radians);

private:
	vec2 m_position; // Window coordinates
	vec2 m_scale; // 1.f in each dimension. 1.f is as big as the associated texture
	float m_rotation; // in radians
	size_t m_num_indices; // passed to glDrawElements
	float m_current_speed;
	float m_x_dir;
	float m_y_dir;
	direction m_desired_direction;
};
