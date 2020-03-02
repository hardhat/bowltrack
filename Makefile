CFLAGS=$(shell sdl2-config --cflags)
LDFLAGS=$(shell sdl2-config --libs) -lSDL2_image
OBJ=main.o render.o bowl.o map.o export.o sprites.o

bowltrack: $(OBJ)
	g++ -o bowltrack $(OBJ) $(LDFLAGS)

clean:
	-rm $(OBJ)
