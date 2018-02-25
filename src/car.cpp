// Header
#include "car.hpp"
#include "lane.hpp"
#include "direction.hpp"

// stlib
#include <vector>
#include <algorithm>

#define PI 3.14159265


Texture Car::car_texture;


bool Car::init()
{
	// Load shared texture
	if (!car_texture.is_valid())
	{
		if (!car_texture.load_from_file(textures_path("RedCar.png")))
		{
			fprintf(stderr, "Failed to load car texture!");
			return false;
		}
	}



	// The position (0,0) corresponds to the center of the texture

	m_wr = car_texture.width * 0.5;
	m_hr = car_texture.height * 0.5;

	TexturedVertex vertices[4];
	vertices[0].position = { -m_wr, +m_hr, 0.f };
	vertices[0].texcoord = { 0.f, 1.f };//top left
	vertices[1].position = { +m_wr, +m_hr, 0.f };
	vertices[1].texcoord = { 1.f, 1.f };//top right
	vertices[2].position = { +m_wr, -m_hr, 0.f };
	vertices[2].texcoord = { 1.f, 0.f };//bottom right
	vertices[3].position = { -m_wr, -m_hr, 0.f };
	vertices[3].texcoord = { 0.f, 0.f };//bottom left

	// counterclockwise as it's the default opengl front winding direction
	uint16_t indices[] = { 0, 3, 1, 1, 3, 2 };

	// Clearing errors
	gl_flush_errors();

	// Vertex Buffer creation
	glGenBuffers(1, &mesh.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(TexturedVertex) * 4, vertices, GL_STATIC_DRAW);

	// Index Buffer creation
	glGenBuffers(1, &mesh.ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * 6, indices, GL_STATIC_DRAW);

	// Vertex Array (Container for Vertex + Index buffer)
	glGenVertexArrays(1, &mesh.vao);
	if (gl_has_errors())
		return false;

	// Loading shaders
	if (!effect.load_from_file(shader_path("textured.vs.glsl"), shader_path("textured.fs.glsl")))
		return false;

	// Setting initial values, scale is negative to make it face the opposite way
	// 1.0 would be as big as the original texture
	m_scale.x = 1;
	m_scale.y = 1;
	m_position = { 5.f, 537.f };
	m_velocity = { 15.0f, .0f };
	m_acceleration = { 4.f, .0f };
	m_max_speed = { 200.f };
	m_can_move = false;
	m_rotation = 0.f;
	m_in_beyond_intersection = false;
	m_at_intersection = false;

	m_turn_placard = new Placard(m_position, m_rotation);

	return true;
}

// Releases all graphics resources
void Car::destroy()
{
	delete m_turn_placard;

	glDeleteBuffers(1, &mesh.vbo);
	glDeleteBuffers(1, &mesh.ibo);
	glDeleteBuffers(1, &mesh.vao);

	glDeleteShader(effect.vertex);
	glDeleteShader(effect.fragment);
	glDeleteShader(effect.program);
}

// Called on each frame by World::update()
void Car::update(float ms)
{
	m_turn_placard->update(m_position, ms);

	// TODO: Implement Update Car [Theo, Mason]
	if (m_velocity.x > 0 && m_velocity.x < m_max_speed) {
		m_velocity.x += m_acceleration.x;
		m_velocity.y += m_acceleration.y;
	}
	else if (m_velocity.x < 0.f){
		m_velocity.x = 0.f;
	}
	else if (m_velocity.x > m_max_speed) {
		m_velocity.x = m_max_speed;
	}

	//For Y position in North Lane
	if (m_velocity.y > 0 && m_velocity.y < m_max_speed) {
		m_velocity.y += m_acceleration.y;
	}
	else if (m_velocity.y < 0.f) {
		m_velocity.y = 0.f;
	}
	else if (m_velocity.y > m_max_speed) {
		m_velocity.y = m_max_speed;
	}
	//printf("%f", m_velocity.x);
	vec2 m_displacement = { m_velocity.x * (ms / 1000), m_velocity.y * (ms / 1000) };
	move(m_displacement);
}

