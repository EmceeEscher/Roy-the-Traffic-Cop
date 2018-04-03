#include "warning.hpp"

// stlib
#include <vector>
#include <algorithm>


Texture Warning::warning_texture;
Warning::Warning() :
	warning_counter(0)
{

}

Warning::~Warning()
{

}
bool Warning::init(direction dir)
{
	warning_coords[direction::NORTH] = { 1100.f,206.f };
	warning_coords[direction::EAST] = { 1774.f,1086.f };
	warning_coords[direction::SOUTH] = { 904.f, 1688.f };
	warning_coords[direction::WEST] = { 150.f,892.f };
	show = true;
	warning_texture.load_from_file(textures_path("warning.png"));


	// Load shared texture
	if (!warning_texture.is_valid())
	{
		if (!warning_texture.load_from_file(textures_path("warning.png")))
		{
			fprintf(stderr, "Failed to load cop texture!");
			return false;
		}
	}

	// The position (0,0) corresponds to the center of the texture
	float wr = warning_texture.width * 0.5;
	float hr = warning_texture.height * 0.5;

	TexturedVertex vertices[4];
	vertices[0].position = { -wr, +hr, 0.f };
	vertices[0].texcoord = { 0.f, 1.f };//top left
	vertices[1].position = { +wr, +hr, 0.f };
	vertices[1].texcoord = { 1.f, 1.f };//top right
	vertices[2].position = { +wr, -hr, 0.f };
	vertices[2].texcoord = { 1.f, 0.f };//bottom right
	vertices[3].position = { -wr, -hr, 0.f };
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
	m_scale.x = 0.5;
	m_scale.y = 0.5;
	m_dir = dir;
	m_position = warning_coords[dir];
	m_prev_time = 0.f;
	amb_init = false;
	//m_rotation = 0.f;

	return true;
}

void Warning::reset() {
	show = false;
}

void Warning::update(float ms)
{
	m_prev_time += ms;
	if (warning_counter < 9) {
		if (m_prev_time > 500) {
			warning_counter++;
			show = !show;
			m_prev_time = 0.f;
		}
	}
	else {
		amb_init = true;
	}
	//TOOD Initialize amb here? or set flag in order to intialize a ambulance somewhere else.
	
}

void Warning::destroy()
{
	glDeleteBuffers(1, &mesh.vbo);
	glDeleteBuffers(1, &mesh.ibo);
	glDeleteBuffers(1, &mesh.vao);

	glDeleteShader(effect.vertex);
	glDeleteShader(effect.fragment);
	glDeleteShader(effect.program);
}

void Warning::draw(const mat3& projection)
{
	transform_begin();
	transform_scale(m_scale);
	transform_translate(m_position);
	transform_end();

	if (show) {
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
		glBindTexture(GL_TEXTURE_2D, warning_texture.id);

		// Setting uniform values to the currently bound program
		glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&transform);
		float color[] = { 1.f, 1.f, 1.f };
		glUniform3fv(color_uloc, 1, color);
		glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);

		// Drawing!
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
	}
}
void Warning::set_position(vec2 position)
{
	m_position = position;
}
/*
bool Ambulance::init()
{
std::vector<TexturedVertex> vertices;
std::vector<uint16_t> indices;

// Reads the ambulance mesh from a file, which contains a list of vertices, textures, and indices
FILE* mesh_file = fopen(mesh_path("ambulance.mesh"), "r");
if (mesh_file == nullptr)
return false;

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
glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

// Index Buffer creation
glGenBuffers(1, &mesh.ibo);
glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);
glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * indices.size(), indices.data(), GL_STATIC_DRAW);

// Vertex Array (Container for Vertex + Index buffer)
glGenVertexArrays(1, &mesh.vao);
if (gl_has_errors())
return false;

// Loading shaders
if (!effect.load_from_file(shader_path("colored.vs.glsl"), shader_path("colored.fs.glsl")))
return false;

// Setting initial values
m_scale.x = 1;
m_scale.y = 1;
//m_position = { 50.f, 100.f }; //TODO: SET THIS
//m_rotation = 0.f; //TODO: SET THIS

return true;
}
*/
