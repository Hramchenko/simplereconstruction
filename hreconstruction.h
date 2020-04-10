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
#ifndef HRECONSTRUCTION_H
#define HRECONSTRUCTION_H

/**
  @author Hramchenko <user@localhost.localdomain>
*/

#include <list>
#include <QString>
#include <QObject>

class QGLWidget;

class HFrame;
class	HVoxel;
class	HComparator;
class HModel;

class HReconstruction: public QObject{
public:
  HReconstruction(QGLWidget* parent);
  ~HReconstruction();
  void	prepare(HModel *model);
  bool	idle();
  bool	isReconstructionComplited();
private:
  void	process();
private:
  HModel *_model;
  long long _voxel[3];
  HComparator*	_comparator;
  QGLWidget* parentWidget;
  int	workFlag;
  int idleValue;
};

#endif
