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

# Put source code and source headers into variables
set(SOURCE_CODE
	src/external/stb_image.cpp
	src/ParameterSlider.cpp
	src/RenderWindow.cpp
	src/Texture.cpp
	src/MainWindow.cpp
    src/Shader.cpp
    src/Mesh.cpp
    src/Simulation.cpp
    src/PushButton.cpp
    src/UpdateTimer.cpp
    src/TextBox.cpp
	src/BoundaryCounter.cpp
)
set(SOURCE_HEADERS
	src/external/stb_image.h
	src/ParameterSlider.h
	src/RenderWindow.h
	src/Texture.h
	src/MainWindow.h
    src/Shader.h
    src/Mesh.h
    src/Simulation.h
    src/PushButton.h
    src/UpdateTimer.h
	src/TextBox.h
	src/BoundaryCounter.h
	src/constants.h
	src/structs.h
)


# Put the non-main source code into a static library
set(STATIC_LIB_NAME my_code)

add_library(${STATIC_LIB_NAME} STATIC
	${SOURCE_CODE}
	${SOURCE_HEADERS}
)

# Link the Qt6 libraries to the static library. The access should be public so that the executable can see them as well.
target_link_libraries(${STATIC_LIB_NAME}
	PUBLIC  Qt6::Core Qt6::Widgets Qt6::OpenGL Qt6::OpenGLWidgets Qt6::Widgets Qt6::Gui)

# Make a variable for the output executable name
set(EXECUTABLE_NAME "simProgram")

add_executable(${EXECUTABLE_NAME} src/main.cpp)

## Link the executable to the static library
target_link_libraries(${EXECUTABLE_NAME} PRIVATE ${STATIC_LIB_NAME})


# Define macros for file directories
target_compile_definitions(${STATIC_LIB_NAME} PRIVATE 
	SHADER_PATH="${CMAKE_INSTALL_PREFIX}/share/${PROJECT_NAME}/shaders/"
)
message("Using CMake Install Prefix")
message("${CMAKE_INSTALL_PREFIX}")

# INSTALL LIBRARY
install(TARGETS ${STATIC_LIB_NAME} DESTINATION lib)
install(FILES 
	${SOURCE_HEADERS}
DESTINATION include)

install(FILES
	src/shaders/fragShaderDomainMapping.frag
	src/shaders/vertexShaderA.vert
DESTINATION share/${PROJECT_NAME}/shaders)

# INSTALL EXECUTABLE
install(TARGETS ${EXECUTABLE_NAME} DESTINATION bin)
#install(TARGETS ${EXECUTABLE_NAME}
#	BUNDLE DESTINATION .
#	RUNTIME DESTINATION .
#)

# Qt dependency management?
#qt_generate_deploy_app_script(
#	TARGET ${EXECUTABLE_NAME}
#	OUTPUT_SCRIPT deploy_script
#	NO_TRANSLATIONS
#)
#install(SCRIPT ${deploy_script})

## CPACK
#include(InstallRequiredSystemLibraries)
#set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/License.txt")
#set(CPACK_PACKAGE_VERSION_MAJOR "0")
#set(CPACK_PACKAGE_VERSION_MINOR "1")
#include(CPack)



### Debug build with everything
##set(DEBUG "debug_build")
##add_executable(${DEBUG} src/main.cpp
## 	${SOURCE_CODE}
##  ${SOURCE_HEADERS}
##	)
##
##target_link_libraries(${DEBUG}
##	PUBLIC  Qt6::Core Qt6::Widgets Qt6::OpenGL Qt6::OpenGLWidgets Qt6::Widgets Qt6::Gui)