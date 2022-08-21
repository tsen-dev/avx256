#ifndef BLEND_DEMO_H
#define BLEND_DEMO_H
 
// Blend frames of video2 into video1 using scalar, AVX256, and cv::addWeighted() (with SIMD acceleration)
void blendDemo(const std::string& video1Path, const std::string& video2Path);

#endif 
