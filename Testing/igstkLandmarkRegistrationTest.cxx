/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkLandmarkRegistrationTest.cxx
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
   //Warning about: identifier was truncated to '255' characters in the debug information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif

#include <iostream>
#include "igstkLandmarkRegistration.h"

int igstkLandmarkRegistrationTest( int argv, char * argc[] )
{
    std::cout << "Testing igstk::LandmarkRegistration" << std::endl;

    typedef igstk::LandmarkRegistration<3>                                 LandmarkRegistrationType;
    typedef igstk::LandmarkRegistration<3>::LandmarkPointContainerType     LandmarkPointContainerType;
    typedef igstk::LandmarkRegistration<3>::LandmarkPointType              LandmarkPointType;
    typedef LandmarkRegistrationType::TransformType::OutputVectorType  OutputVectorType;

    LandmarkRegistrationType::Pointer landmarkRegister = LandmarkRegistrationType::New();    
    LandmarkPointContainerType  fpointcontainer;
    LandmarkPointContainerType  mpointcontainer;
    LandmarkPointType fixedPoint;
    LandmarkPointType movingPoint;


    // Define the 3D rigid body transformation 

    typedef itk::Rigid3DTransform< double > Rigid3DTransformType;

    Rigid3DTransformType::Pointer   rigid3DTransform = Rigid3DTransformType::New();

    Rigid3DTransformType::MatrixType mrotation;

    mrotation.SetIdentity();

    // define rotation angle
    const double angle = 40.0 * atan( 1.0f ) / 45.0;
    const double sinth = sin( angle );
    const double costh = cos( angle );

    // Rotation around the Z axis
    mrotation[0][0] =  costh;
    mrotation[0][1] =  sinth;
    mrotation[1][0] = -sinth;
    mrotation[1][1] =  costh;

    rigid3DTransform->SetRotationMatrix( mrotation );

    // Apply translation

    Rigid3DTransformType::OffsetType ioffset;
    ioffset.Fill( 10.0f );

    rigid3DTransform->SetOffset( ioffset );

    fixedPoint[0] =  25.0;
    fixedPoint[1] =  1.0;
    fixedPoint[2] =  15.0;

    fpointcontainer.push_back(fixedPoint);
  
    movingPoint = rigid3DTransform->TransformPoint(fixedPoint);
    mpointcontainer.push_back(movingPoint);

    fixedPoint[0] =  15.0;
    fixedPoint[1] =  21.0;
    fixedPoint[2] =  17.0;

    fpointcontainer.push_back(fixedPoint);

    movingPoint = rigid3DTransform->TransformPoint(fixedPoint);
    mpointcontainer.push_back(movingPoint);

    fixedPoint[0] =  14.0;
    fixedPoint[1] =  25.0;
    fixedPoint[2] =  11.0;

    fpointcontainer.push_back(fixedPoint);

    movingPoint = rigid3DTransform->TransformPoint(fixedPoint);
    mpointcontainer.push_back(movingPoint);

    fixedPoint[0] =  10.0;
    fixedPoint[1] =  11.0;
    fixedPoint[2] =  8.0;

    fpointcontainer.push_back(fixedPoint);

    movingPoint = rigid3DTransform->TransformPoint(fixedPoint);
    mpointcontainer.push_back(movingPoint);

    //Set the tracker and modality image coordinates
    landmarkRegister->SetTrackerImageLandmarks(fpointcontainer);
    landmarkRegister->SetModalityImageLandmarks(mpointcontainer);


    //Get the tracker and modailty image coordinates and compare with
    LandmarkPointContainerType  trackerLandmarkPointcontainer;
    LandmarkPointContainerType  modalityLandmarkPointcontainer;

    trackerLandmarkPointcontainer=landmarkRegister->GetTrackerImageLandmarks();
    modalityLandmarkPointcontainer=landmarkRegister->GetModalityImageLandmarks();


    OutputVectorType error;
    OutputVectorType::RealValueType tolerance = 0.00001;
    bool failed = false;

    LandmarkRegistrationType::PointsContainerConstIterator
      trackerlandmarkitb = fpointcontainer.begin();

    LandmarkRegistrationType::PointsContainerConstIterator
      trackerlandmarkita = trackerLandmarkPointcontainer.begin();

    while( trackerlandmarkitb != fpointcontainer.end() )
      {
      std::cout<<"Trackerlandmark passed:"<<*trackerlandmarkitb<<std::endl;
      std::cout<<"Trackerlandmark read "<<*trackerlandmarkita<<std::endl;
      error = *trackerlandmarkitb - *trackerlandmarkita;
      if( error.GetNorm() > tolerance )
        {
        failed = true;
        }
      ++trackerlandmarkitb;
      ++trackerlandmarkita;
      }

     if( failed )
      {
      std::cout << "Landmark coordinates are not properly set" << std::endl;
      std::cout << "  [FAILED]" << std::endl;
      return EXIT_FAILURE;
      }
    else
      {
      std::cout << "  Tracker landmark coordinates are properly set [PASSED]" << std::endl;
      }


    LandmarkRegistrationType::PointsContainerConstIterator
      modalitylandmarkitb = mpointcontainer.begin();

    LandmarkRegistrationType::PointsContainerConstIterator
      modalitylandmarkita = modalityLandmarkPointcontainer.begin();


    while( modalitylandmarkitb != mpointcontainer.end() )
      {
      std::cout<<"Modalitylandmark passed:"<<*modalitylandmarkitb<<std::endl;
      std::cout<<"Modalitylandmark read "<<*modalitylandmarkita<<std::endl;
      error = *modalitylandmarkitb - *modalitylandmarkita;
      if( error.GetNorm() > tolerance )
        {
        failed = true;
        }
      ++modalitylandmarkitb;
      ++modalitylandmarkita;
      }

     if( failed )
      {
      std::cout << "Landmark coordinates are not properly set" << std::endl;
      std::cout << "  [FAILED]" << std::endl;
      return EXIT_FAILURE;
      }
    else
      {
      std::cout << "  Modality landmark coordinates are properly set [PASSED]" << std::endl;
      }

    //Print out the two sets of coordinates 
    landmarkRegister->Print(std::cout);

    //Determine the transformation parameters
    landmarkRegister->EvaluateTransform();

    landmarkRegister->GetTransform()->DebugOn();

    //Check if the transformation parameters were evaluated correctely
    LandmarkRegistrationType::PointsContainerConstIterator
      fitr = fpointcontainer.begin();
    LandmarkRegistrationType::PointsContainerConstIterator
      mitr = mpointcontainer.begin();


    OutputVectorType errortr;
    tolerance = 0.1;
    failed = false;

    while( mitr != mpointcontainer.end() )
      {
      std::cout << "  Tracker image Landmark: " << *fitr << " Modality Image landmark " << *mitr
        << " Transformed trackerLandmark : " <<
        landmarkRegister->GetTransform()->TransformPoint( *fitr ) << std::endl;

      errortr = *mitr - landmarkRegister->GetTransform()->TransformPoint( *fitr);
      if( errortr.GetNorm() > tolerance )
        {
        failed = true;
        }
      ++mitr;
      ++fitr;
      }

     if( failed )
      {
      // Hang heads in shame
      std::cout << "  Tracker landmarks transformed by the transform did not match closely "
        << " enough with the modality image landmarks.  The transform computed was: ";
      landmarkRegister->GetTransform()->Print(std::cout);
      std::cout << "  [FAILED]" << std::endl;
      return EXIT_FAILURE;
      }
    else
      {
      std::cout << "  Landmark alignment using Rigid3D transform [PASSED]" << std::endl;
      }

    return EXIT_SUCCESS;
}


