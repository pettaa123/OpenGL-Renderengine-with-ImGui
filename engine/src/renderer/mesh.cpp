#include "mesh.h"

#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>
#include <assimp/DefaultLogger.hpp>
#include <assimp/LogStream.hpp>

#include <glad/glad.h>

#include "core/log.h"


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

	Mesh::Mesh(const std::string& filename)
		: m_filePath(filename)
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

		m_vertices.reserve(mesh->mNumVertices);

		// Extract vertices from model
		for (size_t i = 0; i < m_vertices.capacity(); i++)
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
				vertex.texcoord = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };
			m_vertices.push_back(vertex);
		}
		m_vertexBuffer.reset(VertexBuffer::create());
		m_vertexBuffer->setData(m_vertices.data(), m_vertices.size() * sizeof(Vertex));

		//glm::vec3 position;
		//glm::vec3 normal;
		//glm::vec3 tangent;
		//glm::vec3 binormal;
		//glm::vec2 texcoord;

		Engine::BufferLayout layout = {
			{ Engine::ShaderDataType::Float3, "a_Position" },
			{ Engine::ShaderDataType::Float3, "a_Normal" },
			{ Engine::ShaderDataType::Float3, "a_Tangent" },
			{ Engine::ShaderDataType::Float3, "a_Binormal" },
			{ Engine::ShaderDataType::Float2, "a_Texcoord" }
		};
		m_vertexBuffer->setLayout(layout);

		// Extract indices from model
		m_indices.reserve(mesh->mNumFaces);
		for (size_t i = 0; i < m_indices.capacity(); i++)
		{
			assert(mesh->mFaces[i].mNumIndices == 3 && "Must have 3 indices.");
			m_indices.push_back({ mesh->mFaces[i].mIndices[0], mesh->mFaces[i].mIndices[1], mesh->mFaces[i].mIndices[2] });
		}

		m_indexBuffer.reset(IndexBuffer::create());
		m_indexBuffer->setData(m_indices.data(), m_indices.size() * sizeof(Index));

		m_VAO = Engine::VertexArray::create();
		m_VAO->addVertexBuffer(m_vertexBuffer);
		m_VAO->setIndexBuffer(m_indexBuffer);

		setupMesh();
	}

	Mesh::~Mesh()
	{
	}

	void Mesh::setupMesh()
	{
		m_VAO->bind();
		// TODO: Sort this out
		m_vertexBuffer->bind();
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
		glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, texcoord));
		
		glBindVertexArray(0);		
	}
	/*
	void Mesh::draw(std::shared_ptr<Shader> shader)
	{
		/*
		unsigned int diffuseNr = 1;
		unsigned int specularNr = 1;
		for (unsigned int i = 0; i < textures.size(); i++)
		{
			glActiveTexture(GL_TEXTURE0 + i); // activate proper texture unit before binding
			// retrieve texture number (the N in diffuse_textureN)
			string number;
			string name = textures[i].type;
			if (name == "texture_diffuse")
				number = std::to_string(diffuseNr++);
			else if (name == "texture_specular")
				number = std::to_string(specularNr++);

			shader.setInt(("material." + name + number).c_str(), i);
			glBindTexture(GL_TEXTURE_2D, textures[i].id);
		}
		glActiveTexture(GL_TEXTURE0);
		
		// draw mesh
		glBindVertexArray(m_VAO);
		glDrawElements(GL_TRIANGLES, m_indexBuffer->getCount(), GL_UNSIGNED_INT, 0);
		//glBindVertexArray(0);
	}
	*/

}