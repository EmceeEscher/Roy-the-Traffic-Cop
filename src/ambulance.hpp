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
	// void reset();

	void update(float ms, bool init);

	// Releases all associated resources
	//void destroy();

	//void set_lane(direction dir);

	// Renders
	void draw(const mat3& projection)override;

	void move(vec2 off);

	void set_rotation(float radians);

	void set_position(vec2 position);

	vec2 get_vertex_pos(int index);

	//void update_rotation_on_turn(float t);

	//void turn(float t);

	//vec2 find_end_point(vec2 p1, vec2 p2, float angle);

	//char calculate_turn_dir(direction lane_dir, direction desired_dir);

	//int binomialCoefficient(int n, int k);

	rect_bounding_box get_bounding_box();

	// returns true if the given test_vertex is inside the car's bounding box
	bool check_collision(vec2 test_vertex);

	// creates an implicit line equation using P1 and P2, and then returns true if F(Ptest) >= 0
	bool check_implicit(vec2 P1, vec2 P2, vec2 Ptest);

	//bool show;

private:
	std::map<direction, vec2> m_amb_coords;
	std::map<direction, float> m_amb_rotation;
	std::vector<TexturedVertex> amb_vertices;

	direction m_dir;
	float m_prev_time;
	vec2 m_position; // Window coordinates
	vec2 m_scale;
	float m_rotation; // in radians
	float m_original_rot;
	direction m_lane;
	vec2 m_turn_pivot;
	vec2 m_turn_start_pos;
	direction m_desired_direction;
	float m_wr; //half width of amb texture
	float m_hr; //half height of amb texture

	int m_level = 1;
	vec2 m_acceleration;
	vec2 m_velocity;
	float m_max_speed;
	float t;
	float stopping_distance;
	float stopping_distance_scale;
	float t_scale;
};
