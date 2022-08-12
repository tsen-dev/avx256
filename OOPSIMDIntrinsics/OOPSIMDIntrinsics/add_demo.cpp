#include <string>
#include <chrono>
#include <numeric>
#include <array>
#include <thread>

#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "test.h"
#include "avx256.h"
#include "add_demo.h"


// Create and return a graph for plotting fps moving average points on
cv::Mat createAddPlot(const cv::Size& size, const std::pair<int, int>& axisMax, int avgRange)
{
	cv::Mat plot = cv::Mat::zeros(size.height, size.width, CV_8UC3);

	for (int i = 0; i < axisMax.second; i += 200)
		cv::putText(plot, "- " + std::to_string(i), 
			cv::Point{ 0, static_cast<int>(size.height * (1 - static_cast<double>(i) / axisMax.second)) },
			cv::FONT_HERSHEY_DUPLEX, 0.5, CV_RGB(170, 170, 170)
		);

	cv::putText(plot, "FPS (avg of last " + std::to_string(avgRange) + " frames)", cv::Point{ size.width - 250, 25 }, cv::FONT_HERSHEY_DUPLEX, 0.5, CV_RGB(255, 255, 0));
	cv::putText(plot, "Scalar", cv::Point{ size.width - 100, 50 }, cv::FONT_HERSHEY_DUPLEX, 0.5, CV_RGB(255, 0, 0));
	cv::putText(plot, "AVX256", cv::Point{ size.width - 100, 75 }, cv::FONT_HERSHEY_DUPLEX, 0.5, CV_RGB(0, 255, 0));
	cv::putText(plot, "OpenCVSIMD", cv::Point{ size.width - 100, 100 }, cv::FONT_HERSHEY_DUPLEX, 0.5, CV_RGB(255, 255, 255));
	cv::putText(plot, "OpenCVGPU", cv::Point{ size.width - 100, 125 }, cv::FONT_HERSHEY_DUPLEX, 0.5, CV_RGB(0, 255, 255));

	return plot;
}

// Plot the fps moving average at the current frame, for each addition method used
void plotFPS(cv::Mat& plot, const std::pair<int, int>& axisMax, int frameCount, int avgRange, const std::vector<std::vector<int>>& fpss)
{
	int x = static_cast<int>(plot.cols * frameCount / static_cast<double>(axisMax.first));

	cv::Scalar colours[4] = { CV_RGB(255, 0, 0), CV_RGB(0, 255, 0), CV_RGB(255, 255, 255), CV_RGB(0, 255, 255) };
	int ys[4];

	for (int i = 0; i < 4; ++i)
	{
		if (frameCount < avgRange)
			ys[i] = plot.rows * (1 - (std::accumulate(fpss[i].begin(), fpss[i].begin() + frameCount, 0) / frameCount) / static_cast<double>(axisMax.second));
		else
			ys[i] = plot.rows* (1 - (std::accumulate(fpss[i].begin() + frameCount - avgRange, fpss[i].begin() + frameCount, 0) / avgRange) / static_cast<double>(axisMax.second));

		cv::circle(plot, cv::Point(x, ys[i]), 2, colours[i]);
	}
}

// Display the last fps metric for each addition method 
void writeFPS(cv::Mat& image, int frameCount, const std::vector<std::vector<int>>& fpss)
{
	cv::Scalar colours[4] = { CV_RGB(255, 0, 0), CV_RGB(0, 255, 0), CV_RGB(170, 170, 170), CV_RGB(0, 255, 255) };
	std::vector<std::string> texts{ "Scalar: ", "AVX256: ", "OpenCVSIMD: ", "OpenCVGPU: " };

	for (int i = 0; i < 4; ++i)
		cv::putText(image, texts[i] + std::to_string(fpss[i][frameCount - 1]) + "fps", cv::Point{ 0, (i + 1) * 30 }, cv::FONT_HERSHEY_DUPLEX, 0.75, colours[i], 1);
}

