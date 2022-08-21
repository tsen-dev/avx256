#include <iostream>
#include <string>
#include <numeric>

#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "test.h"
#include "avx256.h"
#include "blend_demo.h"
#include "threshold_demo.h"
#include "abs_diff_demo.h"
#include "bgr_to_rgb_demo.h"

#ifndef TEST

// Create and return a graph for plotting fps moving average points on
cv::Mat createFPSPlot(const cv::Size& size, const std::pair<int, int>& axisMax, int avgRange)
{
	cv::Mat plot = cv::Mat::zeros(size.height, size.width, CV_8UC3);

	for (int i = 0; i < axisMax.second; i += axisMax.second / 10)
		cv::putText(plot, "- " + std::to_string(i),
			cv::Point{ 0, static_cast<int>(size.height * (1 - static_cast<double>(i) / axisMax.second)) },
			cv::FONT_HERSHEY_DUPLEX, 0.5, CV_RGB(170, 170, 170)
		);

	cv::putText(plot, "FPS (avg of last " + std::to_string(avgRange) + " frames)", cv::Point{ size.width - 235, 25 }, cv::FONT_HERSHEY_DUPLEX, 0.5, CV_RGB(170, 170, 170));
	cv::putText(plot, "Scalar", cv::Point{ size.width - 50, 50 }, cv::FONT_HERSHEY_DUPLEX, 0.5, CV_RGB(255, 0, 0));
	cv::putText(plot, "AVX256", cv::Point{ size.width - 63, 75 }, cv::FONT_HERSHEY_DUPLEX, 0.5, CV_RGB(0, 255, 0));
	cv::putText(plot, "OpenCVSIMD", cv::Point{ size.width - 100, 100 }, cv::FONT_HERSHEY_DUPLEX, 0.5, CV_RGB(0, 255, 255));

	return plot;
}

// Plot the fps moving average at the current frame, for each method used
void plotFPS(cv::Mat& plot, const std::pair<int, int>& axisMax, int frameCount, int avgRange, const std::vector<std::vector<int>>& fpss)
{
	int x = static_cast<int>(plot.cols * frameCount / static_cast<double>(axisMax.first));

	cv::Scalar colours[3] = { CV_RGB(255, 0, 0), CV_RGB(0, 255, 0), CV_RGB(0, 255, 255)};
	int ys[3];

	for (int i = 0; i < 3; ++i)
	{
		if (frameCount < avgRange)
			ys[i] = plot.rows * (1 - (std::accumulate(fpss[i].begin(), fpss[i].begin() + frameCount, 0) / frameCount) / static_cast<double>(axisMax.second));
		else
			ys[i] = plot.rows * (1 - (std::accumulate(fpss[i].begin() + frameCount - avgRange, fpss[i].begin() + frameCount, 0) / avgRange) / static_cast<double>(axisMax.second));

		cv::circle(plot, cv::Point(x, ys[i]), 2, colours[i]);
	}
}

// Display the last fps metric for each method 
void writeFPS(cv::Mat& image, int frameCount, const std::vector<std::vector<int>>& fpss)
{
	cv::Scalar colours[3] = { CV_RGB(255, 0, 0), CV_RGB(0, 255, 0), CV_RGB(0, 255, 255) };
	std::vector<std::string> texts{ "Scalar: ", "AVX256: ", "OpenCVSIMD: " };

	for (int i = 0; i < 3; ++i)
		cv::putText(image, texts[i] + std::to_string(fpss[i][frameCount - 1]) + "fps", cv::Point{ 0, (i + 1) * 30 }, cv::FONT_HERSHEY_DUPLEX, 0.75, colours[i], 1);
}

int main(void)
{	
	cv::setNumThreads(0);

	if (AVX256Utils::HasAVX2Support())
	{
		blendDemo("videos/1.mp4", "videos/2.mp4");
		thresholdDemo("videos/1.mp4");
		absDiffDemo("videos/3.mp4");
		bgrToRGBDemo("videos/1.mp4");
	}

	else std::cerr << "Error: Your CPU does not support the AVX2 instruction set!";

	return 0;
}

#endif