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

if [ ! -x /opt/nagios/bin/opclxcputil ]
then
    printf "The activity report tool /opt/nagios/bin/opclxcputil is not executable"
    exit 3
fi

PERC_PROC=`/opt/nagios/bin/opclxcputil  1 5 | awk '/Average/ { print ( $2 + $3 ) }'`

if [ $? -ne 0 ]
then
    printf "Problems getting process status via 'opclxcputil'."
    exit 3
fi


################ Envio de resultado #################

if [ ${PERC_PROC} -ge ${2} ]
then 
	printf "NOW: Mean:${PERC_PROC}%% - Threshold ${2}%%\n"
	exit 2
elif [ ${PERC_PROC} -ge ${1} ]
then
        printf "NOW: Mean:${PERC_PROC}%% - Threshold ${1}%%\n"
        exit 1
else
	printf "NOW: Mean:${PERC_PROC}%% - Threshold ${1}%%\n"
	exit 0
fi
