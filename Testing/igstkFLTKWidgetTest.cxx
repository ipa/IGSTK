/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkFLTKWidgetTest.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#if defined(_MSC_VER)
//  Warning about: identifier was truncated to '255' characters in the 
//  debug information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif

#include <iostream>
#include "igstkView2D.h"
#include "igstkView3D.h"
#include "igstkEvents.h"
#include "igstkAxesObject.h"
#include "igstkEllipsoidObject.h"
#include "igstkCylinderObject.h"
#include "igstkEllipsoidObjectRepresentation.h"
#include "igstkCylinderObjectRepresentation.h"
#include "igstkVTKLoggerOutput.h"
#include "igstkFLTKWidget.h"

#include "igstkLogger.h"
#include "itkStdStreamLogOutput.h"

namespace FLTKWidgetTest{
  
class ViewObserver : public ::itk::Command 
{
public:
  
  typedef  ViewObserver               Self;
  typedef  ::itk::Command             Superclass;
  typedef  ::itk::SmartPointer<Self>  Pointer;
  itkNewMacro( Self );

protected:
  ViewObserver() 
    {
    m_PulseCounter = 0;
    m_View = 0;
    }
public:

  void Execute(const itk::Object *caller, const itk::EventObject & event)
    {
    std::cerr << "Execute( const * ) should not be called" << std::endl;
    }

  void SetView( ::igstk::View * view )
    {
    m_View = view;
    if( m_View )
      {
      m_View->AddObserver( ::igstk::RefreshEvent(), this );
      }
    }

  void SetEndFlag( bool * end )
    {
    m_End = end;
    }

  void Execute(itk::Object *caller, const itk::EventObject & event)
    {
    if( ::igstk::RefreshEvent().CheckEvent( &event ) )
      {
      m_PulseCounter++;

      if( m_PulseCounter > 30 )
        {
        if( m_View )
          {
          m_View->RequestStop();
          } 
        else
          {
          std::cerr << "View pointer is NULL " << std::endl;
          }
        *m_End = true;
        return;
        }
      }
    }
private:
  
  unsigned long       m_PulseCounter;
  ::igstk::View     * m_View;
  bool *              m_End;

};

}

