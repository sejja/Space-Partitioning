#include "scene.hpp"
#include "opengl.hpp"
#include "geometry.hpp"
#include "shape_utils.hpp"
#include "shader.hpp"
#include <iostream>
#include <string>
#include <cstring>
#include <sstream>
#include <fstream>
#include <array>
#include "camera.hpp"
#include "geometry.hpp"

#ifndef WORKDIR
#define WORKDIR "./"
#endif

// Uniform locations
constexpr int cUniformLocation_uniform_m2w   = 0;
constexpr int cUniformLocation_uniform_view  = 1;
constexpr int cUniformLocation_uniform_proj  = 2;
constexpr int cUniformLocation_uniform_color = 3;

scene::scene()
{
    { // Shader with uniforms
        auto vtx_code        = R"(
                #version 440 core
                layout(location = 0) in vec3 attr_position;
                layout(location = 0) uniform mat4 uniform_m2w;
                layout(location = 1) uniform mat4 uniform_view;
                layout(location = 2) uniform mat4 uniform_proj;
                layout(location = 3) uniform vec4 uniform_color;
                void main()
                {
                  vec4 vertex = vec4(attr_position, 1.0f);
                  mat4 mvp = uniform_proj * uniform_view * uniform_m2w;
                  gl_Position = mvp * vertex;
                })";
        auto frag_code       = R"(
                #version 440 core
                out vec4 out_color;
                layout(location = 0) uniform mat4 uniform_m2w;
                layout(location = 1) uniform mat4 uniform_view;
                layout(location = 2) uniform mat4 uniform_proj;
                layout(location = 3) uniform vec4 uniform_color;
                void main()
                {
                  out_color = uniform_color;
                })";
        m_resources.m_shader = new ShaderProgram(
            Shader(vtx_code, Shader::EType::Vertex), 
            Shader(frag_code, Shader::EType::Fragment));
    }

    LoadMirlo();
    CreateOctree(6, 10);
}

scene::~scene()
{
    delete m_resources.m_shader;
}

mat4 abs(const mat4& mat)
{
    mat4 result;
    for (mat4::length_type i = 0; i < 4; ++i)
        for (mat4::length_type j = 0; j < 4; ++j)
            result[i][j] = abs(mat[i][j]);
    return result;
}

aabb transform_aabb(aabb const& aabbp, mat4 const& m2w)
{
    vec3 center = vec3(m2w * vec4(aabbp.pos, 1.f));
    vec3 extent = vec3(abs(m2w) * vec4(aabbp.sca, 0.f));
    aabb result(center - extent, center + extent);
    return result;
}

