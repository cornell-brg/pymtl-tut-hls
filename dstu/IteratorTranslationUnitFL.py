#=========================================================================
# IteratorTranslationUnitFL
#=========================================================================

from pymtl      import *
from pclib.ifcs import InValRdyBundle, OutValRdyBundle
from pclib.cl   import InValRdyQueue, OutValRdyQueue

from BytesMemPortProxyFuture import BytesMemPortProxy

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

#-------------------------------------------------------------------------
# IteratorTranslationUnitFL
#-------------------------------------------------------------------------
class IteratorTranslationUnitFL( Model ):

  DSTU_ID  = 3

  #-----------------------------------------------------------------------
  # Data Structure types
  #-----------------------------------------------------------------------

  VECTOR = 0 # vector
  LIST   = 1 # list

  def __init__( s, cfg_ifc_types, itu_ifc_types, mem_ifc_types ):

    #---------------------------------------------------------------------
    # Interfaces
    #---------------------------------------------------------------------

    s.cfgreq   = InValRdyBundle  ( cfg_ifc_types.req   )
    s.cfgresp  = OutValRdyBundle ( cfg_ifc_types.resp  )

    s.xcelreq  = InValRdyBundle  ( itu_ifc_types.req  )
    s.xcelresp = OutValRdyBundle ( itu_ifc_types.resp )

    s.memreq   = OutValRdyBundle ( mem_ifc_types.req   )
    s.memresp  = InValRdyBundle  ( mem_ifc_types.resp  )

    #---------------------------------------------------------------------
    # Adapters
    #---------------------------------------------------------------------
    # NOTE: Ideally we want the code to use the adapters as below.
    # Where the queue adapters are really _fl_ queue adapters and not _cl_
    # adapters. The FL queue adapter essentially must peek to see if there
    # if there is a message in the current cycle, pop it, and process the
    # message. It must not stall waiting for a valid message. We don't have
    # such a queue adapter yet...
    #
    #   s.cfgreq_q   = InValRdyQueueAdapter  ( s.cfgreq  )
    #   s.cfgresp_q  = OutValRdyQueueAdapter ( s.cfgresp )
    #   s.xcelreq_q  = InValRdyQueueAdapter  ( s.xcelreq  )
    #   s.xcelresp_q = OutValRdyQueueAdapter ( s.xcelresp )
    #
    # As a workaround, I am using the CL queues instead which do not assert
    # a high rdy cycle ( that is they do not act like bypass queues for
    # InValRdyQueueAdapters ), that causes xcels to inject messages and we
    # essentially drop them. This happens because I am using a FL memory
    # port adapter and the code stalls in the logic() block while the
    # src/xcel can inject another message...It's hacky but works for now.

    # Adapter for the CFG request val-rdy bundle
    s.cfgreq_q  = InValRdyQueue  ( cfg_ifc_types.req  )
    s.connect( s.cfgreq_q.in_, s.cfgreq )

    # Adapter for the CFG response val-rdy bundle
    s.cfgresp_q = OutValRdyQueue ( cfg_ifc_types.resp )
    s.connect( s.cfgresp_q.out, s.cfgresp )

    # Adapter for the XCEL request val-rdy bundle
    s.xcelreq_q  = InValRdyQueue  ( itu_ifc_types.req  )
    s.connect( s.xcelreq_q.in_, s.xcelreq )

    # Adapter for the XCEL response val-rdy bundle
    s.xcelresp_q = OutValRdyQueue ( itu_ifc_types.resp )
    s.connect( s.xcelresp_q.out, s.xcelresp )

    # Memory port adapter
    s.mem = BytesMemPortProxy ( mem_ifc_types, s.memreq, s.memresp )

    #---------------------------------------------------------------------
    # Internal State
    #---------------------------------------------------------------------

    s.ds_type  = [0]*32  # stores the type of the data structure
    s.ds_table = [0]*32  # stores pointers to the data structure descriptors
    s.dt_table = [0]*32  # stores pointers to the data type descriptors

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
      # Configure the ITU
      #-------------------------------------------------------------------
      if not s.cfgreq_q.empty() and not s.cfgresp_q.full():

        # get the configuration message
        req = s.cfgreq_q.deq()

        # check if it is a write request
        if req.type_ == 1:

          # dstruct alloc
          # check for the xcel id to match DSTU id
          if   req.raddr == 1 and req.id == s.DSTU_ID:
            for idx,val in enumerate( s.ds_type ):
              if   val == 0:
                # create a dynamic xcel-id
                idx = ( (1<<10) | idx ) & 0x3ff
                s.cfgresp_q.enq( cfg_ifc_types.resp.mk_msg( req.opaque, 1, idx, s.DSTU_ID) )
                s.ds_type[ idx ] = req.data
                break
              elif idx == ( len( s.ds_type ) - 1 ):
                s.cfgresp_q.enq( cfg_ifc_types.resp.mk_msg( req.opaque, 1, -1, s.DSTU_ID) )

          # dstruct dealloc
          # check for the xcel id to match DSTU id
          elif req.raddr == 2 and req.id == s.DSTU_ID:
            s.ds_type[ req.data ] = 0
            s.cfgresp_q.enq( cfg_ifc_types.resp.mk_msg( req.opaque, 1, 0, s.DSTU_ID) )

          # dstruct init ds_table
          # for dynamically created id's
          elif req.raddr == 1:
            print req.id, "WOOHOO"
            s.ds_table[ req.id ] = req.data
            s.cfgresp_q.enq( cfg_ifc_types.resp.mk_msg( req.opaque, 1, 0, req.id) )

          # dstruct init dt_table
          # for dynamically created id's
          elif req.raddr == 2:
            print req.id, "WOOHOO"
            s.dt_table[ req.id ] = req.data
            s.cfgresp_q.enq( cfg_ifc_types.resp.mk_msg( req.opaque, 1, 0, req.id) )

      #-------------------------------------------------------------------
      # Service the ASU
      #-------------------------------------------------------------------
      if not s.xcelreq_q.empty() and not s.xcelresp_q.full():

        # get the accelerator request
        xcel_req = s.xcelreq_q.deq()

        # look up the ds_type
        ds_type = s.ds_type[ xcel_req.ds_id ]

        #-----------------------------------------------------------------
        # Handle VECTOR
        #-----------------------------------------------------------------
        if ds_type == s.VECTOR:
          # get the base addr
          base_addr   = s.ds_table[ xcel_req.ds_id ]

          # get the metadata
          dt_desc_ptr = s.dt_table[ xcel_req.ds_id ]
          dt_value    = s.mem[dt_desc_ptr:dt_desc_ptr+4]
          dt_desc     = TypeDescriptor().unpck( dt_value )

          # PRIMITIVE TYPES
          if dt_desc.type_ == 0:
            mem_addr = base_addr + ( xcel_req.iter * dt_desc.size_ )

            if   xcel_req.type_ == itu_ifc_types.req.TYPE_READ:
              mem_data = s.mem[mem_addr:mem_addr+dt_desc.size_]
              s.xcelresp_q.enq( itu_ifc_types.resp.mk_msg( xcel_req.opaque, 0, mem_data ) )

            elif xcel_req.type_ == itu_ifc_types.req.TYPE_WRITE:
              s.mem[mem_addr:mem_addr+dt_desc.size_] = xcel_req.data
              s.xcelresp_q.enq( itu_ifc_types.resp.mk_msg( xcel_req.opaque, 1, 0 ) )

        #-----------------------------------------------------------------
        # Handle LIST
        #-----------------------------------------------------------------
        elif ds_type == s.LIST:
          # get the node ptr
          node_ptr   = s.ds_table[ xcel_req.ds_id ]

          # get the metadata
          dt_desc_ptr = s.dt_table[ xcel_req.ds_id ]
          dt_value    = s.mem[dt_desc_ptr:dt_desc_ptr+4]
          dt_desc     = TypeDescriptor().unpck( dt_value )

          # PRIMITIVE TYPES
          if dt_desc.type_ == 0:

            # pointer chasing for intermediate nodes
            if not xcel_req.iter == 0:
              for i in xrange( xcel_req.iter ):
                # +-----+------+------+
                # | val | prev | next |
                # +-----+------+------+
                node_ptr = s.mem[node_ptr+8:node_ptr+12]

            if   xcel_req.type_ == itu_ifc_types.req.TYPE_READ:
              mem_data = s.mem[node_ptr:node_ptr+dt_desc.size_]
              s.xcelresp_q.enq( itu_ifc_types.resp.mk_msg( xcel_req.opaque, 0, mem_data ) )

            elif xcel_req.type_ == itu_ifc_types.req.TYPE_WRITE:
              s.mem[node_ptr:node_ptr+dt_desc.size_] = xcel_req.data
              s.xcelresp_q.enq( itu_ifc_types.resp.mk_msg( xcel_req.opaque, 1, 0 ) )


  #-----------------------------------------------------------------------
  # line_trace
  #-----------------------------------------------------------------------

  def line_trace( s ):
    return " {} | {} {} | {}".format( s.xcelreq,
                                      s.memreq,
                                      s.memresp,
                                      s.xcelresp )
