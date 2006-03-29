/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkObliqueImageSpatialObjectRepresentationTest.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#include "igstkObliqueImageSpatialObjectRepresentation.h"
#include "igstkImageSpatialObjectRepresentation.h"
#include "igstkImageSpatialObject.h"
#include "igstkView2D.h"
#include "igstkCTImageReader.h"

#include "itkLogger.h"
#include "itkStdStreamLogOutput.h"
#include "igstkEvents.h"

int igstkObliqueImageSpatialObjectRepresentationTest( 
                                                 int argc , char * argv [] )
{

  igstk::RealTimeClock::Initialize();

  typedef short    PixelType;
  const unsigned int Dimension = 3;

  typedef igstk::ImageSpatialObject< 
                                PixelType, 
                                Dimension> ImageSpatialObjectType;
  
  typedef igstk::ObliqueImageSpatialObjectRepresentation< 
                                  ImageSpatialObjectType 
                                                      >   RepresentationType;

  RepresentationType::Pointer  representation = RepresentationType::New();

  typedef itk::Logger              LoggerType;
  typedef itk::StdStreamLogOutput  LogOutputType;
  
  
  // logger object created for logging mouse activities
  LoggerType::Pointer   logger = LoggerType::New();
  LogOutputType::Pointer logOutput = LogOutputType::New();
  logOutput->SetStream( std::cout );
  logger->AddLogOutput( logOutput );
  logger->SetPriorityLevel( itk::Logger::DEBUG );

  //
  typedef igstk::CTImageReader         ReaderType;

  ReaderType::Pointer   reader = ReaderType::New();

  reader->SetLogger( logger );

  /* Read in a DICOM series */
  std::cout << "Reading CT image : " << argv[1] << std::endl;

  ReaderType::DirectoryNameType directoryName = argv[1];

  reader->RequestSetDirectory( directoryName );
 
  reader->RequestReadImage();

  representation->SetLogger( logger );

  representation->RequestSetImageSpatialObject( reader->GetOutput() );

  // Exercise the TypeMacro() which defines the GetNameOfClass()
  std::string name = representation->GetNameOfClass();

  std::cout << "Name of class = " << name << std::endl;
  
  // Create an FLTK minimal GUI
  Fl_Window * form = new Fl_Window(532,532,"CT Read View Test");
    
  typedef igstk::View2D  View2DType;

  View2DType * view2D = new View2DType( 10,10,512,512,"2D View");

  form->end();
  form->show();
  
  // Set oblique plane parameters
 
  typedef RepresentationType::PointType  PointType;
  typedef RepresentationType::VectorType VectorType;

  PointType  point;

  point[0] = 18.0;
  point[1] = -171.0;
  point[2] = -186.0;

  representation->RequestSetOriginPointOnThePlane( point );
  
  VectorType  vector1;

  vector1[0] = 2.0;
  vector1[1] = 0.0;
  vector1[2] = 0.0;

  representation->RequestSetVector1OnThePlane( vector1 );
  
  VectorType  vector2;

  vector2[0] = 0.0;
  vector2[1] = 4.0;
  vector2[2] = 0.0;

  representation->RequestSetVector2OnThePlane( vector2 );
  
  representation->RequestReslice();  

  view2D->RequestAddObject( representation );
  view2D->RequestResetCamera();

  for(int i=0; i<10; i++) 
    {
    std::cout << "Refresh=" << i << std::endl;
    igstk::PulseGenerator::CheckTimeouts();
    view2D->Update();  // schedule redraw of the view
    Fl::check(); 
    }
  
  delete view2D;
  delete form;

  return EXIT_SUCCESS;
}
