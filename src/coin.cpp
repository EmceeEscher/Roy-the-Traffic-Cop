// Header
#include "coin.hpp"

Texture Coin::coin_texture;

bool Coin::init()
{
	// Load shared texture
	if (!coin_texture.is_valid())
	{
		if (!coin_texture.load_from_file(textures_path("CoinSheet.png")))
		{
			fprintf(stderr, "Failed to load coin texture!");
			return false;
		}
	}

	glGenBuffers(1, &mesh.vbo);
	glGenBuffers(1, &mesh.ibo);

	float wr = coin_texture.width * 0.5;
	float hr = coin_texture.height * 0.5;

	float sprite_width = coin_texture.width / 6;
	coin_vertices[0].position = { -wr, +hr, 0.f };
	coin_vertices[1].position = { -wr + sprite_width, +hr, 0.f };
	coin_vertices[2].position = { -wr + sprite_width, -hr, 0.f };
	coin_vertices[3].position = { -wr, -hr, 0.f };

	curr_frame = 0;
	set_vertices(curr_frame);

	// Vertex Array (Container for Vertex + Index buffer)
	glGenVertexArrays(1, &mesh.vao);
	if (gl_has_errors())
		fprintf(stderr, "having problems creating graphics for the coin");

	// Loading shaders
	if (!effect.load_from_file(shader_path("textured.vs.glsl"), shader_path("textured.fs.glsl")))
		return false;

	// Setting initial values, scale is negative to make it face the opposite way
	// 1.0 would be as big as the original texture
	m_scale.x = 1;
	m_scale.y = 1;
	m_position = { 160.f, 80.f };
	m_prev_time = 0.f;
	m_fps = 100.f;

	return true;
}

// Releases all graphics resources
void Coin::destroy()
{
	glDeleteBuffers(1, &mesh.vbo);
	glDeleteBuffers(1, &mesh.ibo);
	glDeleteBuffers(1, &mesh.vao);

	glDeleteShader(effect.vertex);
	glDeleteShader(effect.fragment);
	glDeleteShader(effect.program);
}

void Coin::set_vertices(int coin_frame) {
	// The position (0,0) corresponds to the center of the texture


	float texture_locs[] = { 0.f, 1.f / 6, 2.f / 6, 3.f / 6, 4.f / 6, 5.f / 6, 1.f };
	
	coin_vertices[0].texcoord = { texture_locs[coin_frame], 1.f };//top left
	coin_vertices[1].texcoord = { texture_locs[coin_frame + 1], 1.f };//top right
	coin_vertices[2].texcoord = { texture_locs[coin_frame + 1], 0.f };//bottom right
	coin_vertices[3].texcoord = { texture_locs[coin_frame], 0.f };//bottom left

	// counterclockwise as it's the default opengl front winding direction
	uint16_t indices[] = { 0, 3, 1, 1, 3, 2 };

	// Clearing errors
	gl_flush_errors();

	// Vertex Buffer creation
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(TexturedVertex) * 4, coin_vertices, GL_DYNAMIC_DRAW);

	// Index Buffer creation
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * 6, indices, GL_DYNAMIC_DRAW);
}

void Coin::update(float elapsed_ms) {
	m_prev_time += elapsed_ms;
	if (m_prev_time > m_fps) {
		m_prev_time = 0;
		curr_frame = (curr_frame + 1) % 6;
		set_vertices(curr_frame);
	}
}

void Coin::draw(const mat3& projection)
{
	transform_begin();
	transform_scale(m_scale);
	transform_translate(m_position);
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
	glBindTexture(GL_TEXTURE_2D, coin_texture.id);

	// Setting uniform values to the currently bound program
	glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&transform);
	float color[] = { 1.f, 1.f, 1.f };
	glUniform3fv(color_uloc, 1, color);
	glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);

	// Drawing!
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
}
