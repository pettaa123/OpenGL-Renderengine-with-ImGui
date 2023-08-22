#pragma once

#include "engine.h"

#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/imgproc/imgproc.hpp>

class Webcam {

public:
	Webcam(int deviceID=0, int apiID=0) :
		m_deviceID(deviceID),
		m_apiID(apiID){
		m_lastFrame = cv::Mat(480,640,CV_8UC3);
		m_newFrame = cv::Mat(480, 640, CV_8UC3);
		m_cap.open(m_deviceID, m_apiID);
		if (!m_cap.isOpened()) {
			Log::error("ERROR! Unable to open camera");
		}
	}
	void updateFrame(cv::Mat& frame) {
		if(!m_cap.read(m_newFrame)){
			Log::error("ERROR! blank frame grabbed");
		}
		else {
			cv::cvtColor(m_newFrame, m_lastFrame, cv::COLOR_BGR2RGB);
			cv::flip(m_newFrame, m_newFrame,0);
			m_lastFrame = m_newFrame;
		}
		frame = m_lastFrame;
	}

private:
	int m_deviceID;
	int m_apiID;
	cv::VideoCapture m_cap;
	cv::Mat m_lastFrame;
	cv::Mat m_newFrame;

};


class WebcamLayer : public Engine::Layer
{
public:
	WebcamLayer();
	virtual ~WebcamLayer() = default;

	virtual void onAttach() override;
	virtual void onDetach() override;

	void onUpdate(Engine::Timestep ts) override;
	virtual void onImGuiRender() override;
	void onEvent(Engine::Event& e) override;
private:
	Engine::ShaderLibrary m_shaderLibrary;

	std::shared_ptr<Engine::Shader> m_webcamShader;
	std::shared_ptr<Engine::Shader> m_objectShader;

	std::shared_ptr<Engine::Texture2D> m_webcamFeedTexture;

	Engine::Camera m_cameraController; //perspective camera
	glm::vec3 im_color = { 0.2f, 0.3f, 0.8f };

	std::unique_ptr<Engine::Model> m_3dObject;

	std::unique_ptr<Webcam> m_webcam;

	std::shared_ptr<Engine::VertexArray> m_squareVA;

	cv::Mat m_frame;

};

