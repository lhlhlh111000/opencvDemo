//
//  process_utils.cpp
//  square_scan
//
//  Created by Cutie on 2019/10/9.
//  Copyright © 2019 cqt. All rights reserved.
//

#include "process_utils.hpp"

double getAngle(Point pt1, Point pt2, Point pt0) {
    double dx1 = pt1.x - pt0.x;
    double dy1 = pt1.y - pt0.y;
    double dx2 = pt2.x - pt0.x;
    double dy2 = pt2.y - pt0.y;
    return (dx1 * dx2 + dy1 * dy2) /
    sqrt((dx1 * dx1 + dy1 * dy1) * (dx2 * dx2 + dy2 * dy2) + 1e-10);
}

int findLargeSquare(const vector<vector<Point> > &squares,
                    vector<Point> &biggest_square) {
    if (!squares.size()) return -1;
    
    int max_width = 0;
    int max_height = 0;
    int max_square_idx = 0;
    for (int i = 0; i < squares.size(); i++) {
        cv::Rect rectangle = boundingRect(Mat(squares[i]));
        if ((rectangle.width >= max_width) && (rectangle.height >= max_height)) {
            max_width = rectangle.width;
            max_height = rectangle.height;
            max_square_idx = i;
        }
    }
    biggest_square = squares[max_square_idx];
    return max_square_idx;
}

double getSpacePointToPoint(Point p1, Point p2) {
    int a = p1.x - p2.x;
    int b = p1.y - p2.y;
    return sqrt(a * a + b * b);
}

Point2f computeIntersect(Vec4i a, Vec4i b) {
    int x1 = a[0], y1 = a[1], x2 = a[2], y2 = a[3], x3 = b[0], y3 = b[1], x4 = b[2], y4 = b[3];
    
    if (float d = ((float) (x1 - x2) * (y3 - y4)) - ((y1 - y2) * (x3 - x4))) {
        cv::Point2f pt;
        pt.x = ((x1 * y2 - y1 * x2) * (x3 - x4) - (x1 - x2) * (x3 * y4 - y3 * x4)) / d;
        pt.y = ((x1 * y2 - y1 * x2) * (y3 - y4) - (y1 - y2) * (x3 * y4 - y3 * x4)) / d;
        return pt;
    } else{
        return cv::Point2f(-1, -1);
    }
}

vector<Point> findSquarePoints(vector<Point> &hull, vector<Point> &largest_square) {
    vector<Point> squarePoints;
    
    vector<Point> approx, newApprox;
    approxPolyDP(Mat(hull), approx, 3, true);
    double maxL = arcLength(Mat(approx), true) * 0.02;
    for (Point p : approx) {
        if (!(getSpacePointToPoint(p, largest_square[0]) > maxL &&
              getSpacePointToPoint(p, largest_square[1]) > maxL &&
              getSpacePointToPoint(p, largest_square[2]) > maxL &&
              getSpacePointToPoint(p, largest_square[3]) > maxL)) {
            newApprox.push_back(p);
        }
    }
    
    vector<Vec4i> lines;
    for (int i = 0; i < newApprox.size(); i++) {
        Point p1 = newApprox[i];
        Point p2 = newApprox[(i + 1) % newApprox.size()];
        if (getSpacePointToPoint(p1, p2) > 2 * maxL) {
            lines.push_back(Vec4i(p1.x, p1.y, p2.x, p2.y));
        }
    }
    
    for (int i = 0; i < lines.size(); i++) {
        Point cornor;
        cornor = computeIntersect(lines[i], lines[(i + 1) % lines.size()]);
        squarePoints.push_back(cornor);
    }
    
    
    return squarePoints;
}

vector<Point> doProcess(Mat &gray_img) {
    vector<vector<Point>> contours, squares, hulls;
    findContours(gray_img, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);
    
    vector<Point> hull, approx;
    for(int i=0; i<contours.size(); i++) {
        convexHull(contours[i], hull);
        approxPolyDP(Mat(hull), approx, arcLength(Mat(hull), true) * 0.02, true);
        if (approx.size() == 4 && isContourConvex(Mat(approx))) {
            double maxCosine = 0;
            for (int j = 2; j < 5; j++) {
                double cosine = fabs(getAngle(approx[j % 4], approx[j - 2], approx[j - 1]));
                maxCosine = MAX(maxCosine, cosine);
            }
            if (maxCosine < 0.3) {
                squares.push_back(approx);
                hulls.push_back(hull);
            }
        }
    }
    
    vector<Point> large_square;
    int index = findLargeSquare(squares, large_square);
    vector<Point> result;
    if(large_square.size() > 0 && index > -1) {
        hull = hulls[index];
        result = findSquarePoints(hull, large_square);
    }
    return result;
}

bool checkSquareArea(Mat &srcImg, vector<Point> points) {
    if(!points.size()) {
        return false;
    }
    
    double squareArea = contourArea(points);
    double imgArea = srcImg.cols*srcImg.rows;
    double p = imgArea/squareArea;
    cout << "p: " << p << endl;
    return imgArea/squareArea < 8;
}
