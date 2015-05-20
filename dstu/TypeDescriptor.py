#-------------------------------------------------------------------------
# TypeDescriptor.py
#-------------------------------------------------------------------------

from pymtl            import *

#-------------------------------------------------------------------------
# TypeDescriptor
#-------------------------------------------------------------------------
# XXX: Move this elsewhere as even ASU's may use them?

class TypeDescriptor( BitStructDefinition ):

  def __init__( s ):

    s.offset = BitField( 8 )
    s.size_  = BitField( 8 )
    s.type_  = BitField( 8 )
    s.fields = BitField( 8 )

  def unpck( s, msg ):
    dt_desc = s()
    dt_desc.value = msg
    return dt_desc

  def __str__( s ):
    return "{}:{}:{}:{}".format( s.offset, s.size_, s.type_, s.fields )
