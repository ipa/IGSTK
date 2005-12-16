/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkPivotCalibration.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more DEBUGrmation.

=========================================================================*/

#ifndef _igstkPivotCalibration_cxx
#define _igstkPivotCalibration_cxx


#include "igstkPivotCalibration.h"
#include "itkVersorRigid3DTransform.h"

namespace igstk
{

/** Constructor */
PivotCalibration::PivotCalibration() :
  m_StateMachine( this )
{
  // Set the state descriptors
  igstkAddStateMacro( IdleState );
  igstkAddStateMacro( SampleAddState );
  igstkAddStateMacro( CalibrationCalculatedState );
  igstkAddStateMacro( CalibrationZCalculatedState );

  // Set the input descriptors 
  igstkAddInputMacro( ResetCalibrationInput );
  igstkAddInputMacro( SampleInput );
  igstkAddInputMacro( CalculateCalibrationInput );
  igstkAddInputMacro( CalculateCalibrationZInput );
  igstkAddInputMacro( SimulatePivotPositionInput );
  igstkAddInputMacro( GetInputSampleInput );

  // Add transition  for idle state
  igstkAddTransitionMacro( IdleState, ResetCalibrationInput, IdleState, Reset );
  igstkAddTransitionMacro( IdleState, SampleInput, SampleAddState, AddSample );
  igstkAddTransitionMacro( IdleState, CalculateCalibrationInput, IdleState, NoAction );
  igstkAddTransitionMacro( IdleState, CalculateCalibrationZInput, IdleState, NoAction );
  igstkAddTransitionMacro( IdleState, SimulatePivotPositionInput, IdleState, NoAction );
  igstkAddTransitionMacro( IdleState, GetInputSampleInput, IdleState, NoAction );
  
  // Add transition  for RotationTranslationAdd state
  igstkAddTransitionMacro( SampleAddState, ResetCalibrationInput, IdleState, Reset );
  igstkAddTransitionMacro( SampleAddState, SampleInput, SampleAddState, AddSample );
  igstkAddTransitionMacro( SampleAddState, CalculateCalibrationInput, CalibrationCalculatedState, CalculateCalibration );
  igstkAddTransitionMacro( SampleAddState, CalculateCalibrationZInput, CalibrationZCalculatedState, CalculateCalibrationZ );
  igstkAddTransitionMacro( SampleAddState, SimulatePivotPositionInput, SampleAddState, NoAction );
  igstkAddTransitionMacro( SampleAddState, GetInputSampleInput, SampleAddState, GetInputSample );
  
  // Add transition  for CalibrationCalculated state
  igstkAddTransitionMacro( CalibrationCalculatedState, ResetCalibrationInput, IdleState, Reset );
  igstkAddTransitionMacro( CalibrationCalculatedState, SampleInput, SampleAddState, AddSample );
  igstkAddTransitionMacro( CalibrationCalculatedState, CalculateCalibrationInput, CalibrationCalculatedState, NoAction );
  igstkAddTransitionMacro( CalibrationCalculatedState, CalculateCalibrationZInput, CalibrationZCalculatedState, CalculateCalibrationZ );
  igstkAddTransitionMacro( CalibrationCalculatedState, SimulatePivotPositionInput, CalibrationCalculatedState, SimulatePivotPosition );
  igstkAddTransitionMacro( CalibrationCalculatedState, GetInputSampleInput, CalibrationCalculatedState, GetInputSample );

  // Add transition  for CalibrationZCalculated state
  igstkAddTransitionMacro( CalibrationZCalculatedState, ResetCalibrationInput, IdleState, Reset );
  igstkAddTransitionMacro( CalibrationZCalculatedState, SampleInput, SampleAddState, AddSample );
  igstkAddTransitionMacro( CalibrationZCalculatedState, CalculateCalibrationInput, CalibrationCalculatedState, CalculateCalibration );
  igstkAddTransitionMacro( CalibrationZCalculatedState, CalculateCalibrationZInput, CalibrationZCalculatedState, NoAction );
  igstkAddTransitionMacro( CalibrationZCalculatedState, SimulatePivotPositionInput, CalibrationZCalculatedState, SimulatePivotPosition );
  igstkAddTransitionMacro( CalibrationZCalculatedState, GetInputSampleInput, CalibrationZCalculatedState, GetInputSample );

  // Select the initial state of the state machine
  this->m_StateMachine.SelectInitialState( this->m_IdleState );

  // Finish the programming and get ready to run
  this->m_StateMachine.SetReadyToRun();

  // Allocte the container pointer for input samples
  this->m_VersorContainer = InputVersorContainerType::New();
  this->m_TranslationContainer = InputVectorContainerType::New();

  // Reset the initial state and variables
  this->Reset();

}

/** Destructor */
PivotCalibration::~PivotCalibration()
{

}

/** Print Self function */
void PivotCalibration::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);

  // Dump the calibration class information
  os << indent << "Pivot Calibration: " << std::endl;

  os << indent << "Number Of Samples: " << this->GetNumberOfSamples() << std::endl;

  os << indent << "Calibration Transform: " << this->m_CalibrationTransform << std::endl;
  
  os << indent << "Pivot Position: " << this->m_PivotPosition << std::endl;
  
  os << indent << "Calibration RMS: " << this->m_RMS << std::endl;

}

