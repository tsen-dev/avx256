#include <string>
#include <chrono>

#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "test.h"
#include "avx256.h"
#include "demo.h"

// Blend image 2 into image1 using the specified alpha with scalar operations, return fps performance metric
int blendScalar(cv::Mat& image1, cv::Mat& image2, float alpha)
{
	std::chrono::steady_clock::time_point start = std::chrono::high_resolution_clock::now();	

	int64_t size = static_cast<uint64_t>(image1.rows) * image1.cols * image1.channels();
	float invAlpha = 1 - alpha;

	float* image1Data = reinterpret_cast<float*>(image1.data);
	float* image2Data = reinterpret_cast<float*>(image2.data);

	for (int64_t i = 0; i < size; ++i)
		image1Data[i] = image1Data[i] * alpha + image2Data[i] * invAlpha;

	std::chrono::steady_clock::time_point end = std::chrono::high_resolution_clock::now();

	return static_cast<int>(1 / std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1, 1>>>(end - start).count());
}

// Blend image 2 into image1 using the specified alpha with AVX256 operations, return fps performance metric
int blendAVX256(cv::Mat& image1, cv::Mat& image2, float alpha)
{	
	std::chrono::steady_clock::time_point start = std::chrono::high_resolution_clock::now();

	AVX256<float> avxImage1{ reinterpret_cast<float*>(image1.data) }, avxImage2{ reinterpret_cast<float*>(image2.data) };
	AVX256<float> avxAlpha{}, avxInvAlpha{};
	avxAlpha = alpha, avxInvAlpha = 1 - alpha;

	int64_t size = static_cast<uint64_t>(image1.rows) * image1.cols * image1.channels();
	int64_t count = size / 8;
	int residualCount = size % 8;

	for (int i = 0; i < count; ++i, avxImage1.Next(), avxImage2.Next())
	{
		avxImage1 += avxImage1;
	}

	/*for (int64_t i = size - residualCount; i < size; ++i)
		image1.data[i] = static_cast<uint8_t>(image1.data[i] + image2.data[i]) >= image1.data[i] ? image1.data[i] + image2.data[i] : UINT8_MAX;*/

	std::chrono::steady_clock::time_point end = std::chrono::high_resolution_clock::now();

	return static_cast<int>(1 / std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1, 1>>>(end - start).count());
}

// Blend image 2 into image1 using the specified alpha with openCV SIMD operations, return fps performance metric
int blendOpenCVSIMD(cv::Mat& image1, cv::Mat& image2, float alpha)
{
	std::chrono::steady_clock::time_point start = std::chrono::high_resolution_clock::now();

	cv::addWeighted(image1, alpha, image2, 1 - alpha, 0, image1);

	std::chrono::steady_clock::time_point end = std::chrono::high_resolution_clock::now();

	return static_cast<int>(1 / std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1, 1>>>(end - start).count());
}

// Blend frames of video2 into video1 using scalar, AVX256, and openCV SIMD operations. The alpha is configurable by a trackbar element
void blendDemo(const std::string& video1Path, const std::string& video2Path)
{
	cv::Mat frame1, frame2, result;
	cv::Mat frame1Scalar, frame1AVX256, frame1OpenCVSIMD;
	cv::VideoCapture video1{ video1Path }, video2{ video2Path };

	int xmax = std::min(video1.get(cv::CAP_PROP_FRAME_COUNT), video2.get(cv::CAP_PROP_FRAME_COUNT)), ymax = 1000, avgRange = 15;
	cv::Mat plot = createFPSPlot(cv::Size(video1.get(cv::CAP_PROP_FRAME_WIDTH), video1.get(cv::CAP_PROP_FRAME_HEIGHT) / 3), std::pair<int, int>{xmax, ymax}, avgRange);

	int alpha = 0;
	cv::namedWindow("Output", cv::WINDOW_AUTOSIZE);
	cv::createTrackbar("Alpha (%)", "Output", nullptr, 100, [](int barPos, void* alpha) {*static_cast<int*>(alpha) = barPos; }, &alpha);

	std::vector<std::vector<int>> fpss(3, std::vector<int>(xmax));
	int frameCount = 0;

	while (true)
	{
		while (video1.read(frame1) && video2.read(frame2))
		{
			frame1.convertTo(frame1, CV_32FC3, 1.0 / 255), frame2.convertTo(frame2, CV_32FC3, 1.0 / 255);

			frame1Scalar = frame1.clone();
			fpss[0][frameCount] = blendScalar(frame1Scalar, frame2, alpha / 100.0);

			frame1AVX256 = frame1.clone();
			fpss[1][frameCount] = blendAVX256(frame1AVX256, frame2, alpha / 100.0);

			frame1OpenCVSIMD = frame1.clone();
			fpss[2][frameCount] = blendOpenCVSIMD(frame1OpenCVSIMD, frame2, alpha / 100.0);

			frame1AVX256.convertTo(frame1AVX256, CV_8UC3, 255);

			++frameCount;

			//if (cv::sum(frame1Scalar != frame1AVX256) != cv::Scalar(0, 0, 0) || cv::sum(frame1Scalar != frame1OpenCVSIMD) != cv::Scalar(0, 0, 0)) break;
			plotFPS(plot, std::pair<int, int>{xmax, ymax}, frameCount, avgRange, fpss);
			writeFPS(frame1AVX256, frameCount, fpss);

			cv::vconcat(std::array<cv::Mat, 2>{frame1AVX256, plot}, result);
			cv::imshow("Output", result);

			if (cv::pollKey() != -1) { cv::destroyAllWindows(); return; }
		}

		frameCount = 0, video1.set(cv::CAP_PROP_POS_FRAMES, 0), video2.set(cv::CAP_PROP_POS_FRAMES, 0);
		plot = createFPSPlot(cv::Size(video1.get(cv::CAP_PROP_FRAME_WIDTH), video1.get(cv::CAP_PROP_FRAME_HEIGHT) / 3), std::pair<int, int>{xmax, ymax}, avgRange);
	}

	
}