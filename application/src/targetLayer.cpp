#include "targetLayer.h"
#include "imgui/imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <opencv2/videoio.hpp>

#include <opencv2/highgui/highgui.hpp>
#include "markerLib/markerFinder.h"
#include "baseLib/stbImage.h"
#include "markerLib/poseEstimator.h"
#include "texturemapping/helper/mathExtensions.h"
#include <glm/gtx/string_cast.hpp>
#include "texturemapping/helper/mathExtensions.h"

#include <opencv2/opencv.hpp>

#include "imgpoints.h"


TargetLayer::TargetLayer()  //m_cameraController(1280.0f / 720.0f)
	: Layer("TargetLayer"), m_cameraController(glm::perspectiveFov(glm::radians(45.0f), 1280.0f, 720.0f, 0.1f, 10000.0f))
{
	/*
	//calibration

	std::vector < std::vector<cv::Point2f>> allImgPoints;

	BaseLib::STBimage stbImage;

	std::vector<cv::Point2f> boundaries{ {61000, 62300}, { 61200, 63000 }, { 61500, 63200 }, { 61500, 63200 } };

	//for (int i = 0; i < 4; i++) {
	//
	//	std::filesystem::path markerImagePath(std::format("assets/calibration/images/{}.png", i + 1));
	//	//std::filesystem::path markerImagePath("assets/marker/chess.jpg");
	//	if (!stbImage.load_16(markerImagePath))
	//		Log::error("error while loading markerImage");
	//
	//	//22 cols , 17 rows
	//	//17,22
	//	std::array<std::array<float, 2>, 22*17> pointBuf;
	//
	//	MarkerLib::MarkerFinder finder;
	//	uint32_t pointBufLen = (uint32_t)(sizeof(float) * pointBuf.size() * pointBuf[0].size());
	//
	//	BaseLib::STBimage clamped = finder.clampImage(stbImage, boundaries[i].x, boundaries[i].y);
	//	finder.selectAndRefineCorners(clamped, &pointBuf[0][0], pointBufLen);
	//	for (size_t i = 0; i < pointBuf.size(); i++)
	//	{
	//		Log::info(std::format("x: {} , y: {}", pointBuf[i][0], pointBuf[i][1]));
	//	}
	//	std::vector<cv::Point2f> allPtsOneImage;
	//	for (auto ar : pointBuf) {
	//		allPtsOneImage.push_back(cv::Point2f(ar[0], ar[1]));
	//	}
	//	allImgPoints.push_back(allPtsOneImage);
	//}

	std::vector<cv::Vec3f> objPoints(17 * 22);
	for (int i = 0; i < 17; i++) {
		for (size_t j = 0; j < 22; j++)
		{
			objPoints[i * 22 + j] = cv::Vec3f(j * 8, i * 8, 0);
		}
	}

	std::vector<std::vector<cv::Vec3f>> ALLOBJPOINTS;
	for (int i = 0; i < 4; i++)
		ALLOBJPOINTS.push_back(objPoints);

	cv::Mat cameraMatrix;
	std::vector<float> distortionCoeffs(5);

	cv::Mat rVec_;
	cv::Mat tVec_;

	double a = cv::calibrateCamera(ALLOBJPOINTS, data::ALLIMGPOINTS, cv::Size(640, 480), cameraMatrix, distortionCoeffs, rVec_, tVec_);

	std::cout << cameraMatrix;
	*/
}



