#pragma once

// stlib
#include <fstream> // stdout, stderr..
#include <cmath>
#include <stdexcept>

// glfw
#define NOMINMAX
#include <gl3w.h>
#include <GLFW/glfw3.h>

// Simple utility macros to avoid mistyping directory name, name has to be a string literal
// audio_path("audio.ogg") -> data/audio/audio.ogg
#ifdef _MSC_VER
  #define data_path "../data"
  #define shader_path(name) "../shaders/" name
#else
  #define data_path "data"
  #define shader_path(name) "shaders/" name
#endif
#define textures_path(name) data_path "/textures/" name
#define audio_path(name) data_path "/audio/" name
#define mesh_path(name) data_path "/meshes/" name

// Not much math is needed and there are already way too many libraries linked (:
// If you want to do some overloads..
struct vec2 { float x, y; };
struct vec3 { float x, y, z; };
struct mat3 { vec3 c0, c1, c2; };
//order of vertex goes bottom left, bottom right, top right, top left (counterclockwise)
//this relative to the original texture, not after rotations
struct rect_bounding_box {vec2 bottom_left, bottom_right, top_right, top_left; };

struct digit_tracker {
	float old_offset, new_offset, flip;
};
struct gt_tracker {
	digit_tracker digit_0;
	digit_tracker digit_1;
	digit_tracker digit_2;
	digit_tracker digit_3;
};


// Utility functions
float dot(vec2 l, vec2 r);
float dot(vec3 l, vec3 r);
mat3  mul(const mat3& l, const mat3& r);
vec2  normalize(vec2 v);
char* get_month_from_index(int month_index);

// OpenGL utilities
// cleans error buffer
void gl_flush_errors();
bool gl_has_errors();

// Single Vertex Buffer element for non-textured meshes (colored.vs.glsl)
struct Vertex
{
	vec3 position;
	vec3 color;
};

// Single Vertex Buffer element for textured sprites (textured.vs.glsl)
struct TexturedVertex
{
	vec3 position;
	vec2 texcoord;
};

// Hack to access tex-elements Vertex Buffer element for textured sprites (textured.vs.glsl)
struct TexturedVertex2
{
	vec3 position;
	vec3 texcoord;
};

// Texture wrapper
struct Texture
{
	Texture();
	~Texture();

	GLuint id;
	int width;
	int height;

	// Loads texture from file specified by path
	bool load_from_file(const char* path);
	bool is_valid()const; // True if texture is valid
};

// A Mesh is a collection of a VertexBuffer and an IndexBuffer. A VAO
// represents a Vertex Array Object and is the container for 1 or more Vertex Buffers and
// an Index Buffer
struct Mesh
{
	GLuint vao;
	GLuint vbo;
	GLuint ibo;
};

// Container for Vertex and Fragment shader, which are then put(linked) together in a
// single program that is then bound to the pipeline.
struct Effect
{
	bool load_from_file(const char* vs_path, const char* fs_path);
	void release();

	GLuint vertex;
	GLuint fragment;
	GLuint program;
};

// Helper container for all the information we need when rendering an object together
// with its transform.
struct Renderable
{
	Mesh mesh;
	Effect effect;
	mat3 transform;

	// projection contains the orthographic projection matrix. As every Renderable::draw()
	// renders itself it needs it to correctly bind it to its shader.
	virtual void draw(const mat3& projection) = 0;

	// gl Immediate mode equivalent, see the Rendering and Transformations section in the
	// specification pdf
	void transform_begin();
	void transform_scale(vec2 scale);
	void transform_rotate(float radians);
	void transform_translate_x(float pos);
	void transform_translate(vec2 pos);
	void transform_end();
};
