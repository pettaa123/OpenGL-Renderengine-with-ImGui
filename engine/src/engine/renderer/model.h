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
    protected:

        std::vector<Mesh> m_meshes;

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

        void init(std::vector<Engine::Mesh::Vertex>& vertices) {

            m_meshes.clear();

            std::shared_ptr<VertexBuffer> vertexBuffer(VertexBuffer::create());
            vertexBuffer->setData(vertices.data(), (uint32_t)vertices.size(), sizeof(Mesh::Vertex));

            Engine::BufferLayout layout = {
                { Engine::ShaderDataType::Float3, "a_Position" },
                { Engine::ShaderDataType::Float3, "a_Normal" },
                { Engine::ShaderDataType::Float3, "a_Tangent" },
                { Engine::ShaderDataType::Float3, "a_Binormal" },
                { Engine::ShaderDataType::Float2, "a_Texcoord" }
            };
            vertexBuffer->setLayout(layout);

            m_meshes.push_back(Mesh(vertexBuffer));
            /*

            std::shared_ptr<IndexBuffer> indexBuffer(IndexBuffer::create());
            indexBuffer->setData(indices->data(), (uint32_t)indices->size() * sizeof(Mesh::Index));

            // process materials
            aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
            // we assume a convention for sampler names in the shaders. Each diffuse texture should be named
            // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
            // Same applies to other texture as the following list summarizes:
            // diffuse: texture_diffuseN
            // specular: texture_specularN
            // normal: texture_normalN


            std::vector<std::pair<aiTextureType, std::string>> textureTypes{
                { aiTextureType_DIFFUSE, "texture_diffuse" },
                { aiTextureType_SPECULAR, "texture_specular" },
                { aiTextureType_HEIGHT, "texture_normal" },
                { aiTextureType_AMBIENT, "texture_height" }
            };
            for (auto type : textureTypes) {
                std::vector<std::shared_ptr<Texture2D>> textures;
                for (size_t i = 0; i < material->GetTextureCount(type.first); i++)
                {
                    aiString str;
                    material->GetTexture(type.first, (uint32_t)i, &str);

                    // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
                    bool skip = false;
                    for (unsigned int j = 0; j < textures_loaded.size(); j++)
                    {
                        if (std::strcmp(textures_loaded[j]->getName().data(), str.C_Str()) == 0)
                        {
                            textures.push_back(textures_loaded[j]);
                            skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
                            break;
                        }
                    }
                    if (!skip)
                    {   // if texture hasn't been loaded already, load it
                        std::shared_ptr<Texture2D> texture = Texture2D::create(directory + '/' + str.C_Str());

                        texture->setType(type.second);
                        //texture.id = TextureFromFile(str.C_Str(), this->directory);
                        textures.push_back(texture);
                        textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecessary load duplicate textures.
                    }
                }
                */
        }

        // processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
        void processNode(void* node, void* scene);

        Mesh processMesh(void* mesh, void* scene);

        // checks all material textures of a given type and loads the textures if they're not loaded yet.
        // the required info is returned as a Texture struct.
        // 
        //std::vector<Texture2D> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);

    private:

        std::string m_directory;

    };
}