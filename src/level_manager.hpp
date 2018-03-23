#pragma once

#include "common.hpp"


using std::string;

class LevelManager
{
public:
	// Creates all the associated render resources and default transform
	bool init();

	// Releases all associated resources
	void destroy();

	// Return current game level
	int get_level();

	void update(float elapsed_ms);


private:

};