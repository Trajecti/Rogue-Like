#include "stdafx.h"
#include "tilemap.h"

void TileMap::draw(sf::RenderTarget & target, sf::RenderStates states) const
{
	int left = 0; int right = 0; int top = 0; int bottom = 0;

	sf::Vector2f temp = target.getView().getCenter() - (target.getView().getSize() / 2.f);
	left = static_cast<int>(temp.x / (chunkSize*tileSize));
	top = static_cast<int>(temp.y / (chunkSize*tileSize));
	temp = target.getView().getSize();
	right = 1 + (static_cast<int>(temp.x/(chunkSize*tileSize)));
	bottom = 1 + (static_cast<int>(temp.y / (chunkSize*tileSize)));

	//fitting directions within array bounds
	left = std::max(0, std::min(left, chunk_x));
	top = std::max(0, std::min(top, chunk_y));
	right = std::max(0, std::min(right, chunk_x));
	bottom = std::max(0, std::min(bottom, chunk_y));

	//filling texture and drawing chunks in camera
	states.texture = &t_texture;

	for (int cx = left; cx < right; ++cx) {
		for (int cy = top; cy < bottom; ++cy) {
			target.draw(t_chunks[cx][cy],states);
		}
	}

}

TileMap::TileMap():
	map_x(0),map_y(0),chunk_x(0),chunk_y(0)
{
}

void TileMap::LoadFrom(TileLoader* gloader)
{
	t_texture.loadFromFile(gloader->GetData().TextureId);
	map_x = gloader->GetData().MapX;
	map_y = gloader->GetData().MapY;

	//if empty map; stop loading via
	if (map_x*map_y == 0) {
		chunk_x = 0;
		chunk_y = 0;
		t_chunks.resize(0);
		return;
	}

	chunk_x = 1 + (map_x / chunkSize);
	chunk_y = 1 + (map_y / chunkSize);
	t_chunks.assign(chunk_x, std::vector<sf::VertexArray>(chunk_y, sf::VertexArray(sf::Quads))); //ready up empty 2d arrays

	for (int cy = 0; cy < map_y; cy++) {
		for (int cx = 0; cx < map_x; cx++) {
			gloader->AppendTile(cx, cy, t_texture.getSize().x, t_chunks[cx / chunkSize][cy / chunkSize]);
		}
	}
}




