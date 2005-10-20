/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkImageSpatialObjectRepresentationTest.cxx
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

#include "igstkImageSpatialObjectRepresentation.h"
#include "igstkImageSpatialObject.h"

#include "itkLogger.h"
#include "itkStdStreamLogOutput.h"

int igstkImageSpatialObjectRepresentationTest( int , char* [] )
{


  typedef short    PixelType;
  const unsigned int Dimension = 3;

  typedef igstk::ImageSpatialObject< 
                                PixelType, 
                                Dimension 
                                       > ImageSpatialObjectType;
  
  typedef igstk::ImageSpatialObjectRepresentation< 
                                  ImageSpatialObjectType 
                                                      >   RepresentationType;

  RepresentationType::Pointer  representation = RepresentationType::New();

  // Test error case
  representation->RequestSetImageSpatialObject( NULL );

  std::string name = representation->GetNameOfClass();

  std::cout << "Name of class = " << name << std::endl;

  return EXIT_SUCCESS;
}

