/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkSerialCommunicationTokens.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkSerialCommunicationTokens_h
#define __igstkSerialCommunicationTokens_h

#include "igstkToken.h"

namespace igstk
{

/** \class SerialCommunicationBaudRate
 * 
 * \brief Defines a compile-time token for every baud rate.
 *
 * This class enforces safety on the values that are admisible as baud rates
 * for the serial port configuration. The actual Baud Rate value is assigned at
 * construction time.
 *
 * \ingroup Communication
 */
class SerialCommunicationBaudRate : public Token
{
  public:

    typedef unsigned long int BaudRateType;

    typedef SerialCommunicationBaudRate Self;

  public:
    SerialCommunicationBaudRate( BaudRateType rate )
      {
      m_BaudRate = rate;  
      }

    ~SerialCommunicationBaudRate() 
      {}
    
    BaudRateType Get() const
      { 
      return m_BaudRate; 
      }
    
   const Self & operator=( const Self & token )
      { m_BaudRate = token.m_BaudRate; return *this; } 

   bool operator!=( const Self & token )
      { return m_BaudRate != token.m_BaudRate; }
        
   bool operator==( const Self & token )
      { return m_BaudRate == token.m_BaudRate; }
        
  private:
    
    BaudRateType m_BaudRate;

};

template< unsigned long NRate >
class SerialCommunicationBaudRateValued : public SerialCommunicationBaudRate
{
  public:
    SerialCommunicationBaudRateValued<NRate>():
               SerialCommunicationBaudRate(NRate) {};
};






/** \class SerialCommunicationDataBitsSize
 * 
 * \brief Defines a compile-time token for every size of data bits
 *
 * This class enforces safety on the values that are admisible as data bits size
 * for the serial port configuration. The actual data size value is assigned at
 * construction time.
 *
 * \ingroup Communication
 */
class SerialCommunicationDataBitsSize : public Token
{
  public:

    typedef unsigned int DataBitsSizeType;

    typedef SerialCommunicationDataBitsSize Self;

  public:
    SerialCommunicationDataBitsSize( DataBitsSizeType rate )
      {
      m_DataBitsSize = rate;  
      }

    ~SerialCommunicationDataBitsSize() 
      {}
    
    DataBitsSizeType Get() const
      { 
      return m_DataBitsSize; 
      }
    
   const Self & operator=( const Self & token )
      { m_DataBitsSize = token.m_DataBitsSize; return *this; } 

   bool operator!=( const Self & token )
      { return m_DataBitsSize != token.m_DataBitsSize; }
        
   bool operator==( const Self & token )
      { return m_DataBitsSize == token.m_DataBitsSize; }
        
  private:
    
    DataBitsSizeType  m_DataBitsSize;

};

template< unsigned int NDataSize >
class SerialCommunicationDataBitsSizeValued : public SerialCommunicationDataBitsSize
{
  public:
    SerialCommunicationDataBitsSizeValued< NDataSize >():
                 SerialCommunicationDataBitsSize(NDataSize) {};
};



/** \class SerialCommunicationParity
 * 
 * \brief Defines a compile-time token for the parity
 *
 * This class enforces safety on the values that are admisible as parity for
 * the serial port configuration. The actual parity value is assigned at
 * construction time.
 *
 * \ingroup Communication
 */
class SerialCommunicationParity : public Token
{
  public:

    typedef unsigned int ParityType;

    typedef SerialCommunicationParity Self;

  public:
    SerialCommunicationParity( ParityType parity )
      {
      m_Parity = parity;  
      }

    ~SerialCommunicationParity() 
      {}
    
    ParityType Get() const
      { 
      return m_Parity; 
      }
    
   const Self & operator=( const Self & token )
      { m_Parity = token.m_Parity; return *this; } 

   bool operator!=( const Self & token )
      { return m_Parity != token.m_Parity; }
        
   bool operator==( const Self & token )
      { return m_Parity == token.m_Parity; }
        
  private:
    
    ParityType  m_Parity;

};

template< unsigned int NParity >
class SerialCommunicationParityValued : public SerialCommunicationParity
{
  public:
    SerialCommunicationParityValued< NParity >():
                   SerialCommunicationParity(NParity) {};
};




/** \class SerialCommunicationStopBits
 * 
 * \brief Defines a compile-time token for the number of stop bits 
 *
 * This class enforces safety on the values that are admisible as stop bits
 * size for the serial port configuration. The actual data size value is
 * assigned at construction time.
 *
 * \ingroup Communication
 */
class SerialCommunicationStopBits : public Token
{
  public:

    typedef unsigned int StopBitsType;

    typedef SerialCommunicationStopBits Self;

  public:
    SerialCommunicationStopBits( StopBitsType rate )
      {
      m_StopBits = rate;  
      }

    ~SerialCommunicationStopBits() 
      {}
    
    StopBitsType Get() const
      { 
      return m_StopBits; 
      }
    
   const Self & operator=( const Self & token )
      { m_StopBits = token.m_StopBits; return *this; } 

   bool operator!=( const Self & token )
      { return m_StopBits != token.m_StopBits; }
        
   bool operator==( const Self & token )
      { return m_StopBits == token.m_StopBits; }
        
  private:
    
    StopBitsType  m_StopBits;

};

template< unsigned int NStopBits >
class SerialCommunicationStopBitsValued : public SerialCommunicationStopBits
{
  public:
    SerialCommunicationStopBitsValued< NStopBits >():
                 SerialCommunicationStopBits(NStopBits) {};
};




/** \class SerialCommunicationHandshake
 * 
 * \brief Defines a compile-time token for the type of handshake.
 *
 * This class enforces safety on the values that are admisible as handshake
 * protocols for the serial port configuration. The actual value is assigned at
 * construction time.
 *
 * \ingroup Communication
 */
class SerialCommunicationHandshake : public Token
{
  public:

    typedef unsigned int HandshakeType;

    typedef SerialCommunicationHandshake Self;

  public:
    SerialCommunicationHandshake( HandshakeType protocol )
      {
      m_Handshake = protocol;  
      }

    ~SerialCommunicationHandshake() 
      {}
    
    HandshakeType Get() const
      { 
      return m_Handshake; 
      }
    
   const Self & operator=( const Self & token )
      { m_Handshake = token.m_Handshake; return *this; } 

   bool operator!=( const Self & token )
      { return m_Handshake != token.m_Handshake; }
        
   bool operator==( const Self & token )
      { return m_Handshake == token.m_Handshake; }
        
  private:
    
    HandshakeType  m_Handshake;

};

template< unsigned int NHandshake >
class SerialCommunicationHandshakeValued : public SerialCommunicationHandshake
{
  public:
    SerialCommunicationHandshakeValued< NHandshake >():
                 SerialCommunicationHandshake(NHandshake) {};
};


} // end namespace igstk

#endif // __igstkSerialCommunicationBaudRate_h

