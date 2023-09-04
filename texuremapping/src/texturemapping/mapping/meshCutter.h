#pragma once

#include "texturemapping/mapping/core/model.h"
#include "texturemapping/core/mappingDataSet.h"
#include "texturemapping/mapping/core/triangleReplacement.h"
#include "texturemapping/mapping/core/intersectionResult.h"
#include <optional>
#include "texturemapping/mapping/core/ray2.h"
#include "texturemapping/mapping/core/vectorResult.h"


namespace TextureMapping {

	/// <summary>
	/// Solves overlaying conflicts generated by projecting images.
	/// </summary>
	/// <seealso cref="BaseLib.TimeTracker" />
	/// <seealso cref="BaseLib.ICancelListener" />
	class MeshCutter {
	private:

		Model m_model;
		const std::vector<glm::vec3>& m_modelVertices;
		const std::vector<glm::vec3>& m_modelNormals;
		glm::vec2 m_defaultTexCoord;
		bool m_isCancelled;

	public:
		/// <summary>
		/// Initializes a new instance of the <see cref="MeshCutter"/> class.
		/// </summary>
		/// <param name="model">The model.</param>
		/// <param name="cancelObserver">The cancel observer.</param>
		MeshCutter(Model& m_model);

		// TODO: Either use "id" or "idx"
		/// <summary>
		/// Solves the overlaying texture conflicts.
		/// </summary>
		/// <param name="dataSets">The data sets.</param>
		/// <param name="observer">The observer.</param>
		/// <returns></returns>
		std::optional< std::pair<std::vector<glm::vec2>, std::vector<int>> > solveOverlayingTexturesConflicts(std::vector<MappingDataSet>& dataSets);

		/// <summary>
		/// Cancels the current process.
		/// </summary>
		inline void cancel() { m_isCancelled = true; }

	private:
		/// <summary>
		/// Handles the triangle replacements.
		/// </summary>
		/// <param name="triangleReplacements">The triangle replacements.</param>
		/// <param name="dataSets">The data sets.</param>
		void handleTriangleReplacements(std::vector<TriangleReplacement> triangleReplacements, std::vector<MappingDataSet>& dataSets);

		/// <summary>
		/// Handles the vertex and texture coordinate replacement.
		/// </summary>
		/// <param name="replacement">The replacement.</param>
		void handleVertexAndTextureCoordinateReplacement(TriangleReplacement replacement);

		/// <summary>
		/// Adds normals for the new triangles.
		/// </summary>
		/// <param name="triangleStartVertexId">The triangle start vertex identifier.</param>
		/// <param name="numberOfInsertions">The number of insertions.</param>
		void addNormals(int triangleStartVertexId, int numberOfInsertions);

		/// <summary>
		/// Calculates the triangle intersections.
		/// </summary>
		/// <param name="imagePolygon">The image polygon.</param>
		/// <param name="vertices">The vertices.</param>
		/// <returns></returns>
		std::vector<IntersectionResult> calculateTriangleIntersections(std::vector<glm::vec2> imagePolygon, std::vector<VertexInformation> vertices);

		/// <summary>
		/// Gets a ray from a polygon side.
		/// </summary>
		/// <param name="index">The index.</param>
		/// <param name="imagePolygon">The image polygon.</param>
		/// <returns></returns>
		Ray2 getPolygonRay(int index, const std::vector<glm::vec2>& imagePolygon);

	private:

		/// <summary>
		/// Determines the valid and invalid vectors.
		/// </summary>
		/// <param name="triangleStartVertexIdx">Index of the triangle start vertex.</param>
		/// <param name="textureCoordinates">The texture coordinates.</param>
		/// <param name="imagePolygon">The image polygon.</param>
		/// <returns></returns>
		VectorResult determineValidAndInvalidVectors(int triangleStartVertexIdx, std::vector<glm::vec2>& textureCoordinates, std::vector<glm::vec2>& imagePolygon);

		/// <summary>
		/// Finds valid or invalid vertices.
		/// </summary>
		/// <param name="triangleStartVertexIdx">Index of the triangle start vertex.</param>
		/// <param name="indices">The indices.</param>
		/// <returns></returns>
		std::vector<glm::vec3> findVertices(int triangleStartVertexIdx, std::vector<int>& indices);

		/// <summary>
		/// Finds the indices.
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <param name="array">The array.</param>
		/// <param name="valuesToFind">The values to find.</param>
		/// <returns></returns>
		//std::vector<int> findIndices<T>(T[] array, T[] valuesToFind);

		/// <summary>
		/// Generates default texture coordinates.
		/// </summary>
		/// <param name="size">The size.</param>
		/// <returns></returns>
		std::vector<glm::vec2> generateDefaultTextureCoordinates(int size);

		/// <summary>
		/// Gets the side index from vertex indices.
		/// </summary>
		/// <param name="idx1">The idx1.</param>
		/// <param name="idx2">The idx2.</param>
		/// <returns></returns>
		int getSideIndexFromVertexIndices(int idx1, int idx2);

		/// <summary>
		/// Gets the vertex informations from intersection.
		/// </summary>
		/// <param name="sideIdx">Index of the side.</param>
		/// <param name="vertexInfos">The vertex infos.</param>
		/// <returns></returns>
		std::vector<VertexInformation> getVertexInformationsFromIntersection(int sideIdx, std::vector<VertexInformation> vertexInfos);

		/// <summary>
		/// Gets the vertex information from intersection.
		/// </summary>
		/// <param name="sideIdx">Index of the side.</param>
		/// <param name="vertexInfos">The vertex infos.</param>
		/// <param name="pool">The pool.</param>
		/// <returns></returns>
		VertexInformation getVertexInformationFromIntersection(int sideIdx, std::vector<VertexInformation> vertexInfos, std::vector<glm::vec3>& pool);
	};
}