/** Method to return the number of samples */
unsigned int PivotCalibration
::GetNumberOfSamples() const
{
  igstkLogMacro( DEBUG, "igstk::PivotCalibration::GetNumberOfSamples called...\n" );

  return this->m_VersorContainer->Size();

}

/** Method to reset the calibration */
void PivotCalibration::NoAction()
{
}

/** Method to reset the calibration */
void PivotCalibration::Reset()
{
  igstkLogMacro( DEBUG, "igstk::PivotCalibration::Reset called...\n" );

  VersorType quaternion;
  VectorType translation;

  // Clear the input container for quaternion and translation
  this->m_VersorContainer->Initialize();
  this->m_TranslationContainer->Initialize();

  // Reset the calibration transform
  quaternion.SetIdentity();
  translation.Fill( 0.0);
  this->m_CalibrationTransform.SetTranslationAndRotation( translation, quaternion, 0.1, 1000);

  // Reset the pivot position 
  this->m_PivotPosition.Fill( 0.0);

  // Reset the RMS calibration error
  this->m_RMS = 0.0;

  // Reset the validation indicator
  this->m_ValidPivotCalibration = false;

  // Reset the validation indicator
  this->m_ValidInputSample = false;
}

/** Method to add the sample information */
void PivotCalibration::AddSample()
{
  igstkLogMacro( DEBUG, "igstk::PivotCalibration::AddSample called...\n" );
  
  this->InternalAddSample( 
                       this->m_VersorToBeSent, 
                       this->m_TranslationToBeSent );
}


/** Internal method to add the sample information */
void PivotCalibration
::InternalAddSample( const VersorType & quaternion, 
                                  const VectorType & translation )
{
  igstkLogMacro( DEBUG, "igstk::PivotCalibration::InternalAddSample called...\n" );

  // Push the quaternion sample into the input container
  this->m_VersorContainer->push_back( quaternion );

  // Push the translation sample into the input container
  this->m_TranslationContainer->push_back( translation );

  // Unvalid the calibration
  this->m_ValidPivotCalibration = false;

}

