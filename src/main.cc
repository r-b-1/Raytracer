#include "rtweekend.h"

#include "hittable.h"
#include "hittable_list.h"
#include "sphere.h"
// #include "framebuffer.h"

#include "camera.h"

int main() {
    // World
    hittable_list world;
    
    world.add(make_shared<sphere>(point3(0,0,-1), 0.5));
    world.add(make_shared<sphere>(point3(0,-100.5,-1),100));

    camera cam;

    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 2000;
    cam.samples_per_pixel = 100;

    // Render
    cam.render(world);

    // std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    
   //  framebuffer.clear();
   //  framebuffer.exportAsPNG("image.png");

}