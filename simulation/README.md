# IntCC NS-3 simulator

It is based on NS-3 version 3.17.

## Quick Start

### Build
`./waf configure`

Please note if gcc version > 5, compilation will fail due to some ns3 code style.  If this what you encounter, please use:

`CC='gcc-5' CXX='g++-5' ./waf configure`

### Run
The direct command to run is:
`./waf --run 'scratch/third mix/config.txt'`

We provide a `run.py` for automatically *generating config* and *running experiment*. Please `python run.py -h` for usage.
Example usage:
`python run.py --cc hp --trace flow --bw 100 --topo topology --hpai 50`

To run IntCC, try:
`bash mybash_ccdfs.sh`


## Experiment notes
To compare with other congestion control algorithms, try:
`bash mybash_bbr.sh`
`bash mybash_hpcc.sh`
`bash mybash_dctcp.sh`