// Add image2 to image1 using scalar addition, return fps performance metric
int addScalar(cv::Mat& image1, cv::Mat& image2)
{
	std::chrono::steady_clock::time_point start = std::chrono::high_resolution_clock::now();	

	int64_t size = static_cast<uint64_t>(image1.rows) * image1.cols * image1.channels();

	for (int64_t i = 0; i < size; ++i)
		image1.data[i] = static_cast<uint8_t>(image1.data[i] + image2.data[i]) >= image1.data[i] ? image1.data[i] + image2.data[i] : UINT8_MAX;

	std::chrono::steady_clock::time_point end = std::chrono::high_resolution_clock::now();

	return static_cast<int>(1 / std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1, 1>>>(end - start).count());
}

// Add image2 to image1 using AVX256 addition, return fps performance metric
int addAVX256(cv::Mat& image1, cv::Mat& image2)
{	
	std::chrono::steady_clock::time_point start = std::chrono::high_resolution_clock::now();

	AVX256<uint8_t> avxImage1{ image1.data }, avxImage2{ image2.data };

	int64_t size = static_cast<uint64_t>(image1.rows) * image1.cols * image1.channels();
	int64_t count = size / 32;
	int residualCount = size % 32;

	for (int i = 0; i < count; ++i, avxImage1.Next(), avxImage2.Next())
		avxImage1 += avxImage2;

	for (int64_t i = size - residualCount; i < size; ++i)
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

// Add image2 to image1 using cv::add() (gpu), return fps performance metric
int addOpenCVGPU(cv::UMat& image1, cv::UMat& image2)
{
	std::chrono::steady_clock::time_point start = std::chrono::high_resolution_clock::now();

	cv::add(image1, image2, image1);

	std::chrono::steady_clock::time_point end = std::chrono::high_resolution_clock::now();

	return static_cast<int>(1 / std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1, 1>>>(end - start).count());
}

// Add frames of video2 to video1 using scalar addition, AVX256 addition, and cv::add() (with both SIMD and GPU acceleration)
void addDemo(const std::string& video1Path, const std::string& video2Path)
{
	cv::Mat frame1, frame2, result;
	cv::Mat frame1Scalar, frame1AVX256, frame1OpenCVSIMD;
	cv::UMat frame1OpenCVGPU, frame2GPU;
	cv::VideoCapture video1{ video1Path }, video2{ video2Path };	

	int xmax = std::min(video1.get(cv::CAP_PROP_FRAME_COUNT), video2.get(cv::CAP_PROP_FRAME_COUNT)), ymax = 3000, avgRange = 15;
	cv::Mat plot = createAddPlot(cv::Size(video1.get(cv::CAP_PROP_FRAME_WIDTH), video1.get(cv::CAP_PROP_FRAME_HEIGHT) / 2.5), std::pair<int, int>{xmax, ymax}, avgRange);

	std::vector<std::vector<int>> fpss(4, std::vector<int>(xmax));
	int frameCount = 0;

	while (video1.read(frame1) && video2.read(frame2))
	{
		frame1Scalar = frame1.clone();
		fpss[0][frameCount] = addScalar(frame1Scalar, frame2);

		frame1AVX256 = frame1.clone();
		fpss[1][frameCount] = addAVX256(frame1AVX256, frame2);

		frame1OpenCVSIMD = frame1.clone();
		fpss[2][frameCount] = addOpenCVSIMD(frame1OpenCVSIMD, frame2);

		frame1OpenCVGPU = frame1.getUMat(cv::ACCESS_FAST), frame2GPU = frame2.getUMat(cv::ACCESS_FAST);
		fpss[3][frameCount] = addOpenCVGPU(frame1OpenCVGPU, frame2GPU);

		++frameCount;

		plotFPS(plot, std::pair<int, int>{xmax, ymax}, frameCount, avgRange, fpss);
		writeFPS(frame1Scalar, frameCount, fpss);

		cv::vconcat(std::array<cv::Mat, 2>{frame1Scalar, plot}, result);
		cv::imshow("Output", result);	

		if (cv::pollKey() != -1) break;
	}

	cv::waitKey();
}