#include "markerLib/markerFinder.h"
#include <opencv2/opencv.hpp>
#include "markerLib/core/log.h"
#include "markerLib/core/log.h"


namespace MarkerLib {
	//implementation
	class MarkerFinder::MarkerFinderImpl
	{
	public:

		MarkerFinderImpl()
		{
		}

		int selectAndRefineCorners(cv::Mat img, std::vector<cv::Point2f>& corners) {

			assert((img.rows > 0) & (img.cols > 0) & (img.channels() > 0) && "Image failed: (stbImage.height > 0) & (stbImage.width > 0) & (stbImage.channels > 0)");


			int type = img.type(), depth = CV_MAT_DEPTH(type), cn = CV_MAT_CN(type);
			CV_CheckType(type, depth == CV_8U && (cn == 1 || cn == 3 || cn == 4),
				"Only 8-bit grayscale or color images are supported");


			if (corners.size() < 3)
				Log::error("minimal expected corners size >= 3");


			std::vector<cv::Rect> img_rois;
			//ToDo:: create as many colors as points
			cv::Scalar colors[]{ {125,125,0,1}, {125,0,125,1},{125,255,125,1},{255,125,125,1} };
			int scale = 2;
			cv::Mat resized;
			cv::resize(img, resized, cv::Size(0, 0), 1.0f / scale, 1.0f / scale);

			for (size_t i = 0; i < corners.size(); i++) {
				cv::Rect roi = cv::selectROI("select marker", resized, true, true);

				cv::drawMarker(resized, cv::Point(roi.x + roi.width / 2, roi.y + roi.height / 2), colors[i], cv::MARKER_TILTED_CROSS, 30, 3);
				//scale roi up again
				img_rois.push_back(cv::Rect(roi.x * scale, roi.y * scale, roi.width * scale, roi.height * scale));
			}
			cv::destroyWindow("select marker");

			cv::Mat img_gray;

			if (img.channels() != 1)
			{
				try {
					cvtColor(img, img_gray, cv::COLOR_BGR2GRAY);
				}
				catch (const cv::Exception& exc) {
					Log::error(exc.err);
					return EXIT_FAILURE;
				}
			}

			for (size_t i = 0; i < corners.size(); i++) {
				cv::Rect& cur_roi = img_rois[i];
				//out_corners.push_back(cv::Point2f(roi.width/2.0f,roi.height/2.0f));
				int winsize = cur_roi.width < cur_roi.height ? cur_roi.width : cur_roi.height;
				winsize = winsize / 2 - 6;
				cv::Mat blurred;
				cv::GaussianBlur(img_gray(cur_roi), blurred, cv::Size(5, 5), 0);
				//get centerpoint from roi as corner guess
				std::vector<cv::Point2f> corner{ { (float)(cur_roi.height / 2) , (float)(cur_roi.width / 2) } };
				cv::cornerSubPix(blurred, corner, cv::Size(winsize, winsize), cv::Size(winsize / 5, winsize / 5),
					cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 1000, 0.001));
				//assign refinement to output data
				corners[i].x = corner[0].x + cur_roi.x;
				corners[i].y = corner[0].y + cur_roi.y;

				cv::Point2f& pt = corners[i];

				const int r = 100;
				cv::line(img, cv::Point(pt.x - r, pt.y - r), cv::Point(pt.x + r, pt.y + r), colors[i], 3);
				cv::line(img, cv::Point(pt.x - r, pt.y + r), cv::Point(pt.x + r, pt.y - r), colors[i], 3);
				cv::circle(img, pt, r, colors[i], 3);
				cv::putText(img, cv::format("(%.1f , %.1f)", pt.x, pt.y), cv::Point(pt.x - 3.8f * r, pt.y - 1.2f * r), cv::FONT_HERSHEY_PLAIN, 5, { 0,0,0 }, 3);


			}
#ifdef _DEBUG
			if (img.rows > 1000) {
				cv::Mat clone(img.clone());
				cv::resize(clone, img, cv::Size(), 0.3f, 0.3f);
			}
			cv::imshow("subpixel output", img);
			cv::waitKey(0);
			cv::destroyWindow("subpixel output");

#endif

			return EXIT_SUCCESS;

		}
		//COMPUTE FAST HISTOGRAM GRADIENT
		template<typename ArrayContainer>
		void icvGradientOfHistogram256(const ArrayContainer& piHist, ArrayContainer& piHistGrad)
		{
			CV_DbgAssert(piHist.size() == 256);
			CV_DbgAssert(piHistGrad.size() == 256);
			piHistGrad[0] = 0;
			int prev_grad = 0;
			for (int i = 1; i < 255; ++i)
			{
				int grad = piHist[i - 1] - piHist[i + 1];
				if (std::abs(grad) < 100)
				{
					if (prev_grad == 0)
						grad = -100;
					else
						grad = prev_grad;
				}
				piHistGrad[i] = grad;
				prev_grad = grad;
			}
			piHistGrad[255] = 0;
		}

