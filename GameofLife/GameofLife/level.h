#pragma once
#include <vector>
#include <boost\multi_array.hpp>
#include <CGAL\Delaunay_triangulation_2.h>
#include <CGAL\Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL\boost\graph\graph_traits_Delaunay_triangulation_2.h>
#include <boost/graph/filtered_graph.hpp>
#include <boost/graph/kruskal_min_spanning_tree.hpp>
#include <boost/graph/adjacency_list.hpp>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Delaunay_triangulation_2<K>                   Delaunay;
typedef K::Point_2                                          Point;

// As we only consider finite vertices and edges
// we need the following filter : from CGAL example
template <typename T>
struct Is_finite {
	const T* t_;
	Is_finite()
		: t_(NULL)
	{}
	Is_finite(const T& t)
		: t_(&t)
	{ }
	template <typename VertexOrEdge>
	bool operator()(const VertexOrEdge& voe) const {
		return !t_->is_infinite(voe);
	}
};

typedef Is_finite<Delaunay> Filter;
typedef boost::filtered_graph<Delaunay, Filter, Filter> Finite_triangulation;
typedef boost::graph_traits<Finite_triangulation>::vertex_descriptor vertex_descriptor;
typedef boost::graph_traits<Finite_triangulation>::vertex_iterator vertex_iterator;
typedef boost::graph_traits<Finite_triangulation>::edge_descriptor edge_descriptor;
// indices associated to the vertices
typedef std::map<vertex_descriptor, int> VertexIndexMap;
// Changing std::map to a property map
typedef boost::associative_property_map<VertexIndexMap> VertexIdPropertyMap;
typedef std::pair<int, int> Edge;
typedef std::pair<int,int> pt;

class room
{
public:
	int xmin, xmax, ymin, ymax,id;
	room(int sxmin, int symin, int max_sz, int intersect_pct);
	room(int x_size, int gx, int gy);
};

class level {
private:
	std::vector<room> rooms;
	std::vector<std::pair<double, double>> room_centers;
	int difficulty, gx, gy,wall,floor,id;
	boost::multi_array<int, 2> map;
public:
	level() {};
	level(int gx, int difficulty, int max_room_size,int id);
	level(int gx, int gy, int difficulty, int wall, int floor, int id);

	int get_gx() { return gx; }
	int get_gy() { return gy; }
	std::vector<room> get_rooms() { return rooms; }
	void set_rooms(std::vector<room> floor) { rooms = floor; }
	
	int get_difficulty() { return difficulty; }
	void gen_level_map(int t_floor, int t_wall);
	boost::multi_array<int, 2> get_map() { return map; }
	
	Delaunay delaunay_triangulated_level();
	std::vector<std::pair<Point, Point>> gen_edges();
	std::vector<pt> tunnel_astar_search(pt p1,pt p2);
	void tunnel_rooms();
	
	void load_map_data(std::vector<int> data);
	void save_map();
};

level load_map(std::string filename);
float sqr_euclid_dis(pt p1, pt p2);


