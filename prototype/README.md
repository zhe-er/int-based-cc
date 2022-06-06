#doc
This document shows the setup of prototype environments.

#define_option_stack
The kernel code in the Linux network stack


#modify_option
The kernel code in the P4 switch


#note
The modification in the linux stack (details)


#headers_with_opts, headers_no_opts, portswitch_with_opts, portswitch_no_opts,
ipswitch_with_opts
The modification in two P4 switches

# IntCC simulation
The simulation experiments are based on NS3 platform. We simulate the cross-datacenter networks and replicate IntCC. The experiments are under `simulation/`. The traffic and analysis folders correspond to the generation flow and analysis respectively.

## NS-3 simulation
The ns-3 simulation is under `simulation/`. Refer to the README.md under it for more details.

## Traffic generator
The traffic generator is under `traffic_gen/`. Refer to the README.md under it for more details.

## Analysis
We provide a few analysis scripts under `analysis/` to view the packet-level events. Refer to the README.md under it for more details.

## Prototype
The core code for the prototype implementation is under `prototype/`. Refer to the README.md under it for more details.





