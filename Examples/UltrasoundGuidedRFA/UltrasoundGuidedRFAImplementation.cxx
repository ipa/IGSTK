/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    UltrasoundGuidedRFAImplementation.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "UltrasoundGuidedRFAImplementation.h"
#include "igstkEvents.h"

namespace igstk
{

/** Constructor */
UltrasoundGuidedRFAImplementation::UltrasoundGuidedRFAImplementation()
                                                         :m_StateMachine(this)
{
  m_Tracker = TrackerType::New();
    
  m_Logger = LoggerType::New();
  m_LogOutput = LogOutputType::New();
  m_LogFile.open("C:/Julien/logUltrasoundGuidedRFA.txt");
  if( !m_LogFile.fail() )
    {
    m_LogOutput->SetStream( m_LogFile );
    }
  else
    {
    std::cerr << "Problem opening Log file, using cerr instead " << std::endl;
    m_LogOutput->SetStream( std::cerr );
    }
  m_Logger->AddLogOutput( m_LogOutput );
  // add stdout for debug purposes
  LogOutputType::Pointer coutLogOutput = LogOutputType::New();
  coutLogOutput->SetStream( std::cout );
  //m_Logger->AddLogOutput( coutLogOutput );
  
  m_Logger->SetPriorityLevel( LoggerType::DEBUG );
  //m_Tracker->SetLogger( m_Logger );

  m_Communication = CommunicationType::New();
  //m_Communication->SetLogger( m_Logger );
  m_Communication->SetPortNumber( igstk::SerialCommunication::PortNumber0 );
  m_Communication->SetParity( igstk::SerialCommunication::NoParity );
  m_Communication->SetBaudRate( igstk::SerialCommunication::BaudRate19200 );
  m_Tracker->SetCommunication(m_Communication);
  m_Communication->OpenCommunication();

  /** Tool calibration transform */
  igstk::Transform toolCalibrationTransform;
  igstk::Transform::VectorType translation;
  translation[0] = 0;   // Tip offset
  translation[1] = 0;
  translation[2] = 390;
      
  igstk::Transform::VersorType rotation;
  rotation.SetRotationAroundY(-3.141597/2.0);
  
  igstk::Transform::VersorType rotation2;
  rotation2.SetRotationAroundX(3.141597/2.0);
    
  rotation = rotation2*rotation;
  
  translation = rotation.Transform(translation);

  toolCalibrationTransform.SetTranslationAndRotation(translation,
                                                  rotation,0.0001,100000);
  m_Tracker->SetToolCalibrationTransform( 0, 0, toolCalibrationTransform);

  /** Tool calibration transform */
  igstk::Transform patientTransform;
  igstk::Transform::VectorType translationP;
  translationP[0] = 0;   // Tip offset
  translationP[1] = 0;
  translationP[2] = 0;

  igstk::Transform::VersorType rotationP;
  rotationP.SetRotationAroundY(-3.141597/2.0);
    
  igstk::Transform::VersorType rotation2P;
  rotation2P.SetRotationAroundZ(3.141597/2.0);
   
  rotationP = rotation2P*rotationP;

  patientTransform.SetTranslationAndRotation(translationP,
                                             rotationP,0.0001,100000);
  m_Tracker->SetPatientTransform(patientTransform);

  m_Tracker->Open();
  m_Tracker->Initialize();

  // Set up the four quadrant views
  this->Display3D->RequestResetCamera();
  this->Display3D->Update();
  this->Display3D->RequestEnableInteractions();
  this->Display3D->RequestSetRefreshRate( 60 ); // 60 Hz
  this->Display3D->RequestStart();

  this->Display2D->RequestResetCamera();
  this->Display2D->Update();
  this->Display2D->RequestEnableInteractions();
  this->Display2D->RequestSetRefreshRate( 60 ); // 60 Hz
  this->Display2D->RequestStart();
      
  m_Tracking = false;

  m_MeshReader = LiverReaderType::New();
  m_VascularNetworkReader = VascularNetworkReaderType::New();
  m_MRImageReader = MRImageReaderType::New();
  m_USImageReader = USImageReaderType::New();
  m_ContourLiverRepresentation = ContourMeshObjectRepresentation::New();
  m_ContourVascularNetworkRepresentation = 
                      ContourVascularNetworkObjectRepresentation::New();
  m_VascularNetworkRepresentation = VascularNetworkRepresentationType::New();
  
  m_ObliquePoint.Fill(0);
  m_HasQuitted = false;
   
  m_LiverMRRepresentation = MRImageRepresentationType::New();
  //m_ObliqueLiverMRRepresentation = MRImageRepresentationType::New();
  m_ObliqueLiverMRRepresentation = MRObliqueImageRepresentationType::New();
  
  this->ObserveAxialBoundsInput(    m_LiverMRRepresentation    );
  
  igstkAddStateMacro( Initial );
  igstkAddInputMacro( AxialBounds );
  igstkAddTransitionMacro( Initial, AxialBounds, 
                           Initial, SetAxialSliderBounds );
 
  igstkSetInitialStateMacro( Initial );
  m_StateMachine.SetReadyToRun();

}

/** Desctructor */
UltrasoundGuidedRFAImplementation
::~UltrasoundGuidedRFAImplementation()
{
  m_Tracker->Reset();
  m_Tracker->StopTracking();
  m_Tracker->Close();
}

/** Quit */
void UltrasoundGuidedRFAImplementation::Quit()
{
  int i = fl_choice("Are you sure you want to quit?", NULL,"Yes","No"); 
  if ( i == 1 )
    {
    mainWindow->hide();
    m_HasQuitted = true;
    }
  else
    {
    return;
    }
}

void UltrasoundGuidedRFAImplementation::SetAxialSliderBoundsProcessing()
{
  const unsigned int min = m_AxialBoundsInputToBeSet.minimum;
  const unsigned int max = m_AxialBoundsInputToBeSet.maximum; 
  const unsigned int slice = static_cast< unsigned int > ( 
                                                   ( min + max ) / 2.0 );
  //m_ObliqueLiverMRRepresentation->RequestSetSliceNumber( slice );
  this->slider->minimum( min );
  this->slider->maximum( max );
  this->slider->value( slice );  
  this->slider->activate();
  Fl::check();
}

void UltrasoundGuidedRFAImplementation::EnableTracking()
{
  m_Tracking = true;
  m_Tracker->StartTracking();
}

/** Start the tracking */
void UltrasoundGuidedRFAImplementation::DisableTracking()
{
  m_Tracker->Reset();
  m_Tracker->StopTracking();
  m_Tracking = false;
}

/** Add the probe object to the display */
void UltrasoundGuidedRFAImplementation
::AddProbe( igstk::UltrasoundProbeObjectRepresentation 
            * cylinderRepresentation )
{
  //this->Display2D->RequestAddObject(    cylinderRepresentation->Copy() );
  this->Display3D->RequestAddObject(    cylinderRepresentation->Copy() );
}

/** Add Axes to the display */
void UltrasoundGuidedRFAImplementation
::AddAxes( igstk::AxesObjectRepresentation * cylinderRepresentation )
{
  //this->Display2D->RequestAddObject(    cylinderRepresentation->Copy() );
  this->Display3D->RequestAddObject(    cylinderRepresentation->Copy() );
}

/** Add object to track */
void UltrasoundGuidedRFAImplementation
::AttachObjectToTrack( igstk::SpatialObject * objectToTrack )
{
  const unsigned int toolPort = 0;
  const unsigned int toolNumber = 0;
  m_Tracker->AttachObjectToTrackerTool( toolPort, toolNumber, objectToTrack );
}

/** Load a liver surface */
void UltrasoundGuidedRFAImplementation
::LoadLiverSurface()
{
  const char * liverfilename = fl_file_chooser("Load a liver mesh",
                                               "*.msh","*.msh");
  if(liverfilename)
    {
    m_MeshReader->RequestSetFileName(liverfilename);
    m_MeshReader->RequestReadObject();
    m_LiverRepresentation = MeshObjectRepresentation::New();
    m_LiverRepresentation->RequestSetMeshObject( m_MeshReader->GetOutput() );
    m_LiverRepresentation->SetColor(1.0,0.0,0.0);
    m_LiverRepresentation->SetOpacity(1.0);
    this->Display3D->RequestAddObject(m_LiverRepresentation);
 
    m_ContourLiverRepresentation->RequestSetMeshObject( 
                                                  m_MeshReader->GetOutput() );
    m_ContourLiverRepresentation->RequestSetOrientation(
                                      ContourMeshObjectRepresentation::Axial);
    m_ContourLiverRepresentation->SetColor(1.0,0.0,0.0);
    m_ContourLiverRepresentation->SetOpacity(1.0);
    this->Display2D->RequestAddObject(m_ContourLiverRepresentation);
    }
}

/** Load liver vasculature */
void UltrasoundGuidedRFAImplementation
::LoadLiverVasculature()
{
  const char * liverfilename = fl_file_chooser("Load a liver vasculature",
                                               "*.tre","*.tre");
  if(liverfilename)
    {
    m_VascularNetworkReader->RequestSetFileName(liverfilename);
    m_VascularNetworkReader->RequestReadObject();
    m_VascularNetworkRepresentation->RequestSetVascularNetworkObject( 
                                       m_VascularNetworkReader->GetOutput() );
    //m_VascularNetworkRepresentation->SetColor(1.0,0.0,0.0);
    //m_VascularNetworkRepresentation->SetOpacity(1.0);
    this->Display3D->RequestAddObject( m_VascularNetworkRepresentation );

    // Represent as a 2D contour on the 2D View
    m_ContourVascularNetworkRepresentation->RequestSetVascularNetworkObject( 
                                       m_VascularNetworkReader->GetOutput() );
    m_ContourVascularNetworkRepresentation->RequestSetOrientation(
                                      ContourMeshObjectRepresentation::Axial);
    m_ContourVascularNetworkRepresentation->SetColor(1.0,1.0,1.0);
    m_ContourVascularNetworkRepresentation->SetOpacity(1.0);
    this->Display2D->RequestAddObject(m_ContourVascularNetworkRepresentation);
    }
}


/** Load liver image */
void UltrasoundGuidedRFAImplementation
::LoadLiverImage()
{
  const char * liverfilename = fl_dir_chooser("Load a liver image","");
  if(liverfilename)
    {
    std::string filename = liverfilename;
    // Remove the slash at the end of the filename
    if(filename[strlen(liverfilename)-1] == '/')
      {
      filename[strlen(liverfilename)-1] = 0;
      }

    //m_MRImageReader->SetLogger(m_Logger);
    m_MRImageReader->RequestSetDirectory(filename.c_str());
    m_MRImageReader->RequestReadImage();
  
    m_LiverMRRepresentation->RequestSetImageSpatialObject( 
                                                m_MRImageReader->GetOutput());
    m_LiverMRRepresentation->SetWindowLevel(52,52);
    m_LiverMRRepresentation->RequestSetOrientation(
                                            MRImageRepresentationType::Axial);
    this->Display3D->RequestAddObject( m_LiverMRRepresentation );

    m_ObliqueLiverMRRepresentation->RequestSetImageSpatialObject(
                                                m_MRImageReader->GetOutput());
    m_ObliqueLiverMRRepresentation->SetWindowLevel(52,52);
    
    //m_ObliqueLiverMRRepresentation->RequestSetOrientation(
     //                                       MRImageRepresentationType::Axial);
    //m_ObliqueLiverMRRepresentation->RequestGetSliceNumberBounds();
    
    m_StateMachine.ProcessInputs();

    this->Display2D->RequestAddObject( m_ObliqueLiverMRRepresentation );
    this->Display2D->RequestResetCamera();
    }
}

/** Load 2D Ultrasound */
void UltrasoundGuidedRFAImplementation
::Load2DUltrasound()
{
  const char * liverfilename = fl_dir_chooser("Load a 2D ultrasound","");
  if(liverfilename)
    {
    std::string filename = liverfilename;

    // Remove the slash at the end of the filename
    if(filename[strlen(liverfilename)-1] == '/')
      {
      filename[strlen(liverfilename)-1] = 0;
      }

    m_USImageReader->RequestSetDirectory(filename.c_str());
    m_USImageReader->RequestReadImage();

    m_LiverUSRepresentation = USImageRepresentationType::New();
    m_LiverUSRepresentation->RequestSetImageSpatialObject(
                                                m_USImageReader->GetOutput());
    m_LiverUSRepresentation->SetWindowLevel(255/2.0,255/2.0);
    this->Display2D->RequestAddObject( m_LiverUSRepresentation );
    this->Display2D->RequestResetCamera();
    }
}

/** Set the slice number */
void UltrasoundGuidedRFAImplementation
::SetSliceNumber(unsigned int value)
{
  m_LiverMRRepresentation->RequestSetSliceNumber(value);
  //m_ObliqueLiverMRRepresentation->RequestSetSliceNumber(value);

  MRObliqueImageRepresentationType::PointType origin;
  origin[0] = 0;
  origin[1] = 0;
  origin[2] = value;
  MRObliqueImageRepresentationType::VectorType v1;
  v1[0] = 1;
  v1[1] = 0;
  v1[2] = 1;
  MRObliqueImageRepresentationType::VectorType v2;
  v2[0] = 0;
  v2[1] = 1;
  v2[2] = 0;

  m_ObliqueLiverMRRepresentation->RequestSetOriginPointOnThePlane(origin);
  m_ObliqueLiverMRRepresentation->RequestSetVector1OnThePlane(v1);
  m_ObliqueLiverMRRepresentation->RequestSetVector2OnThePlane(v2);
  m_ObliqueLiverMRRepresentation->RequestReslice();
  //m_ObliqueLiverMRRepresentation->RequestReslice();
  this->Display2D->RequestResetCamera();
  
}

/** Randomize. Test only. */
void UltrasoundGuidedRFAImplementation
::Randomize()
{
  // Not ready yet
  /*if(m_ObliqueLiverMRRepresentation)
    {
    std::cout << m_ObliquePoint << std::endl;
    m_ObliquePoint[2]++;
    if(m_ObliquePoint[2]>20)
      {
      m_ObliquePoint[2]=0;
      }
    m_ObliqueLiverMRRepresentation->SetLogger(m_Logger);
    m_ObliqueLiverMRRepresentation->RequestSetPointOnthePlane(m_ObliquePoint);
    m_ObliqueLiverMRRepresentation->RequestReslice();
    }*/
}

} // end of namespace
