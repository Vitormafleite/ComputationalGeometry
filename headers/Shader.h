#ifndef SHADER_H
#define SHADER_H

#include <string>
#include "../external/glad/include/glad/glad.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Shader {
public:
    Shader();
    ~Shader();

    bool load(const std::string& vertexPath, const std::string& fragmentPath);
    void use() const;
    GLuint getID() const;

    void setMat4(const std::string& name, const glm::mat4& mat) const;

private:
    GLuint ID;
    bool compileShader(const std::string& source, GLenum type, GLuint& shaderID);
    std::string readFile(const std::string& filepath);
};

#endif // SHADER_H

