#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>

using namespace std;
using namespace cv;

int main(){

    Mat image = imread("lena.png");
    Mat image_Ycbcr;
    Mat planes[3];
    vector<Mat> decompressed_img;
    Mat quantization_mat = (Mat_<double>(8, 8) <<
    16, 11, 10, 16, 24, 40, 51, 61,
    12, 12, 14, 19, 26, 58, 60, 55,
    14, 13, 16, 24, 40, 57, 69, 56,
    14, 17, 22, 29, 51, 87, 80, 62,
    18, 22, 37, 56, 68, 109, 103, 77,
    24, 35, 55, 64, 81, 104, 113, 92,
    49, 64, 78, 87, 103, 121, 120, 101,
    72, 92, 95, 98, 112, 100, 103, 99);
    // Mat quantization_mat = (Mat_<double>(8, 8) <<
    // 1, 1, 1, 1, 1, 1, 1, 1,
    // 1, 1, 1, 1, 1, 1, 1, 1,
    // 1, 1, 1, 1, 1, 1, 1, 1,
    // 1, 1, 1, 1, 1, 1, 1, 1,
    // 1, 1, 1, 1, 1, 1, 1, 1,
    // 1, 1, 1, 1, 1, 1, 1, 1,
    // 1, 1, 1, 1, 1, 1, 1, 1,
    // 1, 1, 1, 1, 1, 1, 1, 1);
    // Mat quantization_mat = (Mat_<double>(8, 8) <<100, 100, 100, 100, 100, 100, 100, 100,100, 100, 100, 100, 100, 100, 100, 100,100, 100, 100, 100, 100, 100, 100, 100,100, 100, 100, 100, 100, 100, 100, 100,100, 100, 100, 100, 100, 100, 100, 100,100, 100, 100, 100, 100, 100, 100, 100,100, 100, 100, 100, 100, 100, 100, 100,100, 100, 100, 100, 100, 100, 100, 100);
    Mat original_img;
    int height = image.size().height;
    int width = image.size().width;

    //initialize input
    cvtColor(image, image_Ycbcr, CV_BGR2YCrCb);
    split(image_Ycbcr, planes);
    //original img
    original_img = planes[0].clone();
    original_img.convertTo(original_img, CV_8UC1);
    Mat process_img = original_img.clone();

    //image_compressing
    for(int i = 0; i < height; i += 8){
        for(int j = 0; j < width; j += 8){
            //creating block
            Mat block = process_img(Rect(j, i, 8, 8));
            //converting the block to float
            block.convertTo(block, CV_64FC1);
            //dct
            dct(block, block, 0);
            //quantization
            divide(block, quantization_mat, block);
            //converting it back to unsigned int
            block.convertTo(block, CV_8UC1);
            //copying the blcokk to original image
            block.copyTo(process_img(Rect(j, i, 8, 8)) );
        }
    }

    //image_decompressing
    for(int i = 0; i < height; i += 8){
        for(int j = 0; j < width; j += 8){
            //creating block
            Mat block = process_img(Rect(j, i, 8, 8));
            //converting the block to float
            block.convertTo( block, CV_64FC1);
            //applying dequantization
            multiply(block, quantization_mat, block);
            //idct
            dct(block, block, 1);

            add(block, 128.0, block);
            //converting it back to unsigned int
            block.convertTo(block, CV_8UC1);
            //copying the block to the original image
            block.copyTo(process_img(Rect(j, i, 8, 8)) );
        }
    }

    // int sum = 0;
    // for(int i = 0; i < height; ++i){
    //     for(int j = 0; j < width; ++j){
    //         int oringin_value = original_img.at<uchar>(i,j);
    //         int decompressed_value = planes[0].at<uchar>(i,j);
    //         sum += pow()

    //     }
    // }
        
    imshow("1",process_img);
    waitKey(0);

    return 0;
}

