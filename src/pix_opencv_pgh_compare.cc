////////////////////////////////////////////////////////
//
// GEM - Graphics Environment for Multimedia
//
// zmoelnig@iem.kug.ac.at
//
// Implementation file
//
//    Copyright (c) 1997-1998 Mark Danks.
//    Copyright (c) Günther Geiger.
//    Copyright (c) 2001-2002 IOhannes m zmoelnig. forum::für::umläute. IEM
//    Copyright (c) 2002 James Tittle & Chris Clepper
//    For information on usage and redistribution, and for a DISCLAIMER OF ALL
//    WARRANTIES, see the file, "GEM.LICENSE.TERMS" in this distribution.
//
/////////////////////////////////////////////////////////

#include "pix_opencv_pgh_compare.h"

CPPEXTERN_NEW_WITH_GIMME(pix_opencv_pgh_compare)

/////////////////////////////////////////////////////////
//
// pix_opencv_pgh_compare
//
/////////////////////////////////////////////////////////
// Constructor
//
/////////////////////////////////////////////////////////
pix_opencv_pgh_compare :: pix_opencv_pgh_compare(int argc, t_atom*argv)
{ 
    m_dataout = outlet_new(this->x_obj, &s_anything);
    m_posout = outlet_new(this->x_obj, &s_anything);

    comp_xsize=320;
    comp_ysize=240;

    // rgba = cvCreateImage(cvSize(comp_xsize,comp_ysize), IPL_DEPTH_8U, 4);
    // rgb = cvCreateImage(cvSize(comp_xsize,comp_ysize), IPL_DEPTH_8U, 3);
    // gray = cvCreateImage(cvSize(comp_xsize,comp_ysize), IPL_DEPTH_8U, 1);
    // rgbar = cvCreateImage(cvSize(comp_xsize,comp_ysize), IPL_DEPTH_8U, 4);
    // rgbr = cvCreateImage(cvSize(comp_xsize,comp_ysize), IPL_DEPTH_8U, 3);
    // grayr = cvCreateImage(cvSize(comp_xsize,comp_ysize), IPL_DEPTH_8U, 1);

    // x_storage = cvCreateMemStorage(0);

    x_bcontourr = -1;
    x_minsize = 10*10;
    x_cdistance = 0.05;
}

