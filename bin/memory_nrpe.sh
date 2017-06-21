#!/bin/sh
# NRPE Plugin Alberto Aliaga 05/07/2004
#
# Codigos de retorno:
#	
#	0 	:	OK
#	1	:	WARNING
#	2	:	CRITICAL
#	3	:	UNKNOWN
#

######## Comprobaciones de parametros ##############
if [ $# -ne 2 ]
then
	echo "ERROR: numero de parametros incorrecto"
	echo "$0 <threshold_warning> <threshold_critical>"
	exit 3
fi


################ Ejecucion monitor #################

CURR=`/usr/bin/free | grep Mem: | awk '{print (($3 - $7) / $2)*100}'`
CURR_MB=`/usr/bin/free | grep Mem: | awk '{print ($3 - $7) / 1024}'`
TOT_MB=`/usr/bin/free | grep Mem: | awk '{print $2 / 1024}'`

################ Envio de resultado #################

if [ `echo "${CURR} > ${2}" | bc` -eq 1 ]
then 
	printf "CRITICAL: Mem: ${CURR_MB}MB (${CURR}%%) / ${TOT_MB}MB - Threshold ${2}%%\n"
	exit 2
elif [ `echo "${CURR} > ${1}" | bc` -eq 1 ]
then
        printf "WARNING: Mem: ${CURR_MB}MB (${CURR}%%) / ${TOT_MB}MB - Threshold ${1}%%\n"
        exit 1
else
	printf "OK: Mem: ${CURR_MB}MB (${CURR}%%) / ${TOT_MB}MB - Threshold ${1}%%\n"
	exit 0
fi
