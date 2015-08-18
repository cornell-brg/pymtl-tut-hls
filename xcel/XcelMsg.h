//========================================================================
// XcelMsg.h
//========================================================================
// Header file that defines the structs used for the accelerator interfaces.

#include <ap_int.h>

#ifndef XCEL_XCELMSG_H
#define XCEL_XCELMSG_H

namespace xcel {

  //----------------------------------------------------------------------
  // XcelReqMsg
  //----------------------------------------------------------------------

  struct XcelReqMsg {

    typedef ap_range_ref<57,false> BitSliceProxy;
    typedef ap_uint<57>            Bits;

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

    BitSliceProxy opq()  { return bits(56,49); }
    BitSliceProxy type() { return bits(48,48); }
    BitSliceProxy addr() { return bits(47,43); }
    BitSliceProxy data() { return bits(42,11); }
    BitSliceProxy id()   { return bits(10, 0); }

    //--------------------------------------------------------------------
    // field inspectors
    //--------------------------------------------------------------------

    BitSliceProxy opq()  const { return bits(56,49); }
    BitSliceProxy type() const { return bits(48,48); }
    BitSliceProxy addr() const { return bits(47,43); }
    BitSliceProxy data() const { return bits(42,11); }
    BitSliceProxy id()   const { return bits(10, 0); }

    //--------------------------------------------------------------------
    // constructors
    //--------------------------------------------------------------------

    XcelReqMsg() : bits( 0 ) { }

    XcelReqMsg( ap_uint<8> opq, ap_uint<1> type,
                ap_uint<5> addr, ap_uint<32> data, ap_uint<11> id )
      : bits( ( opq, type, addr, data, id ) )
    { }

  };

  //----------------------------------------------------------------------
  // XcelRespMsg
  //----------------------------------------------------------------------

  struct XcelRespMsg {

    typedef ap_range_ref<52,false> BitSliceProxy;
    typedef ap_uint<52>            Bits;

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

    BitSliceProxy opq()  { return bits(51,44); }
    BitSliceProxy type() { return bits(43,43); }
    BitSliceProxy data() { return bits(42,11); }
    BitSliceProxy id()   { return bits(10, 0); }

    //--------------------------------------------------------------------
    // field inspectors
    //--------------------------------------------------------------------

    BitSliceProxy opq()  const { return bits(51,44); }
    BitSliceProxy type() const { return bits(43,43); }
    BitSliceProxy data() const { return bits(43,12); }
    BitSliceProxy id()   const { return bits(11, 0); }

    //--------------------------------------------------------------------
    // constructors
    //--------------------------------------------------------------------

    XcelRespMsg() : bits( 0 ) { }

    XcelRespMsg( ap_uint<8> opq, ap_uint<1> type,
                 ap_uint<32> data, ap_uint<11> id )
      : bits( ( opq, type, data, id ) )
    { }

  };

}
#endif
