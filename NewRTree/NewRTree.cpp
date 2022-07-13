// NewRTree.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "rtree.h"
#include "gui.h"
#include <fstream>


int main()
{
    RTree r;
    r.insert({ {5,5}, {10,10}, {15,5} });
    r.insert({ {15,15}, {20,10}, {20,15} });
    r.insert({ {35,10}, {35,15}, {40,10} });
    r.insert({ {35,20}, {40,20}, {40,15} });
    r.insert({ {30,0}, {30,5}, {35,5}, {40, 0}, {35, 0} });
    r.insert({ {40,5}, {45,10}, {45,0} });
    r.insert({ {40,15}, {45,20}, {45,15} });
    r.insert({ {30,25}, {35,25}, {35,20} });
    r.insert({ {35,16}, {36,19} });
    r.insert({ {35,16}, {36,19} });
    r.insert({ {35,16}, {36,19} });
    r.insert({ {35,16}, {36,19} });
    r.print();

    r.erase({17,12}, {43,19});
    r.print();


    RTree a;
    a.insert({ {730, 429}, {752 ,429}, {752 ,436}, {730 ,436} });
    a.insert({ {357 ,99}, {370 ,99}, {370 ,106}, {357 ,106} });
    a.insert({ {235 ,276}, {254 ,276}, {254 ,282}, {235 ,282} });
    a.insert({ {473 ,477}, {484 ,477}, {484 ,482}, {473 ,482} });
    a.insert({ {85 ,72}, {93 ,72}, {93 ,81}, {85 ,81} });
    a.insert({ {413 ,166}, {423 ,166}, {423 ,178}, {413 ,178} });
    a.insert({ {433 ,394}, {453 ,394}, {453 ,404}, {433 ,404} });
    a.insert({ {493 ,7}, {514 ,7}, {514 ,12}, {493 ,12} });
    a.insert({ {542 ,313}, {558 ,313}, {558 ,324}, {542 ,324} });
    a.insert({ {268 ,343}, {283 ,343}, {283 ,352}, {268 ,352} });
    a.insert({ {156 ,32}, {166 ,32}, {166 ,44}, {156 ,44} });
    a.insert({ {673 ,73}, {695 ,73}, {695 ,87}, {673 ,87} });
    a.insert({ {319 ,179}, {328 ,179}, {328 ,185}, {319 ,185} });
    a.insert({ {108 ,203}, {119 ,203}, {119 ,208}, {108 ,208} });
    a.insert({ {461 ,0}, {474 ,0}, {474 ,6}, {461 ,6} });
    a.insert({ {739 ,239}, {747 ,239}, {747 ,247}, {739 ,247} });
    a.insert({ {454 ,126}, {470 ,126}, {470 ,137}, {454 ,137} });
    a.insert({ {611 ,258}, {625 ,258}, {625 ,263}, {611 ,263} });
    a.print();
    a.erase({ 50,50 }, { 150,150 });
    a.print();
    a.eraseRandom();
    a.print();

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
