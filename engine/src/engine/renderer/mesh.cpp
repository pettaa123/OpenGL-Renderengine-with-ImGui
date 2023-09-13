#include "mesh.h"

#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>
#include <assimp/DefaultLogger.hpp>
#include <assimp/LogStream.hpp>

#include <glad/glad.h>

#include "engine/core/log.h"


namespace Engine {

	namespace {
		const unsigned int ImportFlags =
			aiProcess_CalcTangentSpace |
			aiProcess_Triangulate |
			aiProcess_SortByPType |
			aiProcess_PreTransformVertices |
			aiProcess_GenNormals |
			aiProcess_GenUVCoords |
			aiProcess_OptimizeMeshes |
			aiProcess_Debone |
			aiProcess_ValidateDataStructure;
	}

	struct LogStream : public Assimp::LogStream
	{
		static void initialize()
		{
			if (Assimp::DefaultLogger::isNullLogger())
			{
				Assimp::DefaultLogger::create("", Assimp::Logger::VERBOSE);
				Assimp::DefaultLogger::get()->attachStream(new LogStream, Assimp::Logger::Err | Assimp::Logger::Warn);
			}
		}

		void write(const char* message) override
		{
			Log::error(std::format("Assimp error: {0}", message));
		}
	};

	// constructor
	Mesh::Mesh(std::shared_ptr<std::vector<Vertex>> vertices, std::shared_ptr<std::vector<Index>> indices, std::shared_ptr<VertexBuffer> vertexBuffer, std::shared_ptr<IndexBuffer> indexBuffer, std::vector<std::shared_ptr<Texture2D>> textures):
		m_vertices(vertices),
		m_indices(indices),
		m_vertexBuffer(vertexBuffer),
		m_indexBuffer(indexBuffer),
		textures(textures)
	{
		m_VAO = Engine::VertexArray::create();
		m_VAO->addVertexBuffer(m_vertexBuffer);
		m_VAO->setIndexBuffer(m_indexBuffer);
		// now that we have all the required data, set the vertex buffers and its attribute pointers.
		//this was already done by setlayout?
		//setupMesh();
	};

	// constructor
	Mesh::Mesh(std::shared_ptr<VertexBuffer> vertexBuffer) :
		m_vertexBuffer(vertexBuffer),
		m_indexBuffer(nullptr)
	{
		m_VAO = Engine::VertexArray::create();
		m_VAO->addVertexBuffer(m_vertexBuffer);
		// now that we have all the required data, set the vertex buffers and its attribute pointers.
		//this was already done by setlayout?
		setupMesh();
	};

	Mesh::Mesh(const std::string& filename)
		: m_filePath(filename),
		m_vertices(std::make_shared<std::vector<Engine::Mesh::Vertex>>()),
		m_indices(std::make_shared<std::vector<Engine::Mesh::Index>>())
	{
		LogStream::initialize();

		Log::info(std::format("Loading mesh: {0}", filename.c_str()));

		Assimp::Importer importer;

		const aiScene* scene = importer.ReadFile(filename, ImportFlags);
		if (!scene || !scene->HasMeshes())
			Log::error(std::format("Failed to load mesh file: {0}", filename));

		aiMesh* mesh = scene->mMeshes[0];

		assert(mesh->HasPositions() && "Meshes require positions.");
		assert(mesh->HasNormals() && "Meshes require normals.");
		
		m_vertices->reserve(mesh->mNumVertices);

		// Extract vertices from model
		for (size_t i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex vertex;
			vertex.position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
			vertex.normal = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };

			if (mesh->HasTangentsAndBitangents())
			{
				vertex.tangent = { mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z };
				vertex.binormal = { mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z };
			}

			if (mesh->HasTextureCoords(0))
				vertex.texCoords = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };
			m_vertices->push_back(vertex);
		}
		m_vertexBuffer.reset(VertexBuffer::create());
		m_vertexBuffer->setData(m_vertices->data(),(uint32_t)(m_vertices->size()),(uint32_t)sizeof(Vertex));

		Engine::BufferLayout layout = {
			{ Engine::ShaderDataType::Float3, "a_Position" },
			{ Engine::ShaderDataType::Float3, "a_Normal" },
			{ Engine::ShaderDataType::Float3, "a_Tangent" },
			{ Engine::ShaderDataType::Float3, "a_Binormal" },
			{ Engine::ShaderDataType::Float2, "a_TexCoords" }
		};
		m_vertexBuffer->setLayout(layout);

		// Extract indices from model
		m_indices->reserve(mesh->mNumFaces);
		for (size_t i = 0; i < mesh->mNumFaces; i++)
		{
			assert(mesh->mFaces[i].mNumIndices == 3 && "Must have 3 indices.");
			aiFace& face = mesh->mFaces[i];
				m_indices->push_back({ face.mIndices[0], face.mIndices[1], face.mIndices[2] });
		}

		m_indexBuffer.reset(IndexBuffer::create());
		m_indexBuffer->setData(m_indices->data(), (uint32_t)(m_indices->size() * sizeof(Index)));

		m_VAO = Engine::VertexArray::create();
		m_VAO->addVertexBuffer(m_vertexBuffer);
		m_VAO->setIndexBuffer(m_indexBuffer);
	}


	Mesh::~Mesh()
	{
		
	}


	void Mesh::setupMesh()
	{
		m_VAO->bind();
		// TODO: Sort this out
		m_vertexBuffer->bind();
		if(m_indexBuffer.get())
			m_indexBuffer->bind();

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, position));

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, normal));

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, tangent));

		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, binormal));

		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, texCoords));
		
		glBindVertexArray(0);		
	}

	void Mesh::draw(Shader& shader)
	{
		unsigned int diffuseNr = 1;
		unsigned int specularNr = 1;
		for (unsigned int i = 0; i < textures.size(); i++)
		{
			glActiveTexture(GL_TEXTURE0 + i); // activate texture unit first
			// retrieve texture number (the N in diffuse_textureN)
			std::string number;
			std::string name = textures[i]->getType();
			if (name == "texture_diffuse")
				number = std::to_string(diffuseNr++);
			else if (name == "texture_specular")
				number = std::to_string(specularNr++);
			shader.setInt(("material." + name + number).c_str(), i);
			glBindTexture(GL_TEXTURE_2D, textures[i]->getRendererID());
		}
		glActiveTexture(GL_TEXTURE0);
		// draw mesh
		m_VAO->bind();

		if (m_indexBuffer.get()) {
			uint32_t count = m_VAO->getIndexBuffer()->getCount();
			glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, 0);
		}
		else{
			uint32_t count = m_vertexBuffer->getCount();
			glDrawArrays(GL_TRIANGLES, 0, count);
		}

		glBindVertexArray(0);

		// always good practice to set everything back to defaults once configured.
		glActiveTexture(GL_TEXTURE0);
	}

}