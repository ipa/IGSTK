/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkCoordinateReferenceSystemDelegator.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkCoordinateReferenceSystemDelegator_h
#define __igstkCoordinateReferenceSystemDelegator_h

#include "igstkObject.h"
#include "igstkMacros.h"
#include "igstkStateMachine.h"
#include "igstkCoordinateReferenceSystem.h"

namespace igstk
{

/** \class CoordinateReferenceSystemDelegator
 *
 *  \brief Encapsulates common logic for handling coordinate reference system
 *  calls on objects with CoordinateReferenceSystems. Most of
 *  the functionality is delegated to CoordinateReferenceSystem. This class
 *  is intended to be used inside classes like SpatialObject and View. This
 *  class delegates much of the core coordinate system connectivity and
 *  transformation work to the CoordinateReferenceSystem class.
 */
class CoordinateReferenceSystemDelegator : public Object
{
public: 

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( CoordinateReferenceSystemDelegator, Object )
  
  /** Set the object to use for reporting events. */
  void RequestSetReporter( const Object * reporter );

  /** This method implements the construction of a coordinate system graph by 
   *  defining the parent of this object and the Transforms defining their
   *  relative position and orientation */
  template < class TParent >
  void RequestSetTransformAndParent( const Transform & transformToParent, 
                                     const TParent * parent )
    {
    if (parent == NULL)
      {
      igstkPushInputMacro( NullParent );
      m_StateMachine.ProcessInputs();
      return;
      }
    else
      {
      const CoordinateReferenceSystem* parentReferenceSystem = 
      igstk::Friends::CoordinateReferenceSystemHelper
                                    ::GetCoordinateReferenceSystem( parent );

      /** Use event observer to handle the return event */
      this->m_CoordinateReferenceSystem->RequestSetTransformAndParent(
                                                      transformToParent,
                                                      parentReferenceSystem);

      return;
      }
    }

  /** Returns the transform to the parent if available. */
  void RequestGetTransformToParent()
    {
    /** Handle returns with event observer */
    this->m_CoordinateReferenceSystem->RequestGetTransformToParent();
    }

  /**
   * Tries to compute the transformation from this coordinate system
   * to another coordinate system. This method is templated on the 
   * input and should allow computation of a transform to another
   * object that internally has a CoordinateReferenceSystem. 
   *
   * Three types of events may be generated by this call:
   *     CoordinateReferenceSystemTransformToEvent
   *     CoordinateReferenceSystemTransformToNullTargetEvent
   *     CoordinateReferenceSystemTransformToDisconnectedEvent
   */
  template <class TTarget>
  void RequestComputeTransformTo(const TTarget * target)
    {
    if (NULL == target)
      {
      igstkPushInputMacro( NullTarget );
      m_StateMachine.ProcessInputs();
      return;
      }

    /** First get the coordinate system from the target object. */
    const CoordinateReferenceSystem* targetCoordSys = 
      igstk::Friends::CoordinateReferenceSystemHelper::
                                      GetCoordinateReferenceSystem( target );

    /** Handle the response with event observers */
    this->m_CoordinateReferenceSystem->RequestComputeTransformTo(
                                                             targetCoordSys);
    }

protected:
  CoordinateReferenceSystemDelegator();
  ~CoordinateReferenceSystemDelegator();

private:
  /** Purposely not implemented. */
  CoordinateReferenceSystemDelegator
                                (const CoordinateReferenceSystemDelegator& );

  /** Purposely not implemented. */
  CoordinateReferenceSystemDelegator& operator= 
                                 (const CoordinateReferenceSystemDelegator&);

  /** One state */
  igstkDeclareStateMacro( Idle );

  /** A few inputs. */
  igstkDeclareInputMacro( NullParent );
  igstkDeclareInputMacro( NullTarget );

  /** Declaring frienship with helper that will facilitate enforcing the
   *  privacy of the CoordinateReferenceSystem. 
   */
  igstkFriendClassMacro( igstk::Friends::CoordinateReferenceSystemHelper );

  /** Private method for getting the CoordinateReferenceSystem. This method 
   *  is mainly intended to be called from the 
   *  CoordinateReferenceSystemHelper as a secure way of passing the
   *  CoordinateReferenceSystem without breaking its encapsulation. 
   */
  const CoordinateReferenceSystem * GetCoordinateReferenceSystem() const;

  /** Pointer to a CoordinateReferenceSystem object for managing 
   *  coordinate system operations...
   */ 
  CoordinateReferenceSystem::Pointer      m_CoordinateReferenceSystem;

  /** Called when RequestSetTransformAndParent is passed a null parent. */
  void NullParentProcessing();

  /** Called when RequestComputeTransformTo is passed a null target. */
  void NullTargetProcessing();

  /** A pointer to an object through which to send events.
   *  By default, m_Reporter = this.
   */
  Object::ConstPointer m_Reporter;

  /** Typedef, Receptor observer, & callback for watching
   *  CoordinateReferenceSystem events.
   */
  typedef ::itk::ReceptorMemberCommand< Self > CoordinateSystemObserverType;

  /** Make an observer to watch events on the CoordinateReferenceSystem. */
  CoordinateSystemObserverType::Pointer m_CoordinateReferenceSystemObserver;

  /** Call back used by the coordinate system observer which
   *  catches events and re-invokes them.
   */
  void ObserverCallback(const ::itk::EventObject & eventvar);

}; // class CoordinateReferenceSystemDelegator

};

#endif


