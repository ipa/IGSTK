/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkVascularNetworkObject.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "igstkVascularNetworkObject.h"

namespace igstk
{ 

/** Constructor */
VascularNetworkObject::VascularNetworkObject():m_StateMachine(this)
{
  igstkAddInputMacro( GetVessel );

  igstkAddStateMacro( Init );

  igstkAddTransitionMacro( Init,
                           GetVessel,
                           Init,
                           ReportVessel );

  igstkSetInitialStateMacro( Init );

  m_StateMachine.SetReadyToRun();

  m_VesselId = 0;
}

/** This function reports the vessel */
void VascularNetworkObject::ReportVesselProcessing()
{
  const VesselObjectType* vessel = 
           dynamic_cast<const VesselObjectType*>(this->GetObject(m_VesselId));

  VesselObjectModifiedEvent  event;
  event.Set( const_cast<VesselObjectType*>(vessel)  );
  this->InvokeEvent( event );
}

/** Destructor */
VascularNetworkObject::~VascularNetworkObject()  
{
}

/** Request to get a vessel */
void VascularNetworkObject::RequestGetVessel(unsigned long id)
{
  igstkLogMacro( DEBUG, 
              "igstk::VascularNetworkObject::RequestGetVessel called...\n");
  
  // Check that this is a valid id
  if(id < this->GetNumberOfObjects())
    {
    m_VesselId = id;
    m_StateMachine.PushInput(m_GetVesselInput);
    m_StateMachine.ProcessInputs();
    }
}

/** Print object information */
void VascularNetworkObject
::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
}


} // end namespace igstk
