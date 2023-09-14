#include "texturemapping/mapping/meshCutter.h"
#include <algorithm>
#include <glm/gtx/norm.hpp>
#include <ranges>
#include <map>
#include <set>
#include "texturemapping/mapping/core/vertexInformation.h"
#include "texturemapping/helper/mathExtensions.h"
#include "texturemapping/helper/polygonHelper.h"
#include "texturemapping/mapping/core/barycentricCoordinate.h"
#include "texturemapping/mapping/core/ray3.h"
#include <iostream>
#include <chrono>
#include <algorithm>

//#define PARALLEL

namespace TextureMapping {

	MeshCutter::MeshCutter(Model& model)
		:m_model(model),
		m_defaultTexCoord(glm::vec2(-1000, -1000)),
		m_isCancelled(false)
	{
		m_modelVertices = std::make_shared<std::vector<std::optional<Engine::Mesh::Vertex>>>(model.vertices().size());
		std::transform(model.vertices().begin(), model.vertices().end(), m_modelVertices->begin(), [](Engine::Mesh::Vertex& v) {return std::make_optional<Engine::Mesh::Vertex>(v); });
	}

	// TODO: Either use "id" or "idx"
	// THIS DEFINETELY NEEDS SOME PROPER REFACTORING
	std::optional<std::pair<std::vector<glm::vec2>, std::vector<int>>> MeshCutter::solveOverlayingTexturesConflicts(std::vector<MappingDataSet>& dataSets) {
		//shallow copy
		std::vector<MappingDataSet*> clonedDataSets;
		clonedDataSets.reserve(dataSets.size());
		std::transform(dataSets.begin(), dataSets.end(), std::back_inserter(clonedDataSets), [](MappingDataSet& obj) -> MappingDataSet* { return &obj; });


		while (clonedDataSets.size() > 0) {
			if (m_isCancelled) {
				return {};
			}

			std::vector<TriangleReplacement> triangleReplacements;
			MappingDataSet* dominatingSet = clonedDataSets[0];

			std::set<int> uniqueTriangleIds;

			for (auto mds : clonedDataSets) {
				uniqueTriangleIds.insert(mds->texCoordsAndTriangleIds.second.begin(), mds->texCoordsAndTriangleIds.second.end());
			}


			//std::vector<int> uniqueTriangleIds = clonedDataSets.SelectMany(d = > d.TexCoordsAndTriangleIds.Item2).Distinct().ToList();
			//uniqueTriangleIds.Remove(-1);
			auto t1 = std::chrono::high_resolution_clock::now();
			for (const int triangleId : uniqueTriangleIds) {
				//auto t2 = std::chrono::high_resolution_clock::now();
				//auto ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
				//t1 = t2;
				//std::cout << ms_int.count() << "ms loop time\n";

				int triangleStartVertexId = triangleId * 3;

				std::vector<VertexInformation> vertexInfos;
				vertexInfos.push_back(VertexInformation(0));
				vertexInfos.push_back(VertexInformation(1));
				vertexInfos.push_back(VertexInformation(2));


				// For large lists, reinitializing seems to be more efficient than clearing them
				// "Clear" is a O(n) operation
				std::vector<MappingDataSet*> affectedSets;
				std::vector<int> triangleArrayIndices;
				for (MappingDataSet* dataSet : clonedDataSets) {

					// Searching a value in a HashSet is a O(1) operation
					// Doing this before gathering the actual index saves a lot of time
					if (dataSet->triangleHash.contains(triangleId)) {

						affectedSets.push_back(dataSet);

						// One could also use the BinarySearch method here, which is faster than IndexOf.
						// Fo this, the List has to be sorted beforehand. This has been tested, but did not work for some reason.
						// Even after sorting, wrong results came back
						int triangleArrayIdx = 0;

						auto itr = std::find(dataSet->texCoordsAndTriangleIds.second.begin(), dataSet->texCoordsAndTriangleIds.second.end(), triangleId);
						if (itr != dataSet->texCoordsAndTriangleIds.second.end())
							triangleArrayIdx = static_cast<int>(itr - dataSet->texCoordsAndTriangleIds.second.begin());

						int texId = triangleArrayIdx * 3;

						triangleArrayIndices.push_back(triangleArrayIdx);
						std::vector<glm::vec2> texVectors = {
							dataSet->texCoordsAndTriangleIds.first[texId],
							dataSet->texCoordsAndTriangleIds.first[texId + 1],
							dataSet->texCoordsAndTriangleIds.first[texId + 2]
						};

						for (size_t b = 0; b < texVectors.size(); b++) {
							vertexInfos[b].underlayingTexCoords.push_back(texVectors[b]);
						}
					}
				}
				//std::find_if(myvector.begin(), myvector.end(), [](MappingDataSet* e) { return e->getValue() == 0; })
				auto itr = std::find(affectedSets.begin(), affectedSets.end(), dominatingSet);
				int domIdx = -1;
				if (itr != affectedSets.end())
					domIdx = static_cast<int>(itr - affectedSets.begin());
				if (domIdx == -1) {
					continue;
				}

				for (int a = 0; a < 3; a++) {
					VertexInformation& vertexInfo = vertexInfos[a];
					vertexInfo.texCoord = vertexInfo.underlayingTexCoords[domIdx];
					vertexInfo.underlayingTexCoords.erase(vertexInfo.underlayingTexCoords.begin() + domIdx);
					vertexInfo.vertex = (*m_modelVertices)[triangleStartVertexId + a]->position;
				}

				std::vector<glm::vec2> dominatingTexCoords = {
					vertexInfos[0].texCoord,
					vertexInfos[1].texCoord,
					vertexInfos[2].texCoord
				};

				int numberOfUnderlaying = static_cast<int>(vertexInfos[0].underlayingTexCoords.size());
				bool isOverlaying = vertexInfos[0].underlayingTexCoords.size() > 0;
				std::vector<glm::vec2> imagePolygon = dominatingSet->imagePolygonPointsInUV;
				int validCount = static_cast<int>(std::count_if(dominatingTexCoords.begin(), dominatingTexCoords.end(), [&](glm::vec2& v) { return PolygonHelper::isPointInsidePolygon(imagePolygon, v); }));
				//int validCount = dominatingTexCoords.Count(t = > PolygonHelper.IsPointInsidePolygon(imagePolygon, t));

				// CASE: Triangle is completely inside the image polygon
				if (validCount == 3) {
					for (size_t a = 0; a < affectedSets.size(); a++) {
						MappingDataSet* dataSet = affectedSets[a];
						if (dataSet == dominatingSet) {
							continue;
						}
						dataSet->texCoordsAndTriangleIds.second[triangleArrayIndices[a]] = -1;
					}

					auto t2 = std::chrono::high_resolution_clock::now();
					/* Getting number of milliseconds as an integer. */
				}
				// CASE: Triangle intersects the image polygon
				else {
					std::vector<glm::vec3> newVertices;
					std::vector<std::vector<glm::vec2>> newTextureCoordinates;
					newTextureCoordinates.reserve(numberOfUnderlaying + 1);

					// This might not be a good criterion at the first look, but the image corner only plays a role
					// if it is inside the triangle - in this case the correct corner will be detected in any case

					auto lambda = [&](auto& a, auto& b) { return glm::distance2(vertexInfos[0].texCoord, a) < glm::distance2(vertexInfos[0].texCoord, b); };
					glm::vec2 imageCornerTex = *std::min_element(imagePolygon.begin(), imagePolygon.end(), lambda);
					//C# Vector2 imageCornerTex = imagePolygon.MinBy(p => vertexInfos[0].TexCoord.DistanceSquaredTo(p));
					auto itr = std::find(imagePolygon.begin(), imagePolygon.end(), imageCornerTex);
					int imageCornerIdx = static_cast<int>(itr - imagePolygon.begin());
					//int imageCornerIdx = Array.FindIndex(imagePolygon, p => p == imageCornerTex);

					VectorResult vectors = determineValidAndInvalidVectors(triangleStartVertexId, dominatingTexCoords, imagePolygon);
					std::vector<IntersectionResult> intersections = calculateTriangleIntersections(imagePolygon, vertexInfos);

					// This case can not occur through projection, but it can occur when a triangle has been split up and the current dominating set
					// was an underlaying set
					if (intersections.size() < 2) {
						dominatingSet->texCoordsAndTriangleIds.second[triangleArrayIndices[0]] = -1;

						continue;

					}

					glm::vec3 intersectionVertex1 = intersections[0].vertexInfo.vertex;
					glm::vec3 intersectionVertex2 = intersections[1].vertexInfo.vertex;

					glm::vec2 intersectionTex1 = intersections[0].vertexInfo.texCoord;
					glm::vec2 intersectionTex2 = intersections[1].vertexInfo.texCoord;
					std::vector<glm::vec2> underlayingIntersectionTex1 = intersections[0].vertexInfo.underlayingTexCoords;
					std::vector<glm::vec2> underlayingIntersectionTex2 = intersections[1].vertexInfo.underlayingTexCoords;

					// Check if the image corner is inside the triangle
					bool isImageCornerInsideTriangle = MathExtension::isPointInsideTriangle(imageCornerTex, vertexInfos[0].texCoord, vertexInfos[1].texCoord, vertexInfos[2].texCoord);

					// CASE: Image corner is inside the triangle
					if (isImageCornerInsideTriangle) {
						BarycentricCoordinate imageCornerBarycentric = BarycentricCoordinate::fromTrianglePoint(imageCornerTex, vertexInfos[0].texCoord, vertexInfos[1].texCoord, vertexInfos[2].texCoord);
						glm::vec3 imageCornerVertex = imageCornerBarycentric.toVector3(vertexInfos[0].vertex, vertexInfos[1].vertex, vertexInfos[2].vertex);
						std::vector<glm::vec2> underlayingImageCornerTexCoords(numberOfUnderlaying);

						for (int a = 0; a < underlayingImageCornerTexCoords.size(); a++) {
							if (isOverlaying) {
								underlayingImageCornerTexCoords[a] = imageCornerBarycentric.toVector2(vertexInfos[0].underlayingTexCoords[a], vertexInfos[1].underlayingTexCoords[a], vertexInfos[2].underlayingTexCoords[a]);
							}
							else {
								underlayingImageCornerTexCoords[a] = m_defaultTexCoord;
							}
						}

						// Connect the image corner with all invalid vertices
						// CASE: Two triangle vertices are inside, one is outside the image polygon
						if (validCount == 2) {
							VertexInformation vertexInfo = getVertexInformationFromIntersection(intersections[0].sideIdx, vertexInfos, vectors.validVertices);

							glm::vec3 invalidVertex = vectors.invalidVertices[0];
							glm::vec3 validVertex1 = vertexInfo.vertex;
							glm::vec3 validVertex2 = *std::find_if(vectors.validVertices.begin(), vectors.validVertices.end(), [&](glm::vec3& vv) {return glm::all(glm::notEqual(vv, validVertex1)); });
							//glm::vec2 validVertex2 = vectors.validVertices.First(vv = > vv != validVertex1);

							glm::vec2 validTex1 = vertexInfo.texCoord;
							glm::vec2 validTex2 = *std::find_if(vectors.validTextureCoordinates.begin(), vectors.validTextureCoordinates.end(), [&](glm::vec2& vv) {return glm::all(glm::notEqual(vv, validTex1)); });
							//glm::vec2 validTex2 = vectors.validTextureCoordinates.First(t = > t != validTex1);
							std::vector<glm::vec2> underlayingTex3 = vertexInfos[vectors.invalidIndices[0]].underlayingTexCoords;

							newVertices.insert(newVertices.end(), { imageCornerVertex, invalidVertex, intersectionVertex2 });
							newVertices.insert(newVertices.end(), { imageCornerVertex, intersectionVertex2, validVertex2 });
							newVertices.insert(newVertices.end(), { imageCornerVertex, validVertex2, validVertex1 });
							newVertices.insert(newVertices.end(), { imageCornerVertex, validVertex1, intersectionVertex1 });
							newVertices.insert(newVertices.end(), { imageCornerVertex, intersectionVertex1, invalidVertex });

							std::vector<glm::vec2> newTexCoords;
							newTexCoords.insert(newTexCoords.end(), { m_defaultTexCoord, m_defaultTexCoord, m_defaultTexCoord });
							newTexCoords.insert(newTexCoords.end(), { imageCornerTex, intersectionTex2, validTex2 });
							newTexCoords.insert(newTexCoords.end(), { imageCornerTex, validTex2, validTex1 });
							newTexCoords.insert(newTexCoords.end(), { imageCornerTex, validTex1, intersectionTex1 });
							newTexCoords.insert(newTexCoords.end(), { m_defaultTexCoord, m_defaultTexCoord, m_defaultTexCoord });;
							newTextureCoordinates.push_back(newTexCoords);

							for (int a = 0; a < numberOfUnderlaying; a++) {
								newTexCoords.insert(newTexCoords.end(), { underlayingImageCornerTexCoords[a], underlayingTex3[a], underlayingIntersectionTex2[a] });
								newTexCoords.insert(newTexCoords.end(), { m_defaultTexCoord, m_defaultTexCoord, m_defaultTexCoord });
								newTexCoords.insert(newTexCoords.end(), { m_defaultTexCoord, m_defaultTexCoord, m_defaultTexCoord });
								newTexCoords.insert(newTexCoords.end(), { m_defaultTexCoord, m_defaultTexCoord, m_defaultTexCoord });
								newTexCoords.insert(newTexCoords.end(), { underlayingImageCornerTexCoords[a], underlayingIntersectionTex1[a], underlayingTex3[a] });
								newTextureCoordinates.push_back(newTexCoords);
							}

						}
						// CASE: One triangle vertex is inside, two are outside the image polygon
						else if (validCount == 1) {
							VertexInformation vertexInfo1 = getVertexInformationFromIntersection(intersections[0].sideIdx, vertexInfos, vectors.invalidVertices);
							VertexInformation vertexInfo2 = getVertexInformationFromIntersection(intersections[1].sideIdx, vertexInfos, vectors.invalidVertices);
							// Could also be used, should be approximately the same speed
							//VertexInformation vertexInfo2 = vertexInfos.First(vi => vi != vertexInfo1 && vectors.InvalidVertices.Contains(vivertex));

							glm::vec3 validVertex = vectors.validVertices[0];
							glm::vec3 invalidVertex1 = vertexInfo1.vertex;
							glm::vec3 invalidVertex2 = vertexInfo2.vertex;

							glm::vec2 validTex = vectors.validTextureCoordinates[0];
							glm::vec2 invalidTex1 = vertexInfo1.texCoord;
							glm::vec2 invalidTex2 = vertexInfo2.texCoord;
							std::vector<glm::vec2> underlayingTex1 = vertexInfo1.underlayingTexCoords;
							std::vector<glm::vec2> underlayingTex2 = vertexInfo2.underlayingTexCoords;

							newVertices.insert(newVertices.end(), { imageCornerVertex, invalidVertex1, invalidVertex2 });
							newVertices.insert(newVertices.end(), { imageCornerVertex, invalidVertex2, intersectionVertex2 });
							newVertices.insert(newVertices.end(), { imageCornerVertex, intersectionVertex2, validVertex });
							newVertices.insert(newVertices.end(), { imageCornerVertex, validVertex, intersectionVertex1 });
							newVertices.insert(newVertices.end(), { imageCornerVertex, intersectionVertex1, invalidVertex1 });

							std::vector<glm::vec2> newTexCoords;
							newTexCoords.insert(newTexCoords.end(), { m_defaultTexCoord, m_defaultTexCoord, m_defaultTexCoord });
							newTexCoords.insert(newTexCoords.end(), { m_defaultTexCoord, m_defaultTexCoord, m_defaultTexCoord });
							newTexCoords.insert(newTexCoords.end(), { imageCornerTex, intersectionTex2, validTex });
							newTexCoords.insert(newTexCoords.end(), { imageCornerTex, validTex, intersectionTex1 });
							newTexCoords.insert(newTexCoords.end(), { m_defaultTexCoord, m_defaultTexCoord, m_defaultTexCoord });
							newTextureCoordinates.push_back(newTexCoords);

							for (int a = 0; a < numberOfUnderlaying; a++) {
								newTexCoords.clear();
								newTexCoords.insert(newTexCoords.end(), { underlayingImageCornerTexCoords[a], underlayingTex1[a], underlayingTex2[a] });
								newTexCoords.insert(newTexCoords.end(), { underlayingImageCornerTexCoords[a], underlayingTex2[a], underlayingIntersectionTex2[a] });
								newTexCoords.insert(newTexCoords.end(), { m_defaultTexCoord, m_defaultTexCoord, m_defaultTexCoord });
								newTexCoords.insert(newTexCoords.end(), { m_defaultTexCoord, m_defaultTexCoord, m_defaultTexCoord });
								newTexCoords.insert(newTexCoords.end(), { underlayingImageCornerTexCoords[a], underlayingIntersectionTex1[a], underlayingTex1[a] });
								newTextureCoordinates.push_back(newTexCoords);
							}
						}
						//// CASE: All triangle vertices are outside the image polygon
						else if (validCount == 0) {
							// The side index is the same for both intersections
							std::vector<VertexInformation> sideVertexInfos = getVertexInformationsFromIntersection(intersections[0].sideIdx, vertexInfos);
							glm::vec3 invalidVertex1 = sideVertexInfos[0].vertex;
							glm::vec3 invalidVertex2 = sideVertexInfos[1].vertex;
							glm::vec3 invalidVertex3 = *std::find_if(vectors.invalidVertices.begin(), vectors.invalidVertices.end(), [&](glm::vec3& v) {return glm::all(glm::notEqual(v, invalidVertex1)) && glm::all(glm::notEqual(v, invalidVertex2)); });

							auto lambda = [&](auto& a, auto& b) { return glm::distance2(a.vertexInfo.vertex, invalidVertex1) < glm::distance2(b.vertexInfo.vertex, invalidVertex1); };
							IntersectionResult intersection1 = *std::min_element(intersections.begin(), intersections.end(), lambda);
							IntersectionResult intersection2 = *std::find_if(intersections.begin(), intersections.end(), [&](auto& v) {return v != intersection1; });
							intersectionVertex1 = intersection1.vertexInfo.vertex;
							intersectionVertex2 = intersection2.vertexInfo.vertex;

							glm::vec2 invalidTex1 = sideVertexInfos[0].texCoord;
							glm::vec2 invalidTex2 = sideVertexInfos[1].texCoord;
							glm::vec2 invalidTex3 = *std::find_if(vectors.invalidTextureCoordinates.begin(), vectors.invalidTextureCoordinates.end(), [&](auto& v) {return glm::all(glm::notEqual(v, invalidTex1)) && glm::all(glm::notEqual(v, invalidTex2)); });
							//glm::vec2 invalidTex3 = vectors.invalidTextureCoordinates.First(v = > v != invalidTex1 && v != invalidTex2);
							intersectionTex1 = intersection1.vertexInfo.texCoord;
							intersectionTex2 = intersection2.vertexInfo.texCoord;
							std::vector<glm::vec2> underlayingTex1 = sideVertexInfos[0].underlayingTexCoords;
							std::vector<glm::vec2> underlayingTex2 = sideVertexInfos[1].underlayingTexCoords;

							std::vector<glm::vec2> underlayingTex3 = generateDefaultTextureCoordinates(numberOfUnderlaying);
							if (isOverlaying) {
								auto firstLambda = [&](auto& vi) {return (vi.underlayingTexCoords != underlayingTex1) && (vi.underlayingTexCoords != underlayingTex2); };
								underlayingTex3 = (*std::find_if(vertexInfos.begin(), vertexInfos.end(), firstLambda)).underlayingTexCoords;
								//underlayingTex3 = vertexInfos.First(vi = > vi.UnderlayingTexCoords != underlayingTex1 && vi.UnderlayingTexCoords != underlayingTex2).UnderlayingTexCoords;
							}
							underlayingIntersectionTex1 = intersection1.vertexInfo.underlayingTexCoords;
							underlayingIntersectionTex2 = intersection2.vertexInfo.underlayingTexCoords;

							newVertices.insert(newVertices.begin(), { imageCornerVertex, invalidVertex3, invalidVertex2 });
							newVertices.insert(newVertices.begin(), { imageCornerVertex, invalidVertex2, intersectionVertex2 });
							newVertices.insert(newVertices.begin(), { imageCornerVertex, intersectionVertex2, intersectionVertex1 });
							newVertices.insert(newVertices.begin(), { imageCornerVertex, intersectionVertex1, invalidVertex1 });
							newVertices.insert(newVertices.begin(), { imageCornerVertex, invalidVertex1, invalidVertex3 });

							std::vector<glm::vec2> newTexCoords;
							newTexCoords.insert(newTexCoords.begin(), { m_defaultTexCoord, m_defaultTexCoord, m_defaultTexCoord });
							newTexCoords.insert(newTexCoords.begin(), { m_defaultTexCoord, m_defaultTexCoord, m_defaultTexCoord });
							newTexCoords.insert(newTexCoords.begin(), { imageCornerTex, intersectionTex2, intersectionTex1 });
							newTexCoords.insert(newTexCoords.begin(), { m_defaultTexCoord, m_defaultTexCoord, m_defaultTexCoord });
							newTexCoords.insert(newTexCoords.begin(), { m_defaultTexCoord, m_defaultTexCoord, m_defaultTexCoord });
							newTextureCoordinates.push_back(newTexCoords);

							for (int a = 0; a < numberOfUnderlaying; a++) {
								newTexCoords.clear();
								newTexCoords.insert(newTexCoords.begin(), { underlayingImageCornerTexCoords[a], underlayingTex3[a], underlayingTex2[a] });
								newTexCoords.insert(newTexCoords.begin(), { underlayingImageCornerTexCoords[a], underlayingTex2[a], underlayingIntersectionTex2[a] });
								newTexCoords.insert(newTexCoords.begin(), { m_defaultTexCoord, m_defaultTexCoord, m_defaultTexCoord });
								newTexCoords.insert(newTexCoords.begin(), { underlayingImageCornerTexCoords[a], underlayingIntersectionTex1[a], underlayingTex1[a] });
								newTexCoords.insert(newTexCoords.begin(), { underlayingImageCornerTexCoords[a], underlayingTex1[a], underlayingTex3[a] });
								newTextureCoordinates.push_back(newTexCoords);
							}
						}
					}
					// CASE: Image corner is outside the triangle
					else {
						// CASE: Triangle gets intersected four times
						if (intersections.size() == 4) {
							std::vector<std::vector<IntersectionResult>> intersectionsBySide;

							int c = 0;
							auto subsetLambda = [&](const auto& i) {return i.sideIdx == c; };
							std::vector<IntersectionResult> output;
							// Use std::copy_if to filter and insert elements directly into output
							std::copy_if(intersections.begin(), intersections.end(), std::back_inserter(output), subsetLambda);
							// Add the filtered subset to intersectionsBySide				
							c = 1;
							std::copy_if(intersections.begin(), intersections.end(), std::back_inserter(output), subsetLambda);
							c = 2;
							std::copy_if(intersections.begin(), intersections.end(), std::back_inserter(output), subsetLambda);
							intersectionsBySide.emplace_back(std::move(output));

							// CASE: One triangle vertex is inside, two are outside the image polygon
							if (validCount == 1) {
								auto it = std::find_if(intersectionsBySide.begin(), intersectionsBySide.end(), [](auto& v) {return v.size() == 2; });
								//std::vector<IntersectionResult> sideWithTwoIntersections = intersectionsBySide.First(v = > v.Length == 2);
								if (it != intersectionsBySide.end()) {
									// Remove the found element from the vector
									intersectionsBySide.erase(it);
								}
								std::vector<IntersectionResult> sidesWithOneIntersection;
								//flatten
								for (auto const& v : intersectionsBySide) {
									sidesWithOneIntersection.insert(sidesWithOneIntersection.end(), v.begin(), v.end());
								}

								glm::vec3 validVertex = vectors.validVertices[0];
								glm::vec3 invalidVertex1 = vectors.invalidVertices[0];
								glm::vec3 invalidVertex2 = vectors.invalidVertices[1];
								auto minLambda = [&](auto& a, auto& b) {return glm::distance2(a.vertexInfo.vertex, invalidVertex1) < glm::distance2(b.vertexInfo.vertex, invalidVertex1); };
								IntersectionResult intersection1 = *std::min_element(sidesWithOneIntersection.begin(), sidesWithOneIntersection.end(), minLambda);
								//IntersectionResult intersection1 = sidesWithOneIntersection.MinBy(v = > vvertexInfovertex.DistanceSquaredTo(invalidVertex1));
								IntersectionResult intersection2 = *std::find_if(sidesWithOneIntersection.begin(), sidesWithOneIntersection.end(), [&](auto& v) {return v != intersection1; });
								//IntersectionResult intersection2 = sidesWithOneIntersection.First(v = > v != intersection1);
								IntersectionResult intersection3 = *std::min_element(sidesWithOneIntersection.begin(), sidesWithOneIntersection.end(), minLambda);
								//IntersectionResult intersection3 = sideWithTwoIntersections.MinBy(v = > vvertexInfovertex.DistanceSquaredTo(invalidVertex1));
								IntersectionResult intersection4 = *std::find_if(sidesWithOneIntersection.begin(), sidesWithOneIntersection.end(), [&](auto& v) {return v != intersection3; });
								//IntersectionResult intersection4 = sideWithTwoIntersections.First(v = > v != intersection3);

								intersectionVertex1 = intersection1.vertexInfo.vertex;
								intersectionVertex2 = intersection2.vertexInfo.vertex;
								glm::vec3 intersectionVertex3 = intersection3.vertexInfo.vertex;
								glm::vec3 intersectionVertex4 = intersection4.vertexInfo.vertex;

								glm::vec2 validTex = vectors.validTextureCoordinates[0];
								glm::vec2 invalidTex1 = vectors.invalidTextureCoordinates[0];
								glm::vec2 invalidTex2 = vectors.invalidTextureCoordinates[1];
								intersectionTex1 = intersection1.vertexInfo.texCoord;
								intersectionTex2 = intersection2.vertexInfo.texCoord;
								glm::vec2 intersectionTex3 = intersection3.vertexInfo.texCoord;
								glm::vec2 intersectionTex4 = intersection4.vertexInfo.texCoord;

								std::vector<glm::vec2> underlayingTex1 = vertexInfos[vectors.invalidIndices[0]].underlayingTexCoords;
								std::vector<glm::vec2> underlayingTex2 = vertexInfos[vectors.invalidIndices[1]].underlayingTexCoords;
								underlayingIntersectionTex1 = intersection1.vertexInfo.underlayingTexCoords;
								underlayingIntersectionTex2 = intersection2.vertexInfo.underlayingTexCoords;
								std::vector<glm::vec2> underlayingIntersectionTex3 = intersection3.vertexInfo.underlayingTexCoords;
								std::vector<glm::vec2> underlayingIntersectionTex4 = intersection4.vertexInfo.underlayingTexCoords;

								newVertices.insert(newVertices.end(), { intersectionVertex1, invalidVertex1, intersectionVertex3 });
								newVertices.insert(newVertices.end(), { intersectionVertex1, intersectionVertex3, validVertex });
								newVertices.insert(newVertices.end(), { intersectionVertex3, intersectionVertex4, validVertex });
								newVertices.insert(newVertices.end(), { intersectionVertex4, intersectionVertex2, validVertex });
								newVertices.insert(newVertices.end(), { intersectionVertex4, invalidVertex2, intersectionVertex2 });

								std::vector<glm::vec2> newTexCoords;
								newTexCoords.insert(newTexCoords.end(), { m_defaultTexCoord, m_defaultTexCoord, m_defaultTexCoord });
								newTexCoords.insert(newTexCoords.end(), { intersectionTex1, intersectionTex3, validTex });
								newTexCoords.insert(newTexCoords.end(), { intersectionTex3, intersectionTex4, validTex });
								newTexCoords.insert(newTexCoords.end(), { intersectionTex4, intersectionTex2, validTex });
								newTexCoords.insert(newTexCoords.end(), { m_defaultTexCoord, m_defaultTexCoord, m_defaultTexCoord });
								newTextureCoordinates.push_back(newTexCoords);

								for (int a = 0; a < numberOfUnderlaying; a++) {
									newTexCoords.clear();
									newTexCoords.insert(newTexCoords.end(), { underlayingIntersectionTex1[a], underlayingTex1[a], underlayingIntersectionTex3[a] });
									newTexCoords.insert(newTexCoords.end(), { m_defaultTexCoord, m_defaultTexCoord, m_defaultTexCoord });
									newTexCoords.insert(newTexCoords.end(), { m_defaultTexCoord, m_defaultTexCoord, m_defaultTexCoord });
									newTexCoords.insert(newTexCoords.end(), { m_defaultTexCoord, m_defaultTexCoord, m_defaultTexCoord });
									newTexCoords.insert(newTexCoords.end(), { underlayingIntersectionTex4[a], underlayingTex2[a], underlayingIntersectionTex2[a] });
									newTextureCoordinates.push_back(newTexCoords);
								}
							}
							// CASE: All triangle vertices are outside the image polygon
							else if (validCount == 0) {
								// The +1 will always work, as the polygon contains the first point inserted as extra point in the end.
								// Meaning, if the criterion for the image corner applies to this point, the very first one will be selected,
								// the last one will never be selected
								std::map<VertexInformation, bool> verticesByRelativePosition;
								for (int v = 0; v < 3; v++) {
									verticesByRelativePosition.insert({ vertexInfos[v], MathExtension::pointRelativeToSide(imageCornerTex, imagePolygon[imageCornerIdx + 1], vertexInfos[v].texCoord) });
								}
								int trueCount = static_cast<int>(std::count_if(verticesByRelativePosition.begin(), verticesByRelativePosition.end(), [](const std::pair<VertexInformation, bool>& p) {return p.second == true; }));
								//int trueCount = verticesByRelativePosition.Count(p = > p.Value == true);
								bool sameSideCriterion = trueCount == 2;
								// Create a vector to store vertexInfos on the same side
								std::vector<VertexInformation> vertexInfosOnSameSide;

								// Iterate through the map and select vertexInfos based on sameSideCriterion
								for (const auto& pair : verticesByRelativePosition) {
									if (pair.second == sameSideCriterion) {
										vertexInfosOnSameSide.push_back(pair.first);
									}
								}

								// Find the vertexInfo on the other side
								VertexInformation vertexInfoOnOtherSide;
								for (const auto& pair : verticesByRelativePosition) {
									if (std::find(vertexInfosOnSameSide.begin(), vertexInfosOnSameSide.end(), pair.first) == vertexInfosOnSameSide.end()) {
										vertexInfoOnOtherSide = pair.first;
										break;
									}
								}

								glm::vec3 invalidVertex1 = vertexInfosOnSameSide[0].vertex;
								glm::vec3 invalidVertex2 = vertexInfosOnSameSide[1].vertex;
								glm::vec3 invalidVertex3 = vertexInfoOnOtherSide.vertex;
								std::vector<glm::vec2> underlayingTex1 = vertexInfosOnSameSide[0].underlayingTexCoords;
								std::vector<glm::vec2> underlayingTex2 = vertexInfosOnSameSide[1].underlayingTexCoords;
								std::vector<glm::vec2> underlayingTex3 = vertexInfoOnOtherSide.underlayingTexCoords;

								int invalidVertexSideIdx1 = getSideIndexFromVertexIndices(vertexInfoOnOtherSide.vertexIdx, vertexInfosOnSameSide[0].vertexIdx);
								int invalidVertexSideIdx2 = getSideIndexFromVertexIndices(vertexInfoOnOtherSide.vertexIdx, vertexInfosOnSameSide[1].vertexIdx);

								// Side 1: v2-v3
								// Side 2: v1-v3
								//NOT USEDauto output = intersectionsBySide
								//NOT USED	| std::views::filter([](auto& v) {return v.size() == 2; });
								//NOT USED//std::vector<std::vector<IntersectionResult>> intersectionsWithTwoBySide(output.begin(), output.end());
								//std::vector<std::vector<IntersectionResult>> intersectionsWithTwoBySide = intersectionsBySide.Where(v = > v.Length == 2).ToArray();
								auto minLambda = [&](auto& a, auto& b) {return glm::distance2(a.vertexInfo.vertex, invalidVertex1) < glm::distance2(b.vertexInfo.vertex, invalidVertex1); };
								IntersectionResult intersection1 = *std::min_element(intersectionsBySide[invalidVertexSideIdx1].begin(), intersectionsBySide[invalidVertexSideIdx1].end(), minLambda);
								//IntersectionResult intersection1 = intersectionsBySide[invalidVertexSideIdx1].MinBy(v = > vvertexInfovertex.DistanceSquaredTo(invalidVertex1));
								auto minLambda2 = [&](auto& a, auto& b) {return glm::distance2(a.vertexInfo.vertex, invalidVertex2) < glm::distance2(b.vertexInfo.vertex, invalidVertex2); };
								IntersectionResult intersection2 = *std::min_element(intersectionsBySide[invalidVertexSideIdx2].begin(), intersectionsBySide[invalidVertexSideIdx2].end(), minLambda2);
								IntersectionResult intersection3 = *std::find_if(intersectionsBySide[invalidVertexSideIdx1].begin(), intersectionsBySide[invalidVertexSideIdx1].end(), [&](auto& v) {return v != intersection1; });
								//IntersectionResult intersection3 = intersectionsBySide[invalidVertexSideIdx1].First(v = > v != intersection1);
								IntersectionResult intersection4 = *std::find_if(intersectionsBySide[invalidVertexSideIdx2].begin(), intersectionsBySide[invalidVertexSideIdx2].end(), [&](auto& v) {return v != intersection2; });
								//IntersectionResult intersection4 = intersectionsBySide[invalidVertexSideIdx2].First(v = > v != intersection2);

								intersectionVertex1 = intersection1.vertexInfo.vertex;
								intersectionVertex2 = intersection2.vertexInfo.vertex;
								glm::vec3 intersectionVertex3 = intersection3.vertexInfo.vertex;
								glm::vec3 intersectionVertex4 = intersection4.vertexInfo.vertex;
								intersectionTex1 = intersection1.vertexInfo.texCoord;
								intersectionTex2 = intersection2.vertexInfo.texCoord;
								glm::vec2 intersectionTex3 = intersection3.vertexInfo.texCoord;
								glm::vec2 intersectionTex4 = intersection4.vertexInfo.texCoord;
								underlayingIntersectionTex1 = intersection1.vertexInfo.underlayingTexCoords;
								underlayingIntersectionTex2 = intersection2.vertexInfo.underlayingTexCoords;
								std::vector<glm::vec2> underlayingIntersectionTex3 = intersection3.vertexInfo.underlayingTexCoords;
								std::vector<glm::vec2> underlayingIntersectionTex4 = intersection4.vertexInfo.underlayingTexCoords;

								newVertices.insert(newVertices.end(), { invalidVertex1, intersectionVertex1, invalidVertex2 });
								newVertices.insert(newVertices.end(), { invalidVertex2, intersectionVertex1, intersectionVertex2 });
								newVertices.insert(newVertices.end(), { intersectionVertex1, intersectionVertex4, intersectionVertex2 });
								newVertices.insert(newVertices.end(), { intersectionVertex1, intersectionVertex3, intersectionVertex4 });
								newVertices.insert(newVertices.end(), { intersectionVertex3, invalidVertex3, intersectionVertex4 });

								std::vector<glm::vec2> newTexCoords;
								newTexCoords.insert(newTexCoords.end(), { m_defaultTexCoord, m_defaultTexCoord, m_defaultTexCoord });
								newTexCoords.insert(newTexCoords.end(), { m_defaultTexCoord, m_defaultTexCoord, m_defaultTexCoord });
								newTexCoords.insert(newTexCoords.end(), { intersectionTex1, intersectionTex4, intersectionTex2 });
								newTexCoords.insert(newTexCoords.end(), { intersectionTex1, intersectionTex3, intersectionTex4 });
								newTexCoords.insert(newTexCoords.end(), { m_defaultTexCoord, m_defaultTexCoord, m_defaultTexCoord });
								newTextureCoordinates.push_back(newTexCoords);

								for (int a = 0; a < numberOfUnderlaying; a++) {
									newTexCoords.clear();
									newTexCoords.insert(newTexCoords.end(), { underlayingTex1[a], underlayingIntersectionTex1[a], underlayingTex2[a] });
									newTexCoords.insert(newTexCoords.end(), { underlayingTex2[a], underlayingIntersectionTex1[a], underlayingIntersectionTex2[a] });
									newTexCoords.insert(newTexCoords.end(), { m_defaultTexCoord, m_defaultTexCoord, m_defaultTexCoord });
									newTexCoords.insert(newTexCoords.end(), { m_defaultTexCoord, m_defaultTexCoord, m_defaultTexCoord });
									newTexCoords.insert(newTexCoords.end(), { underlayingIntersectionTex3[a], underlayingTex3[a], underlayingIntersectionTex4[a] });
									newTextureCoordinates.push_back(newTexCoords);
								}
							}
						}
						// CASE: Triangle gets intersected two times
						else {
							// CASE: Two triangle vertices are inside, one is outside the image polygon
							if (validCount == 2) {
								VertexInformation vertexInfo1 = getVertexInformationFromIntersection(intersections[0].sideIdx, vertexInfos, vectors.validVertices);
								VertexInformation vertexInfo2 = getVertexInformationFromIntersection(intersections[1].sideIdx, vertexInfos, vectors.validVertices);

								glm::vec3 invalidVertex = vectors.invalidVertices[0];
								glm::vec3 validVertex1 = vertexInfo1.vertex;
								glm::vec3 validVertex2 = vertexInfo2.vertex;

								glm::vec2 invalidTex = vectors.invalidTextureCoordinates[0];
								glm::vec2 validTex1 = vertexInfo1.texCoord;
								glm::vec2 validTex2 = vertexInfo2.texCoord;

								std::vector<glm::vec2> underlayingTex3 = generateDefaultTextureCoordinates(numberOfUnderlaying);
								if (isOverlaying) {
									underlayingTex3 = (*std::find_if(vertexInfos.begin(), vertexInfos.end(), [&](auto& vi) {return (vi != vertexInfo1) && (vi != vertexInfo2); })).underlayingTexCoords;
									//underlayingTex3 = vertexInfos.First(vi = > vi != vertexInfo1 && vi != vertexInfo2).UnderlayingTexCoords;
								}

								newVertices.insert(newVertices.end(), { intersectionVertex1, invalidVertex, intersectionVertex2 });
								newVertices.insert(newVertices.end(), { intersectionVertex1, intersectionVertex2, validVertex2 });
								newVertices.insert(newVertices.end(), { intersectionVertex1, validVertex2, validVertex1 });
								//
								std::vector<glm::vec2> newTexCoords;
								newTexCoords.insert(newTexCoords.end(), { m_defaultTexCoord, m_defaultTexCoord, m_defaultTexCoord });
								newTexCoords.insert(newTexCoords.end(), { intersectionTex1, intersectionTex2, validTex2 });
								newTexCoords.insert(newTexCoords.end(), { intersectionTex1, validTex2, validTex1 });
								newTextureCoordinates.push_back(newTexCoords);

								for (int a = 0; a < numberOfUnderlaying; a++) {
									newTexCoords.clear();
									newTexCoords.insert(newTexCoords.end(), { underlayingIntersectionTex1[a], underlayingTex3[a], underlayingIntersectionTex2[a] });
									newTexCoords.insert(newTexCoords.end(), { m_defaultTexCoord, m_defaultTexCoord, m_defaultTexCoord });
									newTexCoords.insert(newTexCoords.end(), { m_defaultTexCoord, m_defaultTexCoord, m_defaultTexCoord });
									newTextureCoordinates.push_back(newTexCoords);
								}
							}
							// CASE: One triangle vertex is inside, two are outside the image polygon
							else if (validCount == 1) {
								VertexInformation vertexInfo1 = getVertexInformationFromIntersection(intersections[0].sideIdx, vertexInfos, vectors.invalidVertices);
								VertexInformation vertexInfo2 = getVertexInformationFromIntersection(intersections[1].sideIdx, vertexInfos, vectors.invalidVertices);

								glm::vec3 validVertex = vectors.validVertices[0];
								glm::vec3 invalidVertex1 = vertexInfo1.vertex;
								glm::vec3 invalidVertex2 = vertexInfo2.vertex;

								glm::vec2 validTex = vectors.validTextureCoordinates[0];
								glm::vec2 invalidTex1 = vertexInfo1.texCoord;
								glm::vec2 invalidTex2 = vertexInfo2.texCoord;
								std::vector<glm::vec2> underlayingTex1 = vertexInfo1.underlayingTexCoords;
								std::vector<glm::vec2> underlayingTex2 = vertexInfo2.underlayingTexCoords;

								newVertices.insert(newVertices.end(), { intersectionVertex1, invalidVertex1, invalidVertex2 });
								newVertices.insert(newVertices.end(), { intersectionVertex1, invalidVertex2, intersectionVertex2 });
								newVertices.insert(newVertices.end(), { intersectionVertex1, intersectionVertex2, validVertex });

								std::vector<glm::vec2> newTexCoords;
								newTexCoords.insert(newTexCoords.end(), { m_defaultTexCoord, m_defaultTexCoord, m_defaultTexCoord });
								newTexCoords.insert(newTexCoords.end(), { m_defaultTexCoord, m_defaultTexCoord, m_defaultTexCoord });
								newTexCoords.insert(newTexCoords.end(), { intersectionTex1, intersectionTex2, validTex });
								newTextureCoordinates.push_back(newTexCoords);

								for (int a = 0; a < numberOfUnderlaying; a++) {
									newTexCoords.clear();
									newTexCoords.insert(newTexCoords.end(), { underlayingIntersectionTex1[a], underlayingTex1[a], underlayingTex2[a] });
									newTexCoords.insert(newTexCoords.end(), { underlayingIntersectionTex1[a], underlayingTex2[a], underlayingIntersectionTex2[a] });
									newTexCoords.insert(newTexCoords.end(), { m_defaultTexCoord, m_defaultTexCoord, m_defaultTexCoord });
									newTextureCoordinates.push_back(newTexCoords);
								}
							}
						}
					}

					std::vector<std::optional<Engine::Mesh::Vertex>> newVertexes;
					for (auto& nv : newVertices) {
						newVertexes.push_back(std::make_optional<Engine::Mesh::Vertex>(nv));
					}

					//std::vector<MappingDataSet> affectedDataSets;
					//for (const auto* ptr : affectedSets) {
					//	affectedDataSets.emplace_back(*ptr); // Create a new MappingDataSet object from the pointer
					//}

					if (newVertices.size() > 0) {
						TriangleReplacement replacement{
							triangleStartVertexId,
							triangleArrayIndices,
							newVertexes,
							clonedDataSets,
							newTextureCoordinates,
							affectedSets
						};
						triangleReplacements.push_back(replacement);
					}
				}
			}
			auto t2 = std::chrono::high_resolution_clock::now();
			auto ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
			std::cout << ms_int.count() << "ms loop time\n";

			handleTriangleReplacements(triangleReplacements);

			clonedDataSets.erase(std::remove_if(clonedDataSets.begin(), clonedDataSets.end(), [dominatingSet](const MappingDataSet* dataSet) {return dataSet == dominatingSet; }),clonedDataSets.end());
			//clonedDataSets.Remove(dominatingSet);

		}

		int vertexCountWithNulls = static_cast<int>(m_modelVertices->size());
		std::vector<int> modelOriginIds(vertexCountWithNulls / 3);
		std::vector<glm::vec2> modelTexCoords = generateDefaultTextureCoordinates(vertexCountWithNulls);

		for (size_t i = 0; i < dataSets.size(); i++) {
			MappingDataSet& dataSet = dataSets[i];
			std::vector<glm::vec2> texCoords = dataSet.texCoordsAndTriangleIds.first;

			for (int a = 0; a < dataSet.texCoordsAndTriangleIds.second.size(); a++) {
				int triangleId = dataSet.texCoordsAndTriangleIds.second[a];
				if (triangleId > -1) {
					int texStartId = a * 3;
					if (texCoords[texStartId].x != m_defaultTexCoord.x) {
						int triangleVertexId = triangleId * 3;
						modelTexCoords[triangleVertexId] = texCoords[texStartId];
						modelTexCoords[triangleVertexId + 1] = texCoords[texStartId + 1];
						modelTexCoords[triangleVertexId + 2] = texCoords[texStartId + 2];
						modelOriginIds[triangleId] = static_cast<int>(i);
					}
				}
			}
		}

		m_model.vertices().clear();
		std::vector<glm::vec2> modelTexCoordsList;
		std::vector<int> modelOriginIdsList;

		for (int i = 0; i < vertexCountWithNulls; i += 3) {
			if ((*m_modelVertices)[i].has_value()) {
				for (int a = 0; a < 3; a++) {
					int idx = i + a;
					m_model.vertices().push_back(std::move(*(*m_modelVertices)[idx]));
					//m_model.vertices().back().texCoords = modelTexCoords[idx];
					modelTexCoordsList.push_back(modelTexCoords[idx]);
				}
				modelOriginIdsList.push_back(modelOriginIds[i / 3]);
			}
		}

		std::pair<std::vector<glm::vec2>, std::vector<int>> result(modelTexCoordsList, modelOriginIdsList);
		return result;
	}

