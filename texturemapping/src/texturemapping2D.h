#pragma once

#include "engine.h"

class Texturemapping2D : public Engine::Layer
{
public:
	Texturemapping2D();
	virtual ~Texturemapping2D() = default;

	virtual void onAttach() override;
	virtual void onDetach() override;

	void onUpdate(Engine::Timestep ts) override;
	virtual void onImGuiRender() override;
	void onEvent(Engine::Event& e) override;
private:
	Engine::OrthographicCameraController m_cameraController;
	//
	//// Temp
	std::shared_ptr<Engine::VertexArray> m_squareVA;
	std::shared_ptr<Engine::Shader> m_flatColorShader;
	//
	std::shared_ptr<Engine::Texture2D> m_checkerboardTexture;

	glm::vec4 m_squareColor = { 0.2f, 0.3f, 0.8f, 1.0f };
};