# ethslurp
Drink the block chain

#### ethslurp

      Usage:   ethslurp [-n|-p|-s|-wf|-t|-v|-h] addr

Fetches data off the Ethereum block chain for an arbitrary account or smart contract. Optionally prettyPrints or stores the data in various formats.

#### Where:

  	addr                 the ethereum address of the account or contract you wish to read
  	-n  (or -nobackup)   do not save a timestamped backup file of the previous data file
  	-p  (or -pretty)     output data to screen in human readable format and quit
  	-s  (or -slurp)      pull fresh data from the blockchain
  	-wf (or -fmt)        write the data in various formats (.txt only for now)
  	-v  (or -verbose)    set verbose level. Follow with a number to set level
  	-h  (or -help)       display this help screen
