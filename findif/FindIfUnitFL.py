#=========================================================================
# FindIfUnitFL
#=========================================================================
# Functional-level model of a find-if algorithm-specific-unit that works
# on integer data types. The ASU model accepts two iterators and a
# predicate condition.
#
# predicate value   func
# ---------------   ----
#               0   greater than zero
#               1   less    than zero
#               2   equal   to   zero
#               3   has an odd integer
#               4   has an even integer

from pymtl       import *
from pclib.ifcs  import ChildReqRespBundle, ParentReqRespBundle
from pclib.ifcs  import valrdy_to_str
from pclib.cl    import ChildReqRespQueueAdapter, ParentReqRespQueueAdapter

#-------------------------------------------------------------------------
# FindIfUnitFL
#-------------------------------------------------------------------------
class FindIfUnitFL( Model ):

  def __init__( s, cfg_ifc_types, itu_ifc_types ):

    # Interfaces

    s.cfg_ifc = ChildReqRespBundle ( cfg_ifc_types )
    s.itu_ifc = ParentReqRespBundle( itu_ifc_types )

    # Adapters

    s.cfg     = ChildReqRespQueueAdapter ( s.cfg_ifc )
    s.itu     = ParentReqRespQueueAdapter( s.itu_ifc )

    # Internal State
    s.go               = False  # reg 0
    s.iter_first_ds_id = 0      # reg 1
    s.iter_first_index = 0      # reg 2
    s.iter_last_ds_id  = 0      # reg 3
    s.iter_last_index  = 0      # reg 4
    s.predicate_val    = 0      # reg 5

    s.itu_req_set      = False

    # Implementation

    @s.tick_fl
    def logic():
      s.cfg.xtick()
      s.itu.xtick()

      # Configure State
      # TBD: need to send ack's for cfg messages
      if not s.cfg.req_q.empty() and not s.cfg.resp_q.full():

        # get the coprocessor message
        req = s.cfg_ifc.req.unpck( s.cfg.get_req() )

        # set go state
        if   req.addr == 0 and req.type_ == 1:
          s.go = True

        # set iter first ds-id
        elif req.addr == 1 and req.type_ == 1:
          s.iter_first_ds_id = req.data

        # set iter first index
        elif req.addr == 2 and req.type_ == 1:
          s.iter_first_index = req.data

        # set iter last ds-id
        elif req.addr == 3 and req.type_ == 1:
          s.iter_last_ds_id = req.data

        # set iter last index
        elif req.addr == 4 and req.type_ == 1:
          s.iter_last_index = req.data

        # set predicate value
        elif req.addr == 5 and req.type_ == 1:
          s.predicate_val = req.data

      # Go State
      if s.go:

        # check if the iterators belong to the same data structure and bail
        # if not
        if not s.iter_first_ds_id == s.iter_last_ds_id:
          # return the last iterator index
          if not s.cfg.resp_q.full():
            s.cfg.push_resp( s.cfg_ifc.resp.mk_resp( 0, s.iter_last_index ) )
            s.go = False

        # check for termination
        elif s.iter_first_index == s.iter_last_index:
          # return the last iterator index
          if not s.cfg.resp_q.full():
            s.cfg.push_resp( s.cfg_ifc.resp.mk_resp( 0, s.iter_last_index ) )
            s.go = False

        # compute find-if
        else:

          # set load request
          if not s.itu.req_q.full() and not s.itu_req_set:
            ds_id = s.iter_first_ds_id
            index = s.iter_first_index
            s.itu.push_req( s.itu_ifc.req.mk_req( 0, ds_id, index, 0 ) )
            s.itu_req_set = True

          # get the value and apply the predicate
          if not s.itu.resp_q.empty() and s.itu_req_set:
            resp = s.itu.get_resp()

            # greater than zero
            if   s.predicate_val == 0:
              if resp.data > 0:
                s.cfg.push_resp( s.cfg_ifc.resp.mk_resp( 0, s.iter_first_index ) )
                s.go = False
              else:
                s.iter_first_index = s.iter_first_index + 1
                s.itu_req_set = False

            # lesser than zero
            elif s.predicate_val == 1:
              if resp.data < 0:
                s.cfg.push_resp( s.cfg_ifc.resp.mk_resp( 0, s.iter_first_index ) )
                s.go = False
              else:
                s.iter_first_index = s.iter_first_index + 1
                s.itu_req_set = False

            # equal to zero
            elif s.predicate_val == 2:
              if resp.data == 0:
                s.cfg.push_resp( s.cfg_ifc.resp.mk_resp( 0, s.iter_first_index ) )
                s.go = False
              else:
                s.iter_first_index = s.iter_first_index + 1
                s.itu_req_set = False

            # is odd
            elif s.predicate_val == 3:
              if resp.data % 2 == 1:
                s.cfg.push_resp( s.cfg_ifc.resp.mk_resp( 0, s.iter_first_index ) )
                s.go = False
              else:
                s.iter_first_index = s.iter_first_index + 1
                s.itu_req_set = False

            # is even
            elif s.predicate_val == 4:
              if resp.data % 2 == 0:
                s.cfg.push_resp( s.cfg_ifc.resp.mk_resp( 0, s.iter_first_index ) )
                s.go = False
              else:
                s.iter_first_index = s.iter_first_index + 1
                s.itu_req_set = False

  #-----------------------------------------------------------------------
  # line_trace
  #-----------------------------------------------------------------------

  def line_trace( s ):
    return "{}(){}{}(){}".format(
      valrdy_to_str( s.cfg_ifc.req_msg,
                     s.cfg_ifc.req_val,
                     s.cfg_ifc.req_rdy ),

      valrdy_to_str( s.itu_ifc.req_msg,
                     s.itu_ifc.req_val,
                     s.itu_ifc.req_rdy ),

      valrdy_to_str( s.itu_ifc.resp_msg,
                     s.itu_ifc.resp_val,
                     s.itu_ifc.resp_rdy ),

      valrdy_to_str( s.cfg_ifc.resp_msg,
                     s.cfg_ifc.resp_val,
                     s.cfg_ifc.resp_rdy )
    )
