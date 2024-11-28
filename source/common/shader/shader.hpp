#ifndef SHADER_HPP
#define SHADER_HPP

#include <string>

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace our
{

    class ShaderProgram
    {

    private:
        // Shader Program Handle (OpenGL object name)
        GLuint program;

    public:
        ShaderProgram()
        {
            // TODO: (Req 1) Create A shader program
            //   creates a new program object and returns its ID. A program object is used to link together multiple shader objects
            //    (such as vertex and fragment shaders) into a single, executable program that can be used by the GP
            program = glCreateProgram();
        }
        ~ShaderProgram()
        {
            // TODO: (Req 1) Delete a shader program
            glDeleteProgram(program);
        }

        bool attach(const std::string &filename, GLenum type) const;

        bool link() const;

        void use()
        {
            glUseProgram(program);
        }
        // The getUniformLocation function retrieves the location of a uniform variable within a shader program.
        // Uniform variables are used to pass data from your application to the shaders.
        //      Uniform variables in OpenGL are a type of variable used in shaders to pass data from your application (CPU) to the shaders (GPU). They are called "uniform" because they remain constant
        //      for all processed vertices or fragments during a single draw call. Unlike attribute variables, which can change per vertex, uniform variables
        //      are set once per draw call and are used by all vertices and fragments processed by the shaders.
        GLuint getUniformLocation(const std::string &name)
        {
            // TODO: (Req 1) Return the location of the uniform with the given name
            return glGetUniformLocation(program, name.c_str());
        }
        // A uniform variable in a shader is a global variable declared in the shader code, used to pass data from your application to the shader. It is:

        // Constant for all vertices/fragments during a single rendering call.
        // Set by the application using OpenGL or a similar API before rendering.
        // Used to send parameters like transformation matrices, light positions, or textures to the shader.
        void set(const std::string &uniform, GLfloat value)
        {
            // TODO: (Req 1) Send the given float value to the given uniform
            glUniform1f(getUniformLocation(uniform), value);
        }

        void set(const std::string &uniform, GLuint value)
        {
            // TODO: (Req 1) Send the given unsigned integer value to the given uniform
            glUniform1ui(getUniformLocation(uniform), value);
        }

        void set(const std::string &uniform, GLint value)
        {
            // TODO: (Req 1) Send the given integer value to the given uniform
            glUniform1i(getUniformLocation(uniform), value);
        }

        void set(const std::string &uniform, glm::vec2 value)
        {
            // TODO: (Req 1) Send the given 2D vector value to the given uniform
            glUniform2f(getUniformLocation(uniform), value[0], value[1]);
        }

        void set(const std::string &uniform, glm::vec3 value)
        {
            // TODO: (Req 1) Send the given 3D vector value to the given uniform
            glUniform3f(getUniformLocation(uniform), value[0], value[1], value[2]);
        }

        void set(const std::string &uniform, glm::vec4 value)
        {
            // TODO: (Req 1) Send the given 4D vector value to the given uniform
            glUniform4f(getUniformLocation(uniform), value[0], value[1], value[2], value[3]);
        }

        void set(const std::string &uniform, glm::mat4 matrix)
        {
            // TODO: (Req 1) Send the given matrix 4x4 value to the given uniform
            glUniformMatrix4fv(getUniformLocation(uniform), 1, GL_FALSE, glm::value_ptr(matrix));
        }

        // TODO: (Req 1) Delete the copy constructor and assignment operator.
        ShaderProgram(const ShaderProgram &) = delete;
        ShaderProgram &operator=(ShaderProgram const &) = delete;

        // Question: Why do we delete the copy constructor and assignment operator?
        //  deleting the copy constructor and assignment operator for a class is a way to prevent copying of objects of that class.
        //  This is often done for classes that manage resources that cannot be easily or safely copied
        //, such as file handles, network connections, or OpenGL shader programs.
        //  example : our::ShaderProgram::link()
    };

}

#endif