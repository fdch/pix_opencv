/*-----------------------------------------------------------------
LOG
    GEM - Graphics Environment for Multimedia

    Threshold filter

    Copyright (c) 1997-1999 Mark Danks. mark@danks.org
    Copyright (c) Günther Geiger. geiger@epy.co.at
    Copyright (c) 2001-2002 IOhannes m zmoelnig. forum::für::umläute. IEM. zmoelnig@iem.kug.ac.at
    Copyright (c) 2002 James Tittle & Chris Clepper
    For information on usage and redistribution, and for a DISCLAIMER OF ALL
    WARRANTIES, see the file, "GEM.LICENSE.TERMS" in this distribution.

-----------------------------------------------------------------*/

#ifndef INCLUDE_pix_opencv_calchist_H_
#define INCLUDE_pix_opencv_calchist_H_

#include "opencv2/opencv.hpp"
#include "opencv2/imgproc.hpp"


#include "Base/GemPixObj.h"

/*-----------------------------------------------------------------
-------------------------------------------------------------------
CLASS
    pix_opencv_calchist
    
    square pattern detector
    
KEYWORDS
    pix
    
DESCRIPTION
   
-----------------------------------------------------------------*/
class GEM_EXPORT pix_opencv_calchist : public GemPixObj
{
    CPPEXTERN_HEADER(pix_opencv_calchist, GemPixObj)

    public:

      //////////
      // Constructor
      pix_opencv_calchist();
        
    protected:
        
      //////////
      // Destructor
      virtual ~pix_opencv_calchist();

      //////////
      // Do the processing
      virtual void     processGrayImage(imageStruct &image);
      // virtual void     processRGBAImage(imageStruct &image);

      void sigmaXMess(double f);
      void sigmaYMess(double f);
      void borderMess(int f);

    private:


      double m_sigmaX;
      double m_sigmaY;
      int    m_border;

};
#endif    // for header file
