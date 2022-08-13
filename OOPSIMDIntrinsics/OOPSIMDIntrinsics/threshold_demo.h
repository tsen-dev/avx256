#ifndef THRESHOLD_DEMO_H
#define THRESHOLD_DEMO_H

// Convert the frames of the video into a binary image using scalar, simd, and cv::threshold() (with SIMD acceleration). The threshold boundary is configurable by a trackbar element
void thresholdDemo(const std::string& videoPath);

#endif 
