#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <algorithm>

namespace TextureMapping {

    // Various functions for polygon mathematics.

    class PolygonHelper {
        public:

        /// <summary>
        /// Determines if a point is inside a polygon.
        /// </summary>
        /// <param name="polygonPoints">The polygon points.</param>
        /// <param name="testPointX">The test point x.</param>
        /// <param name="testPointY">The test point y.</param>
        /// <returns>
        ///   <c>true</c> if the point is inside the polygon.
        /// </returns>
            static bool isPointInsidePolygon(const std::vector<glm::vec2>& polygonPoints,const float testPointX,const float testPointY) {
                return isPointInsidePolygon(polygonPoints, glm::vec2(testPointX, testPointY));
        }

        /// <summary>
        /// Determines if a point is inside a polygon.
        /// See https://de.wikipedia.org/wiki/Punkt-in-Polygon-Test_nach_Jordan
        /// </summary>
        /// <param name="polygonPoints">The polygon points.</param>
        /// <param name="testPoint">The test point.</param>
        /// <returns>
        ///   <c>true</c> if the point is inside the polygon.
        /// </returns>
            static bool isPointInsidePolygon(const std::vector<glm::vec2>& polygonPoints,const glm::vec2& testPoint) {
            float t = -1;
            for (int i = 0; i < polygonPoints.size() - 1; i++) {
                t = t * crossProductTest(testPoint, polygonPoints[i], polygonPoints[i + 1]);
            }

            // -1: Outside, 0: On polygon, 1: Inside
            bool isInside = t >= 0;
            return isInside;
        }


