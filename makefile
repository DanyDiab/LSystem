generator : ./scripts/generator.cpp
	g++ ./scripts/generator.cpp -o generator -lglut -lGL -lGLU


debug : ./scripts/generator.cpp
	g++ ./scripts/generator.cpp -g -o debug -lglut -lGL -lGLU