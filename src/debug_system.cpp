//
//	DebugSystem.cpp
//	Space Partitioning
//
//	Created by Diego Revilla on 08/09/23
//	Copyright © 2023. All Rights reserved
//

#include <stdexcept>
#include "debug_system.hpp"

namespace {
    char const* c_vertex_shader = R"(
#version 440 core
layout(location = 0) in vec3 attr_position;
layout(location = 0) uniform mat4 uniform_mvp;
void main()
{
    vec4 vertex = vec4(attr_position, 1.0f);
    gl_Position = uniform_mvp * vertex;
}
)";

    char const* c_fragment_shader = R"(
#version 440 core
out vec4 out_color;
layout(location = 1) uniform vec4 uniform_color;
void main()
{
    out_color = uniform_color;
}
)";
}

// ------------------------------------------------------------------------
/*! Upadte
*
*   Updates the Perspective Matrix
*/ //----------------------------------------------------------------------
debug_system::debug_system(camera const * c) {
    mCamera = c;

    mPoint.InitPoint();
    mPoint.InitializeBuffers();
    mPoint.LoadShader(c_vertex_shader, c_fragment_shader);

    //Segment
    mSegment.InitSegment();
    mSegment.InitializeBuffers();
    mSegment.LoadShader(c_vertex_shader, c_fragment_shader);

    //Triangle
    mTriangle.LoadShader(c_vertex_shader, c_fragment_shader);

    //AABB
    mAABB.InitAABB();
    mAABB.InitializeBuffers();
    mAABB.LoadShader(c_vertex_shader, c_fragment_shader);

    //Plane
    mPlane.InitPlane();
    mPlane.InitializeBuffers();
    mPlane.LoadShader(c_vertex_shader, c_fragment_shader);

    //Sphere (just 1 circle)
    mSphere.InitSphereDisc();
    mSphere.InitializeBuffers();
    mSphere.LoadShader(c_vertex_shader, c_fragment_shader);

    //Frustum
    mFrustum.InitFrustum();
    mFrustum.InitializeBuffers();
    mFrustum.LoadShader(c_vertex_shader, c_fragment_shader);
}

// ------------------------------------------------------------------------
/*! Upadte
*
*   Updates the Perspective Matrix
*/ //----------------------------------------------------------------------
void debug_system::draw_point(vec3 pt, vec4 color) {
    //Enable shader
    mPoint.mShader->Bind();

    //Set uniforms:
    {
        //Build the model to viewport matrix
        //Model->world: apply translation to the point
        glm::mat4 Model = glm::translate(glm::mat4(1.0f), glm::vec3(pt));
        glm::mat4 MVP = mCamera->GetProjectionMatrix() * mCamera->GetCameraMatrix() * Model;//proj * cam * Model; //= mCamera->GetProjMtx() * mCamera->GetCamMtx() * Model;

        //Locate the MVP uniform in the shader
        GLuint Location = glGetUniformLocation(mPoint.mShader->GetGLHandle(), "uniform_mvp");
        if (Location >= 0)
            glUniformMatrix4fv(Location, 1, GL_FALSE, &MVP[0][0]);
        else
            throw std::runtime_error("RUNTIME ERROR: 'uniform_mvp' not found");

        //Locate the color uniform in the fragment shader
        Location = glGetUniformLocation(mPoint.mShader->GetGLHandle(), "uniform_color");
        if (Location >= 0)
            glUniform4f(Location, color.r, color.g, color.b, color.a);
        else
            throw std::runtime_error("RUNTIME ERROR: 'uniform_color' not found");
    }

    mPoint.BindForDrawing();
    glDrawArrays(GL_LINES, 0, mPoint.VtxCount());
}

// ------------------------------------------------------------------------
/*! Upadte
*
*   Updates the Perspective Matrix
*/ //----------------------------------------------------------------------
void debug_system::draw_segment(vec3 s, vec3 e, vec4 color) {
    //Enable shader
    mSegment.mShader->Bind();

    //Set uniforms:
    {
        //Build the model to viewport matrix
        //Model->world: transform based on position of starting point
        glm::mat4 Model = glm::translate(glm::mat4(1.0f), s) * glm::scale(glm::mat4(1.0f), e - s);
        glm::mat4 MVP = mCamera->GetProjectionMatrix() * mCamera->GetCameraMatrix() * Model; //= mCamera->GetProjMtx() * mCamera->GetCamMtx() * Model;

        //Locate the MVP uniform in the shader
        GLuint Location = glGetUniformLocation(mSegment.mShader->GetGLHandle(), "uniform_mvp");
        if (Location >= 0)
            glUniformMatrix4fv(Location, 1, GL_FALSE, &MVP[0][0]);
        else
            throw std::runtime_error("RUNTIME ERROR: 'uniform_mvp' not found");

        //Locate the color uniform in the fragment shader
        Location = glGetUniformLocation(mSegment.mShader->GetGLHandle(), "uniform_color");
        if (Location >= 0)
            glUniform4f(Location, color.r, color.g, color.b, color.a);
        else
            throw std::runtime_error("RUNTIME ERROR: 'uniform_color' not found");
    }

    mSegment.BindForDrawing();
    glDrawArrays(GL_LINES, 0, mSegment.VtxCount());
}

