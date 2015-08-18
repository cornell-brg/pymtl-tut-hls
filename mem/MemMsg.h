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

  template<int data_nbits=32, int len_nbits=2, int opq_nbits=8>
  struct MemReqMsg {

    // field widths
    static const unsigned type_nbits =  3;
    static const unsigned addr_nbits = 32;

    // field msb's lsb's
    static const unsigned data_lsb  = 0;
    static const unsigned data_msb  = data_nbits - 1;

    static const unsigned len_lsb   = data_msb + 1;
    static const unsigned len_msb   = data_msb + len_nbits;

    static const unsigned addr_lsb  = len_msb + 1;
    static const unsigned addr_msb  = len_msb + addr_nbits;

    static const unsigned opq_lsb   = addr_msb + 1;
    static const unsigned opq_msb   = addr_msb + opq_nbits;

    static const unsigned type_lsb  = opq_msb + 1;
    static const unsigned type_msb  = opq_msb + type_nbits;

    static const unsigned msg_nbits = type_msb + 1;

    // typedef declarations
    typedef ap_range_ref<msg_nbits,false> BitSliceProxy;
    typedef ap_uint<msg_nbits>            Bits;

    //--------------------------------------------------------------------
    // message type
    //--------------------------------------------------------------------

    enum {
      TYPE_READ,
      TYPE_WRITE,
      TYPE_AMO_OR,
      TYPE_AMO_ADD,
      TYPE_AMO_AND,
      TYPE_AMO_XCHG,
      TYPE_AMO_MIN
    };

    //--------------------------------------------------------------------
    // message bits
    //--------------------------------------------------------------------

    Bits bits;

    //--------------------------------------------------------------------
    // field mutators
    //--------------------------------------------------------------------

    BitSliceProxy type() { return bits( type_msb, type_lsb ); }
    BitSliceProxy opq()  { return bits(  opq_msb,  opq_lsb ); }
    BitSliceProxy addr() { return bits( addr_msb, addr_lsb ); }
    BitSliceProxy len()  { return bits(  len_msb,  len_lsb ); }
    BitSliceProxy data() { return bits( data_msb, data_lsb ); }

    //--------------------------------------------------------------------
    // field inspectors
    //--------------------------------------------------------------------

    BitSliceProxy type() const { return bits( type_msb, type_lsb ); }
    BitSliceProxy opq()  const { return bits(  opq_msb,  opq_lsb ); }
    BitSliceProxy addr() const { return bits( addr_msb, addr_lsb ); }
    BitSliceProxy len()  const { return bits(  len_msb,  len_lsb ); }
    BitSliceProxy data() const { return bits( data_msb, data_lsb ); }

    //--------------------------------------------------------------------
    // constructors
    //--------------------------------------------------------------------

    MemReqMsg() : bits( 0 ) { }

    MemReqMsg( ap_uint<type_nbits> type_, ap_uint<opq_nbits> opq_,
               ap_uint<addr_nbits> addr_, ap_uint<len_nbits> len_,
               ap_uint<data_nbits> data_ )
      : bits( ( type_, opq_, addr_, len_, data_ ) )
    { }

  };

  //----------------------------------------------------------------------
  // MemRespMsg
  //----------------------------------------------------------------------

  template<int data_nbits=32, int len_nbits=2, int opq_nbits=8>
  struct MemRespMsg {

    // field widths
    static const unsigned type_nbits =  3;

    // field msb's lsb's
    static const unsigned data_lsb  = 0;
    static const unsigned data_msb  = data_nbits - 1;

    static const unsigned len_lsb   = data_msb + 1;
    static const unsigned len_msb   = data_msb + len_nbits;

    static const unsigned opq_lsb   = len_msb + 1;
    static const unsigned opq_msb   = len_msb + opq_nbits;

    static const unsigned type_lsb  = opq_msb + 1;
    static const unsigned type_msb  = opq_msb + type_nbits;

    static const unsigned msg_nbits = type_msb + 1;

    // typedef declarations
    typedef ap_range_ref<msg_nbits,false> BitSliceProxy;
    typedef ap_uint<msg_nbits>            Bits;

    //--------------------------------------------------------------------
    // message type
    //--------------------------------------------------------------------

    enum {
      TYPE_READ,
      TYPE_WRITE,
      TYPE_AMO_OR,
      TYPE_AMO_ADD,
      TYPE_AMO_AND,
      TYPE_AMO_XCHG,
      TYPE_AMO_MIN
    };

    //--------------------------------------------------------------------
    // message bits
    //--------------------------------------------------------------------

    Bits bits;

    //--------------------------------------------------------------------
    // field mutators
    //--------------------------------------------------------------------

    BitSliceProxy type() { return bits( type_msb, type_lsb ); }
    BitSliceProxy opq()  { return bits(  opq_msb,  opq_lsb ); }
    BitSliceProxy len()  { return bits(  len_msb,  len_lsb ); }
    BitSliceProxy data() { return bits( data_msb, data_lsb ); }

    //--------------------------------------------------------------------
    // field inspectors
    //--------------------------------------------------------------------

    BitSliceProxy type() const { return bits( type_msb, type_lsb ); }
    BitSliceProxy opq()  const { return bits(  opq_msb,  opq_lsb ); }
    BitSliceProxy len()  const { return bits(  len_msb,  len_lsb ); }
    BitSliceProxy data() const { return bits( data_msb, data_lsb ); }

    //--------------------------------------------------------------------
    // constructors
    //--------------------------------------------------------------------

    MemRespMsg() : bits( 0 ) { }

    MemRespMsg( ap_uint<type_nbits> type_, ap_uint<opq_nbits> opq_,
                ap_uint<len_nbits> len_, ap_uint<data_nbits> data_ )
      : bits( ( type_, opq_, len_, data_ ) )
    { }

  };

}

#endif
