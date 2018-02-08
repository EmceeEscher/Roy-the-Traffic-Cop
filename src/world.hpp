#pragma once

// internal
#include "common.hpp"
#include "traffic_cop.hpp"
#include "background.hpp"
#include "car.hpp"
#include "game_timer.hpp"

// stlib
#include <vector>
#include <random>

#define SDL_MAIN_HANDLED
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>

// Container for all our entities and game logic. Individual rendering / update is
// deferred to the relative update() methods
class World
{
public:
	World();
	~World();

	// Creates a window, sets up events and begins the game
	bool init(vec2 screen);

	// Releases all associated resources
	void destroy();

	// Steps the game ahead by ms milliseconds
	bool update(float ms);

	// Renders our scene
	void draw();

	// Should the game be over ?
	bool is_over()const;

	// Returns the scale between window coordinates/game coordinates (for high-density displays)
	float get_world_scale();

private:
	// !!! INPUT CALLBACK FUNCTIONS
	void on_key(GLFWwindow*, int key, int, int action, int mod);
	void on_mouse_move(GLFWwindow* window, double xpos, double ypos);

private:
	// Window handle
	GLFWwindow* m_window;

	// Currency earned
	unsigned int m_points;

	// Game entities
	GameTimer m_game_timer;
	TrafficCop m_traffic_cop;
	Background m_background;
	//Just a single car to show for now.
	Car m_car;
	//std::vector<Car> m_cars;


	float m_world_scale;

	bool m_advanced_features;

	Mix_Music* m_background_music;

	// C++ rng
	std::default_random_engine m_rng;
	std::uniform_real_distribution<float> m_dist; // default 0..1
};
