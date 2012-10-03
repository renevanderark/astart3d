sources	= datastruct.cpp geo.cpp image.cpp grid.cpp person.cpp main.cpp
objects	= datastruct.o geo.o image.o grid.o person.o main.o

all: a.out

a.out: $(objects)
	g++ -o a.out $(objects) -lGL -lGLU -lGLEW -lglut
	rm $(objects)

$(objects): $(sources)
	g++ -c $(sources)

clean:
	rm a.out $(objects)

