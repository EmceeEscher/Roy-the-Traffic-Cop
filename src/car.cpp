// Header
#include "car.hpp"
#include "lane.hpp"
#include "direction.hpp"

// stlib
#include <vector>
#include <algorithm>

#define PI 3.14159265
#define ACC 4.0f

Texture Car::car_texture;

bool Car::init(bool isVillain)
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
	m_acceleration = { ACC, .0f };
	m_max_speed = { 200.f };
	m_can_move = false;
	m_is_villain = isVillain;
	m_rotation = 0.f;
	m_in_beyond_intersection = false;
	m_turned = false;
	t = 0.f;
	m_at_intersection = false;
	std::srand(std::time(nullptr));

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
	if (m_in_beyond_intersection == false)
	{
		if (m_turned == false) {
			if (m_velocity.x > 0 && m_velocity.x < m_max_speed) {
				m_velocity.x += m_acceleration.x;
				m_velocity.y += m_acceleration.y;
			}
			else if (m_velocity.x < 0.f) {
				m_velocity.x = 0.f;
			}
			else if (m_velocity.x > m_max_speed) {
				m_velocity.x = m_max_speed;
			}

			//For Y position in North Lane
			if (m_velocity.y > 0.f && m_velocity.y < m_max_speed) {
				m_velocity.y += m_acceleration.y;
			}
			else if (m_velocity.y < 0.f) {
				m_velocity.y = 0.f;
			}
			else if (m_velocity.y > m_max_speed) {
				m_velocity.y = m_max_speed;
			}
		}
		else if (m_turned == true) {
			if (m_velocity.x < abs(m_max_speed)) {
				m_velocity.x += m_acceleration.x;
			}
			else if (m_velocity.x < -m_max_speed) {
				m_velocity.x = -m_max_speed;
			}
			else if (m_velocity.x > m_max_speed) {
				m_velocity.x = m_max_speed;
			}

			if (m_velocity.y < abs(m_max_speed)) {
				m_velocity.y += m_acceleration.y;
			}
			else if (m_velocity.x < -m_max_speed) {
				m_velocity.x = -m_max_speed;
			}
			else if (m_velocity.x > m_max_speed) {
				m_velocity.x = m_max_speed;
			}
		}
		//printf("%f", m_velocity.x);
		vec2 m_displacement = { m_velocity.x * (ms / 1000), m_velocity.y * (ms / 1000) };
		move(m_displacement);
	}
	else
	{
		//printf("in else\n");
		if (t >= 0.f && t <= 1.f)
		{
			turn(t);
			t += 0.005f;
			update_rotation_on_turn(t);
			//printf("%f", t);
		}
		else {
			// m_in_beyond_intersection triggers the stop sign again, m_velocity can't go below 0...
			m_in_beyond_intersection = false;
			m_turned = true; 
			if (m_desired_direction == direction::EAST) {
				m_velocity.y = 0.f;
				m_acceleration.x = ACC;
				m_acceleration.y = 0.f;
			}
			else if (m_desired_direction == direction::WEST) {
				m_velocity.y = 0.f;
				m_acceleration.x = -ACC;
				m_acceleration.y = 0.f;
			}
			else if (m_desired_direction == direction::NORTH || m_desired_direction == direction::SOUTH) {
				m_velocity.x = 0.f;
				m_acceleration.x = 0.f;
				m_acceleration.y = -ACC;
			}
		}
	}
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

bool Car::is_villain()const
{
	return m_is_villain;
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
		m_acceleration = { .0f, ACC };
	}
}

void Car::set_desired_direction(direction turn_dir)
{
	m_desired_direction = turn_dir;
	m_turn_placard->change_turn_direction(get_turn_direction());
	m_is_villain = false;
}

turn_direction Car::get_turn_direction()
{
	switch (m_desired_direction) {
	case direction::EAST:
		switch (m_lane) {
		case direction::NORTH:
			return turn_direction::RIGHT;
		case direction::SOUTH:
			return turn_direction::LEFT;
		case direction::WEST:
			return turn_direction::STRAIGHT;
		default:
			throw std::invalid_argument("received invalid direction pairing");
		}
	case direction::NORTH:
		switch (m_lane) {
		case direction::EAST:
			return turn_direction::LEFT;
		case direction::SOUTH:
			return turn_direction::STRAIGHT;
		case direction::WEST:
			return turn_direction::RIGHT;
		default:
			throw std::invalid_argument("received invalid direction pairing");
		}
	case direction::SOUTH:
		switch (m_lane) {
		case direction::EAST:
			return turn_direction::RIGHT;
		case direction::NORTH:
			return turn_direction::STRAIGHT;
		case direction::WEST:
			return turn_direction::LEFT;
		default:
			throw std::invalid_argument("received invalid direction pairing");
		}
	case direction::WEST:
		switch (m_lane) {
		case direction::EAST:
			return turn_direction::STRAIGHT;
		case direction::SOUTH:
			return turn_direction::RIGHT;
		case direction::NORTH:
			return turn_direction::LEFT;
		default:
			throw std::invalid_argument("received invalid direction pairing");
		}
	default:
		throw std::invalid_argument("received invalid direction pairing");
	}
}

