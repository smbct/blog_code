.PHONY: clean
CC= g++
CFLAGS= -std=c++11 -pedantic -Wfatal-errors -Wconversion -Wredundant-decls -Wshadow -Wall -Wextra -O3 -lgcc_s -lgcc

OBJECTS_FILES= main.o WordCloud.o

SFML_OBJ= -lsfml-system -lsfml-window -lsfml-graphics
OSI_OBJ= -lOsi -lOsiClp -lClp -lCoinUtils -lm

prog: $(OBJECTS_FILES)
	$(CC) $(CFLAGS) $(OBJECTS_FILES) $(SFML_OBJ) $(OSI_OBJ) -o prog

# compilation des fichiers objets
%.o: %.cpp %.hpp
	$(CC) $(CFLAGS) $< -c -o $@

%.o: %.cpp
	$(CC) $(CFLAGS) $< -c -o $@

run:
	./prog

clean:
	@rm -rf *.o prog

# usr/local/[lib - include] pour installer la sfml
