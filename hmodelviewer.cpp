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
#include "hmodelviewer.h"

#include <GL/gl.h>
#include "hmodel.h"

HModelViewer::HModelViewer(){
}

HModelViewer::~HModelViewer(){
}

void	HModelViewer::prepare(HModel *model){
  _model=model;
}

void	HModelViewer::view(){
  glPushMatrix();

  for (int i = 0; i < _model->N(); i++)
  {
    for (int j = 0; j < _model->N(); j++)
    {
      for (int k = 0; k < _model->N(); k++)
      {
        long long index[3] = {i, j, k};
        if (_model->status(index) == 0)
        {
          continue;
        }
        float coords[3];
        _model->position(index, coords);
        glPushMatrix();
        glTranslatef(coords[0], coords[1], coords[2]);
        glScalef(_model->delta(), _model->delta(), _model->delta());
        drawCube();
        glPopMatrix();
      }
    }
  }
  glPopMatrix();
}

inline void	HModelViewer::drawCube(){
  static GLfloat vertices[]={
    0, 0, 0,
    1, 0, 0,
    1, 1, 0,
    0, 1, 0,
    0, 0, 1,
    1, 0, 1,
    1, 1, 1,
    0, 1, 1
  };
  char indices[]={
    0, 3, 2, 1, 2, 3, 7, 6,
    0, 4, 7, 3, 1, 2, 6, 5,
    4, 5, 6, 7, 0, 1, 5, 4
  };
  glPushMatrix();
  glEnable(GL_AUTO_NORMAL);
  glEnable(GL_VERTEX_ARRAY);
  glVertexPointer(3, GL_FLOAT, 0, vertices);
  glDrawElements(GL_QUADS, 24, GL_UNSIGNED_BYTE, indices);
  glDisable(GL_VERTEX_ARRAY);
  glPopMatrix();
}

void	HModelViewer::clear(){

}
