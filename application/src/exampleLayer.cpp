#include "exampleLayer.h"
#include "imgui/imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

ExampleLayer::ExampleLayer()  //m_cameraController(1280.0f / 720.0f)
	: Layer("ExampleLayer"), m_cameraController(glm::perspectiveFov(glm::radians(45.0f), 1280.0f, 720.0f, 0.1f, 10000.0f))
{
	/*
	m_vertexArray = Engine::OpenGLVertexArray::create();

	float vertices[4 * 7] = {
		-0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
		 0.5f, -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,
		 0.5f, 0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,
		 -0.5f,  0.5f, 0.0f, 0.1f, 0.9f, 0.2f, 1.0f
	};

	std::shared_ptr<Engine::VertexBuffer> vertexBuffer = Engine::VertexBuffer::create(vertices, sizeof(vertices));
	Engine::BufferLayout layout = {
		{ Engine::ShaderDataType::Float3, "a_position" },
		{ Engine::ShaderDataType::Float4, "a_color" }
	};
	vertexBuffer->setLayout(layout);
	m_vertexArray->addVertexBuffer(vertexBuffer);

	uint32_t indices[6] = { 0, 1, 2, 2 , 3, 0 };
	std::shared_ptr<Engine::IndexBuffer> indexBuffer = Engine::IndexBuffer::create(indices, sizeof(indices) / sizeof(uint32_t));
	m_vertexArray->setIndexBuffer(indexBuffer);
	*/



	/*
	std::string vertexSrc = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec4 a_Color;
			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;
			out vec3 v_Position;
			out vec4 v_Color;
			void main()
			{
				v_Position = a_Position;
				v_Color = a_Color;
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);	
			}
		)";

	std::string fragmentSrc = R"(
			#version 330 core
			
			layout(location = 0) out vec4 color;
			in vec3 v_Position;
			in vec4 v_Color;
			void main()
			{
				color = vec4(v_Position * 0.5 + 0.5, 1.0);
				color = v_Color;
			}
		)";

	m_shader = Engine::Shader::create("VertexPosColor", vertexSrc, fragmentSrc);

	std::string flatColorShaderVertexSrc = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position;
			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;
			out vec3 v_Position;
			void main()
			{
				v_Position = a_Position;
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);	
			}
		)";

	std::string flatColorShaderFragmentSrc = R"(
			#version 330 core
			
			layout(location = 0) out vec4 color;
			in vec3 v_Position;
			
			uniform vec3 u_Color;
			void main()
			{
				color = vec4(u_Color, 1.0);
			}
		)";

	m_flatColorShader = Engine::Shader::create("FlatColor", flatColorShaderVertexSrc, flatColorShaderFragmentSrc);
	
	std::string textureShaderVertexSrc = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec2 a_TexCoord;
			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;

			out vec2 v_TexCoord;

			void main()
			{
				v_TexCoord = a_TexCoord;
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);	
			}
		)";

	std::string textureShaderFragmentSrc = R"(
			#version 330 core
			
			layout(location = 0) out vec4 color;

			in vec2 v_TexCoord;
			
			uniform sampler2D u_Texture;

			void main()
			{
				color = texture(u_Texture , v_TexCoord);
			}
		)";
		
	m_textureShader = Engine::Shader::create("Texture", textureShaderVertexSrc, textureShaderFragmentSrc);
	

	//
	m_texture = Engine::Texture2D::create("assets/textures/Checkerboard.png");
	m_chernoLogoTexture = Engine::Texture2D::create("assets/textures/ChernoLogo.png");
	//
	m_textureShader->bind();
	m_textureShader->setInt("u_Texture", 0);

	*/
	
	//m_mesh.reset(new Engine::Mesh("assets/meshes/cerberus.fbx"));

}

void ExampleLayer::onAttach()
{
	//m_quadShader.reset(Engine::Shader::create("assets/shaders/quad.glsl"));
	//m_meshVA = Engine::OpenGLVertexArray::create();

	//m_squareVA = Engine::OpenGLVertexArray::create();
	////vertices with texturecoordinates
	//float squareVertices[5 * 4] = {
	//	-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
	//	 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
	//	 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
	//	-0.5f,  0.5f, 0.0f, 0.0f, 1.0f
	//};
	//
	//std::shared_ptr<Engine::VertexBuffer> squareVB = Engine::VertexBuffer::create(squareVertices, sizeof(squareVertices));
	//squareVB->setLayout({
	//	{ Engine::ShaderDataType::Float3, "a_Position" },
	//	{ Engine::ShaderDataType::Float2, "a_TexCoord" }
	//	});
	//m_squareVA->addVertexBuffer(squareVB);
	//
	//uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
	//std::shared_ptr<Engine::IndexBuffer> squareIB = Engine::IndexBuffer::create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t));
	//m_squareVA->setIndexBuffer(squareIB);
	//

	auto loadedShader = m_shaderLibrary.load("assets/shaders/model_loading.glsl");

	m_backpack.reset(new Engine::Model("assets/models/backpack/backpack.obj"));
	//m_mesh.reset(new Engine::Mesh("assets/meshes/cerberus.fbx"));
}

void ExampleLayer::onDetach()
{

}

void ExampleLayer::onUpdate(Engine::Timestep ts) 
{
	// Update
	//m_camera.onUpdate(ts);
	m_cameraController.onUpdate(ts);

	// Render
	Engine::RenderCommand::setClearColor({ m_squareColor, 1 });
	Engine::RenderCommand::clear();

	Engine::Renderer::beginScene(m_cameraController);

	//glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));

	//m_flatColorShader->bind();
	
	
	auto loadedShader = m_shaderLibrary.get("model_loading");;
	//loadedShader->setFloat3("u_Color", m_squareColor);

	//for (int y = 0; y < 20; y++)
	//{
	//	for (int x = 0; x < 20; x++)
	//	{
	//		glm::vec3 pos(x * 0.11f, y * 0.11f, 0.0f);
	//		glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * scale;
	//		//Engine::Renderer::submit(m_flatColorShader, m_squareVA, transform);
	//		Engine::Renderer::submit(loadedShader, m_squareVA, transform);
	//	}
	//}
	
	


	//m_texture->bind();
	//Engine::Renderer::submit(m_textureShader, m_squareVA, glm::scale(glm::mat4(1.0f), glm::vec3(0.5f)));
	//
	//m_chernoLogoTexture->bind();
	//Engine::Renderer::submit(m_textureShader, m_squareVA, glm::translate(glm::mat4(1.0f), glm::vec3(0.4f,-0.25f,0)));
	
	// Triangle
	//Engine::Renderer::submit(m_shader, m_vertexArray);


	//Engine::Renderer::submit(m_textureShader, m_squareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.0f)));
	//Engine::Renderer::submit(loadedShader, m_vertexArray);
	
	//make Models submittable
	


	Engine::Renderer::submit(loadedShader, m_backpack.get());


	Engine::Renderer::endScene();
}

void ExampleLayer::onImGuiRender() 
{
	ImGui::Begin("Settings");
	ImGui::ColorEdit3("Square Color", glm::value_ptr(m_squareColor));
	ImGui::End();
}

void ExampleLayer::onEvent(Engine::Event& e) 
{
	//m_camera.onEvent(e);
	m_cameraController.onEvent(e);
}
