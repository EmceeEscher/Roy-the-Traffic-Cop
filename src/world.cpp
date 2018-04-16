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
int cnt = 1;

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

World::World() :
	m_points(0)
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
	m_game_music = Mix_LoadMUS(audio_path("music.wav"));
	m_background_music = Mix_LoadMUS(audio_path("start_music.wav"));
	m_roy_whistle = Mix_LoadWAV(audio_path("whistle.wav"));
	Mix_VolumeMusic(MIX_MAX_VOLUME / 2);
	Mix_PlayMusic(m_background_music, -1);

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

	is_game_paused = false;
	show_start_splash = true;
	is_game_over = false; 
	game_level = 1;

	m_background.init();
	m_ai.init();
	m_remove_intersection.init();
	m_game_timer.init();
	m_score_display.init();
	m_lane_manager.init(m_ai);
	m_coin_icon.init();
	m_display_screen.init();
	m_level_manager.init();
	m_high_scores.init();
	m_weather.init();

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
	m_remove_intersection.destroy();
	m_traffic_cop.destroy();
	m_background.destroy();
	m_score_display.destroy();
	m_coin_icon.destroy();
	m_weather.destroy();
	glfwDestroyWindow(m_window);
}

// Update our game world
bool World::update(float elapsed_ms)
{
	m_points = m_lane_manager.points();
	m_req_points_next_level = m_level_manager.get_next_level_point_req();
	game_level = m_level_manager.get_level();
	//is_game_over = m_level_manager.get_game_over();

	if (is_game_over) {
		m_high_scores.check_score_and_insert(m_points, is_game_over);
	}

	m_display_screen.update(is_game_paused, show_start_splash, is_game_over, game_level, elapsed_ms);
	m_level_manager.update(m_points, m_game_timer.get_current_time(), elapsed_ms, m_lane_manager);

	if (!is_game_paused && !show_start_splash && !is_game_over) {
		int w, h;
		glfwGetFramebufferSize(m_window, &w, &h);
		vec2 screen = { (float)w, (float)h };

		m_traffic_cop.update(elapsed_ms);
		m_game_timer.advance_time(elapsed_ms, game_level);
		
		m_lane_manager.update(elapsed_ms, game_level);
		m_remove_intersection.update(elapsed_ms, this->hit_count(), game_level);

		m_score_display.update_score(m_points);
		m_coin_icon.update(elapsed_ms);
		m_weather.update(elapsed_ms, game_level, m_game_timer.get_current_time());
		return true;
	}
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
	title_ss << "Required Points to Advance: " << m_req_points_next_level;
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
	for (auto& car : m_lane_manager.get_cars_in_lane(direction::WEST))
		car.draw(projection_2D);
	for (auto& car : m_lane_manager.get_cars_in_lane(direction::EAST))
		car.draw(projection_2D);
	for (auto& car : m_lane_manager.get_cars_in_lane(direction::NORTH))
		car.draw(projection_2D);
	for (auto& car : m_lane_manager.get_cars_in_lane(direction::SOUTH))
		car.draw(projection_2D);
	for (auto& warning : m_lane_manager.get_warning())
		warning.draw(projection_2D);
	for (auto& ambulance : m_lane_manager.get_ambulance())
		ambulance.draw(projection_2D);
	m_traffic_cop.draw(projection_2D);
	m_remove_intersection.draw(projection_2D);
	m_game_timer.draw(projection_2D);
	m_score_display.draw(projection_2D);
	m_coin_icon.draw(projection_2D);
	m_weather.draw(projection_2D);
	m_display_screen.draw(projection_2D);
	m_high_scores.draw(projection_2D);
	
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
	if (action == GLFW_PRESS && key == GLFW_KEY_G && show_start_splash) { //start game with G
		show_start_splash = !show_start_splash;
		Mix_PlayMusic(m_game_music, -1);
	}
	if (action == GLFW_PRESS && key == GLFW_KEY_E && show_start_splash) { //start endless mode with E
		show_start_splash = !show_start_splash;
		Mix_PlayMusic(m_game_music, -1);
		m_level_manager.set_endless_mode(m_lane_manager);
	}
	if (action == GLFW_PRESS && key == GLFW_KEY_P && !show_start_splash) { //pause anytime with P
		is_game_paused = !is_game_paused;
		is_game_paused ? Mix_PauseMusic() : Mix_ResumeMusic();
	}
	if (!is_game_paused && !show_start_splash) {
		if (action == GLFW_PRESS && key == GLFW_KEY_N) { //debug, triggers game over
			is_game_over = true;
		}
		if (action == GLFW_PRESS && (key == GLFW_KEY_UP || key == GLFW_KEY_DOWN || key == GLFW_KEY_LEFT || key == GLFW_KEY_RIGHT)) {
			Mix_PlayChannel(-1, m_roy_whistle, 0);
		}
		if (action == GLFW_PRESS && key == GLFW_KEY_UP) {
			m_traffic_cop.set_rotation(lanes_rot[0]);
			m_lane_manager.turn_car(direction::NORTH);
		}
		if (action == GLFW_PRESS && key == GLFW_KEY_DOWN) {
			m_traffic_cop.set_rotation(lanes_rot[2]);
			m_lane_manager.turn_car(direction::SOUTH);
		}
		if (action == GLFW_PRESS && key == GLFW_KEY_LEFT)
		{
			m_traffic_cop.set_rotation(lanes_rot[1]);
			m_lane_manager.turn_car(direction::WEST);
		}
		if (action == GLFW_PRESS && key == GLFW_KEY_RIGHT) {
			m_traffic_cop.set_rotation(lanes_rot[3]);
			m_lane_manager.turn_car(direction::EAST);
		}
		if (action == GLFW_PRESS && key == GLFW_KEY_W) {
			m_lane_manager.add_ambulance(direction::NORTH);
			//m_lane_manager.input_create_cars(direction::NORTH);
		}
		if (action == GLFW_PRESS && key == GLFW_KEY_A) {
			m_lane_manager.add_ambulance(direction::WEST);
			//m_lane_manager.input_create_cars(direction::WEST);
		}
		if (action == GLFW_PRESS && key == GLFW_KEY_S) {
			m_lane_manager.add_ambulance(direction::SOUTH);
			//m_lane_manager.input_create_cars(direction::SOUTH);
		}
		if (action == GLFW_PRESS && key == GLFW_KEY_D) {
			m_lane_manager.add_ambulance(direction::EAST);

			//m_lane_manager.input_create_cars(direction::EAST);
		}
		if (action == GLFW_PRESS && key == GLFW_KEY_SPACE) {
			if (m_remove_intersection.show) {
				m_remove_intersection.increment();
			}
			if (m_remove_intersection.m_press == 5) {
				m_lane_manager.clear_intersection();
			}
		}

		// Weather debugging, hit Z to cycle between skies
		if (action == GLFW_PRESS && key == GLFW_KEY_Z) {
			m_weather.SetWeatherTexLocs(fmod(cnt, 7));
			cnt++;
		}
	}
	if (action == GLFW_RELEASE && key == GLFW_KEY_R)
	{
		reset_game();
	}
}

