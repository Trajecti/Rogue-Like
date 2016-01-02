#pragma once
#include "boost\multi_array.hpp"
#include "tile.h"
#include <vector>
#include <array>

int game_loop(int res_x, int res_y,int grid_size);
boost::multi_array<tile, 2> init_grid(int x, int y, int size);
boost::multi_array<int,1> update_map(boost::multi_array<tile, 2> &grid);