void Car::generate_desired_direction()
{
	m_desired_direction = m_lane;
	while (m_desired_direction == m_lane) {
		m_desired_direction = direction(rand() % 4);
	}
	m_turn_placard->change_turn_direction(get_turn_direction());
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

void Car::set_original_rotation(float radians)
{
	m_original_rot = radians;
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

void Car::set_desired_direction(direction dir)
{
	m_desired_direction = dir;
}

void Car::set_turn_start(vec2 pos)
{
	m_turn_start_pos = pos;
}

void Car::turn(float t)
{
	//printf("turning\n");
	vec2 p = { 0.f, 0.f };
	std::vector<vec2> controlPoints;
	if (calculate_turn_dir(m_lane, m_desired_direction) == 'l')
	{
		//printf("turning left\n");
		controlPoints.push_back(m_turn_start_pos);
		controlPoints.push_back(m_turn_pivot);
		controlPoints.push_back(find_end_point(m_turn_start_pos, m_turn_pivot, (0.5f * PI)));
	}
	else if (calculate_turn_dir(m_lane, m_desired_direction) == 'r')
	{
		controlPoints.push_back(m_turn_start_pos);
		controlPoints.push_back(m_turn_pivot);
		controlPoints.push_back(find_end_point(m_turn_start_pos, m_turn_pivot, (-0.5f * PI)));
	}
	else
	{
		controlPoints.push_back(m_turn_start_pos);
		controlPoints.push_back(m_turn_start_pos);
		controlPoints.push_back(find_end_point(m_turn_start_pos, m_turn_pivot, PI));
	}
	int m = controlPoints.size() - 1;
	for (int i = 0; i <= m; i++) {
		// m choose i
		int coef = binomialCoefficient(m, i);
		// Bernstein Polynomial
		float bern = coef * pow(t, i) * pow((1 - t), (m - i));
		vec2 cp = controlPoints[i];
		p.x = p.x + (cp.x * bern);
		p.y = p.y + (cp.y * bern);
	}
	//printf("endpoint.x: %f\n", controlPoints[2].x);
	//printf("endpoint.y: %f\n", controlPoints[2].y);
	set_position(p);
}

char Car::calculate_turn_dir(direction lane_dir, direction desired_dir)
{
	switch (lane_dir)
	{
	case direction::EAST:
		if (desired_dir == direction::NORTH)
		{
			m_turn_pivot = { 550.f, 450.f };
			return 'r';
		}
		else if (desired_dir == direction::SOUTH)
		{
			m_turn_pivot = { 450.f, 450.f };
			return 'l';
		}
		else
		{
			m_turn_pivot = { 445.f, 445.f };
			return 's';
		}
		break;
	case direction::WEST:
		if (desired_dir == direction::NORTH)
		{
			m_turn_pivot = { 540.f, 550.f };
			return 'l';
		}
		else if (desired_dir == direction::SOUTH)
		{
			m_turn_pivot = { 470.f, 550.f };
			return 'r';
		}
		else
		{
			m_turn_pivot = { 537.f, 537.f };
			return 's';
		}
		break;
	case direction::SOUTH:
		if (desired_dir == direction::EAST)
		{
			m_turn_pivot = { 540.f, 550.f };
			return 'r';
		}
		else if (desired_dir == direction::WEST)
		{
			m_turn_pivot = { 540.f, 470.f };
			return 'l';
		}
		else
		{
			m_turn_pivot = { 550.f, 470.f };
			return 's';
		}
		break;
	case direction::NORTH:
		if (desired_dir == direction::WEST)
		{
			m_turn_pivot = { 450.f, 450.f };
			return 'r';
		}
		else if (desired_dir == direction::EAST)
		{
			m_turn_pivot = { 470.f, 550.f };
			return 'l';
		}
		else
		{
			m_turn_pivot = { 450.f, 550.f };
			return 's';
		}
		break;
	default:
		return 's';
		break;
	}
}

// Rotate p1 around p2 by 90 degrees CCW if turning left
// Rotate p1 around p2 by 90 degrees CW if turning right
// Rotate p1 around p2 by 180 degrees CCW/CW if going straight
vec2 Car::find_end_point(vec2 p1, vec2 p2, float angle)
{
	float s = sin(angle);
	float c = cos(angle);
	vec2 p3 = {0.f, 0.f};

	p3.x = c * (p1.x - p2.x) - s * (p1.y - p2.y) + p2.x;
	p3.y = s * (p1.x - p2.x) + c * (p1.y - p2.y) + p2.y;

	return p3;
}

int Car::binomialCoefficient(int n, int k)
{
	int result = 1;
	for (int i = 1; i <= k; ++i)
	{
		result *= n - (k - i);
		result /= i;
	}
	return result;
}

void Car::update_rotation_on_turn(float t)
{
	//float current_rot = m_rotation;
	char turn = calculate_turn_dir(m_lane, m_desired_direction);
	float angle;
	if (turn == 'l')
	{
		angle = -0.5f * PI;
	}
	else if (turn == 'r')
	{
		angle = 0.5 * PI;
	}
	else
		angle = 0.f;
	//printf("%f\n", m_rotation);
	set_rotation(m_original_rot + t * angle);
}

void Car::start_timer(float max_time) {
	m_turn_placard->start_timer(max_time);
}
bool Car::is_at_front() {
	return m_at_intersection;
}

