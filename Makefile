raytracing.exe: main.cpp Bryly.cpp Wektor3D.cpp Matematyka.cpp
	g++ main.cpp Bryly.cpp Wektor3D.cpp Matematyka.cpp -o raytracing.exe -lsfml-graphics -lsfml-window -lsfml-system -O3