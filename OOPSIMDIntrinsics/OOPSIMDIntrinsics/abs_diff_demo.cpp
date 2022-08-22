#include <string>
#include <chrono>
#include <array>
#include <cmath>

#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "test.h"
#include "avx256.h"
#include "demo.h"

#ifndef TEST

// Create a mask over the pixels that show a difference from the previous frame using scalar operations, return fps performance metric
int absDiffScalar(cv::Mat& previousFrame, cv::Mat& currentFrame, cv::Mat& mask)
{
	std::chrono::steady_clock::time_point start = std::chrono::high_resolution_clock::now();

	uint64_t size = static_cast<uint64_t>(currentFrame.rows) * currentFrame.cols * currentFrame.channels();

	for (uint64_t i = 0; i < size; ++i)
		mask.data[i] = abs(currentFrame.data[i] - previousFrame.data[i]);

	std::chrono::steady_clock::time_point end = std::chrono::high_resolution_clock::now();

	return static_cast<int>(1 / std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1, 1>>>(end - start).count());
}

// Create a mask over the pixels that show a difference from the previous frame using AVX256, return fps performance metric
int absDiffAVX256(cv::Mat& previousFrame, cv::Mat& currentFrame, cv::Mat& mask)
{
	std::chrono::steady_clock::time_point start = std::chrono::high_resolution_clock::now();

	AVX256<uint8_t> avxPreviousFrame{ previousFrame.data }, avxCurrentFrame{ currentFrame.data }, avxMask{ mask.data };

	uint64_t size = static_cast<uint64_t>(currentFrame.rows) * currentFrame.cols * currentFrame.channels();
	uint64_t count = size / 32;
	int residualCount = size % 32;

	for (int i = 0; i < count; ++i, avxCurrentFrame.Next(), avxPreviousFrame.Next(), avxMask.Next())
		avxMask.Set(avxCurrentFrame).AbsoluteDifference(avxPreviousFrame);

	for (uint64_t i = size - residualCount; i < size; ++i)
		mask.data[i] = abs(currentFrame.data[i] - previousFrame.data[i]);
		
	std::chrono::steady_clock::time_point end = std::chrono::high_resolution_clock::now();

	return static_cast<int>(1 / std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1, 1>>>(end - start).count());
}

// Create a mask over the pixels that show a difference from the previous frame using cv::absdiff(), return fps performance metric
int absDiffOpenCVSIMD(cv::Mat& previousFrame, cv::Mat& currentFrame, cv::Mat& mask)
{
	std::chrono::steady_clock::time_point start = std::chrono::high_resolution_clock::now();

	cv::absdiff(currentFrame, previousFrame, mask);

	std::chrono::steady_clock::time_point end = std::chrono::high_resolution_clock::now();

	return static_cast<int>(1 / std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1, 1>>>(end - start).count());
}

// Create a mask over the pixels that show a difference from the previous frame using scalar, AVX256, and cv::absdiff() (with SIMD acceleration)
void absDiffDemo(const std::string& videoPath)
{
	cv::Mat currentFrameBGR, currentFrameGray, previousFrameGray, result, currentFrameBGRSmall;
	cv::VideoCapture video{ videoPath }; 
	int width = video.get(cv::CAP_PROP_FRAME_WIDTH), height = video.get(cv::CAP_PROP_FRAME_HEIGHT);
	cv::Mat maskScalar(height, width, CV_8UC1), maskAVX256(height, width, CV_8UC1), maskOpenCVSIMD(height, width, CV_8UC1), maskBGR(height, width, CV_8UC3);

	int xmax = video.get(cv::CAP_PROP_FRAME_COUNT) - 1, ymax = 10000, avgRange = 15;
	cv::Mat plot = createFPSPlot(cv::Size(video.get(cv::CAP_PROP_FRAME_WIDTH), video.get(cv::CAP_PROP_FRAME_HEIGHT) / 2.5), std::pair<int, int>{xmax, ymax}, avgRange);

	std::vector<std::vector<int>> fpss(3, std::vector<int>(xmax));	
	video.read(previousFrameGray), cv::cvtColor(previousFrameGray, previousFrameGray, cv::COLOR_BGR2GRAY);
	int frameCount = 0;

	while (true)
	{
		while (video.read(currentFrameBGR))
		{
			cv::cvtColor(currentFrameBGR, currentFrameGray, cv::COLOR_BGR2GRAY);

			fpss[0][frameCount] = absDiffScalar(previousFrameGray, currentFrameGray, maskScalar);
			fpss[1][frameCount] = absDiffAVX256(previousFrameGray, currentFrameGray, maskAVX256);
			fpss[2][frameCount] = absDiffOpenCVSIMD(previousFrameGray, currentFrameGray, maskOpenCVSIMD);

			++frameCount;

			previousFrameGray = currentFrameGray.clone();

			if (cv::sum(maskScalar != maskAVX256) != cv::Scalar(0) || cv::sum(maskScalar != maskOpenCVSIMD) != cv::Scalar(0)) return;			
			cv::cvtColor(maskScalar, maskBGR, cv::COLOR_GRAY2BGR);
			plotFPS(plot, std::pair<int, int>{xmax, ymax}, frameCount, avgRange, fpss);
			writeFPS(maskBGR, frameCount, fpss);
			
			cv::vconcat(std::array<cv::Mat, 2>{maskBGR, plot}, result);			
			cv::resize(currentFrameBGR, currentFrameBGRSmall, cv::Size{ currentFrameBGR.cols / 3, currentFrameBGR.rows / 3 });			
			cv::vconcat(std::array<cv::Mat, 2>{currentFrameBGRSmall, cv::Mat::zeros(cv::Size{ currentFrameBGRSmall.cols, result.rows - currentFrameBGRSmall.rows }, CV_8UC3)}, currentFrameBGRSmall);			
			cv::hconcat(std::array<cv::Mat, 2>{result, currentFrameBGRSmall}, result);

			cv::imshow("Output", result);

			if (cv::pollKey() != -1) { cv::destroyAllWindows(); return; }
		}

		frameCount = 0, video.set(cv::CAP_PROP_POS_FRAMES, 0);
		plot = createFPSPlot(cv::Size(video.get(cv::CAP_PROP_FRAME_WIDTH), video.get(cv::CAP_PROP_FRAME_HEIGHT) / 2.5), std::pair<int, int>{xmax, ymax}, avgRange);
		video.read(previousFrameGray), cv::cvtColor(previousFrameGray, previousFrameGray, cv::COLOR_BGR2GRAY);
	}
}

#endif