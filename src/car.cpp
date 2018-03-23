// Header
#include "car.hpp"
#include "lane.hpp"
#include "direction.hpp"
#include <random>

// stlib
#include <vector>
#include <algorithm>

#define PI 3.14159265
#define ACC 4.0f

Texture Car::car_texture;

TexturedVertex car_vertices[13];

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<> dis(1, 7);
std::uniform_int_distribution<> dis2(0, 3);

uint16_t indices[] = {
	0,1,2,
	3,2,1,
	4,1,0,
	5,6,7,
	7,6,8,
	8,6,9,
	3,1,10,
	1,4,10,
	9,10, 11,
	10, 4, 11,
	12, 10, 5,
	6, 5, 10,
	6, 10, 9,
	3, 10, 12 };

bool Car::init(bool isVillain)
{
	// Load shared texture
	if (!car_texture.is_valid())
	{
		if (!car_texture.load_from_file(textures_path("CarSheet.png")))
		{
			fprintf(stderr, "Failed to load car texture!");
			return false;
		}
	}

	if (isVillain) {
		car_tex_x0 = 0; //villains are red
	}
	else {
		car_tex_x0 = dis(gen); //other cars are a random texture.
	}

	// The position (0,0) corresponds to the center of the texture
	m_wr = car_texture.width * 0.5 / 8.f; //8 cars in sprite sheet
	//m_hr = car_texture.height * 0.5;
	m_hr = 22.00000f; //ignoring mirrors
	float full_car_offset = 0.125 * car_tex_x0;

	//TexturedVertex vertices[13];
	car_vertices[0].position = { -m_wr, 18.00000f,0.f };
	car_vertices[0].texcoord = { 0.000112f + full_car_offset, 0.140812f };

	car_vertices[1].position = { -0.6f*m_wr, 0.000000f ,0.f };
	car_vertices[1].texcoord = { 0.025310f + full_car_offset, 0.499466f };

	car_vertices[2].position = { -m_wr, -18.00000f ,0.f };
	car_vertices[2].texcoord = { 0.000112f + full_car_offset, 0.858120f };

	car_vertices[3].position = { -0.6f*m_wr, -22.00000f ,0.f };
	car_vertices[3].texcoord = { 0.025310f + full_car_offset, 0.942415f};

	car_vertices[4].position = { -0.6f*m_wr, 22.00000f ,0.f };
	car_vertices[4].texcoord = { 0.025310f + full_car_offset, 0.056517f };

	car_vertices[5].position = { 0.76f*m_wr, -22.00000f ,0.f };
	car_vertices[5].texcoord = { 0.109941f + full_car_offset, 0.928707f };

	car_vertices[6].position = { 0.76f*m_wr, 0.000000f ,0.f };
	car_vertices[6].texcoord = { 0.110067f + full_car_offset, 0.499466f };

	car_vertices[7].position = { m_wr, -12.50000f ,0.f };
	car_vertices[7].texcoord = { 0.124801f + full_car_offset, 0.751568f };

	car_vertices[8].position = { m_wr, 12.50000f ,0.f };
	car_vertices[8].texcoord = { 0.124801f + full_car_offset, 0.247364f };

	car_vertices[9].position = { 0.76f*m_wr, 22.00000f,0.f };
	car_vertices[9].texcoord = { 0.109941f + full_car_offset, 0.070225f };

	car_vertices[10].position = { 0.000000f, 0.000000f,0.f };
	car_vertices[10].texcoord = { 0.062549f + full_car_offset, 0.499466f };

	car_vertices[11].position = { 0.000000f, 21.00000f,0.f };
	car_vertices[11].texcoord = { 0.062418f + full_car_offset, 0.080462f };

	car_vertices[12].position = { 0.000000f, -21.00000f,0.f };
	car_vertices[12].texcoord = { 0.062418f + full_car_offset, 0.918470f };

	uint16_t indices[] = {
		0,1,2,
		3,2,1,
		4,1,0,
		5,6,7,
		7,6,8,
		8,6,9,
		3,1,10,
		1,4,10,
		9,10, 11,
		10, 4, 11,
		12, 10, 5,
		6, 5, 10,
		6, 10, 9,
		3, 10, 12 };

	// Clearing errors
	gl_flush_errors();

	// Vertex Buffer creation
	glGenBuffers(1, &mesh.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(TexturedVertex) * 13, car_vertices, GL_STATIC_DRAW);

	// Index Buffer creation
	glGenBuffers(1, &mesh.ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * 42, indices, GL_STATIC_DRAW);

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
	m_position = { -200.f, 537.f };
	m_can_move = false;
	m_is_villain = isVillain;
	m_rotation = 0.f;
	m_in_beyond_intersection = false;
	m_hit = false;
	m_turned = false;
	m_at_intersection = false;
	m_spin_amount = 0.f;
	m_turn_placard = new Placard(m_position, m_rotation);

	m_color[0] = 1.f;
	m_color[1] = 1.f;
	m_color[2] = 1.f;

	// Initialization of variables that will be influenced by levels
	m_level = 1;
	m_velocity = { 15.0f, .0f };
	m_acceleration = { ACC, .0f };
	m_max_speed = 150.f;
	t = 0.f;
	stopping_distance = 97.5f;
	stopping_distance_scale = 0.f;
	t_scale = 1.f;

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


// Used for setting level variables for cars at generation
void Car::set_level(int level) {
	if (level >= 1)
		m_level = level;
	else
		m_level = 1;
	float scale = 1 + (m_level - 1)*5.f / 100.f;
	t_scale = t_scale + m_level * 0.1f;
	m_velocity = { m_velocity.x * scale, m_velocity.y * scale };
	m_max_speed = m_max_speed * scale;
	stopping_distance = stopping_distance * scale;
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
		//printf("%f", m_velocity.x);
		vec2 m_displacement = { m_velocity.x * (ms / 1000), m_velocity.y * (ms / 1000) };
		move(m_displacement);
	}
	else
	{
		if (!m_hit) {

			//printf("in else\n");
			if (t >= 0.f && t <= 1.f)
			{
				turn(t);
				t += 0.01f*t_scale;
				update_rotation_on_turn(t);
				//printf("%f", t);
			}
			else {
				// m_in_beyond_intersection triggers the stop sign again, m_velocity can't go below 0...
				if (t > 1.f) {
					m_turned = true;
					if (m_desired_direction == direction::EAST) {
						m_velocity.y = 0.f;
						m_velocity.x = 0.9f * m_max_speed;
						m_acceleration.x = ACC;
						m_acceleration.y = 0.f;
					}
					else if (m_desired_direction == direction::WEST) {
						m_velocity.y = 0.f;
						m_velocity.x = -0.9f * m_max_speed;
						m_acceleration.x = -ACC;
						m_acceleration.y = 0.f;
					}
					else if (m_desired_direction == direction::NORTH) {
						m_velocity.x = 0.f;
						m_velocity.y = -0.9f * m_max_speed;
						m_acceleration.x = 0.f;
						m_acceleration.y = -ACC;
					}
					else if (m_desired_direction == direction::SOUTH) {
						m_velocity.x = 0.f;
						m_velocity.y = 0.9f * m_max_speed;
						m_acceleration.x = 0.f;
						m_acceleration.y = ACC;
					}
					if (abs(m_velocity.x) < m_max_speed) {
						m_velocity.x += m_acceleration.x;
					}

					if (abs(m_velocity.y) < m_max_speed) {
						m_velocity.y += m_acceleration.y;
					}
					vec2 m_displacement = { m_velocity.x * (ms / 1000), m_velocity.y * (ms / 1000) };
					move(m_displacement);
				}
			}
		}
		//TODO WHAT HAPPENS WHEN THE CAR HAS BEEN IN A COLLISION
		//The car has been in a collision
		else {
			if (sqrt(m_velocity.x * m_velocity.x + m_velocity.y * m_velocity.y > 10.f)) {
				m_velocity.x += m_acceleration.x;
				m_velocity.y += m_acceleration.y;
				m_rotation += m_spin_amount * PI / 18;
			} else {
				m_velocity.x = 0.f;
				m_velocity.y = 0.f;
			}

			vec2 m_displacement = {m_velocity.x * (ms / 1000), m_velocity.y * (ms / 1000) };
			move(m_displacement);
		}
	}
}

