#ifndef __CAMERA_HPP__
#define __CAMERA_HPP__

#include "math.hpp"
#include <glm/glm.hpp>

class camera
{
  private:
  public:
    void update();
    void set_projection(float fov_deg, glm::ivec2 const& window_size, float near, float far);
    void set_position(vec3 const&);
    void set_target(vec3 const&);
    glm::vec3 GetTarget() const {return mTarget;}
    glm::vec3 GetPosition() const { return mPosition; }
    glm::mat4 GetCameraMatrix() const;
    glm::mat4 GetProjectionMatrix() const;
    glm::vec3 mPosition;
  private:
    glm::mat4 mProjectionMatrix;
    glm::mat4 mPerspectiveMatrix;
    glm::vec3 mTarget;
};

#endif // __CAMERA_HPP__
