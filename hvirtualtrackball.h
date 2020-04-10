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
#ifndef HVIRTUALTRACKBALL_H
#define HVIRTUALTRACKBALL_H

/**
	@author Hramchenko <hramchenko@bk.ru>
*/

class QMouseEvent;
class QGLWidget;
class QWheelEvent;

class HVirtualTrackball{
	public:
    HVirtualTrackball(QGLWidget* parent);
    virtual ~HVirtualTrackball();
		void	applyTransform();
	public:
		void	mouseMoveEvent(QMouseEvent* event);
		void	mousePressEvent(QMouseEvent* event);
		void	mouseReleaseEvent(QMouseEvent* event);
		void	wheelEvent(QWheelEvent* event);
	private:	
		void	startMotion(int x, int y);
		void	stopMotion(int x, int y);
		static void	calculateVector(int x, int y, int width, int height, float* v);
	private:
		QGLWidget*	parentWidget;
		float	scale;
		float	translateX;
		float translateY;
		float	angle;
		float	axis[3];
		float	lastPos[3];
		int startX;
		int startY;
		bool	trackingMouse;
		bool	redrawContinue;
		bool	trackballMove;
		bool	isScale;
		bool	isTranslate;
};

#endif
