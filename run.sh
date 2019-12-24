#!/usr/bin/env bash
g++ snek.cpp -o snek -lsfml-graphics -lsfml-system -lsfml-window -lpthread -lX11 -std=c++14 -g
./snek
