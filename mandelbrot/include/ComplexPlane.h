#ifndef COMPLEX_PLANE_H
#define COMPLEX_PLANE_H

#include <SFML/Graphics.hpp>
#include <sstream>
#include <complex>
#include <cmath>

const unsigned int MAX_ITER = 196;
const float BASE_WIDTH = 4.0;
const float BASE_HEIGHT = 4.0;
const float BASE_ZOOM = 0.5;

enum State { CALCULATING, DISPLAYING };

class ComplexPlane : public sf::Drawable {
    public:
        ComplexPlane(int pixelWidth, int pixelHeight);
        void zoom_in();
        void zoom_out();
        void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
        void set_center(sf::Vector2i mousePixel);
        void set_mouse_location(sf::Vector2i mousePixel);
        void load_text(sf::Text& text);
        void update_render(int start_row, int end_row);

        State get_state() const { return m_State; }
        void set_state(State state) { m_State = state; }
    
    private:
        sf::VertexArray m_vArray;
        State m_State;
        sf::Vector2f m_mouseLocation, m_plane_center, m_plane_size;
        sf::Vector2i m_pixel_size;
        int m_zoomCount;
        float m_aspectRatio;

        int count_iterations(sf::Vector2f coord);
        void iterations_to_rgb(size_t count, sf::Uint8& r, sf::Uint8& g, sf::Uint8& b);
        sf::Vector2f map_pixels_to_coords(sf::Vector2i mousePixel);

};

#endif
