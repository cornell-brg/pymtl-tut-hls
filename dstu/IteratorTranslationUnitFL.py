#=========================================================================
# IteratorTranslationUnitFL
#=========================================================================

from pymtl      import *
from pclib.ifcs import InValRdyBundle, OutValRdyBundle
from pclib.cl   import InValRdyQueueAdapter, OutValRdyQueueAdapter
from pclib.fl   import BytesMemPortProxy

#-------------------------------------------------------------------------
# IteratorTranslationUnitFL
#-------------------------------------------------------------------------
class IteratorTranslationUnitFL( Model ):

  def __init__( s, cfg_ifc_types, accel_ifc_types, mem_ifc_types ):

    # Interfaces

    s.cfgreq   = InValRdyBundle  ( cfg_ifc_types.req   )
    s.cfgresp  = OutValRdyBundle ( cfg_ifc_types.resp  )

    s.xcelreq  = InValRdyBundle  ( xcel_ifc_types.req  )
    s.xcelresp = OutValRdyBundle ( xcel_ifc_types.resp )

    s.memreq   = OutValRdyBundle ( mem_ifc_types.req   )
    s.memresp  = InValRdyBundle  ( mem_ifc_types.resp  )

    # Adapters

    s.cfgreq_q   = InValRdyQueueAdapter  ( s.cfgreq  )
    s.cfgresp_q  = OutValRdyQueueAdapter ( s.cfgresp )

    s.xcelreq_q  = InValRdyQueueAdapter  ( s.xcelreq  )
    s.xcelresp_q = OutValRdyQueueAdapter ( s.xcelresp )

    s.mem = BytesMemPortProxy ( mem_ifc_types, s.memreq, s.memresp )

    # Internal State

    s.ds_type  = [0]*32  # stores the type of the data structure
    s.ds_table = [0]*32  # stores pointers to the data structure descriptors
    s.dt_table = [0]*32  # stores pointers to the data type descriptors

    # Data Structure types
    s.vector = 0 # vector
    s.list   = 1 # list

    #---------------------------------------------------------------------
    # Implementation
    #---------------------------------------------------------------------
    @s.tick_fl
    def logic():

      s.cfgreq_q.xtick()
      s.cfgresp_q.xtick()
      s.xcelreq_q.xtick()
      s.xcelresp_q.xtick()

      #-------------------------------------------------------------------
      # configure the ITU
      #-------------------------------------------------------------------
      if not s.cfgreq_q.empty() and not s.cfgresp_q.full():

        # get the configuration message
        req = s.cfgreq_q.deq()

        # check if it is a write request
        if req.type_ == 1:
          if not req.addr == 1:
            s.cfgresp_q.enq( cfg_ifc_types.resp.mk_msg( 1, 0 ) )

          # dstruct alloc
          # XXX: modify XcelMsg to have mk_resp/mk_req messages and commit
          # to gem5 directory as well...
          if   req.addr == 1:
            for idx,val in enumerate( s.ds_type ):
              if   val == 0:
                s.cfgresp_q( cfg_ifc_types.resp.mk_msg( 1, idx ) )
                s.ds_type[ idx ] = req.data
                break
              elif idx == ( len( s.ds_type ) - 1 ):
                s.cfgresp_q( cfg_ifc_types.resp.mk_msg( 1, -1 ) )

          # dstruct init ds_table
          elif req.addr == 2:
            s.ds_table[ req.id ] = req.data

          # dstruct init dt_table
          elif req.addr == 3:
            s.ds_table[ req.id ] = req.data

          # dstruct dealloc
          elif req.addr == 4:
            s.dstruct[ req.data ] = 0

      #-------------------------------------------------------------------
      # service the ASU
      #-------------------------------------------------------------------
      if not s.xcelreq_q.empty() and not s.xcelresp_q.full():

        # get the accelerator request
        xcel_req = s.xcelreq.deq()

        # translate the request and access memory
        # XXX: look at pydgin code and add the same here...

  #-----------------------------------------------------------------------
  # line_trace
  #-----------------------------------------------------------------------

  def line_trace( s ):
    return ""
