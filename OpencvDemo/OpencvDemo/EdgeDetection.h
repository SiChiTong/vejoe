#pragma once
#include <opencv2/opencv.hpp>

using namespace cv;

class EdgeDetection
{
public:
    EdgeDetection(void);
    ~EdgeDetection(void);

    Mat sobel(Mat source);
    Mat laplacian(Mat source);
    Mat canny(Mat source);
};

