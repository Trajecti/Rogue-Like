#include "level.h"
#include <random>
#include <chrono>
#include <vector>
#include <iostream>
#include <algorithm>


room::room(int sxmin, int symin, int max_sz, int intersect_pct) {
	std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_int_distribution<int> pct(1, 100);
	std::uniform_int_distribution<int> dist1(1, ((max_sz - 1) + 1));


	int xsize, ysize;
	if ((pct(mt) % 100) <= intersect_pct) {
		xsize = 1;
		ysize = 1;
	}
	else {
		xsize = dist1(mt);
		ysize = dist1(mt);
	}

	std::uniform_int_distribution<int> distx(1, ((max_sz - xsize - 1) + 2));
	std::uniform_int_distribution<int> disty(1, ((max_sz - ysize - 1) + 2));

	int rx = distx(mt);
	int ry = disty(mt);

	if (rx == 0) { rx = 1; }
	if (ry == 0) { ry = 1; }
	xmin = sxmin + rx;
	ymin = symin + ry;
	xmax = xmin + xsize;
	ymax = ymin + ysize;


}

room::room(int x_size, int gx, int gy)
{
	std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_int_distribution<int> dist1(1, ((x_size - 1) + 1));
	int xsize = dist1(mt);
	int ysize = dist1(mt);

	std::uniform_int_distribution<int> distx(xsize + 1, gx - xsize - 1);
	std::uniform_int_distribution<int> disty(ysize + 1, gy - ysize - 1);

	xmin = distx(mt);
	ymin = disty(mt);
	xmax = xmin + xsize;
	ymax = ymin + ysize;


}

level::level(int square_x, int difficult, int max_room_size, int id_) : map(boost::extents[square_x][square_x]) {
	// requires gx^2 > 4*max_room_size
	id = id_;
	gx = square_x;
	gy = gx;
	difficulty = difficult;
	floor = 864;
	wall = 17 * 64 + 40;

	int total_rsize = (gx*gy)*0.75;
	int allocated = 0;

	std::fill(map.origin(), map.origin() + map.num_elements(), wall);

	/* alternate room generation
	for (int i = 0; i < num_rooms; i++) {
		room temp = room((int)(sqrt(max_room_size)), gx, gy);
		int temp_size = ((temp.xmax - temp.xmin)*(temp.ymax - temp.ymin));
		bool overlap = check_overlap(temp);

		while (((allocated + temp_size) >= total_rsize) && overlap) {
			temp = room((int)(sqrt(max_room_size)), gx, gy);
			temp_size = ((temp.xmax - temp.xmin)*(temp.ymax - temp.ymin));
			overlap = check_overlap(temp);
		}

		allocated += temp_size;
		rooms.push_back(temp);
		room_centers.push_back(std::make_pair((temp.xmin + temp.xmax)*0.5, (temp.ymin + temp.ymax)*0.5));
		fill_room(temp);
	}
	*/

	int orig_size = max_room_size;
	int max_room_space = (max_room_size * 3) / 2;

	// getting rooms with space per row
	std::vector<int> room_coords;
	int room_index = static_cast<int> (gx / (sqrt(max_room_space)));
	int tiles_per_index = gx / room_index;

	int intersect_pct = 0;
	// getting a random coord for each room on grid
	for (int x = 0; x < room_index; x++) {
		for (int y = 0; y < room_index; y++) {
			int sxmin = y*tiles_per_index;
			int symin = x*tiles_per_index;
			room temp = room(sxmin, symin, tiles_per_index, intersect_pct);

			if (((temp.xmax) < gx) && ((temp.ymax) < gy) && ((sxmin + tiles_per_index) < gx) && ((symin + tiles_per_index) < gy)) {
				temp.id = x*room_index + y;
				// adding room and room center to both vectors
				rooms.push_back(temp);
				room_centers.push_back(std::make_pair((temp.xmin + temp.xmax)*0.5, (temp.ymin + temp.ymax)*0.5));


				int t_size = ((temp.xmax - temp.xmin)*(temp.ymax - temp.ymin));
				float pct = (float)t_size / (float)total_rsize;

				max_room_size = static_cast<int>(max_room_size*(1 - pct));
			}

		}
	}

	gen_level_map(864, 17 * 64 + 40);
	tunnel_rooms();

}

level::level(int sx, int sy, int difficult, int t_wall, int t_floor, int t_id) : map(boost::extents[sx][sx])
{
	gx = sx;
	gy = sy;
	difficulty = difficult;
	floor = t_floor;
	wall = t_wall;
	id = t_id;
}



void level::gen_level_map(int t_floor, int t_wall)
{
	floor = t_floor;
	wall = t_wall;
	std::fill(map.origin(), map.origin() + map.num_elements(), t_wall);

	for (size_t i = 0; i < rooms.size(); i++) {
		for (int j = (rooms[i].ymin) + 1; j < rooms[i].ymax; j++) {
			for (int k = (rooms[i].xmin) + 1; k < rooms[i].xmax; k++) {
				map[j][k] = t_floor;
			}
		}
	}

}

