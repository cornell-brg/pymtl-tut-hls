#=========================================================================
# PopCount
#=========================================================================

from pymtl import *

#-------------------------------------------------------------------------
# PopCount
#-------------------------------------------------------------------------
# Wrapper module for HLS generated hardware

class PopCount( VerilogModel ):

  def __init__( s ):

    s.in_V         = InPort ( 64 )
    s.out_r        = OutPort( 32 )
    s.out_r_ap_vld = OutPort( 1  )

    s.set_ports({
      'ap_clk'       : s.clk,
      'ap_rst'       : s.reset,
      'in_V'         : s.in_V,
      'out_r'        : s.out_r,
      'out_r_ap_vld' : s.out_r_ap_vld
    })

  def line_trace(s):
    return "{}|{}|{}".format(
      s.in_V,
      s.out_r,
      s.out_r_ap_vld
    )

#-------------------------------------------------------------------------
# main
#-------------------------------------------------------------------------
# We include a simple ad-hoc test.

def main():

  # Input values

  inputs = [ 0x21, 0x14, 0x1133, 0x00 ]
  input_idx = 0

  # Elaborate the model

  model = PopCount()
  model.elaborate()

  # Create and reset the simulator

  sim = SimulationTool( model )
  sim.reset()

  # Apply input values and display output values

  for i in range(0,10):

    # Write input value to input port

    model.in_V.value = inputs[input_idx]

    # Display input and output ports

    sim.print_line_trace()

    # Tick simulator one cycle

    sim.cycle()

    # If output is valid, then move on to next input value

    if model.out_r_ap_vld:
      input_idx += 1
      if input_idx == len(inputs):
        break

if __name__ == "__main__":
    main()

