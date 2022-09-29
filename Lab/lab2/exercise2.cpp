#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main(){
    Mat image;
    float gamma = 10;
    unsigned char pix[256];
    image = imread("../data/lena.png",0);
    if(image.empty()){
        cout << "Could not open or find the image" << endl;
        return -1;
    }

    //rotate
    Mat trans_image = image.clone();
    // rotate(image, trans_image, ROTATE_90_COUNTERCLOCKWISE);
    for(int row = 0; row < image.rows; ++row){
        for(int col = 0;  col < image.cols; ++col){
            trans_image.at<uchar>(image.cols - col - 1, row) = image.at<uchar>(row,col);
        }
    }
    //refer table
    for(int i = 0; i < 256; ++i){
        pix[i] = saturate_cast<uchar>(pow((float)(i / 255.0),gamma) * 255.0f);
    }
    
    for(int row = 0; row < trans_image.rows; ++row){
        for(int col = 0; col < trans_image.cols; ++col){
            int val = trans_image.at<uchar>(row, col);
            if( val < 127) trans_image.at<uchar>(row,col) = 255 - trans_image.at<uchar>(row,col);
            else trans_image.at<uchar>(row,col) = pix[trans_image.at<uchar>(row,col)];  
        }
    }

    imshow("gray image",image);
    imshow("result",trans_image);
    waitKey(0);
}