	/// Handles the triangle replacements.
	void MeshCutter::handleTriangleReplacements(std::vector<TriangleReplacement>& triangleReplacements) {
		for (size_t i = 0; i < triangleReplacements.size(); i++) {
			TriangleReplacement& replacement = triangleReplacements[i];

			int numberOfNewVertices = static_cast<int>(replacement.newVertices.size());
			int triangleStartVertexId = replacement.triangleStartVertexId;
			int numberOfNewTriangles = static_cast<int>(replacement.newVertices.size() / 3);
			int lastTriangleId = static_cast<int>(m_modelVertices->size() / 3 - 1);
			// Don't remove elements, it is a really slow operation
			// It is sufficient to set only the first vertex to null

			for (int a = 0; a < numberOfNewVertices; a++) {
				replacement.newVertices[a]->normal = (*m_modelVertices)[triangleStartVertexId]->normal;
			}
			(*m_modelVertices)[triangleStartVertexId].reset();
			m_modelVertices->insert(m_modelVertices->end(), replacement.newVertices.begin(), replacement.newVertices.end());

			// TODO: Filter out the triangles with invalid tex coords, don't add them to the datasets. Dont even add them to the newtexcoords in the first place
			for (size_t i = 0; i < replacement.affectedDataSets.size(); i++) {
				const std::vector<glm::vec2>& newTexCoords = replacement.newTextureCoordinates[i];
				MappingDataSet* dataSet = replacement.affectedDataSets[i];

				int triangleArrayIdx = replacement.triangleArrayIndices[i];
				dataSet->texCoordsAndTriangleIds.first.insert(dataSet->texCoordsAndTriangleIds.first.end(), newTexCoords.begin(), newTexCoords.end());
				dataSet->texCoordsAndTriangleIds.second[triangleArrayIdx] = -1;

				// TODO: The dominating set doesnt need this as it will be removed afterwards?
				for (int a = 0; a < numberOfNewTriangles; a++) {
					// TODO: Make a list with the new triangles beforehand and perform AddRange for each dataset
					int newTriangleId = lastTriangleId + a + 1;
					dataSet->texCoordsAndTriangleIds.second.push_back(newTriangleId);
					dataSet->triangleHash.insert(newTriangleId);
				}
			}
			//addNormals(replacement.triangleStartVertexId, numberOfNewVertices); C#
			//handleVertexAndTextureCoordinateReplacement(replacement); C#
		}
		return;
	}

