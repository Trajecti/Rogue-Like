#include "stdafx.h"
#include "entity.h"


entity::entity(int tx, int ty, int sizet, int floor_, std::string filename, sf::IntRect rect,int num_img)
{
	//intializing variables and loading texture
	x = tx;
	y = ty;
	size = sizet;
	health = -1;
	floor = floor_;
	if (!sprite_t.loadFromFile(filename, rect)) { ; }
	sprite_s.setTexture(sprite_t);

	if (num_img != -1) {

		std::string cardinal = "NSEW";
		// imgs 0-3 represent NSEW
		//set dir to be north as default
		//other values can be autoadded
		dir = cardinal[0];

		for (auto i : cardinal) {
			img_change.insert(std::make_pair(cardinal[i], i));
		}

		//face sprite in direction
		sprite_s.setTextureRect(sf::IntRect(size*img_change[dir], 0, size, size));
	}
	else {
		sprite_s.setTextureRect(sf::IntRect(0, 0, size, size));
	}
}

bool entity::move_entity(int x_,int y_,int tile_val)
{
	if (tile_val == floor) {
		x = x_;
		y = y_;
		sprite_s.setPosition(x*size, y*size);
		return true;
	}
	return false;
}


