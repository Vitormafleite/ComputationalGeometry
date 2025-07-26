#include "../headers/Shader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

Shader::Shader() : ID(0) {}

Shader::~Shader() {
    if (ID) {
        glDeleteProgram(ID);
    }
}

std::string Shader::readFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Error: Failed to open shader file: " << filepath << std::endl;
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

bool Shader::compileShader(const std::string& source, GLenum type, GLuint& shaderID) {

    shaderID = glCreateShader(type);

    if (shaderID == 0) {
        std::cerr << "Failed to create shader of type " << type << "\n";
        return false;
    }

    const char* src = source.c_str();
    glShaderSource(shaderID, 1, &src, nullptr);
    glCompileShader(shaderID);

    GLint success;
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[512];
        glGetShaderInfoLog(shaderID, 512, nullptr, log);
        std::cerr << "Shader compile error: " << log << "\n";
        return false;
    }

    return true;
}



bool Shader::load(const std::string& vertexPath, const std::string& fragmentPath) {
    std::string vertexSrc = readFile(vertexPath);
    std::string fragmentSrc = readFile(fragmentPath);

    if (vertexSrc.empty() || fragmentSrc.empty()) {
        std::cerr << "Error: One or both shaders failed to load." << std::endl;
        return false;
    }

    GLuint vertexShader, fragmentShader;

    if (!compileShader(vertexSrc, GL_VERTEX_SHADER, vertexShader)) return false;
    if (!compileShader(fragmentSrc, GL_FRAGMENT_SHADER, fragmentShader)) return false;

    ID = glCreateProgram();
    glAttachShader(ID, vertexShader);
    glAttachShader(ID, fragmentShader);
    glLinkProgram(ID);

    int success;
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success) {
        char log[512];
        glGetProgramInfoLog(ID, 512, nullptr, log);
        std::cerr << "Shader link error: " << log << "\n";
        return false;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return true;
}

void Shader::use() const {
    glUseProgram(ID);
}

GLuint Shader::getID() const {
    return ID;
}

void Shader::setMat4(const std::string& name, const glm::mat4& mat) const {
    GLint loc = glGetUniformLocation(ID, name.c_str());
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(mat));
}


void Shader::setBool(const std::string& name, bool boolean) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)boolean);
}

void Shader::cleanup(){
    if (ID != 0) {
        glDeleteProgram(ID);
        ID = 0;
    }
}
