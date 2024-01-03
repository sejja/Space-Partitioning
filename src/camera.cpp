//
//	Camera.cpp
//	Space Partitioning
//
//	Created by Diego Revilla on 07/09/23
//	Copyright © 2023. All Rights reserved
//

#include "camera.hpp"

// ------------------------------------------------------------------------
/*! Upadte
*
*   Updates the Perspective Matrix
*/ //----------------------------------------------------------------------
void camera::update() {
    mPerspectiveMatrix = glm::lookAt(mPosition, mTarget, glm::vec3(0.f, 1.f, 0.f));
}

// ------------------------------------------------------------------------
/*! Set Position
*
*   Sets the position of the camera
*/ //----------------------------------------------------------------------
void camera::set_position(vec3 const& pos) {
    mPosition = pos;
    update();
}

// ------------------------------------------------------------------------
/*! Set Projection
*
*   Sets the projection of the camera
*/ //----------------------------------------------------------------------
void camera::set_projection(float fov_deg, glm::ivec2 const& window_size, float near, float far) {
    mProjectionMatrix = glm::perspective(glm::radians(fov_deg), window_size.x / static_cast<float>(window_size.y) , near, far);
}

// ------------------------------------------------------------------------
/*! Set Target
*
*   Sets the camera's target
*/ //----------------------------------------------------------------------
void camera::set_target(vec3 const& target) {
    mTarget = target;
    update();
}

// ------------------------------------------------------------------------
/*! Get Camera Matrix
*
*   Returns the Perspective Matrix
*/ //----------------------------------------------------------------------
glm::mat4 camera::GetCameraMatrix() const {
    return mPerspectiveMatrix;
}

// ------------------------------------------------------------------------
/*! Get Projection Matrix
*
*   Returns the Projection Matrix
*/ //----------------------------------------------------------------------
glm::mat4 camera::GetProjectionMatrix() const {
    return mProjectionMatrix;
}
