INC=-lGL -lglut
FLAGS=-O3
LIBFLAG=-O3
OBJ=nbody.o particles.o tiny.o SOIL.o image_helper.o image_DXT.o stb_image_aug.o shader.o camera.o glew.o

nbody: $(OBJ)
	g++ $^ -o $@ $(FLAGS) $(INC)

nbody.o: nbody.cpp
	g++ $< -c $(FLAGS)

particles.o: particles.cpp particles.h
	g++ $< -c $(FLAGS)

camera.o: camera.cpp camera.h
	g++ $< -c $(FLAGS)

tiny.o: external-files/tiny_obj_loader.cc
	g++ $< -c -o $@ $(LIBFLAG)

SOIL.o: external-files/SOIL/SOIL.c
	g++ $< -c $(LIBFLAG) -Wno-write-strings

image_helper.o: external-files/SOIL/image_helper.c
	g++ $< -c $(LIBFLAG) -Wno-write-strings

image_DXT.o: external-files/SOIL/image_DXT.c
	g++ $< -c $(LIBFLAG) -Wno-write-strings

stb_image_aug.o: external-files/SOIL/stb_image_aug.c
	g++ $< -c $(LIBFLAG) -Wno-write-strings

shader.o: shader.cpp shader.h
	g++ $< -c $(LIBFLAG)

glew.o: external-files/glew/glew.c external-files/glew/glew.h external-files/glew/wglew.h external-files/glew/glxew.h
	g++ -DGLEW_NO_GLU -DGLEW_STATIC -O2 -Wall -W -Iinclude -fPIC -o $@ -c $<

trace: nbody
	apitrace trace ./nbody
	qapitrace nbody.trace 
	rm *.trace

clean:
	rm $(OBJ) nbody
