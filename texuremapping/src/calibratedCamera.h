// SPDX-License-Identifier: MIT
// Copyright (c) 2023 Jens-Peter Tuppatsch, German Aerospace Center (DLR)

#pragma once

#include <filesystem>

#include <Eigen/Geometry>

namespace TextureMapping {

	struct Intrinsics {
		float fx, fy;
		float cx, cy;
		int width, height;
		float k1, k2, k3, k4, k5, k6;
		float p1, p2;
	};

	class CalibratedCamera {
		// Constructor
		CalibratedCamera(const std::string& name = "MappingCamera");
		CalibratedCamera(const std::filesystem::path& configPath);

		// Setup method
		bool init();

		// Setters
		void setName(const std::string& name) { m_name = name; }
		void setConfigPath(const std::filesystem::path& configPath) { m_configPath = configPath; m_setUp = false; }
		void setCamera2worldPose(const Eigen::Transform<float, 3, Eigen::Affine>& camera2worldPose);
		void setWorld2cameraPose(const Eigen::Transform<float, 3, Eigen::Affine>& world2cameraPose);
		//void startSavingImages(const std::filesystem::path& save_directory,const std::string& save_image_type = "png");
		//void stopSavingImages();

		// Getters
		const std::string& getName() const { return m_name; };
		const std::filesystem::path& getConfigPath() const { return m_name; };
		const Intrinsics& intrinsics() const { return m_intrinsics; };
		const Eigen::Transform<float, 3, Eigen::Affine>& getCamera2worldPose() const { return m_camera2worldPose; };
		const Eigen::Transform<float, 3, Eigen::Affine>& getWorld2cameraPose() const { return m_world2cameraPose; };

		bool isSetUp() const { return m_setUp; };

	private:

		// Helper methods
		//void SaveImageIfDesired();

		// Data
		std::string m_name;
		std::filesystem::path m_configPath;
		Intrinsics m_intrinsics;
		Eigen::Transform<float, 3, Eigen::Affine> m_camera2worldPose;
		Eigen::Transform<float, 3, Eigen::Affine> m_world2cameraPose;
		//int save_index_ = 0;

		// Internal state
		bool m_setUp = false;
	};


}
