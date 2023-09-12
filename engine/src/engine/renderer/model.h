#pragma once

#include <engine/renderer/mesh.h>
#include <engine/renderer/shader.h>

#include <string>
#include <map>
#include <vector>
#include <numeric>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <glad/glad.h> 



namespace Engine {

    //unsigned int TextureFromFile(const char* path, const std::string& directory, bool gamma = false);

    class Model
    {
    public:

        // constructor, expects a filepath to a 3D model.
        Model(const std::string& path, bool gamma = false);

        // model data 
        std::vector<std::shared_ptr<Texture2D>> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
        std::string directory;
        bool gammaCorrection;


        const std::vector<Mesh>& getMeshes() const;

        // draws the model, and thus all its meshes
        void draw(Shader& shader);

        // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
        void loadModel(const std::string& path);

        // processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
        void processNode(void* node, void* scene);

        Mesh processMesh(void* mesh, void* scene);

        // checks all material textures of a given type and loads the textures if they're not loaded yet.
        // the required info is returned as a Texture struct.
        // 
        //std::vector<Texture2D> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);

    private:

        std::string m_directory;

    protected:

        std::vector<Mesh> m_meshes;

    };
}