		//SMOOTH HISTOGRAM USING WINDOW OF SIZE 2*iWidth+1
		template<typename ArrayContainer>
		void icvSmoothHistogram256(const ArrayContainer& piHist, ArrayContainer& piHistSmooth, int iWidth)
		{
			CV_Assert(iWidth > 0);
			CV_DbgAssert(piHist.size() == 256);
			CV_DbgAssert(piHistSmooth.size() == 256);
			for (int i = 0; i < 256; ++i)
			{
				int iIdx_min = std::max(0, i - iWidth);
				int iIdx_max = std::min(255, i + iWidth);
				int iSmooth = 0;
				for (int iIdx = iIdx_min; iIdx <= iIdx_max; ++iIdx)
				{
					CV_DbgAssert(iIdx >= 0 && iIdx < 256);
					iSmooth += piHist[iIdx];
				}
				piHistSmooth[i] = iSmooth / (2 * iWidth + 1);
			}
		}

		template<typename ArrayContainer>
		void icvGetIntensityHistogram256(const cv::Mat& img, ArrayContainer& piHist)
		{
			for (int i = 0; i < 256; i++)
				piHist[i] = 0;
			// sum up all pixel in row direction and divide by number of columns
			for (int j = 0; j < img.rows; ++j)
			{
				const uchar* row = img.ptr<uchar>(j);
				for (int i = 0; i < img.cols; i++)
				{
					piHist[row[i]]++;
				}
			}
		}

