#=========================================================================
# UserTypes
#=========================================================================

import struct

class TypeEnum( ):
  CHAR   = 1
  UCHAR  = 2
  SHORT  = 3
  USHORT = 4
  INT    = 5
  UINT   = 6
  FLOAT  = 7
  MAX_PRIMITIVE = 10
  POINT  = 11
  RGBA   = 12

#=========================================================================
# Point Type
#=========================================================================
class Point( ):
  
  def __init__( s, label=0, x=0, y=0 ):
    s.label = label
    s.x = x
    s.y = y
  
  def pack( s ):
    return struct.pack("hii", s.label, s.x, s.y)

