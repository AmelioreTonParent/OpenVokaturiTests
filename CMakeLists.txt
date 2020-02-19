cmake_minimum_required(VERSION 3.10)

project(openvokaturitests LANGUAGES C)

# Add path to a FindOpenVokaturi.cmake file
#list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
# Try to import OpenVokaturi static library
#find_package(OpenVokaturi REQUIRED)

#add_library(OpenVokaturi STATIC IMPORTED)
#set_property(TARGET OpenVokaturi PROPERTY IMPORTED_LOCATION ${install_dir}/lib/open/linux/OpenVokaturi-3-3-linux64.o)

include_directories(${PROJECT_SOURCE_DIR}/api)
link_directories(${PROJECT_SOURCE_DIR}/lib/open/linux)

add_library(OpenVokaturi STATIC IMPORTED)
set_property(TARGET OpenVokaturi PROPERTY IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/lib/open/linux/libOpenVokaturi.o)


add_executable(openvokaturitests main.c)

target_link_libraries(openvokaturitests OpenVokaturi)
# to include math.h
target_link_libraries(openvokaturitests m)

install(TARGETS openvokaturitests RUNTIME DESTINATION bin)