		void icvBinarizationHistogramBased(cv::Mat& img)
		{
			assert((img.channels() == 1) & (img.depth() == CV_8U));
			int iCols = img.cols;
			int iRows = img.rows;
			int iMaxPix = iCols * iRows;
			int iMaxPix1 = iMaxPix / 100;
			const int iNumBins = 256;
			const int iMaxPos = 20;
			cv::AutoBuffer<int, 256> piHistIntensity(iNumBins);
			cv::AutoBuffer<int, 256> piHistSmooth(iNumBins);
			cv::AutoBuffer<int, 256> piHistGrad(iNumBins);
			cv::AutoBuffer<int> piMaxPos(iMaxPos);

			icvGetIntensityHistogram256(img, piHistIntensity);

#if 0
			// get accumulated sum starting from bright
			cv::AutoBuffer<int, 256> piAccumSum(iNumBins);
			piAccumSum[iNumBins - 1] = piHistIntensity[iNumBins - 1];
			for (int i = iNumBins - 2; i >= 0; --i)
			{
				piAccumSum[i] = piHistIntensity[i] + piAccumSum[i + 1];
			}
#endif

			// first smooth the distribution
			//const int iWidth = 1;
			icvSmoothHistogram256(piHistIntensity, piHistSmooth, 1);

			// compute gradient
			icvGradientOfHistogram256(piHistSmooth, piHistGrad);

			// check for zeros
			unsigned iCntMaxima = 0;
			for (int i = iNumBins - 2; (i > 2) && (iCntMaxima < iMaxPos); --i)
			{
				if ((piHistGrad[i - 1] < 0) && (piHistGrad[i] > 0))
				{
					int iSumAroundMax = piHistSmooth[i - 1] + piHistSmooth[i] + piHistSmooth[i + 1];
					if (!(iSumAroundMax < iMaxPix1 && i < 64))
					{
						piMaxPos[iCntMaxima++] = i;
					}
				}
			}

			Log::debug(std::format("HIST: MAXIMA COUNT: {0} ({1}, {2}, {3}, ...)", iCntMaxima,
				iCntMaxima > 0 ? piMaxPos[0] : -1,
				iCntMaxima > 1 ? piMaxPos[1] : -1,
				iCntMaxima > 2 ? piMaxPos[2] : -1));

			int iThresh = 0;

			CV_Assert((size_t)iCntMaxima <= piMaxPos.size());

			Log::debug(std::format("HIST: MAXIMA COUNT: {0} ({1}, {2}, {3}, ...)", iCntMaxima,
				iCntMaxima > 0 ? piMaxPos[0] : -1,
				iCntMaxima > 1 ? piMaxPos[1] : -1,
				iCntMaxima > 2 ? piMaxPos[2] : -1));

			if (iCntMaxima == 0)
			{
				// no any maxima inside (only 0 and 255 which are not counted above)
				// Does image black-write already?
				const int iMaxPix2 = iMaxPix / 2;
				for (int sum = 0, i = 0; i < 256; ++i) // select mean intensity
				{
					sum += piHistIntensity[i];
					if (sum > iMaxPix2)
					{
						iThresh = i;
						break;
					}
				}
			}
			else if (iCntMaxima == 1)
			{
				iThresh = piMaxPos[0] / 2;
			}
			else if (iCntMaxima == 2)
			{
				iThresh = (piMaxPos[0] + piMaxPos[1]) / 2;
			}
			else // iCntMaxima >= 3
			{
				// CHECKING THRESHOLD FOR WHITE
				int iIdxAccSum = 0, iAccum = 0;
				for (int i = iNumBins - 1; i > 0; --i)
				{
					iAccum += piHistIntensity[i];
					// iMaxPix/18 is about 5,5%, minimum required number of pixels required for white part of chessboard
					if (iAccum > (iMaxPix / 18))
					{
						iIdxAccSum = i;
						break;
					}
				}

				unsigned iIdxBGMax = 0;
				int iBrightMax = piMaxPos[0];
				// printf("iBrightMax = %d\n", iBrightMax);
				for (unsigned n = 0; n < iCntMaxima - 1; ++n)
				{
					iIdxBGMax = n + 1;
					if (piMaxPos[n] < iIdxAccSum)
					{
						break;
					}
					iBrightMax = piMaxPos[n];
				}

				// CHECKING THRESHOLD FOR BLACK
				int iMaxVal = piHistIntensity[piMaxPos[iIdxBGMax]];

				//IF TOO CLOSE TO 255, jump to next maximum
				if (piMaxPos[iIdxBGMax] >= 250 && iIdxBGMax + 1 < iCntMaxima)
				{
					iIdxBGMax++;
					iMaxVal = piHistIntensity[piMaxPos[iIdxBGMax]];
				}

				for (unsigned n = iIdxBGMax + 1; n < iCntMaxima; n++)
				{
					if (piHistIntensity[piMaxPos[n]] >= iMaxVal)
					{
						iMaxVal = piHistIntensity[piMaxPos[n]];
						iIdxBGMax = n;
					}
				}

				//SETTING THRESHOLD FOR BINARIZATION
				int iDist2 = (iBrightMax - piMaxPos[iIdxBGMax]) / 2;
				iThresh = iBrightMax - iDist2;
				Log::debug(std::format("THRESHOLD SELECTED = {0}, BRIGHTMAX = {0}, DARKMAX = {0}", iThresh, iBrightMax, piMaxPos[iIdxBGMax]));
			}

			if (iThresh > 0)
			{
				img = (img >= iThresh);
			}
		}


		// does a fast check if a chessboard is in the input image. This is a workaround to
	// a problem of cvFindChessboardCorners being slow on images with no chessboard
	// - src: input binary image
	// - size: chessboard size
	// Returns 1 if a chessboard can be in this image and findChessboardCorners should be called,
	// 0 if there is no chessboard, -1 in case of error

		int checkChessboardBinary(const cv::Mat* img, const cv::Size& size)
		{
			assert((img->channels() == 1) && (img->depth() == CV_8U));

			cv::Mat white = img->clone();
			cv::Mat black = img->clone();

			int result = 0;
			for (int erosion_count = 0; erosion_count <= 3; erosion_count++)
			{
				if (1 == result)
					break;

				if (0 != erosion_count) // first iteration keeps original images
				{
					erode(white, white, cv::Mat(), cv::Point(-1, -1), 1);
					dilate(black, black, cv::Mat(), cv::Point(-1, -1), 1);
					cv::imshow("white erode", white);
					cv::imshow("black dilate", black);
					cv::waitKey(0);
				}

				std::vector<std::pair<float, int> > quads;
				fillQuads(&white, &black, 128, 128, quads);
				if (checkQuads(quads, size))
					result = 1;
			}
			return result;
		}

