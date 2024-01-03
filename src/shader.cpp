#include <shader.hpp>
#include <fstream>
#include <sstream>

// ------------------------------------------------------------------------
/*! Default Constructor
*
*   Constructs a Shader
*/ // --------------------------------------------------------------------
Shader::Shader() noexcept :
	mHandle(NULL), ShaderType(EType::Vertex) {
}

// ------------------------------------------------------------------------
/*! Destrutor
*
*   Frees a Shader
*/ // --------------------------------------------------------------------
Shader::~Shader() noexcept {
	if (mHandle) glDeleteShader(mHandle);
}

// ------------------------------------------------------------------------
/*! Custom Constructor
*
*   Constructs a Shader given a file
*/ // --------------------------------------------------------------------
Shader::Shader(const std::string_view& filename, const EType type) :
	ShaderType(type), mHandle(NULL) {
	Compile(const_cast<char*>(filename.data()));
}

// ------------------------------------------------------------------------
/*! Load Source
*
*   Loads a Source for our Shader
*/ // --------------------------------------------------------------------
char* Shader::LoadSource(const std::string_view& filename) const noexcept {
	std::fstream shaderFile(filename.data());
	std::stringstream shaderSource;
	char* source;

	shaderSource << shaderFile.rdbuf();

	//If we could allocate the string
	if (source = reinterpret_cast<char*>(malloc(strlen(shaderSource.str().c_str()) + 1)))
		strcpy(source, shaderSource.str().c_str());
	else
		return nullptr;
	return source;
}

// ------------------------------------------------------------------------
/*! Compile
*
*   Compiles the Shader
*/ // --------------------------------------------------------------------
void Shader::Compile(char* source) {
	//If there is a valid source file
	if (source && strlen(source)) {
		CreateDeviceShader();
		glShaderSource(static_cast<GLuint>(mHandle), 1, &source, NULL);
		glCompileShader(static_cast<GLuint>(mHandle));

		// sanity check
		GLint result;
		glGetShaderiv(static_cast<GLuint>(mHandle), GL_COMPILE_STATUS, &result);

#ifdef _DEBUG
		//If there has been errors during compilation
		if (!result) throw ShaderException("Couldn't compile shader");
#endif
	}
	else
		throw ShaderException("Can't compile empty shader");
}

// ------------------------------------------------------------------------
/*! Set Shader Type
*
*   Sets the type of the shader
*/ // --------------------------------------------------------------------
void Shader::SetShaderType(Shader::EType shaderType, bool createDeviceShader) {
	ShaderType = shaderType;

	//If we need to create a device
	if (createDeviceShader)
		CreateDeviceShader();
}

GLuint Shader::GetGLHandle() const noexcept {
	return mHandle;
}

// ------------------------------------------------------------------------
		/*! Default Constructor
		*
		*   Constructs a Shader Program Class
		*/ // --------------------------------------------------------------------
ShaderProgram::ShaderProgram() noexcept :
	mHandle(glCreateProgram()) {}

// ------------------------------------------------------------------------
/*! Destructor
*
*   Deallocates a program from the GPU
*/ // --------------------------------------------------------------------
ShaderProgram::~ShaderProgram() noexcept {
	glDeleteProgram(mHandle);
}

// ------------------------------------------------------------------------
/*! Custom Constructor
*
*   Constructs a Shader Program class given a Vertex and Fragment Shader
*/ // ---------------------------------------------------------------------
ShaderProgram::ShaderProgram(const Shader& vertexShader, const Shader& fragmentShader)
	: ShaderProgram() {
	//If both of the assets contain valid data
	glAttachShader(mHandle, vertexShader.GetGLHandle());
	glAttachShader(mHandle, fragmentShader.GetGLHandle());
	glLinkProgram(mHandle);

	GLint status;
	glGetProgramiv(mHandle, GL_LINK_STATUS, &status);

	if (status == GL_FALSE) throw ShaderProgramException("Shader Program Failed to Link");
}

// ------------------------------------------------------------------------
/*! Get Uniform Location
*
*  Get the location of an Uniform within a shader
*/ // --------------------------------------------------------------------
GLuint ShaderProgram::getUniformLocation(const std::string_view& id) {
	const std::unordered_map<std::string_view, GLuint>::local_iterator idx = mUniformLocations.find(id);

#if _DEBUG
	int i = idx == mUniformLocations.end() ? (mUniformLocations[id] = glGetUniformLocation(mHandle, id.data())) : idx->second;

	if (i == -1) throw ShaderProgramException("There is no such uniform in the shader");

	return i;
#else
	return idx == mUniformLocations.end() ? (mUniformLocations[id] = glGetUniformLocation(mHandle, id.data())) : idx->second;
#endif
}