        // Cross product test for a point and a side of a polygon.
        // <param name="a">The point to test.</param>
        // <param name="b">First point of the polygon side.</param>
        // <param name="c">second point of the polygon side.</param>
        static float crossProductTest(glm::vec2 a, glm::vec2 b, glm::vec2 c) {
            // TODO: Implement an equality test for floats?
            if ((a.y == b.y) && (a.y == c.y)) {
                if ((b.x <= a.x && a.x <= c.x) || (c.x <= a.x && a.x <= b.x)) {
                    return 0;
                }
                else {
                    return 1;
                }
            }

            if (b.y > c.y) {
                glm::vec2 temp = c;
                c = b;
                b = temp;
            }

            if (a == b) {
                return 0;
            }

            if (a.y <= b.y || a.y > c.y) {
                return 1;
            }

            float delta = (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
            if (delta > 0) {
                return -1;
            }
            else if (delta < 0) {
                return 1;
            }

            return 0;
        }


        /// Stretches or shrinkens a polygon.
        /// <param name="polygon">The polygon.</param>
        /// <param name="offsetInPixels">The offset in pixels.</param>
        /// <param name="insertFirstPointAtEnd">if set to <c>true</c> [insert first point at end].</param>
        /// <returns></returns>
        static std::vector<glm::vec2> performPolygonOffsetting(std::vector<glm::vec2> polygon, double offsetInPixels, bool insertFirstPointAtEnd = false) {
            std::vector<glm::ivec3> path;
            for(glm::vec2 point : polygon) {
                glm::ivec3 intPoint((int)point.x, (int)point.y, 0);
                path.push_back(intPoint);
            }

            std::vector<std::vector<glm::ivec3>> resultList;
            //ClipperOffset clipper = new ClipperOffset();
            //clipper.AddPath(path, JoinType.jtMiter, EndType.etClosedPolygon);
            //clipper.Execute(ref resultList, offsetInPixels);
            std::vector<glm::ivec3> result = resultList[0];

            std::vector<glm::vec2> offsetPolygon;
            for(glm::ivec3 point : result) {
                offsetPolygon.push_back(glm::vec2(point.x, point.y));
            }
            if (insertFirstPointAtEnd) {
                offsetPolygon.push_back(offsetPolygon[0]);
            }

            return offsetPolygon;
        }

        /// <summary>
        /// Gets the convex hull of a point set.
        /// </summary>
        /// <param name="points">The points.</param>
        /// <param name="insertFirstPointAtEnd">if set to <c>true</c> [insert first point at end].</param>
        /// <returns></returns>
        static std::vector<glm::vec2> getConvexHull(std::vector<glm::vec2> points, bool insertFirstPointAtEnd = false) {
            //Stack<Vector2> stack = new Stack<Vector2>();
            //Vector2[] sortedPoints = SortPointsRadiallyCCW(points);
            //
            //Vector2 p0 = sortedPoints[0];
            //stack.Push(p0);
            //stack.Push(sortedPoints[1]);
            //
            //int i = 2;
            //while (i < sortedPoints.Length) {
            //    Vector2 p = sortedPoints[i];
            //    Vector2 p1 = stack.Peek();
            //    // Get the second top element
            //    stack.Pop();
            //    Vector2 p2 = stack.Peek();
            //    stack.Push(p1);
            //
            //    if (isP2LeftComparator(p2, p1, p) < 0) {
            //        stack.Push(p);
            //        i++;
            //    }
            //    else {
            //        stack.Pop();
            //    }
            //}
            //
            //if (insertFirstPointAtEnd) {
            //    stack.Push(p0);
            //}
            return std::vector<glm::vec2>();//stack.ToArray();
        }

        /// <summary>
        /// Sorts the points radially counter-clockwise.
        /// </summary>
        /// <param name="points">The points.</param>
        /// <returns></returns>
        static std::vector<glm::vec2> sortPointsRadiallyCCW(std::vector<glm::vec2> points) {
            /*  Example pattern for sorted points:
                1 4 7
                2 5 8
                3 6 9
            */
            std::vector<glm::vec2> positionSortedPoints(points);
            // Pivot is the right bottommost point
            std::sort(positionSortedPoints.begin(), positionSortedPoints.end(), [](const glm::vec2& a, const glm::vec2& b)
                {
                    if (a.x < b.x) return true;
                    if (b.x < a.x) return false;

                    // a=b for primary condition, go to secondary
                    if (a.y < b.y) return true;
                    if (b.y < a.y) return false;

                    return false; ////CHECK THIS HAS CORRECT OUTPUT
                });

            glm::vec2 pivot = positionSortedPoints.back();

            //std::vector<glm::vec2> radialSortedPoints = positionSortedPoints.Take(positionSortedPoints.Length - 1).ToList();
            //radialSortedPoints.Sort((p1, p2) = > isP2LeftComparator(pivot, p1, p2));
            //radialSortedPoints.Insert(0, pivot);

            return std::vector<glm::vec2>();//radialSortedPoints.ToArray();
        }

        /// <summary>
        /// Determines if P2 is left of pivot and P1.
        /// </summary>
        /// <param name="pivot">The pivot.</param>
        /// <param name="p1">The p1.</param>
        /// <param name="p2">The p2.</param>
        /// <returns></returns>
        static int isP2LeftComparator(glm::vec2 pivot, glm::vec2 p1, glm::vec2 p2) {
            int result = (int)((p1.x - pivot.x) * (p2.y - pivot.y) - (p2.y - pivot.y) * (p1.y - pivot.y));
            return result;
            /*
            if (result == 0) {
                // In this case, p1 and p2 are on a line towards pivot and the one that is the nearest one to pivot can be discarded.
                // However, as this method is called from inside a sorting algorithm, it is a bit difficult to discard results here.
                // Just keep all the points, the total set of points is rather small anyways.
            }
            */
        }

        /*
        private static Vector2? GetRightUpperMostPoint(Vector2[] sortedPoints) {
            float y = sortedPoints[sortedPoints.Length - 1].Y;
            for (int i = sortedPoints.Length - 2; i >= 0; i--) {
                if (y < sortedPoints[i].Y) {
                    return sortedPoints[i + 1];
                }
                y = sortedPoints[i].Y;
            }
            return null;
        }

        private static Vector2? GetLeftBottomMostPoint(Vector2[] sortedPoints) {
            float y = sortedPoints[0].Y;
            for (int i = 0; i < sortedPoints.Length; i++) {
                if (y > sortedPoints[i].Y) {
                    return sortedPoints[i - 1];
                }
                y = sortedPoints[i].Y;
            }
            return null;
        }
        */
    };
}
