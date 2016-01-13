#pragma once

#include <map>
//class that creates entities whether player or AI

class entity {
public:
	int x,y;
	std::map<char, int> dir;
};