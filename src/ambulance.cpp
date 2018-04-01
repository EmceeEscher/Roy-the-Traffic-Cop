#include "ambulance.hpp"
#include <vector>
#include <algorithm>
Texture Ambulance::ambulance_texture;

bool Ambulance::init()
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
m_position = { 550.f, 550.f }; //TODO: SET THIS
m_rotation = 0.f; //TODO: SET THIS

return true;
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

