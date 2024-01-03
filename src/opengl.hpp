#ifndef __OPENGL_HPP__
#define __OPENGL_HPP__




#include "math.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <string>

std::vector<glm::vec<4, unsigned char>> take_screenshoot(unsigned width, unsigned height);
std::vector<glm::vec<4, unsigned char>> save_screenshoot(unsigned width, unsigned height, std::string const& filename);

#endif // __OPENGL_HPP__
