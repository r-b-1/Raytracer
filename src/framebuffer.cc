#include "rtweekend.h"
#include "framebuffer.h"
#include "png++/png.hpp"

#include <algorithm>

void Framebuffer::clear() {
    for (vec3& pixel : fb) {
        pixel = vec3(0, 0, 0);
    }
}

void Framebuffer::setPixel(int i, int j, const vec3& c) {
    fb[j * width + i] = c;
}

void Framebuffer::exportAsPNG(std::string filename) {
    png::image<png::rgb_pixel> imData(width, height);
    for (size_t y = 0; y < imData.get_height(); y++) {
        height = imData.get_height();
        std::clog << "\rScanlines remaining: " << (height - y) << ' ' << std::flush;
        for (size_t x = 0; x < imData.get_width(); x++) {
            const vec3& pixel = fb[y * width + x];

            auto r = static_cast<png::byte>(std::clamp(pixel.x(), 0.0f, 1.0f) * 255.0f);
            auto g = static_cast<png::byte>(std::clamp(pixel.y(), 0.0f, 1.0f) * 255.0f);
            auto b = static_cast<png::byte>(std::clamp(pixel.z(), 0.0f, 1.0f) * 255.0f);

            imData[y][x] = png::rgb_pixel(r, g, b);
        }
    }

    imData.write(filename);
}
