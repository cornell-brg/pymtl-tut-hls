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

    # Implementation

    @s.tick_fl
    def logic():
      s.cfg.xtick()
      s.accel.xtick()
      s.mem.xtick()

      if   not s.accel.req_q.empty() and not s.mem.req_q.full():

        # translate iterator for a vector of integers
        accel_req = s.accel_ifc.req.unpck( s.accel.get_req() )
        base      = s.dstruct[ accel_req.ds_id ]
        offset    = accel_req.index * 4

        # create a memory request message
        mem_addr  = base + offset
        mem_type_ = accel_req.type_
        mem_data  = accel_req.data

        # send a memory request
        s.mem.push_req( s.mem_ifc.req.mk_msg( mem_type_, mem_addr, 0, mem_data ) )

      if not s.mem.resp_q.empty() and not s.accel.resp_q.full():

        # get the memory response
        mem_resp = s.mem_ifc.resp.unpck( s.mem.get_resp() )

        # send the response to the accelerator
        s.accel.push_resp( concat( mem_resp.type_, mem_resp.data ) )

      if not s.cfg.req_q.empty() and not s.cfg.resp_q.full():

        # get the configuration message
        # TBD: need to send ack's for cfg messages
        req = s.cfg.get_req()

        # dstruct alloc
        if   req.addr == 1:
          for idx,val in enumerate( s.dstruct ):
            if   val == 0:
              s.cfg.push_resp( idx )
              break
            elif idx == ( len( s.dstruct ) - 1 ):
              s.cfg.push_resp( -1 )

        # dstruct init
        elif req.addr == 2:
          s.dstruct[ req.id ] = req.data

        # dstruct dealloc
        elif req.addr == 3:
          s.dstruct[ req.data ] = 0

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
