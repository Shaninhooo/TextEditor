all:
	g++ main.cpp src/PieceTable.cpp  src/FileManager.cpp src/RateLimiter.cpp -o main `sdl2-config --cflags --libs` -lSDL2_ttf

