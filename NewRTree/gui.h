#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/System.hpp>
#include "hrtree.h"

class Gui
{
private:
	sf::Color guiColor = sf::Color(250, 250, 250);

	const int width = 800, height = 500;
    const int vertical_offset = 150;
    const int horizontal_offset = 50;
    const sf::Color colors[5] = {
        sf::Color(0, 255, 255),
        sf::Color(255, 0, 255),
        sf::Color(255, 255, 0),
        sf::Color(255, 0, 0),
        sf::Color(0, 255, 0)
    };
	
    bool HILBERT_MODE = false;
    int eraser_treshold = 5;
    int knn = 0;
	
    std::vector<sf::Shape*> points;
	
    
	
    RTree r_tree;
    HRTree h_tree;
	
public:
    sf::RenderWindow window;
	Gui():h_tree(width, height) {
		window.create(sf::VideoMode(width, height), "R-Tree Eren la Gaviota");
	}

    void draw_layout() {

        sf::Texture t = sf::Texture();
        t.loadFromFile("C:/Users/luis/source/repos/NewRTree/resources/background.jpg");
        sf::Sprite fun_background(t);
        fun_background.setScale({ width / fun_background.getLocalBounds().width, height / fun_background.getLocalBounds().height });


        sf::Texture t2 = sf::Texture();
        t2.loadFromFile("C:/Users/luis/source/repos/NewRTree/resources/jedi.jpeg");
        sf::Sprite good_background(t2);
        good_background.setScale({ width / good_background.getLocalBounds().width, height / good_background.getLocalBounds().height });

        sf::Texture t3 = sf::Texture();
        t3.loadFromFile("C:/Users/luis/source/repos/NewRTree/resources/sith.jpg");
        sf::Sprite bad_background(t3);
        bad_background.setScale({ width / bad_background.getLocalBounds().width, height / bad_background.getLocalBounds().height });


        /*if (FUN_MODE) {
            window.draw(fun_background);
            if (music.getStatus() != sf::SoundSource::Status(2)) {
                music.play();
            }
        }
        else {
            music.stop();
            if (HILBERT_MODE) window.draw(bad_background);
            else window.draw(good_background);
        }*/


        sf::RectangleShape aplha(sf::Vector2f(width, height));
        aplha.setFillColor(sf::Color(0, 0, 0, 130));
        aplha.setOutlineColor(guiColor);
        window.draw(aplha);
        sf::RectangleShape rectangle1(sf::Vector2f(width - horizontal_offset, height - vertical_offset));
        rectangle1.setPosition(sf::Vector2f(horizontal_offset / 2, vertical_offset / 2));
        rectangle1.setFillColor(sf::Color::Transparent);
        rectangle1.setOutlineThickness(5);
        window.draw(rectangle1);

        sf::Text title;
        sf::Text info;
        sf::Font font;
        if (!font.loadFromFile("C:/Users/luis/source/repos/NewRTree/resources/ComicMono.ttf"))
        {
            // cout << "can't load font" << endl;
        }

        info.setFont(font);
        title.setFont(font);
        info.setString("Use el \"LEFT CLICK\" para dibujar puntos y \"ENTER\" para unirlos. \nPuede modificar el K de KNN con \"ESPACIO\".\nCualquier bug o duda, comunicarse con jose.huby@utec.edu.pe");
        if (HILBERT_MODE) title.setString("Hilbert-Tree by Eren la Gaviota");
        else title.setString("R-Tree by Eren la Gaviota");
        info.setPosition(sf::Vector2f(horizontal_offset / 2, height - vertical_offset / 2 + 10));
        title.setPosition(sf::Vector2f(horizontal_offset / 2, 0));
        info.setFillColor(guiColor);
        title.setFillColor(guiColor);
        // set the character size
        info.setCharacterSize(12); // in pixels, not points!
        title.setCharacterSize(44); // in pixels, not points!

        window.draw(info);
        window.draw(title);
    }

    void draw_poly(const Poly& points) {

        int size = points.size();

        sf::Vertex a;
        sf::Vertex b;
        for (int i = 0; i < size - 1; i++) {
            const sf::Vertex line[] = {
                    sf::Vertex(sf::Vector2f(points[i].x, points[i].y)),
                    sf::Vertex(sf::Vector2f(points[i + 1].x, points[i + 1].y))
            };
            int radius = 2;
            sf::CircleShape shape(2);
            shape.setPosition(sf::Vector2f(points[i].x - radius, points[i].y - radius));
            window.draw(line, 2, sf::Lines);
            window.draw(shape);
        }

        sf::Vertex line[] = {
                sf::Vertex(sf::Vector2f(points[size - 1].x, points[size - 1].y)),
                sf::Vertex(sf::Vector2f(points[0].x, points[0].y))
        };


        int radius = 2;
        sf::CircleShape shape(2);
        shape.setPosition(sf::Vector2f(points[size - 1].x - radius, points[size - 1].y - radius));
        window.draw(line, 2, sf::Lines);
        window.draw(shape);
    }
    
