#ifndef __PRIMITIVE_HPP__
#define __PRIMITIVE_HPP__

#include <vector>
#include <glm/vec3.hpp>
#include "shader.hpp"

class Primitive {
public:
	std::vector<glm::vec3> mVertices;
	ShaderProgram* mShader = nullptr;

	Primitive();
	~Primitive();

	void InitializeBuffers();
	void LoadShader(const char* vtxShaderPath, const char* fragShaderPath);

	void InitPoint();
	void InitSegment();
	void InitAABB();
	void InitTriangle(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c);
	void InitPlane();
	void InitSphereDisc();
	void InitFrustum();
	void Clear();
	unsigned VtxCount();
	void BindForDrawing();
private:
	GLuint mVAO = 0;
	GLuint mVBO = 0;
};

#endif // __PRIMITIVE_HPP__
