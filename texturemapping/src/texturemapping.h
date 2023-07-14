#pragma once

#include "Engine.h"

class Texturemapping : public Engine::Layer
{
public:
	Texturemapping();
	virtual ~Texturemapping() = default;

	virtual void onAttach() override;
	virtual void onDetach() override;

	void onUpdate(Engine::Timestep ts) override;
	virtual void onImGuiRender() override;
	void onEvent(Engine::Event& e) override;
private:
	Engine::OrthographicCameraController m_CameraController;
	
	// Temp
	Engine::Ref<Engine::VertexArray> m_squareVA;
	Engine::Ref<Engine::Shader> m_flatColorShader;

	Engine::Ref<Engine::Texture2D> m_checkerboardTexture;

	glm::vec4 m_SquareColor = { 0.2f, 0.3f, 0.8f, 1.0f };
};