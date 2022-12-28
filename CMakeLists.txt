cmake_minimum_required(VERSION 3.15) # Проверка версии CMake.

set(PROJECT_NAME Snake)	 
project("${PROJECT_NAME}")


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)


add_definitions(-DSFML_STATIC)

include_directories("ext/SFML/include/")
include_directories("inc/")
include_directories("src/")
link_directories("ext/SFML/lib/")	

add_executable("${PROJECT_NAME}" "main.cpp" "src/Game.cpp" "inc/Game.hpp" "src/Snake.cpp" "inc/Snake.hpp" "inc/Random.hpp" "src/Random.cpp")
											
target_link_libraries("${PROJECT_NAME}" 
					    debug sfml-graphics-s-d 
						debug sfml-window-s-d
						debug sfml-system-s-d
						debug sfml-audio-s-d)

target_link_libraries("${PROJECT_NAME}" 
					    optimized sfml-graphics-s 
						optimized sfml-window-s
						optimized sfml-system-s
						optimized sfml-audio-s
                                                openal32)

target_link_libraries("${PROJECT_NAME}" opengl32 freetype winmm gdi32 openal32 flac vorbisenc vorbisfile vorbis ogg)
file(COPY "data" DESTINATION "/")
file(COPY "openal32.dll" DESTINATION "/")