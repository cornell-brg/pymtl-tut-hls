//========================================================================
// MemStream
//========================================================================
// Input and output memory streams suitable for serializing and
// deserializing objects to either a test memory or a real hardware
// memory interface.

#ifndef MEM_MEM_STREAM_H
#define MEM_MEM_STREAM_H

#include "../mem/MemCommon.h"

namespace mem {

  //----------------------------------------------------------------------
  // OutMemStream
  //----------------------------------------------------------------------

  class OutMemStream {

   public:

    OutMemStream( unsigned int addr_, MemReqStream& memreq, MemRespStream& memresp );

    unsigned int   addr;
    MemReqStream&  m_memreq;
    MemRespStream& m_memresp;

  };

  //----------------------------------------------------------------------
  // InMemStream
  //----------------------------------------------------------------------

  class InMemStream {

   public:

    InMemStream( unsigned int addr_, MemReqStream& memreq, MemRespStream& memresp );

    unsigned int   addr;
    MemReqStream&  m_memreq;
    MemRespStream& m_memresp;

  };

  //----------------------------------------------------------------------
  // Insertion operator overloading
  //----------------------------------------------------------------------

  OutMemStream& operator<<( OutMemStream& os, bool           value );
  OutMemStream& operator<<( OutMemStream& os, unsigned char  value );
  OutMemStream& operator<<( OutMemStream& os,          char  value );
  OutMemStream& operator<<( OutMemStream& os, unsigned short value );
  OutMemStream& operator<<( OutMemStream& os,          short value );
  OutMemStream& operator<<( OutMemStream& os, unsigned int   value );
  OutMemStream& operator<<( OutMemStream& os,          int   value );

  //----------------------------------------------------------------------
  // Extraction operator overloading
  //----------------------------------------------------------------------

  InMemStream&  operator>>( InMemStream&  is,          bool&  value );
  InMemStream&  operator>>( InMemStream&  is, unsigned char&  value );
  InMemStream&  operator>>( InMemStream&  is,          char&  value );
  InMemStream&  operator>>( InMemStream&  is, unsigned short& value );
  InMemStream&  operator>>( InMemStream&  is,          short& value );
  InMemStream&  operator>>( InMemStream&  is, unsigned int&   value );
  InMemStream&  operator>>( InMemStream&  is,          int&   value );

}

#include "mem/MemStream.inl"
#endif /* MEM_MEM_STREAM_H */

