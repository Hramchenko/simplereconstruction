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
#ifndef HMODEL_H
#define HMODEL_H

#include <QString>
#include <QObject>
#include <vector>

using std::vector;

class HVoxel;
class HFrame;

class HModel: public QObject{
public:
  HModel();
  ~HModel();
  void	clearModel();

  vector<HFrame*> &frames(){
    return _frames;
  }

  void initMesh(int n)
  {
    _N = n;
    _N2 = _N*_N;
    _delta = 0.5/_N;
    _data = vector<char>(_N*_N*_N, 3);
  }

  char status(long long *index) const
  {
    return _data[index[2]*_N2 + index[1]*_N + index[0]];
  }

  void setStatus(long long *index, const char &s)
  {
    _data[index[2]*_N2 + index[1]*_N + index[0]] = s;
  }

  void position(const long long *index, float *coords) const
  {
    coords[0] = index[0]*_delta;
    coords[1] = index[1]*_delta;
    coords[2] = index[2]*_delta;
  }

  long long N() const
  {
    return _N;
  }

  float delta() const
  {
    return _delta;
  }

private:
  vector<HFrame*> _frames;
  vector<char> _data = {char(1)};
  float _delta = 0.5;
  long long _N = 1;
  long long _N2 = 1;
};

#endif
