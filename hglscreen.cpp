/***************************************************************************
 *   Copyright (C) 2009 by Hramchenko   *
 *   Hramchenko@bk.ru   *
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
#include "hglscreen.h"

#include <GL/glu.h>
#include <iostream>

#include <QAction>
#include <QApplication>
#include <QDialog>
#include <QMessageBox>

#include "hmodelviewer.h"
#include "hvirtualtrackball.h"
#include "hcomparator.h"
#include "hframe.h"
#include "hreconstruction.h"
#include "hmodel.h"

using namespace std;

HGLScreen::HGLScreen(QWidget* parent): QGLWidget(parent){
	virtualTrackball=new HVirtualTrackball(this);
	stop=true;
  modelViewer = new HModelViewer();
  reconstruction=new HReconstruction(this);
}

HGLScreen::~HGLScreen() {
  delete modelViewer;
	delete virtualTrackball;
	delete reconstruction;
}

void HGLScreen::setModel(HModel *m)
{
  model = m;
  modelViewer->prepare(m);
}

void HGLScreen::initializeGL(){
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_FOG);
	glEnable(GL_BLEND);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHTING);
	glShadeModel(GL_SMOOTH);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST );
	GLfloat material_specular[]={0.8, 0.8, 0.8, 0.5};
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, material_specular);
	GLfloat material_diffuse[]={0.5, 0.5, 1.0, 0.5};
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, material_diffuse);
	GLfloat material_emission[]={0.5, 0.5, 1.0, 0.5};
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, material_emission);
	glClearColor(0.0, 0.0, 0.0, 0.0);
}

void HGLScreen::resizeGL(int width, int height){
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, width, height);
	gluPerspective(20.0, (float)width/(float)height, 0.5, 10.0);
	gluLookAt(4.0, 0.0, 4.0, 0.0, 0.0, 0.0, 4.0, 4.0, 4.0);
	glMatrixMode(GL_MODELVIEW);
}

void HGLScreen::paintGL(){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	virtualTrackball->applyTransform();
	glPushMatrix();
	glRotatef(90, 0, 1, 0);
	glTranslatef(-0.5, -0.5, -0.5);
	modelViewer->view();
	glPopMatrix();
	glFlush();
	if(!stop){
		if(!reconstruction->isReconstructionComplited()){
			reconstruction->idle();
		}
		else
			stop=true;
	}
}

void	HGLScreen::startReconstructionST(){
	updateGL();
  int precision=QInputDialog::getInt(0, "Simple Reconstruction System", tr("Enter mesh nodes count:"), 30, 1, 300, 1);
  model->initMesh(precision);
  reconstruction->prepare(model);
	stop=false;
	while(!(reconstruction->isReconstructionComplited()||stop)){
		updateGL();
    QApplication::instance()->processEvents();
	}
  modelViewer->prepare(model);
	updateGL();
}

void	HGLScreen::stopReconstructionST(){
	stop=true;
	updateGL();
}

void	HGLScreen::mouseMoveEvent(QMouseEvent* event){
	virtualTrackball->mouseMoveEvent(event);
}

void	HGLScreen::mousePressEvent(QMouseEvent* event){
	virtualTrackball->mousePressEvent(event);
}

void	HGLScreen::mouseReleaseEvent(QMouseEvent* event){
	virtualTrackball->mouseReleaseEvent(event);
}

void	HGLScreen::wheelEvent(QWheelEvent* event){
	virtualTrackball->wheelEvent(event);
}

void HGLScreen::closeEvent(QCloseEvent *event)
{
  cout<<"Exit."<<endl;
  exit(0);
}
