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
#
# XXX: Handle user-defined types
# XXX: Handle returning responses with the right accel-id

from pymtl      import *
from pclib.ifcs import InValRdyBundle, OutValRdyBundle
from pclib.cl   import InValRdyQueue, OutValRdyQueue

from dstu.BytesMemPortProxyFuture import BytesMemPortProxy
from dstu.TypeDescriptor import TypeDescriptor
from dstu.UserTypes import TypeEnum

#-------------------------------------------------------------------------
# FindIfUnitFL
#-------------------------------------------------------------------------
class FindIfUnitFL( Model ):

  def __init__( s, cfg_ifc_types, itu_ifc_types, mem_ifc_types ):

    #---------------------------------------------------------------------
    # Interfaces
    #---------------------------------------------------------------------

    s.cfgreq  = InValRdyBundle  ( cfg_ifc_types.req  )
    s.cfgresp = OutValRdyBundle ( cfg_ifc_types.resp )

    s.itureq  = OutValRdyBundle ( itu_ifc_types.req  )
    s.ituresp = InValRdyBundle  ( itu_ifc_types.resp )

    s.memreq   = OutValRdyBundle ( mem_ifc_types.req   )
    s.memresp  = InValRdyBundle  ( mem_ifc_types.resp  )

    #---------------------------------------------------------------------
    # Adapters
    #---------------------------------------------------------------------

    # Adapter for the CFG request val-rdy bundle
    s.cfgreq_q  = InValRdyQueue  ( cfg_ifc_types.req  )
    s.connect( s.cfgreq_q.in_, s.cfgreq )

    # Adapter for the CFG response val-rdy bundle
    s.cfgresp_q = OutValRdyQueue ( cfg_ifc_types.resp )
    s.connect( s.cfgresp_q.out, s.cfgresp )

    # Adapter for the ITU request val-rdy bundle
    s.itureq_q  = OutValRdyQueue  ( itu_ifc_types.req  )
    s.connect( s.itureq_q.out, s.itureq )

    # Adapter for the response val-rdy bundle
    s.ituresp_q = InValRdyQueue ( itu_ifc_types.resp )
    s.connect( s.ituresp_q.in_, s.ituresp )

    # Memory port adapter
    s.mem = BytesMemPortProxy ( mem_ifc_types, s.memreq, s.memresp )

    #---------------------------------------------------------------------
    # Internal State
    #---------------------------------------------------------------------

    s.go               = False  # reg 0
    s.iter_first_ds_id = 0      # reg 1
    s.iter_first_iter  = 0      # reg 2
    s.iter_last_ds_id  = 0      # reg 3
    s.iter_last_iter   = 0      # reg 4
    s.predicate_val    = 0      # reg 5
    s.dt_desc_ptr      = 0      # reg 6

    # boolean flag to check if only one request is in flight
    s.itu_req_set = False
    s.metadata_valid = False

    # which was the last field to be loaded?
    s.loaded_fields = []

    # datatype descriptor
    s.dt_desc = 0

    #---------------------------------------------------------------------
    # predicate helper function
    #---------------------------------------------------------------------
    # helper function to apply the predicate
    def predicate( data ):
      if   s.predicate_val == 0: return data > 0
      elif s.predicate_val == 1: return data < 0
      elif s.predicate_val == 2: return data == 0
      elif s.predicate_val == 3: return ( data % 2 ) == 1
      elif s.predicate_val == 4: return ( data % 2 ) == 0

    def predicate_point( px, py ):
      if   s.predicate_val == 0: return px > 0 and py > 0
      elif s.predicate_val == 1: return px < 0 and py < 0
      elif s.predicate_val == 2: return px == 0 and py == 0
      # TODO: more

    #---------------------------------------------------------------------
    # Implementation
    #---------------------------------------------------------------------
    @s.tick_fl
    def logic():
      s.cfgreq_q.xtick()
      s.cfgresp_q.xtick()
      s.itureq_q.xtick()
      s.ituresp_q.xtick()

      #-------------------------------------------------------------------
      # Configure the ASU
      #-------------------------------------------------------------------
      if not s.go:

        if not s.cfgreq_q.empty() and not s.cfgresp_q.full():

          # get the coprocessor message
          req = s.cfgreq_q.deq()

          # check if it is a write request
          if req.type_ == 1:

            # send the ack for mtx messages
            s.cfgresp_q.enq( cfg_ifc_types.resp.mk_msg( 0, 1, 0, 0 ) )

            # check the address and configure
            if   req.raddr == 0:
              print "starting accel"
              print "first iter =", s.iter_first_ds_id, s.iter_first_iter
              print "last  iter =", s.iter_last_ds_id, s.iter_last_iter
              print "pred =", s.predicate_val
              print "mdata = ", s.dt_desc_ptr
              s.go = True                   # go
              s.itu_req_set = False
              s.metadata_valid = False
              s.loaded_fields = []
            elif req.raddr == 1: s.iter_first_ds_id = req.data # first ds-id
            elif req.raddr == 2: s.iter_first_iter  = req.data # first iter
            elif req.raddr == 3: s.iter_last_ds_id  = req.data # last ds-id
            elif req.raddr == 4: s.iter_last_iter   = req.data # last iter
            elif req.raddr == 5: s.predicate_val    = req.data # predicate
            elif req.raddr == 6: s.dt_desc_ptr      = req.data # dt_desc_ptr

      #-------------------------------------------------------------------
      # Go State
      #-------------------------------------------------------------------

      if s.go:

        # First fetch the metadata
        if not s.metadata_valid:
          print "fetching mdata"
          dt_value = s.mem[s.dt_desc_ptr:s.dt_desc_ptr+4]
          s.dt_desc  = TypeDescriptor().unpck( dt_value )
          s.metadata_valid = True

        # if the iterators dont't belong to the same data structure bail
        if not s.iter_first_ds_id == s.iter_last_ds_id:
          # return the last iterator iter
          if not s.cfgresp_q.full():
            s.cfgresp_q.enq( cfg_ifc_types.resp.mk_msg(0,0,s.iter_last_iter,0) )
            s.go = False

        # check for termination
        elif s.iter_first_iter == s.iter_last_iter:
          # return the last iterator iter
          if not s.cfgresp_q.full():
            s.cfgresp_q.enq( cfg_ifc_types.resp.mk_msg(0,0,s.iter_last_iter,0) )
            s.go = False

        #-----------------------------------------------------------------
        # compute find-if
        #-----------------------------------------------------------------
        # iterator msg is (opaque, type, ds_id, iter, field, data)

        else:

          # handle primitive types
          if s.dt_desc.type_ < TypeEnum.MAX_PRIMITIVE:

            # set load request
            if not s.itureq_q.full() and not s.itu_req_set:
              ds_id = s.iter_first_ds_id
              iter  = s.iter_first_iter
              s.itureq_q.enq( itu_ifc_types.req.mk_msg(0,0,ds_id,iter,0,0) )
              s.itu_req_set = True

            # get the value and apply the predicate
            if not s.ituresp_q.empty() and s.itu_req_set:
              resp = s.ituresp_q.deq()

              # apply predicate
              if predicate( resp.data ):
                s.cfgresp_q.enq( cfg_ifc_types.resp.mk_msg(0,0,s.iter_first_iter,0) )
                s.go = False
              else:
                s.iter_first_iter = s.iter_first_iter + 1
                s.itu_req_set = False

          # Point type
          elif s.dt_desc.type_ == TypeEnum.POINT:
            fields_to_load = [2,3]

            # set load request
            if not s.itureq_q.full() and not s.itu_req_set:
              ds_id = s.iter_first_ds_id
              iter  = s.iter_first_iter
              # only send a DTU request if there are fields left to get
              l = len( s.loaded_fields )
              if l < len( fields_to_load ):
                s.itureq_q.enq( itu_ifc_types.req.mk_msg(0,0,ds_id,iter,fields_to_load[l],0) )
                s.itu_req_set = True

            # get the value and apply the predicate
            if not s.ituresp_q.empty() and s.itu_req_set:
              resp = s.ituresp_q.deq()
              s.loaded_fields.append( resp.data )
              s.itu_req_set = False

            # this comment must be different from apply predicate
            if len( s.loaded_fields ) == len( fields_to_load ):
              # TODO: fix this
              if predicate_point( s.loaded_fields[0], s.loaded_fields[1] ):
                s.cfgresp_q.enq( cfg_ifc_types.resp.mk_msg(0,0,s.iter_first_iter,0) )
                s.go = False
              else:
                s.iter_first_iter = s.iter_first_iter + 1

          else:
            raise SystemExit( s.dt_desc.type_ )

  #-----------------------------------------------------------------------
  # line_trace
  #-----------------------------------------------------------------------

  def line_trace( s ):
    return "{} | {} {} | {}".format( s.cfgreq,
                                     s.itureq,
                                     s.ituresp,
                                     s.cfgresp)