void Car::draw(const mat3& projection)
{
	if (!m_in_beyond_intersection) {
		m_turn_placard->draw(projection);
	}

	transform_begin();
	transform_scale(m_scale);
	transform_translate(m_position);
	transform_rotate(m_rotation);
	transform_end();

	// Setting shaders
	glUseProgram(effect.program);

	// Enabling alpha channel for textures
	glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);

	// Getting uniform locations for glUniform* calls
	GLint transform_uloc = glGetUniformLocation(effect.program, "transform");
	GLint color_uloc = glGetUniformLocation(effect.program, "fcolor");
	GLint projection_uloc = glGetUniformLocation(effect.program, "projection");

	// Setting vertices and indices
	glBindVertexArray(mesh.vao);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);

	// Input data location as in the vertex buffer
	GLint in_position_loc = glGetAttribLocation(effect.program, "in_position");
	GLint in_texcoord_loc = glGetAttribLocation(effect.program, "in_texcoord");
	glEnableVertexAttribArray(in_position_loc);
	glEnableVertexAttribArray(in_texcoord_loc);
	glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void*)0);
	glVertexAttribPointer(in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void*)sizeof(vec3));

	// Enabling and binding texture to slot 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, car_texture.id);

	// Setting uniform values to the currently bound program
	glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&transform);
	float color[] = { 1.f, 1.f, 1.f };
	glUniform3fv(color_uloc, 1, color);
	glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);

	// Drawing!
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
}
// returns the local bounding coordinates scaled by the current size of the car
vec2 Car::get_bounding_box()const
{
	// fabs is to avoid negative scale due to the facing direction
	return { std::fabs(m_scale.x) * car_texture.width, std::fabs(m_scale.y) * car_texture.height };
}

vec2 Car::get_position()const
{
	return m_position;
}

direction Car::get_desired_direction()const
{
	return m_desired_direction;
}

void Car::move(vec2 off)
{
	if (m_lane == direction::WEST || m_lane == direction::NORTH) {
		m_position.x += off.x;
		m_position.y += off.y;
	}
	if (m_lane == direction::EAST||m_lane==direction::SOUTH) {
		m_position.x -= off.x;
		m_position.y -= off.y;
	}
}

void Car::set_lane(direction dir)
{
	m_lane = dir;
	if (dir == direction::NORTH || dir == direction::SOUTH) {
		m_velocity = { .0f, 15.0f };
		m_acceleration = { .0f, 4.0f };
	}
}

direction Car::get_lane()
{
	return m_lane;
}

void Car::set_rotation(float radians)
{
	m_rotation = radians;
	m_turn_placard->set_rotation(m_rotation);
}
void Car::set_position(vec2 position)
{
	m_position = position;
}
void Car::set_at_intersection(bool boolean)
{
	m_at_intersection = boolean;
}

void Car::slow_down()
{
	// TODO: y coordinates
	if (m_lane == direction::WEST || m_lane == direction::EAST) {
		m_velocity.x = m_max_speed - m_acceleration.x; // gets the update loop running again, probably change to a smarter way within the update conditional
		m_acceleration.x *= -1.f;
		m_acceleration.y = 0.f;
	}
	if (m_lane == direction::NORTH || m_lane == direction::SOUTH) {
		m_velocity.y = m_max_speed - m_acceleration.y; // gets the update loop running again, probably change to a smarter way within the update conditional
		m_acceleration.y *= -1.f;
		m_acceleration.x = 0.f;
	}
}

void Car::speed_up() {
	// TODO: y acceleration/velocity
		if (m_lane == direction::WEST || m_lane == direction::EAST) {
			m_acceleration.x *= -1.f;
			m_velocity.x += m_acceleration.x; // gets the update loop running again, probably change to a smarter way within the update conditional
		}
		if (m_lane == direction::NORTH || m_lane == direction::SOUTH) {
			m_acceleration.y *= -1.f;
			m_velocity.y += m_acceleration.y;
		}
}


vec2 Car::get_acc()
{
	return m_acceleration;
}

vec2 Car::get_vel()
{
	return m_velocity;
}

float Car::get_max_speed()
{
	return m_max_speed;
}
vec2 Car::get_scale()
{
	return m_scale;
}


void Car::signal_to_move()
{
	m_can_move = true;
	m_in_beyond_intersection = true;
	m_at_intersection = false;
}

float Car::compute_stopping_dis(float velocity, float acc)
{
	// vf*vf = vi*vi + 2*acc*d
	float dis = (1000.f*1000.f*velocity*velocity) / (2.f * abs(acc*1000.f));
	return dis + m_wr;
}

bool Car::is_approaching_stop(vec2 lane_pos)
{
	float stop_x = lane_pos.x;
	float stop_y = lane_pos.y;
	float x_margin = abs(m_position.x - stop_x);
	float y_margin = abs(m_position.y - stop_y);
	//printf("%f,%f\n", x_margin, y_margin);
	if (std::max(x_margin, y_margin) <= 130.f && (m_can_move == false))
		return true;
	else
		return false;
}
bool Car::is_at_stop(vec2 lane_pos) {
	float stop_x = lane_pos.x;
	float stop_y = lane_pos.y;
	float x_margin = abs(m_position.x - stop_x);
	float y_margin = abs(m_position.y - stop_y);
	if (std::max(x_margin, y_margin) <= 55.f)
		return true;
	else
		return false;
}
bool Car::is_in_beyond_intersec() {
	return m_in_beyond_intersection;
}

void Car::start_timer(float max_time) {
	m_turn_placard->start_timer(max_time);
}
bool Car::is_at_front() {
	return m_at_intersection;
}

