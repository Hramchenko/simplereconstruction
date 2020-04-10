/***************************************************************************
 *   Copyright (C) 2020 by Hramchenko   *
 *   hramchenko@bk.ru   *
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
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "hvirtualtrackball.h"

#include <math.h>

#include <QtOpenGL>

#define M_PI 3.14159265358979323846

HVirtualTrackball::HVirtualTrackball(QGLWidget* parent){
	parentWidget=parent;
	angle=0;
	scale=1;
	translateX=0;
	translateY=0;
	lastPos[0]=lastPos[1]=lastPos[2]=0;
	trackingMouse=false;
	redrawContinue=false;
	trackballMove=false;
	isScale=false;
	isTranslate=false;
}

HVirtualTrackball::~HVirtualTrackball(){
}

void	HVirtualTrackball::mouseMoveEvent(QMouseEvent* event){
	float cur_pos[3];
	float	dx, dy, dz;
	calculateVector(event->x(), event->y(), parentWidget->width(), parentWidget->height(), cur_pos);
	if(trackingMouse){
		dx=cur_pos[0]-lastPos[0];
		dy=cur_pos[1]-lastPos[1];
		dz=cur_pos[2]-lastPos[2];
		if(dx || dy || dz){
			angle=90.0*sqrt(dx*dx+dy*dy+dz*dz);
			axis[0]=lastPos[1]*cur_pos[2]-lastPos[2]*cur_pos[1];
			axis[1]=lastPos[2]*cur_pos[0]-lastPos[0]*cur_pos[2];
			axis[2]=lastPos[0]*cur_pos[1]-lastPos[1]*cur_pos[0];
			lastPos[0]=cur_pos[0];
			lastPos[1]=cur_pos[1];
			lastPos[2]=cur_pos[2];
		}
	}
	if(isTranslate){ 
		translateX=(event->x()-startX)*0.005;
		translateY=(startY-event->y())*0.005;
		startX=event->x();
		startY=event->y();
	}	
	if(isScale){
		scale=1+(event->y()-startY)*0.005;
		startY=event->y();
		redrawContinue=true;
	}
	redrawContinue=true;
	parentWidget->updateGL();
}

void	HVirtualTrackball::mousePressEvent(QMouseEvent* event){
	int x=event->x();
	int y=event->y();
	if(event->button()==Qt::LeftButton){ 
		y=parentWidget->height()-y;
		startMotion(x, y);
	}
	if(event->button()==Qt::RightButton){ 
		startX=x;
		startY=y;
		isTranslate=true;
	}
	if(event->button()==Qt::MidButton) { 
		startY=y;
		isScale=true;
	}
}

void	HVirtualTrackball::mouseReleaseEvent(QMouseEvent* event){
	int x=event->x();
	int y=event->y();
	if(event->button()==Qt::LeftButton) 
		stopMotion(x, y);
	if(event->button()==Qt::RightButton) 
				isTranslate=false;
	if(event->button()==Qt::MidButton) 
				isScale=false;
}

void	HVirtualTrackball::wheelEvent(QWheelEvent* event){
	if(event->delta()>0)
		scale=1+0.08;
	else
		scale=1-0.08;
	parentWidget->updateGL();
}

void	HVirtualTrackball::applyTransform(){
	glMatrixMode(GL_PROJECTION);
	glTranslatef(translateX, translateY, 0);
	translateX=translateY=0;
	glMatrixMode(GL_MODELVIEW);
	glScalef(scale, scale, scale);
	scale=1;
	if(trackballMove)
		glRotatef(angle, axis[0], axis[1], axis[2]);
}

void	HVirtualTrackball::startMotion(int x, int y){
	trackingMouse=true;
	redrawContinue=false;
	startX=x;
	startY=y;
	calculateVector(x, y, parentWidget->width(), parentWidget->height(), lastPos);
	trackballMove=true;
}

void	HVirtualTrackball::stopMotion(int x, int y){
	trackingMouse=false;
	if(startX!=x || startY!=y)
		redrawContinue=true;
	else{
		angle=0;
		redrawContinue=false;
	}
	trackballMove=false;
}

void	HVirtualTrackball::calculateVector(int x, int y, int width, int height, float* v){
	float d;
	float a;
	v[0]=(2.0*x-width)/width;
	v[1]=(height-2.0*y)/height;
	d=(float)sqrt(v[0]*v[0]+v[1]*v[1]);
	v[2]=cos((float)(M_PI/2.0)*((d<1.0)?d:1.0));
	a=1.0/(float)sqrt(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]);
	v[0]*=a;
	v[1]*=a;
	v[2]*=a; 
}


