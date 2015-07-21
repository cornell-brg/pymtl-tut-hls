from pymtl      import *
from pclib.ifcs import InValRdyBundle, OutValRdyBundle
from pclib.ifcs import valrdy_to_str
from pclib.rtl  import SingleElementBypassQueue
from pclib.rtl  import SingleElementPipelinedQueue

class Parent( VerilogModel ):

  def __init__( s ):

    s.req = OutValRdyBundle( 32 )
    s.resp = InValRdyBundle( 32 )

    s.set_ports({
      'ap_clk'           : s.clk,
      'ap_rst'           : s.reset,
      'req_V'            : s.req.msg,
      'req_V_ap_vld'     : s.req.val,
      'req_V_ap_ack'     : s.req.rdy,
      'resp_V'           : s.resp.msg,
      'resp_V_ap_vld'    : s.resp.val,
      'resp_V_ap_ack'    : s.resp.rdy
    })

class Child( VerilogModel ):

  def __init__( s ):

    s.req = InValRdyBundle( 32 )
    s.resp = OutValRdyBundle( 32 )

    s.set_ports({
      'ap_clk'           : s.clk,
      'ap_rst'           : s.reset,
      'req_V'            : s.req.msg,
      'req_V_ap_vld'     : s.req.val,
      'req_V_ap_ack'     : s.req.rdy,
      'resp_V'           : s.resp.msg,
      'resp_V_ap_vld'    : s.resp.val,
      'resp_V_ap_ack'    : s.resp.rdy
    })

class ParentChild( Model ):

  def __init__( s ):
    s.child  = Child()
    s.parentx = Parent()
    s.pipe_q = SingleElementPipelinedQueue( 32 )
    s.byp_q = SingleElementBypassQueue( 32 )

    #s.connect( s.parentx.req,  s.child.req )
    #s.connect( s.parentx.resp, s.child.resp )

    s.connect( s.parentx.req, s.pipe_q.enq )
    s.connect( s.child.req, s.pipe_q.deq )
    s.connect( s.byp_q.enq, s.child.resp )
    s.connect( s.parentx.resp, s.byp_q.deq )

  def line_trace( s ):
    return "{} |{} {}|{}".format(
      valrdy_to_str( s.parentx.req.msg,
                     s.parentx.req.val,
                     s.parentx.req.rdy ),
      valrdy_to_str( s.child.req.msg,
                     s.child.req.val,
                     s.child.req.rdy ),
      valrdy_to_str( s.child.resp.msg,
                     s.child.resp.val,
                     s.child.resp.rdy ),
      valrdy_to_str( s.parentx.resp.msg,
                     s.parentx.resp.val,
                     s.parentx.resp.rdy )
     )

def test( dump_vcd ):

  model = ParentChild()
  model.elaborate()

  sim = SimulationTool( model )
  sim.reset()
  print

  for i in range( 10 ):
    sim.print_line_trace()
    sim.cycle()
