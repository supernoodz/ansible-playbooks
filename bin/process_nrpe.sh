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
if [ $# -ne 3 ]
then
	echo "ERROR: numero de parametros incorrecto"
	echo "$0 <process_name> <threshold_warning> <threshold_critical>"
	exit 3
fi


################ Ejecucion monitor #################

CURR=`ps -fe | grep -v grep | grep -v "process_nrpe.sh" | grep "$1" | wc -l | awk '{print $1}'`
################ Envio de resultado #################

if [ ${CURR} -le ${3} ]
then 
	printf "CRITICAL: hay ${CURR} instancias del proceso '$1' - Threshold ${3}|$1\n"
	exit 2
elif [ ${CURR} -le ${2} ]
then
        printf "WARNING: hay ${CURR} instancias del proceso '$1' - Threshold ${2}|$1\n"
        exit 1
else
	printf "OK: hay ${CURR} instancias del proceso '$1' - Threshold ${2}|$1\n"
	exit 0
fi
