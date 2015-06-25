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

  OPC_READ  = 0
  OPC_WRITE = 1
  OPC_INCR  = 2
  OPC_DECR  = 3

  def __init__( s, data_nbits, opaque_nbits=8 ):
    s.opaque     = BitField( opaque_nbits )
    s.ds_id      = BitField(           11 )
    s.opcode     = BitField(            4 )
    s.iter       = BitField(           22 )
    s.addr       = BitField(   data_nbits )
    s.data       = BitField(   data_nbits )

  def mk_msg( s, opaque, ds_id, opcode, iter, addr, data ):
    msg        = s()
    msg.opaque = opaque
    msg.ds_id  = ds_id
    msg.opcode = opcode
    msg.iter   = iter
    msg.addr   = addr
    msg.data   = data
    return msg

  def unpck( s, msg ):
    req       = s()
    req.value = msg
    return req

  def __str__( s ):

    if   s.opcode == IteratorReqMsg.OPC_READ:
      return "{}:{}:rd:{}:{}:{}".format( s.opaque, s.ds_id, s.iter, s.addr, ' '*(s.data.nbits/4) )
    elif s.opcode == IteratorReqMsg.OPC_WRITE:
      return "{}:{}:wr:{}:{}:{}".format( s.opaque, s.ds_id, s.iter, s.addr, s.data )
    elif s.opcode == IteratorReqMsg.OPC_INCR:
      return "{}:{}:++:{}:{}:{}".format( s.opaque, s.ds_id, s.iter, s.addr, ' '*(s.data.nbits/4) )
    elif s.opcode == IteratorReqMsg.OPC_DECR:
      return "{}:{}:--:{}:{}:{}".format( s.opaque, s.ds_id, s.iter, s.addr, ' '*(s.data.nbits/4) )

#-------------------------------------------------------------------------
# IteratorRespMsg
#-------------------------------------------------------------------------

class IteratorRespMsg( BitStructDefinition ):

  OPC_READ  = 0
  OPC_WRITE = 1
  OPC_INCR  = 2
  OPC_DECR  = 3

  def __init__( s, data_nbits, opaque_nbits=8 ):
    s.opaque     = BitField( opaque_nbits )
    s.ds_id      = BitField(           11 )
    s.opcode     = BitField(            4 )
    s.addr       = BitField(   data_nbits )
    s.data       = BitField(   data_nbits )

  def mk_msg( s, opaque, ds_id, opcode, addr, data ):
    msg        = s()
    msg.opaque = opaque
    msg.ds_id  = ds_id
    msg.opcode = opcode
    msg.addr   = addr
    msg.data   = data
    return msg

  def unpck( s, msg ):
    resp       = s()
    resp.value = msg
    return resp

  def __str__( s ):

    if   s.opcode == IteratorRespMsg.OPC_READ:
      return "{}:{}:rd:{}:{}".format( s.opaque, s.ds_id, s.addr, s.data )
    elif s.opcode == IteratorRespMsg.OPC_WRITE:
      return "{}:{}:wr:{}:{}".format( s.opaque, s.ds_id, s.addr, ' '*(s.data.nbits/4) )
    elif s.opcode == IteratorRespMsg.OPC_INCR:
      return "{}:{}:++:{}:{}".format( s.opaque, s.ds_id, s.addr, ' '*(s.data.nbits/4) )
    elif s.opcode == IteratorRespMsg.OPC_DECR:
      return "{}:{}:--:{}:{}".format( s.opaque, s.ds_id, s.addr, ' '*(s.data.nbits/4) )

