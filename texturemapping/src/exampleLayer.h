#pragma once

#include "engine.h"

class ExampleLayer : public Engine::Layer
{
public:
	ExampleLayer();
	virtual ~ExampleLayer() = default;

	virtual void onAttach() override;
	virtual void onDetach() override;

	void onUpdate(Engine::Timestep ts) override;
	virtual void onImGuiRender() override;
	void onEvent(Engine::Event& e) override;
private:
	Engine::ShaderLibrary m_shaderLibrary;
	std::shared_ptr<Engine::Shader> m_shader;
	std::shared_ptr<Engine::VertexArray> m_vertexArray;

	std::shared_ptr<Engine::Shader> m_flatColorShader;
	std::shared_ptr<Engine::Shader> m_textureShader;
	std::shared_ptr<Engine::VertexArray> m_squareVA;

	std::shared_ptr<Engine::Texture2D> m_texture, m_chernoLogoTexture;

	//Engine::OrthographicCameraController m_cameraController;
	Engine::Camera m_cameraController;
	glm::vec3 m_squareColor = { 0.2f, 0.3f, 0.8f };

	std::unique_ptr<Engine::Mesh> m_mesh;
	std::unique_ptr<Engine::Model> m_backpack;
};

