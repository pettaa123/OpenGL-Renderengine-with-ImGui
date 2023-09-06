// IMPORTANT!!!!
// Never intialize a vector like this: "float2 a = (x, y)".
// It will compile but it is extremly slow and may lead to errors.
// Use "float2 a = (float2)(x, y)" or "float2 a = { x, y }" instead.

float2 project3Dto2D(global float* projectionMatrix, float3 worldPoint) {
	float x = projectionMatrix[0] * worldPoint.x + projectionMatrix[1] * worldPoint.y + projectionMatrix[2] * worldPoint.z + projectionMatrix[3];
	float y = projectionMatrix[4] * worldPoint.x + projectionMatrix[5] * worldPoint.y + projectionMatrix[6] * worldPoint.z + projectionMatrix[7];
	float w = projectionMatrix[8] * worldPoint.x + projectionMatrix[9] * worldPoint.y + projectionMatrix[10] * worldPoint.z + projectionMatrix[11];
	x = x / w;
	y = y / w;
	float2 imagePoint = { x, y };
	return imagePoint;
}

bool isFloatZero(float value) {
	return fabs(value) < 1e-10;
}

// Determines if the lines AB and CD intersect.
bool isLineIntersectingLine(float2 a, float2 b, float2 c, float2 d) {
	float2 cmp = { c.x - a.x, c.y - a.y };
	float2 r = { b.x - a.x, b.y - a.y };

	float cmPxr = cmp.x * r.y - cmp.y * r.x;
	
	if (isFloatZero(cmPxr)) {
		// Lines are collinear, and so intersect if they have any overlap
		return ((c.x - a.x < 0) != (c.x - b.x < 0))
			|| ((c.y - a.y < 0) != (c.y - b.y < 0));
	}
	
	float2 s = { d.x - c.x, d.y - c.y };
	float rxs = r.x * s.y - r.y * s.x;
	
	if (isFloatZero(rxs)) {
		return false; // Lines are parallel.
	}
	
	float cmPxs = cmp.x * s.y - cmp.y * s.x;
	float rxsr = 1 / rxs;
	float t = cmPxs * rxsr;
	float u = cmPxr * rxsr;

	if (t >= 0 && t <= 1 && u >= 0 && u <= 1) {
		return true;
	}
	
	return false;
}


// The last polygon point is the same as the first one again
bool isTriangleIntersectingPolygon(global float* polygonPoints, int numberOfPoints, float2 pixelCoords[]) {
	float2 point1 = { polygonPoints[0], polygonPoints[1] };
	
	for (int i = 0; i < numberOfPoints - 1; i++) {
		int idx = 2 * (i + 1);
		float2 point2 = { polygonPoints[idx], polygonPoints[idx + 1] };
		
		bool isIntersecting = isLineIntersectingLine(point1, point2, pixelCoords[0], pixelCoords[1]);
		if (isIntersecting) {
			return true;
		}
		else {
			isIntersecting = isLineIntersectingLine(point1, point2, pixelCoords[1], pixelCoords[2]);
			if (isIntersecting) {
				return true;
			}
			else {
				isIntersecting = isLineIntersectingLine(point1, point2, pixelCoords[2], pixelCoords[0]);
				if (isIntersecting) {
					return true;
				}
			}
		}
		
		point1 = point2;
	}
	
	return false;
}

