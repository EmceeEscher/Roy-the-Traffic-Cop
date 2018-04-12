// Header
#include "display_screen.hpp"


// stlib
#include <vector>
#include <algorithm>
#include <ft2build.h>
#include FT_FREETYPE_H

FT_Library library;
FT_Face face;
bool error;

Texture DisplayScreen::splash_screens;

bool DisplayScreen::init()
{
	renderText();
	// Load shared texture
	splash_screens.load_from_file(textures_path("SplashScreens.png"));

	// The position (0,0) corresponds to the center of the texture
	float wr = splash_screens.width * 0.5;
	float hr = splash_screens.height * 0.5;
	float indiv_splash_width = 500.f;
	float splash_uv = splash_screens.width / 500.f;

	vertices[0].position = { -indiv_splash_width / 2, +hr, 0.f };
	vertices[1].position = { +indiv_splash_width / 2, +hr, 0.f };
	vertices[2].position = { +indiv_splash_width / 2, -hr, 0.f };
	vertices[3].position = { -indiv_splash_width / 2, -hr, 0.f };

	glGenBuffers(1, &mesh.vbo);
	glGenBuffers(1, &mesh.ibo);

	// Vertex Array (Container for Vertex + Index buffer)
	glGenVertexArrays(1, &mesh.vao);
	if (gl_has_errors())
		return false;

	// Loading shaders
	if (!effect.load_from_file(shader_path("textured.vs.glsl"), shader_path("textured.fs.glsl")))
		return false;

	// Setting initial values, scale is negative to make it face the opposite way
	// 1.0 would be as big as the original texture
	m_scale.x = 1.0;
	m_scale.y = 1.0;
	m_position.x = 500;
	m_position.y = 500;
	draw_splash = true;
	display_duration = 1000.f;
	start_countdown = false;
	prev_level = 0;

	return true;
}

// Releases all graphics resources
void DisplayScreen::destroy()
{
	glDeleteBuffers(1, &mesh.vbo);
	glDeleteBuffers(1, &mesh.ibo);
	glDeleteBuffers(1, &mesh.vao);

	glDeleteShader(effect.vertex);
	glDeleteShader(effect.fragment);
	glDeleteShader(effect.program);
}

void DisplayScreen::update(bool paused, bool show_start, bool game_over, int level, float ms) {
	if (paused) {
		draw_splash = true;
		SetSplashTexLocs(12);
	}
	else if (!paused && show_start && !game_over) {
		draw_splash = true;
		SetSplashTexLocs(0);
	}
	else if (!paused && !show_start && game_over) {
		draw_splash = true;
		SetSplashTexLocs(13);
	}
	else if (prev_level != level){
		prev_level = level;
		draw_splash = true;
		start_countdown = true;
		SetSplashTexLocs(level);
	}
	else if (!paused && !start_countdown) {
		draw_splash = false;
	}

	if (draw_splash && start_countdown) {
		display_duration -= ms;
		if (display_duration <= 0 && draw_splash) {
			draw_splash = false;
			start_countdown = false;
			display_duration = 1000.f;
		}
	}


}

void DisplayScreen::SetSplashTexLocs(int splash_screen) {
	// splash_screen: 0 - start, 1-10 - levels, 11 - survive, 12 - pause, 13 - game over

	float texture_locs[] = { 0.f,500.f/7000.f,1000.f / 7000.f,1500.f / 7000.f,2000.f / 7000.f,2500.f / 7000.f,3000.f / 7000.f,
		3500.f / 7000.f,4000.f / 7000.f,4500.f / 7000.f,5000.f / 7000.f,5500.f / 7000.f,6000.f / 7000.f,6500.f / 7000.f,1.f };

	vertices[0].texcoord = { texture_locs[splash_screen], 1.f };//top left
	vertices[1].texcoord = { texture_locs[splash_screen + 1], 1.f };//top right
	vertices[2].texcoord = { texture_locs[splash_screen + 1], 0.f };//bottom right
	vertices[3].texcoord = { texture_locs[splash_screen], 0.f };//bottom left

	// counterclockwise as it's the default opengl front winding direction
	uint16_t indices[] = { 0, 3, 1, 1, 3, 2 };

	// Clearing errors
	gl_flush_errors();

	// Vertex Buffer creation
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(TexturedVertex) * 4, vertices, GL_STATIC_DRAW);

	// Index Buffer creation
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * 6, indices, GL_STATIC_DRAW);
}

void DisplayScreen::draw(const mat3& projection)
{
	if (draw_splash) {
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
		glBindTexture(GL_TEXTURE_2D, splash_screens.id);

		// Setting uniform values to the currently bound program
		glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&transform);
		float color[] = { 1.f, 1.f, 1.f };
		glUniform3fv(color_uloc, 1, color);
		glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);

		// Drawing!
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
	}
}

void DisplayScreen::textInit() {
	error = FT_Init_FreeType(&library);
	if (error) {
		printf("FT_Init_FreeType error");
	}
	error = FT_New_Face(library,
		"../data/fonts/arial.ttf",
		0,
		&face);
	if (error == FT_Err_Unknown_File_Format)
	{
		printf("the font file could be opened and read, but it appears that its font format is unsupported");
	}
	else if (error)
	{
		printf("another error code means that the font file could not be opened or read, or that it is broken");
	}
	error = FT_Set_Char_Size(
		face,
		0,
		16 * 64,
		1000,
		1000
	);
	if (error) {
		printf("set_char_size error");
	}
	error = FT_Set_Pixel_Sizes(
		face,   /* handle to face object */
		0,      /* pixel_width           */
		16);   /* pixel_height          */
	if (error) {
		printf("set_pixel_sizes");
	}
}

void DisplayScreen::draw_bitmap(FT_Bitmap*  bitmap, FT_Int x, FT_Int y)
{
	FT_Int  i, j, p, q;
	FT_Int  x_max = x + bitmap->width;
	FT_Int  y_max = y + bitmap->rows;


	/* for simplicity, we assume that `bitmap->pixel_mode' */
	/* is `FT_PIXEL_MODE_GRAY' (i.e., not a bitmap font)   */

	for (i = x, p = 0; i < x_max; i++, p++)
	{
		for (j = y, q = 0; j < y_max; j++, q++)
		{
			if (i < 0 || j < 0 ||
				i >= WIDTH || j >= HEIGHT)
				continue;

			image[j][i] |= bitmap->buffer[q * bitmap->width + p];
		}
	}
}

void DisplayScreen::renderText() {
	textInit();

	FT_GlyphSlot slot = face->glyph;
	int pen_x, pen_y, n;

	pen_x = 300;
	pen_y = 200;

	char* text = "theosintent";

	for (n = 0; n < strlen(text); n++) {
		FT_UInt glyph_index;
		glyph_index = FT_Get_Char_Index(face, text[n]);
		error = FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT);
		if (error) {
			printf("Glyph load error. continue");
		}
		error = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
		if (error) {
			printf("render_glyph error. continue");
		}

		my_draw_bitmap(&slot->bitmap,
			pen_x + slot->bitmap_left,
			pen_y - slot->bitmap_top);

		pen_x += slot->advance.x >> 6;
		pen_y += slot->advance.y >> 6;
	}
}