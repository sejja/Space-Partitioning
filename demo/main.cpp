//
// Created by eder on 10/11/21.
//
#include "window.hpp"
#include "debug_system.hpp"
#include "camera.hpp"
#include "imgui.hpp"
#include "octree.hpp"
#include "shapes.hpp"
#include "shape_utils.hpp"
#include "scene.hpp"
#include <chrono>
#include <functional>

#undef max

// Easy access
using node_t = Octree<GameObject>::node;

/**
 * @brief
 *  General options about this demo
 */
struct demo_options
{
    int  render_mode       = 0;    // 0-Bruteforce, 1-Frustum check, 2-Octrees
    bool skyview_enabled   = false; //
    bool debug_draw_octree = false; //
    int  highlight_level   = -1;   // If -1, will draw all levels
    int  octree_levels     = 3;    // How many levels should the octree have
    int  octree_size_bit   = 7;    // Octree root size is restricted to 2^k. (This parameter is k)

    struct
    {
        std::vector<float> history_dt;
        std::vector<float> history_draw_calls;
        std::vector<float> history_aabb_frustum;
        std::vector<float> history_aabb_frustum_positive;
    } stats;
};

/**
 * @brief 
 *  Function to update the camera
 */
void update_camera(float dt, window const& window, camera& camera)
{
    auto*       glfwWindow      = window.GetHandle();
    auto        camera_dir      = camera.GetTarget() - camera.GetPosition();
    auto        camera_position = camera.GetPosition();
    static vec2 cursor_position{};

    // Cursor
    double cursor_x = 0.0;
    double cursor_y = 0.0;
    glfwGetCursorPos(glfwWindow, &cursor_x, &cursor_y);
    { // Input
        if (glfwGetMouseButton(glfwWindow, GLFW_MOUSE_BUTTON_2)) {
            float speed = 10.0f;
            auto  side  = normalize(cross(camera_dir, { 0, 1, 0 }));

            if (glfwGetKey(glfwWindow, GLFW_KEY_LEFT_SHIFT)) {
                speed *= 4.0f;
            }
            // Move
            if (glfwGetKey(glfwWindow, GLFW_KEY_W)) {
                camera_position += normalize(camera_dir) * dt * speed;
            }
            if (glfwGetKey(glfwWindow, GLFW_KEY_S)) {
                camera_position -= normalize(camera_dir) * dt * speed;
            }
            if (glfwGetKey(glfwWindow, GLFW_KEY_A)) {
                camera_position -= normalize(side) * dt * speed;
            }
            if (glfwGetKey(glfwWindow, GLFW_KEY_D)) {
                camera_position += normalize(side) * dt * speed;
            }

            // View
            vec2 cursor_delta = { (float)cursor_x - cursor_position.x, (float)cursor_y - cursor_position.y };
            camera_dir        = vec3(vec4(camera_dir, 0) * rotate(glm::radians(15.0f) * 0.01f * cursor_delta.y, side));
            camera_dir        = vec3(vec4(camera_dir, 0) * rotate(glm::radians(15.0f) * 0.01f * cursor_delta.x, vec3(0, 1, 0)));
        }
        cursor_position    = { (float)cursor_x, (float)cursor_y };
        auto camera_target = camera_position + camera_dir;

        // Submit changes
        camera.set_position(camera_position);
        camera.set_target(camera_target);
        camera.set_projection(60.0f, window.GetDimensions(), 0.01f, 1000.0f);
        camera.update();
    }
}

