# CMakeList.txt: proyecto de CMake para CleanWave, incluya el origen y defina
# la lógica específica del proyecto aquí.
#

# Agregue un origen al ejecutable de este proyecto.
add_executable (CleanWave  "main.cpp" "public/ConsoleUtils.h" "public/Colors.h" "public/BlockList.h" "private/BlockList.cpp" "public/CWP.h" "private/CWP.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET CleanWave PROPERTY CXX_STANDARD 20)
endif()

if(WIN32)
	target_link_libraries(CleanWave "ws2_32.lib")
endif()

set("PUBLIC_INCLUDES" "${CMAKE_CURRENT_SOURCE_DIR}/public")
target_include_directories(CleanWave PUBLIC "${PUBLIC_INCLUDES}")
