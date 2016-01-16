#pragma once
#include <SFML\Graphics\Sprite.hpp>
#include <SFML\Graphics\Texture.hpp>
#include <map>

//class that creates entities whether player or AI

class entity {
public:
	int x,y,size,health,floor;
	char dir;
	sf::Sprite sprite_s;
	sf::Texture sprite_t;
	std::map<char, int> img_change;

	entity(int tx, int ty, int sizet, int floor_, std::string filename,sf::IntRect rect,int num_img);
	~entity() {};
	bool move_entity(int x, int y, int tile_val);

};