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
	echo "$0 <full_file_path> <threshold_warning> <threshold_critical>"
	exit 3
fi


################ Ejecucion monitor #################

if [ ! -f $1 ]
then
        echo "ERROR: El fichero '$1' no existe."
        exit 3
fi

CURR=`ls -l "$1" | awk '{print $5 / (1024*1024)}'`

################ Envio de resultado #################

if [ `echo "${CURR} >= ${3}" | bc` -eq 1 ]
then 
	printf "CRITICAL: el fichero '$1' ocupa ${CURR}MB - Threshold ${3}MB|$1\n"
	exit 2
elif [ `echo "${CURR} >= ${2}" | bc` -eq 1 ]
then
        printf "WARNING: el fichero '$1' ocupa ${CURR}MB - Threshold ${2}MB|$1\n"
        exit 1
else
	printf "OK: el fichero '$1' ocupa ${CURR}MB - Threshold ${2}MB|$1\n"
	exit 0
fi
