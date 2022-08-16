#include <string>
#include <chrono>

#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "avx256.h"
#include "demo.h"

// Add image2 to image1 using scalar addition, return fps performance metric
int addScalar(cv::Mat& image1, cv::Mat& image2)
{
	std::chrono::steady_clock::time_point start = std::chrono::high_resolution_clock::now();	

	uint64_t size = static_cast<uint64_t>(image1.rows) * image1.cols * image1.channels();

	for (uint64_t i = 0; i < size; ++i)
		image1.data[i] = static_cast<uint8_t>(image1.data[i] + image2.data[i]) >= image1.data[i] ? image1.data[i] + image2.data[i] : UINT8_MAX;

	std::chrono::steady_clock::time_point end = std::chrono::high_resolution_clock::now();

	return static_cast<int>(1 / std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1, 1>>>(end - start).count());
}

// Add image2 to image1 using AVX256 addition, return fps performance metric
int addAVX256(cv::Mat& image1, cv::Mat& image2)
{	
	std::chrono::steady_clock::time_point start = std::chrono::high_resolution_clock::now();

	AVX256<uint8_t> avxImage1{ image1.data }, avxImage2{ image2.data };

	uint64_t size = static_cast<uint64_t>(image1.rows) * image1.cols * image1.channels();
	uint64_t count = size / 32;
	int residualCount = size % 32;

	for (int i = 0; i < count; ++i, avxImage1.Next(), avxImage2.Next())
		avxImage1 += avxImage2;

	for (uint64_t i = size - residualCount; i < size; ++i)
		image1.data[i] = static_cast<uint8_t>(image1.data[i] + image2.data[i]) >= image1.data[i] ? image1.data[i] + image2.data[i] : UINT8_MAX;

	std::chrono::steady_clock::time_point end = std::chrono::high_resolution_clock::now();

	return static_cast<int>(1 / std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1, 1>>>(end - start).count());
}

// Add image2 to image1 using cv::add() (simd), return fps performance metric
int addOpenCVSIMD(cv::Mat& image1, cv::Mat& image2)
{
	std::chrono::steady_clock::time_point start = std::chrono::high_resolution_clock::now();

	cv::add(image1, image2, image1);

	std::chrono::steady_clock::time_point end = std::chrono::high_resolution_clock::now();

	return static_cast<int>(1 / std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1, 1>>>(end - start).count());
}

// Add frames of video2 to video1 using scalar addition, AVX256 addition, and cv::add() (with SIMD acceleration)
void addDemo(const std::string& video1Path, const std::string& video2Path)
{
	cv::Mat frame1, frame2, result;
	cv::Mat frame1Scalar, frame1AVX256, frame1OpenCVSIMD;
	cv::VideoCapture video1{ video1Path }, video2{ video2Path };

	int xmax = std::min(video1.get(cv::CAP_PROP_FRAME_COUNT), video2.get(cv::CAP_PROP_FRAME_COUNT)), ymax = 3000, avgRange = 15;
	cv::Mat plot = createFPSPlot(cv::Size(video1.get(cv::CAP_PROP_FRAME_WIDTH), video1.get(cv::CAP_PROP_FRAME_HEIGHT) / 2.5), std::pair<int, int>{xmax, ymax}, avgRange);

	std::vector<std::vector<int>> fpss(3, std::vector<int>(xmax));
	int frameCount = 0;

	while (true)
	{
		while (video1.read(frame1) && video2.read(frame2))
		{
			frame1Scalar = frame1.clone();
			fpss[0][frameCount] = addScalar(frame1Scalar, frame2);

			frame1AVX256 = frame1.clone();
			fpss[1][frameCount] = addAVX256(frame1AVX256, frame2);

			frame1OpenCVSIMD = frame1.clone();
			fpss[2][frameCount] = addOpenCVSIMD(frame1OpenCVSIMD, frame2);

			++frameCount;

			if (cv::sum(frame1Scalar != frame1AVX256) != cv::Scalar(0, 0, 0) || cv::sum(frame1Scalar != frame1OpenCVSIMD) != cv::Scalar(0, 0, 0)) return;
			plotFPS(plot, std::pair<int, int>{xmax, ymax}, frameCount, avgRange, fpss);
			writeFPS(frame1Scalar, frameCount, fpss);

			cv::vconcat(std::array<cv::Mat, 2>{frame1Scalar, plot}, result);
			cv::imshow("Output", result);

			if (cv::pollKey() != -1) { cv::destroyAllWindows(); return; }
		}

		frameCount = 0, video1.set(cv::CAP_PROP_POS_FRAMES, 0), video2.set(cv::CAP_PROP_POS_FRAMES, 0);
		plot = createFPSPlot(cv::Size(video1.get(cv::CAP_PROP_FRAME_WIDTH), video1.get(cv::CAP_PROP_FRAME_HEIGHT) / 2.5), std::pair<int, int>{xmax, ymax}, avgRange);
	}
}