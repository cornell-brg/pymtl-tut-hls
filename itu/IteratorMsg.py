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

  def __init__( s, data_nbits ):

    s.data_nbits = data_nbits

    s.type_      = BitField(           1  )
    s.ds_id      = BitField(           32 )
    s.index      = BitField(           32 )
    s.data       = BitField( s.data_nbits )

  def mk_req( s, type_, ds_id, index, data ):

    msg          = s()
    msg.type_    = type_
    msg.ds_id    = ds_id
    msg.index    = index
    msg.data     = data
    return msg

  def unpck( s, msg ):

    req          = s()
    req.value    = msg
    return req

#-------------------------------------------------------------------------
# IteratorRespMsg
#-------------------------------------------------------------------------

class IteratorRespMsg( BitStructDefinition ):

  def __init__( s, data_nbits ):

    s.data_nbits = data_nbits

    s.type_      = BitField(            1 )
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
