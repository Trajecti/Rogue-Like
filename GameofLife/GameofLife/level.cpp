#include "stdafx.h"
#include "level.h"
#include <algorithm>



layer::layer(int width, int height, int t_wall, int t_floor, int num_rooms, int max_room_space)
{
	gx = width;
	gy = height;
	// given that the wall tile and floor tile will be  a single piece in maze generation, setting  x to be odd
	// so that the first x column can all wall thus the rest can consist of maze pieces
	if (width % 2 == 0) { x = width - 1; }
	else { x = width; }
	// same for y
	if (height % 2 == 0) { y = height - 1; }
	else { y = height; }
	wall = t_wall;
	floor = t_floor;
	region_index = -1;
	wind_pct = 0;

	std::vector<int> temp;
	temp.assign(x*y, wall);
	map = temp;

	//create direction and assign values
	std::string cardinal = "NSEW";
	dir.insert(std::make_pair(cardinal[0], x));
	dir.insert(std::make_pair(cardinal[1], -x));
	dir.insert(std::make_pair(cardinal[2], 1));
	dir.insert(std::make_pair(cardinal[3], -1));

	add_rooms(num_rooms, max_room_space, 50);



	for (int i = 1; i < y; i += 2) {
		for (int j = 1; j < x; j += 2) {
			if (map[j + i*x] == wall) {
				maze_gen(j + i*x);
			}
			else {
				continue;
			}
		}
	}

	connect_regions();
	fill_dead_ends();

}

void layer::maze_gen(int x_)
{
	std::vector<int> cells;

	int current_dir = 0;;


	//adding region and assigning points
	add_region();
	carve(x_);
	cells.push_back(x_);


	while (!cells.empty()) {
		int current = cells.back();
		int tempx = current%x;
		int tempy = current / y;
		// adds valid cells not in region and checks if they are unmade cells that can be carved
		std::vector<int> dead_cells;

		for (std::map<char, int>::iterator it = dir.begin(); it != dir.end(); ++it) {
			if ((carve_possible(current, it->second))) {
				dead_cells.push_back(it->second);
			}
		}

		//if such cells exist, choose and carve randomly, winding_pct constant bias to maintaining current direction
		if (!dead_cells.empty()) {
			std::random_device rd;
			std::mt19937 mt(rd());
			std::uniform_int_distribution<int> pct(0, 100);

			std::vector<int>::iterator it = std::find(dead_cells.begin(), dead_cells.end(), current_dir);
			int next_dir;

			// assesses whther or not to be windy passage or not
			if ((it != dead_cells.end() && (pct(mt) < wind_pct))) {
				next_dir = current_dir;
			}
			else {
				next_dir = dead_cells[(pct(mt)) % dead_cells.size()];
			}

			carve(current + next_dir);
			carve(current + next_dir * 2);

			cells.push_back(current + next_dir * 2);
			current_dir = next_dir;
		}
		else {
			cells.pop_back();
			current_dir = 0;
		}

	}
}

room::room(int x, int y, double rect_pct, int max_room_space, int r_id)
{
	int xsize, ysize, square_size;
	std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_int_distribution<int> dist(1, max_room_space);
	square_size = dist(mt) * 2 + 1; //make sure size is odd for 2-tiled maze  components to work

	// make square into a possible rectangle
	std::uniform_int_distribution<int> dist_rect(0, (int)(0.01*rect_pct*square_size));
	int rect_factor = dist_rect(mt) * 2; //get possible value to change width or height and make a rectangle 
	if (rect_factor % 2 == 1) {
		xsize = square_size + rect_factor + 1;
		ysize = square_size;
	}
	else {
		ysize = square_size + rect_factor + 1;
		xsize = square_size;
	}

	//getting coords
	std::uniform_int_distribution<int> distx(0, (x - xsize) / 2);
	std::uniform_int_distribution<int> disty(0, (y - ysize) / 2);

	//ensuring coordinates are odd and getting final values
	xmin = distx(mt) * 2 + 1;
	ymin = disty(mt) * 2 + 1;
	xmax = xmin + xsize;
	ymax = ymin + ysize;
	id = r_id;
}

void layer::add_rooms(int num_rooms, int max_room_space, int rect_pct)
{
	int id = 0;
	for (int i = 0; i < num_rooms; i++) {

		room temp = room(x, y, rect_pct, max_room_space, id);

		if (overlap(temp)) {
			continue;
		}

		id++;
		rooms.push_back(temp);
		add_region();

		//carving entire room as region
		for (int j = temp.xmin +1 ; j < temp.xmax; ++j) {
			for (int k = temp.ymin +1; k < temp.ymax; ++k) {
				carve(k*x + j);
			}
		}

	}
}


