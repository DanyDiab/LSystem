SRCGEN = ./scripts/generator.cpp
SRCREN = ./scripts/main.cpp ./scripts/Camera.cpp ./scripts/Shader.cpp ./scripts/Renderer.cpp ./scripts/Geometry.cpp ./scripts/turtle.cpp

LINKS = -fsanitize=address -g -lGLEW -lglut -lGL -lGLU

generator : $(SRCGEN)
	g++ $(SRCGEN) -o generator $(LINKS)

lsystem : $(SRCREN)
	g++ $(SRCREN) -o lsystem $(LINKS)

all  : $(SRCGEN) $(SRCREN)
	g++ $(SRCGEN) -o generator $(LINKS)
	g++ $(SRCREN) -o lsystem $(LINKS)
