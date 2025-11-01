cmake_minimum_required(VERSION 1.00) #показывает какой минимальной версией можно собрать проект

set(PROJECT_NAME Server)
project(${PROJECT_NAME})

#настраиваем стандарты языка
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

#настраиваем места хранения скомпилированных файлов
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/release)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}/debug)

#даем названия подпроектам 
set(PROG_LIBRARY "${PROJECT_NAME}") 

#сораняем путь до директории include нашими хедарами 
set(PROG_INCLUDE 
	"${CMAKE_CURRENT_SOURCE_DIR}/include"
	"${CMAKE_CURRENT_SOURCE_DIR}/external"
)
#добавляет путь до нашей директории в область видимости всех проектов
include_directories(${PROG_INCLUDE})

file(GLOB_RECURSE NET "src/net/**/*.cpp")
file(GLOB APP "src/app/*.cpp")
file(GLOB picohttpparser "external/picohttpparser/*.c")

add_executable(main.exe
	${APP}
    	${NET}
   	 ${picohttpparser}
    	src/main.cpp
)



