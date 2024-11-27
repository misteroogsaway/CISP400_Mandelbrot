#include "../include/ComplexPlane.h"

ComplexPlane::ComplexPlane(int pixelWidth, int pixelHeight) {
    m_pixel_size = { pixelWidth, pixelHeight };
    m_aspectRatio = static_cast<float>(pixelHeight) / pixelWidth;
    m_plane_center = { 0, 0 };
    m_plane_size = { BASE_WIDTH, BASE_HEIGHT * m_aspectRatio };
    m_zoomCount = 0;
    m_State = State::CALCULATING;
    m_vArray.setPrimitiveType(sf::Points);
    m_vArray.resize(pixelWidth * pixelHeight);
}

void ComplexPlane::zoom_in() {
    m_zoomCount++;

    float x = BASE_WIDTH * (std::pow(BASE_ZOOM, m_zoomCount)),
          y = BASE_HEIGHT * m_aspectRatio * (std::pow(BASE_ZOOM, m_zoomCount));

    m_plane_size = { x, y };
    m_State = State::CALCULATING;
}

void ComplexPlane::zoom_out() {
    m_zoomCount--;

    float x = BASE_WIDTH * (std::pow(BASE_ZOOM, m_zoomCount)),
          y = BASE_HEIGHT * m_aspectRatio * (std::pow(BASE_ZOOM, m_zoomCount));

    m_plane_size = { x, y };
    m_State = State::CALCULATING;
}

void ComplexPlane::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    target.draw(m_vArray);
}

void ComplexPlane::set_center(sf::Vector2i mousePixel) {
    m_plane_center = map_pixels_to_coords(mousePixel);
    m_State = State::CALCULATING;
}

void ComplexPlane::set_mouse_location(sf::Vector2i mousePixel) {
    m_mouseLocation = map_pixels_to_coords(mousePixel);
}

void ComplexPlane::load_text(sf::Text& text) {
    std::ostringstream str;

    str << "[Center: (" << m_plane_center.x << ", " << m_plane_center.y << ")]" << std::endl
        << "[Cursor: (" << m_mouseLocation.x << ", " << m_mouseLocation.y << ")]" << std::endl
        << "Left-click to zoom in, right-click to zoom out." << std::endl
        << "Press 'T' to hide this text." << std::endl;
    
    text.setString(str.str());
}

void ComplexPlane::update_render(int start_row, int end_row) {
    int width = m_pixel_size.x;
    for (int i = start_row; i < end_row; i++) {
        for (int j = 0; j < width; j++) {
            m_vArray[j + i * width ].position = { (float)j, float(i) };
            sf::Vector2f coordinate = map_pixels_to_coords({ j, i });
            sf::Uint8 r, g, b;
            size_t iterations = count_iterations(coordinate);

            iterations_to_rgb(iterations, r, g, b);
            m_vArray[j + i * width].color = { r, g, b };
        }
    }
}

/* Private Functions */

int ComplexPlane::count_iterations(sf::Vector2f coord) {
    std::complex<double> c(coord.x, coord.y);
    std::complex<double> z(0, 0);

    size_t iterations = 0;

    /* 
     * std::norm beats out std::abs by 100-300ms at 196 iterations and by larger
     * amounts at higher iterations.
     */
    while (std::norm(z) <= 4 && iterations < MAX_ITER) {
        z = z * z + c;
        iterations++;
    }

    return iterations;
}

void ComplexPlane::iterations_to_rgb(size_t count, sf::Uint8& r, sf::Uint8& g, sf::Uint8& b) {

    if (count == MAX_ITER) { r = 0, g = 0, b = 0; }
    else if (count < 5) { r = 11, g = 14, b = 20;}
    else {
        float norm = static_cast<float>(count) / MAX_ITER;
        r = 150 * norm, g = 40 * norm, b = 60 * norm;
    }
}

sf::Vector2f ComplexPlane::map_pixels_to_coords(sf::Vector2i mousePixel) {
    float x_offset = m_plane_center.x - m_plane_size.x / 2.0f,
          y_offset = m_plane_center.y - m_plane_size.y / 2.0f,
          x = (float)(mousePixel.x) / m_pixel_size.x * m_plane_size.x,
          y = (float)(mousePixel.y - m_pixel_size.y) / (0 - m_pixel_size.y) * m_plane_size.y;

    sf::Vector2f mapped = { (x + x_offset), (y + y_offset) };
    return mapped;

}