/** Internal method to calculate the calibration */
void PivotCalibration::InternalCalculateCalibration( unsigned int axis )
{
  /** Use the Moore-Penrose inverse to calculate the calibration matrix
   *  The algorithm used is from the paper "Freehand Ultrasound Calibration using
   *  an Electromagnetic Needle" by Hui Zhang, Filip Banovac, Kevin Cleary to be 
   *  published in SPIE MI 2006. 
   * 
   *  [ r00 r01 r02 tx][ Offset0 ]   [ x0 ]
   *  [ r10 r11 r12 ty][ Offset1 ]   [ y0 ]
   *  [ r20 r21 r22 tz][ Offset2 ] = [ z0 ]
   *  [  0   0   0   1][    1    ]   [  1 ]
   *
   *  After the transformation, the unknowns of [ Offset0 Offset1 Offset2 x0 y0 z0 ]' 
   *  can be calculated by 
   *  
   *  M * [ Offset0 Offset1 Offset2 x0 y0 z0]' = N
   *  [ Offset0 Offset1 Offset2 x0 y0 z0]' = (M' * M)^-1 * M' * N
   *  or [ Offset0 Offset1 Offset2 x0 y0 z0]' = SVD( M, N )
   *  RMS = sqrt( |M * [ Offset0 Offset1 Offset2 x0 y0 z0 ]' - N|^2 / num ) */   

  igstkLogMacro( DEBUG, "igstk::PivotCalibration::InternalCalculateCalibration called...\n" );

  unsigned int i, j, k;
  unsigned int r, c, num;

  // Set the number of sample, tow and column number of matrix
  num = this->GetNumberOfSamples();
  r = num * 3;
  c = 3 + axis;

  // Define the Vnl matrix and intermediate variables
  VnlMatrixType matrix(r, c);
  VnlVectorType x(c), b(r), br(r);  
  VersorType quat;
  VectorType translation;
  MatrixType rotMatrix;

  // Fill the matrix of M
  for (k = 0; k < num; k++)
    {
    quat = this->m_VersorContainer->GetElement(k);
    rotMatrix = quat.GetMatrix();

    for ( j = 0; j < 3; j++)
      {
      for ( i = 0; i < axis; i++)
        {
        matrix[3 * k + j][i] = rotMatrix[j][2 - i];
        }
      for ( i = 0; i < 3; i++)
        {        
        matrix[3 * k + j][i + axis] = 0.0;
        }
      matrix[3 * k + j][j + axis] = -1.0;
      }

    for ( j = 0; j < 3; j++)
      {
        b[3 * k + j] = -this->m_TranslationContainer->GetElement(k)[j];
      }
    }

  // Use SVD to solve the vector M * x = y
  VnlSVDType svd( matrix);
  x = svd.solve( b);

  // Extract the offset components
  translation.Fill( 0.0);
  for ( i = 0; i < axis; i++)
    {
    translation[2 - i] = x[i];
    }
  
  // Extract the pivot position
  for ( i = 0; i < 3; i++)
    {
    this->m_PivotPosition[i] = x[i + axis];
    }

  // Set the calibration matrix
  this->m_CalibrationTransform.SetTranslation(translation, 0.1, 1000);

  // Calculate the RMS error
  br = matrix * x - b;  
  this->m_RMS = sqrt( br.squared_magnitude() / num );

  // Set valid indicator
  this->m_ValidPivotCalibration = true;

}

/** Method to calculate the calibration */
void PivotCalibration::CalculateCalibration()
{
  igstkLogMacro( DEBUG, "igstk::PivotCalibration::CalculateCalibration called...\n" );

  this->InternalCalculateCalibration( 3);
}

/** Method to calculate the calibration along z-axis */
void PivotCalibration::CalculateCalibrationZ()
{
  igstkLogMacro( DEBUG, "igstk::PivotCalibration::CalculateCalibrationZ called...\n" );

  this->InternalCalculateCalibration( 1);
}

/** Calculate the simulated pivot position */
void PivotCalibration::SimulatePivotPosition()
{
  igstkLogMacro( DEBUG, "igstk::PivotCalibration::SimulatePivotPosition called...\n" );
  
  this->m_SimulatedPivotPositionToBeReceived = this->InternalSimulatePivotPosition( this->m_VersorToBeSent, this->m_TranslationToBeSent);
}

/** Internal function to calculate the simulated pivot position */
PivotCalibration::VectorType 
PivotCalibration::InternalSimulatePivotPosition( const VersorType & rotation, 
                                                 const VectorType & translation )
{
  igstkLogMacro( DEBUG, "igstk::PivotCalibration::InternalSimulatePivotPosition called...\n" );

  /** reconstruct the pivot position from any input translation and rotation
   * 
   *  Pos = Rotation * Offset + Translation
   *
   */

  typedef itk::VersorRigid3DTransform<double> RigidTransformType;

  RigidTransformType::Pointer rigidTransform = RigidTransformType::New();

  rigidTransform->SetRotation( rotation );
  rigidTransform->SetTranslation( translation );

  VectorType offset = this->GetCalibrationTransform().GetTranslation();

  VectorType rotatedOffset = rigidTransform->TransformVector( offset );

  rotatedOffset += translation;

  VectorType pivotPosition;
  pivotPosition.Fill( 0.0 );
  
  pivotPosition += rotatedOffset;

  return pivotPosition;

}

