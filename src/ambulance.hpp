#pragma once
#include "direction.hpp"
#include "lane.hpp"
#include <vector>

class Ambulance : public Renderable
{
	static Texture ambulance_texture;
public:
	Ambulance();
	~Ambulance();
	// Creates all the associated render resources and default transform
	bool init(direction dir);

	void set_level(int level);

	// clears symbol on game reset
	void reset();

	void update(float ms);

	// Releases all associated resources
	void destroy();

	void set_position(vec2 position);

	void set_lane(direction dir);

	// Renders
	void draw(const mat3& projection)override;

	void move(vec2 off);

	void set_rotation(float radians);

	void update_rotation_on_turn(float t);

	void turn(float t);

	vec2 find_end_point(vec2 p1, vec2 p2, float angle);

	char calculate_turn_dir(direction lane_dir, direction desired_dir);

	int binomialCoefficient(int n, int k);

	bool show;

private:
	std::map<direction, vec2> m_amb_coords;
	std::map<direction, float> m_amb_rotation;
	direction m_dir;
	float m_prev_time;
	vec2 m_position; // Window coordinates
	vec2 m_scale; // 1.f in each dimension. 1.f is as big as the associated texture
	float m_rotation; // in radians
	float m_original_rot;
	direction m_lane;
	vec2 m_turn_pivot;
	vec2 m_turn_start_pos;
	direction m_desired_direction;


	int m_level = 1;
	vec2 m_acceleration;
	vec2 m_velocity;
	float m_max_speed;
	float t;
	float stopping_distance;
	float stopping_distance_scale;
	float t_scale;
};