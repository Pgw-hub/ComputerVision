#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>

using namespace std;
using namespace cv;

int main(){

    Mat image = imread("lena.png");
    Mat image_Ycbcr;
    Mat planes[3];
    Mat y(512, 512, CV_8UC1);
    vector<Mat> decompressed_img;
    Mat quantization_mat1 = (Mat_<double>(8, 8) <<16, 11, 10, 16, 24, 40, 51, 61,12, 12, 14, 19, 26, 58, 60, 55,14, 13, 16, 24, 40, 57, 69, 56,14, 17, 22, 29, 51, 87, 80, 62,18, 22, 37, 56, 68, 109, 103, 77,24, 35, 55, 64, 81, 104, 113, 92,49, 64, 78, 87, 103, 121, 120, 101,72, 92, 95, 98, 112, 100, 103, 99);
    Mat quantization_mat2 = (Mat_<double>(8, 8) <<1, 1, 1, 1, 1, 1, 1, 1,1, 1, 1, 1, 1, 1, 1, 1,1, 1, 1, 1, 1, 1, 1, 1,1, 1, 1, 1, 1, 1, 1, 1,1, 1, 1, 1, 1, 1, 1, 1,1, 1, 1, 1, 1, 1, 1, 1,1, 1, 1, 1, 1, 1, 1, 1,1, 1, 1, 1, 1, 1, 1, 1);
    Mat quantization_mat3 = (Mat_<double>(8, 8) <<100, 100, 100, 100, 100, 100, 100, 100,100, 100, 100, 100, 100, 100, 100, 100,100, 100, 100, 100, 100, 100, 100, 100,100, 100, 100, 100, 100, 100, 100, 100,100, 100, 100, 100, 100, 100, 100, 100,100, 100, 100, 100, 100, 100, 100, 100,100, 100, 100, 100, 100, 100, 100, 100,100, 100, 100, 100, 100, 100, 100, 100);
    vector<Mat> quant_mat;
    Mat original_img;
    int height = image.size().height;
    int width = image.size().width;
    //prepare quantization mat
    for(int i = 0; i < 3; ++i){
        quant_mat.push_back(quantization_mat1);
        quant_mat.push_back(quantization_mat2);
        quant_mat.push_back(quantization_mat3);
    }


    for(int count = 0; count < 3; count++){

        //initialize input
        image = imread("lena.png");
        cvtColor(image, image_Ycbcr, CV_BGR2YCrCb);
        split(image_Ycbcr, planes);
        original_img = planes[0].clone();
        original_img.convertTo(original_img, CV_8UC1);

        //image_compressing
        for(int i = 0; i < height; i += 8){
            for(int j = 0; j < width; j += 8){
                //creating block
                Mat block = planes[0](Rect(j, i, 8, 8));

                //converting the block to float
                block.convertTo(block, CV_64FC1);

                //SUbtracting the blcok by 128
                // subtract(block, 128.0, block);//이건 왜 하는거지? 

                //dct
                dct(block, block, 0);

                //quantization
                divide(block, quant_mat[count], block);

                //converting it back to unsigned int
                block.convertTo(block, CV_8UC1);
                
                //copying the blcokk to original image
                block.copyTo(planes[0](Rect(j, i, 8, 8)) );
            }
        }

        imshow("dct",planes[0]);

        //image_decompressing
        for(int i = 0; i < height; i += 8){
            for(int j = 0; j < width; j += 8){
                //creating block
                Mat block = planes[0](Rect(j, i, 8, 8));

                //converting the block to float
                block.convertTo( block, CV_64FC1);

                //applying dequantization
                multiply(block, quant_mat[count], block);

                //idct
                dct(block, block, 1);

                //adding 128 to the block
                // add(block, 128.0, block);

                //converting it back to unsigned int
                block.convertTo(block, CV_8UC1);

                //copying the block to the original image
                block.copyTo(planes[0](Rect(j, i, 8, 8)) );
            }
        }
        decompressed_img.push_back(planes[0]);
    }
        double answer = PSNR(original_img,decompressed_img[0]);
        cout << sizeof(answer);
        
    imshow("original img",original_img);
    imshow("1",decompressed_img[0]);
    imshow("2",decompressed_img[1]);
    imshow("3",decompressed_img[2]);
    waitKey(0);

    return 0;
}