void level::load_map_data(std::vector<int> data)
{
	for (size_t i = 0; i < map.shape()[0]; i++) {
		for (size_t j = 0; j < map.shape()[2]; j++) {
			map[i][j] = data[i*gx + j];
		}
	}
}

Delaunay level::delaunay_triangulated_level()
{
	std::vector<Point> points;

	for (size_t i = 0; i < room_centers.size(); i++) {
		points.push_back(Point(std::get<0>(room_centers[i]), std::get<1>(room_centers[i])));
	}

	Delaunay dt;
	dt.insert(points.begin(), points.end());

	return dt;
}

std::vector<std::pair<Point, Point>> level::gen_edges()
{
	VertexIndexMap vertex_id_map;
	VertexIdPropertyMap vertex_index_pmap(vertex_id_map);
	Delaunay dt = delaunay_triangulated_level();
	Filter is_finite(dt);
	Finite_triangulation g(dt, is_finite, is_finite);


	//get all triangulated edges
	std::vector<std::pair<Point, Point>> edge_set, paths;

	for (Delaunay::Finite_edges_iterator it = dt.finite_edges_begin(); it != dt.finite_edges_end(); it++) {
		Delaunay::Edge e = *it;
		Point p1 = e.first->vertex((e.second + 1) % 3)->point();
		Point p2 = e.first->vertex((e.second + 2) % 3)->point();
		edge_set.push_back(std::make_pair(p1, p2));
	}

	//set index
	int index = 0;
	vertex_iterator vit, ve;
	for (boost::tie(vit, ve) = boost::vertices(g); vit != ve; vit++) {
		vertex_descriptor vd = *vit;
		vertex_id_map[vd] = index++;
	}

	//using default euclidean metric
	std::list<edge_descriptor> mst;
	boost::kruskal_minimum_spanning_tree(g, std::back_inserter(mst), vertex_index_map(vertex_index_pmap));


	for (std::list<edge_descriptor>::iterator it = mst.begin(); it != mst.end(); ++it) {
		edge_descriptor ed = *it;
		vertex_descriptor svd = source(ed, dt);
		vertex_descriptor tvd = target(ed, dt);

		Delaunay::Vertex_handle sv = svd;
		Delaunay::Vertex_handle tv = tvd;

		//removes edge from set of possible edges
		std::remove(edge_set.begin(), edge_set.end(), std::make_pair(sv->point(), tv->point()));

		// gets coords of in-neighbour and out-neighbour and then find position in iterator
		paths.push_back(std::make_pair(sv->point(), tv->point()));

	}

	//adding random Delaunay edges to make map more interesting
	std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_int_distribution<int> dist(1, (static_cast<int>(rooms.size()) / 4));
	int num_extra_edges = dist(mt);
	std::shuffle(edge_set.begin(), edge_set.end(), rd);

	for (int i = 0; i < num_extra_edges; i++) {
		// gets coords of in-neighbour and out-neighbour and then find position in iterator
		paths.push_back(edge_set[i]);
	}
	return paths;
}


std::vector<pt> level::tunnel_astar_search(pt p1, pt p2)
{
	//using modified A* search that encourages floor use and discourages turns
	std::vector<pt> closed_set, open_set, route;
	open_set.push_back(p1);
	int count = 0;
	std::map<pt, int> f_score, g_score;
	std::map<pt, pt> path;
	f_score.insert(std::make_pair(p1, sqr_euclid_dis(p1, p2)));
	g_score.insert(std::make_pair(p1, 0));

	while (!open_set.empty()) {
		std::vector<pt>::iterator it = std::min_element(open_set.begin(), open_set.end(),
			[&f_score](pt a, pt b) {return (f_score[a] < f_score[b]); });
		pt current = *it;

		if (current == p2) {
			route.push_back(current);
			for (size_t i = 0; i < path.size(); i++) {
				current = path[current];
				route.push_back(current);
			}
			return route;
		}

		open_set.erase(it);
		closed_set.push_back(current);

		//getting neighbours
		int x = std::get<0>(current);
		int y = std::get<1>(current);
		std::vector<pt> neighbours;
		if (x + 1 != gx) {
			neighbours.push_back(std::make_pair(x + 1, y));
		}
		if (y + 1 != gy) {
			neighbours.push_back(std::make_pair(x, y + 1));
		}
		if (x - 1 != 0) {
			neighbours.push_back(std::make_pair(x - 1, y));
		}
		if (y - 1 != 0) {
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
			int temp_g = g_score[current];
			if (map[x2][y2] == floor) {
				temp_g += 1;
			}
			else if (map[x2][y2] != floor) {
				temp_g += 2;
			}
			//checking if turning a corner
			if (!path.empty()) {
				pt prev = path[current];
				int x1 = std::get<0>(prev);
				int y1 = std::get<1>(prev);
				if ((x1 + 2 != x2) || (x1 - 2 != x2) || (y1 + 2 != y2) || (y1 - 2 != y2)) {
					temp_g += 1000;
				}
			}
			if (std::find(open_set.begin(), open_set.end(), neighbours[i]) == open_set.end()) {
				open_set.push_back(neighbours[i]);
			}
			else if (temp_g >= g_score[neighbours[i]]) {
				continue;
			}

			path.insert(std::make_pair(neighbours[i], current));
			g_score.insert(std::make_pair(neighbours[i], temp_g));
			f_score.insert(std::make_pair(neighbours[i], g_score[neighbours[i]] + sqr_euclid_dis(neighbours[i], p2)));
		}

	}
	// in case of failure, return empty path
	return route;


}

