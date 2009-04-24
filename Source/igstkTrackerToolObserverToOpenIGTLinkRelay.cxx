/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkTrackerToolObserverToOpenIGTLinkRelay.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
// Disabling warning C4355: 'this' : used in base member initializer list
#if defined(_MSC_VER)
#pragma warning ( disable : 4355 )
#endif

#include "igstkTrackerToolObserverToOpenIGTLinkRelay.h"
#include "igstkCoordinateSystemTransformToResult.h"

#include "igstkEvents.h"
#include "igstkTrackerTool.h"
#include "vtkMatrix4x4.h"

namespace igstk
{

/** Constructor */
TrackerToolObserverToOpenIGTLinkRelay::TrackerToolObserverToOpenIGTLinkRelay():m_StateMachine(this)
{
  this->m_Observer = ObserverType::New();
  this->m_Observer->SetCallbackFunction( this, & Self::ResendTransformThroughOpenIGTLink );

  this->m_Matrix = vtkMatrix4x4::New();

  this->m_Socket = igtl::ClientSocket::New(); 
  this->m_TransformMessage = igtl::TransformMessage::New();
  this->m_TransformMessage->SetDeviceName("Tracker");
}

TrackerToolObserverToOpenIGTLinkRelay::~TrackerToolObserverToOpenIGTLinkRelay()
{
  this->m_Observer = NULL; // FIXME also disconnect as an observer

//  this->m_SocketController->Delete();
//  this->m_SocketCommunicator->Delete();
  this->m_Matrix->Delete();

//  this->m_SocketController = NULL;
//  this->m_SocketCommunicator = NULL;
  this->m_Matrix = NULL;

  this->m_FramesPerSecond = 1.0;

  this->m_Socket->CloseSocket();
}


void
TrackerToolObserverToOpenIGTLinkRelay::RequestSetPort( int port )
{
  this->m_Port = port;
}


void
TrackerToolObserverToOpenIGTLinkRelay::RequestSetFramesPerSecond( double fps )
{
  this->m_FramesPerSecond = fps;
}


void
TrackerToolObserverToOpenIGTLinkRelay::RequestSetHostName( const char * hostname )
{
  this->m_HostName = hostname;
}


void
TrackerToolObserverToOpenIGTLinkRelay::RequestSetTrackerTool( const TrackerTool * trackerTool )
{
  this->m_TrackerTool = trackerTool;
  this->m_TrackerTool->AddObserver( CoordinateSystemTransformToEvent(), this->m_Observer );
}


void
TrackerToolObserverToOpenIGTLinkRelay::RequestStart()
{
  char * hostname = const_cast< char * >( this->m_HostName.c_str() );

  std::cout << "Trying to connect to host = " << hostname << std::endl;
  std::cout << "In port = " << this->m_Port << std::endl;

  /*
  if( !this->m_SocketCommunicator->ConnectTo( hostname, this->m_Port ) )
    {
    std::cerr << "Client error: Could not connect to the server." << std::endl;
    }
    */

  //this->m_Tag = 0;
  this->m_Tag = 17;


  int r = this->m_Socket->ConnectToServer(hostname, this->m_Port);
  if (r != 0)
    {
    std::cerr << "Cannot connect to the server." << std::endl;
    exit(0);
    }
}



void
TrackerToolObserverToOpenIGTLinkRelay::ResendTransformThroughOpenIGTLink( itk::Object * caller, const itk::EventObject & event )
{
  std::cout << "TrackerToolObserverToOpenIGTLinkRelay::ResendTransformThroughOpenIGTLink() " << std::endl;

  const CoordinateSystemTransformToEvent * transformEvent =
    dynamic_cast< const CoordinateSystemTransformToEvent * >( &event );

  if( transformEvent )
    {
    igstk::CoordinateSystemTransformToResult transformCarrier = 
      transformEvent->Get();

    igstk::Transform transform = transformCarrier.GetTransform();

    std::cout << "Sending transform " << transform << std::endl;

    transform.ExportTransform( *(this->m_Matrix) );

    igtl::Matrix4x4 matrix;

    for (int i = 0; i < 4; i++)
      for (int j = 0; j < 4; j++)
        matrix[i][j] = (float) this->m_Matrix->GetElement(i, j);

    this->m_TransformMessage->SetMatrix(matrix);
    this->m_TransformMessage->Pack();

    int interval = (int) (1000.0 / this->m_FramesPerSecond);
    igtl::Sleep(interval); // wait
    this->m_Socket->Send(this->m_TransformMessage->GetPackPointer(), this->m_TransformMessage->GetPackSize());
    }
}


/** Print Self function */
void TrackerToolObserverToOpenIGTLinkRelay::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
  os << indent << "Port: " << this->m_Port << std::endl;
  os << indent << "Hostname: " << this->m_HostName << std::endl;
  os << indent << "Tag: " << this->m_Tag << std::endl;
}

}