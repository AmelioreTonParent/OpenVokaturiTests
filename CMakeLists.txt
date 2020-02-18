cmake_minimum_required(VERSION 3.10)

project(openvokaturitests LANGUAGES C)

add_executable(openvokaturitests main.c)

install(TARGETS openvokaturitests RUNTIME DESTINATION bin)
