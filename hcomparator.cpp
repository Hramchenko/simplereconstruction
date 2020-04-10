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
#include "hcomparator.h"

#include <string.h>
#include <cstdlib>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>
#include <opencv2/imgcodecs.hpp>
#include <iostream>
#include "hmodel.h"
#include "hframe.h"

using namespace cv;
using namespace std;

HComparator::HComparator(QGLWidget* parent, int max_width, int max_height){
  pixelBuffer=new QGLPixelBuffer(QSize(max_width, max_height), parent->format(), parent);
  initPixelBuffer();
  currentData=0;
  currentData=new char[max_width*max_height];
}

HComparator::~HComparator(){
  pixelBuffer->makeCurrent();
  glDeleteLists(voxelList, 1);
  delete pixelBuffer;
  delete[] currentData;
}

char	HComparator::compare(HModel *model, const long long *voxel, HFrame* frame){
  if(!(voxel && frame))
    return -1;

  render(model, voxel, frame);
  return compareData(model, voxel, frame);
}

void	HComparator::initPixelBuffer(){
  pixelBuffer->makeCurrent();
  glDisable(GL_CULL_FACE);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_LIGHTING);
  glDisable(GL_BLEND);
  glEnable(GL_VERTEX_ARRAY);

  glClearColor(1, 1, 1, 1);
  glColor3f(0, 0, 0);
  createVoxelList();
}

void	HComparator::createVoxelList(){
  float eps = 1e-4;
  static GLdouble vertices[]={
    0, 0, 0, 1, 0, 0,
    1, 1, 0, 0, 1, 0,
    0, 0, 1, 1, 0, 1,
    1, 1, 1, 0, 1, 1
  };
  for (int i = 0; i < 24; i++)
  {
    if (vertices[i] == 0)
    {
      vertices[i] -= eps;
    }
    if (vertices[i] == 1)
    {
      vertices[i] += eps;
    }
  }
  static GLubyte indices[]={
    0, 3, 2, 1, 2, 3, 7, 6,
    0, 4, 7, 3, 1, 2, 6, 5,
    4, 5, 6, 7, 0, 1, 5, 4
  };
  voxelList=glGenLists(1);
  glVertexPointer(3, GL_DOUBLE, 0, vertices);
  glMatrixMode(GL_MODELVIEW);
  glNewList(voxelList, GL_COMPILE);
  glDrawElements(GL_QUADS, 24, GL_UNSIGNED_BYTE, indices);
  glEndList();
}

inline void	HComparator::render(HModel *model, const long long *voxel, HFrame *frame){
  pixelBuffer->makeCurrent();
  glLoadIdentity();
  glClear(GL_COLOR_BUFFER_BIT);
  frame->setGL();
  float coords[3];
  model->position(voxel, coords);
  glTranslatef(coords[0], coords[1], coords[2]);
  double s = model->delta();
  glScalef(s, s, s);
  glCallList(voxelList);
  glFlush();
}

static inline float clip_val(float v, float a, float b)
{
  if (v < a)
  {
    return a;
  }
  if (v > b)
  {
    return b;
  }
  return v;
}

void	HComparator::getBounds(int& min_x, int& min_y, int& max_x, int& max_y, int width, int height){
  Mat mv(4, 4, CV_32F);
  Mat prj(4, 4, CV_32F);
  Mat pts = (Mat_<float>(8, 4)<<
                                     0, 0, 0, 1,
                                     0, 0, 1, 1,
                                     0, 1, 0, 1,
                                     0, 1, 1, 1,
                                     1, 0, 0, 1,
                                     1, 0, 1, 1,
                                     1, 1, 0, 1,
                                     1, 1, 1, 1);

  glGetFloatv(GL_MODELVIEW_MATRIX, &mv.at<float>(0, 0));
  glGetFloatv(GL_PROJECTION_MATRIX, &prj.at<float>(0, 0));
  mv = mv.t();
  prj = prj.t();
  Mat m = prj*mv;
  Mat new_pts = (m*pts.t()).t();
  for (int i = 0; i < new_pts.rows; i++)
  {
    float *row = new_pts.ptr<float>(i);
    for (int j = 0; j < 4; j++)
    {
      row[j] /= row[3] + 1e-9;
    }
    row[0] = (row[0] + 1)/2*width;
    row[1] = (row[1] + 1)/2*height;
  }
  Mat _min, _max;
  cv::reduce(new_pts, _min, 0, cv::REDUCE_MIN);
  cv::reduce(new_pts, _max, 0, cv::REDUCE_MAX);
  min_x = clip_val(_min.at<float>(0) - 1, 0, width);
  max_x = clip_val(_max.at<float>(0) + 1, min_x, width);

  min_y = clip_val(_min.at<float>(1) - 1, 0, height);
  max_y = clip_val(_max.at<float>(1) + 1, min_y, height);
}

char	HComparator::compareData(HModel *model, const long long *voxel, HFrame* frame){
  int min_x, min_y, max_x, max_y;
  long int width, _width, height;
  auto image=frame->data;
  getBounds(min_x, min_y, max_x, max_y, frame->width, frame->height);
  width=max_x-min_x;
  height=max_y-min_y;
  if ((width == 0) || (height == 0))
  {
    return 4;
  }
  _width=width;
  if(_width%4)
    _width+=4-_width%4;
  glReadPixels(min_x, min_y, width, height, GL_RED, GL_UNSIGNED_BYTE, currentData);
  char result=4;
  for(int j=0; j<height; j++){
    auto data_ptr = &currentData[j*_width];
    auto image_ptr = &image[(min_y + j)*frame->width + min_x];
    for(auto i=0; i<width; i++){
      if(data_ptr[i] == 0){
        if(image_ptr[i] != 0){
          if(result==1)
            return 2;
          result=0;
        }
        else{
          if(result==0)
            return 2;
          result=1;
        }
      }
    }
  }
  return result;
}
