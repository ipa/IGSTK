/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    NavigatorQuadrantViews.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

// generated by Fast Light User Interface Designer (fluid) version 1.0107

#ifndef __NavigatorQuadrantViews_h
#define __NavigatorQuadrantViews_h

#include <FL/Fl.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Value_Slider.H>

#include "igstkFLTKWidget.h"
#include "igstkView2D.h"
#include "igstkView3D.h"
#include "igstkEvents.h"
#include "igstkCTImageSpatialObject.h"

namespace igstk {

/** \class NavigatorQuadrantViews
* 
* \brief Composite FLTK class defines the standard four viewer windows.
*
* This class provides a convenient way to define the commonly used quadrant 
* views. It has three 2D views and one 3D view. From left to right and top to 
* bottom they are: Axial, Sagittal, Coronal, and 3D. For convenience, arrays
* of pointers are also provided for easy accessing of the Views and Widget 
* objects. The index for those object is: Axial=0, Sagittal=1, Coronal=2, 
* 3D=3.
* 
* There are also three slider bars for each 2D views, which can be used for
* image reslicing. When moving the slider bar, a ReslicingEvent carrying
* slice index (current value of the three slider bar) will be triggered. 
* User can observe this event and pass the index to image representation
* object to do reslicing.
* 
* For picking event, observer should be added directly to the internal View
* objects.
*
*/

class NavigatorQuadrantViews : public Fl_Group {
public:

  NavigatorQuadrantViews(int X, int Y, int W, int H, const char *L = 0);

  virtual ~NavigatorQuadrantViews(void);

  typedef igstk::View          ViewType;
  typedef igstk::View2D        ViewType2D;
  typedef igstk::View3D        ViewType3D;

  // Declare View objects
  ViewType2D::Pointer          m_AxialView;
  ViewType2D::Pointer          m_SagittalView;
  ViewType2D::Pointer          m_CoronalView;
  ViewType3D::Pointer          m_3DView;

  /** Annotation is used for displaying 2D texts on View */
  igstk::Annotation2D::Pointer        m_AxialViewAnnotation;
  igstk::Annotation2D::Pointer        m_SagittalViewAnnotation;
  igstk::Annotation2D::Pointer        m_CoronalViewAnnotation;

  // Declare FLTKWidgetNew objects
  igstk::FLTKWidget * m_AxialWidget;
  igstk::FLTKWidget * m_SagittalWidget;
  igstk::FLTKWidget * m_CoronalWidget;

  igstk::FLTKWidget * m_3DWidget;

  // Define slider bars
  Fl_Value_Slider * m_SuperiorLeftSlider;
  Fl_Value_Slider * m_SuperiorRightSlider;
  Fl_Value_Slider * m_InferiorLeftSlider;
  Fl_Value_Slider * m_InferiorRightSlider;

  // Array of pointer to the View, Widget, and Slider bar
  //std::vector< igstk::FLTKWidget * >   m_Displays;
 // std::vector< ViewType::Pointer   >   m_Views;
  std::vector< Fl_Value_Slider   * >   m_Sliders;

  /** Event for reslicing from slider bar */
  typedef CTImageSpatialObject::IndexType  IndexType;

  igstkLoadedEventMacro( ManualReslicingEvent, IGSTKEvent, IndexType );

  typedef struct 
  {
    int current_x;
    int current_y;
    int prev_x;
    int prev_y;
  } WindowLevelStructType;

  typedef struct 
  {
    int key;
    int state;
  } KeyboardCommandType;

  typedef struct 
  {
    int button;
    int dx;
    int dy;
    int x;
    int y;
    int state;
    int key;
  } MouseCommandType;

  igstkLoadedEventMacro( KeyPressedEvent, IGSTKEvent, KeyboardCommandType );
  igstkLoadedEventMacro( MousePressedEvent, IGSTKEvent, MouseCommandType );

  unsigned long AddObserver( 
    const ::itk::EventObject & event, ::itk::Command * observer );

  void RequestUpdateOverlays();

  void RemoveObserver( unsigned long tag );
  void RemoveAllObservers();

protected:

  virtual void RequestResliceImage(void);

private:

  // Call backs
  void cb_SuperiorLeftSlider_i(Fl_Value_Slider*, void*);
  static void cb_SuperiorLeftSlider(Fl_Value_Slider*, void*);
  void cb_SuperiorRightSlider_i(Fl_Value_Slider*, void*);
  static void cb_SuperiorRightSlider(Fl_Value_Slider*, void*);
  void cb_InferiorLeftSlider_i(Fl_Value_Slider*, void*);
  static void cb_InferiorLeftSlider(Fl_Value_Slider*, void*);

  ::itk::Object::Pointer    m_Reporter;

private:
  int handle_key(int event, KeyboardCommandType &keyCommand);
  int handle_mouse(int event, MouseCommandType &mouseCommand);

  int m_Width;
  int m_Height;
  int m_X;
  int m_Y;
  int m_WW;
  int m_HH;

  WindowLevelStructType m_wl;

public:

  virtual int handle(int e);

};



} // end namespace igstk

#endif
