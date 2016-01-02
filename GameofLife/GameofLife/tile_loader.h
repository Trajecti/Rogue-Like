#pragma once

#include <string>
#include "SFML\Graphics\VertexArray.hpp"

//used with help from SFML TileMap page

struct MapData {
	std::string TextureId;
	int MapX, MapY;
	MapData(void) :MapX(0), MapY(0) {}
};

class TileLoader {
protected:
	MapData t_mapdata; //fill with texture and map size in tiles
public:
	virtual ~TileLoader(void) {};
	virtual void AppendTile(int gx, int gy, int tilemap_size_x, sf::VertexArray& grid_array) = 0;
	/*
	 2.implement this method to append(or skip appending) tiles(4 vertices) to given vertex array,
	 you may assume tiles will be requested in row major order:((0,0),(1,0),..,(map_x-1,0),(0,1),..,(map_x-1,map_y-1)),
	 this method will be called exactly MapX*MapY times,
	 for correct displaying and culling it's assumed your appended tile will be square with side equal to tilesize
	 and have it's top left point at (tilesize*gx,tilesize*gy),
	 */
	const MapData& GetData() const { return t_mapdata; }
	
};