#ifndef ADD_DEMO_H
#define ADD_DEMO_H
 
// Add frames of video2 to video1 using scalar addition, AVX256 addition, and cv::add() (with SIMD acceleration)
void addDemo(const std::string& video1Path, const std::string& video2Path);

#endif 
