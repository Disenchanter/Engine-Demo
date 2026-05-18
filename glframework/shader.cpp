#include"shader.h"
#include"../wrapper/checkError.h"

#include<string>
#include<fstream>
#include<sstream>
#include<iostream>

Shader::Shader(const char* vertexPath, const char* fragmentPath) {
	// Load shader source code from files into strings / ¥·¥§©`¥À¥½©`¥¹¤ò¥Õ¥¡¥¤¥ë¤«¤éÕi¤ßÞz¤ß
	std::string vertexCode;
	std::string fragmentCode;

	// File streams for reading vertex/fragment shader files
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;

	// Enable exceptions for file IO
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try {
		// 1: Open files
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		
		// 2: Read file contents into stringstreams
		std::stringstream vShaderStream, fShaderStream;
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();

		// 3: Close files
		vShaderFile.close();
		fShaderFile.close();

		// 4: Convert stringstreams to strings
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	}
	catch (std::ifstream::failure& e) {
		std::cout << "ERROR: Shader File Error: " << e.what() << std::endl;
	}

	const char* vertexShaderSource = vertexCode.c_str();
	const char* fragmentShaderSource = fragmentCode.c_str();
	// 1: Create shader objects (vertex and fragment)
	GLuint vertex, fragment;
	vertex = glCreateShader(GL_VERTEX_SHADER);
	fragment = glCreateShader(GL_FRAGMENT_SHADER);

	// 2: Attach source to shaders
	glShaderSource(vertex, 1, &vertexShaderSource, NULL);
	glShaderSource(fragment, 1, &fragmentShaderSource, NULL);

	// 3: Compile shaders
	glCompileShader(vertex);
	// Check vertex compile errors
	checkShaderErrors(vertex, "COMPILE");
	
	glCompileShader(fragment);
	// Check fragment compile errors
	checkShaderErrors(fragment, "COMPILE");
	
	// 4: Create program
	mProgram = glCreateProgram();

	// 6: Attach shaders to program
	glAttachShader(mProgram, vertex);
	glAttachShader(mProgram, fragment);

	// 7: Link program (this will link compiled shaders)
	glLinkProgram(mProgram);

	// Check link errors
	checkShaderErrors(mProgram, "LINK");

	// Cleanup shader objects
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}
Shader::~Shader() {

}

void Shader::begin() {
	GL_CALL(glUseProgram(mProgram));
}

void Shader::end() {
	GL_CALL(glUseProgram(0));
}

void Shader::setFloat(const std::string& name, float value) {
	// 1: Get uniform location
	GLint location = GL_CALL(glGetUniformLocation(mProgram, name.c_str()));

	// 2: Set float uniform
	GL_CALL(glUniform1f(location, value));
}

void Shader::setVector3(const std::string& name, float x, float y, float z) {
	// 1: Get uniform location
	GLint location = GL_CALL(glGetUniformLocation(mProgram, name.c_str()));
	
	// 2: Set vec3 uniform
	GL_CALL(glUniform3f(location, x, y, z));
}

// Overload: set vec3 from array
void Shader::setVector3(const std::string& name, const float* values) {
	// 1: Get uniform location
	GLint location = GL_CALL(glGetUniformLocation(mProgram, name.c_str()));

	// 2: Set vec3 uniform by pointer
	GL_CALL(glUniform3fv(location, 1, values));
}

void Shader::setVector3(const std::string& name, const glm::vec3 value) {
	// 1: Get uniform location
	GLint location = GL_CALL(glGetUniformLocation(mProgram, name.c_str()));

	// 2: Set vec3 uniform
	GL_CALL(glUniform3f(location, value.x, value.y, value.z));
}

void Shader::setInt(const std::string& name, int value) {
	// 1: Get uniform location
	GLint location = GL_CALL(glGetUniformLocation(mProgram, name.c_str()));

	// 2: Set int uniform
	glUniform1i(location, value);
}

void Shader::setMatrix4x4(const std::string& name, glm::mat4 value) {
	// 1: Get uniform location
	GLint location = GL_CALL(glGetUniformLocation(mProgram, name.c_str()));
	
	// 2: Set matrix uniform (no transpose)
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::setMatrix3x3(const std::string& name, glm::mat3 value) {
	// 1: Get uniform location
	GLint location = GL_CALL(glGetUniformLocation(mProgram, name.c_str()));

	// 2: Set matrix uniform (no transpose)
	glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(value));
}




void Shader::checkShaderErrors(GLuint target, std::string type) {
	int success = 0;
	char infoLog[1024];

	if (type == "COMPILE") {
		glGetShaderiv(target, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(target, 1024, NULL, infoLog);
			std::cout << "Error: SHADER COMPILE ERROR" << "\n" << infoLog << std::endl;
		}
	}
	else if (type == "LINK") {
		glGetProgramiv(target, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(target, 1024, NULL, infoLog);
			std::cout << "Error: SHADER LINK ERROR " << "\n" << infoLog << std::endl;
		}
	}
	else {
		std::cout << "Error: Check shader errors Type is wrong" << std::endl;
	}
}