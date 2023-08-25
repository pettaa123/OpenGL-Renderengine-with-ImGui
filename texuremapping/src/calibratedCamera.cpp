// SPDX-License-Identifier: MIT
// Copyright (c) 2023 Jens-Peter Tuppatsch, German Aerospace Center (DLR)

#include "calibratedCamera.h"


namespace TextureMapping {


        void CalibratedCamera::setWorld2cameraPose(const Eigen::Transform<float, 3, Eigen::Affine>& world2cameraPose) {
            m_world2cameraPose = world2cameraPose;
            m_camera2worldPose = world2cameraPose.inverse();
        }

        void CalibratedCamera::setCamera2worldPose(const Eigen::Transform<float, 3, Eigen::Affine>& camera2worldPose) {
            m_camera2worldPose = camera2worldPose;
            m_world2cameraPose = camera2worldPose.inverse();
        }

        CalibratedCamera::CalibratedCamera(const std::string& name) 
            : m_name( name ) {}

        CalibratedCamera::CalibratedCamera(const std::filesystem::path& configPath)
            : m_configPath( configPath ) {}


}