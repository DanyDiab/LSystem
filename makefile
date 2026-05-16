SRCGEN = ./scripts/generator.cpp ./scripts/Util.cpp
SRCREN = ./scripts/main.cpp ./scripts/Camera.cpp ./scripts/Shader.cpp ./scripts/Renderer.cpp ./scripts/Geometry.cpp ./scripts/turtle.cpp

LINKS = -fsanitize=address -g -lGLEW -lglut -lGL -lGLU

OUTGEN = generator
OUTREN = renderer

generator : $(SRCGEN)
	g++ $(SRCGEN) -o $(OUTGEN) $(LINKS)

lsystem : $(SRCREN)
	g++ $(SRCREN) -o $(OUTREN) $(LINKS)

all  : $(SRCGEN) $(SRCREN)
	g++ $(SRCGEN) -o $(OUTGEN) $(LINKS)
	g++ $(SRCREN) -o $(OUTREN) $(LINKS)
clean :
	rm -rf $(OUTGEN) $(OUTREN)