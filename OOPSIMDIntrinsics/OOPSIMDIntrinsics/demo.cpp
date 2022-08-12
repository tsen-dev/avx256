#include <iostream>
#include <string>

#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "test.h"
#include "add_demo.h"

#ifndef TEST

int main(void)
{
	addDemo("C:/Users/Toprak/Desktop/1.mp4", "C:/Users/Toprak/Desktop/2.mp4");

	return 0;
}

#endif