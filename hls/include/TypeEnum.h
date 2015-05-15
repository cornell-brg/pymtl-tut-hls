#ifndef TYPE_ENUM_H
#define TYPE_ENUM_H

//----------------------------------------------------------------------
// Type enum
//----------------------------------------------------------------------
enum TYPE_ENUM {
  ERROR,
  TYPE_CHAR,
  TYPE_UCHAR,
  TYPE_SHORT,
  TYPE_USHORT,
  TYPE_INT,
  TYPE_UINT,
  TYPE_FLOAT,
  TYPE_POINT
};

template <typename T>
struct TypeEnum {
  static TYPE_ENUM get() { return ERROR; }
};

template<>
struct TypeEnum<char> {
  static TYPE_ENUM get() { return TYPE_CHAR; }
};

template<>
struct TypeEnum<unsigned char> {
  static TYPE_ENUM get() { return TYPE_UCHAR; }
};

template<>
struct TypeEnum<unsigned> {
  static TYPE_ENUM get() { return TYPE_UINT; }
};

template<>
struct TypeEnum<int> {
  static TYPE_ENUM get() { return TYPE_INT; }
};

#endif