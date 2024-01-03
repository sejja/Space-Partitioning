#include <bitset>
#include "common.hpp"
#include "octree.hpp"

TEST(quadtree, location_root_only)
{
    uint32_t root_size = 4;
    ASSERT_EQ(LocationalCode::compute_locational_code<2>({-1, -1}, root_size, 1), 0b100);
    ASSERT_EQ(LocationalCode::compute_locational_code<2>({1, -1}, root_size, 1), 0b101);
    ASSERT_EQ(LocationalCode::compute_locational_code<2>({-1, 1}, root_size, 1), 0b110);
    ASSERT_EQ(LocationalCode::compute_locational_code<2>({1, 1}, root_size, 1), 0b111);
}

TEST(quadtree, location_root_only_big)
{
    uint32_t root_size = 128;
    ASSERT_EQ(LocationalCode::compute_locational_code<2>({-1, -1}, root_size, 1), 0b100);
    ASSERT_EQ(LocationalCode::compute_locational_code<2>({1, -1}, root_size, 1), 0b101);
    ASSERT_EQ(LocationalCode::compute_locational_code<2>({-1, 1}, root_size, 1), 0b110);
    ASSERT_EQ(LocationalCode::compute_locational_code<2>({1, 1}, root_size, 1), 0b111);
}

TEST(quadtree, location_2levels)
{
    uint32_t root_size = 128;
    ASSERT_EQ(LocationalCode::compute_locational_code<2>({-1, -1}, root_size, 2), 0b10011);
    ASSERT_EQ(LocationalCode::compute_locational_code<2>({1, -1}, root_size, 2), 0b10110);
    ASSERT_EQ(LocationalCode::compute_locational_code<2>({-1, 1}, root_size, 2), 0b11001);
    ASSERT_EQ(LocationalCode::compute_locational_code<2>({1, 1}, root_size, 2), 0b11100);
}

TEST(quadtree, location_2levels_borderline)
{
    uint32_t root_size = 4;
    ASSERT_EQ(LocationalCode::compute_locational_code<2>({-1, -1}, root_size, 2), 0b10011);
    ASSERT_EQ(LocationalCode::compute_locational_code<2>({1, -1}, root_size, 2), 0b10111);
    ASSERT_EQ(LocationalCode::compute_locational_code<2>({-1, 1}, root_size, 2), 0b11011);
    ASSERT_EQ(LocationalCode::compute_locational_code<2>({1, 1}, root_size, 2), 0b11111);
}

TEST(octree, location_root_only)
{
    uint32_t root_size = 4;
    ASSERT_EQ(LocationalCode::compute_locational_code({-1, -1, -1}, root_size, 1), 0b1000);
    ASSERT_EQ(LocationalCode::compute_locational_code({1, -1, -1}, root_size, 1), 0b1001);
    ASSERT_EQ(LocationalCode::compute_locational_code({-1, 1, -1}, root_size, 1), 0b1010);
    ASSERT_EQ(LocationalCode::compute_locational_code({1, 1, -1}, root_size, 1), 0b1011);
    ASSERT_EQ(LocationalCode::compute_locational_code({-1, -1, 1}, root_size, 1), 0b1100);
    ASSERT_EQ(LocationalCode::compute_locational_code({1, -1, 1}, root_size, 1), 0b1101);
    ASSERT_EQ(LocationalCode::compute_locational_code({-1, 1, 1}, root_size, 1), 0b1110);
    ASSERT_EQ(LocationalCode::compute_locational_code({1, 1, 1}, root_size, 1), 0b1111);
}

TEST(octree, location_root_only_big)
{
    uint32_t root_size = 128;
    ASSERT_EQ(LocationalCode::compute_locational_code({-1, -1, -1}, root_size, 1), 0b1000);
    ASSERT_EQ(LocationalCode::compute_locational_code({1, -1, -1}, root_size, 1), 0b1001);
    ASSERT_EQ(LocationalCode::compute_locational_code({-1, 1, -1}, root_size, 1), 0b1010);
    ASSERT_EQ(LocationalCode::compute_locational_code({1, 1, -1}, root_size, 1), 0b1011);
    ASSERT_EQ(LocationalCode::compute_locational_code({-1, -1, 1}, root_size, 1), 0b1100);
    ASSERT_EQ(LocationalCode::compute_locational_code({1, -1, 1}, root_size, 1), 0b1101);
    ASSERT_EQ(LocationalCode::compute_locational_code({-1, 1, 1}, root_size, 1), 0b1110);
    ASSERT_EQ(LocationalCode::compute_locational_code({1, 1, 1}, root_size, 1), 0b1111);
}

