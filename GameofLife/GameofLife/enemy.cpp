#include "stdafx.h"
#include "enemy.h"


enemy::enemy(int tx, int ty, int sizet, int floor_, std::string filename, sf::IntRect rect, int num_img) 
	:entity(tx,ty,sizet,floor_,filename,rect,num_img) {

}


enemy::~enemy()
{
}

void enemy::find_player(pt player,int length, boost::multi_array<int,2> map)
{
	//using modified A* search that finds player
	std::vector<pt> closed_set, open_set, route;
	pt p1 = std::make_pair(x, y);


	open_set.push_back(p1);
	int count = 0;
	std::map<pt, int> f_score, g_score;
	std::map<pt, pt> path;
	f_score.insert(std::make_pair(p1, sqr_euclid_dis(p1, player)));
	g_score.insert(std::make_pair(p1, 0));

	while (!open_set.empty()) {
		std::vector<pt>::iterator it = std::min_element(open_set.begin(), open_set.end(),
			[&f_score](pt a, pt b) {return (f_score[a] < f_score[b]); });
		pt current = *it;

		if (current == player) {
			route.push_back(current);
			for (size_t i = 0; i < path.size(); i++) {
				current = path[current];
				if (current != player) {
					route.push_back(current);
				}
				else {
					x = route.back().first;
					y = route.back().second;
					move_entity(x, y, map[x][y]);
				}
			}
			//moves enemy
			
			
			
		}

		open_set.erase(it);
		closed_set.push_back(current);

		//getting neighbours
		int x = std::get<0>(current);
		int y = std::get<1>(current);
		std::vector<pt> neighbours;
		
		if ((x!=49) && (map[x+1][y] == floor)) {
			neighbours.push_back(std::make_pair(x + 1, y));
		}
		if ((y!=49) && (map[x][y+1] == floor)) {
			neighbours.push_back(std::make_pair(x, y + 1));
		}
		if ((x!=0) && (map[x - 1][y] == floor)) {
			neighbours.push_back(std::make_pair(x - 1, y));
		}
		if ((y!=0) && (map[x][y-1] == floor)) {
			neighbours.push_back(std::make_pair(x, y - 1));
		}

		for (size_t i = 0; i < neighbours.size(); i++) {
			//getting neighbour coordinates
			int x2 = std::get<0>(neighbours[i]);
			int y2 = std::get<1>(neighbours[i]);

			if (std::find(closed_set.begin(), closed_set.end(), neighbours[i]) != closed_set.end()) {
				continue;
			}

			// adding costs of tunnelling through walls,floors or turning corners
			int temp_g = g_score[current] + 1;

			if (std::find(open_set.begin(), open_set.end(), neighbours[i]) == open_set.end()) {
				open_set.push_back(neighbours[i]);
			}
			else if (temp_g >= g_score[neighbours[i]]) {
				continue;
			}

			path.insert(std::make_pair(neighbours[i], current));
			g_score.insert(std::make_pair(neighbours[i], temp_g));
			f_score.insert(std::make_pair(neighbours[i], g_score[neighbours[i]] + sqr_euclid_dis(neighbours[i], player)));
		}

	}

	//in case of failure do nothing

}

float sqr_euclid_dis(pt p1, pt p2)
{
	int x1 = std::get<0>(p1);
	int x2 = std::get<0>(p2);
	int y1 = std::get<1>(p1);
	int y2 = std::get<1>(p2);
	return ((x2 - x1)*(x2 - x1)) + ((y2 - y1)*(y2 - y1));
}