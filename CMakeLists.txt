cmake_minimum_required(VERSION 3.0)

set(CMAKE_BUILD_TYPE None)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11 -fpermissive -g -w -Wl,-rpath,\"\$ORIGIN\"")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11 -fpermissive -g -w")

set(PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/FlumenGame)

file(GLOB_RECURSE SOURCE_FILES
	"${PROJECT_SOURCE_DIR}/*"
)

include_directories(/home/andrei/Documents/Code/Libraries/glm)
include_directories(/home/andrei/Documents/Code/Libraries/FlumenCore)
include_directories(/home/andrei/Documents/Code/Libraries/FlumenEngine)
include_directories(/usr/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

link_directories(/home/andrei/Documents/Code/Libraries/FlumenCore)
link_directories(/home/andrei/Documents/Code/Libraries/FlumenEngine)

project (Game)
 
add_executable(Game ${SOURCE_FILES})

target_link_libraries(Game GL GLEW pthread SDL2 SDL2_ttf SDL2_image Core Engine)