#include <string>
#include <chrono>

#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "test.h"
#include "avx256.h"
#include "demo.h"

#ifndef TEST

// Convert a BGR video to RGB using scalar operations, return fps performance metric
int bgrToRGBScalar(cv::Mat& image)
{
	std::chrono::steady_clock::time_point start = std::chrono::high_resolution_clock::now();

	uint64_t size = static_cast<uint64_t>(image.rows) * image.cols * image.channels();

	for (int i = 0; i < size; i += 3)
		std::swap(image.data[i], image.data[i + 2]);

	std::chrono::steady_clock::time_point end = std::chrono::high_resolution_clock::now();

	return static_cast<int>(1 / std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1, 1>>>(end - start).count());
}

// Convert a BGR video to RGB using AVX256, return fps performance metric
int bgrToRGBAVX256(cv::Mat& image)
{
	std::chrono::steady_clock::time_point start = std::chrono::high_resolution_clock::now();

	/*
	*                 order1                               order2                                   order3
	bgrbgrbgrbgrbgr(b|gr)bgrbgrbgrbgr(bg  ||  r)bgrbgrbgrbgrbgr|bgrbgrbgrbgrbgr(b  ||  gr)bgrbgrbgrbgr(bg|r)bgrbgrbgrbgrbgr  || ...
	*/

	AVX256<uint8_t> avxImage{ image.data };
	std::array<uint8_t, 32>
		order1{ {
			2, 1, 0, 5, 4, 3, 8, 7, 6, 11, 10, 9, 14, 13, 12, 15,
			0, 1, 4, 3, 2, 7, 6, 5, 10, 9, 8, 13, 12, 11, 14, 15
		} },
		order2{ {
			0, 3, 2, 1, 6, 5, 4, 9, 8, 7, 12, 11, 10, 15, 14, 13,
			2, 1, 0, 5, 4, 3, 8, 7, 6, 11, 10, 9, 14, 13, 12, 15
		} },
		order3{ {
			0, 1, 4, 3, 2, 7, 6, 5, 10, 9, 8, 13, 12, 11, 14, 15,
			0, 3, 2, 1, 6, 5, 4, 9, 8, 7, 12, 11, 10, 15, 14, 13
			} };


	uint64_t size = static_cast<uint64_t>(image.rows) * image.cols * image.channels();
	uint64_t count = size / (32 * 3);
	int residualCount = size % (32 * 3);

	for (int i = 0; i < count; ++i, avxImage.Next())
	{
		avxImage.Permute8(order1);
		std::swap(avxImage.Data[15], avxImage.Data[17]);
		std::swap(avxImage.Data[30], avxImage.Data[32]);
		avxImage.Next();
		avxImage.Permute8(order2);
		std::swap(avxImage.Data[31], avxImage.Data[33]);
		avxImage.Next();
		avxImage.Permute8(order3);
		std::swap(avxImage.Data[14], avxImage.Data[16]);
	}

	for (uint64_t i = size - residualCount; i < size; i += 3)
		std::swap(image.data[i], image.data[i + 2]);

	std::chrono::steady_clock::time_point end = std::chrono::high_resolution_clock::now();

	return static_cast<int>(1 / std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1, 1>>>(end - start).count());
}

// Convert a BGR video to RGB using cv::cvtColor() (with SIMD acceleration), return fps performance metric
int bgrToRGBOpenCVSIMD(cv::Mat& image)
{
	std::chrono::steady_clock::time_point start = std::chrono::high_resolution_clock::now();

	cv::cvtColor(image, image, cv::COLOR_BGR2RGB);

	std::chrono::steady_clock::time_point end = std::chrono::high_resolution_clock::now();

	return static_cast<int>(1 / std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1, 1>>>(end - start).count());
}

// Convert a BGR video to RGB using scalar, AVX256, and cv::cvtColor() (with SIMD acceleration)
void bgrToRGBDemo(const std::string& videoPath)
{
	cv::Mat frame, result;
	cv::Mat frameScalar, frameAVX256, frameOpenCVSIMD;
	cv::VideoCapture video{ videoPath };

	int xmax = video.get(cv::CAP_PROP_FRAME_COUNT), ymax = 3000, avgRange = 15;
	cv::Mat plot = createFPSPlot(cv::Size(video.get(cv::CAP_PROP_FRAME_WIDTH), video.get(cv::CAP_PROP_FRAME_HEIGHT) / 2.5), std::pair<int, int>{xmax, ymax}, avgRange);

	std::vector<std::vector<int>> fpss(3, std::vector<int>(xmax));
	int frameCount = 0;

	while (true)
	{
		while (video.read(frame))
		{
			frameScalar = frame.clone();
			fpss[0][frameCount] = bgrToRGBScalar(frameScalar);

			frameAVX256 = frame.clone();
			fpss[1][frameCount] = bgrToRGBAVX256(frameAVX256);

			frameOpenCVSIMD = frame.clone();
			fpss[2][frameCount] = bgrToRGBOpenCVSIMD(frameOpenCVSIMD);

			++frameCount;

			if (cv::sum(frameScalar != frameAVX256) != cv::Scalar(0) || cv::sum(frameScalar != frameOpenCVSIMD) != cv::Scalar(0)) return;
			plotFPS(plot, std::pair<int, int>{xmax, ymax}, frameCount, avgRange, fpss);
			writeFPS(frameScalar, frameCount, fpss);

			cv::vconcat(std::array<cv::Mat, 2>{frameScalar, plot}, result);
			cv::imshow("Output", result);

			if (cv::pollKey() != -1) { cv::destroyAllWindows(); return; }
		}

		frameCount = 0, video.set(cv::CAP_PROP_POS_FRAMES, 0);
		plot = createFPSPlot(cv::Size(video.get(cv::CAP_PROP_FRAME_WIDTH), video.get(cv::CAP_PROP_FRAME_HEIGHT) / 2.5), std::pair<int, int>{xmax, ymax}, avgRange);
	}
}

#endif