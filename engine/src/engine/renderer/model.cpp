
#include <fstream>
#include <sstream>
#include <iostream>
#include "engine/renderer/model.h"
#include "engine/core/log.h"
#include "stb_image/stb_image.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"




namespace Engine {
	// constructor, expects a filepath to a 3D model.
	Model::Model(const std::string& path, bool gamma)
		: gammaCorrection(gamma)
	{
		loadModel(path);
	}

	// draws the model, and thus all its meshes
	void Model::draw(Shader& shader)
	{
		for (uint32_t i = 0; i < meshes.size(); i++)
			meshes[i].draw(shader);
	}

	// loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
	void Model::loadModel(const std::string& path)
	{
		// read file via ASSIMP
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_SortByPType | aiProcess_PreTransformVertices | aiProcess_GenNormals | aiProcess_GenUVCoords | aiProcess_OptimizeMeshes | aiProcess_Debone | aiProcess_ValidateDataStructure);
		// check for errors
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
		{
			Log::error(std::format("ERROR::ASSIMP:: {0}", importer.GetErrorString()));
			return;
		}
		// retrieve the directory path of the filepath
		directory = path.substr(0, path.find_last_of('/'));

		// process ASSIMP's root node recursively
		processNode(scene->mRootNode, (void*)scene);
	};

	// processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
	void Model::processNode(void* node_in, void* scene_in)
	{
		aiNode* node = static_cast<aiNode*>(node_in);
		const aiScene* scene = static_cast<const aiScene*>(scene_in);

		// process each mesh located at the current node
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			// the node object only contains indices to index the actual objects in the scene. 
			// the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			meshes.push_back(processMesh(mesh, (void*)scene));
		}
		// after we've processed all of the meshes (if any) we then recursively process each of the children nodes
		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			processNode(node->mChildren[i], (void*)scene);
		}

	};

	Mesh Model::processMesh(void* mesh_in, void* scene_in)
	{
		aiMesh* mesh = static_cast<aiMesh*>(mesh_in);
		const aiScene* scene = static_cast<const aiScene*>(scene_in);

		// data to fill
		std::vector<Mesh::Vertex> vertices;
		std::vector<Mesh::Index> indices;

		assert(mesh->HasPositions() && "Mesh requires positions.");
		assert(mesh->HasNormals() && "Mesh requires normals.");

		vertices.reserve(mesh->mNumVertices);

		// walk through each of the mesh's vertices
		for (size_t i = 0; i < vertices.capacity(); i++)
		{
			Mesh::Vertex vertex;
			vertex.position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
			vertex.normal = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };

			if (mesh->HasTangentsAndBitangents())
			{
				vertex.tangent = { mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z };
				vertex.binormal = { mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z };
			}

			if (mesh->HasTextureCoords(0)) {
				vertex.texCoords = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };
			}

			vertices.push_back(vertex);
		}

		m_vertexBuffer.reset(VertexBuffer::create());
		m_vertexBuffer->setData(vertices.data(), (uint32_t)vertices.size() * sizeof(Mesh::Vertex));

		Engine::BufferLayout layout = {
			{ Engine::ShaderDataType::Float3, "a_Position" },
			{ Engine::ShaderDataType::Float3, "a_Normal" },
			{ Engine::ShaderDataType::Float3, "a_Tangent" },
			{ Engine::ShaderDataType::Float3, "a_Binormal" },
			{ Engine::ShaderDataType::Float2, "a_Texcoord" }
		};
		m_vertexBuffer->setLayout(layout);

		// now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
		for (size_t i = 0; i < mesh->mNumFaces; i++)
		{
			assert(mesh->mFaces[i].mNumIndices == 3 && "Must have 3 indices.");
			Mesh::Index index = { mesh->mFaces[i].mIndices[0], mesh->mFaces[i].mIndices[1], mesh->mFaces[i].mIndices[2] };
			indices.push_back(index);
		}

		m_indexBuffer.reset(IndexBuffer::create());
		m_indexBuffer->setData(indices.data(), (uint32_t)indices.size() * sizeof(Mesh::Index));

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
		}
		/*
		// 1. diffuse maps
		std::vector<Texture2D> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		// 2. specular maps
		std::vector<Mesh::Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		// 3. normal maps
		std::vector<Mesh::Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
		// 4. height maps
		std::vector<Mesh::Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
		textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
		*/
		// return a mesh object created from the extracted mesh data

		return Mesh(m_vertexBuffer, m_indexBuffer, textures_loaded);
	}

	uint32_t Model::getVerticesCount() const{
		auto acc_lambda = [&](uint32_t count,const Mesh& m) {return count + m.getVerticesCount(); };
		uint32_t count = std::accumulate(meshes.begin(), meshes.end(), 0, acc_lambda);
		return count;
	}


	/*
	unsigned int TextureFromFile(const char* path, const std::string& directory, bool gamma)
	{
		std::string filename = std::string(path);
		filename = directory + '/' + filename;

		unsigned int textureID;
		glGenTextures(1, &textureID);

		int width, height, nrComponents;
		unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
		if (data)
		{
			GLenum format;
			if (nrComponents == 1)
				format = GL_RED;
			else if (nrComponents == 3)
				format = GL_RGB;
			else if (nrComponents == 4)
				format = GL_RGBA;

			glBindTexture(GL_TEXTURE_2D, textureID);
			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			stbi_image_free(data);
		}
		else
		{
			std::cout << "Texture failed to load at path: " << path << std::endl;
			stbi_image_free(data);
		}

		return textureID;
	}
	*/
};


