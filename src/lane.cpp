// Header
#include "lane.hpp"

#define PI 3.14159265

Lane::Lane(direction dir, float villainSpawnProbability)
{
	m_dir = dir;
	m_time_remaining = MaxTimePerCar;
	m_villain_spawn_probability = villainSpawnProbability;
	std::srand(std::time(nullptr));
	m_lane_coords[direction::NORTH] = { 450.f,398.f };
	m_lane_coords[direction::WEST] = { 400.f,540.f };
	m_lane_coords[direction::SOUTH] = { 550.f,590.f };
	m_lane_coords[direction::EAST] = { 610.f,450.f };

	SDL_Init(SDL_INIT_AUDIO);
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
	m_car_horn = Mix_LoadWAV(audio_path("carHorn1.wav"));
	m_car_rev = Mix_LoadWAV(audio_path("carRev.wav"));

}

Lane::~Lane()
{
	m_cars.clear();
}

int Lane::get_lane_num()const
{
	return m_lane_num;
}

float Lane::get_time_remaining() const
{
    return m_time_remaining;
}

void Lane::set_time_remaining(float time_remaining) {
	m_time_remaining = time_remaining;
}

void Lane::set_stop_sign(vec2 loc)
{
	float x = loc.x;
	float y = loc.y;
	m_stop_sign_loc.x = x;
	m_stop_sign_loc.y = y;
}

vec2 Lane::get_stop_sign()const
{
	return m_stop_sign_loc;
}

std::deque<Car> Lane::get_cars() const
{
    return m_cars;
}

bool Lane::update(float ms)
{
	m_time_remaining -= ms;
	if (m_time_remaining <= 2500) {
		Mix_VolumeChunk(m_car_horn, MIX_MAX_VOLUME / 50);
		Mix_PlayChannel(-1, m_car_horn, 0);
	}
	return true;
}

void Lane::add_car(carType type)
{
	//timer will still run even if there are no cars in line, so need to reset it
	//when adding to an empty lane
	if (this->is_lane_empty()) {
		m_time_remaining = MaxTimePerCar;
	}

	if (this->is_lane_full()) {
		// change following code based on carType once we have more than one
		fprintf(stderr, "lane is full");
	}
	else{
		Car new_car;
		bool new_villain = (rand() / (RAND_MAX + 1.)) <= m_villain_spawn_probability && !is_lane_empty();

		if(new_car.init(new_villain)){
			if (m_dir == direction::NORTH) {
				new_car.set_position({ 450.f,-200.f });
				new_car.set_rotation(PI / 2);
				new_car.set_original_rotation(PI / 2);
				new_car.set_lane(direction::NORTH);
			}
			else if (m_dir == direction::WEST) {
				new_car.set_position({ -200.f,537.f });
				new_car.set_lane(direction::WEST);
				new_car.set_rotation(0);
				new_car.set_original_rotation(0);
			}
			else if (m_dir == direction::EAST) {
				new_car.set_position({ 1200.f,445.f });
				new_car.set_rotation(PI);
				new_car.set_original_rotation(PI);
				new_car.set_lane(direction::EAST);
			}
			else if (m_dir == direction::SOUTH) {
				new_car.set_position({ 550,1200.f });
				new_car.set_rotation(3.0*PI / 2.0);
				new_car.set_original_rotation(3.0*PI / 2.0);
				new_car.set_lane(direction::SOUTH);
			}
			new_car.generate_desired_direction();
			m_cars.emplace_back(new_car);
			if (m_cars.size() == 1) {
				m_cars[0].start_timer(MaxTimePerCar);
			}
		}
	}
}

void Lane::turn_car()
{
	//TODO Bug where references only the front car until it is popped out of screen.
	//This means that there is a delay in execution between the 1st car and 2nd car.
	if (!this->is_lane_empty()) {
		int index = 0;
		for (Car& car : m_cars) {
			if (car.is_in_beyond_intersec()) {
				++index;
			}
		}
		if (index < m_cars.size()) {
			//printf("Index: %d\n", index);
			//printf("Array Size: %d\n", m_cars.size());
			Car& selected_car = m_cars.at(index);
			if (m_dir == direction::WEST || m_dir == direction::EAST) {
				if (selected_car.get_vel().x <= 0.f && selected_car.is_at_stop(m_lane_coords[m_dir])) {
					selected_car.set_turn_start(selected_car.get_position());
					selected_car.signal_to_move();
					if (selected_car.is_in_beyond_intersec()) {
						selected_car.speed_up();
					}
					Mix_VolumeChunk(m_car_rev, MIX_MAX_VOLUME / 10);
					Mix_PlayChannel(-1, m_car_rev, 0);
				}
			}

			if (m_dir == direction::NORTH || m_dir == direction::SOUTH) {
				if (selected_car.get_vel().y <= 0.f && selected_car.is_at_stop(m_lane_coords[m_dir])) {
					selected_car.set_turn_start(selected_car.get_position());
					selected_car.signal_to_move();
					if (selected_car.is_in_beyond_intersec()) {
						selected_car.speed_up();
					}
					Mix_VolumeChunk(m_car_rev, MIX_MAX_VOLUME / 10);
					Mix_PlayChannel(-1, m_car_rev, 0);
				}
			}
		}

		if (index + 1 < m_cars.size()) {
			m_cars[index + 1].start_timer(MaxTimePerCar);
		}
	}
}

bool Lane::is_lane_full() const
{
	return m_cars.size() >= MaxCarsPerLane;
}

bool Lane::is_lane_empty() const
{
	return m_cars.size() == 0;
}

void Lane::erase_first_car()
{
	m_cars.erase(m_cars.begin());
}

void Lane::set_car_direction(direction dir, int car_index) {
	m_cars[car_index].set_desired_direction(dir);
}