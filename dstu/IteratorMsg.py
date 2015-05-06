#=========================================================================
# IteratorMsg
#=========================================================================

from pymtl import *

#-------------------------------------------------------------------------
# IteratorMsg
#-------------------------------------------------------------------------

class IteratorMsg( object ):

  def __init__( s, data_nbits, opaque_nbits=8 ):
    s.req  = IteratorReqMsg ( data_nbits, opaque_nbits)
    s.resp = IteratorRespMsg( data_nbits, opaque_nbits)

#-------------------------------------------------------------------------
# IteratorReqMsg
#-------------------------------------------------------------------------

class IteratorReqMsg( BitStructDefinition ):

  TYPE_READ  = 0
  TYPE_WRITE = 1

  def __init__( s, data_nbits, opaque_nbits=8 ):
    s.opaque     = BitField( opaque_nbits )
    s.type_      = BitField(            1 )
    s.ds_id      = BitField(           11 )
    s.iter       = BitField(           22 )
    s.field      = BitField(           32 )
    s.data       = BitField(   data_nbits )

  def mk_msg( s, opaque, type_, ds_id, iter, field, data ):
    msg        = s()
    msg.opaque = opaque
    msg.type_  = type_
    msg.ds_id  = ds_id
    msg.iter   = iter
    msg.field  = field
    msg.data   = data
    return msg

  def unpck( s, msg ):
    req       = s()
    req.value = msg
    return req

  def __str__( s ):

    if   s.type_ == IteratorReqMsg.TYPE_READ:
      return "{}:rd:{}:{}:{}:{}".format( s.opaque, s.ds_id, s.iter, s.field, ' '*(s.data.nbits/4) )
    elif s.type_ == IteratorReqMsg.TYPE_WRITE:
      return "{}:wr:{}:{}:{}:{}".format( s.opaque, s.ds_id, s.iter, s.field, s.data )

#-------------------------------------------------------------------------
# IteratorRespMsg
#-------------------------------------------------------------------------

class IteratorRespMsg( BitStructDefinition ):

  TYPE_READ  = 0
  TYPE_WRITE = 1

  def __init__( s, data_nbits, opaque_nbits=8 ):
    s.opaque     = BitField( opaque_nbits )
    s.type_      = BitField(            1 )
    s.data       = BitField(   data_nbits )
    s.ds_id      = BitField(           11 )

  def mk_msg( s, opaque, type_, data, ds_id ):
    msg        = s()
    msg.opaque = opaque
    msg.type_  = type_
    msg.data   = data
    msg.ds_id  = ds_id
    return msg

  def unpck( s, msg ):
    resp       = s()
    resp.value = msg
    return resp

  def __str__( s ):

    if   s.type_ == IteratorRespMsg.TYPE_READ:
      return "{}:rd:{}:{}".format( s.opaque, s.data, s.ds_id )
    elif s.type_ == IteratorRespMsg.TYPE_WRITE:
      return "{}:wr:{}:{}".format( s.opaque, ' '*(s.data.nbits/4), s.ds_id )