void scene::LoadMirlo()
{
    int  i       = 0;
    bool failure = false;
    while (!failure) {
        // Create filename of the mesh
        std::stringstream ss;
        ss << WORKDIR "assets/mirlo_" << i << ".binary";
        std::string filename = ss.str();

        // Open file
        std::ifstream fs(filename, std::ios::binary);
        if (!fs.is_open()) {
            failure = true;
            break;
        }

        // Check header
        char header[6]{};
        fs.read(header, sizeof(header) - 1);

        // Read vertex count
        unsigned vertex_count = 0;
        fs.read(reinterpret_cast<char*>(&vertex_count), 4);

        // Read index count
        unsigned index_count = 0;
        fs.read(reinterpret_cast<char*>(&index_count), 4);
        assert(index_count == 0); // No indices in this lab

        // Read attributes
        bool has_positions = false;
        bool has_normals   = false;
        bool has_uvs       = false;
        fs.read(reinterpret_cast<char*>(&has_positions), 1);
        fs.read(reinterpret_cast<char*>(&has_normals), 1);
        fs.read(reinterpret_cast<char*>(&has_uvs), 1);
        assert(has_positions);
        assert(!has_normals); // No extra data in this lab
        assert(!has_uvs);     // No extra data in this lab

        // Read vertices
        unsigned                         triangle_count = vertex_count / 3;
        std::vector<std::array<vec3, 3>> triangles(triangle_count);
        for (unsigned t_idx = 0; t_idx < triangle_count; ++t_idx) {
            for (unsigned v_idx = 0; v_idx < 3; ++v_idx) {
                fs.read(reinterpret_cast<char*>(&triangles[t_idx][v_idx]), sizeof(vec3)); // Position
            }
        }

        // Local BV
        vec3 bv_min = triangles.front().at(0);
        vec3 bv_max = bv_min;
        for (unsigned t_idx = 0; t_idx < triangle_count; ++t_idx) {
            for (unsigned v_idx = 0; v_idx < 3; ++v_idx) {
                bv_min = glm::min(bv_min, triangles[t_idx][v_idx]);
                bv_max = glm::max(bv_max, triangles[t_idx][v_idx]);
            }
        }

        { // LOAD GRAPHICS MESH
            uint32_t mesh_vbo = 0;
            glGenBuffers(1, &mesh_vbo);
            glBindBuffer(GL_ARRAY_BUFFER, mesh_vbo);
            glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(sizeof(float) * 3 * 3 * triangles.size()), triangles.data(), GL_STATIC_DRAW);
            uint32_t mesh_vao = 0;
            glGenVertexArrays(1, &mesh_vao);
            glBindVertexArray(mesh_vao);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, nullptr);
            glEnableVertexAttribArray(0);

            NaiveMesh mesh{};
            mesh.vao       = mesh_vao;
            mesh.vtx_count = static_cast<unsigned>(triangles.size() * 3);
            mesh.bv_model  = aabb( bv_min, bv_max );
            m_resources.mirlo_meshes.push_back(mesh);
        }

        i++;
    }
    std::cout << "Loaded resources: " << i << "\n";
    if (i == 0) throw std::runtime_error("Could not load resources, ensure WORKDIR preprocessor definition is correct");

    { // Load scene
        // Create filename of the scene
        std::stringstream ss;
        ss << WORKDIR "assets/scene.txt";
        std::string filename = ss.str();

        std::ifstream fs(filename, std::ios::binary);
        if (!fs.is_open()) {
            throw std::runtime_error("Could not open scene file");
        }

        while (!fs.bad() && !fs.eof()) {
            int  mesh_index = 0;
            mat4 m2w        = glm::identity<mat4>();

            fs >> mesh_index;
            fs >> m2w[0].x >> m2w[0].y >> m2w[0].z >> m2w[0].w >>
                m2w[1].x >> m2w[1].y >> m2w[1].z >> m2w[1].w >> 
                m2w[2].x >> m2w[2].y >> m2w[2].z >> m2w[2].w >>
                m2w[3].x >> m2w[3].y >> m2w[3].z >> m2w[3].w;

            if (fs.bad() || fs.eof()) break;

            // Create the objects
            auto const& mesh = m_resources.mirlo_meshes.at(mesh_index);
            GameObject  obj{};
            obj.color = glm::linearRand(vec4(0.2, 0.2, 0.2, 1), vec4(0.5, 0.5, 0.5, 1));
            obj.m2w = m2w;
            obj.mesh_vao = mesh.vao;
            obj.mesh_vtx_count = mesh.vtx_count;
            obj.bv = transform_aabb(mesh.bv_model, m2w); // [TODO] Retrieve the AABB in world coordinates
            m_objects.push_back(obj);
        }
    }
}

/**
 * @brief
 *  Render all the objects without regard to their spatial situation
 */
void scene::Render(mat4 const& p, mat4 const& v)
{
    stat_draw_calls = 0;

    // Reset state
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDisable(GL_BLEND);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);

    auto* shader = m_resources.m_shader;
    shader->Bind();
    glUniformMatrix4fv(cUniformLocation_uniform_view, 1, GL_FALSE, &v[0][0]);
    glUniformMatrix4fv(cUniformLocation_uniform_proj, 1, GL_FALSE, &p[0][0]);
    for (auto const& obj : m_objects) {
        // Skip non visible
        if (!obj.visible) continue;

        // Shader
        auto& m2w = obj.m2w;
        glUniformMatrix4fv(cUniformLocation_uniform_m2w, 1, GL_FALSE, &m2w[0][0]);
        glUniform4fv(cUniformLocation_uniform_color, 1, &obj.color[0]);
        // Mesh
        glBindVertexArray(obj.mesh_vao);
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(obj.mesh_vtx_count));
        stat_draw_calls++;
    }
}
void scene::MakeAllVisible()
{
    for (auto& obj : m_objects) {
        obj.visible = true;
    }
}

void CheckFrustrumObjectCollisions(Octree<GameObject>::node* node, frustrum const& frus) {
    GameObject* pointer = node->first;

    while (pointer) {
        const glm::mat4& model = pointer->m2w;
        glm::vec3 min = model * glm::vec4(pointer->bv.min, 1.f);
        glm::vec3 max = model * glm::vec4(pointer->bv.max, 1.f);
        eResult c = classify_frustum_aabb_naive(frus, aabb(pointer->bv.min, pointer->bv.max));

        if (c == eOUTSIDE)  // its is outside, not render it
            pointer->visible = false;
        else
            pointer->visible = true;

        pointer = pointer->m_octree_next_obj;
    }
}

/**
 * @brief
 *  Render all objects that are within the frustum
 */
