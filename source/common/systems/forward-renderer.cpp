#include "forward-renderer.hpp"
#include "../mesh/mesh-utils.hpp"
#include "../texture/texture-utils.hpp"

namespace our
{

    void ForwardRenderer::initialize(glm::ivec2 windowSize, const nlohmann::json &config)
    {
        // First, we store the window size for later use
        this->windowSize = windowSize;

        // Then we check if there is a sky texture in the configuration
        if (config.contains("sky"))
        {
            // First, we create a sphere which will be used to draw the sky
            this->skySphere = mesh_utils::sphere(glm::ivec2(16, 16));

            // We can draw the sky using the same shader used to draw textured objects
            ShaderProgram *skyShader = new ShaderProgram();
            skyShader->attach("assets/shaders/textured.vert", GL_VERTEX_SHADER);
            skyShader->attach("assets/shaders/textured.frag", GL_FRAGMENT_SHADER);
            skyShader->link();

            // TODO: (Req 10) Pick the correct pipeline state to draw the sky
            //  Hints: the sky will be draw after the opaque objects so we would need depth testing but which depth funtion should we pick?
            //  We will draw the sphere from the inside, so what options should we pick for the face culling.
            PipelineState skyPipelineState{};
            skyPipelineState.depthTesting.enabled = true;
            skyPipelineState.depthTesting.function = GL_LEQUAL;
            skyPipelineState.faceCulling.enabled = true;

            // Draw the sphere from the inside
            skyPipelineState.faceCulling.culledFace = GL_FRONT;
            skyPipelineState.faceCulling.frontFace = GL_CCW;

            // Load the sky texture (note that we don't need mipmaps since we want to avoid any unnecessary blurring while rendering the sky)
            std::string skyTextureFile = config.value<std::string>("sky", "");
            Texture2D *skyTexture = texture_utils::loadImage(skyTextureFile, false);

            // Setup a sampler for the sky
            Sampler *skySampler = new Sampler();
            skySampler->set(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            skySampler->set(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            skySampler->set(GL_TEXTURE_WRAP_S, GL_REPEAT);
            skySampler->set(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            // Combine all the aforementioned objects (except the mesh) into a material
            this->skyMaterial = new TexturedMaterial();
            this->skyMaterial->shader = skyShader;
            this->skyMaterial->texture = skyTexture;
            this->skyMaterial->sampler = skySampler;
            this->skyMaterial->pipelineState = skyPipelineState;
            this->skyMaterial->tint = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
            this->skyMaterial->alphaThreshold = 1.0f;
            this->skyMaterial->transparent = false;
        }

        // Then we check if there is a postprocessing shader in the configuration
        if (config.contains("postprocess"))
        {
            // TODO: (Req 11) Create a framebuffer
            // From destroy Function &_^_^_

            glGenFramebuffers(1, &postprocessFrameBuffer);

            // TODO: (Req 11) Create a color and a depth texture and attach them to the framebuffer
            //  Hints: The color format can be (Red, Green, Blue and Alpha components with 8 bits for each channel).
            //  The depth format can be (Depth component with 24 bits).

            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, postprocessFrameBuffer);
            colorTarget = texture_utils::empty(GL_RGBA8, windowSize);
            depthTarget = texture_utils::empty(GL_DEPTH_COMPONENT24, windowSize);

            glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTarget->getOpenGLName(), 0);
            glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTarget->getOpenGLName(), 0);
            // TODO: (Req 11) Unbind the framebuffer just to be safe
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            // Create a vertex array to use for drawing the texture
            glGenVertexArrays(1, &postProcessVertexArray);

            // Create a sampler to use for sampling the scene texture in the post processing shader
            Sampler *postprocessSampler = new Sampler();
            postprocessSampler->set(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            postprocessSampler->set(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            postprocessSampler->set(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            postprocessSampler->set(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            // Create the post processing shader
            ShaderProgram *postprocessShader = new ShaderProgram();
            postprocessShader->attach("assets/shaders/fullscreen.vert", GL_VERTEX_SHADER);
            postprocessShader->attach(config.value<std::string>("postprocess", ""), GL_FRAGMENT_SHADER);
            postprocessShader->link();

            // Create a post processing material
            postprocessMaterial = new TexturedMaterial();
            postprocessMaterial->shader = postprocessShader;
            postprocessMaterial->texture = colorTarget;
            postprocessMaterial->sampler = postprocessSampler;
            // The default options are fine but we don't need to interact with the depth buffer
            // so it is more performant to disable the depth mask
            postprocessMaterial->pipelineState.depthMask = false;
        }
    }

    void ForwardRenderer::destroy()
    {
        // Delete all objects related to the sky
        if (skyMaterial)
        {
            delete skySphere;
            delete skyMaterial->shader;
            delete skyMaterial->texture;
            delete skyMaterial->sampler;
            delete skyMaterial;
        }
        // Delete all objects related to post processing
        if (postprocessMaterial)
        {
            glDeleteFramebuffers(1, &postprocessFrameBuffer);
            glDeleteVertexArrays(1, &postProcessVertexArray);
            delete colorTarget;
            delete depthTarget;
            delete postprocessMaterial->sampler;
            delete postprocessMaterial->shader;
            delete postprocessMaterial;
        }
    }

    void ForwardRenderer::render(World *world)
    {
        // First of all, we search for a camera and for all the mesh renderers
        CameraComponent *camera = nullptr;
        opaqueCommands.clear();
        transparentCommands.clear();
        for (auto entity : world->getEntities())
        {
            // If we hadn't found a camera yet, we look for a camera in this entity
            if (!camera)
                camera = entity->getComponent<CameraComponent>();
            // If this entity has a mesh renderer component
            if (auto meshRenderer = entity->getComponent<MeshRendererComponent>(); meshRenderer)
            {
                // We construct a command from it
                RenderCommand command;
                command.localToWorld = meshRenderer->getOwner()->getLocalToWorldMatrix();
                command.center = glm::vec3(command.localToWorld * glm::vec4(0, 0, 0, 1));
                command.mesh = meshRenderer->mesh;
                command.material = meshRenderer->material;
                // if it is transparent, we add it to the transparent commands list
                if (command.material->transparent)
                {
                    transparentCommands.push_back(command);
                }
                else
                {
                    // Otherwise, we add it to the opaque command list
                    opaqueCommands.push_back(command);
                }
            }
        }

        // If there is no camera, we return (we cannot render without a camera)
        if (camera == nullptr)
            return;

        // TODO: (Req 9) Modify the following line such that "cameraForward" contains a vector pointing the camera forward direction
        //  HINT: See how you wrote the CameraComponent::getViewMatrix, it should help you solve this one
        //  done  de ya abo heggi
        // 7bebe ya moo <3
        auto owner = camera->getOwner();
        auto M = owner->getLocalToWorldMatrix();
        glm::vec3 eye = glm::vec3(M * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
        glm::vec3 center = glm::vec3(M * glm::vec4(0.0f, 0.0f, -1.0f, 1.0f));
        glm::vec3 cameraForward = glm::normalize(center - eye);

        std::sort(transparentCommands.begin(), transparentCommands.end(), [cameraForward](const RenderCommand &first, const RenderCommand &second)
                  {
                      // TODO: (Req 9) Finish this function
                      //  HINT: the following return should return true "first" should be drawn before "second".
                      //  we calculate dot product of the center point along camera forward direction
                      float firstDistance = first.center.x * cameraForward.x + first.center.y * cameraForward.y + first.center.z * cameraForward.z;
                      float secondDistance = second.center.x * cameraForward.x + second.center.y * cameraForward.y + second.center.z * cameraForward.z;
                      // then we compare the 2 distance and put the further first (the logical operator here is >, but probably the cameraForward is inverted)
                      return firstDistance > secondDistance; });

        // TODO: (Req 9) Get the camera ViewProjection matrix and store it in VP
        // vp = camera->getProjectionMatrix(this->windowSize) * camera->getViewMatrix();

        glm::mat4 VP = camera->getProjectionMatrix(this->windowSize) * camera->getViewMatrix();
        // TODO: (Req 9) Set the OpenGL viewport using viewportStart and viewportSize

        // Parameters
        //     x, y
        //     Specify the lower left corner of the viewport rectangle, in pixels. The initial value is (0,0).

        //     width, height
        //     Specify the width and height of the viewport. When a GL context is first attached to a window, width and height are set to the dimensions of that window.

        //     Description
        //     glViewport specifies the affine transformation of x
        //     and y
        //     from normalized device coordinates to window coordinates. Let (xnd,ynd)
        //     be normalized device coordinates.

        glViewport(0, 0, this->windowSize.x, this->windowSize.y);
        // TODO: (Req 9) Set the clear color to black and the clear depth to 1

        // clear color and depth values
        glClearColor(0.0, 0.0, 0.0, 1.0);
        glClearDepth(1.0);

        // TODO: (Req 9) Set the color mask to true and the depth mask to true (to ensure the glClear will affect the framebuffer)
        // enables/disables writing on the depth buffer (enabled here)
        // controls whether writing to the depth buffer is enabled or disabled. The depth buffer is used to keep track of the depth of
        // each pixel on the screen to handle occlusion
        glDepthMask(GL_TRUE);
        // enable/disable writing colors in the frame buffer (enabled here)
        // This function allows you to selectively enable or disable writing of color components to the framebuffer.
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

        // If there is a postprocess material, bind the framebuffer
        if (postprocessMaterial)
        {
            // TODO: (Req 11) bind the framebuffer
            glBindFramebuffer(GL_FRAMEBUFFER, postprocessFrameBuffer);
        }

        // TODO: (Req 9) Clear the color and depth buffers
        // Enabling the depth test before clearing the color and depth buffers ensures that the depth buffer is properly managed during the rendering process

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // TODO: (Req 9) Draw all the opaque commands
        //  Don't forget to set the "transform" uniform to be equal the model-view-projection matrix for each render command
        for (RenderCommand opaqueCommand : this->opaqueCommands)
        {
            // set the "transform" uniform to be equal the model-view-projection matrix for each render command
            glm::mat4 transform = VP * opaqueCommand.localToWorld;
            opaqueCommand.material->setup();
            opaqueCommand.material->shader->set("transform", transform);
            opaqueCommand.mesh->draw();
        }
        // glEnable(GL_DEPTH_TEST);
        // If there is a sky material, draw the sky
        if (this->skyMaterial)
        {
            // TODO: (Req 10) setup the sky material

            this->skyMaterial->setup();

            // TODO: (Req 10) Get the camera position

            glm::vec3 cameraPosition = glm::vec3(camera->getOwner()->getLocalToWorldMatrix() * glm::vec4(0, 0, 0, 1));

            // TODO: (Req 10) Create a model matrix for the sy such that it always follows the camera (sky sphere center = camera position)

            // translate the identity matrix by cameraPosition whatever it goes and apply this to the sky sphere center
            glm::mat4 skySphereModel = glm::translate(glm::mat4(1.0), cameraPosition);

            // TODO: (Req 10) We want the sky to be drawn behind everything (in NDC space, z=1)
            //  We can acheive the is by multiplying by an extra matrix after the projection but what values should we put in it?
            // glm::mat4 alwaysBehindTransform = glm::mat4(
            //     1.0f, 0.0f, 0.0f, 0.0f,
            //     0.0f, 1.0f, 0.0f, 0.0f,
            //     0.0f, 0.0f, 1.0f, 0.0f,
            //     0.0f, 0.0f, 0.0f, 1.0f);
            glm::mat4 alwaysBehindTransform = glm::mat4(
                1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 1.0f);
            // TODO: (Req 10) set the "transform" uniform

            this->skyMaterial->shader->set("transform", alwaysBehindTransform * VP * skySphereModel);
            // TODO: (Req 10) draw the sky sphere
            this->skySphere->draw();
        }
        // TODO: (Req 9) Draw all the transparent commands
        //  Don't forget to set the "transform" uniform to be equal the model-view-projection matrix for each render command
        for (RenderCommand transparentCommand : this->transparentCommands)
        {
            // set the "transform" uniform to be equal the model-view-projection matrix for each render command
            glm::mat4 transform = VP * transparentCommand.localToWorld;
            transparentCommand.material->setup();
            transparentCommand.material->shader->set("transform", transform);
            transparentCommand.mesh->draw();
        }
        // If there is a postprocess material, apply postprocessing
        if (postprocessMaterial)
        {
            // TODO: (Req 11) Return to the default framebuffer
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            // TODO: (Req 11) Setup the postprocess material and draw the fullscreen triangle


            // Draw the fullscreen triangle
            // We don't need to bind a vertex buffer since we are using the vertex array
            glBindVertexArray(postProcessVertexArray);
            postprocessMaterial->setup();
            glDrawArrays(GL_TRIANGLES, 0, 3);
            glBindVertexArray(0);
        }
    }

}