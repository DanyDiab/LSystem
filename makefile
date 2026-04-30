SRCGEN = ./scripts/generator.cpp
SRCREN = ./scripts/renderer.cpp ./scripts/turtle.cpp

LINKS = -fsanitize=address -g -lGLEW -lglut -lGL -lGLU

generator : $(SRCGEN)
	g++ $(SRCGEN) -o generator $(LINKS)

renderer : $(SRCREN)
	g++ $(SRCREN) -o renderer $(LINKS)

all  : $(SRCGEN) $(SRCREN)
	g++ $(SRCGEN) -o generator $(LINKS)
	g++ $(SRCREN) -o renderer $(LINKS)