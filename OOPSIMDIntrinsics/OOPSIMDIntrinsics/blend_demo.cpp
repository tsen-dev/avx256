#include <string>
#include <chrono>
#include <cmath>

#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "test.h"
#include "avx256.h"
#include "demo.h"

#ifndef TEST

// Blend image2 into image1 using scalar addition, return fps performance metric
int blendScalar(cv::Mat& image1, cv::Mat& image2)
{
	std::chrono::steady_clock::time_point start = std::chrono::high_resolution_clock::now();	

	uint64_t size = static_cast<uint64_t>(image1.rows) * image1.cols * image1.channels();

	for (uint64_t i = 0; i < size; ++i)
		image1.data[i] = (image1.data[i] + image2.data[i] + 1) >> 1;

	std::chrono::steady_clock::time_point end = std::chrono::high_resolution_clock::now();

	return static_cast<int>(1 / std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1, 1>>>(end - start).count());
}

// Blend image2 into image1 using AVX256 Average(), return fps performance metric
int blendAVX256(cv::Mat& image1, cv::Mat& image2)
{	
	std::chrono::steady_clock::time_point start = std::chrono::high_resolution_clock::now();

	AVX256<uint8_t> avxImage1{ image1.data }, avxImage2{ image2.data };

	uint64_t size = static_cast<uint64_t>(image1.rows) * image1.cols * image1.channels();
	uint64_t count = size / 32;
	int residualCount = size % 32;

	for (int i = 0; i < count; ++i, avxImage1.Next(), avxImage2.Next())
		avxImage1.Average(avxImage2);

	for (uint64_t i = size - residualCount; i < size; ++i)
		image1.data[i] = (image1.data[i] + image2.data[i] + 1) >> 1;

	std::chrono::steady_clock::time_point end = std::chrono::high_resolution_clock::now();

	return static_cast<int>(1 / std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1, 1>>>(end - start).count());
}

// Blend image2 into image1 using cv::addWeighted() (simd), return fps performance metric
int blendOpenCVSIMD(cv::Mat& image1, cv::Mat& image2)
{
	std::chrono::steady_clock::time_point start = std::chrono::high_resolution_clock::now();

	cv::addWeighted(image1, 0.5, image2, 0.5, 0, image1);

	std::chrono::steady_clock::time_point end = std::chrono::high_resolution_clock::now();

	return static_cast<int>(1 / std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1, 1>>>(end - start).count());
}

// Blend frames of video2 into video1 using scalar addition, AVX256 addition, and cv::add() (with SIMD acceleration)
void blendDemo(const std::string& video1Path, const std::string& video2Path)
{
	cv::Mat frame1, frame2, result;
	cv::Mat frame1Scalar, frame1AVX256, frame1OpenCVSIMD;
	cv::VideoCapture video1{ video1Path }, video2{ video2Path };

	int xmax = std::min(video1.get(cv::CAP_PROP_FRAME_COUNT), video2.get(cv::CAP_PROP_FRAME_COUNT)), ymax = 3000, avgRange = 15, size = video1.get(cv::CAP_PROP_FRAME_HEIGHT) * video1.get(cv::CAP_PROP_FRAME_WIDTH) * 3;
	cv::Mat plot = createFPSPlot(cv::Size(video1.get(cv::CAP_PROP_FRAME_WIDTH), video1.get(cv::CAP_PROP_FRAME_HEIGHT) / 2.5), std::pair<int, int>{xmax, ymax}, avgRange);

	std::vector<std::vector<int>> fpss(3, std::vector<int>(xmax));
	int frameCount = 0;

	while (true)
	{
		while (video1.read(frame1) && video2.read(frame2))
		{
			frame1Scalar = frame1.clone();
			fpss[0][frameCount] = blendScalar(frame1Scalar, frame2);

			frame1AVX256 = frame1.clone();
			fpss[1][frameCount] = blendAVX256(frame1AVX256, frame2);

			frame1OpenCVSIMD = frame1.clone();
			fpss[2][frameCount] = blendOpenCVSIMD(frame1OpenCVSIMD, frame2);

			++frameCount;			

			if (cv::sum(frame1Scalar != frame1AVX256) != cv::Scalar(0, 0, 0) || !std::equal(frame1Scalar.data, frame1Scalar.data + size, frame1OpenCVSIMD.data, [](const int& a, const int& b) {return std::abs(a - b) <= 1; })) return;
			plotFPS(plot, std::pair<int, int>{xmax, ymax}, frameCount, avgRange, fpss);
			writeFPS(frame1OpenCVSIMD, frameCount, fpss);

			cv::vconcat(std::array<cv::Mat, 2>{frame1OpenCVSIMD, plot}, result);
			cv::imshow("Output", result);

			if (cv::pollKey() != -1) { cv::destroyAllWindows(); return; }
		}

		frameCount = 0, video1.set(cv::CAP_PROP_POS_FRAMES, 0), video2.set(cv::CAP_PROP_POS_FRAMES, 0);
		plot = createFPSPlot(cv::Size(video1.get(cv::CAP_PROP_FRAME_WIDTH), video1.get(cv::CAP_PROP_FRAME_HEIGHT) / 2.5), std::pair<int, int>{xmax, ymax}, avgRange);
	}
}

#endif