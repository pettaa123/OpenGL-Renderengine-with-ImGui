#pragma once

#include <vector>
#include <memory>
#include <execution>
#include <format>
#include "texturemapping/modeling/model.h"
#include "texturemapping/core/mappingDataSet.h"
#include "texturemapping/mapping/core/projectionResult.h"
#include "texturemapping/hardwareacceleration/base/graphicsDevice.h"
#include "texturemapping/core/log.h"
#include "texturemapping/core/intrinsics.h"



namespace TextureMapping {

	/// Abstract base accelerator class.
	class Accelerator {

	protected:

		//std::vector<float> getVertices() { return m_vertices; };
		//std::vector<float> getDefaultTexCoords() { return m_defaultTexCoords; };
		//int getNumberOfTriangles() { return m_numberOfTriangles; };

		Accelerator(const Model& model) {
			m_vertices = generateVertexCache(model);
			m_defaultTexCoords = generateDefaultTextureCoordinates(model);
			m_numberOfTriangles = m_vertices.size() / 9;
		}

		virtual ~Accelerator() = default;

		std::vector<float> m_vertices;
		std::vector<float> m_defaultTexCoords;
		size_t m_numberOfTriangles;


	public:

		static std::shared_ptr<Accelerator> create(const Model& model);
		static std::vector<GraphicsDevice> getGraphicsDevices() {

			std::vector<boost::compute::device> devices = boost::compute::system::devices();
			std::vector<GraphicsDevice> graphicsDevices;

			for (int i = 0; i < devices.size(); i++) {
				if (devices[i].type() == boost::compute::device::type::gpu) {
					graphicsDevices.push_back(
						GraphicsDevice{
							devices[i].name(),
							devices[i].driver_version(),
							false,
							devices[i]
						});
				};
			}

			return graphicsDevices;

		}


		/*
		public static List<GraphicsDevice> GetGraphicsDevices(AcceleratorTechnique technique) {
			ManagementObjectSearcher objVideo = new ManagementObjectSearcher("select * from Win32_VideoController");

			// All property names
			// https://msdn.microsoft.com/en-us/library/aa394512(v=vs.85).aspx

			// TODO: Problem: A CUDA device can not really be related to a OpenCL or Win32 device, as CUDA does not provide the "real" driver version,
			// but only the CUDA runtime version. As of now, they will be matched by names.

			List<CUDADevice> cudaDevices = CUDAAccelerator.GetDevices();
			List<ComputeDevice> openCLDevices = OpenCLAccelerator.GetDevices();

			List<GraphicsDevice> devices = new List<GraphicsDevice>();
			foreach (ManagementObject obj in objVideo.Get()) {
				string name = obj["Name"].ToString();
				string driverVersion = obj["DriverVersion"].ToString();
				GraphicsDevice device = new GraphicsDevice(name, driverVersion, technique);

				if (technique == AcceleratorTechnique.CUDA) {
					CUDADevice relatedDevice = cudaDevices.FirstOrDefault(d => name.ToLower().Contains(d.Name.ToLower()));
					if (relatedDevice != null) {
						device.CUDADevice = relatedDevice;
						devices.Add(device);
					}
				}
				else if (technique == AcceleratorTechnique.OpenCL) {
					// TODO: This does not work, the versions are different
					ComputeDevice relatedDevice = openCLDevices.FirstOrDefault(d => d.DriverVersion == driverVersion);
					if (relatedDevice != null) {
						device.OpenCLDevice = relatedDevice;
						devices.Add(device);
					}
				}
			}

			return devices;
		}
		*/

		/// Projects the image.
		virtual ProjectionResult projectImage(const MappingDataSet& correspondence, std::vector<float>& projectionMatrix, int dataSetID) = 0;
		/// Undistorts the image.
		virtual void undistortImage(STBimage& image, const Intrinsics& intrinsicParameters) = 0;


		// Filters the projection result.
	protected:

		ProjectionResult filterResult(std::vector<float>& textureCoordinates) {
			// Removing the unset coordinates drastically removes memory usage
			std::vector<int> texTriangleStartIds;
			std::unordered_set<int> texTriangleStartIdsHash;
			std::vector<glm::vec2> coordinates;

			// TODO: Make this parallel?
			// There should be always 6 values in row that are either set or unset
			for (int i = 0; i < textureCoordinates.size(); i += 6) {
				float texCoord = textureCoordinates[i];
				// TODO: Store this value somewhere for general use
				if (texCoord != -1000) {
					int triangleId = i / 6;
					int texId = triangleId * 3;
					texTriangleStartIds.push_back(triangleId);
					texTriangleStartIdsHash.insert(triangleId);
					for (int b = 0; b < 3; b++) {
						int idx = i + 2 * b;
						glm::vec2 texCoordVec(textureCoordinates[idx], textureCoordinates[idx + 1]);
						coordinates.push_back(texCoordVec);
					}
				}
			}

			ProjectionResult result{
				texTriangleStartIdsHash,
				std::pair<std::vector<glm::vec2>, std::vector<int>>(coordinates, texTriangleStartIds)
			};
			return result;
		}

	private:

		// TODO: This method exists twice (also in the Projector)
		/// Generates default texture coordinates.
		std::vector<float> generateDefaultTextureCoordinates(const TextureMapping::Model& model) const {
			std::vector<float> textureCoordinates(model.getVerticesCount()*2,-1000);
			return textureCoordinates;
		}

		std::vector<float> generateVertexCache(const TextureMapping::Model& model) const {
			std::vector<float> verticesAsFloats(model.getVerticesCount() * 3);
			uint32_t idx = 0;
			for (auto& v : model.getVertices()) {
				verticesAsFloats[idx++] = v.x;
				verticesAsFloats[idx++] = v.y;
				verticesAsFloats[idx] = v.z;
			}

			/*
			Parallel.ForEach(model.Vertices, (vertex, loopState, index) = > {
				long idx = index * 3;
				verticesAsFloats[idx++] = vertex.X;
				verticesAsFloats[idx++] = vertex.Y;
				verticesAsFloats[idx] = vertex.Z;
			});
			*/
			return verticesAsFloats;
		}
	};
}
