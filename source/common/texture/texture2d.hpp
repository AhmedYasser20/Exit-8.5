#pragma once

#include <glad/gl.h>

namespace our {

    // This class defined an OpenGL texture which will be used as a GL_TEXTURE_2D
    class Texture2D {
        // The OpenGL object name of this texture 
        GLuint name = 0;
    public:
        // This constructor creates an OpenGL texture and saves its object name in the member variable "name" 
        Texture2D() {
            //TODO: (Req 5) Complete this function
            glGenTextures(1, &name);    // generate a texture object name and store it in the member variable "name"
            // Note: 1st argument is the number of textures to generate, 2nd argument is a pointer to an array in which the generated texture names are stored
        };

        // This deconstructor deletes the underlying OpenGL texture
        ~Texture2D() { 
            //TODO: (Req 5) Complete this function
            glDeleteTextures(1, &name); // delete the texture object with the name stored in the member variable "name"
        }

        // Get the internal OpenGL name of the texture which is useful for use with framebuffers
        GLuint getOpenGLName() {
            return name;
        }

        // This method binds this texture to GL_TEXTURE_2D
        void bind() const {
            //TODO: (Req 5) Complete this function
            glBindTexture(GL_TEXTURE_2D, name); // bind the texture object with the name stored in the member variable "name" to the target GL_TEXTURE_2D
        }
        static void ActivateTexture0(){ // activate texture unit 0
            glActiveTexture(GL_TEXTURE0);
        }
        // This static method ensures that no texture is bound to GL_TEXTURE_2D
        static void unbind(){
            //TODO: (Req 5) Complete this function
            glBindTexture(GL_TEXTURE_2D, 0); // bind the texture object with the name 0 to the target GL_TEXTURE_2D
        }

        Texture2D(const Texture2D&) = delete;
        Texture2D& operator=(const Texture2D&) = delete;
    };
    
}