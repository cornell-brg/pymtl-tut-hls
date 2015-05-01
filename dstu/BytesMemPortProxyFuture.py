#=========================================================================
# BytesMemPortProxyFuture.py
#=========================================================================
# These classes provides the Bytes interface, but the implementation
# essentially turns reads/writes into memory requests sent over a
# port-based memory interface. We use greenlets to enable us to wait
# until the response has come back before returning to the function
# accessing the list.
#
# NOTE: This model supports BitStruct message types with opaque fields and
# handles delays in memory request as well as memory responses.

from greenlet import greenlet

class BytesMemPortProxy (object):

  #-----------------------------------------------------------------------
  # Constructor
  #-----------------------------------------------------------------------

  def __init__( s, mem_ifc_types, memreq, memresp ):

    s.mem_ifc_types  = mem_ifc_types

    # Shorter names

    s.mk_req         = s.mem_ifc_types.req.mk_msg
    s.mk_resp        = s.mem_ifc_types.resp.mk_msg
    s.rd             = s.mem_ifc_types.req.TYPE_READ
    s.wr             = s.mem_ifc_types.req.TYPE_WRITE
    s.data_nbytes    = ( s.mem_ifc_types.req.data.nbits )/8

    # References to the memory request and response ports

    s.memreq         = memreq
    s.memresp        = memresp

    s.trace          = " "

  #-----------------------------------------------------------------------
  # __getitem__
  #-----------------------------------------------------------------------

  def __getitem__( s, key ):

    # Calculate base address and length for request

    if isinstance( key, slice ):
      addr   = int(key.start)
      nbytes = int(key.stop) - int(key.start)
    else:
      addr   = int(key)
      nbytes = 1

    len_ = nbytes if nbytes < s.data_nbytes else 0

    # Create a memory request to send out memory request port

    s.trace = "r"
    s.memreq.msg.next  = s.mk_req( s.rd, 0, addr, len_, 0 )
    s.memreq.val.next  = 1
    s.memresp.rdy.next = 1

    # Yield so we wait at least one cycle for the ready/response

    greenlet.getcurrent().parent.switch(0)

    # If memory request is not ready yet then yeild

    s.memreq.val.next  = 1
    s.memresp.rdy.next = 1

    while not s.memreq.rdy:
      s.trace = ";"
      greenlet.getcurrent().parent.switch(0)

    # If memory response has not arrived, then yield

    s.memreq.val.next  = 0
    s.memresp.rdy.next = 1

    while not s.memresp.val:
      s.trace = ":"
      greenlet.getcurrent().parent.switch(0)

    # When memory response has arrived, return the corresponding data

    s.trace = " "
    s.memreq.val.next  = 0
    s.memresp.rdy.next = 0

    return s.memresp.msg.data[0:nbytes*8]

  #-----------------------------------------------------------------------
  # __setitem__
  #-----------------------------------------------------------------------

  def __setitem__( s, key, value ):

    # Calculate base address and length for request

    if isinstance( key, slice ):
      addr   = int(key.start)
      nbytes = int(key.stop) - int(key.start)
    else:
      addr   = int(key)
      nbytes = 1

    len_ = nbytes if nbytes < s.data_nbytes else 0

    # Create a memory request to send out memory request port

    s.trace = "w"
    s.memreq.msg.next  = s.mk_req( s.wr, 0, addr, len_, value )
    s.memreq.val.next  = 1
    s.memresp.rdy.next = 1

    # Yield so we wait at least one cycle for the response

    greenlet.getcurrent().parent.switch(0)

    # If memory request is not ready yet then yeild

    s.memreq.val.next  = 1
    s.memresp.rdy.next = 1

    while not s.memreq.rdy:
      s.trace = ";"
      greenlet.getcurrent().parent.switch(0)

    # If memory response has not arrived, then yield

    s.memreq.val.next  = 0
    s.memresp.rdy.next = 1

    while not s.memresp.val:
      s.trace = ":"
      greenlet.getcurrent().parent.switch(0)

    # When memory response has arrived, then we are done

    s.trace = " "
    s.memreq.val.next  = 0
    s.memresp.rdy.next = 0



  #-----------------------------------------------------------------------
  # line_trace
  #-----------------------------------------------------------------------

  def line_trace( s ):
    return s.trace

