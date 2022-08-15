#ifndef ABS_DIFF_DEMO_H
#define ABS_DIFF_DEMO_H

// Create a mask over the pixels that show a difference from the previous frame using scalar, AVX256, and cv::absdiff() (with SIMD acceleration)
void absDiffDemo(const std::string& videoPath);

#endif 
