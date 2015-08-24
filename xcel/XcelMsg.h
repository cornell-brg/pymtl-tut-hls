//========================================================================
// XcelMsg.h
//========================================================================
// Header file that defines the structs used for the accelerator interfaces.

#include <ap_int.h>

#ifndef XCEL_XCEL_MSG_H
#define XCEL_XCEL_MSG_H

namespace xcel {

  //----------------------------------------------------------------------
  // XcelReqMsg
  //----------------------------------------------------------------------

  struct XcelReqMsg {

    // field widths
    static const unsigned opq_nbits   =  8;
    static const unsigned type_nbits  =  1;
    static const unsigned addr_nbits  =  5;
    static const unsigned data_nbits  = 32;
    static const unsigned id_nbits    = 11;

    // field msb's lsb's
    static const unsigned id_lsb   = 0;
    static const unsigned id_msb   = id_nbits - 1;

    static const unsigned data_lsb = id_msb + 1;
    static const unsigned data_msb = id_msb + data_nbits;

    static const unsigned addr_lsb = data_msb + 1;
    static const unsigned addr_msb = data_msb + addr_nbits;

    static const unsigned type_lsb = addr_msb + 1;
    static const unsigned type_msb = addr_msb + type_nbits;

    static const unsigned opq_lsb  = type_msb + 1;
    static const unsigned opq_msb  = type_msb + opq_nbits;

    static const unsigned msg_nbits = opq_msb + 1;

    // typedef declarations
    typedef ap_range_ref<msg_nbits,false> BitSliceProxy;
    typedef ap_uint<msg_nbits>            Bits;

    //--------------------------------------------------------------------
    // message type
    //--------------------------------------------------------------------

    enum {
      TYPE_READ,
      TYPE_WRITE
    };

    //--------------------------------------------------------------------
    // message bits
    //--------------------------------------------------------------------

    Bits bits;

    //--------------------------------------------------------------------
    // field mutators
    //--------------------------------------------------------------------

    BitSliceProxy opq()   { return bits(   opq_msb,   opq_lsb ); }
    BitSliceProxy type()  { return bits(  type_msb,  type_lsb ); }
    BitSliceProxy addr()  { return bits(  addr_msb,  addr_lsb ); }
    BitSliceProxy data()  { return bits(  data_msb,  data_lsb ); }
    BitSliceProxy id()    { return bits(    id_msb,    id_lsb ); }

    //--------------------------------------------------------------------
    // field inspectors
    //--------------------------------------------------------------------

    BitSliceProxy opq()  const { return bits(   opq_msb,   opq_lsb ); }
    BitSliceProxy type() const { return bits(  type_msb,  type_lsb ); }
    BitSliceProxy addr() const { return bits(  addr_msb,  addr_lsb ); }
    BitSliceProxy data() const { return bits(  data_msb,  data_lsb ); }
    BitSliceProxy id()   const { return bits(    id_msb,    id_lsb ); }

    //--------------------------------------------------------------------
    // constructors
    //--------------------------------------------------------------------

    XcelReqMsg() : bits( 0 ) { }

    XcelReqMsg( ap_uint<opq_nbits>   opq, ap_uint<type_nbits> type,
                ap_uint<addr_nbits> addr, ap_uint<data_nbits> data,
                ap_uint<id_nbits> id )
      : bits( ( opq, type, addr, data, id ) )
    { }

  };

  //----------------------------------------------------------------------
  // XcelRespMsg
  //----------------------------------------------------------------------

  struct XcelRespMsg {

    // field widths
    static const unsigned opq_nbits   =  8;
    static const unsigned type_nbits  =  1;
    static const unsigned addr_nbits  =  5;
    static const unsigned data_nbits  = 32;
    static const unsigned id_nbits    = 11;

    // field msb's lsb's
    static const unsigned id_lsb   = 0;
    static const unsigned id_msb   = id_nbits - 1;

    static const unsigned data_lsb = id_msb + 1;
    static const unsigned data_msb = id_msb + data_nbits;

    static const unsigned type_lsb = data_msb + 1;
    static const unsigned type_msb = data_msb + type_nbits;

    static const unsigned opq_lsb  = type_msb + 1;
    static const unsigned opq_msb  = type_msb + opq_nbits;

    static const unsigned msg_nbits = opq_msb + 1;

    // typedef declarations
    typedef ap_range_ref<msg_nbits,false> BitSliceProxy;
    typedef ap_uint<msg_nbits>            Bits;

    //--------------------------------------------------------------------
    // message type
    //--------------------------------------------------------------------

    enum {
      TYPE_READ,
      TYPE_WRITE
    };

    //--------------------------------------------------------------------
    // message bits
    //--------------------------------------------------------------------

    Bits bits;

    //--------------------------------------------------------------------
    // field mutators
    //--------------------------------------------------------------------

    BitSliceProxy opq()   { return bits(   opq_msb,   opq_lsb ); }
    BitSliceProxy type()  { return bits(  type_msb,  type_lsb ); }
    BitSliceProxy data()  { return bits(  data_msb,  data_lsb ); }
    BitSliceProxy id()    { return bits(    id_msb,    id_lsb ); }

    //--------------------------------------------------------------------
    // field inspectors
    //--------------------------------------------------------------------

    BitSliceProxy opq()  const { return bits(   opq_msb,   opq_lsb ); }
    BitSliceProxy type() const { return bits(  type_msb,  type_lsb ); }
    BitSliceProxy data() const { return bits(  data_msb,  data_lsb ); }
    BitSliceProxy id()   const { return bits(    id_msb,    id_lsb ); }

    //--------------------------------------------------------------------
    // constructors
    //--------------------------------------------------------------------

    XcelRespMsg() : bits( 0 ) { }

    XcelRespMsg( ap_uint<opq_nbits>   opq, ap_uint<type_nbits> type,
                 ap_uint<data_nbits> data, ap_uint<id_nbits> id )
      : bits( ( opq, type, data, id ) )
    { }

  };

}

#endif /* XCEL_XCEL_MSG_H */

