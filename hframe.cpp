/***************************************************************************
 *   Copyright (C) 2020 by Hramchenko                                      *
 *   hramchenko@bk.ru                                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 ***************************************************************************/
#include "hframe.h"

#include <iostream>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/aruco.hpp>
#include <opencv2/calib3d.hpp>
#include <GL/gl.h>
#include <GL/glu.h>

using namespace std;
using namespace cv;

HFrame::HFrame(){
  data=0;
  width=height=0;
}

HFrame::HFrame(const HFrame& rhs){
  data=rhs.data;
  width=rhs.width;
  height=rhs.height;
}

HFrame::~HFrame(){
  delete[] data;
}

void	HFrame::setGL(){
  glMatrixMode(GL_PROJECTION);
  glViewport(0, 0, width, height);
  glLoadMatrixd((double*)intrisicParameters.data);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glLoadMatrixd((double*)modelviewMatrix.data);
}

bool HFrame::loadData(Mat img_bgra, Mat img_bgr, double focal_length){
  loadMask(img_bgra);
  focalLength = focal_length;
  loadIntrisicParameters(img_bgra, focal_length);
  auto arucoDict = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);
  auto grid = cv::aruco::GridBoard::create(6, 8, 0.04, 0.02, arucoDict);
  Mat gray;
  vector<Mat> board_corners;
  Mat board_ids;
  cvtColor(img_bgr, gray, COLOR_BGR2GRAY);
  cv::aruco::detectMarkers(gray, arucoDict, board_corners, board_ids);
  if (board_corners.empty())
  {
    return false;
  }
  double pixelsFocalLength = focalLengthToPixels(focalLength, img_bgra.rows);
  cameraMatrix = (Mat_<double>(3, 3)<< pixelsFocalLength, 0, img_bgra.cols*0.5, 0, pixelsFocalLength, img_bgra.rows*0.5, 0, 0, 1);

  cv::aruco::estimatePoseBoard(board_corners, board_ids, grid, cameraMatrix, Mat(), rvec, tvec);
  loadExtrisicParameters(rvec, tvec, modelviewMatrix);
  return true;
}

Mat HFrame::mask()
{
  return cv::Mat(height, width, CV_8UC1, data);
}

void HFrame::drawAxis(Mat &img)
{
  cv::aruco::drawAxis(img, cameraMatrix, Mat(), rvec, tvec, 0.1);
}

void HFrame::loadMask(const Mat &img__)
{
  Mat img_;
  int max_width = 2048;
  scale = 1;
  if (img__.cols > 2048)
  {
    scale = max_width/float(img__.cols);
    resize(img__, img_, Size(), scale, scale, INTER_NEAREST);
  }
  else
  {
    img_ = img__.clone();
  }

  Mat img;
  flip(img_, img, 0);

  Mat mask_channel;
  CV_Assert(img.channels() == 4);
  extractChannel(img, mask_channel, 3);
  mask_channel = 255 - mask_channel;

  width=img.cols;
  height=img.rows;
  data=new unsigned char[width*height];
  for (int j = 0; j < height; j++)
  {
    unsigned char *row = mask_channel.ptr<unsigned char>(j);
    for (int i = 0; i < width; i++)
    {
      unsigned char v = 0;
      if (row[i] > 127)
      {
        v  = 255;
      }
      data[j*width + i] = v;
    }
  }
}

void HFrame::loadIntrisicParameters(const Mat &img, double focal_length)
{
  // http://kgeorge.github.io/2014/03/08/calculating-opengl-perspective-matrix-from-opencv-intrinsic-matrix/
  Mat_<double> persp(4,4); persp.setTo(0);
  double f = focalLengthToPixels(focal_length, img.rows);
  double fx = f;
  double fy = f;
  double cx = img.cols/(double)2;
  double cy = img.rows/(double)2;
  persp(0,0) = fx/cx;
  persp(1,1) = fy/cy;
  double near = 0.01;
  double far = 1000;

  persp(2,2) = -(far+near)/(far-near);
  persp(2,3) = -2.0*far*near / (far-near);
  persp(3,2) = -1.0;

  persp = persp.t(); //to col-major for OpenGL

  intrisicParameters = persp.clone();
}

void HFrame::loadExtrisicParameters(const Mat &Rvec, const Mat &Tvec, Mat &modelview_matrix)
{
  // http://kgeorge.github.io/2014/03/08/calculating-opengl-perspective-matrix-from-opencv-intrinsic-matrix/
  CV_Assert(Rvec.rows == 3);
  CV_Assert(Tvec.rows == 3);
  Mat Rot(3,3,CV_32FC1);
  Rodrigues(Rvec, Rot);

  // [R | t] matrix
  Mat_<double> para = Mat_<double>::eye(4,4);
  Rot.convertTo(para(Rect(0,0,3,3)),CV_64F);
  Tvec.copyTo(para(Rect(3,0,1,3)));

  Mat cvToGl = Mat::zeros(4, 4, CV_64F);
  cvToGl.at<double>(0, 0) = 1.0f;
  cvToGl.at<double>(1, 1) = -1.0f; // Invert the y axis
  cvToGl.at<double>(2, 2) = -1.0f; // invert the z axis
  cvToGl.at<double>(3, 3) = 1.0f;

  para = cvToGl * para;

  Mat(para.t()).copyTo(modelview_matrix); // transpose to col-major for OpenGL
}
