////////////////////////////////////////////////////////
//
// GEM - Graphics Environment for Multimedia
//
// zmoelnig@iem.kug.ac.at
//
// Implementation file
//
//    Copyright (c) 1997-2000 Mark Danks.
//    Copyright (c) Günther Geiger.
//    Copyright (c) 2001-2002 IOhannes m zmoelnig. forum::für::umläute. IEM
//    Copyright (c) 2002 James Tittle & Chris Clepper
//    For information on usage and redistribution, and for a DISCLAIMER OF ALL
//    WARRANTIES, see the file, "GEM.LICENSE.TERMS" in this distribution.
//
/////////////////////////////////////////////////////////
// 
// based on code written by Lluis Gomez i Bigorda 
// lluisgomez _at_ hangar _dot_ org
// 
// Template for pix_opencv class

#include "pix_opencv_calchist.h"
#include <stdio.h>
#include <RTE/MessageCallbacks.h>


CPPEXTERN_NEW(pix_opencv_calchist)

/////////////////////////////////////////////////////////
//
// pix_opencv_calchist
//
/////////////////////////////////////////////////////////
// Constructor
//
/////////////////////////////////////////////////////////
pix_opencv_calchist :: pix_opencv_calchist():
  m_sigmaX(1.5),
  m_sigmaY(1.5),
  m_border(7)
{ 

}

/////////////////////////////////////////////////////////
// Destructor
//
/////////////////////////////////////////////////////////
pix_opencv_calchist :: ~pix_opencv_calchist()
{ 
}

/////////////////////////////////////////////////////////
// processImage
//
/////////////////////////////////////////////////////////     
void pix_opencv_calchist :: processGrayImage(imageStruct &image)
{ 
  cv::Mat frame, edges;
  cv::Size imgSize = cv::Size(image.xsize,image.ysize);
  int c = image.csize;

    
  frame = cv::Mat(imgSize,CV_8UC1,image.data,image.xsize*c);
  edges = frame;
 

  cv::GaussianBlur(edges, edges, cv::Size(0,0), m_sigmaX, m_sigmaY, m_border);
  // cv::Canny(edges, edges, 0, 30, 3);

  memcpy( image.data, edges.data, image.xsize*image.ysize*image.csize );


  // } else if ( image.csize == 3 ) {
  //   im = cv::Mat( image.ysize, image.xsize, CV_8UC3, image.data, image.csize*image.xsize); // just transform imageStruct to IplImage without copying data
  //   cv::cvtColor(im,gray,CV_RGB2GRAY);
  // } else if ( image.csize == 4 ) {
  //   im = cv::Mat( image.ysize, image.xsize, CV_8UC4, image.data, image.csize*image.xsize); // just transform imageStruct to IplImage without copying data
  //   cv::cvtColor(im,gray,CV_RGBA2GRAY);
  // } else { 
  //   error("pix_opencv_calchist : unsupported video format %d",image.csize);
  //   return;
  // }


    // // Quantize the hue to 30 levels
    // // and the saturation to 32 levels
    // int hbins = 64, sbins = 64;
    // int histSize[] = {hbins, sbins};
    // // hue varies from 0 to 179, see cvtColor
    // float hranges[] = { 0, 180 };
    // // saturation varies from 0 (black-gray-white) to
    // // 255 (pure spectrum color)
    // float sranges[] = { 0, 256 };
    // const float* ranges[] = { hranges, sranges };

    // // cv::MatND hist;
    // // // we compute the histogram from the 0-th and 1-st channels
    // // int channels[] = {0};
    // // cv::calcHist( &gray, 1, channels, cv::Mat(), // do not use mask
    // //          hist, 2, histSize, ranges,
    // //          true, // the histogram is uniform
    // //          false );
    // // double maxVal=0;
    // // cv::minMaxLoc(hist, 0, &maxVal, 0, 0);
    // int scale = 10;

    // cv::Mat histImg = cv::Mat::zeros(sbins*scale, hbins*10, CV_8UC3);


    // for( int h = 0; h < hbins; h++ )
    //     for( int s = 0; s < sbins; s++ )
    //     {
    //         float binVal = hist.at<float>(h, s);
    //         int intensity = cvRound(binVal*255/maxVal);

    //         cv::rectangle( histImg, cv::Point(h*scale, s*scale),
    //                     cv::Point( (h+1)*scale - 1, (s+1)*scale - 1),
    //                     cv::Scalar::all(intensity),
    //                     -1 );
    //     }
    // cv::resize(histImg,im,cv::Size(image.xsize,image.ysize),0,0,cv::INTER_CUBIC);
    // image=im;
    // cv::rectangle(im,histImg);
    // 
    // image.data = (unsigned char*)(gray.data);
    // if (gray.data)

}
void pix_opencv_calchist :: sigmaXMess( double f )
{
  //  Gaussian kernel standard deviation in X direction.
  m_sigmaX = f;
}
void pix_opencv_calchist :: sigmaYMess( double f )
{
/*!
 * Gaussian kernel standard deviation in Y direction; if sigmaY is zero, it is 
 * set to be equal to sigmaX, if both sigmas are zeros, they are computed from
 * ksize.width and ksize.height, respectively (see getGaussianKernel for
 * details); to fully control the result regardless of possible future 
 * modifications of all this semantics, it is recommended to specify all of
 * ksize, sigmaX, and sigmaY.
*/
  m_sigmaX = f;
}
void pix_opencv_calchist :: borderMess( int f ) 
{
  m_border = f>0?f:0;
}
/////////////////////////////////////////////////////////
// static member function
//
/////////////////////////////////////////////////////////
void pix_opencv_calchist :: obj_setupCallback(t_class *classPtr)
{
  CPPEXTERN_MSG1(classPtr, "sigmaX", sigmaXMess, double);
  CPPEXTERN_MSG1(classPtr, "sigmaY", sigmaYMess, double);
  CPPEXTERN_MSG1(classPtr, "border", borderMess, int);
}
