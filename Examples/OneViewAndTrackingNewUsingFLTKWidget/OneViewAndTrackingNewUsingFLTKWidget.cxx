/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    OneViewAndTrackingNewUsingFLTKWidget.cxx
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
//  Warning about: identifier was truncated to '255' characters 
//  in the debug information (MVC6.0 Debug)
#pragma warning( disable : 4284 )
#endif

#include "OneViewAndTrackingNewUsingFLTKWidgetImplementation.h"
#include "igstkViewNew3D.h"
#include "igstkEllipsoidObject.h"
#include "igstkCylinderObject.h"
#include "igstkEllipsoidObjectRepresentation.h"
#include "igstkCylinderObjectRepresentation.h"


int main(int , char** )
{ 

  igstk::RealTimeClock::Initialize();

  if( argc < 4 )

    {
    std::cerr << " Usage: " << argv[0] << "\t" 
                            << "MicronTracker_Camera_Calibration_file" << "\t"
                            << "MicronTracker_initialization_file"  << "\t"
                            << "Marker_template_directory " << std::endl; 
    return EXIT_FAILURE;
    }

 
 
  OneViewAndTrackingNewUsingFLTKWidgetImplementation   application;

  // Create the ellipsoid 
  igstk::EllipsoidObject::Pointer ellipsoid = igstk::EllipsoidObject::New();
  ellipsoid->SetRadius(200,200,300); // about a human skull
  
  // Create the ellipsoid representation
  igstk::EllipsoidObjectRepresentation::Pointer 
        ellipsoidRepresentation = igstk::EllipsoidObjectRepresentation::New();
  ellipsoidRepresentation->RequestSetEllipsoidObject( ellipsoid );
  ellipsoidRepresentation->SetColor(0.0,1.0,0.0);
  ellipsoidRepresentation->SetOpacity(1.0);

  // Create the cylinder 
  igstk::CylinderObject::Pointer cylinder = igstk::CylinderObject::New();
  cylinder->SetRadius(1.0);
  //cylinder->SetHeight(300.0);  // about the size of a needle
  cylinder->SetHeight(50.0);  // about the size of a needle

  // Create the cylinder representation
  igstk::CylinderObjectRepresentation::Pointer 
          cylinderRepresentation = igstk::CylinderObjectRepresentation::New();
  cylinderRepresentation->RequestSetCylinderObject( cylinder );
  cylinderRepresentation->SetColor(1.0,0.0,0.0);
  cylinderRepresentation->SetOpacity(1.0);

  // instantiate a 3D view 
  typedef igstk::ViewNew3D        ViewNew3DType;
  ViewNew3DType::Pointer view3D = ViewNew3DType::New();

  /** We pass the bare pointer so that the compiler can figure out the type
      for the templated method. */
  ellipsoid->RequestSetTransformAndParent( transformToView, 
                                           view3D.GetPointer() );

  /* cylinder->RequestSetTransformAndParent( transform, 
                                          ellipsoid.GetPointer() ); */

  // Make the view the parent of the tracker 
  application.AttachTrackerToView( view3D ); 


  view3D->RequestAddObject( ellipsoidRepresentation );
  view3D->RequestAddObject( cylinderRepresentation );
 
  // Set the refresh rate and start 
  // the pulse generators of the views.

  view3D->SetRefreshRate( 30 );
  view3D->RequestStart();
  //view3D->SetCameraPosition(0.0, 0.0, -600.0);
  view3D->SetCameraPosition(-225.0,100.00,-1600.0);

  application.Display3D->RequestSetView( view3D );

  application.Show();

  std::string  CameraCalibrationFileDirectory = argv[1];
  std::string InitializationFile = argv[2];
  std::string markerTemplateDirectory = argv[3];

  application.InitializeTracker( InitializationFile, CameraCalibrationFileDirectory, markerTemplateDirectory );
  application.ConfigureTrackerToolsAndAttachToTheTracker();

  // Associate the cylinder spatial object to the first tracker tool 
  application.AttachObjectToTrackerTool ( 1, cylinder );

  // Associate the ellispsoid spatial object to the second tracker tool 
  application.AttachObjectToTrackerTool ( 2, ellipsoid );

  igstk::Transform             toolTransform; 
  igstk::Transform::VectorType position;

  while( !application.HasQuitted() )
    {
    Fl::wait(0.001);
    igstk::PulseGenerator::CheckTimeouts();

    application.GetTrackerToolTransform( toolTransform );

    position = toolTransform.GetTranslation();
    std::cout << "Trackertool:" 
              << "  Position = (" << position[0]
              << "," << position[1] << "," << position[2]
              << ")" << std::endl;

    std::cout << "CylinderSpatialObject:"
              << "  Position = (" << position[0]
              << "," << position[1] << "," << position[2]
              << ")" << std::endl;

    }

  return EXIT_SUCCESS;
}