int main()
{
    ::window       w(1920, 1080, "Open World Space Partitioning", true);
    ::camera       cam;
    ::camera       sky_cam;
    ::debug_system debug(&cam);
    ::demo_options options;
    ::scene        scene;

    glfwSwapInterval(0); // Disable vsync
    imgui_init(w.GetHandle());

    cam.set_position({ 0, 10, -10 });
    cam.set_target({ 0, 0, 0 });
    cam.set_projection(60.0f, { 3000, 1080 }, 0.01f, 1000.0f);
    cam.update();

    while (w.update()) {
        // DT
        static auto prev = std::chrono::high_resolution_clock::now();
        auto        now  = std::chrono::high_resolution_clock::now();
        float       dt   = std::chrono::duration<float>(now - prev).count();
        prev             = now;

        // Camera
        update_camera(dt, w, cam);
        cam.update();

        //
        glViewport(0, 0, w.GetDimensions().x, w.GetDimensions().y);
        glDisable(GL_SCISSOR_TEST);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClearDepth(1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Draw octree
        auto debug_draw_octree = [&]() {
            if (!options.debug_draw_octree) return;

            if (options.highlight_level == -1) {
                for (auto& it : scene.get_octree().m_nodes) {
                    if (it.second->first) {
                        aabb b = LocationalCode::compute_bv(it.second->locational_code, scene.get_octree().root_size());
                        debug.draw_aabb(b.pos, b.sca, glm::vec4(0.4 * it.second->locational_code, 1.0, 0, 0));
                    }
                }
            }
            // [TODO] Traverse octree
        };

        // Frustum to test
        frustrum frust(cam.GetProjectionMatrix() * cam.GetCameraMatrix());

        // Render modes
        switch (options.render_mode) {
            case 0:
                // All is visible, regardless of their position
                scene.MakeAllVisible();
                break;
            case 1:
                // Make visible only those inside frustum
                scene.FrustumCheck(frust);
                break;
            case 2:
                // Make visible only those inside frustum (accelerate with octree)
                scene.OctreeCheck(frust);
                break;
        }

        //debug.draw_frustum_lines(frust.get_matrix(), vec4(1.f));
        //debug.draw_plane(vec3(0.f), frust.mplanes[0].normal, frust.mplanes[0].d, vec4(0.5f, 0.5f, 0.5f, 0.5f));
        //debug.draw_plane(vec3(0.f), frust.mplanes[1].normal, frust.mplanes[1].d, vec4(0.5f, 0.5f, 0.5f, 0.5f));
        //debug.draw_plane(vec3(0.f), frust.mplanes[2].normal, frust.mplanes[2].d, vec4(0.5f, 0.5f, 0.5f, 0.5f));
        //debug.draw_plane(vec3(0.f), frust.mplanes[3].normal, frust.mplanes[3].d, vec4(0.5f, 0.5f, 0.5f, 0.5f));
        //debug.draw_plane(vec3(0.f), frust.mplanes[4].normal, frust.mplanes[4].d, vec4(0.5f, 0.5f, 0.5f, 0.5f));
        //debug.draw_plane(vec3(0.f), frust.mplanes[5].normal, frust.mplanes[5].d, vec4(0.5f, 0.5f, 0.5f, 0.5f));

        // Render
        scene.Render(cam.GetProjectionMatrix(), cam.GetCameraMatrix());
        debug_draw_octree();

        // Sky view
        if (options.skyview_enabled) {
            glEnable(GL_SCISSOR_TEST);
            ivec2 skyview_viewport_size(1000, 500);
            glViewport(w.GetDimensions().x - skyview_viewport_size.x, 0, skyview_viewport_size.x, skyview_viewport_size.y);
            glScissor(w.GetDimensions().x - skyview_viewport_size.x, 0, skyview_viewport_size.x, skyview_viewport_size.y);
            glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // Special camera for sky view
            sky_cam.set_position(vec3(500, 500, 500));
            sky_cam.set_target(cam.GetPosition());
            sky_cam.set_projection(50.0f, skyview_viewport_size, 0.1f, 1000.0f);
            sky_cam.update();
            scene.Render(sky_cam.GetProjectionMatrix(), sky_cam.GetCameraMatrix());

            // Debug draw frustum
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glDisable(GL_CULL_FACE);

            // Render everything from this camera
            debug.change_camera(&sky_cam);
            debug.draw_frustum_lines(frust.get_matrix(), vec4(1, 1, 1, 0.5f));
            debug_draw_octree();
            debug.change_camera(&cam); 
        }

        { // Editor
            // Stats
            imgui_new_frame();
            ImGui::SetNextWindowSizeConstraints(ImVec2(100, w.GetDimensions().y), ImVec2(std::numeric_limits<float>::max(), w.GetDimensions().y));
            ImGui::SetNextWindowPos({});
            if (ImGui::Begin("Options")) {
                if (ImGui::RadioButton("Render all", options.render_mode == 0)) options.render_mode = 0;
                if (ImGui::RadioButton("Frustum check", options.render_mode == 1)) options.render_mode = 1;
                if (ImGui::RadioButton("Octree check", options.render_mode == 2)) options.render_mode = 2;

                ImGui::Checkbox("Skyview", &options.skyview_enabled);

                ImGui::Separator();
                { // DT
                    float const v         = 1.0f / dt;
                    auto&       container = options.stats.history_dt;
                    container.push_back(v);
                    if (container.size() > 100) container.erase(container.begin());
                    ImGui::PlotLines("FPS", container.data(), static_cast<int>(container.size()), 0, "", 0, FLT_MAX, ImVec2(0, 64));
                    ImGui::Text("Current: %.02f", v);
                }

                { // Render objects
                    auto const v         = scene.stat_draw_calls;
                    auto&      container = options.stats.history_draw_calls;
                    container.push_back(static_cast<float>(v));
                    if (container.size() > 100) container.erase(container.begin());
                    ImGui::PlotLines("Draw calls", container.data(), static_cast<int>(container.size()), 0, "", 0, FLT_MAX, ImVec2(200, 64));
                    ImGui::Text("Current: %d", v);
                }

                { // Frustum vs AABB
                    auto const v         = scene.stat_frustum_aabb_checks;
                    auto&      container = options.stats.history_aabb_frustum;
                    container.push_back(static_cast<float>(v));
                    if (container.size() > 100) container.erase(container.begin());
                    ImGui::PlotLines("Frustum vs AABB", container.data(), static_cast<int>(container.size()), 0, "", 0, FLT_MAX, ImVec2(200, 64));
                    ImGui::Text("Current: %d", v);
                }

                { // Frustum vs AABB (passed)
                    auto const v         = scene.stat_frustum_aabb_positive;
                    auto&      container = options.stats.history_aabb_frustum_positive;
                    container.push_back(static_cast<float>(v));
                    if (container.size() > 100) container.erase(container.begin());
                    ImGui::PlotLines("Frustum vs AABB (passed)", container.data(), static_cast<int>(container.size()), 0, "", 0, FLT_MAX, ImVec2(200, 64));
                    ImGui::Text("Current: %d", v);
                }

                ImGui::Separator();

                ImGui::Checkbox("Debug draw octree", &options.debug_draw_octree);
                if (ImGui::SliderInt("Octree levels", &options.octree_levels, 1, 10)) {
                    scene.get_octree().set_levels(options.octree_levels);
                    scene.CreateOctree(options.octree_levels, options.octree_size_bit);
                }
                if (ImGui::SliderInt("Octree size bit", &options.octree_size_bit, 1, 32)) {
                    scene.get_octree().set_root_size((1u << options.octree_size_bit));
                    scene.CreateOctree(options.octree_levels, options.octree_size_bit);
                }
                ImGui::SliderInt("Highlight level", &options.highlight_level, -1, options.octree_levels);

                if (ImGui::Button("Color by octree node")) {


                    for (auto& it : scene.get_octree().m_nodes) {
                        if (it.second->first) {
                            auto col = glm::linearRand(vec4(0, 0, 0, 1), vec4(1, 1, 1, 1));

                            auto obj = it.second->first;

                            if(!obj) continue;

                            do {
								obj->color = col;
								obj = obj->m_octree_next_obj;
							} while (obj != nullptr);
                        }
                    }
                }
                if (ImGui::Button("Color randomly")) {
                    for (auto& obj : scene.objects()) {
                        auto col  = glm::linearRand(vec4(0, 0, 0, 1), vec4(1, 1, 1, 1));
                        obj.color = col;
                    }
                }
            }
            ImGui::End();
            imgui_end_frame();
        }
    }
    imgui_destroy();
    return 0;
}