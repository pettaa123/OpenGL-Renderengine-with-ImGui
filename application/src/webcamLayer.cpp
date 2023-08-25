#include "webcamLayer.h"
#include "imgui/imgui.h"
#include <opencv2/highgui.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <opencv2/videoio.hpp>
#include "core/window.h"


WebcamLayer::WebcamLayer()  //m_cameraController(1280.0f / 720.0f)
	: Layer("WebcamLayer"), m_cameraController(glm::perspectiveFov(glm::radians(45.0f), 1280.0f,720.0f, 0.1f, 10000.0f))
{
}

void WebcamLayer::onAttach()
{

	m_webcam = std::make_unique<Webcam>(0,0);
	m_webcam->updateFrame(m_frame);

	int frameWidth = m_frame.cols;
	int frameHeight = m_frame.rows;
	Engine::Application::s_get().getWindow().setWindowSize(frameWidth,frameHeight);

	m_cameraController.setProjectionMatrix(glm::mat4(
		glm::perspectiveFov(glm::radians(45.0f),(float)frameWidth,(float)frameHeight,0.1f,10000.0f
		)));

	Engine::RenderCommand::setViewport(0,0,(uint32_t)frameWidth, (uint32_t)frameHeight);
	Log::info(std::format("webcam frame channels: {0} depth: {1}", m_frame.channels(), m_frame.depth()));

	assert(m_frame.channels() == 3 && "Webcam Image Format not supported");
	assert(m_frame.depth() == CV_8U && "Webcam Image Format not supported");
	Engine::ImageFormat format = Engine::ImageFormat::RGB8;

		
	m_webcamFeedTexture = Engine::Texture2D::create(Engine::TextureSpecification{ (uint32_t)m_frame.cols, (uint32_t)m_frame.rows, format} );
	//size is correct if mat.isContinuous() https://stackoverflow.com/questions/26441072/finding-the-size-in-bytes-of-cvmat
	assert(m_frame.isContinuous() && "mat not continuous");
	m_webcamFeedTexture->setData((void*)m_frame.data, m_frame.total() * m_frame.elemSize());

	//auto loadedShader = m_shaderLibrary.load("assets/shaders/model_loading.glsl");

	//m_3dObject.reset(new Engine::Model("assets/models/backpack/backpack.obj"));

	m_squareVA = Engine::OpenGLVertexArray::create();
	m_squareVA->bind();
	//vertices with texturecoordinates
	float squareVertices[5 * 4] = {
		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
		 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
		-1.0f,  1.0f, 0.0f, 0.0f, 1.0f
	};
	std::shared_ptr<Engine::VertexBuffer> squareVB = Engine::VertexBuffer::create(squareVertices, sizeof(squareVertices));
	squareVB->setLayout({
		{ Engine::ShaderDataType::Float3, "a_Position" },
		{ Engine::ShaderDataType::Float2, "a_TexCoords" }
		});
	m_squareVA->addVertexBuffer(squareVB);
	
	uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
	std::shared_ptr<Engine::IndexBuffer> squareIB = Engine::IndexBuffer::create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t));
	m_squareVA->setIndexBuffer(squareIB);

	m_squareVA->unbind();

	auto loadedShader = m_shaderLibrary.load("assets/shaders/webcam.glsl");

	Engine::Application::s_get().getWindow().getWindowSize(&frameWidth, &frameHeight);

	/// CHESSBOARD
	Chessboard cb;
	m_chessboardVA = Engine::OpenGLVertexArray::create();
	m_chessboardVA->bind();

	float* verticesPtr = nullptr;
	uint32_t size = 0;
	cb.getVertices(&verticesPtr,size);
	std::shared_ptr<Engine::VertexBuffer> vb = Engine::VertexBuffer::create(verticesPtr, size);
	vb->setLayout({
		{ Engine::ShaderDataType::Float3, "a_Position" }
		});
	m_chessboardVA->addVertexBuffer(vb);
	uint32_t* indicesPtr = nullptr;
	uint32_t count = 0;
	cb.getIndices(&indicesPtr,count);
	std::shared_ptr<Engine::IndexBuffer> ib = Engine::IndexBuffer::create(indicesPtr, count);
	m_chessboardVA->setIndexBuffer(ib);

	//cb->getVertices();
	m_chessboardVA->unbind();
	loadedShader = m_shaderLibrary.load("assets/shaders/FlatColor.glsl");

}

void WebcamLayer::onDetach()
{

}

void WebcamLayer::onUpdate(Engine::Timestep ts)
{
	m_cameraController.onUpdate(ts);

	// Render
	// 
	// background color
	Engine::RenderCommand::setClearColor({ 0.9,0.9,0.9, 1 });
	Engine::RenderCommand::clear();

	Engine::Renderer::beginScene(m_cameraController);

	auto loadedShader = m_shaderLibrary.get("webcam");;
	m_webcam->updateFrame(m_frame);
	m_webcamFeedTexture->setData((void*)m_frame.data, m_frame.total() * m_frame.elemSize());
	m_squareVA->bind();
	m_webcamFeedTexture->bind();
	Engine::Renderer::submit(loadedShader, m_squareVA, glm::mat4(1.0f));

	loadedShader = m_shaderLibrary.get("FlatColor");;
	loadedShader->bind();
	loadedShader->setFloat3("u_Color", m_squareColor);

	glm::mat4 t = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	Engine::Renderer::submit(loadedShader, m_chessboardVA,t);


	Engine::Renderer::endScene();
}

void WebcamLayer::onImGuiRender()
{
	ImGui::Begin("Settings");
	ImGui::ColorEdit3("Square Color", glm::value_ptr(m_squareColor));
	ImGui::End();
}

void WebcamLayer::onEvent(Engine::Event& e)
{
	m_cameraController.onEvent(e);
}