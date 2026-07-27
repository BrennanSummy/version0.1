cmake_minimum_required(VERSION 4.3.0)

project(Adsorption_Simulation
    VERSION 0.1
	DESCRIPTION "A simulation project with a great CMake file"
	LANGUAGES CXX)

# Flags to turn on Undefined Behavior Sanitizer
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined -fno-sanitize-recover=all")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=undefined")

# Turn on Auto Meta-Object-Compiler for Qt. This searches for macros like Q_OBJECT and flags files accordingly for MOC processing
set(CMAKE_AUTOMOC ON)
# Find Qt6 installation on this device
find_package(Qt6 REQUIRED COMPONENTS Core Widgets OpenGLWidgets OpenGL Gui)

# Put the non-main source code into a static library
set(STATIC_LIB_NAME my_code)
add_library(${STATIC_LIB_NAME} STATIC
	src/external/stb_image.cpp
	src/external/stb_image.h
	src/ParameterSlider.cpp
	src/ParameterSlider.h
	src/RenderWindow.cpp
	src/RenderWindow.h
	src/Texture.cpp
	src/Texture.h
	src/MainWindow.cpp
	src/MainWindow.h
    src/Shader.h
    src/Shader.cpp
    src/Mesh.h
    src/Mesh.cpp
    src/Simulation.cpp
    src/Simulation.h
    src/PushButton.cpp
    src/PushButton.h
    src/UpdateTimer.cpp
    src/UpdateTimer.h
)

# Link the Qt6 libraries to the static library that I am building. The access should be public so that the executable can see them as well.
target_link_libraries(${STATIC_LIB_NAME}
	PUBLIC  Qt6::Core Qt6::Widgets Qt6::OpenGL Qt6::OpenGLWidgets Qt6::Widgets Qt6::Gui)

# Make a variable for the output executable name
set(EXECUTABLE_NAME "runner")

add_executable(${EXECUTABLE_NAME} src/main.cpp)
## Link the executable to the static library
target_link_libraries(${EXECUTABLE_NAME} PRIVATE ${STATIC_LIB_NAME})
