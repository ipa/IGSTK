/*=========================================================================

Program:   Image Guided Surgery Software Toolkit
Module:    igstkTrackerConfigurationGUIBase.h
Language:  C++
Date:      $Date$
Version:   $Revision$

Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __TrackerConfigurationGUIBase_h
#define __TrackerConfigurationGUIBase_h

#include "igstkTrackerConfiguration.h"

namespace igstk
{

class TrackerConfigurationGUIBase
{

public:
  
  typedef TrackerConfiguration::TrackerType TrackerType;

  TrackerType GetTrackerType() { return m_TrackerType; };

  virtual void SetConfiguration( TrackerConfiguration config )
  {
    m_TrackerConfiguration = config;
    InitializeGUI();
  }

  virtual void RequestGetConfiguration()
  {
    GetGUISetting();
    ConfigurationEvent confEvent;
    confEvent.Set( m_TrackerConfiguration );
    m_Reporter->InvokeEvent( confEvent );
  }

  igstkLoadedEventMacro( ConfigurationEvent, IGSTKEvent, TrackerConfiguration );

  unsigned long AddObserver( const ::itk::EventObject & event, ::itk::Command * observer );
  void  RemoveObserver( unsigned long tag );

  TrackerConfigurationGUIBase();
  virtual ~TrackerConfigurationGUIBase();

protected:
  
  // Tracker specific GUI class has to implement this methods
  virtual void InitializeGUI() = 0;
  virtual void GetGUISetting() = 0;

private:
  
  TrackerType               m_TrackerType;
  TrackerConfiguration      m_TrackerConfiguration;
  ::itk::Object::Pointer    m_Reporter;

};

} // end of name space
#endif