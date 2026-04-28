INCLUDES = -I./vendor
SRCGEN = ./scripts/generator.cpp
SRCREN = ./scripts/renderer.cpp

generator : $(SRCGEN)
	g++ $(INCLUDES) $(SRCGEN) -o generator

renderer : $(SRCREN)
	g++ $(INCLUDES) $(SRCREN) -o renderer -lglut -lGL -lGLU

debug : .$(SRC)
	g++ $(INCLUDES) $(SRCGEN) -g -o debug -lglut -lGL -lGLU