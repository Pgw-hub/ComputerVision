#include <iostream>
#include <cmath>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>

using namespace std;
using namespace cv;

Mat quantization_mat1 = (Mat_<double>(8, 8) <<
16, 11, 10, 16, 24, 40, 51, 61,
12, 12, 14, 19, 26, 58, 60, 55,
14, 13, 16, 24, 40, 57, 69, 56,
14, 17, 22, 29, 51, 87, 80, 62,
18, 22, 37, 56, 68, 109, 103, 77,
24, 35, 55, 64, 81, 104, 113, 92,
49, 64, 78, 87, 103, 121, 120, 101,
72, 92, 95, 98, 112, 100, 103, 99);
Mat quantization_mat2 = (Mat_<double>(8, 8) <<
1, 1, 1, 1, 1, 1, 1, 1,
1, 1, 1, 1, 1, 1, 1, 1,
1, 1, 1, 1, 1, 1, 1, 1,
1, 1, 1, 1, 1, 1, 1, 1,
1, 1, 1, 1, 1, 1, 1, 1,
1, 1, 1, 1, 1, 1, 1, 1,
1, 1, 1, 1, 1, 1, 1, 1,
1, 1, 1, 1, 1, 1, 1, 1);
Mat quantization_mat3 = (Mat_<double>(8, 8) <<
100, 100, 100, 100, 100, 100, 100, 100,
100, 100, 100, 100, 100, 100, 100, 100,
100, 100, 100, 100, 100, 100, 100, 100,
100, 100, 100, 100, 100, 100, 100, 100,
100, 100, 100, 100, 100, 100, 100, 100,
100, 100, 100, 100, 100, 100, 100, 100,
100, 100, 100, 100, 100, 100, 100, 100,
100, 100, 100, 100, 100, 100, 100, 100);

#define SIZE 512
int main(){

    Mat image = imread("lena.png");
    Mat image_Ycbcr;
    Mat Ycb[3];
    Mat original_img(SIZE, SIZE, CV_8UC1);
    Mat result1 = Mat_<double>(8, 8);
    Mat result2 = Mat_<double>(8, 8);
    Mat result3 = Mat_<double>(8, 8);

    int height = image.size().height;
    int width = image.size().width;

    //extract y channel
    cvtColor(image,image_Ycbcr, CV_BGR2YCrCb);
    split(image_Ycbcr,Ycb);
    for(int j = 0; j < SIZE; ++j){
        for(int i = 0; i < SIZE; ++i){
            original_img.at<uchar>(j,i) = 0;
            original_img.at<uchar>(j,i) = Ycb[0].at<uchar>(j,i);
        }
    }

    imshow("original_img",original_img);

   //initialize container
    Mat compressed_img1(SIZE, SIZE, CV_8UC1);
    Mat compressed_img2(SIZE, SIZE, CV_8UC1);
    Mat compressed_img3(SIZE, SIZE, CV_8UC1);
    Mat AnswerImg1, AnswerImg2, AnswerImg3;
    original_img.convertTo(AnswerImg1, CV_64F);
    original_img.convertTo(AnswerImg2, CV_64F);
    original_img.convertTo(AnswerImg3, CV_64F);
    vector<Mat> AnswerImg, quant_m, result_img;
    for(int i = 0; i < 3; i++){
        AnswerImg.push_back(AnswerImg1);
        AnswerImg.push_back(AnswerImg2);
        AnswerImg.push_back(AnswerImg3);
        quant_m.push_back(quantization_mat1);
        quant_m.push_back(quantization_mat2);
        quant_m.push_back(quantization_mat3);
        result_img.push_back(result1);
        result_img.push_back(result2);
        result_img.push_back(result3);
    }
    

    for(int count = 0; count < 3; count++){
        for(int i = 0; i < SIZE; i+=8){
            for(int j = 0; j < SIZE; j+=8){
                //dct
                Mat block = AnswerImg[count](Rect(j, i, 8, 8));
                dct(block, block, 0);
                // divide(block,quant_m[count],block);
                for(int k = 0; k < SIZE/8; ++k){
                    for(int l = 0; l < SIZE/8; ++l){
                        result_img[count].at<double>(k,l) = round(block.at<double>(k,l) / quant_m[count].at<double>(k,l));
                        result_img[count].at<double>(k,l) = result_img[count].at<double>(k,l) * quant_m[count].at<double>(k,l);
                    }
                }
                //inverse_dct
                dct( result_img[count], block, 1);
            }
        }  
    } 

    
    // for(int k = 0; k < 3; ++k){
    //     int sum = 0;
    //     int max = 0;
    //     for(int i = 0; i < SIZE; ++i){
    //         for(int j = 0; j < SIZE; ++j){
    //         if(original_img.at<uchar>(i,j) > max){
    //             max = original_img.at<uchar>(i,j);
    //         }
    //         sum += pow((result_img[k].at<uchar>(i,j) - original_img.at<uchar>(i,j)),2);
    //         }
    //     }
    //     sum = sum / (SIZE * SIZE);
    //     double psnr = 20 * log10(max) - 10 * log10(sum);
    //     cout << "QM" << k + 1 <<": psnr = " << psnr << endl;
    
    // }

    

    imshow("com",result_img[0]);
    imshow("com1",result_img[1]);
    imshow("com2",result_img[2]);




    waitKey(0);





}
