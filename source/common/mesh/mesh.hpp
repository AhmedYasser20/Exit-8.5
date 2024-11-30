#pragma once

#include <glad/gl.h>
#include "vertex.hpp"

namespace our
{

#define ATTRIB_LOC_POSITION 0
#define ATTRIB_LOC_COLOR 1
#define ATTRIB_LOC_TEXCOORD 2
#define ATTRIB_LOC_NORMAL 3

    class Mesh
    {
        // Here, we store the object names of the 3 main components of a mesh:
        // A vertex array object, A vertex buffer and an element buffer
        unsigned int VBO, EBO;
        unsigned int VAO;
        // We need to remember the number of elements that will be draw by glDrawElements
        GLsizei elementCount;

    public:
        // The constructor takes two vectors:
        // - vertices which contain the vertex data.
        // - elements which contain the indices of the vertices out of which each rectangle will be constructed.

        // The mesh class does not keep a these data on the RAM. Instead, it should create
        // a vertex buffer to store the vertex data on the VRAM,
        // an element buffer to store the element data on the VRAM,
        // a vertex array object to define how to read the vertex & element buffer during rendering
        Mesh(const std::vector<Vertex> &vertices, const std::vector<unsigned int> &elements)
        {
            // TODO: (Req 2) Write this function

            // Generate the VAO
            // Bind the VAO
            glGenVertexArrays(1, &VAO);
            glBindVertexArray(VAO);

            // Generate the VBO
            // Bind the VBO
            // Copy the vertices data to the VBO
            // The VBO is used to store the vertex data
            // The vertices data is copied to the VBO so that it can be used by the GPU during rendering
            // The GL_STATIC_DRAW flag is used to tell OpenGL that the data will not be changed
            // This allows OpenGL to optimize the data for faster rendering
            glGenBuffers(1, &VBO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

            elementCount = static_cast<GLsizei>(elements.size());

            // Generate the EBO
            // Bind the EBO
            // Copy the elements data to the EBO
            // The EBO is used to store the indices of the vertices that will be used to draw the triangles
            // The elements data is copied to the EBO so that it can be used by the GPU during rendering
            // The GL_STATIC_DRAW flag is used to tell OpenGL that the data will not be changed
            // This allows OpenGL to optimize the data for faster rendering
            glGenBuffers(1, &EBO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, elementCount * sizeof(unsigned int), elements.data(), GL_STATIC_DRAW);

            // Tell OpenGL how to read Attribute Values from the Buf
            // The glVertexAttribPointer function tells OpenGL how to read the vertex data from the VBO
            // The first parameter is the attribute location in the shader
            // The second parameter is the number of components per attribute
            // The third parameter is the data type of each component
            // The fourth parameter is whether the data should be normalized
            // The fifth parameter is the stride between consecutive attributes
            // The last parameter is the offset of the first component
            // The glEnableVertexAttribArray function tells OpenGL to enable the attribute location
            // This allows the attribute location to be used during rendering
            glVertexAttribPointer(ATTRIB_LOC_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, position));
            // Make The position attribute Available during rendering
            // The position attribute is available at location 0
            glEnableVertexAttribArray(ATTRIB_LOC_POSITION);

            
            glVertexAttribPointer(ATTRIB_LOC_COLOR, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (void *)offsetof(Vertex, color));
            glEnableVertexAttribArray(ATTRIB_LOC_COLOR);

            glVertexAttribPointer(ATTRIB_LOC_TEXCOORD, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, tex_coord));
            glEnableVertexAttribArray(ATTRIB_LOC_TEXCOORD);

            glVertexAttribPointer(ATTRIB_LOC_NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, normal));
            glEnableVertexAttribArray(ATTRIB_LOC_NORMAL);

            // Unbind the VAO
            // This is a good practice to avoid accidentally modifying the state of the currently bound VAO elsewhere in the code
            glBindVertexArray(0);

            // remember to store the number of elements in "elementCount" since you will need it for drawing
            // For the attribute locations, use the constants defined above: ATTRIB_LOC_POSITION, ATTRIB_LOC_COLOR, etc
        }

        // this function should render the mesh
        void draw()
        {
            // TODO: (Req 2) Write this function
            glBindVertexArray(VAO);
            // This function renders primitives (in this case, triangles) from array data using indices.
            glDrawElements(GL_TRIANGLES, elementCount, GL_UNSIGNED_INT, 0);
            // This function unbinds the currently bound VAO by binding to VAO 0. This is a good practice
            // to avoid accidentally modifying the state of the currently bound VAO elsewhere in the code.
            glBindVertexArray(0);
        }

        // this function should delete the vertex & element buffers and the vertex array object
        ~Mesh()
        {
            // TODO: (Req 2) Write this function
            // Delete the VBO, EBO, and VAO
            // The glDeleteBuffers function is used to delete the VBO and EBO
            // The glDeleteVertexArrays function is used to delete the VAO
            // The first parameter is the number of buffers/arrays
            // The second parameter is the address of the buffer/array
            // The VBO, EBO, and VAO are deleted to free the memory used by them
            // This is a good practice to avoid memory leaks
            
            glDeleteBuffers(1, &VBO);
            glDeleteBuffers(1, &EBO);
            glDeleteVertexArrays(1, &VAO);
        }

        Mesh(Mesh const &) = delete;
        Mesh &operator=(Mesh const &) = delete;
    };

}