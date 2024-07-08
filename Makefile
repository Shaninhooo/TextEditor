all:
	g++ main.cpp src/PieceTable.cpp -o main `sdl2-config --cflags --libs` -lSDL2_ttf


test: 
	g++ test.cpp -o test `sdl2-config --cflags --libs` -lSDL2_ttf