	std::vector<IntersectionResult> MeshCutter::calculateTriangleIntersections(std::vector<glm::vec2> imagePolygon, std::vector<VertexInformation> vertices) {
		int numberOfUnderlaying = static_cast<int>(vertices[0].underlayingTexCoords.size());
		std::vector<Ray2> triangleRays(3);
		std::vector<std::vector<Ray2>> underlayingRays(numberOfUnderlaying);
		for (auto& v : underlayingRays)
			v.resize(3);

		std::vector<Ray3> triangleSides(3);
		std::vector<int> indices{ 0, 1, 0, 1, 2, 1, 0, 2, 0 };

		int idx = 0;
		for (int i = 0; i < 3; i++) {
			triangleRays[i] = Ray2(vertices[indices[idx]].texCoord, vertices[indices[idx + 1]].texCoord - vertices[indices[idx + 2]].texCoord);
			for (int a = 0; a < numberOfUnderlaying; a++) {
				underlayingRays[a][i] = Ray2(vertices[indices[idx]].underlayingTexCoords[a], vertices[indices[idx + 1]].underlayingTexCoords[a] - vertices[indices[idx + 2]].underlayingTexCoords[a]);
			}
			triangleSides[i] = Ray3(vertices[indices[idx]].vertex, vertices[indices[idx + 1]].vertex - vertices[indices[idx + 2]].vertex);
			idx += 3;
		}

		std::vector<IntersectionResult> intersections;
		for (size_t i = 0; i < triangleRays.size(); i++) {
			Ray2 triangleRay = triangleRays[i];

			// The last point is the first point again
			for (size_t a = 0; a < imagePolygon.size() - 1; a++) {
				Ray2 polygonRay = getPolygonRay(static_cast<int>(a), imagePolygon);
				std::optional<glm::vec2> intersectionPoint = triangleRay.getIntersectionWithRay(polygonRay);
				if (intersectionPoint.has_value()) {
					float sideLength = glm::length(triangleRay.getDirection());
					float relativeCoordinate = glm::length(triangleRay.getOrigin() - *intersectionPoint) / sideLength;
					glm::vec3 vertex = triangleSides[i].getOrigin() + triangleSides[i].getDirection() * relativeCoordinate;

					std::vector<glm::vec2> underlayingTexCoords(underlayingRays.size());
					for (int b = 0; b < numberOfUnderlaying; b++) {
						underlayingTexCoords[b] = underlayingRays[b][i].getOrigin() + underlayingRays[b][i].getDirection() * relativeCoordinate;
					}

					IntersectionResult intersection((int)i, vertex, *intersectionPoint, underlayingTexCoords);
					intersections.push_back(intersection);
				}
			}
		}

		return intersections;
	}