void World::reset_game() {
	int w, h;
	glfwGetWindowSize(m_window, &w, &h);

	m_lane_manager.reset();
	m_game_timer.reset();
	m_level_manager.init(); // only sets primitives, no memory leak
	m_remove_intersection.reset();
	m_display_screen.reset();
	m_weather.reset();

	Mix_PlayMusic(m_background_music, -1);

	is_game_over = false;
	is_game_paused = false;
	show_start_splash = true;
}

void World::on_mouse_move(GLFWwindow* window, double xpos, double ypos)
{
	printf("mouse position: %f,%f\n", xpos, ypos);
}

int World::hit_count() {
	int counter = 0;
	std::deque<Car> &west_cars = m_lane_manager.m_lanes[direction::WEST]->m_cars;
	for (int i = 0; i < west_cars.size(); i++) {
		if (west_cars[i].is_hit()) {
			counter++;
		}
	}
	std::deque<Car> &east_cars = m_lane_manager.m_lanes[direction::EAST]->m_cars;
	for (int i = 0; i < east_cars.size(); i++) {
		if (east_cars[i].is_hit()) {
			counter++;
		}
	}
	std::deque<Car> &north_cars = m_lane_manager.m_lanes[direction::NORTH]->m_cars;
	for (int i = 0; i < north_cars.size(); i++) {
		if (north_cars[i].is_hit()) {
			counter++;
		}
	}
	std::deque<Car> &south_cars = m_lane_manager.m_lanes[direction::SOUTH]->m_cars;
	for (int i = 0; i < south_cars.size(); i++) {
		if (south_cars[i].is_hit()) {
			counter++;
		}
	}
	return counter;
}