void level::tunnel_rooms()
{
	std::vector<std::pair<Point, Point>> paths = gen_edges();
	//creating vector to store all floor tiles to be added
	std::vector<pt> tunnel;

	for (size_t i = 0; i < paths.size(); i++) {


		//rounding double values of room centes to ints and adding tunnel to map
		pt p1 = std::make_pair((int)(std::get<0>(paths[i]).x()), (int)(std::get<0>(paths[i]).y()));
		pt p2 = std::make_pair((int)(std::get<1>(paths[i]).x()), (int)(std::get<1>(paths[i]).y()));
		//pt p1 = std::make_pair(rooms[room1].xmin + 1, rooms[room1].ymin + 1);
		//pt p2 = std::make_pair(rooms[room2].xmin + 1, rooms[room2].ymin + 1);
		tunnel = tunnel_astar_search(p1, p2);

		for (size_t j = 0; j < tunnel.size(); j++) {
			map[std::get<0>(tunnel[j])][std::get<1>(tunnel[j])] = floor;
		}

	}

}

#include <fstream>
#include <boost/lexical_cast.hpp>
#include <string>

void level::save_map()
{
	std::string str_id = boost::lexical_cast<std::string>(id);
	std::string str_gx = boost::lexical_cast<std::string>(gx);
	std::string str_gy = boost::lexical_cast<std::string>(gy);
	std::string str_diff = boost::lexical_cast<std::string>(difficulty);
	std::string str_floor = boost::lexical_cast<std::string>(floor);
	std::string str_wall = boost::lexical_cast<std::string>(wall);

	std::ofstream save_file("floor" + str_id + ".txt");

	if (save_file.is_open()) {
		save_file << str_id + "x" + str_gx + "x" + str_gy + "x" + str_diff + "x" + str_wall + "x" + str_floor + '\n';
		std::string output = "";

		for (size_t i = 0; i < map.shape()[0]; i++) {
			for (size_t j = 0; j < map.shape()[1]; j++) {
				std::string str_tile = boost::lexical_cast<std::string>(map[i][j]);
				output += (str_tile + " ");
			}
			save_file << output + '\n';
		}
		save_file.close();
	}
	else {
		std::cout << "Error: Saving floor to file failed. \n";
	}

}

/* level load_map(std::string filename) {

	std::string line;
	std::ifstream save(filename);
	std::vector<int> map_data;

	int l_id, l_gx, l_gy, l_difficulty, l_wall, l_floor;
	bool first_line = true;
	int ix = 0;
	std::vector<std::string> variables, tiles;

	save.open(filename);
	std::getline(save, line);
	for (size_t i = 0; i < 6; i++) {
		// find x delimiter
		size_t pos = line.find_first_of("x");
		variables.push_back(line.substr(0, pos));
		line.erase(0, pos + 1);
	}
	l_id = boost::lexical_cast<int>(variables[0]);
	l_gx = boost::lexical_cast<int>(variables[1]);
	l_gy = boost::lexical_cast<int>(variables[2]);
	l_difficulty = boost::lexical_cast<int>(variables[3]);
	l_wall = boost::lexical_cast<int>(variables[4]);
	l_floor = boost::lexical_cast<int>(variables[5]);
	level floor = level(l_gx, l_gy, l_difficulty, l_wall, l_floor, l_id);

	while (std::getline(save, line)) {
		for (int i = 0; i < floor.get_gx(); i++) {
			// find x delimiter
			size_t pos = line.find_first_of(" ");
			tiles.push_back(line.substr(0, pos));
			line.erase(0, pos);
		}
		for (size_t i = 0; i < tiles.size(); i++) {
			map_data.push_back(boost::lexical_cast<int>(tiles[i]));
		}
	}

	save.close();
	floor.load_map_data(map_data);
	return floor;
}
*/

float sqr_euclid_dis(pt p1, pt p2)
{
	int x1 = std::get<0>(p1);
	int x2 = std::get<0>(p2);
	int y1 = std::get<1>(p1);
	int y2 = std::get<1>(p2);
	return ((x2 - x1)*(x2 - x1)) + ((y2 - y1)*(y2 - y1));
}







