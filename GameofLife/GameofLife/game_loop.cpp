#include "SFML\Graphics.hpp"
#include <iostream>
#include "level.h"
#include "boost\multi_array.hpp"
#include "game_loop.h"
#include "maploader.h"
#include "entity.h"
#include "enemy.h"


const int wall = 17 * 64 + 40;
const int t_floor = 864;
const int tsize = 32;

int game_loop(int res_x, int res_y, int grid_size) {
	//int grid_x = (res_x) / grid_size;
	//int grid_y = (res_y) / grid_size;
	int grid_x = 50;

	// Game Loop starts
	sf::RenderWindow window(sf::VideoMode(res_x, res_y), "Roguelike");
	window.setVerticalSyncEnabled(true);


	TileMap map;
	MapLoader loader(grid_x, grid_x, "DungeonCrawlTileset.png");

	//generate floor with only odd sizes
	layer floor(grid_x, grid_x, wall, t_floor, 25, 10);

	typedef boost::multi_array<int, 2> array_init;
	array_init tmap(boost::extents[grid_x][grid_x]);
	tmap = floor.get_map();

	loader.set_tile_num(tmap);
	map.LoadFrom(&loader);

	int px = 0;
	int py = 0;
	int ex = 0;
	int ey = 0;
	//find random starting position
	while (px == 0) {
		int tempx = rand() % grid_x;
		int tempy = rand() % grid_x;
		if (tmap[tempx][tempy] == t_floor) {
			px = tempx;
			py = tempy;
		}
	}

	while (ex == 0) {
		int tempx = rand() % grid_x;
		int tempy = rand() % grid_x;
		if (tmap[tempx][tempy] == t_floor) {
			ex = tempx;
			ey = tempy;
		}
	}

	//create player character
	entity player(px, py, tsize, t_floor, "DungeonCrawlTileset.png", sf::IntRect(32 * 4, 32 * 2, 32, 32), -1);
	player.move_entity(px, py, tmap[player.x][player.y]);

	enemy e1(ex, ey, tsize, t_floor, "DungeonCrawlTileset.png", sf::IntRect(32 * 4, 32 * 2, 32, 32), -1);
	e1.move_entity(ex, ey, tmap[ex][ey]);

	sf::View cam = window.getDefaultView();
	cam.zoom(0.5f);
	cam.setCenter(static_cast<sf::Vector2f>(window.mapPixelToCoords(sf::Vector2i((px*tsize), (py*tsize)))));
	float shift = (static_cast<sf::Vector2f>(window.mapPixelToCoords(sf::Vector2i(tsize, tsize)))).x;

	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {

			if (event.type == sf::Event::Closed) {
				window.close();
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
				cam.zoom(1.05f);

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::E))
				cam.zoom(0.95f);

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
				if (player.move_entity(player.x, player.y - 1, tmap[player.x][player.y - 1])) {
					cam.move(0.f, -1.f*shift);
				}
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
				if (player.move_entity(player.x, player.y + 1, tmap[player.x][player.y + 1])) {
					cam.move(0.f, 1.f*shift);
				}
			}

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
				if (player.move_entity(player.x - 1, player.y, tmap[player.x - 1][player.y])) {
					cam.move(-1.f*shift, 0.f);
				}
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
				if (player.move_entity(player.x + 1, player.y, tmap[player.x + 1][player.y])) {
					cam.move(1.f*shift, 0.f);
				}
			}

			int mx = sf::Mouse::getPosition(window).x;
			int my = sf::Mouse::getPosition(window).y;
		}

		//e1.find_player(std::make_pair(player.x, player.y), 1,tmap);
		window.setView(cam);
		window.clear(sf::Color::Black);
		window.draw(map);
		window.draw(player.sprite_s);
		window.draw(e1.sprite_s);
		window.display();
	}
	return 0;
}


boost::multi_array<int, 1> update_draw(int direction) {
	// let 0= up, 1=right,2=down and 3=left
	boost::multi_array<int, 1> vec;
	return vec;
}