void TargetLayer::onAttach()
{
	m_camModel = std::make_shared<Engine::Model>("assets/models/cam.stl");

	auto loadedShader = m_shaderLibrary.load("assets/shaders/FlatColor.glsl");
	loadedShader = m_shaderLibrary.load("assets/shaders/model_loading.glsl");

	BaseLib::STBimage stbImage;
	//std::filesystem::path markerImagePath("assets/marker/IMG_2821.png");
	std::filesystem::path markerImagePath("assets/marker/PCB - 1250 0.04hz 10% 5cyc controlled_gray.png");
	if (!stbImage.load(markerImagePath))
		Log::error("error while loading markerImage");

	//2 cols , 4 rows
	std::array<std::array<float, 2>, 4> pointBuf;

	MarkerLib::MarkerFinder finder;
	uint32_t pointBufLen = (uint32_t)(sizeof(float) * pointBuf.size() * pointBuf[0].size());
	//finder.selectAndRefineCorners(stbImage, &pointBuf[0][0],pointBufLen);
	//for (size_t i = 0; i < pointBuf.size(); i++)
	//{
	//	Log::info(std::format("x: {} , y: {}", pointBuf[i][0], pointBuf[i][1]));
	//}

	std::filesystem::path intrinsicsA700File("assets/marker/intrinsics/a700_55k_intrinsics.json");
	TextureMapping::Intrinsics intrinsicsA700 = TextureMapping::IntrinsicsImporter::loadFromJSON(intrinsicsA700File);

	MarkerLib::PoseEstimator estimator;
	float objPtsBuf[4][3]{
		{25.0	,-20.0	,0},
		{25	,-82.5	,0},
		{130	,-42.5	,0},
		{100	,-80	,0}
	};

	glm::mat3 rmat;
	glm::vec3 tvec;
	float imgArray[4][2]
	{
		{405.26923 , 66.26354  },
		{ 586.9587 , 69.387184 },
		{ 465.76837 , 372.44147},
		{ 576.46313 , 287.07285}

	};

	std::vector<float> reprojectionError(1);

	estimator.estimate(&objPtsBuf[0][0], sizeof(float) * 12, &imgArray[0][0], 8 * sizeof(float), glm::value_ptr(intrinsicsA700.toMat3()), intrinsicsA700.distortionCoeffs().data(),
		glm::value_ptr(rmat), glm::value_ptr(tvec), false, (int)MarkerLib::SolvePnPMethod::SOLVEPNP_ITERATIVE, reprojectionError.data(), reprojectionError.size() * sizeof(float));

	glm::mat3x4 projectionMatrix = TextureMapping::MathExtension::createProjectionMatrix(rmat, tvec);
	glm::mat3x4 realProjetionMatrix = TextureMapping::MathExtension::mult(intrinsicsA700.toMat3(), projectionMatrix);

	rmat = glm::transpose(rmat);  // rotation of inverse
	tvec = tvec * -rmat; // translation of inverse


	m_camModel->position = tvec;
	m_camModel->rotationQuaternion = glm::quat_cast(rmat);
	m_camModel->updateModelMatrix();






	std::cout << glm::to_string(realProjetionMatrix) << std::endl;



	for (auto& e : reprojectionError)
		std::cout << e << std::endl;

	std::vector<TextureMapping::MappingDataSet> dataSets = TextureMapping::MappingDataSetImporter::loadFromJSON(std::filesystem::path("assets/marker"));

	m_targetModel = std::make_shared<TextureMapping::Model>("assets/marker/20_09_2023_2pcb4.STL");



	for (TextureMapping::MappingDataSet& d : dataSets) {
		d.projectionImage = d.alteredImage.has_value() ? d.alteredImage.value() : d.loadedImage;
		d.performCropping();
		d.model = m_targetModel;
	}

	auto devs = TextureMapping::OpenCLAccelerator::getDevices();
	auto executor = TextureMapping::OpenCLAccelerator(devs[0], *m_targetModel.get());


	TextureMapping::ImageToModelProjector projector(*m_targetModel, executor);
	for (size_t i = 0; i < dataSets.size(); i++)
	{
		projector.projectImage(dataSets[i], i, realProjetionMatrix);
	}
	std::optional<TextureMapping::MergingResult> mergingResult = projector.finish();



	m_targetModel->recreateMesh();

	//LIGHT
	/*
	m_lightVA = Engine::OpenGLVertexArray::create();
	m_lightVA->bind();
	std::shared_ptr<Engine::VertexBuffer> lightVB(Engine::VertexBuffer::create(3,sizeof(float)));
	lightVB->setLayout({
		{ Engine::ShaderDataType::Float3, "a_LightColor" }
		});
	m_lightVA->addVertexBuffer(lightVB);


	m_lightColor = glm::vec3(0.7f, 0.7f, 0.7f);
	*/

}

void TargetLayer::onDetach()
{

}

void TargetLayer::onUpdate(Engine::Timestep ts)
{
	// Render
	// 
	m_cameraController.onUpdate(ts);

	// background color
	Engine::RenderCommand::setClearColor({ 0.9,0.9,0.9, 1 });
	Engine::RenderCommand::clear();

	Engine::Renderer::beginScene(m_cameraController);


	//////////////MAPPING

	auto loadedShader = m_shaderLibrary.get("FlatColor");
	loadedShader->bind();
	loadedShader->setFloat3("u_LightColor", m_lightColor);
	loadedShader->setFloat3("u_Color", m_camColor);
	loadedShader->setMat4("u_Model", m_camModel.get()->modelMatrix);
	Engine::Renderer::submit(m_shaderLibrary.get("FlatColor"), m_camModel.get());


	loadedShader = m_shaderLibrary.get("model_loading");
	loadedShader->bind();
	loadedShader->setFloat3("u_Color", m_targetColor);
	loadedShader->setMat4("u_Model", m_targetModel.get()->modelMatrix);
	Engine::Renderer::submit(m_shaderLibrary.get("model_loading"), m_targetModel.get());




	////////////////////MAPPING ENDE


	Engine::Renderer::endScene();



}

void TargetLayer::onImGuiRender()
{
	ImGui::Begin("Settings");
	ImGui::ColorEdit3("Light Color", glm::value_ptr(m_lightColor));
	ImGui::ColorEdit3("Target Color", glm::value_ptr(m_targetColor));
	ImGui::ColorEdit3("Cam Color", glm::value_ptr(m_camColor));
	if (ImGui::InputFloat3("Cam Position", glm::value_ptr(m_camModel->position))) {
		m_camModel->updateModelMatrix();
	}
	ImGui::End();
}

void TargetLayer::onEvent(Engine::Event& e)
{
	m_cameraController.onEvent(e);
}
