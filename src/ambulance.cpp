#include "ambulance.hpp"
#include <vector>
#include <algorithm>

#define PI 3.14159265
#define ACC 8.0f


Texture Ambulance::ambulance_texture;

Ambulance::Ambulance()
{
	m_amb_coords[direction::NORTH] = { 550.f,-110.f };
	m_amb_coords[direction::WEST] = { -110.f,440.f };
	m_amb_coords[direction::SOUTH] = { 450.f,1100.f };
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
	std::vector<uint16_t> indices;

	// Reads the ambulance mesh from a file, which contains a list of vertices, textures, and indices
	FILE* mesh_file = fopen(mesh_path("ambulance.mesh"), "r");
	if (mesh_file == nullptr) return false;

	//load texture
	ambulance_texture.load_from_file(textures_path("Ambulance.png"));

	m_wr = ambulance_texture.width * 0.5;
	m_hr = ambulance_texture.height * 0.5;

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
		amb_vertices.push_back(vertex);
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
	glBufferData(GL_ARRAY_BUFFER, sizeof(TexturedVertex) * amb_vertices.size(), amb_vertices.data(), GL_STATIC_DRAW);

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
	m_original_rot = m_amb_rotation[dir];
	m_lane = dir;
	m_has_started_moving = false;
	initialize_pivots();
	t = 0.f;
	phase = 1;

	// Initialization of variables that will be influenced by levels
	m_level = 1;

	return true;
}

void Ambulance::destroy() {
	glDeleteBuffers(1, &mesh.vbo);
	glDeleteBuffers(1, &mesh.ibo);
	glDeleteVertexArrays(1, &mesh.vao);

	glDeleteShader(effect.vertex);
	glDeleteShader(effect.fragment);
	glDeleteShader(effect.program);
}

void Ambulance::set_level(int level) {
	if (level >= 1)
		m_level = level;
	else
		m_level = 1;
	t_scale = t_scale + m_level * 0.1f;
}


void Ambulance::update(float ms, bool init) {

	if (init) {
		if (t >= 0.f && t <= 1.f)
		{
			turn(t);
			t += 0.01f;
			update_rotation_on_turn(t);
		}
		else
		{
			m_original_rot = m_rotation;
			t = 0.f;
			phase++;
		}
	}
}

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
	m_position.x += off.x;
	m_position.y += off.y;
}

void Ambulance::set_rotation(float radians)
{
	m_rotation = radians;
}


void Ambulance::set_position(vec2 position)
{
	m_position = position;
}

rect_bounding_box Ambulance::get_bounding_box() {
	vec2 bottom_left = {
		(m_position.x - m_wr * cos(m_rotation) + m_hr * sin(m_rotation)),
		(m_position.y - m_wr * sin(m_rotation) - m_hr * cos(m_rotation))
	};

	vec2 bottom_right = {
		(m_position.x + m_wr * cos(m_rotation) + m_hr * sin(m_rotation)),
		(m_position.y + m_wr * sin(m_rotation) - m_hr * cos(m_rotation))
	};

	vec2 top_right = {
		(m_position.x + m_wr * cos(m_rotation) - m_hr * sin(m_rotation)),
		(m_position.y + m_wr * sin(m_rotation) + m_hr * cos(m_rotation))
	};

	vec2 top_left = {
		(m_position.x - m_wr * cos(m_rotation) - m_hr * sin(m_rotation)),
		(m_position.y - m_wr * sin(m_rotation) + m_hr * cos(m_rotation))
	};

	rect_bounding_box bounding_box = {bottom_left, bottom_right, top_right, top_left};

	return bounding_box;
}

vec2 Ambulance::get_vertex_pos(int index) {
	vec2 vertex = {
		(m_position.x + amb_vertices[index].position.x * m_scale.x * cos(m_rotation)
									- amb_vertices[index].position.y * m_scale.y * sin(m_rotation)),
		(m_position.y + amb_vertices[index].position.x * m_scale.x * sin(m_rotation)
									+ amb_vertices[index].position.y * m_scale.y * cos(m_rotation))
	};

	return vertex;
}

