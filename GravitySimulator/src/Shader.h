#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>

class Shader {
public:
    // Program ID
    GLuint ID;
    
    // Constructor
    Shader();
    Shader(const char* vertexSource, const char* fragmentSource);
    
    // Load from file
    bool loadFromFile(const char* vertexPath, const char* fragmentPath);
    
    // Load from string
    bool loadFromString(const char* vertexSource, const char* fragmentSource);
    
    // Use/activate the shader
    void use() const;
    
    // Utility uniform functions
    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setVec2(const std::string& name, const glm::vec2& value) const;
    void setVec3(const std::string& name, const glm::vec3& value) const;
    void setVec4(const std::string& name, const glm::vec4& value) const;
    void setMat3(const std::string& name, const glm::mat3& mat) const;
    void setMat4(const std::string& name, const glm::mat4& mat) const;
    
    // Cleanup
    void destroy();
    
private:
    // Utility functions
    GLuint compileShader(const char* source, GLenum type);
    bool linkProgram(GLuint vertexShader, GLuint fragmentShader);
    void checkCompileErrors(GLuint shader, const std::string& type);
};

#endif