		void fillQuads(cv::Mat* white, cv::Mat* black, double white_thresh, double black_thresh, std::vector<std::pair<float, int> >& quads)
		{
			cv::Mat thresh;
			{
				std::vector< std::vector<cv::Point> > contours;
				std::vector< cv::Vec4i > hierarchy;
				threshold(*white, thresh, white_thresh, 255, cv::THRESH_BINARY);
				findContours(thresh, contours, hierarchy, cv::RETR_CCOMP, cv::CHAIN_APPROX_SIMPLE);
				icvGetQuadrangleHypotheses(contours, hierarchy, quads, 1);
			}

			{
				std::vector< std::vector<cv::Point> > contours;
				std::vector< cv::Vec4i > hierarchy;
				threshold(*black, thresh, black_thresh, 255, cv::THRESH_BINARY_INV);
				findContours(thresh, contours, hierarchy, cv::RETR_CCOMP, cv::CHAIN_APPROX_SIMPLE);
				icvGetQuadrangleHypotheses(contours, hierarchy, quads, 0);
			}
		}

		void icvGetQuadrangleHypotheses(const std::vector<std::vector< cv::Point > >& contours, const std::vector< cv::Vec4i >& hierarchy, std::vector<std::pair<float, int> >& quads, int class_id)
		{
			const float min_aspect_ratio = 0.3f;
			const float max_aspect_ratio = 3.0f;
			const float min_box_size = 10.0f;

			typedef std::vector< std::vector< cv::Point > >::const_iterator iter_t;
			iter_t i;
			for (i = contours.begin(); i != contours.end(); ++i)
			{
				const iter_t::difference_type idx = i - contours.begin();
				if (hierarchy.at(idx)[3] != -1)
					continue; // skip holes

				const std::vector< cv::Point >& c = *i;
				cv::RotatedRect box = cv::minAreaRect(c);

				float box_size = MAX(box.size.width, box.size.height);
				if (box_size < min_box_size)
				{
					continue;
				}

				float aspect_ratio = box.size.width / MAX(box.size.height, 1);
				if (aspect_ratio < min_aspect_ratio || aspect_ratio > max_aspect_ratio)
				{
					continue;
				}

				quads.emplace_back(box_size, class_id);
			}
		}

		static void countClasses(const std::vector<std::pair<float, int> >& pairs, size_t idx1, size_t idx2, std::vector<int>& counts)
		{
			counts.assign(2, 0);
			for (size_t i = idx1; i != idx2; i++)
			{
				counts[pairs[i].second]++;
			}
		}

		static bool checkQuads(std::vector<std::pair<float, int> >& quads, const cv::Size& size)
		{
			const size_t min_quads_count = size.width * size.height / 2;
			std::sort(quads.begin(), quads.end(), [](const std::pair<float, int>& p1, const std::pair<float, int>& p2) {return p1.first < p2.first; });

			// now check if there are many hypotheses with similar sizes
			// do this by floodfill-style algorithm
			const float size_rel_dev = 0.4f;

			for (size_t i = 0; i < quads.size(); i++)
			{
				size_t j = i + 1;
				for (; j < quads.size(); j++)
				{
					if (quads[j].first / quads[i].first > 1.0f + size_rel_dev)
					{
						break;
					}
				}

				if (j + 1 > min_quads_count + i)
				{
					// check the number of black and white squares
					std::vector<int> counts;
					countClasses(quads, i, j, counts);
					const int black_count = cvRound(ceil(size.width / 2.0) * ceil(size.height / 2.0));
					const int white_count = cvRound(floor(size.width / 2.0) * floor(size.height / 2.0));
					if (counts[0] < black_count * 0.75 ||
						counts[1] < white_count * 0.75)
					{
						continue;
					}
					return true;
				}
			}
			return false;
		}
	};

	MarkerFinder::MarkerFinder()
		:m_markerFinder{ std::make_unique < MarkerFinderImpl>() }
	{
	}

	MarkerFinder::~MarkerFinder() = default;



	int MarkerFinder::selectAndRefineCorners(const BaseLib::STBimage& stbImage, float* pointBuf, uint32_t pointBufLen) {
		int chans = 0;

		if (stbImage.channels == 4)
		{
			chans = CV_8UC4;
		}
		else if (stbImage.channels == 3)
		{
			chans = CV_8UC3;
		}
		else if (stbImage.channels == 1)
		{
			chans = CV_8UC1;
		}
		cv::Mat img(stbImage.height, stbImage.width, chans, stbImage.data.get());

		uint32_t len = pointBufLen / 2 / sizeof(float);

		std::vector<cv::Point2f> corners(len);


		if (m_markerFinder->selectAndRefineCorners(img, corners)) {
			return EXIT_FAILURE;
		}

		if (corners.size() != len) {
			Log::error("extracted points size differs from pam pointBufLen");
		}

		for (size_t i = 0; i < corners.size(); i++) {
			pointBuf[i * 2 + 0] = corners[i].x;
			pointBuf[i * 2 + 1] = corners[i].y;
		}
		return EXIT_SUCCESS;

	}


}





