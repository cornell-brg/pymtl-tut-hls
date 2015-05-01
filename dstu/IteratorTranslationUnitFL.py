#=========================================================================
# IteratorTranslationUnitFL
#=========================================================================

from pymtl      import *
from pclib.ifcs import InValRdyBundle, OutValRdyBundle
from pclib.cl   import InValRdyQueueAdapter, OutValRdyQueueAdapter
from pclib.cl   import InValRdyQueue, OutValRdyQueue

from BytesMemPortProxyFuture import BytesMemPortProxy

#-------------------------------------------------------------------------
# TypeDescriptor
#-------------------------------------------------------------------------

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

  def __init__( s, cfg_ifc_types, itu_ifc_types, mem_ifc_types ):

    # Interfaces

    s.cfgreq   = InValRdyBundle  ( cfg_ifc_types.req   )
    s.cfgresp  = OutValRdyBundle ( cfg_ifc_types.resp  )

    s.xcelreq  = InValRdyBundle  ( itu_ifc_types.req  )
    s.xcelresp = OutValRdyBundle ( itu_ifc_types.resp )

    s.memreq   = OutValRdyBundle ( mem_ifc_types.req   )
    s.memresp  = InValRdyBundle  ( mem_ifc_types.resp  )

    # Adapters

    s.cfgreq_q   = InValRdyQueueAdapter  ( s.cfgreq  )
    s.cfgresp_q  = OutValRdyQueueAdapter ( s.cfgresp )

    #s.xcelreq_q  = InValRdyQueueAdapter  ( s.xcelreq  )
    #s.xcelresp_q = OutValRdyQueueAdapter ( s.xcelresp )

    s.xcelreq_q  = InValRdyQueue  ( itu_ifc_types.req  )
    s.xcelresp_q = OutValRdyQueue ( itu_ifc_types.resp )

    s.connect( s.xcelreq_q.in_, s.xcelreq )
    s.connect( s.xcelresp_q.out, s.xcelresp )

    s.mem = BytesMemPortProxy ( mem_ifc_types, s.memreq, s.memresp )

    # Internal State

    s.ds_type  = [0]*32  # stores the type of the data structure
    s.ds_table = [0]*32  # stores pointers to the data structure descriptors
    s.dt_table = [0]*32  # stores pointers to the data type descriptors

    # Data Structure types
    VECTOR = 0 # vector
    LIST   = 1 # list

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
          if not req.raddr == 1:
            s.cfgresp_q.enq( cfg_ifc_types.resp.mk_msg( 0, 1, 0, 3 ) )

          # dstruct alloc
          if   req.raddr == 1:
            for idx,val in enumerate( s.ds_type ):
              if   val == 0:
                s.cfgresp_q.enq( cfg_ifc_types.resp.mk_msg( 0, 1, idx, 3 ) )
                s.ds_type[ idx ] = req.data
                break
              elif idx == ( len( s.ds_type ) - 1 ):
                s.cfgresp_q.enq( cfg_ifc_types.resp.mk_msg( 0, 1, -1, 3 ) )

          # dstruct init ds_table
          elif req.raddr == 2:
            s.ds_table[ req.id ] = req.data

          # dstruct init dt_table
          elif req.raddr == 3:
            s.dt_table[ req.id ] = req.data

          # dstruct dealloc
          elif req.raddr == 4:
            s.dstruct[ req.data ] = 0

      #-------------------------------------------------------------------
      # service the ASU
      #-------------------------------------------------------------------
      if not s.xcelreq_q.empty() and not s.xcelresp_q.full():

        # get the accelerator request
        xcel_req = s.xcelreq_q.deq()

        # look up the ds_type
        ds_type = s.ds_type[ xcel_req.ds_id ]

        if ds_type == VECTOR:
          # get the base addr
          base_addr   = s.ds_table[ xcel_req.ds_id ]

          # get the metadata
          dt_desc_ptr = s.dt_table[ xcel_req.ds_id ]
          #dt_value    = s.mem[dt_desc_ptr:dt_desc_ptr+4]
          dt_desc     = TypeDescriptor().unpck( dt_desc_ptr )

          # PRIMITIVE TYPES
          if dt_desc.type_ == 0:
            #print "base_addr {}".format( base_addr )
            #print "iter {}".format( xcel_req.iter )
            #print "size {}".format( dt_desc.size_ )
            mem_addr = base_addr + ( xcel_req.iter * dt_desc.size_ )
            #print "mem_addr {}".format( mem_addr )
            if   xcel_req.type_ == itu_ifc_types.req.TYPE_READ:
              #print "read!"
              mem_data = s.mem[mem_addr:mem_addr+dt_desc.size_]
              s.xcelresp_q.enq( itu_ifc_types.resp.mk_msg( 0, mem_data ) )
            elif xcel_req.type_ == itu_ifc_types.req.TYPE_WRITE:
              s.mem[mem_addr:mem_addr+4] = xcel_req.data
              s.xcelresp_q.enq( itu_ifc_types.resp.mk_msg( 1, 0 ) )

  #-----------------------------------------------------------------------
  # line_trace
  #-----------------------------------------------------------------------

  def line_trace( s ):
    return " {} {} | {} {}".format( s.xcelreq,
                                    s.xcelresp,
                                    s.memreq,
                                    s.memresp )
