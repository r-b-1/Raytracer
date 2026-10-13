

#include "shader.h"



class MirrorShader : public Shader {
  public:
    explicit MirrorShader(const color& baseColor_) : baseColor_(baseColor_) {}

    bool scatter(const ray& r_in, const HitStruct& rec, color& attenuation, ray& scattered)
    const override {
        vec3 reflected = reflect(r_in.direction(), rec.normal);
        scattered = ray(rec.p, reflected);
        attenuation = baseColor_;
        return true;
    }

  private:
    color baseColor_;
};

