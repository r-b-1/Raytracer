#include "framebuffer.h"

#include <iostream>
#include "png++/png.hpp"

#include <algorithm>

void Framebuffer::clear() {
    for (Vec3& pixel : fb) {
        pixel = Vec3(0, 0, 0);
    }
}

void Framebuffer::setPixel(int i, int j, const Vec3& c) {
    fb[j * width + i] = c;
}

void Framebuffer::exportAsPNG(std::string filename) {
    
    png::image<png::rgb_pixel> imData(width, height);
    for (size_t y = 0; y < imData.get_height(); y++) {
        const size_t remaining = imData.get_height() - y;
        std::clog << "\rEncoding PNG rows: " << remaining << ' ' << std::flush;
        for (size_t x = 0; x < imData.get_width(); x++) {
            const Vec3& pixel = fb[y * width + x];

            auto r = static_cast<png::byte>(std::clamp(pixel.x(), 0.0f, 1.0f) * 255.0f);
            auto g = static_cast<png::byte>(std::clamp(pixel.y(), 0.0f, 1.0f) * 255.0f);
            auto b = static_cast<png::byte>(std::clamp(pixel.z(), 0.0f, 1.0f) * 255.0f);

            imData[y][x] = png::rgb_pixel(r, g, b);
        }
    }
    std::clog << '\n';

    imData.write(filename);
}