/** Get the rotation and translation inputed */
void PivotCalibration::GetInputSample()
{
  igstkLogMacro( DEBUG, "igstk::PivotCalibration::GetInputSample called...\n" );

  this->m_ValidInputSample = this->InternalGetInputSample( 
                                               this->m_InputIndexToBeSent, 
                                               this->m_VersorToBeReceived, 
                                               this->m_TranslationToBeReceived);
}

/** Internal method to get the rotation and translation inputed */
bool PivotCalibration
::InternalGetInputSample( unsigned int index, 
                                       VersorType & versor, 
                                       VectorType & translation )
{
  igstkLogMacro( DEBUG, "igstk::PivotCalibration::InternalGetInputSample called...\n" );

  if ( index >= 0 && index < this->GetNumberOfSamples() )
    {
    versor      = this->m_VersorContainer->GetElement( index);
    translation = this->m_TranslationContainer->GetElement( index);

    return true;
    }
  else
    {
    versor.SetIdentity();
    translation.Fill( 0.0);

    return false;
    }
}


/** Method to invoke the reset function */
void PivotCalibration::RequestReset()
{
  igstkLogMacro( DEBUG, "igstk::PivotCalibration::RequestReset called...\n" );

  this->m_StateMachine.PushInput( this->m_ResetCalibrationInput );
  this->m_StateMachine.ProcessInputs();
}

/** Method to invoke adding the sample */
void PivotCalibration
::RequestAddSample( const VersorType & versor, 
                                 const VectorType & translation )
{
  igstkLogMacro( DEBUG, "igstk::PivotCalibration::RequestAddSample called...\n" );
  
  this->m_VersorToBeSent = versor;
  this->m_TranslationToBeSent = translation;

  this->m_StateMachine.PushInput( this->m_SampleInput );
  this->m_StateMachine.ProcessInputs();
}

/** Method to invoke the calculation */
void PivotCalibration::RequestCalculateCalibration()
{
  igstkLogMacro( DEBUG, "igstk::PivotCalibration::RequestCalculateCalibration called...\n" );

  this->m_StateMachine.PushInput( this->m_CalculateCalibrationInput );
  this->m_StateMachine.ProcessInputs();

}

/** Method to invoke the calculation only along z-axis */
void PivotCalibration::RequestCalculateCalibrationZ()
{
  igstkLogMacro( DEBUG, "igstk::PivotCalibration::RequestCalculateCalibrationZ called...\n" );

  this->m_StateMachine.PushInput( this->m_CalculateCalibrationZInput );
  this->m_StateMachine.ProcessInputs();

}

/** Method to invoke to simulate the pivot position */
PivotCalibration::VectorType 
PivotCalibration::RequestSimulatePivotPosition( const VersorType & versor, 
                                                const VectorType & translation )
{
  igstkLogMacro( DEBUG, "igstk::PivotCalibration::RequestSimulatePivotPosition called...\n" );

  this->m_VersorToBeSent = versor;
  this->m_TranslationToBeSent = translation;

  this->m_StateMachine.PushInput( this->m_SimulatePivotPositionInput );
  this->m_StateMachine.ProcessInputs();

  return this->m_SimulatedPivotPositionToBeReceived;
}

/** Method to invoke to get the rotation and translation in the input container */
bool PivotCalibration
::RequestGetInputSample( unsigned int index, 
                                      VersorType & versor, 
                                      VectorType& translation )
{
  igstkLogMacro( DEBUG, "igstk::PivotCalibration::RequestGetInputSample called...\n" );

  this->m_InputIndexToBeSent = index;
  this->m_ValidInputSample = false;

  this->m_StateMachine.PushInput( this->m_GetInputSampleInput );
  this->m_StateMachine.ProcessInputs();

  versor = this->m_VersorToBeReceived;
  translation = this->m_TranslationToBeReceived;

  return this->m_ValidInputSample;

}

} // end namespace igstk

#endif
