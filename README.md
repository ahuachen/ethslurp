# ethslurp
Drink the block chain (http://ethslurp.com)

#### ethslurp

    Usage:   ./ethslurp [-b|-c|-d|-e|-f|-i|-l|-o|-s|-t|-v|-h] addr

    Purpose: Fetches data off the Ethereum block chain for an arbitrary
             account or smart contract. Optionally formats the output
            to your specification.

#### Where:

  	addr                 the address of the account or contract to slurp (required)
  	-b  (or -backup)     save a date stamped backup of the previous slurp
  	-c  (or -clear)      clear all previously cached slurps
  	-d  (or -date)       specify date range as 'start-[end]' (yyyymmdd[:hhmmss])
  	-e  (or -expense)    include expenditures only
  	-f  (or -fmt)        pretty print, optionally add ':txt,' ':csv,' or ':html'
  	-i  (or -income)     include income transactions only
  	-l  (or -last)       re-display most recent (last) slurp
  	-o  (or -open)       open output in an associated editor
  	-s  (or -slurp)      force EthSlurp to take a slurp (ignore cached data)
  	-t  (or -test)       generate intermediary files but do not execute the commands
  	-v  (or -verbose)    set verbose level. Follow with a number to set level (-v0 for silent)
  	-h  (or -help)       display this help screen
