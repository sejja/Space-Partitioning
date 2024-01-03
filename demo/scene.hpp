#ifndef SCENE_HPP
#define SCENE_HPP
#include "math.hpp"
#include "shapes.hpp"
#include "octree.hpp"
#include "shader.hpp"
#include <vector>

/**
 * @brief
 */
struct GameObject
{
    bool visible = true;

    // BV
    aabb bv = {};

    // Space partitioning data
    // [TODO]
    Octree<GameObject>::node* m_octree_node = nullptr;
    GameObject* m_octree_next_obj = nullptr;
    GameObject* m_octree_prev_obj = nullptr;
    static int id_counter;
    int m_ID = id_counter++;

    // Render data
    mat4     m2w;
    uint32_t mesh_vao;
    uint32_t mesh_vtx_count;
    vec4     color;
};

/**
 * @brief
 */
class scene
{
  private:
    std::vector<GameObject> m_objects;

    struct NaiveMesh
    {
        unsigned vao;
        unsigned vtx_count;
        aabb     bv_model;
    };

    // Graphics resources
    struct
    {
        ::ShaderProgram*              m_shader;
        unsigned               m_mesh_quad = 0;
        std::vector<NaiveMesh> mirlo_meshes;
    } m_resources;

    Octree<GameObject> m_octree;

  public:
    // Stats
    int stat_draw_calls            = 0;
    int stat_frustum_aabb_checks   = 0;
    int stat_frustum_aabb_positive = 0;

  public:
    scene();
    ~scene();
    scene(scene const& rhs) = delete;
    scene& operator=(scene const& rhs) = delete;

    void LoadMirlo();
    void MakeAllVisible();
    void FrustumCheck(frustrum const& frustum);
    void OctreeCheck(frustrum const& frustum);
    void Render(mat4 const& v, mat4 const& p);
    void CreateOctree(int levels, int sizebit);

    [[nodiscard]] decltype(m_objects) const& objects() const { return m_objects; }
    [[nodiscard]] decltype(m_objects)&       objects() { return m_objects; }
    [[nodiscard]] decltype(m_octree)&        get_octree() { return m_octree; }
};

#endif // SCENE_HPP