void layer::connect_regions() {

	//finding connector_regions

	std::map<int, std::vector<int>> connector_regions;

	//starting from non-edge areas to prevent out-of-bound errors 
	for (int i = 1; i < (y - 1); ++i) {
		for (int j = 1; j < (x - 1); ++j) {

			int tile = i*x + j;
			if (map[tile] != wall) { continue; }

			std::vector<int> regions_beside_tile;
			for (std::map<char, int>::iterator it = dir.begin(); it != dir.end(); ++it) {
				int tile_region = map_data[i*gx + j + it->second];
				if (std::find(regions_beside_tile.begin(), regions_beside_tile.end(), tile_region) == regions_beside_tile.end()) {
					//checking how many regions are neighbours of the tile
					regions_beside_tile.push_back(tile_region);
				}

				if (regions_beside_tile.size() < 2) {
					continue;
				}

				connector_regions.insert(std::make_pair(tile, regions_beside_tile));
			}
		}

		//tracks index value through merges and second tracks unconnected regions
		std::map<int, int> region_index_value, unconnected_region;

		for (int i = 0; i < region_index; ++i) {
			region_index_value.insert(std::make_pair(i, i));
			unconnected_region.insert(std::make_pair(i, i));
		}

		std::random_device rd;
		std::mt19937 mt(rd());

		std::uniform_int_distribution<int> pct(0, 100);
		std::uniform_int_distribution<int> dist(0, connector_regions.size());
		int main_region;
		while ((unconnected_region.size() > 1) && !connector_regions.empty()) {

			//select random connector and connect by making it a floor
			auto it = connector_regions.begin();
			std::advance(it, dist(mt) % connector_regions.size());
			map[it->first] = floor;

			//making merged regions all region of connector
			main_region = map_data[it->first];
			std::vector<int> connected_regions = it->second;

			for (int i = 0; i < region_index; ++i) {
				int current_index = region_index_value[i];
				if (std::find(connected_regions.begin(), connected_regions.end(), current_index) != connected_regions.end()) {
					region_index_value[i] = main_region;
					unconnected_region.erase(current_index);
				}
			}

			int x1 = it->first%x;
			int y1 = it->first / x;

			auto i = connector_regions.begin();
			while (i != connector_regions.end()) {
				int x2 = i->first%x;
				int y2 = i->first / x;
				bool close = false;
				if ((abs(x2 - x1) < 2) || (abs(y2 - y1)) < 2) {
					close = true;
				}
				//checking if regions in each connector still exist
				std::vector<int> inner_regions = i->second;
				auto j = inner_regions.begin();
				while (j != inner_regions.end()) {
					if (unconnected_region.find(*j) == unconnected_region.end()) {
						j = inner_regions.erase(j);
					}
					else {
						j++;
					}
				}

				if (inner_regions.size() < 2) {
					//randomly let some non close connectors connect
					if ((pct(mt) < connect_pct) && (!close)) {
						map[i->first] = floor;
					}

					//ensures iterator continutes after deletion
					i = connector_regions.erase(i);
				}
				else {
					i++;
				}
			}





		}


	}

}
void layer::fill_dead_ends() {
	bool dead_ends_exist = true;

	while (dead_ends_exist) {
		dead_ends_exist = false;

		//starting from non-edge areas to prevent out-of-bound errors 
		for (int i = 1; i < (y - 1); ++i) {
			for (int j = 1; j < (x - 1); ++j) {
				// wall tile cannot be a dead end
				if (map[i] == wall) { continue; }
				int walls_around = 0;

				//checking # of  tiles around that are walls
				for (auto it : dir) {
					if (map[i + it.second] == wall) {
						++walls_around;
					}
				}
				if (walls_around == 3) {
					map[i] = wall;
					dead_ends_exist = true;
				}
			}
		}
	}
}
boost::multi_array<int, 2> layer::get_map()
{
	typedef boost::multi_array<int, 2> array_init;
	array_init grid(boost::extents[gx][gy]);

	int a, b;
	for (size_t i = 0; i < map.size(); ++i) {
		a = i%x;
		b = i / y;
		grid[a][b] = map[i];
	}

	return grid;
}
bool layer::carve_possible(int loc, int dir_)
{
	// checking if area past 2-tile exists or not
	int a = loc%x;
	int dir = dir_;
	int axis_l = x;

	if ((abs(dir)) != 1) {
		a = loc / x;
		dir = (dir / y);
		axis_l = y;
	}

	int dir3 = 3 * dir;

	if (((a + dir3) < axis_l) && ((a + dir3) >= 0)) {
		return (map[loc + 2 * dir_] == wall);
	}
	else {
		return false;
	}
}

bool layer::overlap(room & r)
{
	for (size_t i = 0; i < rooms.size(); ++i) {
		if ((rooms[i].xmin < r.xmax) && (rooms[i].xmax > r.xmin) && (rooms[i].ymin < r.ymax) && (rooms[i].ymax > r.ymin)) {
			return true;
		}
	}

	return false;
}


