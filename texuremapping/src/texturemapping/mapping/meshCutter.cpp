#include "texturemapping/mapping/meshCutter.h"
#include <algorithm>
#include <iostream>

namespace TextureMapping {

	MeshCutter::MeshCutter(Model& model)
		:m_model(model),
		m_defaultTexCoord(glm::vec2(-1000, -1000)),
		m_modelVertices(model.getVertices()),
		m_modelNormals(model.getNormals())
	{}

	// TODO: Either use "id" or "idx"
	/// <summary>
	/// Solves the overlaying texture conflicts.
	/// </summary>
	/// <param name="dataSets">The data sets.</param>
	/// <param name="observer">The observer.</param>
	/// <returns></returns>
	std::optional<std::pair<std::vector<glm::vec2>, std::vector<int>>> MeshCutter::solveOverlayingTexturesConflicts(std::vector<MappingDataSet>& dataSets) {

		std::vector<MappingDataSet> clonedDataSets(dataSets);

		while (clonedDataSets.size() > 0) {
			if (m_isCancelled) {
				return {};
			}
#if PARALLEL
			ConcurrentBag<TriangleReplacement> triangleReplacements = new ConcurrentBag<TriangleReplacement>();
#else
			std::vector<TriangleReplacement> triangleReplacements;
			MappingDataSet& dominatingSet = clonedDataSets[0];

			std::vector<int> uniqueTriangleIds;

			for (auto& mds : clonedDataSets) {
				for (int i = 0; i < mds.texCoordsAndTriangleIds.second.size(); i++) {
					uniqueTriangleIds.push_back(mds.texCoordsAndTriangleIds.second[i]);
				}
			}
			// sort followed by unique, to remove all duplicates

			std::sort(uniqueTriangleIds.begin(), uniqueTriangleIds.end());

			for (auto& item : uniqueTriangleIds)
				std::cout << item << " ";

			std::cout << "-------------------------------------------" << std::endl;

			auto last = std::unique(uniqueTriangleIds.begin(), uniqueTriangleIds.end());
			uniqueTriangleIds.erase(last, uniqueTriangleIds.end());

			for (auto& item : uniqueTriangleIds)
				std::cout << item << " ";

			std::cout << "-------------------------------------------" << std::endl;


			//std::vector<int> uniqueTriangleIds = clonedDataSets.SelectMany(d = > d.TexCoordsAndTriangleIds.Item2).Distinct().ToList();
			//uniqueTriangleIds.Remove(-1);
#endif

#if PARALLEL
			// For is faster than foreach
			Parallel.For(0, uniqueTriangleIds.Count, i = > {
#else
			for (int i = 0; i < uniqueTriangleIds.size(); i++) {
#endif
				int triangleId = uniqueTriangleIds[i];
				int triangleStartVertexId = triangleId * 3;

				std::vector<VertexInformation> vertexInfos{
					VertexInformation(0),
					VertexInformation(1),
					VertexInformation(2)
				};

				// For large lists, reinitializing seems to be more efficient than clearing them
				// "Clear" is a O(n) operation
				std::vector<MappingDataSet> affectedSets;
				std::vector<int> triangleArrayIndices;
				/*
								for (int a = 0; a < clonedDataSets.Count; a++) {
									MappingDataSet dataSet = clonedDataSets[a];

									// Searching a value in a HashSet is a O(1) operation
									// Doing this before gathering the actual index saves a lot of time
									if (dataSet.TriangleHash.Contains(triangleId)) {
										affectedSets.Add(dataSet);

										// One could also use the BinarySearch method here, which is faster than IndexOf.
										// Fo this, the List has to be sorted beforehand. This has been tested, but did not work for some reason.
										// Even after sorting, wrong results came back
										int triangleArrayIdx = dataSet.TexCoordsAndTriangleIds.Item2.IndexOf(triangleId);
										int texId = triangleArrayIdx * 3;

										triangleArrayIndices.Add(triangleArrayIdx);
										Vector2[] texVectors = {
											dataSet.TexCoordsAndTriangleIds.Item1[texId],
											dataSet.TexCoordsAndTriangleIds.Item1[texId + 1],
											dataSet.TexCoordsAndTriangleIds.Item1[texId + 2]
										};

										for (int b = 0; b < texVectors.Length; b++) {
											vertexInfos[b].UnderlayingTexCoords.Add(texVectors[b]);
										}
									}
								}

								int domIdx = affectedSets.IndexOf(dominatingSet);
								if (domIdx == -1) {
				#if PARALLEL
									return;
				#else
									continue;
				#endif
								}

								for (int a = 0; a < 3; a++) {
									VertexInformation vertexInfo = vertexInfos[a];
									vertexInfo.TexCoord = vertexInfo.UnderlayingTexCoords[domIdx];
									vertexInfo.UnderlayingTexCoords.RemoveAt(domIdx);
									vertexInfo.Vertex = _modelVertices[triangleStartVertexId + a].Value;
								}

								Vector2[] dominatingTexCoords = {
									vertexInfos[0].TexCoord,
									vertexInfos[1].TexCoord,
									vertexInfos[2].TexCoord
								};

								int numberOfUnderlaying = vertexInfos[0].UnderlayingTexCoords.Count;
								bool isOverlaying = vertexInfos[0].UnderlayingTexCoords.Count > 0;
								Vector2[] imagePolygon = dominatingSet.ImagePolygonPointsInUV;

								int validCount = dominatingTexCoords.Count(t = > PolygonHelper.IsPointInsidePolygon(imagePolygon, t));

								// CASE: Triangle is completely inside the image polygon
								if (validCount == 3) {
									for (int a = 0; a < affectedSets.Count; a++) {
										MappingDataSet dataSet = affectedSets[a];
										if (dataSet == dominatingSet) {
											continue;
										}
										dataSet.TexCoordsAndTriangleIds.Item2[triangleArrayIndices[a]] = -1;
									}
				#if PARALLEL
									return;
				#else
									continue;
				#endif
								}
								// CASE: Triangle intersects the image polygon
								else {
									Customstd::vector<Vector3 ? > newVertices = new Customstd::vector<Vector3 ? >();
									Customstd::vector<Vector2>[] newTextureCoordinates = new Customstd::vector<Vector2>[numberOfUnderlaying + 1];

									// This might not be a good criterion at the first look, but the image corner only plays a role
									// if it is inside the triangle - in this case the correct corner will be detected in any case
									Vector2 imageCornerTex = imagePolygon.MinBy(p = > vertexInfos[0].TexCoord.DistanceSquaredTo(p));
									int imageCornerIdx = Array.FindIndex(imagePolygon, p = > p == imageCornerTex);

									VectorResult vectors = DetermineValidAndInvalidVectors(triangleStartVertexId, dominatingTexCoords, imagePolygon);
									std::vector<IntersectionResult> intersections = CalculateTriangleIntersections(imagePolygon, vertexInfos);

									// This case can not occur through projection, but it can occur when a triangle has been split up and the current dominating set
									// was an underlaying set
									if (intersections.Count < 2) {
										dominatingSet.TexCoordsAndTriangleIds.Item2[triangleArrayIndices[0]] = -1;
				#if PARALLEL
										return;
				#else
										continue;
				#endif
									}

									Vector3 intersectionVertex1 = intersections[0].VertexInfo.Vertex;
									Vector3 intersectionVertex2 = intersections[1].VertexInfo.Vertex;

									Vector2 intersectionTex1 = intersections[0].VertexInfo.TexCoord;
									Vector2 intersectionTex2 = intersections[1].VertexInfo.TexCoord;
									std::vector<Vector2> underlayingIntersectionTex1 = intersections[0].VertexInfo.UnderlayingTexCoords;
									std::vector<Vector2> underlayingIntersectionTex2 = intersections[1].VertexInfo.UnderlayingTexCoords;

									// Check if the image corner is inside the triangle
									bool isImageCornerInsideTriangle = MathExt.IsPointInsideTriangle(imageCornerTex, vertexInfos[0].TexCoord, vertexInfos[1].TexCoord, vertexInfos[2].TexCoord);

									// CASE: Image corner is inside the triangle
									if (isImageCornerInsideTriangle) {
										BarycentricCoordinate imageCornerBarycentric = BarycentricCoordinate.FromTrianglePoint(imageCornerTex, vertexInfos[0].TexCoord, vertexInfos[1].TexCoord, vertexInfos[2].TexCoord);
										Vector3 imageCornerVertex = imageCornerBarycentric.ToVector3(vertexInfos[0].Vertex, vertexInfos[1].Vertex, vertexInfos[2].Vertex);
										Vector2[] underlayingImageCornerTexCoords = new Vector2[numberOfUnderlaying];

										for (int a = 0; a < underlayingImageCornerTexCoords.Length; a++) {
											if (isOverlaying) {
												underlayingImageCornerTexCoords[a] = imageCornerBarycentric.ToVector2(vertexInfos[0].UnderlayingTexCoords[a], vertexInfos[1].UnderlayingTexCoords[a], vertexInfos[2].UnderlayingTexCoords[a]);
											}
											else {
												underlayingImageCornerTexCoords[a] = _defaultTexCoord;
											}
										}

										// Connect the image corner with all invalid vertices
										// CASE: Two triangle vertices are inside, one is outside the image polygon
										if (validCount == 2) {
											VertexInformation vertexInfo = GetVertexInformationFromIntersection(intersections[0].SideIdx, vertexInfos, vectors.ValidVertices);

											Vector3 invalidVertex = vectors.InvalidVertices[0];
											Vector3 validVertex1 = vertexInfo.Vertex;
											Vector3 validVertex2 = vectors.ValidVertices.First(vv = > vv != validVertex1);

											Vector2 validTex1 = vertexInfo.TexCoord;
											Vector2 validTex2 = vectors.ValidTextureCoordinates.First(t = > t != validTex1);
											std::vector<Vector2> underlayingTex3 = vertexInfos[vectors.InvalidIndices[0]].UnderlayingTexCoords;

											newVertices.Add(imageCornerVertex, invalidVertex, intersectionVertex2);
											newVertices.Add(imageCornerVertex, intersectionVertex2, validVertex2);
											newVertices.Add(imageCornerVertex, validVertex2, validVertex1);
											newVertices.Add(imageCornerVertex, validVertex1, intersectionVertex1);
											newVertices.Add(imageCornerVertex, intersectionVertex1, invalidVertex);

											Customstd::vector<Vector2> newTexCoords = new Customstd::vector<Vector2>();
											newTexCoords.Add(_defaultTexCoord, _defaultTexCoord, _defaultTexCoord);
											newTexCoords.Add(imageCornerTex, intersectionTex2, validTex2);
											newTexCoords.Add(imageCornerTex, validTex2, validTex1);
											newTexCoords.Add(imageCornerTex, validTex1, intersectionTex1);
											newTexCoords.Add(_defaultTexCoord, _defaultTexCoord, _defaultTexCoord);
											newTextureCoordinates[0] = newTexCoords;

											for (int a = 0; a < numberOfUnderlaying; a++) {
												newTexCoords = new Customstd::vector<Vector2>();
												newTexCoords.Add(underlayingImageCornerTexCoords[a], underlayingTex3[a], underlayingIntersectionTex2[a]);
												newTexCoords.Add(_defaultTexCoord, _defaultTexCoord, _defaultTexCoord);
												newTexCoords.Add(_defaultTexCoord, _defaultTexCoord, _defaultTexCoord);
												newTexCoords.Add(_defaultTexCoord, _defaultTexCoord, _defaultTexCoord);
												newTexCoords.Add(underlayingImageCornerTexCoords[a], underlayingIntersectionTex1[a], underlayingTex3[a]);
												newTextureCoordinates[a + 1] = newTexCoords;
											}
										}
										// CASE: One triangle vertex is inside, two are outside the image polygon
										else if (validCount == 1) {
											VertexInformation vertexInfo1 = GetVertexInformationFromIntersection(intersections[0].SideIdx, vertexInfos, vectors.InvalidVertices);
											VertexInformation vertexInfo2 = GetVertexInformationFromIntersection(intersections[1].SideIdx, vertexInfos, vectors.InvalidVertices);
											// Could also be used, should be approximately the same speed
											//VertexInformation vertexInfo2 = vertexInfos.First(vi => vi != vertexInfo1 && vectors.InvalidVertices.Contains(vi.Vertex));

											Vector3 validVertex = vectors.ValidVertices[0];
											Vector3 invalidVertex1 = vertexInfo1.Vertex;
											Vector3 invalidVertex2 = vertexInfo2.Vertex;

											Vector2 validTex = vectors.ValidTextureCoordinates[0];
											Vector2 invalidTex1 = vertexInfo1.TexCoord;
											Vector2 invalidTex2 = vertexInfo2.TexCoord;
											std::vector<Vector2> underlayingTex1 = vertexInfo1.UnderlayingTexCoords;
											std::vector<Vector2> underlayingTex2 = vertexInfo2.UnderlayingTexCoords;

											newVertices.Add(imageCornerVertex, invalidVertex1, invalidVertex2);
											newVertices.Add(imageCornerVertex, invalidVertex2, intersectionVertex2);
											newVertices.Add(imageCornerVertex, intersectionVertex2, validVertex);
											newVertices.Add(imageCornerVertex, validVertex, intersectionVertex1);
											newVertices.Add(imageCornerVertex, intersectionVertex1, invalidVertex1);

											Customstd::vector<Vector2> newTexCoords = new Customstd::vector<Vector2>();
											newTexCoords.Add(_defaultTexCoord, _defaultTexCoord, _defaultTexCoord);
											newTexCoords.Add(_defaultTexCoord, _defaultTexCoord, _defaultTexCoord);
											newTexCoords.Add(imageCornerTex, intersectionTex2, validTex);
											newTexCoords.Add(imageCornerTex, validTex, intersectionTex1);
											newTexCoords.Add(_defaultTexCoord, _defaultTexCoord, _defaultTexCoord);
											newTextureCoordinates[0] = newTexCoords;

											for (int a = 0; a < numberOfUnderlaying; a++) {
												newTexCoords = new Customstd::vector<Vector2>();
												newTexCoords.Add(underlayingImageCornerTexCoords[a], underlayingTex1[a], underlayingTex2[a]);
												newTexCoords.Add(underlayingImageCornerTexCoords[a], underlayingTex2[a], underlayingIntersectionTex2[a]);
												newTexCoords.Add(_defaultTexCoord, _defaultTexCoord, _defaultTexCoord);
												newTexCoords.Add(_defaultTexCoord, _defaultTexCoord, _defaultTexCoord);
												newTexCoords.Add(underlayingImageCornerTexCoords[a], underlayingIntersectionTex1[a], underlayingTex1[a]);
												newTextureCoordinates[a + 1] = newTexCoords;
											}
										}
										// CASE: All triangle vertices are outside the image polygon
										else if (validCount == 0) {
											// The side index is the same for both intersections
											VertexInformation[] sideVertexInfos = GetVertexInformationsFromIntersection(intersections[0].SideIdx, vertexInfos);
											Vector3 invalidVertex1 = sideVertexInfos[0].Vertex;
											Vector3 invalidVertex2 = sideVertexInfos[1].Vertex;
											Vector3 invalidVertex3 = vectors.InvalidVertices.First(v = > v != invalidVertex1 && v != invalidVertex2);
											IntersectionResult intersection1 = intersections.MinBy(v = > v.VertexInfo.Vertex.DistanceSquaredTo(invalidVertex1));
											IntersectionResult intersection2 = intersections.First(v = > v != intersection1);
											intersectionVertex1 = intersection1.VertexInfo.Vertex;
											intersectionVertex2 = intersection2.VertexInfo.Vertex;

											Vector2 invalidTex1 = sideVertexInfos[0].TexCoord;
											Vector2 invalidTex2 = sideVertexInfos[1].TexCoord;
											Vector2 invalidTex3 = vectors.InvalidTextureCoordinates.First(v = > v != invalidTex1 && v != invalidTex2);
											intersectionTex1 = intersection1.VertexInfo.TexCoord;
											intersectionTex2 = intersection2.VertexInfo.TexCoord;
											std::vector<Vector2> underlayingTex1 = sideVertexInfos[0].UnderlayingTexCoords;
											std::vector<Vector2> underlayingTex2 = sideVertexInfos[1].UnderlayingTexCoords;

											std::vector<Vector2> underlayingTex3 = GenerateDefaultTextureCoordinates(numberOfUnderlaying).ToList();
											if (isOverlaying) {
												underlayingTex3 = vertexInfos.First(vi = > vi.UnderlayingTexCoords != underlayingTex1 && vi.UnderlayingTexCoords != underlayingTex2).UnderlayingTexCoords;
											}
											underlayingIntersectionTex1 = intersection1.VertexInfo.UnderlayingTexCoords;
											underlayingIntersectionTex2 = intersection2.VertexInfo.UnderlayingTexCoords;

											newVertices.Add(imageCornerVertex, invalidVertex3, invalidVertex2);
											newVertices.Add(imageCornerVertex, invalidVertex2, intersectionVertex2);
											newVertices.Add(imageCornerVertex, intersectionVertex2, intersectionVertex1);
											newVertices.Add(imageCornerVertex, intersectionVertex1, invalidVertex1);
											newVertices.Add(imageCornerVertex, invalidVertex1, invalidVertex3);

											Customstd::vector<Vector2> newTexCoords = new Customstd::vector<Vector2>();
											newTexCoords.Add(_defaultTexCoord, _defaultTexCoord, _defaultTexCoord);
											newTexCoords.Add(_defaultTexCoord, _defaultTexCoord, _defaultTexCoord);
											newTexCoords.Add(imageCornerTex, intersectionTex2, intersectionTex1);
											newTexCoords.Add(_defaultTexCoord, _defaultTexCoord, _defaultTexCoord);
											newTexCoords.Add(_defaultTexCoord, _defaultTexCoord, _defaultTexCoord);
											newTextureCoordinates[0] = newTexCoords;

											for (int a = 0; a < numberOfUnderlaying; a++) {
												newTexCoords = new Customstd::vector<Vector2>();
												newTexCoords.Add(underlayingImageCornerTexCoords[a], underlayingTex3[a], underlayingTex2[a]);
												newTexCoords.Add(underlayingImageCornerTexCoords[a], underlayingTex2[a], underlayingIntersectionTex2[a]);
												newTexCoords.Add(_defaultTexCoord, _defaultTexCoord, _defaultTexCoord);
												newTexCoords.Add(underlayingImageCornerTexCoords[a], underlayingIntersectionTex1[a], underlayingTex1[a]);
												newTexCoords.Add(underlayingImageCornerTexCoords[a], underlayingTex1[a], underlayingTex3[a]);
												newTextureCoordinates[a + 1] = newTexCoords;
											}
										}
									}
									// CASE: Image corner is outside the triangle
									else {
										// CASE: Triangle gets intersected four times
										if (intersections.Count == 4) {
											std::vector<IntersectionResult[]> intersectionsBySide = new std::vector<IntersectionResult[]>();
											intersectionsBySide.Add(intersections.Where(v = > v.SideIdx == 0).ToArray());
											intersectionsBySide.Add(intersections.Where(v = > v.SideIdx == 1).ToArray());
											intersectionsBySide.Add(intersections.Where(v = > v.SideIdx == 2).ToArray());

											// CASE: One triangle vertex is inside, two are outside the image polygon
											if (validCount == 1) {
												IntersectionResult[] sideWithTwoIntersections = intersectionsBySide.First(v = > v.Length == 2);
												intersectionsBySide.Remove(sideWithTwoIntersections);
												IntersectionResult[] sidesWithOneIntersection = intersectionsBySide.SelectMany(v = > v).ToArray();

												Vector3 validVertex = vectors.ValidVertices[0];
												Vector3 invalidVertex1 = vectors.InvalidVertices[0];
												Vector3 invalidVertex2 = vectors.InvalidVertices[1];
												IntersectionResult intersection1 = sidesWithOneIntersection.MinBy(v = > v.VertexInfo.Vertex.DistanceSquaredTo(invalidVertex1));
												IntersectionResult intersection2 = sidesWithOneIntersection.First(v = > v != intersection1);
												IntersectionResult intersection3 = sideWithTwoIntersections.MinBy(v = > v.VertexInfo.Vertex.DistanceSquaredTo(invalidVertex1));
												IntersectionResult intersection4 = sideWithTwoIntersections.First(v = > v != intersection3);

												intersectionVertex1 = intersection1.VertexInfo.Vertex;
												intersectionVertex2 = intersection2.VertexInfo.Vertex;
												Vector3 intersectionVertex3 = intersection3.VertexInfo.Vertex;
												Vector3 intersectionVertex4 = intersection4.VertexInfo.Vertex;

												Vector2 validTex = vectors.ValidTextureCoordinates[0];
												Vector2 invalidTex1 = vectors.InvalidTextureCoordinates[0];
												Vector2 invalidTex2 = vectors.InvalidTextureCoordinates[1];
												intersectionTex1 = intersection1.VertexInfo.TexCoord;
												intersectionTex2 = intersection2.VertexInfo.TexCoord;
												Vector2 intersectionTex3 = intersection3.VertexInfo.TexCoord;
												Vector2 intersectionTex4 = intersection4.VertexInfo.TexCoord;

												std::vector<Vector2> underlayingTex1 = vertexInfos[vectors.InvalidIndices[0]].UnderlayingTexCoords;
												std::vector<Vector2> underlayingTex2 = vertexInfos[vectors.InvalidIndices[1]].UnderlayingTexCoords;
												underlayingIntersectionTex1 = intersection1.VertexInfo.UnderlayingTexCoords;
												underlayingIntersectionTex2 = intersection2.VertexInfo.UnderlayingTexCoords;
												std::vector<Vector2> underlayingIntersectionTex3 = intersection3.VertexInfo.UnderlayingTexCoords;
												std::vector<Vector2> underlayingIntersectionTex4 = intersection4.VertexInfo.UnderlayingTexCoords;

												newVertices.Add(intersectionVertex1, invalidVertex1, intersectionVertex3);
												newVertices.Add(intersectionVertex1, intersectionVertex3, validVertex);
												newVertices.Add(intersectionVertex3, intersectionVertex4, validVertex);
												newVertices.Add(intersectionVertex4, intersectionVertex2, validVertex);
												newVertices.Add(intersectionVertex4, invalidVertex2, intersectionVertex2);


												Customstd::vector<Vector2> newTexCoords = new Customstd::vector<Vector2>();
												newTexCoords.Add(_defaultTexCoord, _defaultTexCoord, _defaultTexCoord);
												newTexCoords.Add(intersectionTex1, intersectionTex3, validTex);
												newTexCoords.Add(intersectionTex3, intersectionTex4, validTex);
												newTexCoords.Add(intersectionTex4, intersectionTex2, validTex);
												newTexCoords.Add(_defaultTexCoord, _defaultTexCoord, _defaultTexCoord);
												newTextureCoordinates[0] = newTexCoords;

												for (int a = 0; a < numberOfUnderlaying; a++) {
													newTexCoords = new Customstd::vector<Vector2>();
													newTexCoords.Add(underlayingIntersectionTex1[a], underlayingTex1[a], underlayingIntersectionTex3[a]);
													newTexCoords.Add(_defaultTexCoord, _defaultTexCoord, _defaultTexCoord);
													newTexCoords.Add(_defaultTexCoord, _defaultTexCoord, _defaultTexCoord);
													newTexCoords.Add(_defaultTexCoord, _defaultTexCoord, _defaultTexCoord);
													newTexCoords.Add(underlayingIntersectionTex4[a], underlayingTex2[a], underlayingIntersectionTex2[a]);
													newTextureCoordinates[a + 1] = newTexCoords;
												}
											}
											// CASE: All triangle vertices are outside the image polygon
											else if (validCount == 0) {
												// The +1 will always work, as the polygon contains the first point inserted as extra point in the end.
												// Meaning, if the criterion for the image corner applies to this point, the very first one will be selected,
												// the last one will never be selected
												Dictionary<VertexInformation, bool> verticesByRelativePosition = new Dictionary<VertexInformation, bool>();
												for (int v = 0; v < 3; v++) {
													verticesByRelativePosition.Add(vertexInfos[v], MathExt.PointRelativeToSide(imageCornerTex, imagePolygon[imageCornerIdx + 1], vertexInfos[v].TexCoord));
												}

												int trueCount = verticesByRelativePosition.Count(p = > p.Value == true);
												bool sameSideCriterion = trueCount == 2;
												VertexInformation[] vertexInfosOnSameSide = verticesByRelativePosition.Where(p = > p.Value == sameSideCriterion).Select(p = > p.Key).ToArray();
												VertexInformation vertexInfoOnOtherSide = verticesByRelativePosition.First(p = > !vertexInfosOnSameSide.Contains(p.Key)).Key;

												Vector3 invalidVertex1 = vertexInfosOnSameSide[0].Vertex;
												Vector3 invalidVertex2 = vertexInfosOnSameSide[1].Vertex;
												Vector3 invalidVertex3 = vertexInfoOnOtherSide.Vertex;
												std::vector<Vector2> underlayingTex1 = vertexInfosOnSameSide[0].UnderlayingTexCoords;
												std::vector<Vector2> underlayingTex2 = vertexInfosOnSameSide[1].UnderlayingTexCoords;
												std::vector<Vector2> underlayingTex3 = vertexInfoOnOtherSide.UnderlayingTexCoords;

												int invalidVertexSideIdx1 = GetSideIndexFromVertexIndices(vertexInfoOnOtherSide.VertexIdx, vertexInfosOnSameSide[0].VertexIdx);
												int invalidVertexSideIdx2 = GetSideIndexFromVertexIndices(vertexInfoOnOtherSide.VertexIdx, vertexInfosOnSameSide[1].VertexIdx);

												// Side 1: v2-v3
												// Side 2: v1-v3
												IntersectionResult[][] intersectionsWithTwoBySide = intersectionsBySide.Where(v = > v.Length == 2).ToArray();
												IntersectionResult intersection1 = intersectionsBySide[invalidVertexSideIdx1].MinBy(v = > v.VertexInfo.Vertex.DistanceSquaredTo(invalidVertex1));
												IntersectionResult intersection2 = intersectionsBySide[invalidVertexSideIdx2].MinBy(v = > v.VertexInfo.Vertex.DistanceSquaredTo(invalidVertex2));
												IntersectionResult intersection3 = intersectionsBySide[invalidVertexSideIdx1].First(v = > v != intersection1);
												IntersectionResult intersection4 = intersectionsBySide[invalidVertexSideIdx2].First(v = > v != intersection2);

												intersectionVertex1 = intersection1.VertexInfo.Vertex;
												intersectionVertex2 = intersection2.VertexInfo.Vertex;
												Vector3 intersectionVertex3 = intersection3.VertexInfo.Vertex;
												Vector3 intersectionVertex4 = intersection4.VertexInfo.Vertex;
												intersectionTex1 = intersection1.VertexInfo.TexCoord;
												intersectionTex2 = intersection2.VertexInfo.TexCoord;
												Vector2 intersectionTex3 = intersection3.VertexInfo.TexCoord;
												Vector2 intersectionTex4 = intersection4.VertexInfo.TexCoord;
												underlayingIntersectionTex1 = intersection1.VertexInfo.UnderlayingTexCoords;
												underlayingIntersectionTex2 = intersection2.VertexInfo.UnderlayingTexCoords;
												std::vector<Vector2> underlayingIntersectionTex3 = intersection3.VertexInfo.UnderlayingTexCoords;
												std::vector<Vector2> underlayingIntersectionTex4 = intersection4.VertexInfo.UnderlayingTexCoords;

												newVertices.Add(invalidVertex1, intersectionVertex1, invalidVertex2);
												newVertices.Add(invalidVertex2, intersectionVertex1, intersectionVertex2);
												newVertices.Add(intersectionVertex1, intersectionVertex4, intersectionVertex2);
												newVertices.Add(intersectionVertex1, intersectionVertex3, intersectionVertex4);
												newVertices.Add(intersectionVertex3, invalidVertex3, intersectionVertex4);

												Customstd::vector<Vector2> newTexCoords = new Customstd::vector<Vector2>();
												newTexCoords.Add(_defaultTexCoord, _defaultTexCoord, _defaultTexCoord);
												newTexCoords.Add(_defaultTexCoord, _defaultTexCoord, _defaultTexCoord);
												newTexCoords.Add(intersectionTex1, intersectionTex4, intersectionTex2);
												newTexCoords.Add(intersectionTex1, intersectionTex3, intersectionTex4);
												newTexCoords.Add(_defaultTexCoord, _defaultTexCoord, _defaultTexCoord);
												newTextureCoordinates[0] = newTexCoords;

												for (int a = 0; a < numberOfUnderlaying; a++) {
													newTexCoords = new Customstd::vector<Vector2>();
													newTexCoords.Add(underlayingTex1[a], underlayingIntersectionTex1[a], underlayingTex2[a]);
													newTexCoords.Add(underlayingTex2[a], underlayingIntersectionTex1[a], underlayingIntersectionTex2[a]);
													newTexCoords.Add(_defaultTexCoord, _defaultTexCoord, _defaultTexCoord);
													newTexCoords.Add(_defaultTexCoord, _defaultTexCoord, _defaultTexCoord);
													newTexCoords.Add(underlayingIntersectionTex3[a], underlayingTex3[a], underlayingIntersectionTex4[a]);
													newTextureCoordinates[a + 1] = newTexCoords;
												}
											}
										}
										// CASE: Triangle gets intersected two times
										else {
											// CASE: Two triangle vertices are inside, one is outside the image polygon
											if (validCount == 2) {
												VertexInformation vertexInfo1 = GetVertexInformationFromIntersection(intersections[0].SideIdx, vertexInfos, vectors.ValidVertices);
												VertexInformation vertexInfo2 = GetVertexInformationFromIntersection(intersections[1].SideIdx, vertexInfos, vectors.ValidVertices);

												Vector3 invalidVertex = vectors.InvalidVertices[0];
												Vector3 validVertex1 = vertexInfo1.Vertex;
												Vector3 validVertex2 = vertexInfo2.Vertex;

												Vector2 invalidTex = vectors.InvalidTextureCoordinates[0];
												Vector2 validTex1 = vertexInfo1.TexCoord;
												Vector2 validTex2 = vertexInfo2.TexCoord;

												std::vector<Vector2> underlayingTex3 = GenerateDefaultTextureCoordinates(numberOfUnderlaying).ToList();
												if (isOverlaying) {
													underlayingTex3 = vertexInfos.First(vi = > vi != vertexInfo1 && vi != vertexInfo2).UnderlayingTexCoords;
												}

												newVertices.Add(intersectionVertex1, invalidVertex, intersectionVertex2);
												newVertices.Add(intersectionVertex1, intersectionVertex2, validVertex2);
												newVertices.Add(intersectionVertex1, validVertex2, validVertex1);

												Customstd::vector<Vector2> newTexCoords = new Customstd::vector<Vector2>();
												newTexCoords.Add(_defaultTexCoord, _defaultTexCoord, _defaultTexCoord);
												newTexCoords.Add(intersectionTex1, intersectionTex2, validTex2);
												newTexCoords.Add(intersectionTex1, validTex2, validTex1);
												newTextureCoordinates[0] = newTexCoords;

												for (int a = 0; a < numberOfUnderlaying; a++) {
													newTexCoords = new Customstd::vector<Vector2>();
													newTexCoords.Add(underlayingIntersectionTex1[a], underlayingTex3[a], underlayingIntersectionTex2[a]);
													newTexCoords.Add(_defaultTexCoord, _defaultTexCoord, _defaultTexCoord);
													newTexCoords.Add(_defaultTexCoord, _defaultTexCoord, _defaultTexCoord);
													newTextureCoordinates[a + 1] = newTexCoords;
												}
											}
											// CASE: One triangle vertex is inside, two are outside the image polygon
											else if (validCount == 1) {
												VertexInformation vertexInfo1 = GetVertexInformationFromIntersection(intersections[0].SideIdx, vertexInfos, vectors.InvalidVertices);
												VertexInformation vertexInfo2 = GetVertexInformationFromIntersection(intersections[1].SideIdx, vertexInfos, vectors.InvalidVertices);

												Vector3 validVertex = vectors.ValidVertices[0];
												Vector3 invalidVertex1 = vertexInfo1.Vertex;
												Vector3 invalidVertex2 = vertexInfo2.Vertex;

												Vector2 validTex = vectors.ValidTextureCoordinates[0];
												Vector2 invalidTex1 = vertexInfo1.TexCoord;
												Vector2 invalidTex2 = vertexInfo2.TexCoord;
												std::vector<Vector2> underlayingTex1 = vertexInfo1.UnderlayingTexCoords;
												std::vector<Vector2> underlayingTex2 = vertexInfo2.UnderlayingTexCoords;

												newVertices.Add(intersectionVertex1, invalidVertex1, invalidVertex2);
												newVertices.Add(intersectionVertex1, invalidVertex2, intersectionVertex2);
												newVertices.Add(intersectionVertex1, intersectionVertex2, validVertex);

												Customstd::vector<Vector2> newTexCoords = new Customstd::vector<Vector2>();
												newTexCoords.Add(_defaultTexCoord, _defaultTexCoord, _defaultTexCoord);
												newTexCoords.Add(_defaultTexCoord, _defaultTexCoord, _defaultTexCoord);
												newTexCoords.Add(intersectionTex1, intersectionTex2, validTex);
												newTextureCoordinates[0] = newTexCoords;

												for (int a = 0; a < numberOfUnderlaying; a++) {
													newTexCoords = new Customstd::vector<Vector2>();
													newTexCoords.Add(underlayingIntersectionTex1[a], underlayingTex1[a], underlayingTex2[a]);
													newTexCoords.Add(underlayingIntersectionTex1[a], underlayingTex2[a], underlayingIntersectionTex2[a]);
													newTexCoords.Add(_defaultTexCoord, _defaultTexCoord, _defaultTexCoord);
													newTextureCoordinates[a + 1] = newTexCoords;
												}
											}
										}
									}

									if (newVertices.Count > 0) {
										TriangleReplacement replacement = new TriangleReplacement(){
											DataSets = clonedDataSets,
											AffectedDataSets = affectedSets,
											NewVertices = newVertices,
											TriangleArrayIndices = triangleArrayIndices,
											TriangleStartVertexId = triangleStartVertexId,
											NewTextureCoordinates = newTextureCoordinates,
										};
										triangleReplacements.Add(replacement);
									}
								}
				#if PARALLEL
							});
							HandleTriangleReplacements(triangleReplacements.ToList(), dataSets);
				#else
							}
							HandleTriangleReplacements(triangleReplacements, dataSets);
				#endif
							clonedDataSets.Remove(dominatingSet);
							StopWatch();
							Profiler.SolverOverlayingTimeMs += TrackedTime;

							observer.IncrementSolving(Profiler.SolverOverlayingTimeMs);
						}

						StartWatch();
						int vertexCountWithNulls = _modelVertices.Count;
						int[] modelOriginIds = new int[vertexCountWithNulls / 3];
						Vector2[] modelTexCoords = GenerateDefaultTextureCoordinates(vertexCountWithNulls);

						for (int i = 0; i < dataSets.Count; i++) {
							MappingDataSet dataSet = dataSets[i];
							std::vector<Vector2> texCoords = dataSet.TexCoordsAndTriangleIds.Item1;

							for (int a = 0; a < dataSet.TexCoordsAndTriangleIds.Item2.Count; a++) {
								int triangleId = dataSet.TexCoordsAndTriangleIds.Item2[a];
								if (triangleId > -1) {
									int texStartId = a * 3;
									if (texCoords[texStartId].X != _defaultTexCoord.X) {
										int triangleVertexId = triangleId * 3;
										modelTexCoords[triangleVertexId] = texCoords[texStartId];
										modelTexCoords[triangleVertexId + 1] = texCoords[texStartId + 1];
										modelTexCoords[triangleVertexId + 2] = texCoords[texStartId + 2];
										modelOriginIds[triangleId] = i;
									}
								}
							}
						}

						_model.Vertices = new std::vector<Vector3>();
						_model.Normals = new std::vector<Vector3>();
						std::vector<Vector2> modelTexCoordsList = new std::vector<Vector2>();
						std::vector<int> modelOriginIdsList = new std::vector<int>();

						for (int i = 0; i < vertexCountWithNulls; i += 3) {
							Vector3 ? vertex = _modelVertices[i];
							if (vertex.HasValue) {
								for (int a = 0; a < 3; a++) {
									int idx = i + a;
									_model.Vertices.Add(_modelVertices[idx].Value);
									_model.Normals.Add(_modelNormals[idx].Value);
									modelTexCoordsList.Add(modelTexCoords[idx]);
								}
								modelOriginIdsList.Add(modelOriginIds[i / 3]);
							}
						}

						StopWatch();
						Profiler.SolverOverlayingTimeMs += TrackedTime;

						Tuple<std::vector<Vector2>, std::vector<int>> result = new Tuple<std::vector<Vector2>, std::vector<int>>(modelTexCoordsList, modelOriginIdsList);
						return result;
					}

					/// <summary>
					/// Cancels the current process.
					/// </summary>
					void cancel() {
						_isCancelled = true;
					}

					/// <summary>
					/// Handles the triangle replacements.
					/// </summary>
					/// <param name="triangleReplacements">The triangle replacements.</param>
					/// <param name="dataSets">The data sets.</param>
					private void HandleTriangleReplacements(std::vector<TriangleReplacement> triangleReplacements, std::vector<MappingDataSet> dataSets) {
						for (int i = 0; i < triangleReplacements.Count; i++) {
							TriangleReplacement replacement = triangleReplacements[i];
							int numberOfNewVertices = replacement.NewVertices.Count;
							int triangleId = replacement.TriangleStartVertexId / 3;

							AddNormals(replacement.TriangleStartVertexId, numberOfNewVertices);
							HandleVertexAndTextureCoordinateReplacement(replacement);
						}
					}

					/// <summary>
					/// Handles the vertex and texture coordinate replacement.
					/// </summary>
					/// <param name="replacement">The replacement.</param>
					private void HandleVertexAndTextureCoordinateReplacement(TriangleReplacement replacement) {
						int triangleStartVertexId = replacement.TriangleStartVertexId;
						int numberOfNewTriangles = replacement.NewVertices.Count / 3;

						// Don't remove elements, it is a really slow operation
						// It is sufficient to set only the first vertex to null
						_modelVertices[triangleStartVertexId] = null;

						int lastTriangleId = _modelVertices.Count / 3 - 1;
						_modelVertices.AddRange(replacement.NewVertices);

						// TODO: Filter out the triangles with invalid tex coords, don't add them to the datasets. Dont even add them to the newtexcoords in the first place
						for (int i = 0; i < replacement.AffectedDataSets.Count; i++) {
							std::vector<Vector2> newTexCoords = replacement.NewTextureCoordinates[i];
							MappingDataSet dataSet = replacement.AffectedDataSets[i];

							int triangleArrayIdx = replacement.TriangleArrayIndices[i];
							dataSet.TexCoordsAndTriangleIds.Item1.AddRange(newTexCoords);
							dataSet.TexCoordsAndTriangleIds.Item2[triangleArrayIdx] = -1;

							// TODO: The dominating set doesnt need this as it will be removed afterwards?
							for (int a = 0; a < numberOfNewTriangles; a++) {
								// TODO: Make a list with the new triangles beforehand and perform AddRange for each dataset
								int newTriangleId = lastTriangleId + a + 1;
								dataSet.TexCoordsAndTriangleIds.Item2.Add(newTriangleId);
								dataSet.TriangleHash.Add(newTriangleId);
							}
						}
					}

					/// <summary>
					/// Adds normals for the new triangles.
					/// </summary>
					/// <param name="triangleStartVertexId">The triangle start vertex identifier.</param>
					/// <param name="numberOfInsertions">The number of insertions.</param>
					private void AddNormals(int triangleStartVertexId, int numberOfInsertions) {
						// All new triangles have the same normal like the one before
						// No need to set the old normal to null like for the vertices
						Vector3 normal = _modelNormals[triangleStartVertexId].Value;
						for (int a = 0; a < numberOfInsertions; a++) {
							_modelNormals.Add(normal);
						}
					}

					/// <summary>
					/// Calculates the triangle intersections.
					/// </summary>
					/// <param name="imagePolygon">The image polygon.</param>
					/// <param name="vertices">The vertices.</param>
					/// <returns></returns>
					private std::vector<IntersectionResult> CalculateTriangleIntersections(Vector2[] imagePolygon, VertexInformation[] vertices) {
						int numberOfUnderlaying = vertices[0].UnderlayingTexCoords.Count;
						Ray2[] triangleRays = new Ray2[3];
						Ray2[, ] underlayingRays = new Ray2[numberOfUnderlaying, 3];
						Ray3[] triangleSides = new Ray3[3];
						int[] indices = new int[] { 0, 1, 0, 1, 2, 1, 0, 2, 0 };

						int idx = 0;
						for (int i = 0; i < 3; i++) {
							triangleRays[i] = new Ray2(vertices[indices[idx]].TexCoord, vertices[indices[idx + 1]].TexCoord - vertices[indices[idx + 2]].TexCoord);
							for (int a = 0; a < numberOfUnderlaying; a++) {
								underlayingRays[a, i] = new Ray2(vertices[indices[idx]].UnderlayingTexCoords[a], vertices[indices[idx + 1]].UnderlayingTexCoords[a] - vertices[indices[idx + 2]].UnderlayingTexCoords[a]);
							}
							triangleSides[i] = new Ray3(vertices[indices[idx]].Vertex, vertices[indices[idx + 1]].Vertex - vertices[indices[idx + 2]].Vertex);
							idx += 3;
						}

						std::vector<IntersectionResult> intersections = new std::vector<IntersectionResult>();
						for (int i = 0; i < triangleRays.Length; i++) {
							Ray2 triangleRay = triangleRays[i];

							// The last point is the first point again
							for (int a = 0; a < imagePolygon.Length - 1; a++) {
								Ray2 polygonRay = GetPolygonRay(a, imagePolygon);
								Vector2 ? intersectionPoint = triangleRay.GetIntersectionWithRay(polygonRay);
								if (intersectionPoint != null) {
									float sideLength = triangleRay.Direction.Length;
									float relativeCoordinate = (triangleRay.Start - intersectionPoint.Value).Length / sideLength;
									Vector3 vertex = triangleSides[i].Start + triangleSides[i].Direction * relativeCoordinate;

									Vector2[] underlayingTexCoords = new Vector2[underlayingRays.Length];
									for (int b = 0; b < numberOfUnderlaying; b++) {
										underlayingTexCoords[b] = underlayingRays[b, i].Start + underlayingRays[b, i].Direction * relativeCoordinate;
									}

									IntersectionResult intersection = new IntersectionResult(i, vertex, intersectionPoint.Value, underlayingTexCoords);
									intersections.Add(intersection);
								}
							}
						}

						return intersections;
					}

					Ray2 Ray2::getPolygonRay(int index, const std::vector<glm::vec2>& imagePolygon) {
						glm::vec2 origin = imagePolygon[index];
						glm::vec2 direction = imagePolygon[index + 1] - origin;
						Ray2 polygonRay(origin, direction);
						return polygonRay;
					}

					/// <summary>
					/// Determines the valid and invalid vectors.
					/// </summary>
					/// <param name="triangleStartVertexIdx">Index of the triangle start vertex.</param>
					/// <param name="textureCoordinates">The texture coordinates.</param>
					/// <param name="imagePolygon">The image polygon.</param>
					/// <returns></returns>
					private VectorResult DetermineValidAndInvalidVectors(int triangleStartVertexIdx, Vector2[] textureCoordinates, Vector2[] imagePolygon) {
						Vector2[] validTextureCoordinates = textureCoordinates.Where(t = > PolygonHelper.IsPointInsidePolygon(imagePolygon, t)).ToArray();
						Vector2[] invalidTextureCoordinates = textureCoordinates.Except(validTextureCoordinates).ToArray();
						int[] validIndices = FindIndices(textureCoordinates, validTextureCoordinates);
						int[] invalidIndices = FindIndices(textureCoordinates, invalidTextureCoordinates);
						Vector3[] validVertices = FindVertices(triangleStartVertexIdx, validIndices);
						Vector3[] invalidVertices = FindVertices(triangleStartVertexIdx, invalidIndices);

						VectorResult result = new VectorResult(){
							ValidIndices = validIndices,
							InvalidIndices = invalidIndices,
							ValidVertices = validVertices,
							InvalidVertices = invalidVertices,
							ValidTextureCoordinates = validTextureCoordinates,
							InvalidTextureCoordinates = invalidTextureCoordinates
						};
						return result;
					}

					/// <summary>
					/// Finds valid or invalid vertices.
					/// </summary>
					/// <param name="triangleStartVertexIdx">Index of the triangle start vertex.</param>
					/// <param name="indices">The indices.</param>
					/// <returns></returns>
					private Vector3[] FindVertices(int triangleStartVertexIdx, int[] indices) {
						Vector3[] vertices = new Vector3[indices.Length];
						for (int i = 0; i < indices.Length; i++) {
							vertices[i] = _modelVertices[triangleStartVertexIdx + indices[i]].Value;
						}
						return vertices;
					}

					/// <summary>
					/// Finds the indices.
					/// </summary>
					/// <typeparam name="T"></typeparam>
					/// <param name="array">The array.</param>
					/// <param name="valuesToFind">The values to find.</param>
					/// <returns></returns>
					private int[] FindIndices<T>(T[] array, T[] valuesToFind) {
						int[] indices = new int[valuesToFind.Length];
						for (int i = 0; i < indices.Length; i++) {
							indices[i] = Array.IndexOf(array, valuesToFind[i]);
						}
						return indices;
					}

					/// <summary>
					/// Generates default texture coordinates.
					/// </summary>
					/// <param name="size">The size.</param>
					/// <returns></returns>
					private Vector2[] GenerateDefaultTextureCoordinates(int size) {
						Vector2[] textureCoordinates = new Vector2[size];
						for (int i = 0; i < textureCoordinates.Length; i++) {
							textureCoordinates[i] = _defaultTexCoord;
						}
						return textureCoordinates;
					}

					/// <summary>
					/// Gets the side index from vertex indices.
					/// </summary>
					/// <param name="idx1">The idx1.</param>
					/// <param name="idx2">The idx2.</param>
					/// <returns></returns>
					private int GetSideIndexFromVertexIndices(int idx1, int idx2) {
						if (idx1 == 0 && idx2 == 1 || idx1 == 1 && idx2 == 0) {
							return 0;
						}
						int sideIdx = Math.Abs(idx1 - idx2);
						return sideIdx;
					}

					/// <summary>
					/// Gets the vertex informations from intersection.
					/// </summary>
					/// <param name="sideIdx">Index of the side.</param>
					/// <param name="vertexInfos">The vertex infos.</param>
					/// <returns></returns>
					private VertexInformation[] GetVertexInformationsFromIntersection(int sideIdx, VertexInformation[] vertexInfos) {
						VertexInformation[] result = new VertexInformation[]{ vertexInfos[sideIdx], vertexInfos[(sideIdx + 1) % 3] };
						return result;
					}

					/// <summary>
					/// Gets the vertex information from intersection.
					/// </summary>
					/// <param name="sideIdx">Index of the side.</param>
					/// <param name="vertexInfos">The vertex infos.</param>
					/// <param name="pool">The pool.</param>
					/// <returns></returns>
					private VertexInformation GetVertexInformationFromIntersection(int sideIdx, VertexInformation[] vertexInfos, Vector3[] pool) {
						VertexInformation[] candidates = GetVertexInformationsFromIntersection(sideIdx, vertexInfos);
						//VertexInformation result = pool.Select(p => candidates.FirstOrDefault(v => v.Vertex.DistanceSquaredTo(p) < 0.00000001f)).First();
						VertexInformation result = candidates.First(v = > pool.Contains(v.Vertex));
						return result;*/
			}
		}
		std::optional<std::pair<std::vector<glm::vec2>, std::vector<int>>> removeMe;
		return removeMe;
	}
}
