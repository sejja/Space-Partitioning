#ifndef __DEBUG_SYSTEM_HPP__
#define __DEBUG_SYSTEM_HPP__
#include "camera.hpp"
#include "primitive.hpp"

class debug_system
{
  private:
  public:
    debug_system(camera const*);

    void draw_point(vec3 pt, vec4 color);
    void draw_segment(vec3 s, vec3 e, vec4 color);
    void draw_triangle(vec3 a, vec3 b, vec3 c, vec4 color);
    void draw_aabb(vec3 c, vec3 size, vec4 color);
    void draw_plane(vec3 pt, vec3 n, float scale, vec4 color);
    void draw_sphere(vec3 c, float r, glm::vec4 color);
    void draw_frustum_lines(mat4 const& vp, vec4 color);

    void change_camera(camera const*);

private:
	camera const* mCamera;
    Primitive mPoint;
    Primitive mSegment;
    Primitive mTriangle;
    Primitive mAABB;
    Primitive mPlane;
    Primitive mSphere;
    Primitive mFrustum;
};

#endif // __DEBUG_SYSTEM_HPP__
