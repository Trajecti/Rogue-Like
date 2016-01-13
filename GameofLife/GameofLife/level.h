#pragma once
#include <vector>
#include <map>
#include <random>
#include <boost\multi_array.hpp>

// classes that create rooms and also generaates the floor in a procedurally generated manner
// credit to Bob Nystrom's explanations on generating random floors

class room
{
public:
	int xmin, xmax, ymin, ymax, id;
	room(int x, int y, double rect_pct, int room_space, int r_id);
};

class layer {
public:
	void add_rooms(int num_rooms,int room_space,int rect_pct);
	void add_region() { region_index += 1; }
	void carve(int pt) { map_data[pt] = region_index; map[pt] = floor; }
	layer(int x_, int y_, int t_wall, int t_floor, int num_rooms, int max_room_space);
	void maze_gen(int x);
	bool carve_possible(int x, int dir);
	bool overlap(room& r);
	void connect_regions();
	void fill_dead_ends();
	boost::multi_array<int, 2> get_map();
private:
	typedef std::pair<int, int> pt; //give region and coordinate;
	int x, y, gx, gy, wall, floor, wind_pct, connect_pct, region_index;
	std::map<char, int> dir;
	std::map<int, int> map_data;
	std::vector<int> map;
	std::vector<room> rooms;
};

