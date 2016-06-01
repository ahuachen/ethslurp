# ethslurp
Drink the blockchain (http://ethslurp.com)

Watch a video tutorial <a href="https://www.youtube.com/watch?v=ZZDV1yAgces">here</a>

### ethslurp

    Usage:   ../ethslurp [-b|-c|-d|-e|-f|-i|-l|-m|-o|-r|-s|-t|-v|-h] addr

    Purpose: Fetches data off the Ethereum blockchain for an arbitrary
             account or smart contract. Optionally formats the output
            to your specification.

### where:

    addr                 the address of the account or contract to slurp (required)
    -b  (or -blocks)     export records in block range (:0[:max])
    -c  (or -clear)      clear all previously cached slurps
    -d  (or -dates)      export records in date range (:yyyymmdd[hhmmss][:yyyymmdd[hhmmss]])
    -e  (or -expense)    include expenditures only
    -f  (or -fmt)        pretty print, optionally add ':txt,' ':csv,' or ':html'
    -i  (or -income)     include income transactions only
    -l  (or -list)       list previously slurped addresses in cache
    -m  (or -max)        maximum transactions to slurp (:100000)
    -o  (or -open)       open the configuration file for editing
    -r  (or -rerun)      re-run the most recent slurp
    -s  (or -slurp)      force EthSlurp to take a slurp (ignore cached data)
    -t  (or -test)       generate intermediary files but do not execute the commands
    -v  (or -verbose)    set verbose level. Follow with a number to set level (-v0 for silent)
    -h  (or -help)       display this help screen
