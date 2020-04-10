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

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <string.h>
#include <math.h>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <QApplication>

#include "hglscreen.h"
#include "hframe.h"
#include "hmodel.h"

using namespace std;
using namespace cv;

void	sigsegv_handler(int a){
  cout<<"Program crashed."<<endl;
	exit(0);
}

void print_usage_and_exit(char **argv)
{
  cout<<"usage: "<<argv[0]<<" <focal_length(int)> <path_to_images_list>"<<endl;
  exit(0);
}

inline std::string strip(const std::string &s)
{
   auto  wsfront=std::find_if_not(s.begin(),s.end(),[](int c){return std::isspace(c);});
   return std::string(wsfront,std::find_if_not(s.rbegin(),std::string::const_reverse_iterator(wsfront),[](int c){return std::isspace(c);}).base());
}

int	main(int argc, char* argv[]){
  if (argc != 3)
  {
    print_usage_and_exit(argv);
  }

  auto application=new QApplication(argc, argv);

  if (!QGLFormat::hasOpenGL() || !QGLPixelBuffer::hasOpenGLPbuffers()) {
    cout<<"This system does not support OpenGL/pbuffers."<<endl;
    return -1;
  }

  struct sigaction sa;
  memset(&sa, 0, sizeof sa);
  sa.sa_handler=&sigsegv_handler;
  sigaction(SIGSEGV, &sa, NULL);

  double focal_length = stod(argv[1]);

  ifstream f(argv[2]);

  vector<Mat> images;
  HModel model;
  vector<HFrame*> &frames = model.frames();
  while (1)
  {
    if (f.eof() || f.fail())
    {
      break;
    }    
    string line;
    std::getline(f, line);
    string f_name = strip(line);
    if (f_name.empty())
    {
      continue;
    }
    cout<<"Loading "<<f_name<<endl;
    Mat img = imread(f_name, -1);
    if (img.empty())
    {
      cout<<f_name<<" not found."<<endl;
      exit(0);
    }
    string jpeg_file = f_name.substr(0, f_name.size() - 4) + ".jpg";
    Mat img_bgr = imread(jpeg_file, 1);
    if (img_bgr.empty())
    {
      cout<<jpeg_file<<" not found."<<endl;
      exit(0);
    }
    HFrame *frame = new HFrame;
    if (!frame->loadData(img, img_bgr, focal_length))
    {
      cout<<"Error while loading " + f_name<<endl;
      exit(0);
    }
    string maskFile = f_name.substr(0, f_name.size() - 4) + ".mask.png";
    auto mask = frame->mask();
    imwrite(maskFile, mask);

    string axisFile = f_name.substr(0, f_name.size() - 4) + ".axis.jpg";
    Mat vis = img_bgr.clone();
    frame->drawAxis(vis);
    vector<int> encode_param = {int(IMWRITE_JPEG_QUALITY), 90};
    imwrite(axisFile, vis, encode_param);
    frames.push_back(frame);
  }

  cout<<"Images loading finished."<<endl;

  auto window=new HGLScreen(0);

  window->resize(1024, 768);
  window->setModel(&model);
  window->show();
  window->startReconstructionST();
  return application->exec();
}
