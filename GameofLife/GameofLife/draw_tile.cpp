#include "StdAfx.h"
#include "draw_tile.h"
#include <SFML\Graphics.hpp>
#include "boost\multi_array.hpp"
#include <algorithm>
#include <vector>

draw_tile::draw_tile(void)
{
}

bool draw_tile::load(const std::string& tileset, sf::Vector2u squareSize, boost::multi_array<int, 1> squares, unsigned int width, unsigned int height)
{
	// load the tileset texture
	if (!m_tileset.loadFromFile(tileset))
		return false;

	// resize the vertex array to fit the level size
	m_vertices.setPrimitiveType(sf::Quads);
	m_vertices.resize(width * height * 4);

	// populate the vertex array, with one quad per tile
	for (unsigned int i = 0; i < width; ++i)
		for (unsigned int j = 0; j < height; ++j)
		{
			// get the current tile number
			int squarenumber = squares[i + j * width];

			// find its position in the tileset texture
			float tu = (float) (squarenumber % (m_tileset.getSize().x / squareSize.x));
			float tv = (float) (squarenumber / (m_tileset.getSize().x / squareSize.x));

			// get a pointer to the current tile's quad
			sf::Vertex* quad = &m_vertices[(i + j * width) * 4];

			// define its 4 corners
			quad[0].position = sf::Vector2f((float)i * squareSize.x, (float)j * squareSize.y);
			quad[1].position = sf::Vector2f(((float)i + 1) * squareSize.x, (float)j * squareSize.y);
			quad[2].position = sf::Vector2f(((float)i + 1) * squareSize.x, ((float)j + 1) * squareSize.y);
			quad[3].position = sf::Vector2f((float)i * squareSize.x, ((float)j + 1) * squareSize.y);

			// define its 4 texture coordinates
			quad[0].texCoords = sf::Vector2f(tu * squareSize.x, tv * squareSize.y);
			quad[1].texCoords = sf::Vector2f((tu + 1) * squareSize.x, tv * squareSize.y);
			quad[2].texCoords = sf::Vector2f((tu + 1) * squareSize.x, (tv + 1) * squareSize.y);
			quad[3].texCoords = sf::Vector2f(tu * squareSize.x, (tv + 1) * squareSize.y);
		}

	return true;
}

draw_tile::~draw_tile(void)
{
}

void draw_tile::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	// apply the transform
	states.transform *= getTransform();

	// apply the tileset texture
	states.texture = &m_tileset;

	// draw the vertex array
	target.draw(m_vertices, states);
}