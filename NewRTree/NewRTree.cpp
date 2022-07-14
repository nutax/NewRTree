#include <iostream>
#include <random>
#include <chrono>
#include "rtree.h"
#include "gui.h"
#include <fstream>


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
