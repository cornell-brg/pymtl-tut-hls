//========================================================================
// MemMsg.h
//========================================================================
// Header file that defines the structs used for the memory interfaces

#include <ap_int.h>

#ifndef MEM_MEMMSG_H
#define MEM_MEMMSG_H

namespace mem {

  //----------------------------------------------------------------------
  // MemReqMsg
  //----------------------------------------------------------------------

  struct MemReqMsg {

    typedef ap_range_ref<77,false> BitSliceProxy;
    typedef ap_uint<77>            Bits;

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

    BitSliceProxy type() { return bits(76,74); }
    BitSliceProxy opq()  { return bits(73,66); }
    BitSliceProxy addr() { return bits(65,34); }
    BitSliceProxy len()  { return bits(33,32); }
    BitSliceProxy data() { return bits(31, 0); }

    //--------------------------------------------------------------------
    // field inspectors
    //--------------------------------------------------------------------

    BitSliceProxy type() const { return bits(76,74); }
    BitSliceProxy opq()  const { return bits(73,66); }
    BitSliceProxy addr() const { return bits(65,34); }
    BitSliceProxy len()  const { return bits(33,32); }
    BitSliceProxy data() const { return bits(31, 0); }

    //--------------------------------------------------------------------
    // constructors
    //--------------------------------------------------------------------

    MemReqMsg() : bits( 0 ) { }

    MemReqMsg( ap_uint<3> type_, ap_uint<8> opq_, ap_uint<32> addr_,
               ap_uint<2> len_, ap_uint<32> data_ )
      : bits( ( type_, opq_, addr_, len_, data_ ) )
    { }

  };

  //----------------------------------------------------------------------
  // MemRespMsg
  //----------------------------------------------------------------------

  struct MemRespMsg {

    typedef ap_range_ref<45,false> BitSliceProxy;
    typedef ap_uint<45>            Bits;

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

    BitSliceProxy type() { return bits(44,42); }
    BitSliceProxy opq()  { return bits(41,34); }
    BitSliceProxy len()  { return bits(33,32); }
    BitSliceProxy data() { return bits(31, 0); }

    //--------------------------------------------------------------------
    // field inspectors
    //--------------------------------------------------------------------

    BitSliceProxy type() const { return bits(44,42); }
    BitSliceProxy opq()  const { return bits(41,34); }
    BitSliceProxy len()  const { return bits(33,32); }
    BitSliceProxy data() const { return bits(31, 0); }

    //--------------------------------------------------------------------
    // constructors
    //--------------------------------------------------------------------

    MemRespMsg() : bits( 0 ) { }

    MemRespMsg( ap_uint<3> type_, ap_uint<8> opq_,
                ap_uint<2> len_, ap_uint<32> data_ )
      : bits( ( type_, opq_, len_, data_ ) )
    { }

  };

}

#endif
