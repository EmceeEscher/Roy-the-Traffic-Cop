#pragma once

// internal
#include "common.hpp"
#include "traffic_cop.hpp"
#include "ai.hpp"
#include "background.hpp"
#include "car.hpp"
#include "game_timer.hpp"
#include "high_scores.hpp"
#include "lane_manager.hpp"
#include "lane.hpp"
#include "direction.hpp"
#include "remove_intersection.hpp"
#include "score_display.hpp"
#include "coin.hpp"
#include "display_screen.hpp"
#include "level_manager.hpp"
#include "weather.hpp"
#include "ambulance.hpp"

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
	
	// reset game to starting state (hopefully without memory leaks)
	void reset_game();


private:
	// !!! INPUT CALLBACK FUNCTIONS
	void on_key(GLFWwindow*, int key, int, int action, int mod);
	void on_mouse_move(GLFWwindow* window, double xpos, double ypos);

	int hit_count();

private:
	// Window handle
	GLFWwindow* m_window;

	// Currency earned
	unsigned int m_points;

	// Game Paused Status
	bool is_game_paused;

	// Start Screen Status
	bool show_start_splash;

	// Game over status
	bool is_game_over;
	bool has_updated_high_score;

	// Game level
	int game_level;

	//total points required to get to next level
	int m_req_points_next_level; 

	// Game entities
	GameTimer m_game_timer;
	TrafficCop m_traffic_cop;
	Background m_background;
	AI m_ai;
	RemoveIntersection m_remove_intersection;
	LaneManager m_lane_manager;
	ScoreDisplay m_score_display;
	Coin m_coin_icon;
	DisplayScreen m_display_screen;
	LevelManager m_level_manager;
	Ambulance m_ambulance;
	HighScores m_high_scores;
	Weather m_weather;

	float lanes_rot[4];

	vec2 lanes[4];

	Mix_Music* m_background_music;
	Mix_Music* m_game_music;
	Mix_Chunk* m_roy_whistle;

	// C++ rng
	std::default_random_engine m_rng;
	std::uniform_real_distribution<float> m_dist; // default 0..1
};
