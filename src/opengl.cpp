#include "opengl.hpp"
#include <iostream>
#include <cassert>
#include <lodepng.h>
#include <sstream>
/**
 * Takes an screenshot
 * @param width
 * @param height
 * @return
 */
std::vector<glm::vec<4, unsigned char>> take_screenshoot(unsigned width, unsigned height)
{
    std::vector<glm::vec<4, unsigned char>> result(width * height);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadBuffer(GL_BACK);
    glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, result.data());
    // Flip vertically (inefficiently)
    for (unsigned x = 0; x < width; ++x) {
        for (unsigned y = 0; y < height / 2; ++y) {
            std::swap(result.at(y * width + x), result.at((height - 1 - y) * width + x));
        }
    }
    return result;
}

/**
 * Saves an screenshoot to a BMP
 * @param width
 * @param height
 * @param filename
 * @return
 */
std::vector<glm::vec<4, unsigned char>> save_screenshoot(unsigned width, unsigned height, std::string const& filename)
{
    auto pixels = take_screenshoot(width, height);

    auto data = &(pixels.front()[0]);
    auto err  = lodepng::encode(filename, data, width, height);
    if (err) {
        std::stringstream ss;
        ss << "Lodepng failed to save file " << filename;
        throw std::runtime_error(ss.str());
    }
    return pixels;
}
