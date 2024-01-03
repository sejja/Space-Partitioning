#include "common.hpp"
#include <fstream>
#include <cstring>

std::ifstream& operator>> (std::ifstream& in, glm::vec3& c1) {

    in >> c1.x;
    in >> c1.y;
    in >> c1.z;

    return in;
}

namespace testing {
    namespace internal {
        testing::AssertionResult& operator<< (testing::AssertionResult& out, const glm::vec3& c1) {
            out << c1.x << " " << c1.y << " " << c1.z;
            return out;
        }

        testing::AssertionResult& operator<< (testing::AssertionResult& out, const glm::vec2& c1) {
            out << c1.x << " " << c1.y;
            return out;
        }

        AssertionResult DoubleNearPredFormat(const char* expr1, const char* expr2, const char* abs_error_expr, glm::vec3 const& val1, glm::vec3 const& val2, double abs_error)
        {
            auto x_res = DoubleNearPredFormat(expr1, expr2, abs_error_expr, val1.x, val2.x, abs_error);
            auto y_res = DoubleNearPredFormat(expr1, expr2, abs_error_expr, val1.y, val2.y, abs_error);
            auto z_res = DoubleNearPredFormat(expr1, expr2, abs_error_expr, val1.z, val2.z, abs_error);

            if (!x_res || !y_res || !z_res) {
                return AssertionFailure()
                       << "The difference between " << expr1 << " and " << expr2
                       << " exceeds " << abs_error_expr << ", where\n"
                       << expr1 << " evaluates to " << val1 << ",\n"
                       << expr2 << " evaluates to " << val2 << ", and\n"
                       << abs_error_expr << " evaluates to " << abs_error << ".";
            }

            return AssertionSuccess();
        }

        AssertionResult DoubleNearPredFormat(const char* expr1, const char* expr2, const char* abs_error_expr, glm::vec2 const& val1, glm::vec2 const& val2, double abs_error)
        {
            auto x_res = DoubleNearPredFormat(expr1, expr2, abs_error_expr, val1.x, val2.x, abs_error);
            auto y_res = DoubleNearPredFormat(expr1, expr2, abs_error_expr, val1.y, val2.y, abs_error);

            if (!x_res || !y_res) {
                return AssertionFailure()
                       << "The difference between " << expr1 << " and " << expr2
                       << " exceeds " << abs_error_expr << ", where\n"
                       << expr1 << " evaluates to " << val1 << ",\n"
                       << expr2 << " evaluates to " << val2 << ", and\n"
                       << abs_error_expr << " evaluates to " << abs_error << ".";
            }

            return AssertionSuccess();
        }

        AssertionResult DoubleNearPredFormat(const char* expr1, const char* expr2, const char* abs_error_expr, aabb const& val1, aabb const& val2, double abs_error)
        {
            auto a_res = DoubleNearPredFormat(expr1, expr2, abs_error_expr, val1.min, val2.min, abs_error);
            auto b_res = DoubleNearPredFormat(expr1, expr2, abs_error_expr, val1.max, val2.max, abs_error);

            if (!a_res || !b_res) {
                return AssertionFailure()
                       << "The difference between " << expr1 << " and " << expr2
                       << " exceeds " << abs_error_expr << ", where\n"
                       << expr1 << " evaluates to [" << val1.min << "], [" << val1.max << "],\n"
                       << expr2 << " evaluates to [" << val2.min << "], [" << val2.max << "], and\n"
                       << abs_error_expr << " evaluates to " << abs_error << ".";
            }

            return AssertionSuccess();
        }

        AssertionResult DoubleNearPredFormat(const char* expr1, const char* expr2, const char* abs_error_expr, triangle const& val1, triangle const& val2, double abs_error)
        {
            auto a_res = DoubleNearPredFormat(expr1, expr2, abs_error_expr, val1[0], val2[0], abs_error);
            auto b_res = DoubleNearPredFormat(expr1, expr2, abs_error_expr, val1[1], val2[1], abs_error);
            auto c_res = DoubleNearPredFormat(expr1, expr2, abs_error_expr, val1[2], val2[2], abs_error);

            if (!a_res || !b_res || !c_res) {
                return AssertionFailure()
                       << "The difference between " << expr1 << " and " << expr2
                       << " exceeds " << abs_error_expr << ", where\n"
                       << expr1 << " evaluates to " << val1[0] << ", " << val1[1] << ", " << val1[2] << ",\n"
                       << expr2 << " evaluates to " << val2[0] << ", " << val2[1] << ", " << val2[2] << ", and\n"
                       << abs_error_expr << " evaluates to " << abs_error << ".";
            }

            return AssertionSuccess();
        }
    }
}

std::vector<triangle> load_triangles_ascii(char const* path)
{
    std::ifstream f(path);
    if (!f.is_open()) {
        std::stringstream ss;
        ss << "Could not open file: " << path;
        throw std::runtime_error(ss.str());
    }

    std::vector<triangle> triangles;

    int n = 0;
    f >> n;
    for (int i = 0; i < n; ++i) {
        triangle t;
        f >> t[0];
        f >> t[1];
        f >> t[2];
        triangles.push_back(t);
    }

    return triangles;
}

std::vector<triangle> load_triangles_binary(char const* path)
{
    std::ifstream f(path);
    if (!f.is_open()) {
        std::stringstream ss;
        ss << "Could not open file: " << path;
        throw std::runtime_error(ss.str());
    }

    // Check header
    char header[6]{};
    f.read(header, sizeof(header) - 1);

    // Read vertex count
    unsigned vertex_count = 0;
    f.read(reinterpret_cast<char*>(&vertex_count), 4);

    // Read vertex count
    unsigned index_count = 0;
    f.read(reinterpret_cast<char*>(&index_count), 4);

    // Read attributes
    bool has_positions = false;
    bool has_normals   = false;
    bool has_uvs       = false;
    f.read(reinterpret_cast<char*>(&has_positions), 1);
    f.read(reinterpret_cast<char*>(&has_normals), 1);
    f.read(reinterpret_cast<char*>(&has_uvs), 1);

    // Read vertices
    unsigned triangle_count = vertex_count / 3;
    std::vector<triangle> triangles(triangle_count);
    for (unsigned t_idx = 0; t_idx < triangle_count; ++t_idx) {
        for (unsigned v_idx = 0; v_idx < 3; ++v_idx) {
            f.read(reinterpret_cast<char*>(&triangles[t_idx][v_idx]), sizeof(vec3)); // Position
            if (has_normals) f.seekg(sizeof(vec3));
            if (has_uvs) f.seekg(sizeof(vec2));
        }
    }

    return triangles;
}