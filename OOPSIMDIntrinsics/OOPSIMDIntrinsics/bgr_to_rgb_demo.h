#ifndef BGR_TO_RGB_DEMO_H
#define BGR_TO_RGB_DEMO_H

// Convert a BGR video to RGB using scalar, AVX256, and cv::cvtColor() (with SIMD acceleration)
void bgrToRGBDemo(const std::string& videoPath);

#endif 
