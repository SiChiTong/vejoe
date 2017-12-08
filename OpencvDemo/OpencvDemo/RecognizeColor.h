#pragma once
#include <opencv2/opencv.hpp>

using namespace cv;

typedef enum _EnumRecogColor
{
    Red,
    Blue,
    Green,
    Yellow,
    Purple,
    Black,
    White,
    Gray
}EnumRecogColor;

class RecognizeColor
{
public:
    RecognizeColor(void);
    ~RecognizeColor(void);

    Mat recoginze(Mat sourceFrame, EnumRecogColor color);
private:

    void getColorBoundary(EnumRecogColor color, Scalar *pScalarLow, Scalar *pScalarUp);
};


