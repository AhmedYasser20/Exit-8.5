#pragma once
#include <application.hpp>

#include <ecs/world.hpp>
#include <systems/forward-renderer.hpp>
#include <systems/free-camera-controller.hpp>
#include <systems/movement.hpp>
#include <asset-loader.hpp>
#include <components/light.hpp>
#include <components/camera.hpp>

// This state shows how to use the ECS framework and deserialization.
class WinState : public our::State
{

    our::World world;
    our::ForwardRenderer renderer;
    our::FreeCameraControllerSystem cameraController;
    our::MovementSystem movementSystem;

    void onInitialize() override
    {
        // First of all, we get the scene configuration from the app config
        auto &config = getApp()->getConfig()["scene"];
        // If we have assets in the scene config, we deserialize them
        if (config.contains("assets"))
        {
            our::deserializeAllAssets(config["assets"]);
        }
        if (config.contains("physicsWorld"))
        {
            world.deserialize_physics(config["physicsWorld"]);
        }
        // If we have a world in the scene config, we use it to populate our world
        if (config.contains("world5"))
        {
            world.deserialize(config["world5"]);
        }
        // We initialize the camera controller system since it needs a pointer to the app
        cameraController.enter(getApp());
        // Then we initialize the renderer
        auto size = getApp()->getFrameBufferSize();
        renderer.initialize(size, config["renderer"]);
    }

    void onDraw(double deltaTime) override
    {
        // Here, we just run a bunch of systems to control the world logic
        movementSystem.update(&world, (float)deltaTime);
        cameraController.update(&world, (float)deltaTime);
        // And finally we use the renderer system to draw the scene
        renderer.render(&world);

        // Get a reference to the keyboard object
        auto &keyboard = getApp()->getKeyboard();

        // red door
        if (world.getDoor() != -1 && world.getDoor() != 1)
        {
            world.setDoor(-1);

            getApp()->changeState("level1");
        }
        // green door
        else if (world.getDoor() != -1 && world.getDoor() != 2)
        {
            world.setDoor(-1);

            getApp()->changeState("level2");
        }
        // Torch
        if (keyboard.justPressed(GLFW_KEY_F))
        {
            // Find camera entity and its light component
            for (auto entity : world.getEntities())
            {
                if (auto camera = entity->getComponent<our::CameraComponent>())
                {
                    if (auto light = entity->getComponent<our::lightComponent>())
                    {
                        light->toggle();
                    }
                    break;
                }
            }
        }
        if (keyboard.justPressed(GLFW_KEY_ESCAPE))
        {
            // If the escape  key is pressed in this frame, go to the play state
            getApp()->changeState("menu");
        }
    }

    void onDestroy() override
    {
        // Don't forget to destroy the renderer
        renderer.destroy();
        // On exit, we call exit for the camera controller system to make sure that the mouse is unlocked
        cameraController.exit();
        // Clear the world
        world.clear();
        // and we delete all the loaded assets to free memory on the RAM and the VRAM
        our::clearAllAssets();
    }
};