TEST(octree, location_2levels)
{
    uint32_t root_size = 128;
    ASSERT_EQ(LocationalCode::compute_locational_code({-1, -1, -1}, root_size, 2), 0b1000111);
    ASSERT_EQ(LocationalCode::compute_locational_code({1, -1, -1}, root_size, 2), 0b1001110);
    ASSERT_EQ(LocationalCode::compute_locational_code({-1, 1, -1}, root_size, 2), 0b1010101);
    ASSERT_EQ(LocationalCode::compute_locational_code({1, 1, -1}, root_size, 2), 0b1011100);
    ASSERT_EQ(LocationalCode::compute_locational_code({-1, -1, 1}, root_size, 2), 0b1100011);
    ASSERT_EQ(LocationalCode::compute_locational_code({1, -1, 1}, root_size, 2), 0b1101010);
    ASSERT_EQ(LocationalCode::compute_locational_code({-1, 1, 1}, root_size, 2), 0b1110001);
    ASSERT_EQ(LocationalCode::compute_locational_code({1, 1, 1}, root_size, 2), 0b1111000);
}

TEST(octree, location_outside)
{
    uint32_t root_size = 128;
    ASSERT_EQ(LocationalCode::compute_locational_code({64, -1, -1}, root_size, 2), 0b1);
    ASSERT_EQ(LocationalCode::compute_locational_code({-65, -1, -1}, root_size, 2), 0b1);
    ASSERT_EQ(LocationalCode::compute_locational_code({-64, -64, -64}, root_size, 2), 0b1000000); // Borderline
}

TEST(octree, common_locational_codes)
{
    ASSERT_EQ(LocationalCode::common_locational_code(0b1000111, 0b1000111), 0b1000111);
    ASSERT_EQ(LocationalCode::common_locational_code(0b1000111010, 0b1000111), 0b1000111);
    ASSERT_EQ(LocationalCode::common_locational_code(0b1000111, 0b1000111010), 0b1000111);
    ASSERT_EQ(LocationalCode::common_locational_code(0b1000111010, 0b1000111011), 0b1000111);
    ASSERT_EQ(LocationalCode::common_locational_code(0b1010111010, 0b1001111011), 0b1);
    ASSERT_EQ(LocationalCode::common_locational_code(0b1, 0b1111), 0b1);
    ASSERT_EQ(LocationalCode::common_locational_code(0b1111, 0b1), 0b1);
    ASSERT_EQ(LocationalCode::common_locational_code(0b1, 0b1), 0b1);
}

TEST(octree, location_bv)
{
    uint32_t root_size = 128;
    ASSERT_EQ(LocationalCode::compute_locational_code(aabb({-63, -60, -63}, {-62, -52, -62}), root_size, 2), 0b1000000);
}

TEST(octree, bv)
{
    aabb bv{};
    bv = LocationalCode::compute_bv(0b1, 128);
    ASSERT_NEAR(bv.min, glm::vec3(-64, -64, -64), 1e-1f);
    ASSERT_NEAR(bv.max, glm::vec3(64, 64, 64), 1e-1f);

    bv = LocationalCode::compute_bv(0b1000, 128);
    ASSERT_NEAR(bv.min, glm::vec3(-64, -64, -64), 1e-1f);
    ASSERT_NEAR(bv.max, glm::vec3(0, 0, 0), 1e-1f);

    bv = LocationalCode::compute_bv(0b1000000, 128);
    ASSERT_NEAR(bv.min, glm::vec3(-64, -64, -64), 1e-1f);
    ASSERT_NEAR(bv.max, glm::vec3(-32, -32, -32), 1e-1f);

    bv = LocationalCode::compute_bv(0b1111, 128);
    ASSERT_NEAR(bv.min, glm::vec3(0, 0, 0), 1e-1f);
    ASSERT_NEAR(bv.max, glm::vec3(64, 64, 64), 1e-1f);

    bv = LocationalCode::compute_bv(0b1111111, 128);
    ASSERT_NEAR(bv.min, glm::vec3(32, 32, 32), 1e-1f);
    ASSERT_NEAR(bv.max, glm::vec3(64, 64, 64), 1e-1f);

    bv = LocationalCode::compute_bv(0b1010100, 128);
    ASSERT_NEAR(bv.min, glm::vec3(-64, 0, -32), 1e-1f);
    ASSERT_NEAR(bv.max, glm::vec3(-32, 32, 0), 1e-1f);
}

TEST(exercises, final)
{

}