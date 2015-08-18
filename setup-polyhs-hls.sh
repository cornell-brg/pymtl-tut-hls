#=========================================================================
# setup-polyhs-hls.sh
#=========================================================================
# Script that sets up the development environment for the polyhs-hls flow

# activate the pymtl-polyhs virtualenv
source /research/brg/install/venv-pkgs/pypy2.5.1-shared/bin/activate

# setup the environment variable for the verilator include directory
export PYMTL_VERILATOR_INCLUDE_DIR="${STOW_PKGS_GLOBAL_PREFIX}/share/verilator/include"

# source the setup scripts for xilinx vivado
source ${BARE_PKGS_GLOBAL_PREFIX}/xilinx-vivado-2015.2/setup.sh

# setup the environment variable for the xilinx vivado hls include directory
XILINX_VIVADO_HLS_PATH=$(dirname $(which vivado_hls))
export XILINX_VIVADO_HLS_INCLUDE_DIR="${XILINX_VIVADO_HLS_PATH}/../include"