/////////////////////////////////////////////////////////
// Destructor
//
/////////////////////////////////////////////////////////
pix_opencv_pgh_compare :: ~pix_opencv_pgh_compare()
{ 
    // cvReleaseImage(&rgba);
    // cvReleaseImage(&rgb);
    // cvReleaseImage(&gray);
    // cvReleaseImage(&rgbar);
    // cvReleaseImage(&rgbr);
    // cvReleaseImage(&grayr);
}
#if 0
/////////////////////////////////////////////////////////
// processDualImage
//
/////////////////////////////////////////////////////////
void pix_opencv_pgh_compare :: processRGBA_RGBA(imageStruct &left, imageStruct &right)
{
 double dist = 100.0, ndist;
 int i = 0;                   // Indicator of cycles.
 CvSeq *contourl=NULL, *contourlp;
 CvRect rect;
 CvMemStorage *mstorage;
 CvSeq  *contourr = NULL;
 int    size;
 int dims[] = {8, 8};
 float range[] = {-180, 180, -100, 100};
 float *ranges[] = {&range[0], &range[2]};
 CvHistogram *histl, *histr ;

  if ((left.xsize!=right.xsize) || (left.ysize!=right.ysize) )
  {
    post( "pix_opencv_pgh_compare : left and right image are not of the same size" );
    return;
  }

  if ((this->comp_xsize!=left.xsize)&&(this->comp_ysize!=left.ysize))
  {
    this->comp_xsize=left.xsize;
    this->comp_ysize=left.ysize;

    cvReleaseImage(&rgba);
    cvReleaseImage(&rgb);
    cvReleaseImage(&gray);
    cvReleaseImage(&rgbar);
    cvReleaseImage(&rgbr);
    cvReleaseImage(&grayr);

    rgba = cvCreateImage(cvSize(comp_xsize,comp_ysize), IPL_DEPTH_8U, 4);
    rgb = cvCreateImage(cvSize(comp_xsize,comp_ysize), IPL_DEPTH_8U, 3);
    gray = cvCreateImage(cvSize(comp_xsize,comp_ysize), IPL_DEPTH_8U, 1);
    rgbar = cvCreateImage(cvSize(comp_xsize,comp_ysize), IPL_DEPTH_8U, 4);
    rgbr = cvCreateImage(cvSize(comp_xsize,comp_ysize), IPL_DEPTH_8U, 3);
    grayr = cvCreateImage(cvSize(comp_xsize,comp_ysize), IPL_DEPTH_8U, 1);
  }

  memcpy( rgbar->imageData, right.data, right.xsize*right.ysize*4 );
  cvCvtColor(rgbar, grayr, CV_BGRA2GRAY);

  // calculate the biggest contour
  try {
      cvFindContours( grayr, x_storage, &contourr, sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0) );
  }
  catch(...) {
      post( "pix_opencv_pgh_compare : error calculating contours" );
      return;
  }

  if ( contourr )
  {
    size=0;
    for( ; contourr != 0; contourr = contourr->h_next )
    {
       rect = cvBoundingRect( contourr, 1);
       if ( rect.width*rect.height > size && rect.width*rect.height < (comp_xsize-2)*(comp_ysize-2))
       {
          x_bcontourr = contourr;
          size = rect.width*rect.height;
       }
    }
  }

  memcpy( rgba->imageData, left.data, left.xsize*left.ysize*4 );
  cvCvtColor(rgba, gray, CV_BGRA2GRAY);

  mstorage = cvCreateMemStorage(0);

  cvFindContours( gray, mstorage, &contourl, sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0) );

  i=0;
  if ( contourl && x_bcontourr )
  {
    contourlp=contourl;
    for( ; contourlp != 0; contourlp = contourlp->h_next )
    {
      rect = cvBoundingRect( contourlp, 1);
      if ( rect.width*rect.height > x_minsize && rect.width*rect.height < (comp_xsize-2)*(comp_ysize-2))
      {
         histr = cvCreateHist(2, dims, CV_HIST_ARRAY, ranges, 1);
         histl = cvCreateHist(2, dims, CV_HIST_ARRAY, ranges, 1);
         cvCalcPGH(x_bcontourr, histr);
         cvCalcPGH(contourlp, histl);
         cvNormalizeHist(histr, 100.0f);
         cvNormalizeHist(histl, 100.0f);
         ndist = cvCompareHist(histr, histl, CV_COMP_BHATTACHARYYA);
         cvReleaseHist(&histr);
         cvReleaseHist(&histl);

         if ( ndist < dist ) dist = ndist;
         if ( ndist < x_cdistance )
         {
           cvRectangle( gray, cvPoint(rect.x,rect.y), cvPoint(rect.x+rect.width,rect.y+rect.height), CV_RGB(255,255,255), 2, 8 , 0 );
           cvDrawContours( gray, contourlp, CV_RGB(255,255,255), CV_RGB(255,255,255), 0, 1, 8, cvPoint(0,0) );
           SETFLOAT(&rlist[0], i++);
           SETFLOAT(&rlist[1], rect.x);
           SETFLOAT(&rlist[2], rect.y);
           SETFLOAT(&rlist[3], rect.width);
           SETFLOAT(&rlist[4], rect.height);
           outlet_list( m_posout, 0, 5, rlist );
         }
         else
         {
           cvRectangle( gray, cvPoint(rect.x,rect.y), cvPoint(rect.x+rect.width,rect.y+rect.height), CV_RGB(128,128,128), 2, 8 , 0 );
           cvDrawContours( gray, contourlp, CV_RGB(128,128,128), CV_RGB(128,128,128), 0, 1, 8, cvPoint(0,0) );
         }
      }
    }
  }

  if ( dist < 100.00 ) outlet_float( m_dataout, dist );

  cvReleaseMemStorage(&mstorage);

  cvCvtColor(gray, rgba, CV_GRAY2BGR);
  memcpy( left.data, rgba->imageData, left.xsize*left.ysize*4 );

}

