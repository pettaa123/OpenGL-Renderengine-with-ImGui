#pragma once

#include "engine/engine.h"
#include "texturemapping/texturemapping.h"
#include <vector>
#include <functional>
#include <glm/vec3.hpp>

class TargetLayer : public Engine::Layer
{
public:
	TargetLayer(int width, int height);
	virtual ~TargetLayer() = default;

	virtual void onAttach() override;
	virtual void onDetach() override;

	void onUpdate(Engine::Timestep ts) override;
	virtual void onImGuiRender() override;
	void onEvent(Engine::Event& e) override;
private:
	//MODELS
	std::shared_ptr<Engine::Model>			m_camModel;
	std::shared_ptr<TextureMapping::Model>	m_targetModel;

	//LIGHT?
	std::shared_ptr<Engine::VertexArray> m_lightVA;
	glm::vec3								m_lightColor;

	Engine::ShaderLibrary					m_shaderLibrary;
	std::shared_ptr<Engine::Shader>			m_webcamShader;
	std::shared_ptr<Engine::Shader>			m_objectShader;
	std::shared_ptr<Engine::Texture2D>		m_webcamFeedTexture;
	Engine::Camera							m_cameraController; //perspective camera
	glm::vec3								m_targetColor = { 0.5f, 0.2f, 0.5f };
	glm::vec3								m_camPosition = { 0.0f, 0.0f, 0.0f };
	glm::vec3								m_camColor = { 0.3f, 0.3f, 0.3f };

	//MARKER


	std::shared_ptr<Engine::Texture2D> m_texture;

	std::vector<glm::vec3> m_measurements;


};

