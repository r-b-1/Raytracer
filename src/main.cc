#include "rtweekend.h"

#include "hittable.h"
#include "hittable_list.h"
#include "sphere.h"
// #include "framebuffer.h"
#include "scene.h"
#include "triangle.h"

#include "camera.h"
#include "perspective_camera.h"
#include "orthographic_camera.h"

int main() {
    PerspectiveCamera cam;

    Scene scene;

    cam.lookfrom = point3(0, 0, 0);
    cam.lookat = point3(0, 0, -6);

    scene.objects.push_back(std::make_shared<sphere>(point3(0, 0, -6), 0.5));

    scene.objects.push_back(std::make_shared<triangle>(point3(-1.2, -0.2, -7), point3(0.8, -0.5, -5), point3(0.9, 0, -5)));
    scene.objects.push_back(std::make_shared<triangle>(point3(0.773205, -0.93923, -7), point3(0.0330127, 0.94282, -5), point3(-0.45, 0.779423, -5)));
    scene.objects.push_back(std::make_shared<triangle>(point3(0.426795, 1.13923, -7), point3(-0.833013, -0.44282, -5 ), point3(-0.45, -0.779423, -5)));    

    cam.render(scene, "scene_perspective.png");

    OrthographicCamera ortho;
    ortho.lookfrom = cam.lookfrom;
    ortho.lookat = cam.lookat;
    ortho.vup = cam.vup;
    ortho.image_width = cam.image_width;
    ortho.aspect_ratio = cam.aspect_ratio;
    ortho.render(scene, "scene_orthographic.png");    

    



    

}