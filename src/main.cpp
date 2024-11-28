#include "../include/ComplexPlane.h"
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>

int main() {
    int screen_width  = sf::VideoMode::getDesktopMode().width,
        screen_height = sf::VideoMode::getDesktopMode().height;

    ComplexPlane plot(screen_width, screen_height);
    sf::VideoMode video(screen_width, screen_height);
    sf::RenderWindow window(video, "Mandelbrot Set",  sf::Style::Default);

    sf::Font font;
    if (!font.loadFromFile("./FiraSans-Regular.ttf")) {
        std::cout << "Failed to load font file, aborting program..." << std::endl;
        return -1;
    }
    sf::Text text("", font, 25);

    /* Configure multithreading */
    std::vector<std::thread> thread_pool;
    int available_cores = std::thread::hardware_concurrency(),
        chunk_height    = screen_height / available_cores;

    bool display_text = true;


    while (window.isOpen()) {
        sf::Event event;
        //ComplexPlane rgb(screen_width, screen_height);
        //rgb.update_render(screen_width, screen_height);
        //window.draw(rgb);

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) { window.close(); }

            if (event.type == sf::Event::MouseButtonPressed) {
                plot.set_center({ event.mouseButton.x, event.mouseButton.y });
                if (event.mouseButton.button == sf::Mouse::Left) { plot.zoom_in(); } 
                else { plot.zoom_out(); }
            } 
            else if (event.type == sf::Event::MouseMoved) {
                plot.set_mouse_location({ event.mouseMove.x, event.mouseMove.y });
            }
            else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::T) {
                display_text = !display_text;
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) { window.close(); }
        }

        /*
         * Splits the screen into available_cores chunks along the vertical axis to speed up rendering.
         *
         * Since each thread focuses on its own chunk of plot.m_vArray, there is no race condition and
         * therefore does not need a mutex lock.
         */
        if (plot.get_state() == State::CALCULATING) {
            for (int i = 0; i < available_cores; i++) {
                int starting_row = i * chunk_height,
                    ending_row   = (i + 1) * chunk_height;

                thread_pool.emplace_back(&ComplexPlane::update_render, &plot, starting_row, ending_row);
            }

            for (std::thread& thread : thread_pool) {
                if (thread.joinable()) { thread.join(); }
            }

            thread_pool.clear();
            plot.set_state(State::DISPLAYING);
        }

        window.clear();
        window.draw(plot);

        if (display_text) {
            plot.load_text(text);
            window.draw(text);
        }

        window.display();
    }

    return 0;
}