#ifndef GLSLPROGRAM_H
#define GLSLPROGRAM_H

#ifdef WIN32
#pragma warning(disable : 4290)
#endif

#include <string>
using std::string;
#include <map>

#include <glm/glm.hpp>
using glm::mat3;
using glm::mat4;
using glm::vec2;
using glm::vec3;
using glm::vec4;

#include <GL/glew.h>

#include <stdexcept>

class GLSLProgramException : public std::runtime_error {
public:
    GLSLProgramException(const string& msg)
        : std::runtime_error(msg)
    {
    }
};

namespace GLSLShader {
enum GLSLShaderType {
    VERTEX = GL_VERTEX_SHADER,
    FRAGMENT = GL_FRAGMENT_SHADER,
    GEOMETRY = GL_GEOMETRY_SHADER,
    TESS_CONTROL = GL_TESS_CONTROL_SHADER,
    TESS_EVALUATION = GL_TESS_EVALUATION_SHADER,
    COMPUTE = GL_COMPUTE_SHADER
};
};

class GLSLProgram {
private:
    int handle;
    bool linked;
    std::map<string, int> uniformLocations;

    bool fileExists(const string& fileName);
    string getExtension(const char* fileName);

    // Make these private in order to make the object non-copyable
public:
    GLSLProgram();
    ~GLSLProgram();

    GLint getUniformLocation(const char* name);
    void compileShader(const char* fileName) noexcept;
    void compileShader(const char* fileName, GLSLShader::GLSLShaderType type) noexcept;
    void compileShader(const string& source, GLSLShader::GLSLShaderType type,
        const char* fileName = NULL) noexcept;

    void link() noexcept;
    void validate() noexcept;
    void use() noexcept;

    int getHandle();
    bool isLinked();

    void bindAttribLocation(GLuint location, const char* name);
    void bindFragDataLocation(GLuint location, const char* name);

    void setUniform(const char* name, float x, float y, float z);
    void setUniform(const char* name, const vec2& v);
    void setUniform(const char* name, const vec3& v);
    void setUniform(const char* name, const vec4& v);
    void setUniform(const char* name, const mat4& m);
    void setUniform(const char* name, const mat3& m);
    void setUniform(const char* name, float val);
    void setUniform(const char* name, int val);
    void setUniform(const char* name, bool val);
    void setUniform(const char* name, GLuint val);

    void printActiveUniforms();
    void printActiveUniformBlocks();
    void printActiveAttribs();

    const char* getTypeString(GLenum type);
};

#endif // GLSLPROGRAM_H
