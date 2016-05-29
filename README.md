# ethslurp
Drink the block chain (http://ethslurp.com)

Watch a video tutorial <a href="https://www.youtube.com/watch?v=ZZDV1yAgces">here</a>

### source code

While we fully intend to post our source code to open source, we decided against doing so now
because the software is not fully tested. All the data we present, the documentation, and
the videos were produced using the software, but it's just not quite ready for prime time.
Please return soon and often. 

### ethslurp

    Usage:   ./ethslurp [-b|-c|-e|-f|-i|-l|-o|-s|-t|-v|-h] addr

    Purpose: Fetches data off the Ethereum block chain for an arbitrary
             account or smart contract. Optionally formats the output
            to your specification.

### where:

  	addr                 the address of the account or contract to slurp (required)
  	-b  (or -block)      ignore all records from blocks earlier than :num (:0)
  	-c  (or -clear)      clear all previously cached slurps
  	-e  (or -expense)    include expenditures only
  	-f  (or -fmt)        pretty print, optionally add ':txt,' ':csv,' or ':html'
  	-i  (or -income)     include income transactions only
  	-l  (or -last)       re-display most recent (last) slurp
  	-o  (or -open)       open output in an associated editor
  	-s  (or -slurp)      force EthSlurp to take a slurp (ignore cached data)
  	-t  (or -test)       generate intermediary files but do not execute the commands
  	-v  (or -verbose)    set verbose level. Follow with a number to set level (-v0 for silent)
  	-h  (or -help)       display this help screen
