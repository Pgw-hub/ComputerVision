#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main(){

    //moon
    Mat image;
    image = imread("moon.png",0);
    imshow("moon",image);

    //roi설정 이미지 만들기
    Rect rect(image.cols/2, 0, image.cols/2, image.rows);
    Mat rect_roi = image(rect);

    //roi한사진 sharpening하기
    Mat unsharp_mask, sharpening, avg_image;
    unsharp_mask.convertTo(unsharp_mask,CV_16S);
    sharpening.convertTo(unsharp_mask,CV_16S);

    blur(rect_roi, avg_image, Size(3,3));
    unsharp_mask = rect_roi - avg_image;
    sharpening = rect_roi + (6 * unsharp_mask);

    //최종 답은 원본이미지와 sharpening이미지 반반합쳐서 만들기
    Mat answer = image.clone();
    int half = image.cols/2;
    for(int row = 0; row < image.rows; ++row){
        for(int col = 0; col < half; ++col){
            answer.at<uchar>(row,col) = image.at<uchar>(row,col);
            answer.at<uchar>(row,col + half) = sharpening.at<uchar>(row,col);
        }
    }
    imshow("moon_filtered",answer);

    //slatnpepper
    Mat image2;
    image2 = imread("saltnpepper.png",0);
    imshow("saltnpepper",image2);

    //roi설정 이미지 만들기
    Rect rect2(0, 0, image2.cols/2, image2.rows);
    Mat rect2_roi = image2(rect2);
    
    //해당 사진 blur처리하기
    Mat mf1;
    medianBlur(rect2_roi,mf1,9);

    //최종 답 합치기
    Mat answer2 = image2.clone();
    half = image2.cols / 2;
    for(int row = 0; row < image2.rows; ++row){
        for(int col = 0; col < half; ++col){
            answer2.at<uchar>(row,col) = mf1.at<uchar>(row,col);
            answer2.at<uchar>(row,col + half) = image2.at<uchar>(row,col + half);
        }
    }
    imshow("saltnpepper_filtered",answer2);
    waitKey(0);

    return 0;
}
