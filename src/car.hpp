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

	// get or set the lane associated with car
	void set_lane(int n);
	int get_lane();

	// car to slows down to stop sign or speeds away
	void slow_down();
	void speed_up();

	// Set indicator for passage
	void signal_to_move();

	// get acceleration, velocity, max_speed
	vec2 get_acc();
	vec2 get_vel();
	float get_max_speed();

	// Intersection Detection
	bool is_approaching_stop(vec2 lane_pos);
	
	// Calculation for safe stopping distance
	float compute_stopping_dis(float velocity, float acc);

private:
	vec2 m_position; // Window coordinates
	vec2 m_scale; // 1.f in each dimension. 1.f is as big as the associated texture
	float m_rotation; // in radians
	size_t m_num_indices; // passed to glDrawElements
	//float m_current_speed;
	//float m_x_dir;
	//float m_y_dir;
	vec2 m_velocity;
	vec2 m_acceleration;
	bool m_can_move;
	vec2 m_displacement;
	int m_lane;
	direction m_desired_direction;
	float m_max_speed;
	float m_wr;
	float m_hr;
};
