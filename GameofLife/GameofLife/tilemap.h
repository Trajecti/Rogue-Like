#pragma once

#include "SFML\Graphics.hpp"
#include <vector>
#include "tile_loader.h"

const int tile_size = 32;
const int chunk_size = 100; 
class TileLoader;

class TileMap: public sf::Drawable {
private:
	enum { tileSize = tile_size,chunkSize= chunk_size };
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states)const;
	sf::Texture t_texture;
	int map_x, map_y, chunk_x, chunk_y; //dimensions of map in tiles; number of chunks
	std::vector<std::vector<sf::VertexArray>> t_chunks;

public:
	TileMap();
	virtual ~TileMap() {};
	void LoadFrom(TileLoader* gloader);
};