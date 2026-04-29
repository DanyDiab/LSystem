INCLUDES = -I./vendor
SRCGEN = ./scripts/generator.cpp
SRCREN = ./scripts/renderer.cpp

LINKS = -lGLEW -lglut -lGL -lGLU

generator : $(SRCGEN)
	g++ $(INCLUDES) $(SRCGEN) -o generator

renderer : $(SRCREN)
	g++ $(INCLUDES) $(SRCREN) -o renderer $(LINKS)

debug : .$(SRC)
	g++ $(INCLUDES) $(SRCGEN) -g -o debug $(LINKS)