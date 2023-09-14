#pragma once

#include "engine/engine.h"
#include "texturemapping/texturemapping.h"
#include <vector>
#include <functional>


class TargetLayer : public Engine::Layer
{
public:
	TargetLayer();
	virtual ~TargetLayer() = default;

	virtual void onAttach() override;
	virtual void onDetach() override;

	void onUpdate(Engine::Timestep ts) override;
	virtual void onImGuiRender() override;
	void onEvent(Engine::Event& e) override;
private:
	Engine::ShaderLibrary					m_shaderLibrary;
	std::shared_ptr<Engine::Shader>			m_webcamShader;
	std::shared_ptr<Engine::Shader>			m_objectShader;
	std::shared_ptr<Engine::Texture2D>		m_webcamFeedTexture;
	Engine::Camera							m_cameraController; //perspective camera
	glm::vec3								m_squareColor = { 0.5f, 0.2f, 0.5f };

	//MARKER
	std::shared_ptr<TextureMapping::Model> m_markerModel;

	std::shared_ptr<Engine::Texture2D> m_texture;

};

