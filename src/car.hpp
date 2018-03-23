#pragma once

#include "common.hpp"
#include "direction.hpp"
#include "placard.hpp"
#include "turn_direction.hpp"
#include <string>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <stdexcept>

#define SDL_MAIN_HANDLED
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>

using std::string;

class Car : public Renderable
{
	static Texture car_texture;

public:

	TexturedVertex vertices[12]; //Vertices of triangles in Car mesh
	struct Triangle {
		vec2 a, b, c;
	};


	// Creates all the associated render resources and default transform
	bool init(bool isVillain);

	// Releases all associated resources
	void destroy();

	// Update position based on direction
	// ms represents the number of milliseconds elapsed from the previous update() call
	void update(float ms);

	// Renders
	void draw(const mat3& projection)override;

	// Returns the current position
	vec2 get_position()const;

	// Returns whether the car is a villain
	bool is_villain()const;

	uint16_t get_index(int index);

	float get_triangle_area(vec2 p1, vec2 p2, vec2 p3);

	vec2 get_vertex_pos(int index);

	// Returns the desired direction of car
	direction get_desired_direction()const;

	// Sets the desired direction for the car and removes it's villainy
	void set_desired_direction(direction turn_dir);

	// Randomly assignes the desired direction for the car
	// Call after set_lane to ensure desired direction != our lane
	void generate_desired_direction();

	// Moves the position by the specified offset
	void move(vec2 off);

	// Set rotation in radians
	void set_rotation(float radians);

	// Set initial rotation
	void set_original_rotation(float radians);

	void set_position(vec2 position);

	void set_at_intersection(bool boolean);

	// get or set the lane associated with car
	void set_lane(direction dir);

	direction get_lane();

	// car to slows down to stop sign or speeds away
	void slow_down();
	void speed_up();

	// Set indicator for passage
	void signal_to_move();

	// get acceleration, velocity, max_speed
	vec2 get_acc();
	vec2 get_vel();
	float get_max_speed();

	vec2 get_scale();

	// Intersection Detection
	bool is_approaching_stop(vec2 lane_pos);

	bool is_at_stop(vec2 lane_pos);

	// Calculation for safe stopping distance
	float compute_stopping_dis(float velocity, float acc);

	// Return if in or beyond intersection
	bool is_in_beyond_intersec();

	// Starts the color change of the placard
	void start_timer(float max_time);

	bool is_at_front();

	// Turning function according to a given direction
	void turn(float t);

	// Helper for Bezier construction
	int binomialCoefficient(int n, int k);

	// Helper 2 for Bezier construction
	vec2 find_end_point(vec2 p1, vec2 p2, float angle);

	// Helper 3 for determing the turn direction
	char calculate_turn_dir(direction lane_dir, direction desired_dir);

	// Make the car rotate when turning
	void update_rotation_on_turn(float t);

	void set_turn_start(vec2 start_pos);

	// get the car's turning direction
	turn_direction get_turn_direction();

	// returns a struct containing the four corners of the cars boundary box
	// directions (e.g. bottom_left) are relative to car's original coordinates,
	// NOT after rotation (so if you rotate by PI, "bottom_left" will be on top right)
	rect_bounding_box get_bounding_box();

	// returns true if the given test_vertex is inside the car's bounding box
	bool check_collision(vec2 test_vertex);

	bool check_mesh_collision(vec2 test_vertex, Triangle t);

	// creates an implicit line equation using P1 and P2, and then returns true if F(Ptest) >= 0
	bool check_implicit(vec2 P1, vec2 P2, vec2 Ptest);

	// gets called when a car gets in a collision in the intersection. The parameter
	// is which triangle in the mesh got hit if the car is the victim, -1 if this car is the attacker
	void collided(int hit_triangle);

	// returns the direction that the car should move after getting in a collision
	// based on which triangle in the mesh got hit
	vec2 get_collision_direction(int hit_triangle);

	float get_collision_spin(int hit_triangle);

	void spinout();

	void change_color();

	bool is_hit();

	// useful debug method for testing car's turn placard direction
	turn_direction get_placard_direction();

private:
	float m_color[3];
	vec2 m_position; // Window coordinates
	vec2 m_scale; // 1.f in each dimension. 1.f is as big as the associated texture
	float m_rotation; // in radians
	float m_original_rot;
	size_t m_num_indices; // passed to glDrawElements
	vec2 m_velocity;
	vec2 m_acceleration;
	bool m_can_move;
	bool m_is_villain;
	vec2 m_displacement;
	direction m_lane;
	direction m_desired_direction;
	vec2 m_turn_start_pos;
	vec2 m_turn_pivot;
	float m_max_speed;
	float m_wr; //half width of car texture
	float m_hr; //half height of car texture
	float t;
	bool m_in_beyond_intersection;
	bool m_turned;
	bool m_hit; //car collided or not
	Placard* m_turn_placard;
	bool m_at_intersection;
	float car_tex_x0; //specifies near x offset of the indicated car texture
	float m_spin_amount;
	Mix_Chunk* m_crash;
};
