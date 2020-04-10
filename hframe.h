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
#ifndef HFRAME_H
#define HFRAME_H

#include <string>
#include <opencv2/core.hpp>

using cv::Mat;

class HFrame{
public:
  HFrame();
  HFrame(const HFrame& rhs);
  virtual ~HFrame();
  virtual	void	setGL();
  bool loadData(Mat img_rgba, cv::Mat img_rgb, double focal_length);
  Mat mask();
  void drawAxis(Mat &img);
  void loadMask(const cv::Mat &img_);
  void loadIntrisicParameters(const cv::Mat &img, double focal_length);
  void loadExtrisicParameters(const Mat &rvec, const Mat &tvec, Mat &modelview_matrix);
  static double focalLengthToPixels(double f, int img_height)
  {
    return f*(img_height/(double)24.0);
  }

  std::string	name;
  unsigned char*	data;
  int	width;
  int	height;
  double focalLength;
  Mat intrisicParameters;
  Mat modelviewMatrix;
  Mat cameraMatrix;
  Mat rvec;
  Mat tvec;
  double scale;
};

#endif
