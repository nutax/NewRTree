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
        /*

        hilbert_tree.for_each_polygon(draw_poly);

        auto pos = sf::Mouse::getPosition(window);

        if (variable)
            hilbert_tree.for_each_nearest(variable, { pos.x, pos.y }, draw_nearest);

        if (HILBERT_MODE) hilbert_tree.for_each_box(draw_box_hilbert);
        else r_tree.for_each_box(draw_box_r);

        if ((sf::Mouse::getPosition(window).x > horizontal_offset / 2 && sf::Mouse::getPosition(window).x < width - horizontal_offset / 2
            && sf::Mouse::getPosition(window).y > vertical_offset / 2 && sf::Mouse::getPosition(window).y < height - vertical_offset / 2)) {
            int radius = 4;
            sf::CircleShape* shape = new sf::CircleShape(radius);
            shape->setPosition(sf::Mouse::getPosition(window).x - radius, sf::Mouse::getPosition(window).y - radius);
            shape->setFillColor(sf::Color(250, 250, 250));
            window.draw(*shape);
            window.setMouseCursorVisible(false);
        }
        else {
            window.setMouseCursorVisible(true);
        }*/


        gui.window.display();



    }
}
