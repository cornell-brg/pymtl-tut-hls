#=========================================================================
# IteratorMsg
#=========================================================================

from pymtl import *

#-------------------------------------------------------------------------
# IteratorMsg
#-------------------------------------------------------------------------

class IteratorMsg( object ):

  def __init__( s, data_nbits ):
    s.req  = IteratorReqMsg ( data_nbits )
    s.resp = IteratorRespMsg( data_nbits )

#-------------------------------------------------------------------------
# IteratorReqMsg
#-------------------------------------------------------------------------

class IteratorReqMsg( BitStructDefinition ):

  TYPE_READ  = 0
  TYPE_WRITE = 1

  def __init__( s, data_nbits ):
    s.type_      = BitField(         1  )
    s.iter       = BitField(         32 )
    s.field      = BitField(         32 )
    s.data       = BitField( data_nbits )

  def mk_req( s, type_, iter, field, data ):
    msg       = s()
    msg.type_ = type_
    msg.iter  = iter
    msg.field = field
    msg.data  = data
    return msg

  def unpck( s, msg ):
    req       = s()
    req.value = msg
    return req

  def __str__( s ):

    if   s.type_ == IteratorReqMsg.TYPE_READ:
      return "rd:{}:{}:{}".format( s.iter, s.field, ' '*(s.data.nbits/4) )
    elif s.type_ == IteratorReqMsg.TYPE_WRITE:
      return "wr:{}:{}:{}".format( s.iter, s.field, s.data )

#-------------------------------------------------------------------------
# IteratorRespMsg
#-------------------------------------------------------------------------

class IteratorRespMsg( BitStructDefinition ):

  TYPE_READ  = 0
  TYPE_WRITE = 1

  def __init__( s, data_nbits ):
    s.type_      = BitField(          1 )
    s.data       = BitField( data_nbits )

  def mk_resp( s, type_, data ):
    msg       = s()
    msg.type_ = type_
    msg.data  = data
    return msg

  def unpck( s, msg ):
    resp       = s()
    resp.value = msg
    return resp

  def __str__( s ):

    if   s.type_ == IteratorRespMsg.TYPE_READ:
      return "rd:{}".format( s.data )
    elif s.type_ == IteratorRespMsg.TYPE_WRITE:
      return "wr:{}".format( ' '*(s.data.nbits/4) )

