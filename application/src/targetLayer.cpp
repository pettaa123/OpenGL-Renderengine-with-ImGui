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

TargetLayer::TargetLayer()  //m_cameraController(1280.0f / 720.0f)
	: Layer("TargetLayer"), m_cameraController(glm::perspectiveFov(glm::radians(45.0f), 1280.0f, 720.0f, 0.1f, 10000.0f))
{

}

void TargetLayer::onAttach()
{

	BaseLib::STBimage stbImage;
	std::filesystem::path markerImagePath("assets/marker/IMG_2821.png");
	//std::filesystem::path markerImagePath("assets/marker/chess.jpg");
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

	std::filesystem::path intrinsicsIPhoneFile("assets/marker/intrinsics/iphone_intrinsics.json");
	TextureMapping::Intrinsics intrinsicsIPhone = TextureMapping::IntrinsicsImporter::loadFromJSON(intrinsicsIPhoneFile);

	MarkerLib::PoseEstimator estimator;
	float objPtsBuf[4][3]{
		{25,20,0},
		{130,42.5,0},
		{25,82.5,0},
		{100,80,0}
 };

	glm::mat3 rmat;
	glm::vec3 tvec;
	float imgArray[4][2]
	{
		{393.524719 ,564.781433 },
		{3684.94727 ,1260.65320 },
		{502.313965 ,2479.42603 },
		{2741.11938 ,2385.86646 }
	};

	std::vector<float> reprojectionError(1);

	estimator.estimate(&objPtsBuf[0][0], sizeof(float) * 12, &imgArray[0][0], 8* sizeof(float), glm::value_ptr(intrinsicsIPhone.toMat3()), intrinsicsIPhone.distortionCoeffs().data(),
		glm::value_ptr(rmat), glm::value_ptr(tvec), false, (int)MarkerLib::SolvePnPMethod::SOLVEPNP_ITERATIVE, reprojectionError.data(), reprojectionError.size() * sizeof(float));

	glm::mat3x4 projectionMatrix = TextureMapping::MathExtension::createProjectionMatrix(rmat, tvec);

	glm::mat3x4 realProjetionMatrix = TextureMapping::MathExtension::mult(intrinsicsIPhone.toMat3(), projectionMatrix);




	std::cout << glm::to_string(realProjetionMatrix) << std::endl;



	for (auto& e : reprojectionError)
		std::cout << e << std::endl;

	std::vector<TextureMapping::MappingDataSet> dataSets = TextureMapping::MappingDataSetImporter::loadFromJSON(std::filesystem::path("assets/marker"));

	m_markerModel = std::make_shared<TextureMapping::Model>("assets/marker/marker_test.stl");



	for (TextureMapping::MappingDataSet& d : dataSets) {
		d.projectionImage = d.alteredImage.has_value() ? d.alteredImage.value() : d.loadedImage;
		d.performCropping();
		d.model = m_markerModel;
	}

	auto devs = TextureMapping::OpenCLAccelerator::getDevices();
	auto executor = TextureMapping::OpenCLAccelerator(devs[0], *m_markerModel.get());


	TextureMapping::ImageToModelProjector projector(*m_markerModel, executor);
	for (size_t i = 0; i < dataSets.size(); i++)
	{
		projector.projectImage(dataSets[i], i, realProjetionMatrix);
	}
	std::optional<TextureMapping::MergingResult> mergingResult = projector.finish();



	m_markerModel->recreateMesh();


	
	auto loadedShader = m_shaderLibrary.load("assets/shaders/model_loading.glsl");

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

	auto loadedShader = m_shaderLibrary.get("model_loading");;
	loadedShader->bind();
	loadedShader->setFloat3("u_Color", m_squareColor);

	Engine::Renderer::submit(m_shaderLibrary.get("model_loading"), m_markerModel.get());


	////////////////////MAPPING ENDE


	Engine::Renderer::endScene();



}

void TargetLayer::onImGuiRender()
{
	ImGui::Begin("Settings");
	ImGui::ColorEdit3("Square Color", glm::value_ptr(m_squareColor));
	ImGui::End();
}

void TargetLayer::onEvent(Engine::Event& e)
{
	m_cameraController.onEvent(e);
}