int crossProductTest(float2 a, float2 b, float2 c) {
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
		float2 temp = c;
		c = b;
		b = temp;
	}

	if (a.x == b.x && a.y == b.y) {
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

bool isPointInsidePolygon(global float* polygonPoints, int numberOfPoints, float2 testPoint) {
	int t = -1;
	float2 point1 = { polygonPoints[0], polygonPoints[1] };
	for (int i = 0; i < numberOfPoints - 1; i++) {
		int idx = 2 * (i + 1);
		float2 point2 = { polygonPoints[idx], polygonPoints[idx + 1] };
		t = t * crossProductTest(testPoint, point1, point2);
		point1 = point2;
	}

	// -1: Outside, 0: On polygon, 1: Inside
	bool isInside = t >= 0;
	return isInside;
}

kernel void project(
	global float* vertices,
	global float* projectionMatrix,
	global float* polygonPoints,
	global float* modelPoints,
	global float* textureCoordinates,
	//global float* TCP,
	int numberOfPoints,
	int numberOfModelPoints,
	int imageWidth,
	int imageHeight)
	//int IsDLT) 
	{
	//global float* pixelMinCriterion,
	//global float* pixelMaxCriterion,

	size_t triangleId = get_global_id(0);
	size_t vertexId = triangleId * 9;
	float3 triangleVertices[3] = { { vertices[vertexId], vertices[vertexId + 1], vertices[vertexId + 2] },
								   { vertices[vertexId + 3], vertices[vertexId + 4], vertices[vertexId + 5] },
								   { vertices[vertexId + 6], vertices[vertexId + 7], vertices[vertexId + 8] } };
								   						
	
	bool isTriangleValid = false;
	
	//printf(" proj: %f \n",projectionMatrix[0]);
	//printf(" pp: %f \n",polygonPoints[0]);
	//printf(" mp: %f \n",modelPoints[0]);	
	//printf(" tc: %f \n",textureCoordinates[0]);	
	//
	//printf(" imageWidth: %d \n",imageWidth);
	//printf(" imageHeight: %d \n",imageHeight);
	//printf(" numberOfPoints: %zu \n",numberOfPoints);	
	//printf(" numberOfModelPoints: %zu \n",numberOfModelPoints);	
	
	for (int i = 0; i < numberOfModelPoints; i++) {
		int idx = i * 3;
		float3 modelPoint = { modelPoints[idx], modelPoints[idx + 1], modelPoints[idx + 2] };
		//float3 TCPs = { TCP[0], TCP[1], TCP[2] };
		// Only test one of the triangle vertices, the triangles should be very small compared to the measure fields
	
		
		if (fast_length(triangleVertices[0] - modelPoint) < 500) {
			
				isTriangleValid = true;
			
			
			break;
		}
	}
	
	if (!isTriangleValid) {
		return;
	}
	
	isTriangleValid = false;
	float2 pixelCoords[ 3];
	for (int i = 0; i < 3; i++) {
		float2 pixelCoord = project3Dto2D(projectionMatrix, triangleVertices[i]);
		
		// Imaging the projecting frustum of the projection matrix, inputs from vertices that are very far away from it
		// will result in pixel coordinates in the order of hundreds of thousands of pixels. Those huge lines will cause
		// errors in the intersection algorithm. Thus, such projected pixels will be filtered out.
		//if (pixelCoord.x < pixelMinCriterion[0] || pixelCoord.y < pixelMinCriterion[1] || pixelCoord.x > pixelMaxCriterion[0] || pixelCoord.y > pixelMaxCriterion[1]) {
		//	return;
		//}
		
		if (!isTriangleValid) {
			isTriangleValid = isTriangleValid || isPointInsidePolygon(polygonPoints, numberOfPoints, pixelCoord);
		}
		
		pixelCoords[i] = pixelCoord;
	}
	
	// Don't only test if one of the pixelCoords is inside the polygon.
	// At the corners, sometimes there are triangles that intersect the polygon, but none of its vertices are inside it.
	if (!isTriangleValid) {
		isTriangleValid = isTriangleIntersectingPolygon(polygonPoints, numberOfPoints, pixelCoords);
	}
	
	if (isTriangleValid) {
		size_t texStartIdx = triangleId * 6;
		if (textureCoordinates[texStartIdx] == -1000) {
			for (int i = 0; i < 3; i++) {
				size_t idx = texStartIdx + 2 * i;
				float uvX = pixelCoords[i].x / imageWidth;
				float uvY = pixelCoords[i].y / imageHeight;
				textureCoordinates[idx] = uvX;
				textureCoordinates[idx + 1] = uvY;
			}
		}
	}
}

