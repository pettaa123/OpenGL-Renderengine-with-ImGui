#pragma once

#include "engine/engine.h"
#include <vector>
#include <opencv2/opencv.hpp>
#include <functional>

struct Intrinsics {
	float fx, fy;
	float cx, cy;
	int width, height;
	float k1, k2, k3, k4, k5, k6;
	float p1, p2;
};

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

	void calcDistortionMap(){
		// Calculate distortion map
		cv::Mat1f camera_matrix(3, 3);
		camera_matrix << m_intrinsics.fx, 0, m_intrinsics.cx, 0, m_intrinsics.fy, m_intrinsics.cy, 0, 0, 1;
		cv::Mat1f new_camera_matrix(3, 3);
		new_camera_matrix << m_intrinsics.fx, 0, m_intrinsics.cx, 0, m_intrinsics.fy,
			m_intrinsics.cy, 0, 0, 1;
		cv::Mat1f distortion_coeff(1, 8);
		distortion_coeff << m_intrinsics.k1, m_intrinsics.k2, m_intrinsics.p1, m_intrinsics.p2, m_intrinsics.k3,
			m_intrinsics.k4, m_intrinsics.k5, m_intrinsics.k6;
		cv::Mat map1, map2, map3;
		cv::initUndistortRectifyMap(
			camera_matrix, distortion_coeff, cv::Mat{}, new_camera_matrix,
			cv::Size{ m_intrinsics.width, m_intrinsics.height }, CV_32FC1, map1, map2);
		cv::convertMaps(map1, map2, m_distortionMap, map3, CV_16SC2, true);
	}

private:
	int m_deviceID;
	int m_apiID;
	cv::VideoCapture m_cap;
	cv::Mat m_lastFrame;
	cv::Mat m_newFrame;

	Intrinsics m_intrinsics;
	cv::Mat m_distortionMap;

};

class Chessboard {

public:
	Chessboard(float size = 8, int cols = 23, int rows = 18) :
		m_size(size),
		m_columns(cols),
		m_rows(rows)

	{
		//reserve memory for all black squares
		m_vertices.reserve(cols * rows * 3 * 4 / 2);
		m_indices.reserve(cols * rows * 6 / 2);

		uint32_t done = 0;

		for (size_t x = 0; x < cols; x++) {
			for (size_t y = 0; y < rows; y+=2)
			{				
				float xLeft = -cols / 2.0f * size + x * size;
				float yTop = -rows / 2.0f * size + y * size + (x % 2) * size;
				//generate all 4 vertices of each black square 
				//top left
				m_vertices.push_back(xLeft);
				m_vertices.push_back(yTop);
				m_vertices.push_back(.0f);
				//top right
				m_vertices.push_back(xLeft + size);
				m_vertices.push_back(yTop);
				m_vertices.push_back(.0f);
				//bottom right
				m_vertices.push_back(xLeft + size);
				m_vertices.push_back(yTop + size);
				m_vertices.push_back(.0f);
				//bottom left
				m_vertices.push_back(xLeft);
				m_vertices.push_back(yTop + size);
				m_vertices.push_back(.0f);

				//generate vertices for each square counterclockwise
				
				uint32_t offset = done * 4;

				m_indices.push_back(0+offset);
				m_indices.push_back(3+offset);
				m_indices.push_back(2+offset);

				m_indices.push_back(0+offset);
				m_indices.push_back(2+offset);
				m_indices.push_back(1+offset);

				done++;
			}
		}
	}

	void getVertices(float** data, uint32_t& size) { *data = m_vertices.data(); size = (uint32_t)m_vertices.size() * (uint32_t)sizeof(float); }
	void getIndices(uint32_t** data, uint32_t& count) { *data = m_indices.data(); count = (uint32_t)m_indices.size(); }

private:
	float m_size;
	int m_columns;
	int m_rows;
	std::vector<float> m_vertices;
	std::vector<uint32_t> m_indices;
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
	Engine::ShaderLibrary					m_shaderLibrary;
	std::shared_ptr<Engine::Shader>			m_webcamShader;
	std::shared_ptr<Engine::Shader>			m_objectShader;
	std::shared_ptr<Engine::Texture2D>		m_webcamFeedTexture;
	Engine::Camera							m_cameraController; //perspective camera
	std::shared_ptr<Engine::VertexArray>	m_chessboardVA;
	std::unique_ptr<Webcam>					m_webcam;
	std::unique_ptr<Engine::Model>          m_3dObject;
	std::shared_ptr<Engine::VertexArray>	m_squareVA;
	cv::Mat									m_frame;
	glm::vec3								m_squareColor = { 0.5f, 0.2f, 0.5f };

};