    void draw_box(const MBB& box, int lvl, int height) {
        sf::Color const& color = colors[lvl % 5];
        int offset = (lvl < height) * 2 * (height - lvl);

        auto aX = box.min.x - (offset);
        auto aY = box.min.y - (offset);

        auto bX = box.max.x + (offset);
        auto bY = box.max.y + (offset);

        sf::Vertex line1[] = {
                sf::Vertex(sf::Vector2f(aX, aY), color),
                sf::Vertex(sf::Vector2f(aX, bY), color)
        };
        window.draw(line1, 2, sf::Lines);

        sf::Vertex line2[] = {
                sf::Vertex(sf::Vector2f(aX, aY), color),
                sf::Vertex(sf::Vector2f(bX, aY), color)
        };
        window.draw(line2, 2, sf::Lines);

        sf::Vertex line3[] = {
                sf::Vertex(sf::Vector2f(aX, bY), color),
                sf::Vertex(sf::Vector2f(bX, bY), color)
        };
        window.draw(line3, 2, sf::Lines);

        sf::Vertex line4[] = {
                sf::Vertex(sf::Vector2f(bX, aY), color),
                sf::Vertex(sf::Vector2f(bX, bY), color)
        };
        window.draw(line4, 2, sf::Lines);

    }

	
    void draw() {
        for (auto it = points.begin();it != points.end();it++)
        {
            window.draw(**it);
        }

        r_tree.forEachPoly([this](const Poly& points) { this->draw_poly(points); });
		if (HILBERT_MODE) h_tree.forEachMBB([this](const MBB& mbb, int level, int height) { this->draw_box(mbb, level, height); });
        else r_tree.forEachMBB([this](const MBB& mbb, int level, int height) { this->draw_box(mbb, level, height); });
		
        auto pos = sf::Mouse::getPosition(window);
        if (knn) {
            if (HILBERT_MODE) h_tree.forEachNearest(knn, { (float)pos.x, (float)pos.y }, [this](const Poly& poly, Vec2 source, Vec2 target, int distance) { this->draw_nearest(poly, target, source, distance); });
            else r_tree.forEachNearest(knn, { (float)pos.x, (float)pos.y }, [this](const Poly& poly, Vec2 source, Vec2 target, int distance) { this->draw_nearest(poly, target, source, distance); });
        } 
    }
	
    void draw_nearest(Poly const& points, Vec2 const& source, Vec2 const& target, int distance) {
        sf::Color color(250, 0, 0);
        int size = points.size();

        sf::Vertex a;
        sf::Vertex b;

        for (int i = 0; i < size - 1; i++) {
            const sf::Vertex line[] = {
                    sf::Vertex(sf::Vector2f(points[i].x, points[i].y), color),
                    sf::Vertex(sf::Vector2f(points[i + 1].x, points[i + 1].y), color)
            };
            int radius = 2;
            sf::CircleShape shape(2);
            shape.setPosition(sf::Vector2f(points[i].x - radius, points[i].y - radius));
            window.draw(line, 2, sf::Lines);
            window.draw(shape);
        }

        sf::Vertex line[] = {
                sf::Vertex(sf::Vector2f(points[size - 1].x, points[size - 1].y), color),
                sf::Vertex(sf::Vector2f(points[0].x, points[0].y), color)
        };


        int radius = 2;
        sf::CircleShape shape(2);
        shape.setPosition(sf::Vector2f(points[size - 1].x - radius, points[size - 1].y - radius));
        window.draw(line, 2, sf::Lines);
        window.draw(shape);

        sf::Vertex la_linea[] = {
                sf::Vertex(sf::Vector2f(source.x, source.y), color),
                sf::Vertex(sf::Vector2f(target.x, target.y), color)
        };
        window.draw(la_linea, 2, sf::Lines);

    }
	
    void EventHandler()
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            switch (event.type) {
                case sf::Event::Closed:
                {
                    window.close();
                    exit(0);
                }

                case sf::Event::MouseButtonPressed:
                {

                    MouseEvent(event);
                    break;
                }
                
                case sf::Event::KeyReleased:
                {
                    KeyReleasedEvent(event);
                    break;
                }
            }
        }
    }
    void KeyReleasedEvent(sf::Event& event)
    {
        switch (event.key.code)
        {
        case sf::Keyboard::F:
        {
            //FUN_MODE = !FUN_MODE;
            break;
        }
        case sf::Keyboard::H:
        {
            HILBERT_MODE = !HILBERT_MODE;
            break;
        }
        case sf::Keyboard::Enter:
        {
            Poly polygon;
            for (int i = 0; i < points.size(); i++)
            {
                sf::Vector2f pos = points[i]->getPosition();
                polygon.push_back({ pos.x, pos.y });
            }
            points.clear();
            r_tree.insert(polygon);
            h_tree.insert(polygon);
            h_tree.print();
            break;
        }

        case sf::Keyboard::Space:
        {

            if (knn < r_tree.size()) knn++;
            else knn = 0;
            break;
        }
        }
    }
    void MouseEvent(sf::Event& event)
    {
        if ((event.mouseButton.x > horizontal_offset / 2 && event.mouseButton.x < width - horizontal_offset / 2
            && event.mouseButton.y > vertical_offset / 2 && event.mouseButton.y < height - vertical_offset / 2)) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                int radius = 2;
                sf::CircleShape* shape = new sf::CircleShape(radius);
                shape->setPosition(event.mouseButton.x - radius, event.mouseButton.y - radius);
                shape->setFillColor(sf::Color(250, 250, 250));
                points.push_back(shape);
            }
            else if (event.mouseButton.button == sf::Mouse::Right) {
                r_tree.erase({ (float)event.mouseButton.x - eraser_treshold, (float)event.mouseButton.y - eraser_treshold },
                    { (float)event.mouseButton.x + eraser_treshold, (float)event.mouseButton.y + eraser_treshold });
            }
        }
    }
};

