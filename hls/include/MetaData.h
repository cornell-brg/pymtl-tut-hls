#ifndef METADATA_H
#define METADATA_H

#include "TypeEnum.h"

//----------------------------------------------------------------------
// Macros to manipulate the fields of the data-type descriptor
//----------------------------------------------------------------------
// The data-type descriptor currently has the following format
//
//  8 bits   8 bits 8 bits 8 bits
// +--------+------+------+------------+
// | offset | size | type | num_fields |
// +--------+------+------+------------+
//

#define OFFSET_MASK  0xff
#define SIZE_MASK    0xff
#define TYPE_MASK    0xff
#define FIELDS_MASK  0xff

#define OFFSET_SHIFT 0x18
#define SIZE_SHIFT   0x10
#define TYPE_SHIFT   0x8
#define FIELDS_SHIFT 0x0

#define SET_OFFSET( x, off  )  (x) = (((x) & ~(OFFSET_MASK<<OFFSET_SHIFT)) | ((off)<<OFFSET_SHIFT))
#define SET_SIZE( x, size )    (x) = (((x) & ~(SIZE_MASK<<SIZE_SHIFT))     | ((size)<<SIZE_SHIFT))
#define SET_TYPE( x, type )    (x) = (((x) & ~(TYPE_MASK<<TYPE_SHIFT))     | ((type)<<TYPE_SHIFT))
#define SET_FIELDS( x, field ) (x) = (((x) & ~(FIELDS_MASK<<FIELDS_SHIFT)) | ((field)<<FIELDS_SHIFT))

#define GET_OFFSET( x ) (((x)>>OFFSET_SHIFT) & OFFSET_MASK)
#define GET_SIZE( x ) (((x)>>SIZE_SHIFT) & SIZE_MASK)
#define GET_TYPE( x ) (((x)>>TYPE_SHIFT) & TYPE_MASK)
#define GET_FIELDS( x ) (((x)>>FIELDS_SHIFT) & FIELDS_MASK)

//----------------------------------------------------------------------
// MetaData
//----------------------------------------------------------------------
// MetaData object stores the information we want to pass to polymorphic
// hardware

// For now this is the maximum number of primitive fields a
// struct can have
#define MAX_FIELDS 6

class MetaData {
  private:
    unsigned int metadata[MAX_FIELDS];
  public:
    MetaData() {
      for (int i = 0; i < MAX_FIELDS; ++i)
        metadata[i] = 0;
    }
    MetaData( const MetaData& md ) {
      for (int i = 0; i < MAX_FIELDS; ++i)
        metadata[i] = md.metadata[i];
    }
   ~MetaData() { }

    void init( unsigned int data[] ) {
      for (int i = 0; i < MAX_FIELDS; ++i)
        metadata[i] = data[i];
    }
    const unsigned int* getDataLoc() const { return metadata; }
    const unsigned int  getData(int i) const { return metadata[i];  }
    void dump() const {
      unsigned offset = GET_OFFSET(metadata[0]);
      unsigned size = GET_SIZE(metadata[0]);
      unsigned type = GET_TYPE(metadata[0]);
      unsigned fields = GET_FIELDS(metadata[0]);
      printf ("offset:%d, size:%d, type:%d, fields:%d\n", offset, size, type, fields);
    }
};

//----------------------------------------------------------------------
// MetaCreator
// Creates meta-data for primitive types
//----------------------------------------------------------------------
template<typename T>
class MetaCreator {
  public:
    MetaCreator() {

      // Create a data-type descriptor
      unsigned int data[MAX_FIELDS];
      data[0] = 0;

      // Data-type descriptor
      //
      //  8 bits   8 bits 8 bits 8 bits
      // +--------+------+------+------------+
      // | offset | size | type | num_fields |
      // +--------+------+------+------------+

      SET_OFFSET( data[0], 0           );
      SET_SIZE  ( data[0], sizeof( T ) );
      SET_TYPE  ( data[0], TypeEnum<T>::get()); // Enum for types?
      SET_FIELDS( data[0], 0           );

      get()->init( data );
    }

    static MetaData* get() {
      static MetaData instance;
      return &instance;
    }
};

//----------------------------------------------------------------------
// Read MetaData
//----------------------------------------------------------------------
#include "MemIface.h"

void mem_read_metadata (volatile MemIfaceType &iface, MemAddrType addr, MetaData &metadata) {
  MemReqType req;
  MemRespType resp;
  unsigned mdata[MAX_FIELDS];

  // read the first field
  resp = mem_read (iface, addr, 0);
  mdata[0] = MEM_RESP_DATA(resp);
  ap_uint<8> n_fields = GET_FIELDS(mdata[0]);

  // read extra entries if struct
  for (ap_uint<8> i = 1; i < MAX_FIELDS; ++i) {
    if (i < n_fields) {
      addr += sizeof(unsigned);
      resp = mem_read (iface, addr, 0);
      mdata[i] = MEM_RESP_DATA(resp);
    }
  }

  metadata.init(mdata);
}

#endif
