// Header
#include "world.hpp"
#include "direction.hpp"

// stlib
#include <string.h>
#include <cassert>
#include <sstream>
#include <math.h>

#define PI 3.14159265
#define OFF_SCREEN 1000

// Same as static in c, local to compilation unit
namespace
{
	namespace
	{
		void glfw_err_cb(int error, const char* desc)
		{
			fprintf(stderr, "%d: %s", error, desc);
		}
	}
}

World::World()
{
	// Seeding rng with random device
	m_rng = std::default_random_engine(std::random_device()());
}

World::~World()
{

}

// World initialization
bool World::init(vec2 screen)
{
	//-------------------------------------------------------------------------
	// GLFW / OGL Initialization
	// Core Opengl 3.
	glfwSetErrorCallback(glfw_err_cb);
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW");
		return false;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, 1);
#if __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	glfwWindowHint(GLFW_RESIZABLE, 0);
	m_window = glfwCreateWindow((int)screen.x, (int)screen.y, "Roy the Traffic Cop", nullptr, nullptr);
	if (m_window == nullptr)
		return false;

	glfwMakeContextCurrent(m_window);
	glfwSwapInterval(1); // vsync

	// Load OpenGL function pointers
	gl3w_init();

	// Setting callbacks to member functions (that's why the redirect is needed)
	// Input is handled using GLFW, for more info see
	// http://www.glfw.org/docs/latest/input_guide.html
	glfwSetWindowUserPointer(m_window, this);
	auto key_redirect = [](GLFWwindow* wnd, int _0, int _1, int _2, int _3) { ((World*)glfwGetWindowUserPointer(wnd))->on_key(wnd, _0, _1, _2, _3); };
	auto cursor_pos_redirect = [](GLFWwindow* wnd, double _0, double _1) { ((World*)glfwGetWindowUserPointer(wnd))->on_mouse_move(wnd, _0, _1); };
	glfwSetKeyCallback(m_window, key_redirect);
	glfwSetCursorPosCallback(m_window, cursor_pos_redirect);

	//-------------------------------------------------------------------------
	// Loading music and sounds
	if (SDL_Init(SDL_INIT_AUDIO) < 0)
	{
		fprintf(stderr, "Failed to initialize SDL Audio");
		return false;
	}

	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1)
	{
		fprintf(stderr, "Failed to open audio device");
		return false;
	}

	m_background_music = Mix_LoadMUS(audio_path("music.wav"));
	m_roy_whistle = Mix_LoadWAV(audio_path("whistle.wav"));

	if (m_background_music == nullptr || m_roy_whistle == nullptr)
	{
		fprintf(stderr, "Failed to load sounds, make sure the data directory is present");
		return false;
	}

	// Playing background music undefinitely
	Mix_PlayMusic(m_background_music, -1);

	fprintf(stderr, "Loaded music");

	int fb_w, fb_h;
			glfwGetFramebufferSize(m_window, &fb_w, &fb_h);

	// Rotation values for each lane
	lanes_rot[0] = PI;			// North
	lanes_rot[1] = PI/2.0;		// West
	lanes_rot[2] = 0;			// South
	lanes_rot[3] = 3.0*PI/2.0;	// East

	// Hard coded stop sign positions
	lanes[0] = { 450.f,400.f };
	lanes[1] = { 400.f,540.f };
	lanes[2] = { 550.f,600.f };
	lanes[3] = { 600.f,450.f };

	m_advanced_features = false;

	m_background.init();
	m_lane_manager.init();
	//TODO: remove the following two lines. Car initialization should be handled by lanes, not world
	m_car.init();
	m_car.set_lane(direction::WEST);
	return m_traffic_cop.init();
}

// Releases all the associated resources
void World::destroy()
{
	if (m_background_music != nullptr)
		Mix_FreeMusic(m_background_music);
	if (m_roy_whistle != nullptr)
		Mix_FreeChunk(m_roy_whistle);

	Mix_CloseAudio();

	m_traffic_cop.destroy();
	m_background.destroy();
	m_car.destroy();
	glfwDestroyWindow(m_window);
}