	Ray2 MeshCutter::getPolygonRay(int index, const std::vector<glm::vec2>& imagePolygon) {
		glm::vec2 origin = imagePolygon[index];
		glm::vec2 direction = imagePolygon[index + 1] - origin;
		Ray2 polygonRay(origin, direction);
		return polygonRay;
	}

	VectorResult MeshCutter::determineValidAndInvalidVectors(int triangleStartVertexIdx, const std::vector<glm::vec2>& textureCoordinates, const std::vector<glm::vec2>& imagePolygon) {
		auto output = textureCoordinates |
			std::views::filter([&](const auto& t) {return PolygonHelper::isPointInsidePolygon(imagePolygon, t.x, t.y); });
		std::vector<glm::vec2> validTextureCoordinates(output.begin(), output.end());
		//Vector2[] validTextureCoordinates = textureCoordinates.Where(t = > PolygonHelper.IsPointInsidePolygon(imagePolygon, t)).ToArray();
		auto output2 = textureCoordinates |
			std::views::filter([&](const auto& t) {return std::find(validTextureCoordinates.begin(), validTextureCoordinates.end(), t) == validTextureCoordinates.end(); });
		std::vector<glm::vec2> invalidTextureCoordinates(output2.begin(), output2.end());
		//textureCoordinates.Except(validTextureCoordinates).ToArray();
		std::vector<int> validIndices;
		for (auto& c : validTextureCoordinates) {
			auto itr = std::find(textureCoordinates.begin(), textureCoordinates.end(), c);
			validIndices.push_back(static_cast<int>(itr - textureCoordinates.begin()));
		}
		//std::vector<int> validIndices = findIndices(textureCoordinates, validTextureCoordinates);
		std::vector<int> invalidIndices;
		for (auto& c : invalidTextureCoordinates) {
			auto itr = std::find(textureCoordinates.begin(), textureCoordinates.end(), c);
			invalidIndices.push_back(static_cast<int>(itr - textureCoordinates.begin()));
		}
		//std::vector<int> invalidIndices = findIndices(textureCoordinates, invalidTextureCoordinates);
		std::vector<glm::vec3> validVertices = findVertices(triangleStartVertexIdx, validIndices);
		//std::vector<glm::vec3> validVertices = findVertices(triangleStartVertexIdx, validIndices);
		std::vector<glm::vec3> invalidVertices = findVertices(triangleStartVertexIdx, invalidIndices);
		//std::vector<glm::vec3> invalidVertices = findVertices(triangleStartVertexIdx, invalidIndices);

		VectorResult result{
			validIndices,
			invalidIndices,
			validTextureCoordinates,
			invalidTextureCoordinates,
			validVertices,
			invalidVertices
		};
		return result;
	}

