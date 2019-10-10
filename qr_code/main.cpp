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
using namespace std;
//using std::string;
//using std::vector;
//using std::cout

Mat doGray(Mat& srcImg); // 灰度
Mat doBlur(Mat& srcImg); // 均值滤波除高频噪声
Mat doThold(Mat& srcImg); // 二值化，使图像中的数据大量减少，凸出目标轮廓
Point centerPoint(vector<vector<Point> > contours, int i); // 中心点提取
bool isNeedCorrection(Point point[]); // 是否需要修正

bool checkHasQrCode(vector<vector<Point>> allContours, vector<Vec4i> hierarchy, vector<vector<Point>>& resutlContours); // 检测图片中是否有二维码
Mat qrRun(Mat& srcImg);

int main(int argc, const char * argv[]) {
    string path = "/Users/cqt/Downloads/opencv_img/qr_code_2.jpg";
//    string path = "/Users/cqt/Downloads/opencv_img/baby.jpg";
    Mat image = imread(path);
    namedWindow("origin");
    imshow("origin", image);
    
    Mat gray = qrRun(image);
    if(!gray.empty()) {
        namedWindow("qrRun");
        imshow("qrRun", gray);
    }
    waitKey(0);
    
    return 0;
}

Mat qrRun(Mat& srcImg) {
    RNG rng(12345);
    
    Mat src_all = srcImg.clone();
    Mat src_gray = doGray(src_all);
    src_gray = doBlur(src_gray);
    
    Mat threshold_output;
    vector<vector<Point>> contours, contours2;
    vector<Vec4i> hierarchy;
    Mat drawing = Mat::zeros(srcImg.size(), CV_8UC3);
    Mat drawing2 = Mat::zeros(srcImg.size(), CV_8UC3);
    Mat drawingAllContours = Mat::zeros(srcImg.size(), CV_8UC3);
    
    threshold_output = doThold(src_gray);
    findContours(threshold_output, contours, hierarchy, RETR_TREE, CHAIN_APPROX_NONE, Point(0,0));
    
    bool hasQrCode = checkHasQrCode(contours, hierarchy, contours2);
    if(!hasQrCode) {
        cout << "Do not find qr code" << endl;
        Mat empty;
        return empty;
    }
    
    //获取定位角的中心坐标
    Point point[3];
    for(int i=0; i<contours2.size(); i++) {
        point[i] = centerPoint(contours2, i);
    }
    
    
    vector<vector<Point>> squares;
    vector<Point> hull, approx;
    for(int i=0; i<contours.size(); i++) {
        convexHull(contours[i], hull);
        approxPolyDP(Mat(hull), approx, arcLength(Mat(hull), true) * 0.02, true);
        if(approx.size() == 4) {
            squares.push_back(approx);
        }
    }
    
    vector<Point> qrSquares;
    double tempArea = -1;
    if(squares.size() > 0) {
//        drawContours(threshold_output, squares, -1, (0,255,0), 3);
        for(int i=0; i<squares.size(); i++) {
            int point1Result = pointPolygonTest(squares[i], point[0], 0);
            int point2Result = pointPolygonTest(squares[i], point[1], 0);
            int point3Result = pointPolygonTest(squares[i], point[2], 0);
            if(point1Result >= 0 && point2Result >= 0 && point3Result >= 0
               && (-1 == tempArea || fabs(contourArea(squares[i])) < tempArea)) {
                qrSquares = squares[i];
            }
        }
    }
    
    if(!qrSquares.empty()) {
        vector<vector<Point>> tempContours;
        tempContours.push_back(qrSquares);
        drawContours(threshold_output, tempContours, -1, (0,255,0), 3);
    }
    
    return threshold_output;
}

Mat doBlur(Mat& srcImg) {
    blur(srcImg, srcImg, Size(3,3));
    return srcImg;
}

Mat doGray(Mat& srcImg) {
    cvtColor(srcImg, srcImg, COLOR_RGBA2GRAY);
    return srcImg;
}

Mat doThold(Mat& srcImg) {
    threshold(srcImg, srcImg, 112, 255, THRESH_BINARY);
    return srcImg;
}

Point centerPoint(vector<vector<Point> > contours, int i) {
    long centerx = 0, centery = 0, n = contours[i].size();
    //在提取的小正方形的边界上每隔周长个像素提取一个点的坐标，
    //求所提取四个点的平均坐标（即为小正方形的大致中心）
    centerx = (contours[i][n / 4].x + contours[i][n * 2 / 4].x + contours[i][3 * n / 4].x + contours[i][n - 1].x) / 4;
    centery = (contours[i][n / 4].y + contours[i][n * 2 / 4].y + contours[i][3 * n / 4].y + contours[i][n - 1].y) / 4;
    Point point1 = Point(centerx, centery);
    return point1;
}

bool isNeedCorrection(Point point[]){
    for (int i = 0; i < 3; i++){
        if (point[i].x == point[(i + 1) % 3].x&&point[i].y == point[(i + 2) % 3].y)
            return true;
        if (point[i].y == point[(i + 1) % 3].y&&point[i].x == point[(i + 2) % 3].x)
            return true;
    }
    return false;
}

bool checkHasQrCode(vector<vector<Point>> allContours, vector<Vec4i> hierarchy, vector<vector<Point>>& resultContours) {
    int c = 0, ic = 0;
    // 边缘检测
    // 通过黑色定位角作为父轮廓，有两个子轮廓的特点，筛选出三个定位角
    int parentIdx = -1;
    for(int i=0; i<allContours.size(); i++) {
        if(hierarchy[i][2] != -1 && ic == 0) {
            parentIdx = i;
            ic++;
        }else if(hierarchy[i][2] != -1) {
            ic++;
        }else if(hierarchy[i][2] == -1) {
            ic = 0;
            parentIdx = -1;
        }
        
        //特征轮廓检测
        //有两个子轮廓
        if(ic == 2) {
            c++;
            cout << "找到第" << c << "黑色定位角"<< endl << endl;
            //保存找到的三个黑色定位角
            resultContours.push_back(allContours[parentIdx]);
            ic = 0;
            parentIdx = -1;
        }
    }
    
    return resultContours.size() == 3;
}
