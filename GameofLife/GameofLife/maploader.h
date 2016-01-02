#pragma once
#include "tile_loader.h"
#include "tilemap.h"
#include "SFML\Graphics.hpp"
#include "boost\multi_array.hpp"

class MapLoader :public TileLoader {
private:
	boost::multi_array<int,2> tile_num;
public:
	MapLoader(int x, int y, std::string filename);
	boost::multi_array<int, 2> get_tile_num();
	void set_tile_num(boost::multi_array<int, 2> vec);
	virtual void AppendTile(int gx, int gy, int tilemap_size_x, sf::VertexArray& grid_array);
};