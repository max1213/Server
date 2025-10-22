all:CMakeLists.txt
	cmake -S . -B build
	cmake --build build
clean:
	rm -rf build
	rm -rf *.exe

