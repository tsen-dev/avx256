#ifndef DEMO_H
#define DEMO_H

// Create and return a graph for plotting fps moving average points on
cv::Mat createFPSPlot(const cv::Size& size, const std::pair<int, int>& axisMax, int avgRange);

// Plot the fps moving average at the current frame, for each computation method used
void plotFPS(cv::Mat& plot, const std::pair<int, int>& axisMax, int frameCount, int avgRange, const std::vector<std::vector<int>>& fpss);

// Display the last fps metric for each computation method 
void writeFPS(cv::Mat& image, int frameCount, const std::vector<std::vector<int>>& fpss);

#endif 
