#include <iostream>
#include "rtree.hpp"
#include "gui.hpp"
#include <fstream>

/*
Como correr:
    g++ -std=c++17 -g -c main.cpp &&  g++ -g main.o -o sfml-app -lsfml-graphics -lsfml-audio -lsfml-window -lsfml-system && ./sfml-app
*/

int main()
{
    Gui gui;
    while (gui.window.isOpen())
    {
        gui.EventHandler();

        gui.window.clear();

        gui.draw_layout();

        gui.draw();

        gui.window.display();
    }
}