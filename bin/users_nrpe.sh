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

CURR=`w | head -1 | awk '/user/ {print $(NF-6)}'`

################ Envio de resultado #################

if [ ${CURR} -ge ${2} ]
then 
	printf "Hay ${CURR} usuarios conectados - Threshold ${2}\n"
	exit 2
elif [ ${CURR} -ge ${1} ]
then
        printf "Hay ${CURR} usuarios conectados - Threshold ${1}\n"
        exit 1
else
	printf "Hay ${CURR} usuarios conectados - Threshold ${1}\n"
	exit 0
fi
