constant sampler_t sampler =
CLK_NORMALIZED_COORDS_FALSE
| CLK_ADDRESS_CLAMP_TO_EDGE
| CLK_FILTER_LINEAR;


float2 mult(global float* matrix, float x, float y) {
	float resultX = matrix[0] * x + matrix[1] * y + matrix[2];
	float resultY = matrix[3] * x + matrix[4] * y + matrix[5];
	float2 result = { resultX, resultY };
	return result;
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

// http://stackoverflow.com/questions/12117825/how-can-i-undistort-an-image-in-matlab-using-the-known-camera-parameters
// Images can not have the "global" attribute
kernel void undistortImage(__read_only image2d_t inputImg,
						   __write_only image2d_t outputImg,
						   global float* intrinsic,
						   global float* invIntrinsic,
						   global float* distortionCoefficients) {

	float k1 = distortionCoefficients[0];
	float k2 = distortionCoefficients[1];
	float p1 = distortionCoefficients[2];
	float p2 = distortionCoefficients[3];
	float k3 = distortionCoefficients[4];
	
	int i = get_global_id(0);
	int j = get_global_id(1);
	int2 point = { i, j };
	float2 pointF = { i, j };

	
	// TODO: Pass these as parameter?
	int width = get_image_width(inputImg);
	int height = get_image_height(inputImg);
	
	float2 zPlanePoint = mult(invIntrinsic, pointF.x, pointF.y);
	float x = zPlanePoint.x;
	float y = zPlanePoint.y;
	
	float r2 = x * x + y * y;
	float r4 = r2 * r2;
	float r6 = r2 * r2 * r2;
	float distortedX = x * (1 + k1 * r2 + k2 * r4 + k3 * r6) + 2 * p1 * x * y + p2 * (r2 + 2 * x * x);
	float distortedY = y * (1 + k1 * r2 + k2 * r4 + k3 * r6) + 2 * p2 * x * y + p1 * (r2 + 2 * y * y);

	float2 imagePlanePoint = mult(intrinsic, distortedX, distortedY);
	
	
	if (imagePlanePoint.x >= 0 && imagePlanePoint.x < width && imagePlanePoint.y >= 0 && imagePlanePoint.y < height) {
		printf(" i: %f j: %f  imageplanepoint x:y %f:%f\n",pointF.x,pointF.y, imagePlanePoint.x,imagePlanePoint.y);
		write_imagef(outputImg,point , read_imagef(inputImg, sampler,imagePlanePoint )); 
	}
}

kernel void cropImage(image2d_t inputImg,
					  __write_only image2d_t outputImg,
					  global float* polygon,
					  global int* defaultColor,
					  int numberOfPoints) {
	
	int x = get_global_id(0);
	int y = get_global_id(1);
	
	int2 point = { x, y };
	float2 pointf = { x, y };
	
	// For testing
	//int4 color = { defaultColor[2], defaultColor[1], defaultColor[0], 255 };
	//write_imagei(outputImg, point, color);
	
	if (isPointInsidePolygon(polygon, numberOfPoints, pointf)) {
		write_imagei(outputImg, point, read_imagei(inputImg, sampler, point));
	}
	else {
		int4 color = { defaultColor[2], defaultColor[1], defaultColor[0], 255 };
		write_imagei(outputImg, point, color);
	}
}
