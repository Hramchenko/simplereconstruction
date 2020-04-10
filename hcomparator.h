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
#ifndef HCOMPARATOR_H
#define HCOMPARATOR_H

#include <QtOpenGL>
#include <QGLWidget>
#include <QGLPixelBuffer>

class	HModel;
class	HFrame;

class HComparator{
public:
  HComparator(QGLWidget* parent, int max_width, int max_height);
  ~HComparator();
  char	compare(HModel* model, const long long *voxel, HFrame* frame);
private:
  void getBounds(int &min_x, int &min_y, int &max_x, int &max_y, int width, int height);
  void	initPixelBuffer();
  void	createVoxelList();
  void	render(HModel *model, const long long *voxel, HFrame* frame);
  char	compareData(HModel *model, const long long *voxel, HFrame* frame);
private:
  QGLPixelBuffer *pixelBuffer;
  long	voxelList;
  char*	currentData;
};

#endif
