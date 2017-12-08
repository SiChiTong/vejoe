#include "stdafx.h"
#include "RecognizeColor.h"

#define MIN_RECOG_RECT_AREA         300
#define MAX_RECOG_RECT_AREA         1500
#define MIN_RECOG_RECT_WH_RATIO     -1
#define MAX_RECOG_RECT_WH_RATIO     -1


RecognizeColor::RecognizeColor(void)
{
   
}


RecognizeColor::~RecognizeColor(void)
{
}

Mat RecognizeColor::recoginze(Mat sourceFrame, EnumRecogColor color)
{
    Mat imgHsv;
    Mat imgThreshold;
    Scalar colorLow;
    Scalar colorUp;
    Point anchor(-1, -1);
    Mat kernel = getStructuringElement(MORPH_RECT, Size(2, 2), anchor);

    getColorBoundary(color, &colorLow, &colorUp);
    cvtColor(sourceFrame, imgHsv, COLOR_BGR2HSV);

    inRange(imgHsv, colorLow, colorUp, imgThreshold);

    morphologyEx(imgThreshold, imgThreshold, MORPH_OPEN, kernel, anchor, 1, 0, Scalar());
    morphologyEx(imgThreshold, imgThreshold, MORPH_CLOSE, kernel, anchor, 1, 0, Scalar());

    return imgThreshold;
}

void RecognizeColor::getColorBoundary(EnumRecogColor color, Scalar *pScalarLow, Scalar *pScalarUp)
{
    if (color == Red)
    {
        pScalarLow->val[0] = 156;
        pScalarLow->val[1] = 43;
        pScalarLow->val[2] = 46;
        pScalarUp->val[0] = 180;
        pScalarUp->val[1] = 255;
        pScalarUp->val[2] = 255;
        return;
    }

    if (color == Blue) 
    {
        pScalarLow->val[0] = 100;
        pScalarLow->val[1] = 43;
        pScalarLow->val[2] = 46;
        pScalarUp->val[0] = 124;
        pScalarUp->val[1] = 255;
        pScalarUp->val[2] = 255;
        return;
    }

    if (color == Green) 
    {
        pScalarLow->val[0] = 35;
        pScalarLow->val[1] = 43;
        pScalarLow->val[2] = 46;
        pScalarUp->val[0] = 77;
        pScalarUp->val[1] = 255;
        pScalarUp->val[2] = 255;
        return;
    }

    if (color == Yellow)
    {
        pScalarLow->val[0] = 26;
        pScalarLow->val[1] = 43;
        pScalarLow->val[2] = 46;
        pScalarUp->val[0] = 34;
        pScalarUp->val[1] = 255;
        pScalarUp->val[2] = 255;
        return;
    }

    if (color == Purple)
    {
        pScalarLow->val[0] = 125;
        pScalarLow->val[1] = 43;
        pScalarLow->val[2] = 46;
        pScalarUp->val[0] = 155;
        pScalarUp->val[1] = 255;
        pScalarUp->val[2] = 255;
        return;
    }

    if (color == Black)
    {
        pScalarLow->val[0] = 0;
        pScalarLow->val[1] = 0;
        pScalarLow->val[2] = 0;
        pScalarUp->val[0] = 180;
        pScalarUp->val[1] = 255;
        pScalarUp->val[2] = 46;
        return;
    }

    if (color == White)
    {
        pScalarLow->val[0] = 0;
        pScalarLow->val[1] = 0;
        pScalarLow->val[2] = 221;
        pScalarUp->val[0] = 180;
        pScalarUp->val[1] = 30;
        pScalarUp->val[2] = 255;
        return;
    }

    if (color == Gray)
    {
        pScalarLow->val[0] = 0;
        pScalarLow->val[1] = 0;
        pScalarLow->val[2] = 46;
        pScalarUp->val[0] = 180;
        pScalarUp->val[1] = 43;
        pScalarUp->val[2] = 220;
        return;
    }
}

