#!/bin/sh
# NRPE Plugin Alberto Aliaga 05/07/2004
#
# Codigos de retorno:
#	
#	0 	:	OK
#	1	:	WARNING
#	2	:	CRITICAL
#	3	|	UNKNOWN
#

######## Comprobaciones de parametros ##############
if [ $# -ne 2 ]
then
	echo "ERROR: numero de parametros incorrecto"
	echo "$0 <threshold_warning> <threshold_critical>"
	exit 3
fi


################ Ejecucion monitor #################

CURR=`free | 
             awk '/^Swap/ {
	                    tot=$2; use=$3
		          }
		 END      {
		            if (tot > 0) 
			    { 
			      printf "%d\n", (use / tot + 0.005) * 100 
			    }
			    else
			    {
                              printf "0\n"
			    }
			  }'`
  if [ $? -ne 0 ]
  then
    printf "Problems getting result from 'free'.\n"
    exit 3
  fi

################ Envio de resultado #################

if [ `echo "${CURR} > ${2}" | bc` -eq 1 ]
then 
	printf "CRITICAL: Paged Mem: (${CURR}%%) - Threshold ${2}%%\n"
	exit 2
elif [ `echo "${CURR} > ${1}" | bc` -eq 1 ]
then
        printf "WARNING: Paged Mem: (${CURR}%%) - Threshold ${1}%%\n"
        exit 1
else
	printf "OK: Paged Mem: (${CURR}%%) - Threshold ${1}%%\n"
	exit 0
fi
