#include "shader.hpp"

#include <cassert>
#include <iostream>
#include <fstream>
#include <string>

// Forward definition for error checking functions
std::string checkForShaderCompilationErrors(GLuint shader);
std::string checkForLinkingErrors(GLuint program);

bool our::ShaderProgram::attach(const std::string &filename, GLenum type) const
{
    // Here, we open the file and read a string from it containing the GLSL code of our shader
    std::ifstream file(filename);
    if (!file)
    {
        std::cerr << "ERROR: Couldn't open shader file: " << filename << std::endl;
        return false;
    }
    std::string sourceString = std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
    const char *sourceCStr = sourceString.c_str();
    file.close();

    // TODO: Complete this function
    // Note: The function "checkForShaderCompilationErrors" checks if there is
    //  an error in the given shader. You should use it to check if there is a
    //  compilation error and print it so that you can know what is wrong with
    //  the shader. The returned string will be empty if there is no errors.

    // reates a new shader object and returns its ID. This shader object can
    // then be used to compile shader source code of a specific type (e.g., vertex shader, fragment shader).
    GLuint shader = glCreateShader(type);

    // is used to set the source code for a shader object. This function specifies the source code that will be compiled for the shader.
    // papameters:
    //  shader: The ID of the shader object whose source code is to be replaced.
    //  count: The number of strings in the string array.
    //  string: An array of pointers to strings containing the source code.
    //  length: An array of lengths of the strings, or NULL if the strings are null-terminated.

    // The source code for the shader is stored in the sourceCStr variable.
    glShaderSource(shader, 1, &sourceCStr, nullptr);

    // is used to compile a shader object. This function compiles the source code strings that have been stored in the shader object specified by shader.
    // The compilation status will be stored as part of the shader object's state.
    // This status will be set to GL_TRUE if the shader was compiled without errors and is ready for use, and GL_FALSE otherwise.
    // It is possible that a shader object may fail to compile for a variety of reasons. The compile status will be set to GL_FALSE if an error occurs during the compilation process.
    // The source code is then compiled by the GPU.
    glCompileShader(shader);

    // Check and return any error in the compilation process
    std::string error = checkForShaderCompilationErrors(shader);
    // If there is an error, print it and return false
    // Otherwise, attach the shader to the program and return true
    if (!error.empty())
    {
        std::cerr << "ERROR: Couldn't compile shader: " << filename << std::endl;
        std::cerr << error << std::endl;
        // is used to delete the shader object specified by shader.
        // This function deletes the shader object specified by shader. 
        //This frees the memory and invalidates the name associated with the shader object.
        // If a shader object is attached to a program object, it will be flagged for deletion, but it won't be deleted until it is no longer attached to any program object.
        // If shader is 0, the function has no effect.
        glDeleteShader(shader);
        return false;
    }

    // is used to attach a shader object to a program object.
    // This indicates that shader will be included in link operations that will be performed on program.
    // The shader object is then attached to the program object.
    glAttachShader(program, shader);

    // is used to delete the shader object specified by shader.
    glDeleteShader(shader);

    // We return true if the compilation succeeded
    return true;
}

bool our::ShaderProgram::link() const
{
    // TODO: Complete this function
    // Note: The function "checkForLinkingErrors" checks if there is
    //  an error in the given program. You should use it to check if there is a
    //  linking error and print it so that you can know what is wrong with the
    //  program. The returned string will be empty if there is no errors.

    // Linking creates an executable that can be used by the GPU to render graphics.
    // program: The ID of the program object to be linked. contains the vertex and fragment shaders.
    glLinkProgram(program);

    // Check and return any error in the linking process
    // is used to retrieve information about the program object specified by program.
    std::string error = checkForLinkingErrors(program);
    // If there is an error, print it and return false
    // Otherwise, return true
    if (!error.empty())
    {
        std::cerr << "ERROR: Couldn't link shader program" << std::endl;
        std::cerr << error << std::endl;
        return false;
    }
    return true;
}

////////////////////////////////////////////////////////////////////
// Function to check for compilation and linking error in shaders //
////////////////////////////////////////////////////////////////////

std::string checkForShaderCompilationErrors(GLuint shader)
{
    // Check and return any error in the compilation process
    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (!status)
    {
        GLint length;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
        char *logStr = new char[length];
        glGetShaderInfoLog(shader, length, nullptr, logStr);
        std::string errorLog(logStr);
        delete[] logStr;
        return errorLog;
    }
    return std::string();
}

std::string checkForLinkingErrors(GLuint program)
{
    // Check and return any error in the linking process
    GLint status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (!status)
    {
        GLint length;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
        char *logStr = new char[length];
        glGetProgramInfoLog(program, length, nullptr, logStr);
        std::string error(logStr);
        delete[] logStr;
        return error;
    }
    return std::string();
}