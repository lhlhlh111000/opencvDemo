//
//  main.cpp
//  water_mark
//
//  Created by Cutie on 2019/7/5.
//  Copyright © 2019 cqt. All rights reserved.
//

#include <stdio.h>
#include <iostream>
#include <opencv4/opencv2/opencv.hpp>
using namespace cv;
using std::string;


int main(int argc, const char * argv[]) {
    
    Mat img = imread("/Users/cqt/Downloads/opencv_img/baby2.jpg");
    Mat logo = imread("/Users/cqt/Downloads/opencv_img/logo.png");
    Mat logoMask = imread("/Users/cqt/Downloads/opencv_img/logo.png", 0);
    
    int tSize = img.cols/8;
    
    resize(logo, logo, Size(tSize, logo.rows*tSize/logo.cols));
    resize(logoMask, logoMask, Size(tSize, logo.rows*tSize/logo.cols));
    
    Mat imgROI = img(Rect(100, 100, logo.cols, logo.rows));
    
    addWeighted(imgROI, 1.0, logo, 0.2, 0, imgROI);
//    logo.copyTo(imgROI, logoMask);
    namedWindow("water_mark", WINDOW_AUTOSIZE);
    imshow("water_mark", img);
    
    waitKey(0);
    return 0;
}