// ------------------------------------------------------------------------
/*! Upadte
*
*   Updates the Perspective Matrix
*/ //----------------------------------------------------------------------
void debug_system::draw_triangle(vec3 a, vec3 b, vec3 c, vec4 color) {
    mTriangle.Clear();
    mTriangle.InitTriangle(a, b, c);
    mTriangle.InitializeBuffers();

    //Enable shader
    mTriangle.mShader->Bind();

    //Set uniforms:
    {
        //Model = identity (not required)
        glm::mat4 MVP = mCamera->GetProjectionMatrix() * mCamera->GetCameraMatrix();

        //Locate the MVP uniform in the shader
        int Location = glGetUniformLocation(mTriangle.mShader->GetGLHandle(), "uniform_mvp");
        if (Location >= 0)
            glUniformMatrix4fv(Location, 1, GL_FALSE, &MVP[0][0]);
        else
            throw std::runtime_error("RUNTIME ERROR: 'uniform_mvp' not found");

        //Locate the color uniform in the fragment shader
        Location = glGetUniformLocation(mTriangle.mShader->GetGLHandle(), "uniform_color");
        if (Location >= 0)
            glUniform4f(Location, color.r, color.g, color.b, color.a);
        else
            throw std::runtime_error("RUNTIME ERROR: 'uniform_color' not found");
    }
    mTriangle.BindForDrawing();
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

// ------------------------------------------------------------------------
/*! Upadte
*
*   Updates the Perspective Matrix
*/ //----------------------------------------------------------------------
void debug_system::draw_aabb(vec3 c, vec3 size, vec4 color) {
    //Enable shader
    mAABB.mShader->Bind();

    //Set uniforms:
    {
        //Model: translate center, then scale in the 3 main axis
        glm::mat4 Model = glm::translate(glm::mat4(1.0f), c) * glm::scale(glm::mat4(1.0f), size);

        //Build the model to viewport matrix
        glm::mat4 MVP = mCamera->GetProjectionMatrix() * mCamera->GetCameraMatrix() * Model; //= mCamera->GetProjMtx() * mCamera->GetCamMtx() * Model;

        //Locate the MVP uniform in the shader
        int Location = glGetUniformLocation(mAABB.mShader->GetGLHandle(), "uniform_mvp");
        if (Location >= 0)
            glUniformMatrix4fv(Location, 1, GL_FALSE, &MVP[0][0]);
        else
            throw std::runtime_error("RUNTIME ERROR: 'uniform_mvp' not found");

        //Locate the color uniform in the fragment shader
        Location = glGetUniformLocation(mAABB.mShader->GetGLHandle(), "uniform_color");
        if (Location >= 0)
            glUniform4f(Location, color.r, color.g, color.b, color.a);
        else
            throw std::runtime_error("RUNTIME ERROR: 'uniform_color' not found");
    }

    mAABB.BindForDrawing();
    glDrawArrays(GL_LINES, 0, mAABB.VtxCount());
}

// ------------------------------------------------------------------------
/*! Upadte
*
*   Updates the Perspective Matrix
*/ //----------------------------------------------------------------------
void debug_system::draw_plane(vec3 pt, vec3 n, float scale, vec4 color) {
    //Enable shader
    mPlane.mShader->Bind();

    // Model: scale 2D, translate to the point, lookAt
    // following the normal
    glm::vec3 Normal = glm::normalize(n);
    glm::vec3 Right;

    //Sanity check to avoid cross product with the same vector
    if (Normal == glm::vec3(0.f, 1.0f, 0.0f))
        Right = glm::cross(Normal, glm::vec3(1.0f, 0.0f, 0.0f));

    else
        Right = glm::cross(Normal, glm::vec3(0.0f, 1.0f, 0.0f));

    glm::vec3 Up = glm::cross(Right, Normal);

    glm::mat4 ScaleMtx = glm::scale(glm::mat4(1.0f), glm::vec3(scale));
    glm::mat4 Model = glm::translate(glm::mat4(1.0f), pt) * glm::lookAt(pt, Normal, Up) * ScaleMtx;

    //Set uniforms:
    {
        //Model = identity (not required)
        glm::mat4 MVP = mCamera->GetProjectionMatrix() * mCamera->GetCameraMatrix() * Model; // = mCamera->GetProjMtx() * mCamera->GetCamMtx() * Model;

        //Locate the MVP uniform in the shader
        int Location = glGetUniformLocation(mPlane.mShader->GetGLHandle(), "uniform_mvp");
        if (Location >= 0)
            glUniformMatrix4fv(Location, 1, GL_FALSE, &MVP[0][0]);
        else
            throw std::runtime_error("RUNTIME ERROR: 'uniform_mvp' not found");

        //Locate the color uniform in the fragment shader
        Location = glGetUniformLocation(mPlane.mShader->GetGLHandle(), "uniform_color");
        if (Location >= 0)
            glUniform4f(Location, color.r, color.g, color.b, color.a);
        else
            throw std::runtime_error("RUNTIME ERROR: 'uniform_color' not found");
    }

    //Draw
    mPlane.BindForDrawing();
    glDrawArrays(GL_TRIANGLES, 0, mPlane.VtxCount());
}

// ------------------------------------------------------------------------
/*! Upadte
*
*   Updates the Perspective Matrix
*/ //----------------------------------------------------------------------
void debug_system::draw_sphere(vec3 c, float r, glm::vec4 color) {
    //Enable shader
    mSphere.mShader->Bind();

    //Set uniforms based on which disc to draw
    //1st disc
    {
        //Model: translate by the center, scale time the radius
        glm::mat4 Model = glm::translate(glm::mat4(1.0f), c) * glm::scale(glm::mat4(1.0f), glm::vec3(r));

        //Build the model to viewport matrix
        glm::mat4 MVP = mCamera->GetProjectionMatrix() * mCamera->GetCameraMatrix() * Model; // = mCamera->GetProjMtx() * mCamera->GetCamMtx();

        //Locate the MVP uniform in the shader
        int Location = glGetUniformLocation(mSphere.mShader->GetGLHandle(), "uniform_mvp");
        if (Location >= 0)
            glUniformMatrix4fv(Location, 1, GL_FALSE, &MVP[0][0]);
        else
            throw std::runtime_error("RUNTIME ERROR: 'uniform_mvp' not found");

        //Locate the color uniform in the fragment shader
        Location = glGetUniformLocation(mSphere.mShader->GetGLHandle(), "uniform_color");
        if (Location >= 0)
            glUniform4f(Location, color.r, color.g, color.b, color.a);
        else
            throw std::runtime_error("RUNTIME ERROR: 'uniform_color' not found");

        mSphere.BindForDrawing();
        glDrawArrays(GL_LINES, 0, mSphere.VtxCount());
    }


    //2nd disc
    {
        //Rotate to aligh with XZ plane
        glm::mat4 Model = glm::translate(glm::mat4(1.0f), c) * glm::rotate(glm::mat4(1.0f), glm::radians(90.f), glm::vec3(0.f, 1.0f, 0.f)) * glm::scale(glm::mat4(1.0f), glm::vec3(r));
        glm::mat4 MVP = mCamera->GetProjectionMatrix() * mCamera->GetCameraMatrix() * Model; // = mCamera->GetProjMtx() * mCamera->GetCamMtx();

        //Locate the MVP uniform in the shader
        int Location = glGetUniformLocation(mSphere.mShader->GetGLHandle(), "uniform_mvp");
        if (Location >= 0)
            glUniformMatrix4fv(Location, 1, GL_FALSE, &MVP[0][0]);
        else
            throw std::runtime_error("RUNTIME ERROR: 'uniform_mvp' not found");

        //Locate the color uniform in the fragment shader
        Location = glGetUniformLocation(mSphere.mShader->GetGLHandle(), "uniform_color");
        if (Location >= 0)
            glUniform4f(Location, color.r, color.g, color.b, color.a);
        else
            throw std::runtime_error("RUNTIME ERROR: 'uniform_color' not found");

        //Draw
        mSphere.BindForDrawing();
        glDrawArrays(GL_LINES, 0, mSphere.VtxCount());
    }


    //3rd disc
    {
        //Rotate to align with YZ plane
        glm::mat4 Model = glm::translate(glm::mat4(1.0f), c) * glm::rotate(glm::mat4(1.0f), glm::radians(90.f), glm::vec3(1.f, 0.0f, 0.f)) * glm::scale(glm::mat4(1.0f), glm::vec3(r));
        glm::mat4 MVP = mCamera->GetProjectionMatrix() * mCamera->GetCameraMatrix() * Model; // = mCamera->GetProjMtx() * mCamera->GetCamMtx();

        //Locate the MVP uniform in the shader
        int Location = glGetUniformLocation(mSphere.mShader->GetGLHandle(), "uniform_mvp");
        if (Location >= 0)
            glUniformMatrix4fv(Location, 1, GL_FALSE, &MVP[0][0]);
        else
            throw std::runtime_error("RUNTIME ERROR: 'uniform_mvp' not found");

        //Locate the color uniform in the fragment shader
        Location = glGetUniformLocation(mSphere.mShader->GetGLHandle(), "uniform_color");
        if (Location >= 0)
            glUniform4f(Location, color.r, color.g, color.b, color.a);
        else
            throw std::runtime_error("RUNTIME ERROR: 'uniform_color' not found");

        //Draw
        mSphere.BindForDrawing();
        glDrawArrays(GL_LINES, 0, mSphere.VtxCount());
    }

    //Horizon disc
    //4th: horizon disk
    // Calculate the horizon ring's radius
    // and center
    glm::vec3 View = { 0.f, 1.f, 0.f };// = mCamera->mPosition - c;
    float Dist = length(View);
    float L = sqrt(Dist * Dist - r * r);
    float HorizonR = (r * L) / Dist;
    float T = sqrt(r * r - HorizonR * HorizonR);

    //Compute lookAt matrix so it constantly looks to the camera
    glm::vec3 Right = glm::cross(View, glm::vec3(0.f, 1.0f, 0.f));
    glm::vec3 Up = glm::cross(View, Right);

    glm::mat4 LookMtx = glm::inverse(glm::lookAt(glm::vec3(0.0f), View, Up));

    //Set uniforms:
    {
        //Model: translate by the center, scale time the radius
        //and apply look matrix
        glm::mat4 Model = glm::translate(glm::mat4(1.0f), c + View * T) * LookMtx * glm::scale(glm::mat4(1.0f), glm::vec3(HorizonR));

        //Build the model to viewport matrix
        glm::mat4 MVP = mCamera->GetProjectionMatrix() * mCamera->GetCameraMatrix() * Model; // = mCamera->GetProjMtx() * mCamera->GetCamMtx();

        int Location = glGetUniformLocation(mSphere.mShader->GetGLHandle(), "uniform_mvp");
        if (Location >= 0)
            glUniformMatrix4fv(Location, 1, GL_FALSE, &MVP[0][0]);
        else
            throw std::runtime_error("RUNTIME ERROR: 'uniform_mvp' not found");

        //Locate the color uniform in the fragment shader
        Location = glGetUniformLocation(mSphere.mShader->GetGLHandle(), "uniform_color");
        if (Location >= 0)
            glUniform4f(Location, color.r, color.g, color.b, color.a);
        else
            throw std::runtime_error("RUNTIME ERROR: 'uniform_color' not found");
    }
    mSphere.BindForDrawing();
    glDrawArrays(GL_LINES, 0, mSphere.VtxCount());
}

// ------------------------------------------------------------------------
/*! Upadte
*
*   Updates the Perspective Matrix
*/ //----------------------------------------------------------------------
void debug_system::draw_frustum_lines(mat4 const &vp, vec4 color) {
    //Enable shader
    mFrustum.mShader->Bind();

    //Set uniforms:
    {
        //Build the model to viewport matrix
        //Model: use inverse of givel world-to-perspective matrix
        const glm::mat4 MVP = mCamera->GetProjectionMatrix() * mCamera->GetCameraMatrix() * glm::inverse(vp);

        //Locate the MVP uniform in the shader
        int Location = glGetUniformLocation(mFrustum.mShader->GetGLHandle(), "uniform_mvp");
        if (Location >= 0)
            glUniformMatrix4fv(Location, 1, GL_FALSE, &MVP[0][0]);
        else
            throw std::runtime_error("RUNTIME ERROR: 'uniform_mvp' not found");

        //Locate the color uniform in the fragment shader
        Location = glGetUniformLocation(mFrustum.mShader->GetGLHandle(), "uniform_color");
        if (Location >= 0)
            glUniform4f(Location, color.r, color.g, color.b, color.a);
        else
            throw std::runtime_error("RUNTIME ERROR: 'uniform_color' not found");
    }

    //Draw
    mFrustum.BindForDrawing();
    glDrawArrays(GL_LINES, 0, mFrustum.VtxCount());
}

void debug_system::change_camera(camera const* c) {
    mCamera = c;
}