// Update our game world
bool World::update(float elapsed_ms)
{
	int w, h;
    glfwGetFramebufferSize(m_window, &w, &h);
	vec2 screen = { (float)w, (float)h };

	// TODO: Maybe have to update traffic cop here? OR potentially we just have to set the rotation.
	m_lane_manager.update(elapsed_ms);

	//TODO: make this work for other cars.
	// With init_vel=15.f, acc=3.f, call slow down 160.f away from target
	if ( m_car.is_approaching_stop(lanes[1]) && m_car.get_acc().x > 0.f)
	{
		m_car.slow_down();
	}
	m_car.update(elapsed_ms);
	if (m_car.get_position().x > OFF_SCREEN) {
		// TODO: why does this make the car huge?
		//m_car.destroy();
	}
	return true;
}

// Render our game world
void World::draw()
{
	// Clearing error buffer
	gl_flush_errors();

	// Getting size of window
	int w, h;
        glfwGetFramebufferSize(m_window, &w, &h);


	// Updating window title with points
	std::stringstream title_ss;
	title_ss << "Points: " << m_points;
	glfwSetWindowTitle(m_window, title_ss.str().c_str());

	// Clearing backbuffer
	glViewport(0, 0, w, h);
	glDepthRange(0.00001, 10);
	const float clear_color[3] = { 0.f, 0.f, 0.f };
	glClearColor(clear_color[0], clear_color[1], clear_color[2], 1.0);
	glClearDepth(1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Fake projection matrix, scales with respect to window coordinates
	// PS: 1.f / w in [1][1] is correct.. do you know why ? (:
	float left = 0.f;// *-0.5;
	float top = 0.f;// (float)h * -0.5;
	float right = (float)w;// *0.5;
	float bottom = (float)h;// *0.5;

	float sx = 2.f / (right - left);
	float sy = 2.f / (top - bottom);
	float tx = -(right + left) / (right - left);
	float ty = -(top + bottom) / (top - bottom);
	mat3 projection_2D{ { sx, 0.f, 0.f },{ 0.f, sy, 0.f },{ tx, ty, 1.f } };

	m_background.draw(projection_2D);
	m_car.draw(projection_2D);
	m_traffic_cop.draw(projection_2D);



	// Presenting
	glfwSwapBuffers(m_window);
}

// Should the game be over ?
bool World::is_over()const
{
	return glfwWindowShouldClose(m_window);
}

// On key callback
void World::on_key(GLFWwindow*, int key, int, int action, int mod)
{
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// HANDLE KEY PRESSES HERE
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	if (action == GLFW_PRESS && (key == GLFW_KEY_UP || key == GLFW_KEY_DOWN || key == GLFW_KEY_LEFT|| key == GLFW_KEY_RIGHT)) {
		Mix_PlayChannel(-1, m_roy_whistle, 0);
	}
	if (action == GLFW_PRESS && key == GLFW_KEY_UP)
		m_traffic_cop.set_rotation(lanes_rot[0]);
	if (action == GLFW_PRESS && key == GLFW_KEY_DOWN)
		m_traffic_cop.set_rotation(lanes_rot[2]);
	if (action == GLFW_PRESS && key == GLFW_KEY_LEFT)
	{
		m_traffic_cop.set_rotation(lanes_rot[1]);
		if(m_car.get_vel().x <= 0.f) {
			m_car.signal_to_move(); // signal car to move

			// I removed the condition functions, we will later use a queue to do this
			m_car.speed_up();
		}
	}
	if (action == GLFW_PRESS && key == GLFW_KEY_RIGHT)
		m_traffic_cop.set_rotation(lanes_rot[3]);
}

void World::on_mouse_move(GLFWwindow* window, double xpos, double ypos)
{
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// HANDLE MOUSE CONTROL HERE (if we end up using it)
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
}
