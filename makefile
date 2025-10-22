OBJ = ./build/obj/
SRC = ./src/
EXTERNAL = ./external/picohttpparser/

INCLUDE = -I./include/ -I./external/

OBJFILE = $(OBJ)main.o $(OBJ)server.o $(OBJ)picohttpparser.o $(OBJ)JsonPacketManager.o

all: $(OBJ) $(OBJFILE)
	g++ -std=c++11 $(OBJFILE) -o main.exe

# C++ файлы
$(OBJ)%.o: $(SRC)%.cpp
	g++ $(INCLUDE) -std=c++11 -c $< -o $@

# C файлы
$(OBJ)picohttpparser.o: $(EXTERNAL)picohttpparser.c
	gcc $(INCLUDE) -c $< -o $@

# Создание папки для объектов
$(OBJ):
	mkdir -p $(OBJ)

clean:
	rm -f $(OBJ)*.o
	rm -f *.exe

