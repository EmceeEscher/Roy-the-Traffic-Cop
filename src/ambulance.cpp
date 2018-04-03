#include "ambulance.hpp"
#include <vector>
#include <algorithm>

#define PI 3.14159265
#define ACC 4.0f


Texture Ambulance::ambulance_texture;

Ambulance::Ambulance()
{
	m_amb_coords[direction::NORTH] = { 550.f,-110.f };
	m_amb_coords[direction::WEST] = { -110.f,440.f };
	m_amb_coords[direction::SOUTH] = { 449.f,1100.f };
	m_amb_coords[direction::EAST] = { 1170.f,545.f };
	m_amb_rotation[direction::NORTH] = PI / 2;
	m_amb_rotation[direction::WEST] = 0.f;
	m_amb_rotation[direction::SOUTH] = 3.0*PI / 2.0;
	m_amb_rotation[direction::EAST] = PI;
}
Ambulance::~Ambulance()
{

}
bool Ambulance::init(direction dir)
{
std::vector<TexturedVertex> vertices;
std::vector<uint16_t> indices;

// Reads the ambulance mesh from a file, which contains a list of vertices, textures, and indices
FILE* mesh_file = fopen(mesh_path("ambulance.mesh"), "r");
if (mesh_file == nullptr) return false;

//load texture
ambulance_texture.load_from_file(textures_path("Ambulance.png"));

// Reading vertices and texture
size_t num_vertices;
fscanf(mesh_file, "%zu\n", &num_vertices);
for (size_t i = 0; i < num_vertices; ++i)
{
float x, y, z;
float u, v;
fscanf(mesh_file, "%f %f %f %f %f \n", &x, &y, &z, &u, &v);
TexturedVertex vertex;
vertex.position = { x, y, z };
vertex.texcoord = { u, v };
vertices.push_back(vertex);
}

// Reading associated indices
size_t num_indices;
fscanf(mesh_file, "%zu\n", &num_indices);
for (size_t i = 0; i < num_indices; ++i)
{
int idx[3];
fscanf(mesh_file, "%d %d %d\n", idx, idx + 1, idx + 2);
indices.push_back((uint16_t)idx[0]);
indices.push_back((uint16_t)idx[1]);
indices.push_back((uint16_t)idx[2]);
}

// Done reading
fclose(mesh_file);

// Clearing errors
gl_flush_errors();

// Vertex Buffer creation
glGenBuffers(1, &mesh.vbo);
glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
glBufferData(GL_ARRAY_BUFFER, sizeof(TexturedVertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

// Index Buffer creation
glGenBuffers(1, &mesh.ibo);
glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);
glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * indices.size(), indices.data(), GL_STATIC_DRAW);

// Vertex Array (Container for Vertex + Index buffer)
glGenVertexArrays(1, &mesh.vao);
if (gl_has_errors())
return false;

// Loading shaders
if (!effect.load_from_file(shader_path("textured.vs.glsl"), shader_path("textured.fs.glsl")))
return false;

// Setting initial values
m_scale.x = 13;
m_scale.y = 13;
m_position = m_amb_coords[dir];
m_rotation = m_amb_rotation[dir]; //TODO: SET THIS

 // The position (0,0) corresponds to the center of the texture
m_wr = ambulance_texture.width * 0.5 / 8.f; //8 cars in sprite sheet
//m_hr = car_texture.height * 0.5;
m_hr = 22.00000f; //ignoring mirrors
m_lane = dir;
								  
// Initialization of variables that will be influenced by levels
m_level = 1;
if (dir == direction::NORTH || dir == direction::SOUTH) {
	m_velocity = { .0f, 15.0f };
	m_acceleration = { .0f, ACC };
}
else {
	m_velocity = { 15.0f, .0f };
	m_acceleration = { ACC, .0f };
}
m_max_speed = 180.f;
t = 0.f;
stopping_distance = 117.f;
stopping_distance_scale = 0.f;
t_scale = 1.f;
return true;
}
void Ambulance::set_level(int level) {
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


void Ambulance::update(float ms, bool init) {
//TODO Temporary comment for testing purposes
//	if (init) {
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
		//printf("%f", m_velocity.x);
		vec2 m_displacement = { m_velocity.x * (ms / 1000), m_velocity.y * (ms / 1000) };
		move(m_displacement);
//	}

	//TODO
	//if (t >= 0.f && t <= 1.f)
	//{
	//	turn(t);
	//	t += 0.01f*t_scale;
	//	update_rotation_on_turn(t);
	//}
}

//TODO: Some update function

void Ambulance::draw(const mat3& projection)
{

	transform_begin();
	transform_translate(m_position);
	transform_rotate(m_rotation);
	transform_scale(m_scale);
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
	glBindTexture(GL_TEXTURE_2D, ambulance_texture.id);

	// Setting uniform values to the currently bound program
	glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&transform);
	float color[3] = { 1.f, 1.f, 1.f };
	glUniform3fv(color_uloc, 1, color);
	glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);

	// Drawing!
	glDrawElements(GL_TRIANGLES, 42, GL_UNSIGNED_SHORT, nullptr);
}

void Ambulance::move(vec2 off)
{
	if (m_lane == direction::WEST || m_lane == direction::NORTH) {
		m_position.x += off.x;
		m_position.y += off.y;
	}
	if (m_lane == direction::EAST || m_lane == direction::SOUTH) {
		m_position.x -= off.x;
		m_position.y -= off.y;
	}
	
}
void Ambulance::set_rotation(float radians)
{
	m_rotation = radians;
	//m_turn_placard->set_rotation(m_rotation);
}

void Ambulance::update_rotation_on_turn(float t)
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

void Ambulance::turn(float t)
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
void Ambulance::set_position(vec2 position)
{
	m_position = position;
}

vec2 Ambulance::find_end_point(vec2 p1, vec2 p2, float angle)
{
	float s = sin(angle);
	float c = cos(angle);
	vec2 p3 = { 0.f, 0.f };

	p3.x = c * (p1.x - p2.x) - s * (p1.y - p2.y) + p2.x;
	p3.y = s * (p1.x - p2.x) + c * (p1.y - p2.y) + p2.y;

	return p3;
}
char Ambulance::calculate_turn_dir(direction lane_dir, direction desired_dir)
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
			m_turn_pivot = { 530.f, 445.f };
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
			m_turn_pivot = { 470.f, 537.f };
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
			m_turn_pivot = { 550.f, 530.f };
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
			m_turn_pivot = { 450.f, 470.f };
			return 's';
		}
		break;
	default:
		printf("hitting default\n");
		return 's';
		break;
	}
}
int Ambulance::binomialCoefficient(int n, int k)
{
	int result = 1;
	for (int i = 1; i <= k; ++i)
	{
		result *= n - (k - i);
		result /= i;
	}
	return result;
}

//bounding box inputs go in order: bottom left, bottom right, top right, top left
rect_bounding_box Ambulance::get_bounding_box() {
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

	rect_bounding_box bounding_box = { bottom_left, bottom_right, top_right, top_left };

	return bounding_box;
}
