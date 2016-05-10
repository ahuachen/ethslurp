# ethslurp
Drink from the block chain hose

>>$ ethslurp

  Usage:   ethslurp [-n|-p|-s|-wf|-t|-v|-h] addr
  Purpose: Fetches data off the Ethereum block chain for an arbitrary account or smart contract.
           Optionally prettyPrints or stores the data in various formats.
           
  Where:
  	addr                 the ethereum address of the account or contract you wish to read (required)
  	-n  (or -nobackup)   do not save a timestamped backup file of the previous data file (data will be backed up by default)
  	-p  (or -pretty)     output data to screen in human readable format and quit
  	-s  (or -slurp)      pull fresh data from the blockchain
  	-wf (or -fmt)        write the data in various formats (.txt only for now) with optional (on by default) backup
  	-t  (or -test)       generate intermediary files but do not execute the commands
  	-v  (or -verbose)    set verbose level. Follow with a number to set level (-v0 for silent)
  	-h  (or -help)       display this help screen
  	
