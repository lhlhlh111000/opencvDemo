//
//  square_scan.cpp
//  square_scan
//
//  Created by Cutie on 2019/10/8.
//  Copyright © 2019 cqt. All rights reserved.
//


#include "old_process.cpp"
#include "new_process.cpp"

#include <dirent.h>


vector<string> dirFileNameList(string dirPath) {
    vector<string> fileNames;
    struct dirent *ptr;
    DIR *dir;
    dir=opendir(dirPath.c_str());
    while((ptr=readdir(dir))!=NULL) {
        if(ptr->d_name[0] == '.')
            continue;
        fileNames.push_back(ptr->d_name);
    }
    for (int i = 0; i < fileNames.size(); ++i) {
        cout << fileNames[i] << endl;
    }
    closedir(dir);
    
    return fileNames;
}

void showFindResult(string path) {
    Mat img_rgb = imread(path);
    resize(img_rgb, img_rgb, Size(img_rgb.cols/2, img_rgb.rows/2));
    imshow("origin", img_rgb);
    vector<Point> points = doOldFind(img_rgb);
//    if(!points.size()) {
        points = doFind(img_rgb);
//    }
    
    if(points.size()) {
        for (int i = 0; i < points.size(); i++) {
            line(img_rgb, points[i], points[(i + 1) % points.size()],
                 Scalar(0, 0, 255), 5);
        }
    }
    imshow("result", img_rgb);
}

int main(int argc, const char * argv[]) {
    string dirPath = "/Users/cqt/Downloads/opencv_img/o";
    vector<string> fileNames = dirFileNameList(dirPath);
    if(fileNames.size()) {
        int i=0;
        while (i<fileNames.size()) {
            showFindResult(dirPath + "/" + fileNames[i]);
            i++;
            waitKey(0);
        }
    }
    return 0;
}
