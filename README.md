# Rogue-Like
My attempt at creating a procedurally generated rogue-like

The program involves the generation of a 2D tilemap engine (thanks to SFML vertex arrays). Each room is then randomly generated and then mazes are constructed using a growing tree algorithm in all non room areas. Each maze and room is counted as a region that are then all connected one by one. Finally all dead ends are removed from the map; resulting in a beautiful and windy maze that does not look ugly or jagged at all.
