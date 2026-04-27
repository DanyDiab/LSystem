INCLUDES = -I./vendor
SRC = ./scripts/generator.cpp

generator : ./scripts/generator.cpp
	g++ $(INCLUDES) $(SRC) -o generator -lglut -lGL -lGLU

debug : ./scripts/generator.cpp
	g++ $(INCLUDES) $(SRC) -g -o debug -lglut -lGL -lGLU