void Car::draw(const mat3& projection)
{
	if (!m_in_beyond_intersection && !m_turned) {
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

	// Setting car_vertices and indices
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
	//float color[3] = { 1.f, 1.f, 1.f };
	glUniform3fv(color_uloc, 1, m_color);
	glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);

	// Drawing!
	glDrawElements(GL_TRIANGLES, 42, GL_UNSIGNED_SHORT, nullptr);
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
	if (!m_turned && !m_hit) {
		if (m_lane == direction::WEST || m_lane == direction::NORTH) {
			m_position.x += off.x;
			m_position.y += off.y;
		}
		if (m_lane == direction::EAST||m_lane==direction::SOUTH) {
			m_position.x -= off.x;
			m_position.y -= off.y;
		}
	} else {
		m_position.x += off.x;
		m_position.y += off.y;
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
	switch (m_lane) {
		case direction::EAST:
			switch (m_desired_direction) {
				case direction::NORTH:
					return turn_direction::RIGHT;
				case direction::WEST:
					return turn_direction::STRAIGHT;
				case direction::SOUTH:
					return turn_direction::LEFT;
				default:
				  throw std::invalid_argument("received invalid direction pairing");
			}
			break;
		case direction::SOUTH:
			switch (m_desired_direction) {
				case direction::NORTH:
					return turn_direction::STRAIGHT;
				case direction::WEST:
					return turn_direction::LEFT;
				case direction::EAST:
					return turn_direction::RIGHT;
				default:
					throw std::invalid_argument("received invalid direction pairing");
			}
			break;
		case direction::WEST:
			switch (m_desired_direction) {
				case direction::NORTH:
					return turn_direction::LEFT;
				case direction::EAST:
					return turn_direction::STRAIGHT;
				case direction::SOUTH:
					return turn_direction::RIGHT;
				default:
					throw std::invalid_argument("received invalid direction pairing");
			}
			break;
		case direction::NORTH:
			switch (m_desired_direction) {
				case direction::EAST:
					return turn_direction::LEFT;
				case direction::WEST:
					return turn_direction::RIGHT;
				case direction::SOUTH:
					return turn_direction::STRAIGHT;
				default:
				  throw std::invalid_argument("received invalid direction pairing");
			}
			break;
		default:
			throw std::invalid_argument("received invalid direction pairing");
			break;
	}
}

void Car::generate_desired_direction()
{
	m_desired_direction = m_lane;
	while (m_desired_direction == m_lane) {
		m_desired_direction = direction(dis2(gen));
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
	if (!m_turned) {
		if (m_lane == direction::WEST || m_lane == direction::EAST) {
			m_acceleration.x *= -1.f;
			m_velocity.x += m_acceleration.x; // gets the update loop running again, probably change to a smarter way within the update conditional
		}
		if (m_lane == direction::NORTH || m_lane == direction::SOUTH) {
			m_acceleration.y *= -1.f;
			m_velocity.y += m_acceleration.y;
		}
	} else {
		m_velocity.x += m_acceleration.x;
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

uint16_t Car::get_index(int index) {
	return indices[index];
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
	if (std::max(x_margin, y_margin) <= stopping_distance && (m_can_move == false))
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
			//Change
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
			//Change
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
			m_turn_pivot = { 540.f, 435.f };
			return 'l';
		}
		else
		{
			//Change
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
			//Change
			m_turn_pivot = { 450.f, 540.f };
			return 'l';
		}
		else
		{
			//Change
			m_turn_pivot = { 450.f, 550.f };
			return 's';
		}
		break;
	default:
		printf("hitting default\n");
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

turn_direction Car::get_placard_direction() {
	return m_turn_placard->get_turn_direction();
}

//bounding box inputs go in order: bottom left, bottom right, top right, top left
rect_bounding_box Car::get_bounding_box() {
	// I use negative rotation because I did the math assuming
	// counterclockwise was positive but instead clockwise is positive.
	// Also need to remember that positive y is downward

	vec2 bottom_left = {
		(m_position.x - m_wr * cos(-m_rotation) + m_hr * sin(-m_rotation)),
		(m_position.y + m_wr * sin(-m_rotation) + m_hr * cos(-m_rotation))
	};
	vec2 bottom_right = {
		(m_position.x + m_wr * cos(-m_rotation) + m_hr * sin(-m_rotation)),
		(m_position.y - m_wr * sin(-m_rotation) + m_hr * cos(-m_rotation))
	};
	vec2 top_right = {
		(m_position.x + m_wr * cos(-m_rotation) - m_hr * sin(-m_rotation)),
		(m_position.y - m_wr * sin(-m_rotation) - m_hr * cos(-m_rotation))
	};
	vec2 top_left = {
		(m_position.x - m_wr * cos(-m_rotation) - m_hr * sin(-m_rotation)),
		(m_position.y + m_wr * sin(-m_rotation) - m_hr * cos(-m_rotation))
	};

	rect_bounding_box bounding_box = {bottom_left, bottom_right, top_right, top_left};

	return bounding_box;
}

float Car::get_triangle_area(vec2 p0, vec2 p1, vec2 p2) {
	float tArea = (abs((p1.x - p0.x)*(p2.y - p0.y) - (p2.x - p0.x)*(p1.y - p0.y))) / 2.0f;
	return tArea;
}

vec2 Car::get_vertex_pos(int index) {

	vec2 vertex = {
		(m_position.x + car_vertices[index].position.x * cos(m_rotation) - car_vertices[index].position.y * sin(m_rotation)),
		(m_position.y + car_vertices[index].position.x * sin(m_rotation) + car_vertices[index].position.y * cos(m_rotation))
	};

	return vertex;
}

bool Car::check_collision(vec2 test_vertex) {
	rect_bounding_box bounding_box = this->get_bounding_box();

	bool implicitBottom = check_implicit(bounding_box.bottom_left, bounding_box.bottom_right, test_vertex);
	bool implicitRight = check_implicit(bounding_box.bottom_right, bounding_box.top_right, test_vertex);
	bool implicitTop = check_implicit(bounding_box.top_right, bounding_box.top_left, test_vertex);
	bool implicitLeft = check_implicit(bounding_box.top_left, bounding_box.bottom_left, test_vertex);

	return (implicitBottom && implicitRight && implicitTop && implicitLeft);
}

bool Car::check_mesh_collision(vec2 test_vertex, Triangle t) {

	float tArea = get_triangle_area(t.a, t.b, t.c); // first car's triangle area

	float collisionArea1 = get_triangle_area(t.a, t.b, test_vertex);
	float collisionArea2 = get_triangle_area(t.a, test_vertex, t.c);
	float collisionArea3 = get_triangle_area(test_vertex, t.b, t.c);
	float collisionSum = collisionArea1 + collisionArea2 + collisionArea3;
	if (tArea == 0.f) {
		printf("why Triangle: (%f %f) (%f %f) (%f %f) Vertex: (%f, %f)\n", t.a.x, t.a.y, t.b.x, t.b.y, t.c.x, t.c.y, test_vertex.x, test_vertex.y); 
	}
	//printf("Second car tArea: %f   collisionArea: %f \n", tArea, collisionSum);

	return (abs(collisionSum - tArea) <= 1.0f);
}

bool Car::check_implicit(vec2 P1, vec2 P2, vec2 Ptest) {
	float A = P2.y - P1.y;
	float B = P1.x - P2.x;
	float C = P1.y * P2.x - P2.y * P1.x;
	float result = A * Ptest.x + B * Ptest.y + C;
	return result >= 0.f;
}

void Car::collided(int hit_triangle) {
	if (!m_hit) {
		m_hit = true;
		float speed_scale = m_max_speed;
		float acc_scale = ACC;
		vec2 new_dir = get_collision_direction(hit_triangle);
		m_spin_amount = get_collision_spin(hit_triangle);
		m_velocity = {new_dir.x * speed_scale, new_dir.y * speed_scale};
		m_acceleration = {new_dir.x * -acc_scale, new_dir.y * -acc_scale};
	}
}

float Car::get_collision_spin(int hit_triangle) {
	switch(hit_triangle) {
		case 1:
		case 3:
		case 4:
		case 9:
		case 10:
		case 13:
			return -1.f;
		case 2:
		case 5:
		case 6:
		case 7:
		case 8:
		case 11:
			return 1.f;
		default:
			return 0.f;
	}
}

vec2 Car::get_collision_direction(int hit_triangle) {
	vec2 velocity_dir;
	// 0.866f roughly equals sqrt(3) / 2, can't use sqrt cuz it's a double, not float
	switch(hit_triangle) {
		case 0:
			velocity_dir = {1.f, 0.f}; // to the right
			break;
		case 1:
			velocity_dir = {0.866f, 0.5f}; //bottom right
			break;
		case 2:
			velocity_dir = {0.866f, -0.5f}; //top right
			break;
		case 3:
		case 4:
			velocity_dir = {0.5f, 0.866f}; //slightly bottom right
			break;
		case 5:
		case 6:
			velocity_dir = {0.5f, -0.866f}; //slightly top right
			break;
		case 7:
		case 8:
			velocity_dir = {-0.5f, 0.866f}; //slightly bottom left
			break;
		case 9:
		case 10:
			velocity_dir = {-0.5f, -0.866f}; //slightly top left
			break;
		case 11:
			velocity_dir = {-0.866f, 0.5f}; //bottom left
			break;
		case 12:
			velocity_dir = {-1.f, 0.f}; //to the left
			break;
		case 13:
			velocity_dir = {-0.866f, -0.5f}; //top left
			break;
		default:
			velocity_dir = {0.f, 0.f}; //attacker car, don't move
			break;
	}

	vec2 rotated_vel = {
		(velocity_dir.x * cos(m_rotation) + velocity_dir.y * -sin(m_rotation)),
		(velocity_dir.x * sin(m_rotation) + velocity_dir.y * cos(m_rotation))
	};

	return rotated_vel;
}

void Car::change_color() {
	printf("change color\n");
	m_color[0] = 0.f;
	m_color[1] = 0.f;
	m_color[2] = 0.f;
}

bool Car::is_hit() {
	return m_hit;
}
