#=========================================================================
# CoprocessorMsg
#=========================================================================

from pymtl import *

#-------------------------------------------------------------------------
# CoprocessorMsg
#-------------------------------------------------------------------------

class CoprocessorMsg( object ):

  def __init__( s, addr_nbits, data_nbits, id_nbits ):
    s.req  = CoprocessorReqMsg ( addr_nbits, data_nbits, id_nbits )
    s.resp = CoprocessorRespMsg(                       data_nbits )

#-------------------------------------------------------------------------
# CoprocessorReqMsg
#-------------------------------------------------------------------------

class CoprocessorReqMsg( BitStructDefinition ):

  def __init__( s, addr_nbits, data_nbits, id_nbits ):

    s.addr_nbits = addr_nbits
    s.data_nbits = data_nbits
    s.id_nbits   = id_nbits

    s.type_      = BitField(           1  )
    s.addr       = BitField( s.addr_nbits )
    s.data       = BitField( s.data_nbits )
    s.id         = BitField( s.id_nbits   )

  def mk_req( s, type_, addr, data, id ):

    msg          = s()
    msg.type_    = type_
    msg.addr     = addr
    msg.data     = data
    msg.id       = id
    return msg

  def unpck( s, msg ):

    req          = s()
    req.value    = msg
    return req

#-------------------------------------------------------------------------
# CoprocessorRespMsg
#-------------------------------------------------------------------------

class CoprocessorRespMsg( BitStructDefinition ):

  def __init__( s, data_nbits ):

    s.data_nbits = data_nbits

    s.type_      = BitField(           1  )
    s.data       = BitField( s.data_nbits )

  def mk_resp( s, type_, data ):

    msg          = s()
    msg.type_    = type_
    msg.data     = data
    return msg

  def unpck( s, msg ):

    resp         = s()
    resp.value   = msg
    return resp
