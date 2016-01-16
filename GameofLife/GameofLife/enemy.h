#pragma once
#include "entity.h"
#include <boost\multi_array.hpp>

typedef std::pair<int, int> pt;
class enemy :
	public entity
{
public:
	enemy(int tx, int ty, int sizet, int floor_, std::string filename, sf::IntRect rect, int num_img);
	~enemy();
	void enemy::find_player(pt player,int length, boost::multi_array<int, 2> map);
};

float sqr_euclid_dis(pt p1, pt p2);