/////////////////////////////////////////////////////////
// processDualImage
//
/////////////////////////////////////////////////////////
void pix_opencv_pgh_compare :: processRGB_RGB(imageStruct &left, imageStruct &right)
{
 double dist = 100.0, ndist;
 int i = 0;                   // Indicator of cycles.
 CvSeq *contourl=NULL, *contourlp;
 CvRect rect;
 CvMemStorage *mstorage;
 CvSeq  *contourr = NULL;
 int    size;
 int dims[] = {8, 8};
 float range[] = {-180, 180, -100, 100};
 float *ranges[] = {&range[0], &range[2]};
 CvHistogram *histl, *histr ;

  if ((left.xsize!=right.xsize) || (left.ysize!=right.ysize) )
  {
    post( "pix_opencv_pgh_compare : left and right image are not of the same size" );
    return;
  }

  if ((this->comp_xsize!=left.xsize)&&(this->comp_ysize!=left.ysize))
  {
    this->comp_xsize=left.xsize;
    this->comp_ysize=left.ysize;

    cvReleaseImage(&rgba);
    cvReleaseImage(&rgb);
    cvReleaseImage(&gray);
    cvReleaseImage(&rgbar);
    cvReleaseImage(&rgbr);
    cvReleaseImage(&grayr);

    rgba = cvCreateImage(cvSize(comp_xsize,comp_ysize), IPL_DEPTH_8U, 4);
    rgb = cvCreateImage(cvSize(comp_xsize,comp_ysize), IPL_DEPTH_8U, 3);
    gray = cvCreateImage(cvSize(comp_xsize,comp_ysize), IPL_DEPTH_8U, 1);
    rgbar = cvCreateImage(cvSize(comp_xsize,comp_ysize), IPL_DEPTH_8U, 4);
    rgbr = cvCreateImage(cvSize(comp_xsize,comp_ysize), IPL_DEPTH_8U, 3);
    grayr = cvCreateImage(cvSize(comp_xsize,comp_ysize), IPL_DEPTH_8U, 1);
  }

  memcpy( rgbr->imageData, right.data, right.xsize*right.ysize*3 );
  cvCvtColor(rgbr, grayr, CV_BGRA2GRAY);

  // calculate the biggest contour
  try {
      cvFindContours( grayr, x_storage, &contourr, sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0) );
  }
  catch(...) {
      post( "pix_opencv_pgh_compare : error calculating contours" );
      return;
  }

  if ( contourr )
  {
    size=0;
    for( ; contourr != 0; contourr = contourr->h_next )
    {
       rect = cvBoundingRect( contourr, 1);
       if ( rect.width*rect.height > size && rect.width*rect.height < (comp_xsize-2)*(comp_ysize-2))
       {
          x_bcontourr = contourr;
          size = rect.width*rect.height;
       }
    }
  }

  memcpy( rgb->imageData, left.data, left.xsize*left.ysize*3 );
  cvCvtColor(rgb, gray, CV_BGRA2GRAY);

  mstorage = cvCreateMemStorage(0);

  cvFindContours( gray, mstorage, &contourl, sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0) );

  if ( contourl && x_bcontourr )
  {
    contourlp=contourl;
    for( ; contourlp != 0; contourlp = contourlp->h_next )
    {
      rect = cvBoundingRect( contourlp, 1);
      if ( rect.width*rect.height > x_minsize && rect.width*rect.height < (comp_xsize-2)*(comp_ysize-2))
      {
         histr = cvCreateHist(2, dims, CV_HIST_ARRAY, ranges, 1);
         histl = cvCreateHist(2, dims, CV_HIST_ARRAY, ranges, 1);
         cvCalcPGH(x_bcontourr, histr);
         cvCalcPGH(contourlp, histl);
         cvNormalizeHist(histr, 100.0f);
         cvNormalizeHist(histl, 100.0f);
         ndist = cvCompareHist(histr, histl, CV_COMP_BHATTACHARYYA);
         cvReleaseHist(&histr);
         cvReleaseHist(&histl);

         if ( ndist < dist ) dist = ndist;
         if ( ndist < x_cdistance )
         {
           cvRectangle( gray, cvPoint(rect.x,rect.y), cvPoint(rect.x+rect.width,rect.y+rect.height), CV_RGB(255,255,255), 2, 8 , 0 );
           cvDrawContours( gray, contourlp, CV_RGB(255,255,255), CV_RGB(255,255,255), 0, 1, 8, cvPoint(0,0) );
         }
         else
         {
           cvRectangle( gray, cvPoint(rect.x,rect.y), cvPoint(rect.x+rect.width,rect.y+rect.height), CV_RGB(128,128,128), 2, 8 , 0 );
           cvDrawContours( gray, contourlp, CV_RGB(128,128,128), CV_RGB(128,128,128), 0, 1, 8, cvPoint(0,0) );
         }
      }
    }
  }

  if ( dist < 100.00 ) outlet_float( m_dataout, dist );

  cvReleaseMemStorage(&mstorage);

  cvCvtColor(gray, rgb, CV_GRAY2BGR);
  memcpy( left.data, rgb->imageData, left.xsize*left.ysize*3 );

}
#endif
/////////////////////////////////////////////////////////
// processDualImage
//
/////////////////////////////////////////////////////////
void pix_opencv_pgh_compare :: processDualImage(imageStruct &left, imageStruct &right)
{
  double dist = 100.0, ndist;
  int i = 0;                   // Indicator of cycles.
  std::vector<cv::Point> contourl, contourr;
  // cv::Mat<std::Mat<cv::Point> > contourl, contourr;
  cv::Rect rect;
  int size;
  int dims = 1;
  float hranges[] = { 0, 180 };
  float sranges[] = { 0, 256 };
  
  const float* ranges[] = { hranges, sranges };




  cv::Mat histl, histr ;
  // Quantize the hue to 30 levels
  // and the saturation to 32 levels
  int hbins = 30, sbins = 32;
  int histSize[] = {hbins, sbins};
  // we compute the histogram from the 0-th and 1-st channels
  int channels[] = {0, 1};
  
  if ( left.xsize < 0  || left.ysize < 0 || 
       right.xsize < 0 || right.ysize < 0   ) return;

  if ((left.xsize!=right.xsize) || (left.ysize!=right.ysize) )
  {
    post( "pix_opencv_pgh_compare : left and right image are not of the same size" );
    return;
  }
    
  cv::Mat imgMat2l, inputl;
  cv::Mat imgMat2r, inputr;
  std::vector<cv::Mat> split_arrayl, split_arrayr;
      
  if ( left.csize == 1 )
  {
    // just transform leftStruct to cv::Mat without copying data
    imgMat2l = cv::Mat( left.ysize, left.xsize, CV_8UC1, 
    left.data, left.csize*left.xsize); 
    inputl = imgMat2l;
  } else if ( left.csize == 4 )
  {
    // just transform leftStruct to cv::Mat without copying data
    imgMat2l = cv::Mat( left.ysize, left.xsize, CV_8UC4,
    left.data, left.csize*left.xsize); 
    cv::split(imgMat2l,split_arrayl);
    inputl = split_arrayl[3]; // select alpha channel to find contours
  } else {
    error("left image: suport only RGBA or GRAY left");
    return;
  }
  if ( right.csize == 1 )
  {
    // just transform rightStruct to cv::Mat without copying data
    imgMat2r = cv::Mat( right.ysize, right.xsize, CV_8UC1, 
    right.data, right.csize*right.xsize); 
    inputr = imgMat2r;
  } else if ( right.csize == 4 )
  {
    // just transform rightStruct to cv::Mat without copying data
    imgMat2r = cv::Mat( right.ysize, right.xsize, CV_8UC4,
    right.data, right.csize*right.xsize); 
    cv::split(imgMat2r,split_arrayr);
    inputr = split_arrayr[3]; // select alpha channel to find contours
  } else {
    error("right image: suport only RGBA or GRAY right");
    return;
  }
  // copy data because findContours will destroy it...
  cv::Mat imgMatl = inputl.clone();
  cv::Mat imgMatr = inputr.clone();

  // calculate the biggest contour
  try {
    cv::findContours(imgMatr, contourr, cv::Mat(),
    CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE);
  }
  catch(...) {
      post( "pix_opencv_pgh_compare : error calculating contours" );
      return;
  }


  if ( contourr.size() )
  {
    size=0;
    for( size_t i = 0 ; i < contourr.size() ; i++ )
    {
      rect = cv::boundingRect(cv::Mat(contourr[i]));
      if ( rect.width*rect.height > size && 
           rect.width*rect.height < (comp_xsize-2)*(comp_ysize-2) )
      {
        x_bcontourr = i;
        size = rect.width*rect.height;
      }
    }
  }

  cv::findContours(imgMatl, contourl, cv::Mat(),
    CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE);

  if ( contourl.size() && x_bcontourr!=-1 )
  {
    cv::Mat xbcont = cv::Mat(contourr[x_bcontourr]);
    for( size_t i = 0 ; i < contourl.size() ; i++ )
    {
      cv::Mat lcont = cv::Mat(contourl[i]);
      rect = cv::boundingRect(lcont);
      if ( rect.width*rect.height > x_minsize && 
           rect.width*rect.height < (comp_xsize-2)*(comp_ysize-2) )
      {
        cv::calcHist(&xbcont, 1, channels, cv::Mat(), 
          histr, dims, histSize, ranges);
        cv::calcHist(&lcont, 1, channels, cv::Mat(), 
          histl, dims, histSize, ranges);
        cv::normalize(histr, histr, 0, 100.f, 32);
        cv::normalize(histl, histl, 0, 100.f, 32);
        ndist = cv::compareHist(histr, histl, CV_COMP_BHATTACHARYYA);
        histr.release();
        histl.release();

        if ( ndist < dist )
          dist = ndist;
        
        if ( ndist < x_cdistance )
        {
          cv::rectangle( imgMat2l,
            cv::Point(rect.x,rect.y),
            cv::Point(rect.x+rect.width,rect.y+rect.height),
            cv::Scalar(255,255,255));
          cv::drawContours( imgMat2l, contourl, i, cv::Scalar(255,255,255));
        }
        else
        {
          cv::rectangle( imgMat2l, 
            cv::Point(rect.x,rect.y), 
            cv::Point(rect.x+rect.width,rect.y+rect.height), 
            cv::Scalar(128,128,128));
          cv::drawContours( imgMat2l, contourl, i, cv::Scalar(128,128,128));
        }
      }
    }
  }

  if ( dist < 100.00 ) 
    outlet_float( m_dataout, dist );

  // memcpy( left.data, gray->imageData, left.xsize*left.ysize );

}
#if 0
void pix_opencv_pgh_compare :: processYUV_YUV(imageStruct &left, imageStruct &right)
{
   post( "pix_opencv_pgh_compare : YUV colorspace not supported" );
}
#endif 

