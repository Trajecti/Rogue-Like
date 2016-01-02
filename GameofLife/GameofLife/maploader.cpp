#include "maploader.h"


MapLoader::MapLoader(int x,int y,std::string filename): tile_num(boost::extents[x][y]) {
	// filling map with map dimensions in tiles
	t_mapdata.MapX = x;
	t_mapdata.MapY = y;
	t_mapdata.TextureId = filename;
}

boost::multi_array<int, 2> MapLoader::get_tile_num() {
	return tile_num;
}

void MapLoader::set_tile_num(boost::multi_array<int, 2> vec) {
	tile_num = vec;
}

void MapLoader::AppendTile(int gx_, int gy_, int tilemap_size_x, sf::VertexArray& grid_array) {
	if (!tile_num.empty()) {
		sf::Vertex ver;
		// find its position in the tileset texture
		int squarenumber = tile_num[gx_][gy_];
		float tu = (float) (squarenumber % (tilemap_size_x / tile_size));
		float tv = (float) (squarenumber / (tilemap_size_x / tile_size));
		float tf_sz = (float)tile_size;
		float gx = (float)gx_;
		float gy = (float)gy_;

		ver.position = sf::Vector2f(gx*tf_sz, gy*tf_sz);
		ver.texCoords = sf::Vector2f(tu * tile_size, tv * tile_size);
		grid_array.append(ver);

		ver.position = sf::Vector2f((gx*tf_sz) + tf_sz, gy*tf_sz);
		ver.texCoords = sf::Vector2f((tu + 1) * tile_size, tv * tile_size);
		grid_array.append(ver);

		ver.position = sf::Vector2f((gx*tf_sz) + tf_sz, (gy*tf_sz) + tf_sz);
		ver.texCoords = sf::Vector2f((tu + 1) * tile_size, (tv + 1) * tile_size);
		grid_array.append(ver);

		ver.position = sf::Vector2f(gx*tf_sz, (gy*tf_sz) + tf_sz);
		ver.texCoords = sf::Vector2f(tu * tile_size, (tv + 1) * tile_size);
		grid_array.append(ver);

	}
}