int igstkFLTKWidgetTest( int, char * [] )
{
  igstk::RealTimeClock::Initialize();

  typedef igstk::View2D  View2DType;
  typedef igstk::View3D  View3DType;

  bool bEnd = false;

  typedef igstk::Object::LoggerType     LoggerType;
  typedef itk::StdStreamLogOutput  LogOutputType;
  
  // logger object created for logging mouse activities
  LoggerType::Pointer   logger = LoggerType::New();
  LogOutputType::Pointer logOutput = LogOutputType::New();
  logOutput->SetStream( std::cout );
  logger->AddLogOutput( logOutput );
  logger->SetPriorityLevel( itk::Logger::CRITICAL );

  // Create an igstk::VTKLoggerOutput and then test it.
  igstk::VTKLoggerOutput::Pointer vtkLoggerOutput = 
                                                igstk::VTKLoggerOutput::New();
  vtkLoggerOutput->OverrideVTKWindow();
  vtkLoggerOutput->SetLogger(logger);  // redirect messages from 
                                       // VTK OutputWindow -> logger

  try
    {
    // Create the referene system
    igstk::AxesObject::Pointer worldReference = igstk::AxesObject::New();
   
    // Create the ellipsoid 
    igstk::EllipsoidObject::Pointer ellipsoid = igstk::EllipsoidObject::New();
    ellipsoid->SetRadius(0.1,0.1,0.1);
    
    // Create the ellipsoid representation
    igstk::EllipsoidObjectRepresentation::Pointer ellipsoidRepresentation =
                             igstk::EllipsoidObjectRepresentation::New();

    ellipsoidRepresentation->RequestSetEllipsoidObject( ellipsoid );
    ellipsoidRepresentation->SetColor(0.0,1.0,0.0);
    ellipsoidRepresentation->SetOpacity(1.0);

    // Create the cylinder 
    igstk::CylinderObject::Pointer cylinder = igstk::CylinderObject::New();
    cylinder->SetRadius(0.1);
    cylinder->SetHeight(0.5);

    // Create the cylinder representation
    igstk::CylinderObjectRepresentation::Pointer cylinderRepresentation =
                              igstk::CylinderObjectRepresentation::New();

    cylinderRepresentation->RequestSetCylinderObject( cylinder );
    cylinderRepresentation->SetColor(1.0,0.0,0.0);
    cylinderRepresentation->SetOpacity(1.0);

    const double validityTimeInMilliseconds = 1e300; // 100 seconds
    igstk::Transform transform;
    igstk::Transform::VectorType translation;
    translation[0] = 0;
    translation[1] = 0;
    translation[2] = 0;
    igstk::Transform::VersorType rotation;
    rotation.Set( 0.0, 0.0, 0.0, 1.0 );
    igstk::Transform::ErrorType errorValue = 10; // 10 millimeters

    transform.SetTranslationAndRotation( 
        translation, rotation, errorValue, validityTimeInMilliseconds );

    ellipsoid->RequestSetTransformAndParent( transform, worldReference.GetPointer() );

    translation[1] = -0.25;  // translate the cylinder along Y
    translation[2] = -2.00;  // translate the cylinder along Z
    rotation.Set( 0.7071, 0.0, 0.0, 0.7071 );

    transform.SetTranslationAndRotation( 
        translation, rotation, errorValue, validityTimeInMilliseconds );

    cylinder->RequestSetTransformAndParent( transform, worldReference.GetPointer() );

    View2DType::Pointer view2D = View2DType::New();
    view2D->SetLogger( logger );
    
    View3DType::Pointer view3D = View3DType::New();
    view3D->SetLogger( logger );

    igstk::Transform identityTransform;
    identityTransform.SetToIdentity( igstk::TimeStamp::GetLongestPossibleTime() );

    view2D->RequestSetTransformAndParent( identityTransform, worldReference.GetPointer() );
    view3D->RequestSetTransformAndParent( identityTransform, worldReference.GetPointer() );

    // Add the cylinder to the view
    view2D->RequestAddObject( ellipsoidRepresentation );
    view3D->RequestAddObject( cylinderRepresentation );

    // Create an FLTK minimal GUI
    typedef igstk::FLTKWidget      FLTKWidgetType;

    // End of the GUI creation

    // Set the refresh rate and start 
    // the pulse generators of the views.
    view2D->SetRefreshRate( 10 );

    // Set the refresh rate and start 
    // the pulse generators of the views.
    view3D->SetRefreshRate( 10 );

    Fl_Window * form = new Fl_Window(601,301,"View Test");
    
    // instantiate FLTK widget 
    FLTKWidgetType * fltkWidget2D = 
                      new FLTKWidgetType( 10,10,280,280,"2D View");
    fltkWidget2D->RequestSetView( view2D );
    fltkWidget2D->SetLogger( logger );
    
    FLTKWidgetType * fltkWidget3D = 
                      new FLTKWidgetType( 310,10,280,280,"3D View");
    fltkWidget3D->RequestSetView( view3D );
    fltkWidget3D->SetLogger( logger );

    // Add the cylinder to the view 2D
    view2D->RequestAddObject( cylinderRepresentation );

    // Add the ellipsoid to the view 3D
    view3D->RequestAddObject( ellipsoidRepresentation );

    form->end();
    form->show();

    view3D->RequestResetCamera();
    view2D->RequestResetCamera();

    view2D->RequestStart();
    view3D->RequestStart();

    typedef FLTKWidgetTest::ViewObserver ObserverType;
    ObserverType::Pointer viewObserver = ObserverType::New();
    
    viewObserver->SetView( view3D );
    viewObserver->SetEndFlag( &bEnd );

    while( !bEnd )
      {
      Fl::wait(0.01);
      igstk::PulseGenerator::CheckTimeouts();
      }

    //Test a widget without a view connected to it 
    FLTKWidgetType * fltkWidget3DWithoutViewConnected = 
                      new FLTKWidgetType( 310,10,280,280,"3D View");
    fltkWidget3DWithoutViewConnected->SetLogger( logger );

    delete fltkWidget2D;
    delete fltkWidget3D;
    delete fltkWidget3DWithoutViewConnected;
    delete form;
    }
  catch(...)
    {
    std::cerr << "Exception catched !!" << std::endl;
    return EXIT_FAILURE;
    }

  if( vtkLoggerOutput->GetNumberOfErrorMessages()  > 0 )
    {
    return EXIT_FAILURE;
    }
 
  return EXIT_SUCCESS;
}
