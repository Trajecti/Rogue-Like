#include "SFML\Graphics.hpp"
#include <iostream>
#include "level.h"
#include "boost\multi_array.hpp"
#include "game_loop.h"
#include "maploader.h"


const int wall = 17 * 64 + 40; 
const int t_floor = 864 ;

int game_loop(int res_x, int res_y, int grid_size) {
	//int grid_x = (res_x) / grid_size;
	//int grid_y = (res_y) / grid_size;
	int grid_x = 49;

	// Game Loop starts
	sf::RenderWindow window(sf::VideoMode(res_x, res_y), "Roguelike");
	window.setVerticalSyncEnabled(true);
	sf::View cam = window.getDefaultView();

	TileMap map;
	MapLoader loader(grid_x,grid_x,"DungeonCrawlTileset.png");
	//generate floor with only odd sizes
	layer floor(grid_x, grid_x, wall, t_floor,25,10);
	loader.set_tile_num(floor.get_map());
	map.LoadFrom(&loader);
	//floor.save_map();
	//load_map("floor0.txt");

	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {

			if (event.type == sf::Event::Closed) {
				window.close();
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
				cam.zoom(1.05f);

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
				cam.move(0.f, -10.f);

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::E))
				cam.zoom(0.95f);

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
				cam.move(-10.f, 0.f);

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
				cam.move(0.f, 10.f);

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
				cam.move(10.f, 0.f);

			int mx = sf::Mouse::getPosition(window).x;
			int my = sf::Mouse::getPosition(window).y;
		}

		window.setView(cam);
		window.clear(sf::Color::Black);
		window.draw(map);
		window.display();
	}
	return 0;
}


boost::multi_array<int, 1> update_draw(int direction) {
	// let 0= up, 1=right,2=down and 3=left
	boost::multi_array<int, 1> vec;
	return vec;
}
