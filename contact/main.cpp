//
//  main.cpp
//  contact
//
//  Created by Cutie on 2019/7/5.
//  Copyright © 2019 cqt. All rights reserved.
//

#include <iostream>
#include <opencv4/opencv2/opencv.hpp>
using namespace cv;
using std::string;

void quareImg(Mat &img) {
    int m = img.rows;
    int n = img.cols;
    
    if(m > n) {
        int y = (m - n)/2;
        img = Mat(img, Rect(0, y, n, n));
    }else {
        int x = (n - m)/2;
        img = Mat(img, Rect(x, 0, m, m));
    }
}

void resizeImg(Mat &img) {
    Size aSize = Size(500, 500);
    resize(img, img, aSize);
}

int main(int argc, const char * argv[]) {
    Mat combine, combine1, combine2;
    string a = "/Users/cqt/Downloads/opencv_img/baby.jpg";
    string b = "/Users/cqt/Downloads/opencv_img/baby1.jpg";
    string c = "/Users/cqt/Downloads/opencv_img/baby2.jpg";
    string d = "/Users/cqt/Downloads/opencv_img/baby3.jpg";
    
    Mat img1 = imread(a);
    Mat img2 = imread(b);
    Mat img3 = imread(c);
    Mat img4 = imread(d);
    
//    Size aSize = Size(500, 500)
//    Mat img1Size = Mat(aSize, CV_32S);
//    Mat img2Size = Mat(aSize, CV_32S);
//    Mat img3Size = Mat(aSize, CV_32S);
//    Mat img4Size = Mat(aSize, CV_32S);
//
//    resize(img1, img1Size, aSize);
//    resize(img2, img2Size, aSize);
//    resize(img3, img3Size, aSize);
//    resize(img4, img4Size, aSize);
//
//    hconcat(img1Size, img2Size, combine1);
//    hconcat(img3Size, img4Size, combine2);
//    vconcat(combine1, combine2, combine);
    
    quareImg(img1);
    quareImg(img2);
    quareImg(img3);
    quareImg(img4);
    
    resizeImg(img1);
    resizeImg(img2);
    resizeImg(img3);
    resizeImg(img4);
    
    hconcat(img1, img2, combine1);
    hconcat(img3, img4, combine2);
    vconcat(combine1, combine2, combine);
    
    namedWindow("concat", WINDOW_AUTOSIZE);
    imshow("concat", combine);
    
    waitKey(0);
    
    return 0;
}
