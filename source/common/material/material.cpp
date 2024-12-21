#include "material.hpp"

#include "../asset-loader.hpp"
#include "deserialize-utils.hpp"

namespace our {

    // This function should setup the pipeline state and set the shader to be used
    void Material::setup() const {
        //TODO: (Req 7) Write this function
        this->pipelineState.setup();
        if(shader) {  // Check if shader exists
            shader->use();  // Now we can safely use it
        } else {
            throw std::runtime_error("Attempting to setup material with no shader");
        }
    }

    // This function read the material data from a json object
    void Material::deserialize(const nlohmann::json& data){
        if(!data.is_object()) return;

        if(data.contains("pipelineState")){
            pipelineState.deserialize(data["pipelineState"]);
        }
        shader = AssetLoader<ShaderProgram>::get(data["shader"].get<std::string>());
        transparent = data.value("transparent", false);
    }

    // This function should call the setup of its parent and
    // set the "tint" uniform to the value in the member variable tint 
    void TintedMaterial::setup() const {
        //TODO: (Req 7) Write this function
        Material::setup(); 
        shader->set("tint",tint);
    }

    // This function read the material data from a json object
    void TintedMaterial::deserialize(const nlohmann::json& data){
        Material::deserialize(data);
        if(!data.is_object()) return;
        tint = data.value("tint", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    }

    // This function should call the setup of its parent and
    // set the "alphaThreshold" uniform to the value in the member variable alphaThreshold
    // Then it should bind the texture and sampler to a texture unit and send the unit number to the uniform variable "tex" 
    void TexturedMaterial::setup() const {
        TintedMaterial::setup();
        if(!shader || !texture){
            throw std::runtime_error("Attempting to setup textured material with missing components");
            return ;
        }
        // Sets a uniform variable named "alphaThreshold" in the shader to the value of the member variable `alphaThreshold`. This is typically used for transparency or alpha testing in shaders.
        shader->set("alphaThreshold", alphaThreshold);    
        // Activates the texture unit 0. This is necessary before binding a texture to a texture unit.
        texture->ActivateTexture0();    
        // Binds the texture to the currently active texture unit (which is texture unit 0 in this case). This makes the texture available for use in the shader.
        texture->bind();

        // Binds the sampler to texture unit 0. A sampler is used to control how the texture is sampled (e.g., filtering, wrapping).
        if(sampler) // not always a sampler is provided
            sampler->bind(0);
        // Sets a uniform variable named "tex" in the shader to the value 0. This tells the shader to use the texture bound to texture unit 0.
        shader->set("tex", 0);
    }

    // This function read the material data from a json object
    void TexturedMaterial::deserialize(const nlohmann::json& data){
        TintedMaterial::deserialize(data);
        if(!data.is_object()) return;
        alphaThreshold = data.value("alphaThreshold", 0.0f);
        texture = AssetLoader<Texture2D>::get(data.value("texture", ""));
        sampler = AssetLoader<Sampler>::get(data.value("sampler", ""));
    }

    void LitMaterial::setup() const {
        TexturedMaterial::setup();        
        if(ambient_occlusion){
            glActiveTexture(GL_TEXTURE1);
            ambient_occlusion->bind();             
            if(sampler)
                sampler->bind(1);            
            shader->set("material.ambient_occlusion", 1 );
        }
        if(specular){
            glActiveTexture(GL_TEXTURE2);
            specular->bind();             
            if(sampler)
                sampler->bind(2);            
            shader->set("material.specular", 2);            
        }
        if(roughness){
            glActiveTexture(GL_TEXTURE3);
            roughness->bind();             
            if(sampler)
                sampler->bind(3);            
            shader->set("material.roughness", 3);            
        }
    }
    void LitMaterial::deserialize(const nlohmann::json& data) {
        TexturedMaterial::deserialize(data);
        ambient_occlusion = AssetLoader<Texture2D>::get(data.value("ambient_occlusion", ""));

        std::string namespec = data.value("specular", "");
        std::cout << namespec << std::endl;   
        specular = AssetLoader<Texture2D>::get(namespec);
        roughness = AssetLoader<Texture2D>::get(data.value("roughness", ""));
            
    }

}