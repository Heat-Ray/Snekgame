#!/usr/bin/env bash
sudo apt-get update
sudo apt install build-essential
sudo apt install libsfml-dev
g++ snek.cpp -o snek -lsfml-graphics -lsfml-system -lsfml-window -lpthread -lX11 -std=c++14 -g
chmod +x run.sh
