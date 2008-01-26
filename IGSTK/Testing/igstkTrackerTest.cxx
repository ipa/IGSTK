/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkTrackerTest.cxx
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
#pragma warning( disable : 4786 )
#endif

#include <iostream>
#include <fstream>

#include "igstkTracker.h"
#include "igstkTrackerTool.h"

namespace igstk
{

namespace TrackerTest
{

class DummyTracker : public Tracker
{
public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( DummyTracker, Tracker )

  typedef Superclass::TransformType           TransformType;
  typedef Superclass::ResultType              ResultType;


protected:


DummyTracker():m_StateMachine(this)
{
}

~DummyTracker()
{
}

ResultType InternalOpen( void )
{
  return SUCCESS;
}

ResultType InternalStartTracking( void )
{
  return SUCCESS;
}

ResultType InternalReset( void )
{
  return SUCCESS;
}

ResultType InternalStopTracking( void )
{
  return SUCCESS;
}

ResultType InternalDeactivateTools( void )
{
  return SUCCESS;
}

ResultType InternalClose( void )
{
  return SUCCESS;
}

ResultType 
VerifyTrackerToolInformation( TrackerToolType * trackerTool )
{
  return SUCCESS;
}

ResultType 
RemoveTrackerToolFromInternalDataContainers( TrackerToolType * trackerTool )
{
  return SUCCESS;
}

ResultType 
InternalUpdateStatus( void )
{
  igstkLogMacro( DEBUG, "DummyTracker::InternalUpdateStatus called ...\n");

  static double x = 0;
  static double y = 0;
  static double z = 0;

  typedef TrackerToolsContainerType::const_iterator  ConstIteratorType;

  TrackerToolsContainerType trackerToolContainer = this->GetTrackerToolContainer();
 
  ConstIteratorType inputItr = trackerToolContainer.begin();
  ConstIteratorType inputEnd = trackerToolContainer.end();
 
  typedef igstk::Transform   TransformType;
  TransformType transform;

  transform.SetToIdentity( this->GetValidityTime() );

  typedef TransformType::VectorType PositionType;
  PositionType  position;
  position[0] = x;
  position[1] = y;
  position[2] = z;

  typedef TransformType::ErrorType  ErrorType;
  ErrorType errorValue = 0.5; // +/- half millimeter Uncertainty

  transform.SetTranslation( position, errorValue, this->GetValidityTime() );

  // set the raw transform in all the tracker tools
  while( inputItr != inputEnd )
    {
    this->SetTrackerToolRawTransform( trackerToolContainer[inputItr->first], transform );
    this->SetTrackerToolTransformUpdate( trackerToolContainer[inputItr->first], true );
    ++inputItr;
    }
 
  x += 0.1;
  y += 0.1;
  z += 0.1;

  return SUCCESS;
}

ResultType 
InternalThreadedUpdateStatus( void )
{
  igstkLogMacro( DEBUG, "DummyTracker::InternalThreadedUpdateStatus called ...\n");
  return SUCCESS;
}

/** Print Self function */
void PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
}

};

}

}

int igstkTrackerTest( int, char * [] )
{
  igstk::RealTimeClock::Initialize();

  typedef igstk::TrackerTest::DummyTracker      TrackerType; 
  typedef igstk::TrackerTool                    TrackerToolType;
  typedef TrackerToolType::TransformType        TransformType;
    
  TrackerType::Pointer tracker = TrackerType::New();

  tracker->RequestOpen();
  tracker->RequestSetFrequency( 30 );

  TrackerToolType::Pointer trackerTool = TrackerToolType::New();

  trackerTool->RequestConfigure();
  trackerTool->RequestAttachToTracker( tracker );

  tracker->RequestStartTracking();
  tracker->RequestStopTracking();
  tracker->RequestClose();

  std::cout << tracker << std::endl;

  return EXIT_SUCCESS;
}