bool Ambulance::check_collision(vec2 test_vertex) {
	rect_bounding_box bounding_box = this->get_bounding_box();

	bool implicitBottom = check_implicit(bounding_box.bottom_left, bounding_box.bottom_right, test_vertex);
	bool implicitRight = check_implicit(bounding_box.bottom_right, bounding_box.top_right, test_vertex);
	bool implicitTop = check_implicit(bounding_box.top_right, bounding_box.top_left, test_vertex);
	bool implicitLeft = check_implicit(bounding_box.top_left, bounding_box.bottom_left, test_vertex);

	return (implicitBottom && implicitRight && implicitTop && implicitLeft);
}

bool Ambulance::check_implicit(vec2 P1, vec2 P2, vec2 Ptest) {
	float A = P2.y - P1.y;
	float B = P1.x - P2.x;
	float C = P1.y * P2.x - P2.y * P1.x;
	float result = A * Ptest.x + B * Ptest.y + C;
	return result <= 0.f;
}

bool Ambulance::is_moving() {
	return m_has_started_moving;
}

vec2 Ambulance::get_position()const
{
	return m_position;
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

void Ambulance::initialize_pivots()
{
	switch (m_lane)
	{
	case direction::EAST:
		m_start = m_amb_coords[direction::EAST];
		m_pivot1 = { 600.f,545.f };
		m_pivot2 = { 500.f,545.f };
		m_pivot3 = { 500.f,500.f };
		m_pivot4 = { 500.f,440.f };
		m_pivot5 = { 400.f,440.f };
		m_end = m_amb_coords[direction::WEST];
		break;
	case direction::WEST:
		m_start = m_amb_coords[direction::WEST];
		m_pivot1 = { 400.f, 440.f };
		m_pivot2 = { 500.f, 440.f };
		m_pivot3 = { 500.f,500.f };
		m_pivot4 = { 500.f, 545.f };
		m_pivot5 = { 600.f, 545.f };
		m_end = m_amb_coords[direction::EAST];
		break;
	case direction::SOUTH:
		m_start = m_amb_coords[direction::SOUTH];
		m_pivot1 = { 450.f,575.f };
		m_pivot2 = { 450.f,500.f };
		m_pivot3 = { 500.f,500.f };
		m_pivot4 = { 550.f,500.f };
		m_pivot5 = { 550.f,385.f };
		m_end = m_amb_coords[direction::NORTH];
		break;
	case direction::NORTH:
		m_start = m_amb_coords[direction::NORTH];
		m_pivot1 = { 550.f,385.f };
		m_pivot2 = { 550.f,500.f };
		m_pivot3 = { 500.f,500.f };
		m_pivot4 = { 450.f,500.f };
		m_pivot5 = { 450.f,575.f };
		m_end = m_amb_coords[direction::SOUTH];
		break;

	default:
		break;
	}
}

void Ambulance::turn(float t)
{
	//printf("turning\n");
	vec2 p = { 0.f, 0.f };
	std::vector<vec2> controlPoints;
	if (phase == 1)
	{
		controlPoints.push_back(m_start);
		controlPoints.push_back(m_pivot1);
	}
	else if (phase == 2)
	{
		controlPoints.push_back(m_pivot1);
		controlPoints.push_back(m_pivot2);
		controlPoints.push_back(m_pivot3);
	}
	else if (phase == 3)
	{
		controlPoints.push_back(m_pivot3);
		controlPoints.push_back(m_pivot4);
		controlPoints.push_back(m_pivot5);
	}
	else if (phase == 4)
	{
		controlPoints.push_back(m_pivot5);
		controlPoints.push_back(m_end);
	}
	else
	{
		controlPoints.push_back(m_end);
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

	set_position(p);
}


void Ambulance::update_rotation_on_turn(float t)
{
	//float current_rot = m_rotation;
	float angle;
	if (phase == 3)
	{
		angle = -0.5f * PI;
	}
	else if (phase == 2)
	{
		angle = 0.5f * PI;
	}
	else
		angle = 0.f;
	//printf("%f\n", m_rotation);
	set_rotation(m_original_rot + t * angle);
}