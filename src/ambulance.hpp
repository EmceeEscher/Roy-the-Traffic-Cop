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

	// Frees graphics resources
	void destroy();

	void set_level(int level);

	// clears symbol on game reset
	// void reset();

	void update(float ms, bool init);

	// Releases all associated resources
	//void destroy();

	// Renders
	void draw(const mat3& projection)override;

	void move(vec2 off);

	void set_rotation(float radians);

	void set_position(vec2 position);

	// Gets the coordinates of a specific vertex (indexing is identical to car class)
	vec2 get_vertex_pos(int index);

	rect_bounding_box get_bounding_box();

	// returns true if the given test_vertex is inside the car's bounding box
	bool check_collision(vec2 test_vertex);

	// creates an implicit line equation using P1 and P2, and then returns true if F(Ptest) <= 0
	// (not sure why it's negative here, think it's because of difference in bounding_box)
	bool check_implicit(vec2 P1, vec2 P2, vec2 Ptest);

	// Returns true if the warning has stopped flashing and the ambulance has started moving
	// (LaneManager will only check collisions for ambulances that have this set to true)
	bool is_moving();

	vec2 get_position() const;

	void initialize_pivots();
	int binomialCoefficient(int n, int k);
	void turn(float t);
	void update_rotation_on_turn(float t);

private:
	std::map<direction, vec2> m_amb_coords;
	std::map<direction, vec2> m_amb_pivots;
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
	bool m_has_started_moving;

	int m_level = 1;
	vec2 m_acceleration;
	vec2 m_velocity;
	float m_max_speed;
	float t;
	float stopping_distance;
	float stopping_distance_scale;
	float t_scale;

	// Variables for ambulance movement
	int phase;
	vec2 m_start;
	vec2 m_pivot1;
	vec2 m_pivot2;
	vec2 m_pivot3;
	vec2 m_pivot4;
	vec2 m_pivot5;
	vec2 m_end;

};
