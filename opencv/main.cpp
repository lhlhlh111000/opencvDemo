//
//  main.cpp
//  opencv
//
//  Created by Cutie on 2019/6/18.
//  Copyright © 2019 cqt. All rights reserved.
//

#include <iostream>
#include <opencv4/opencv2/opencv.hpp>
using namespace cv;
using std::string;

int main(int argc, const char * argv[]) {
    string path = "/Users/cqt/Downloads/opencv_img/baby.jpg";
    Mat image = imread(path);
    namedWindow("origin");
    imshow("origin", image);
    
    Mat gray;
    cvtColor(image, gray, COLOR_RGBA2GRAY);
    namedWindow("gray");
    imshow("gray", gray);
    waitKey(0);
    
    return 0;
}