void scene::FrustumCheck(frustrum const& frustum)
{
    // Stats
    stat_frustum_aabb_checks   = 0;
    stat_frustum_aabb_positive = 0;

    // [TODO]
    for (auto& x : m_octree.m_nodes) {
        CheckFrustrumObjectCollisions(x.second, frustum);
    }

}

/**
 * @brief
 *
 */
void scene::OctreeCheck(frustrum const& frustum)
{
    stat_frustum_aabb_checks   = 0;
    stat_frustum_aabb_positive = 0;

    for (auto& it : m_octree.m_nodes) {
        if (it.second->first) {//if it has objects inside
            aabb node = LocationalCode::compute_bv(it.second->locational_code, m_octree.root_size());
            eResult c = ::classify_frustum_aabb_naive(frustum, node);
            stat_frustum_aabb_checks++;

            if (c == eINSIDE) {
                GameObject* pointer = it.second->first;
                while (pointer) {
                    pointer->visible = true;
                    pointer = pointer->m_octree_next_obj;
                    stat_frustum_aabb_positive++;
                }
            }
            else if (c == eOUTSIDE) {
                GameObject* pointer = it.second->first;
                while (pointer) {
                    pointer->visible = false;
                    pointer = pointer->m_octree_next_obj;
                }
            }
            else {// overlaping
                CheckFrustrumObjectCollisions(it.second, frustum);
            }
        }
    }
}

int GameObject::id_counter = 0;

void AddObjToNode(GameObject& obj, Octree<GameObject>::node* node) {
    if (obj.m_octree_node->first == nullptr) {
        obj.m_octree_node->first = &obj; //set this object as the first object
    }
    else {
        bool flag = false; //flag to check if the object was already inserted
        GameObject* pointer = obj.m_octree_node->first;
        while (pointer->m_octree_next_obj != nullptr) {
            pointer = pointer->m_octree_next_obj;
            if (pointer->m_ID == obj.m_ID) {//same object
                flag = true;
                break;
            }
        }
        //set the pointers if the object was not already inside, check also with the first and last objects
        if (!flag && obj.m_octree_node->first->m_ID != obj.m_ID) {
            pointer->m_octree_next_obj = &obj;
            obj.m_octree_prev_obj = pointer;
        }
    }
}

void EraseObjFromPrevNode(GameObject& obj, Octree<GameObject>& tree) {
    //traverse through the linked list, in order to erase it from the prev linked list
    GameObject* pointer = obj.m_octree_node->first;
    if (obj.m_octree_node->first) {
        //if it was in the 1 position
        if (obj.m_octree_node->first->m_ID == obj.m_ID) {
            if (pointer->m_octree_next_obj == nullptr) //alone in the linked list
            {
                obj.m_octree_node->first = nullptr;
                if (obj.m_octree_node->children_active == 0) //if not children nodes delete
                    tree.delete_node(obj.m_octree_node->locational_code);

            }
            else { //stil more objects in the linked list
                obj.m_octree_node->first = obj.m_octree_next_obj;
                obj.m_octree_next_obj = nullptr;
            }
        }
        else {
            while (pointer->m_octree_next_obj != nullptr) {
                //traverse
                pointer = pointer->m_octree_next_obj;

                if (pointer->m_ID == obj.m_ID) {
                    //if it is in the last position
                    if (pointer->m_octree_next_obj == nullptr) {
                        pointer->m_octree_prev_obj->m_octree_next_obj = nullptr;
                    }
                    else {
                        pointer->m_octree_prev_obj->m_octree_next_obj = pointer->m_octree_next_obj;
                        pointer->m_octree_next_obj->m_octree_prev_obj = pointer->m_octree_prev_obj;
                    }

                    //set to null the pointers of the object
                    obj.m_octree_prev_obj = nullptr;
                    obj.m_octree_next_obj = nullptr;
                }
            }
        }
    }
}

/**
 * @brief
 */
void scene::CreateOctree(int levels, int sizebit) {
    m_octree.set_root_size(1u << sizebit);
   m_octree.set_levels(levels);

   auto& renderables = m_objects;
   //traverse thorugh all renderables
   for (auto& it : renderables) {
       const glm::mat4& model = it.m2w;
       // get or create if not created yet the node that encapsulates the object
       Octree<GameObject>::node* new_node = m_octree.create_node(aabb(glm::vec4(it.bv.min, 1.f)
           , glm::vec4(it.bv.max, 1.f)));
       //check if it already had another octree::node list 
       if (it.m_octree_node) {
           if (it.m_octree_node == new_node)
               continue;
           else {
               EraseObjFromPrevNode(it, m_octree);
               it.m_octree_node = new_node;
               AddObjToNode(it, it.m_octree_node);
           }
       }
       else {
           it.m_octree_node = new_node;
           AddObjToNode(it, it.m_octree_node);
       }
   }
}
