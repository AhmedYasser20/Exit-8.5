#pragma once

#include <glad/gl.h>
#include <glm/vec4.hpp>
#include <json/json.hpp>

namespace our
{
    // There are some options in the render pipeline that we cannot control via shaders
    // such as blending, depth testing and so on
    // Since each material could require different options (e.g. transparent materials usually use blending),
    // we will encapsulate all these options into a single structure that will also be responsible for configuring OpenGL's pipeline
    struct PipelineState
    {
        // This set of pipeline options specifies whether face culling will be used or not and how it will be configured
        struct
        {
            bool enabled = false;
            GLenum culledFace = GL_BACK;
            GLenum frontFace = GL_CCW;
        } faceCulling;

        // This set of pipeline options specifies whether depth testing will be used or not and how it will be configured
        struct
        {
            bool enabled = false;
            GLenum function = GL_LEQUAL;
        } depthTesting;

        // This set of pipeline options specifies whether blending will be used or not and how it will be configured
        struct
        {
            bool enabled = false;
            GLenum equation = GL_FUNC_ADD;
            GLenum sourceFactor = GL_SRC_ALPHA;
            GLenum destinationFactor = GL_ONE_MINUS_SRC_ALPHA;
            glm::vec4 constantColor = {0, 0, 0, 0};
        } blending;

        // These options specify the color and depth mask which can be used to
        // prevent the rendering/clearing from modifying certain channels of certain targets in the framebuffer
        glm::bvec4 colorMask = {true, true, true, true}; // To know how to use it, check glColorMask
        bool depthMask = true;                           // To know how to use it, check glDepthMask

        // This function should set the OpenGL options to the values specified by this structure
        // For example, if faceCulling.enabled is true, you should call glEnable(GL_CULL_FACE), otherwise, you should call glDisable(GL_CULL_FACE)
        void setup() const
        {
            // TODO: (Req 4) Write this function

            glColorMask(colorMask.x, colorMask.y, colorMask.z, colorMask.w); // enable/disable writing of frame buffer color components
            glDepthMask(depthMask);                                          // enable/disable writing to the depth buffer

            // For example, if faceCulling.enabled is true, you should call glEnable(GL_CULL_FACE), otherwise, you should call glDisable(GL_CULL_FACE)
            if (faceCulling.enabled) // check if face culling is enabled
            {
                glEnable(GL_CULL_FACE);             // all the faces that are not front faces are discarded when enabling the culled face
                glCullFace(faceCulling.culledFace); // specify that we want to culled the back faces
                glFrontFace(faceCulling.frontFace); // determine rather we want clockwise faces or counter clockwise faces to not be culled
            }
            else
            {
                glDisable(GL_CULL_FACE);
            }

            // The depth buffer is a buffer that stores the depth of each fragment
            // The depth testing is a technique used to determine whether a fragment is visible or not
            if (depthTesting.enabled) // check if depth testing is enabled
            {
                // When depth testing is enabled, the depth of the fragment is compared with the depth value stored in the depth buffer
                // If the depth of the fragment is less than the stored depth value, the fragment is visible
                // If the depth of the fragment is greater than the stored depth value, the fragment is not visible
                glEnable(GL_DEPTH_TEST); // enable depth testing
                // OpenGL provides a set of comparison functions that can be used to determine whether a fragment is visible or not
                // This allows us to specify how the depth of the fragment should be compared with the depth value stored in the depth buffer
                glDepthFunc(depthTesting.function); // specify the depth comparison function
            }
            else
            {
                glDisable(GL_DEPTH_TEST); // disable depth testing
            }

            // Blending is a technique used to combine the color of a fragment with the color of the fragment that is already in the frame buffer
            // This is useful for rendering transparent objects
            if (blending.enabled) // check if blending is enabled
            {
                glEnable(GL_BLEND); // enable blending
                // The blending equation is used to determine how the colors of the source and destination fragments should be combined before being using them in the blending function
                glBlendEquation(blending.equation); // specify the blending equation
                // The blending function is used to determine how the colors of the source and destination fragments should be combined
                glBlendFunc(blending.sourceFactor, blending.destinationFactor); // specify the blending function
                // The constant color is used to specify a constant color that should be used when blending
                glBlendColor(blending.constantColor.r, blending.constantColor.g, blending.constantColor.b, blending.constantColor.a); // specify the constant color
            }
            else
            {
                glDisable(GL_BLEND); // disable blending
            }
        }

        // Given a json object, this function deserializes a PipelineState structure
        void deserialize(const nlohmann::json &data);
    };

}