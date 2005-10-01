/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkLandmarkRegistration.txx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more DEBUGrmation.

=========================================================================*/
#ifndef __igstkLandmarkRegistration_txx
#define __igstkLandmarkRegistration_txx

#include "igstkLandmarkRegistration.h"

namespace igstk
{ 

/** Constructor */
template <unsigned int TDimension>
LandmarkRegistration<TDimension>::LandmarkRegistration() :
  m_StateMachine( this ), m_Logger( NULL)
{
  // Set the state descriptors
  m_StateMachine.AddState(m_IdleState,
                          "IdleState");
  m_StateMachine.AddState(m_ImageLandmark1AddedState,
                          "ImageLandmark1AddedState");
  m_StateMachine.AddState(m_TrackerLandmark1AddedState,
                          "TrackerLandmark1AddedState");
  m_StateMachine.AddState(m_ImageLandmark2AddedState,
                          "ImageLandmark2AddedState");
  m_StateMachine.AddState(m_TrackerLandmark2AddedState,
                          "TrackerLandmark2AddedState");
  m_StateMachine.AddState(m_ImageLandmark3AddedState,
                          "ImageLandmark3AddedState");
  m_StateMachine.AddState(m_TrackerLandmark3AddedState,
                          "TrackerLandmark3AddedState");


    // Set the input descriptors 
  m_StateMachine.AddInput(m_ImageLandmarkInput,
                          "ImageLandmarkInput");
  m_StateMachine.AddInput(m_TrackerLandmarkInput,
                          "TrackerLandmarkInput");
  m_StateMachine.AddInput(m_ComputeTransformInput,
                          "ComputeTransformInput");
  m_StateMachine.AddInput(m_ResetRegistrationInput,
                          "ResetRegistrationInput");

  // Add transition  for landmark point adding
  m_StateMachine.AddTransition(m_IdleState,
                               m_ImageLandmarkInput,
                               m_ImageLandmark1AddedState,
                               &LandmarkRegistration::AddImageLandmarkPoint);

  m_StateMachine.AddTransition(m_ImageLandmark1AddedState,
                               m_TrackerLandmarkInput,
                               m_TrackerLandmark1AddedState,
                               &LandmarkRegistration::AddTrackerLandmarkPoint);

  m_StateMachine.AddTransition(m_TrackerLandmark1AddedState,
                               m_ImageLandmarkInput,
                               m_ImageLandmark2AddedState,
                               &LandmarkRegistration::AddImageLandmarkPoint);

  m_StateMachine.AddTransition(m_ImageLandmark2AddedState,
                               m_TrackerLandmarkInput,
                               m_TrackerLandmark2AddedState,
                               &LandmarkRegistration::AddTrackerLandmarkPoint);

  m_StateMachine.AddTransition(m_TrackerLandmark2AddedState,
                               m_ImageLandmarkInput,
                               m_ImageLandmark3AddedState,
                               &LandmarkRegistration::AddImageLandmarkPoint);

  m_StateMachine.AddTransition(m_ImageLandmark3AddedState,
                               m_TrackerLandmarkInput,
                               m_TrackerLandmark3AddedState,
                               &LandmarkRegistration::AddTrackerLandmarkPoint);

  m_StateMachine.AddTransition(m_TrackerLandmark3AddedState,
                               m_ImageLandmarkInput,
                               m_ImageLandmark3AddedState,
                               &LandmarkRegistration::AddImageLandmarkPoint);

  m_StateMachine.AddTransition(m_TrackerLandmark3AddedState,
                               m_ComputeTransformInput,
                               m_ImageLandmark3AddedState,
                               &LandmarkRegistration::ComputeTransform);

  // Add transitions for all invalid requests 
  m_StateMachine.AddTransition(m_IdleState,
                               m_ComputeTransformInput,
                               m_IdleState,
                               &LandmarkRegistration::ReportInvalidRequest);

  m_StateMachine.AddTransition(m_ImageLandmark1AddedState,
                               m_ComputeTransformInput,
                               m_ImageLandmark1AddedState,
                               &LandmarkRegistration::ReportInvalidRequest);

  m_StateMachine.AddTransition(m_ImageLandmark2AddedState,
                               m_ComputeTransformInput,
                               m_ImageLandmark2AddedState,
                               &LandmarkRegistration::ReportInvalidRequest);

  m_StateMachine.AddTransition(m_ImageLandmark3AddedState,
                               m_ComputeTransformInput,
                               m_ImageLandmark3AddedState,
                               &LandmarkRegistration::ReportInvalidRequest);

  m_StateMachine.AddTransition(m_TrackerLandmark1AddedState,
                               m_ComputeTransformInput,
                               m_TrackerLandmark1AddedState,
                               &LandmarkRegistration::ReportInvalidRequest);

  m_StateMachine.AddTransition(m_TrackerLandmark2AddedState,
                               m_ComputeTransformInput,
                               m_TrackerLandmark2AddedState,
                               &LandmarkRegistration::ReportInvalidRequest);
 

   // Select the initial state of the state machine
  m_StateMachine.SelectInitialState( m_IdleState );

  // Finish the programming and get ready to run
  m_StateMachine.SetReadyToRun();

  m_Transform = TransformType::New();
  m_TransformInitializer = TransformInitializerType::New();

} 

/** Destructor */
template <unsigned int TDimension>
LandmarkRegistration<TDimension>::~LandmarkRegistration()  
{

}

/* The "AddImageLandmark" method adds landmark points to the image
* landmark point container */
template <unsigned int TDimension>
void 
LandmarkRegistration<TDimension>::AddImageLandmarkPoint() 
{
  igstkLogMacro( DEBUG, "igstk::LandmarkRegistration::"
                 "AddImageLandmarkPoint called...\n");
  m_ImageLandmarks.push_back(m_ImageLandmarkPoint);
}

  
/* The "AddTrackerLandmark" method adds landmark points to the
* tracker landmark point container */
template <unsigned int TDimension>
void  
LandmarkRegistration<TDimension>::AddTrackerLandmarkPoint()
{
  igstkLogMacro( DEBUG, "igstk::LandmarkRegistration::"
                 "AddTrackerLandmarkPoint called...\n");
  m_TrackerLandmarks.push_back(m_TrackerLandmarkPoint);
}

/* The "ResetRegsitration" method empties the landmark point
containers to start the process again */
template <unsigned int TDimension>
void 
LandmarkRegistration<TDimension>::ResetRegistration()
{
  igstkLogMacro( DEBUG, "igstk::LandmarkRegistration::"
                 "ResetRegistration called...\n");

  //Empty the image and tracker landmark containers
  while( m_ImageLandmarks.size() > 0 )
    {
    m_ImageLandmarks.pop_back();
    }
  while( m_TrackerLandmarks.size() > 0 )
    {
    m_TrackerLandmarks.pop_back();
    }
  
  m_StateMachine.SelectInitialState( m_IdleState );
}

/* The "ComputeTransform" method calculates the rigid body
  transformation parameters */
template <unsigned int TDimension>
void 
LandmarkRegistration<TDimension>:: ComputeTransform()
{
  igstkLogMacro( DEBUG, "igstk::LandmarkRegistration::"
                 "ComputeTransform called...\n");
  m_TransformInitializer->SetFixedLandmarks(m_TrackerLandmarks);
  m_TransformInitializer->SetMovingLandmarks(m_ImageLandmarks);
  m_TransformInitializer->SetTransform( m_Transform );
  m_TransformInitializer->InitializeTransform(); 
}


/* The "GetTransformParameters()" method returns transformation
* parameters */
template <unsigned int TDimension>
itk::VersorRigid3DTransform< double >::Pointer
LandmarkRegistration<TDimension>::GetTransform()
{
  return m_Transform;
}

/* The ReportInvalidRequest function reports invalid requests */
template <unsigned int TDimension>
void  
LandmarkRegistration<TDimension>::ReportInvalidRequest()
{
    igstkLogMacro( DEBUG, "igstk::LandmarkRegistration::"
                   "ReportInvalidRequest called...\n");
    this->InvokeEvent(InvalidRequestErrorEvent());
}

template <unsigned int TDimension>
void LandmarkRegistration<TDimension>::RequestAddImageLandmarkPoint(
                                           const LandmarkImagePointType & pt)
{
  igstkLogMacro( DEBUG, "igstk::LandmarkRegistration::"
                 "RequestAddImageLandmarkPoint called...\n");
  this->m_ImageLandmarkPoint = pt;
  this->m_StateMachine.PushInput( this->m_ImageLandmarkInput);
  this->m_StateMachine.ProcessInputs();
}

template <unsigned int TDimension>  
void 
LandmarkRegistration<TDimension>::RequestAddTrackerLandmarkPoint(
                                      const LandmarkImagePointType & pt)
{
  igstkLogMacro( DEBUG, "igstk::LandmarkRegistration::"
                 "RequestAddTrackerLandmarkPoint called...\n");
  this->m_TrackerLandmarkPoint = pt;
  this->m_StateMachine.PushInput( this->m_TrackerLandmarkInput );
  this->m_StateMachine.ProcessInputs();
}

template <unsigned int TDimension>
void 
LandmarkRegistration<TDimension>::RequestResetRegistration()
{
  igstkLogMacro( DEBUG, "igstk::LandmarkRegistration::"
                 "RequestResetRegistration called...\n");
  this->m_StateMachine.PushInput( this->m_ResetRegistrationInput );
  this->m_StateMachine.ProcessInputs();
}

template <unsigned int TDimension>
void 
LandmarkRegistration<TDimension>::RequestComputeTransform()
{
  igstkLogMacro( DEBUG, "igstk::LandmarkRegistration::"
                 "RequestComputeTransform called...\n");
  this->m_StateMachine.PushInput( this->m_ComputeTransformInput );
  this->m_StateMachine.ProcessInputs();
}

/** Print Self function */
template <unsigned int TDimension>
void 
LandmarkRegistration<TDimension>::PrintSelf( std::ostream& os,
                                             itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
  os << indent << "Tracker Landmarks: " << std::endl;
  PointsContainerConstIterator fitr = m_TrackerLandmarks.begin();
  while( fitr != m_TrackerLandmarks.end() )
    {
    os << indent << *fitr << std::endl;
    ++fitr;
    }
  os << indent << "Image Landmarks: " << std::endl;
  PointsContainerConstIterator mitr = m_ImageLandmarks.begin();
  while( mitr != m_ImageLandmarks.end() )
    {
    os << indent << *mitr << std::endl;
    ++mitr;
    }
}

} // end namespace igstk

#endif