	std::vector<glm::vec3> MeshCutter::findVertices(int triangleStartVertexIdx, const std::vector<int>& indices) {
		std::vector<glm::vec3> vertices(indices.size());
		for (size_t i = 0; i < indices.size(); i++) {
			vertices[i] = (*m_modelVertices)[triangleStartVertexIdx + indices[i]]->position;
		}
		return vertices;
	}

	//std::vector<int> MeshCutter::findIndices<T>(std::vector<glm::vec2> array, T[] valuesToFind) {
	//	int[] indices = new int[valuesToFind.Length];
	//	for (int i = 0; i < indices.Length; i++) {
	//		indices[i] = Array.IndexOf(array, valuesToFind[i]);
	//	}
	//	return indices;
	//}

	std::vector<glm::vec2> MeshCutter::generateDefaultTextureCoordinates(int size) {
		std::vector<glm::vec2> textureCoordinates(size, m_defaultTexCoord);
		return textureCoordinates;
	}

	int MeshCutter::getSideIndexFromVertexIndices(int idx1, int idx2) {
		if (idx1 == 0 && idx2 == 1 || idx1 == 1 && idx2 == 0) {
			return 0;
		}
		int sideIdx = abs(idx1 - idx2);
		return sideIdx;
	}

	std::vector<VertexInformation> MeshCutter::getVertexInformationsFromIntersection(int sideIdx, const std::vector<VertexInformation>& vertexInfos) {
		std::vector<VertexInformation> result;
		result.push_back(vertexInfos[sideIdx]);
		result.push_back(vertexInfos[(sideIdx + 1) % 3]);
		return result;
	}

	VertexInformation MeshCutter::getVertexInformationFromIntersection(int sideIdx, const std::vector<VertexInformation>& vertexInfos, const std::vector<glm::vec3>& pool) {
		std::vector<VertexInformation> candidates = getVertexInformationsFromIntersection(sideIdx, vertexInfos);

		VertexInformation result;
		for (const VertexInformation& candidate : candidates) {
			if (std::find(pool.begin(), pool.end(), candidate.vertex) != pool.end()) {
				result = candidate;
				break;
			}
		}

		return result;
	}

}

