#include "placard.hpp"
#define PI 3.14159265

Texture Placard::placard_texture;

Placard::Placard(vec2 parent_position, float parent_rotation){
  if (!placard_texture.is_valid()) {
    if (!placard_texture.load_from_file(textures_path("SignSheetv2.png"))) {
      fprintf(stderr, "Failed to load sign texture!\n");
      //return false;
    }
  }

  // The position (0,0) corresponds to the center of the texture
  m_wr = placard_texture.width * 0.5;
  m_hr = placard_texture.height * 0.5;

  m_sprite_width = placard_texture.width / 4;

  //initialize the map telling which section of the spritesheet to show
  m_texture_coords[turn_direction::STRAIGHT] = {0.f, 0.25f};
  m_texture_coords[turn_direction::RIGHT] = {0.25f, 0.5f};
  m_texture_coords[turn_direction::LEFT] = {0.5f, 0.75f};
  m_texture_coords[turn_direction::EMERGENCY] = {0.75f, 1.f};


  //for setting the array below: x refers to the left boundary of the sprite,
  //y refers to the right boundary (NOT vertical positioning)
  //sprites are laid out in a single row in the spritesheet, so they all use the full height
	TexturedVertex vertices[4];
	vertices[0].position = { -m_wr, +m_hr, 0.f };
	vertices[0].texcoord = { m_texture_coords[m_desired_turn].x, 1.f };//top left
	vertices[1].position = { -m_wr + m_sprite_width, +m_hr, 0.f };
	vertices[1].texcoord = { m_texture_coords[m_desired_turn].y, 1.f };//top right
	vertices[2].position = { -m_wr + m_sprite_width, -m_hr, 0.f };
	vertices[2].texcoord = { m_texture_coords[m_desired_turn].y, 0.f };//bottom right
	vertices[3].position = { -m_wr, -m_hr, 0.f };
	vertices[3].texcoord = { m_texture_coords[m_desired_turn].x, 0.f };//bottom left

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
    fprintf(stderr, "Failed to initialize placard graphics!\n");
		//return false;

	// Loading shaders
	if (!effect.load_from_file(shader_path("textured.vs.glsl"), shader_path("textured.fs.glsl")))
    fprintf(stderr, "Failed to load shaders for signs!\n");
		//return false;

  m_scale = {1.f, 1.f};
  m_rotation = parent_rotation + PI / 2;
  m_position = parent_position;
  m_position.x = m_position.x + sin(m_rotation) * m_offset_from_parent;
  m_position.y = m_position.y + cos(m_rotation) * m_offset_from_parent;

  // fprintf(stderr, "Successfully created placard!\n");
}

// Releases all graphics resources
Placard::~Placard()
{
	glDeleteBuffers(1, &mesh.vbo);
	glDeleteBuffers(1, &mesh.ibo);
	glDeleteBuffers(1, &mesh.vao);

	glDeleteShader(effect.vertex);
	glDeleteShader(effect.fragment);
	glDeleteShader(effect.program);
}

void Placard::update(vec2 parent_position, float ms) {
  if (m_is_counting_down) {
    m_curr_time -= ms;
    m_flash_time -= ms;
  }
  m_position = parent_position;
  m_position.x = m_position.x + cos(m_rotation) * m_offset_from_parent;
  m_position.y = m_position.y + sin(m_rotation) * m_offset_from_parent;
}

void Placard::draw(const mat3& projection)
{
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
	glBindTexture(GL_TEXTURE_2D, placard_texture.id);

	// Setting uniform values to the currently bound program
	glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&transform);

  // This is how we change the color of the sign
  float color[] = {m_red, m_blue, m_green}; // default color
  if (m_is_counting_down) {
    //if timer is counting down interpolate between green and red depending on how much time left
    if (m_curr_time > m_max_time / 4) {
      float interpolation_value = (m_max_time - m_curr_time) / m_max_time;

      color[0] = interpolation_value; //RED (should be 1 when time is up)
      color[1] = 1.f - interpolation_value; //GREEN (should be 0 when time is up)
      color[2] = 0.f; //BLUE
    } else {
      // flash back and forth between red and black
      if (m_has_flashed) {
        color[0] = 0.f;
      } else {
        color[0] = 1.f;
      }
      if (m_flash_time < 0) {
        m_has_flashed = !m_has_flashed;
        m_flash_time = m_flash_length;
      }

      color[1] = 0.f;
      color[2] = 0.f;
    }
  }

	glUniform3fv(color_uloc, 1, color);
	glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);

	// Drawing!
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
}

void Placard::start_timer(float max_time) {
  m_is_counting_down = true;
  m_max_time = max_time;
  m_curr_time = max_time;
  m_flash_time = m_flash_length;
}

void Placard::set_rotation(float parent_rotation) {
  m_rotation = parent_rotation + PI / 2;

  m_position.x = m_position.x + cos(m_rotation) * m_offset_from_parent;
  m_position.y = m_position.y + sin(m_rotation) * m_offset_from_parent;
}

void Placard::change_turn_direction(turn_direction new_direction) {
  m_desired_turn = new_direction;

  TexturedVertex vertices[4];
	vertices[0].position = { -m_wr, +m_hr, 0.f };
	vertices[0].texcoord = { m_texture_coords[m_desired_turn].x, 1.f };//top left
	vertices[1].position = { -m_wr + m_sprite_width, +m_hr, 0.f };
	vertices[1].texcoord = { m_texture_coords[m_desired_turn].y, 1.f };//top right
	vertices[2].position = { -m_wr + m_sprite_width, -m_hr, 0.f };
	vertices[2].texcoord = { m_texture_coords[m_desired_turn].y, 0.f };//bottom right
	vertices[3].position = { -m_wr, -m_hr, 0.f };
	vertices[3].texcoord = { m_texture_coords[m_desired_turn].x, 0.f };//bottom left

  // counterclockwise as it's the default opengl front winding direction
	uint16_t indices[] = { 0, 3, 1, 1, 3, 2 };

  glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(TexturedVertex) * 4, vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * 6, indices, GL_STATIC_DRAW);
}

void Placard::change_color(float red, float green, float blue){
  m_red = red;
  m_green = green;
  m_blue = blue;
}
