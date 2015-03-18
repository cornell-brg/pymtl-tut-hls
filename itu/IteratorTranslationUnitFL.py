#=========================================================================
# IteratorTranslationUnitFL
#=========================================================================

from pymtl       import *
from pclib.ifcs  import ChildReqRespBundle, ParentReqRespBundle
from pclib.ifcs  import valrdy_to_str
from pclib.cl    import ChildReqRespQueueAdapter, ParentReqRespQueueAdapter

#-------------------------------------------------------------------------
# IteratorTranslationUnitFL
#-------------------------------------------------------------------------
class IteratorTranslationUnitFL( Model ):

  def __init__( s, cfg_ifc_types, accel_ifc_types, mem_ifc_types ):

    # Interfaces

    s.cfg_ifc   = ChildReqRespBundle ( cfg_ifc_types   )
    s.accel_ifc = ChildReqRespBundle ( accel_ifc_types )
    s.mem_ifc   = ParentReqRespBundle( mem_ifc_types   )

    # Adapters

    s.cfg       = ChildReqRespQueueAdapter ( s.cfg_ifc   )
    s.accel     = ChildReqRespQueueAdapter ( s.accel_ifc )
    s.mem       = ParentReqRespQueueAdapter( s.mem_ifc   )

    # Internal State

    s.dstruct   = [0]*32
    s.data_type = [0]*32

    # Data Structure types
    s.vector_char    = 0 # vector of chars
    s.vector_int     = 1 # vector of ints
    s.vector_float   = 2 # vector of floats
    s.vector_pointer = 3 # vector of pointers
    s.list_char      = 4 # list   of chars
    s.list_int       = 5 # list   of ints
    s.list_float     = 6 # list   of floats
    s.list_pointer   = 8 # list   of pointers

    # helper function that translates an iterator request message to a
    # memory request message
    def translate_iterator( accel_req ):

      # params for the memory message
      mem_addr  = 0
      mem_len   = 0
      mem_type_ = accel_req.type_
      mem_data  = accel_req.data

      # look up the data structure and data type
      s.dstruct_type = s.data_type[ accel_req.ds_id ]

      # translate based on data structure and type
      if    s.dstruct_type == s.vector_char:
        # BASE + INDEX * SIZE( CHAR )
        base     = s.dstruct[ accel_req.ds_id ]
        offset   = accel_req.index
        mem_addr = base + offset
        mem_len  = 1
      elif  s.dstruct_type == s.vector_int:
        # BASE + INDEX * SIZE( INT )
        base     = s.dstruct[ accel_req.ds_id ]
        offset   = accel_req.index * 4
        mem_addr = base + offset
        mem_len  = 0
      elif  s.dstruct_type == s.vector_float:
        # BASE + INDEX * SIZE( FLOAT )
        base     = s.dstruct[ accel_req.ds_id ]
        offset   = accel_req.index * 4
        mem_addr = base + offset
        mem_len  = 0
      elif  s.dstruct_type == s.vector_pointer:
        # BASE + INDEX * SIZE( FLOAT )
        base     = s.dstruct[ accel_req.ds_id ]
        offset   = accel_req.index * 4
        mem_addr = base + offset
        mem_len  = 0

      # return the memory message
      return s.mem_ifc.req.mk_msg( mem_type_, mem_addr, mem_len, mem_data )

    # Implementation

    @s.tick_fl
    def logic():
      s.cfg.xtick()
      s.accel.xtick()
      s.mem.xtick()

      if not s.cfg.req_q.empty() and not s.cfg.resp_q.full():

        # get the configuration message
        req = s.cfg.get_req()

        # check if it is a write request
        if req.type_ == 1 and not req.addr == 1:
          s.cfg.push_resp( s.cfg_ifc.resp.mk_resp( 1, 0 ) )

        # dstruct alloc
        if   req.addr == 1:
          for idx,val in enumerate( s.dstruct ):
            if   val == 0:
              s.cfg.push_resp( s.cfg_ifc.resp.mk_resp( 1, idx ) )
              s.data_type[ idx ] = req.data
              break
            elif idx == ( len( s.dstruct ) - 1 ):
              s.cfg.push_resp( s.cfg_ifc.resp.mk_resp( 1, -1 ) )

        elif req.addr == 2: s.dstruct[ req.id ] = req.data  # dstruct init
        elif req.addr == 3: s.dstruct[ req.data ] = 0       # dstruct dealloc

      if   not s.accel.req_q.empty() and not s.mem.req_q.full():

        # get the accelerator request
        accel_req = s.accel_ifc.req.unpck( s.accel.get_req() )
        # translate the iterator request
        mem_req   = translate_iterator( accel_req )
        # send the memory request
        s.mem.push_req( mem_req )

      if not s.mem.resp_q.empty() and not s.accel.resp_q.full():

        # get the memory response
        mem_resp = s.mem_ifc.resp.unpck( s.mem.get_resp() )
        # send the response to the accelerator
        s.accel.push_resp( concat( mem_resp.type_, mem_resp.data ) )

  #-----------------------------------------------------------------------
  # line_trace
  #-----------------------------------------------------------------------

  def line_trace( s ):

    return "{}(){}(){}(){}".format(
      valrdy_to_str( s.accel_ifc.req_msg,
                     s.accel_ifc.req_val,
                     s.accel_ifc.req_rdy ),

      valrdy_to_str( s.mem_ifc.req_msg,
                     s.mem_ifc.req_val,
                     s.mem_ifc.req_rdy ),

      valrdy_to_str( s.mem_ifc.resp_msg,
                     s.mem_ifc.resp_val,
                     s.mem_ifc.resp_rdy ),

      valrdy_to_str( s.accel_ifc.resp_msg,
                     s.accel_ifc.resp_val,
                     s.accel_ifc.resp_rdy )
    )
