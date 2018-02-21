#pragma once

#include "common.hpp"
#include "turn_direction.hpp"

#include <math.h>
#include <map>

class Placard : public Renderable
{
  static Texture placard_texture;
public:

  Placard(vec2 parent_position, float parent_rotation);
  ~Placard();

  // Creates all the associated render resources and default transform
  //bool init(vec2 parent_position, float parent_rotation);

  // Releases all associated resources
  void destroy();

  // Update position based on parent position
	// ms represents the number of milliseconds elapsed from the previous update() call
  void update(vec2 parent_position, float ms);

  // Renders
  void draw(const mat3& projection)override;

	// void set_position(vec2 position);
  //
  // // Will start the placard changing color to show remaining time
  // void start_timer(float timer_total);

private:
	vec2 m_position; // Window coordinates
	vec2 m_scale; // 1.f in each dimension. 1.f is as big as the associated texture
	float m_rotation; // in radians
  float m_wr;
  float m_hr;
  float m_timer; //in ms
	size_t m_num_indices; // passed to glDrawElements
  float m_offset_from_parent = 100.f;
  bool m_is_counting_down = false;

  //for the vec2s in both of these maps, x is the left boundary of the sprite, y is the right boundary
  //TODO: maybe shouldn't use a vec2, but C++ doesn't have nice tuples...
  std::map<turn_direction, vec2> m_texture_positions;
  std::map<turn_direction, vec2> m_texture_coords;

  turn_direction m_desired_turn = turn_direction::EMERGENCY;
};
