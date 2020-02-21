CFLAGS=$(shell sdl2-config --cflags)
LDFLAGS=$(shell sdl2-config --libs)
OBJ=main.o render.o bowl.o map.o

bowltrack: $(OBJ)
	g++ -o bowltrack $(OBJ) $(LDFLAGS)

clean:
	-rm $(OBJ)
