cmake_minimum_required(VERSION 3.17)
project(Battleship C)

set(CMAKE_C_STANDARD 99)

add_executable(Battleship main.c manager.c manager.h player.c player.h game.c game.h)