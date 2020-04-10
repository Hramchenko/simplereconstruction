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
#include "hreconstruction.h"

#include "hframe.h"
#include "hcomparator.h"
#include "hmodel.h"

HReconstruction::HReconstruction(QGLWidget* parent): QObject(){
	parentWidget=parent;
	workFlag=0;
  idleValue=30;
  _comparator=0;
}

HReconstruction::~HReconstruction(){
  delete _comparator;
}

void	HReconstruction::prepare(HModel *model){
  _model = model;
	int width=0;
	int height=0;
  for (auto &current_frame: _model->frames()){
    if(current_frame->width > width)
      width=current_frame->width;
    if(current_frame->height > height)
      height=current_frame->height;
	}
  _comparator=new HComparator(parentWidget, width, height);
	workFlag=1;
  _voxel[0] = _voxel[1] = _voxel[2] = 0;
}

bool	HReconstruction::isReconstructionComplited(){
	return !workFlag;
}

bool	HReconstruction::idle(){
	if(!workFlag)
		return false;
	process();
	return true;
}

void	HReconstruction::process(){
  int idle_counter = 0;
  _voxel[0] = _voxel[0]%_model->N();
  _voxel[1] = _voxel[1]%_model->N();
  _voxel[2] = _voxel[2]%_model->N();

  for (; _voxel[0] < _model->N(); _voxel[0]++)
  {
    for (; _voxel[1] < _model->N(); _voxel[1]++)
    {
      for (; _voxel[2] < _model->N(); _voxel[2]++)
      {
        char voxel_type = 1;
        for (auto &frame: _model->frames()){
          char t=_comparator->compare(_model, _voxel, frame);
          if ((t == 0) || (t == 4))
          {
            voxel_type = 0;
            break;
          }
        }
        _model->setStatus(_voxel, voxel_type);
        if(idle_counter++>idleValue)
          return;
      }
      _voxel[2] = 0;
    }
    _voxel[1] = 0;
  }
  workFlag=0;
}
