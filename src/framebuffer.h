#include <vector>
#include <string>
#include "vec3.h"
#pragma once

class Framebuffer {
    public:
        Framebuffer(int w, int h) : width(w), height(h) {
            fb.resize( width * height );
        }

        void clear();

        void setPixel( int i, int j, const vec3& c );

        void exportAsPNG( std::string filename );

        private:

        std::vector< vec3 > fb;
        int width, height;
};