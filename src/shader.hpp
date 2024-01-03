#ifndef __SHADER_HPP__
#define __SHADER_HPP__

#include "opengl.hpp"
#include <string_view>
#include <exception>
#include <unordered_map>

class Shader {

friend class ShaderProgram;
public:
#pragma region //Declarations
	struct ShaderException : public std::exception {
		ShaderException(const char* message) : std::exception(message) {}
	};

	enum class EType : unsigned char {
		Vertex,
		Fragment
	};
#pragma endregion

#pragma region //Constructors & Destructor
	Shader() noexcept;
	Shader(const std::string_view& filename, const EType type);
	~Shader() noexcept;
#pragma endregion

#pragma region //Methods
	void SetShaderType(const EType type, const bool createDeviceShader = false);
	GLuint GetGLHandle() const noexcept;

private:
	char* LoadSource(const std::string_view& filename) const noexcept;
	void Compile(char* source);
	void inline CreateDeviceShader() noexcept;
#pragma endregion

#pragma region //Members
	GLuint mHandle;
	EType ShaderType;
#pragma endregion
};

class ShaderProgram {
#pragma region //Declarations
public:
	struct ShaderProgramException : public std::exception {
		ShaderProgramException(const char* message) : std::exception(message) {}
	};
#pragma endregion

#pragma region //Constructors
		ShaderProgram() noexcept;
	ShaderProgram(const Shader& vertexShader, const Shader& fragmentShader);
	~ShaderProgram() noexcept;
#pragma endregion

#pragma region //Methods
	void inline Bind() const noexcept;
	void inline SetShaderUniform(const std::string_view& name, const int value);
	void inline SetShaderUniform(const std::string_view& name, const bool value);
	void inline SetShaderUniform(const std::string_view& name, int* value, const int count = 1);
	void inline SetShaderUniform(const std::string_view& name, float* value, const int count = 1);
	void inline SetShaderUniform(const std::string_view& name, glm::mat4* value, const int count = 1);
	void inline SetShaderUniform(const std::string_view& name, glm::mat3* value, const int count = 1);
	void inline SetShaderUniform(const std::string_view& name, glm::vec2* value, const int count = 1);
	void inline SetShaderUniform(const std::string_view& name, glm::vec3* value, const int count = 1);
	void inline SetShaderUniform(const std::string_view& name, glm::vec4* value, const int count = 1);
	GLuint inline GetGLHandle() const noexcept;
private:
	GLuint getUniformLocation(const std::string_view& id);
#pragma endregion

#pragma region //Members
	std::unordered_map<std::string_view, GLuint> mUniformLocations;
	unsigned int mHandle;
#pragma endregion
};

inline GLuint ShaderProgram::GetGLHandle() const noexcept {
	return mHandle;
}

// ------------------------------------------------------------------------
/*! Create Device Shader
*
*   Creates a Device for the Shader
*/ // --------------------------------------------------------------------
void Shader::CreateDeviceShader() noexcept {
	mHandle = glCreateShader(ShaderType == EType::Vertex ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER);
}

// ------------------------------------------------------------------------
/*! Set Shader Uniform
*
*   Sets uniform values in form of an integer
*/ // --------------------------------------------------------------------
void ShaderProgram::SetShaderUniform(const std::string_view& name, const int value) {
	glUniform1i(getUniformLocation(name), value);
}

// ------------------------------------------------------------------------
/*! Set Shader Uniform
*
*   Sets uniform values in form of a boolean
*/ // --------------------------------------------------------------------
void ShaderProgram::SetShaderUniform(const std::string_view& name, const bool value) {
	glUniform1i(getUniformLocation(name), value);
}

// ------------------------------------------------------------------------
/*! Set Shader Uniform
*
*   Sets uniform values in form of integers
*/ // --------------------------------------------------------------------
void ShaderProgram::SetShaderUniform(const std::string_view& name, int* value, const int count) {
	glUniform1iv(getUniformLocation(name), count, value);
}

// ------------------------------------------------------------------------
/*! Set Shader Uniform
*
*   Sets uniform values in form of floats
*/ // --------------------------------------------------------------------
void ShaderProgram::SetShaderUniform(const std::string_view& name, float* value, const int count) {
	glUniform1fv(getUniformLocation(name), count, value);
}

// ------------------------------------------------------------------------
/*! Set Shader Uniform
*
*   Sets uniform values in form of Matrices 4x4
*/ // --------------------------------------------------------------------
void ShaderProgram::SetShaderUniform(const std::string_view& name, glm::mat4* value, const int count) {
	glUniformMatrix4fv(getUniformLocation(name), count, GL_FALSE, reinterpret_cast<float*>(value));
}

// ------------------------------------------------------------------------
/*! Set Shader Uniform
*
*   Sets uniform values in form of matrices 3x3
*/ // --------------------------------------------------------------------
void ShaderProgram::SetShaderUniform(const std::string_view& name, glm::mat3* value, const int count) {
	glUniformMatrix4fv(getUniformLocation(name), count, GL_FALSE, reinterpret_cast<float*>(value));
}

// ------------------------------------------------------------------------
/*! Set Shader Uniform
*
*   Sets uniform values in form of Vectors of size 2
*/ // --------------------------------------------------------------------
void ShaderProgram::SetShaderUniform(const std::string_view& name, glm::vec2* value, const int count) {
	glUniform2fv(getUniformLocation(name), count, reinterpret_cast<float*>(value));
}

// ------------------------------------------------------------------------
/*! Set Shader Uniform
*
*   Sets uniform values in form of Vectors of size 3
*/ // --------------------------------------------------------------------
void ShaderProgram::SetShaderUniform(const std::string_view& name, glm::vec3* value, const int count) {
	glUniform3fv(getUniformLocation(name), count, reinterpret_cast<float*>(value));
}

// ------------------------------------------------------------------------
/*! Set Shader Uniform
*
*   Sets uniform values in form of Vectors of size 4
*/ // --------------------------------------------------------------------
void ShaderProgram::SetShaderUniform(const std::string_view& name, glm::vec4* value, const int count) {
	glUniform4fv(getUniformLocation(name), count, reinterpret_cast<float*>(value));
}

// ------------------------------------------------------------------------
/*! Bind
*
*   Binds the Shader Program
*/ // --------------------------------------------------------------------
void ShaderProgram::Bind() const noexcept {
	glUseProgram(mHandle);
}

#endif // __SHADER_HPP__
