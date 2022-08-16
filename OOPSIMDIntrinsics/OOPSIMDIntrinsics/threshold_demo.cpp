#include <string>
#include <chrono>
#include <array>

#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "avx256.h"
#include "demo.h"

// Convert the image to a binary image using the specified boundary with cv::threshold() (simd)
int thresholdOpenCVSIMD(cv::Mat& image, uint8_t boundary)
{
	std::chrono::steady_clock::time_point start = std::chrono::high_resolution_clock::now();

	cv::threshold(image, image, boundary, UINT8_MAX, cv::THRESH_BINARY);

	std::chrono::steady_clock::time_point end = std::chrono::high_resolution_clock::now();

	return static_cast<int>(1 / std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1, 1>>>(end - start).count());
}

// Convert the image to a binary image using the specified boundary with AVX256 operations
int thresholdAVX256(cv::Mat& image, uint8_t boundary)
{
	std::chrono::steady_clock::time_point start = std::chrono::high_resolution_clock::now();

	AVX256<uint8_t> avxImage{ image.data };
	AVX256<uint8_t> avxBoundary{}; 
	avxBoundary = boundary;

	uint64_t size = static_cast<uint64_t>(image.rows) * image.cols * image.channels();
	uint64_t count = size / 32;
	int residualCount = size % 32;

	for (int i = 0; i < count; ++i, avxImage.Next())
		avxImage = avxImage > avxBoundary;

	for (uint64_t i = size - residualCount; i < size; ++i)
		image.data[i] = (image.data[i] > boundary) * UINT8_MAX;

	std::chrono::steady_clock::time_point end = std::chrono::high_resolution_clock::now();

	return static_cast<int>(1 / std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1, 1>>>(end - start).count());
}

// Convert the image to a binary image using the specified boundary with scalar operations
int thresholdScalar(cv::Mat& image, uint8_t boundary)
{
	std::chrono::steady_clock::time_point start = std::chrono::high_resolution_clock::now();

	uint64_t size = static_cast<uint64_t>(image.rows) * image.cols * image.channels();

	for (uint64_t i = 0; i < size; ++i)
		image.data[i] = (image.data[i] > boundary) * UINT8_MAX;

	std::chrono::steady_clock::time_point end = std::chrono::high_resolution_clock::now();

	return static_cast<int>(1 / std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1, 1>>>(end - start).count());
}

// Convert the frames of the video into a binary image using scalar, AVX256, and cv::threshold() (with SIMD acceleration). The threshold boundary is configurable by a trackbar element
void thresholdDemo(const std::string& videoPath)
{
	cv::Mat frame, result;
	cv::Mat frameScalar, frameAVX256, frameOpenCVSIMD;
	cv::VideoCapture video{ videoPath };

	int xmax = video.get(cv::CAP_PROP_FRAME_COUNT), ymax = 20000, avgRange = 15;
	cv::Mat plot = createFPSPlot(cv::Size(video.get(cv::CAP_PROP_FRAME_WIDTH), video.get(cv::CAP_PROP_FRAME_HEIGHT) / 3), std::pair<int, int>{xmax, ymax}, avgRange);

	uint8_t boundary = 0;
	cv::namedWindow("Output", cv::WINDOW_AUTOSIZE);
	cv::createTrackbar("Threshold", "Output", nullptr, 255, [](int barPos, void* boundary) {*static_cast<int*>(boundary) = barPos; }, &boundary);	

	std::vector<std::vector<int>> fpss(3, std::vector<int>(xmax));
	int frameCount = 0;

	while (true)
	{
		while (video.read(frame))
		{
			cv::cvtColor(frame, frame, cv::COLOR_BGR2GRAY);

			frameScalar = frame.clone();
			fpss[0][frameCount] = thresholdScalar(frameScalar, boundary);

			frameAVX256 = frame.clone();
			fpss[1][frameCount] = thresholdAVX256(frameAVX256, boundary);

			frameOpenCVSIMD = frame.clone();
			fpss[2][frameCount] = thresholdOpenCVSIMD(frameOpenCVSIMD, boundary);

			++frameCount;

			if (cv::sum(frameScalar != frameAVX256) != cv::Scalar(0) || cv::sum(frameScalar != frameOpenCVSIMD) != cv::Scalar(0)) return;
			cv::cvtColor(frameScalar, frameScalar, cv::COLOR_GRAY2BGR);
			plotFPS(plot, std::pair<int, int>{xmax, ymax}, frameCount, avgRange, fpss);
			writeFPS(frameScalar, frameCount, fpss);

			cv::vconcat(std::array<cv::Mat, 2>{frameScalar, plot}, result);
			cv::imshow("Output", result);

			if (cv::pollKey() != -1) { cv::destroyAllWindows(); return; }
		}

		frameCount = 0, video.set(cv::CAP_PROP_POS_FRAMES, 0);
		plot = createFPSPlot(cv::Size(video.get(cv::CAP_PROP_FRAME_WIDTH), video.get(cv::CAP_PROP_FRAME_HEIGHT) / 3), std::pair<int, int>{xmax, ymax}, avgRange);
	}
}