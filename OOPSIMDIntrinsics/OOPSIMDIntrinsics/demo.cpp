#include <iostream>
#include <array>
#include <random>
#include <string>

#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "test.h"
#include "avx256.h"

#ifndef TEST

void addScalar(cv::Mat& image, uint8_t value)
{
	int size = image.dataend - image.datastart;

	for (int i = 0; i < size; ++i)
		image.data[i] = (image.data[i] > UINT8_MAX - value) ? 255 : image.data[i] + value;
}

void addAVX256(cv::Mat& image, uint8_t value)
{
	AVX256<uint8_t> avxImage{ image.data };

	AVX256<uint8_t> avxValue;
	avxValue = value;

	int size = image.dataend - image.datastart;
	int count = size / 32;
	int residualCount = size % 32;

	for (int i = 0; i < count; ++i, avxImage.Next())
		avxImage += avxValue;

	for (int i = size - residualCount; i < size; ++i)
		image.data[i] = (image.data[i] > UINT8_MAX - value) ? 255 : image.data[i] + value;
}

void addDemo()
{

}

void runDemos()
{
	addDemo();
}

int main(void)
{
	//runDemos();

	cv::Mat frame;
	
	cv::VideoCapture webcam{ 0 };

	while (webcam.read(frame))
	{
		cv::resize(frame, frame, cv::Size{}, 5, 5, cv::InterpolationFlags::INTER_LINEAR);

		clock_t start = clock();
		addAVX256(frame, 100);
		clock_t time = clock() - start;

		cv::putText(frame, std::to_string((float) CLOCKS_PER_SEC / time), cv::Point{0, 100}, cv::FONT_HERSHEY_DUPLEX, 2.0, CV_RGB(0, 255, 0), 2);
		cv::imshow("Webcam", frame);

		if (cv::waitKey(1) >= 0) break;
	}

	return 0;
}

#endif