/////////////////////////////////////////////////////////
// static member function
//
/////////////////////////////////////////////////////////
void pix_opencv_pgh_compare :: obj_setupCallback(t_class *classPtr)
{ 
  class_addmethod(classPtr, (t_method)&pix_opencv_pgh_compare::floatMinSizeMessCallback,
          gensym("minsize"), A_FLOAT, A_NULL);
  class_addmethod(classPtr, (t_method)&pix_opencv_pgh_compare::clearMessCallback,
          gensym("clear"), A_NULL);
  class_addmethod(classPtr, (t_method)&pix_opencv_pgh_compare::floatCriteriaMessCallback,
          gensym("criteria"), A_FLOAT, A_NULL);
}

void pix_opencv_pgh_compare :: floatMinSizeMessCallback(void *data, t_floatarg minsize)
{
  GetMyClass(data)->floatMinSizeMess((float)minsize);
}

void pix_opencv_pgh_compare :: clearMessCallback(void *data)
{
  GetMyClass(data)->clearMess();
}

void pix_opencv_pgh_compare :: floatCriteriaMessCallback(void *data, t_floatarg criteria)
{
  GetMyClass(data)->floatCriteriaMess((float)criteria);
}

void  pix_opencv_pgh_compare :: floatMinSizeMess(float minsize)
{
   if ( (int)minsize > 0 )
   {
     x_minsize = (int)minsize;
   }
}

void  pix_opencv_pgh_compare :: clearMess(void)
{
   x_bcontourr = -1;
}

void  pix_opencv_pgh_compare :: floatCriteriaMess(float criteria)
{
   if ( criteria > 0.0 )
   {
      x_cdistance = criteria;
   }
}
