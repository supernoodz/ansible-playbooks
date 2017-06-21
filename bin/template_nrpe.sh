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

# LA LINEA DE ABAJO NO FUNCIONABA EN SOLARIS,
# ADEMAS DE QUE EL AWK SE PUEDE CAPTURAR A SI MISMO!!
# NUEVA LINEA, SOPORTE LINUX, HPUX, SOLARIS. (SI ES SOLARIS LA CARGA LLEVA ',')
CURR=`w | head -1 | awk '/load/ {print $(NF-1)}' | sed 's/,$//g' | sed 's/\.$//g'`


################ Envio de resultado #################

if [ ${CURR} -ge ${2} ]
then 
	printf "${CURR} procesos en cola en los ultimos 5 min min - Threshold ${2}\n"
	exit 2
elif [ ${CURR} -ge ${1} ]
then
        printf "${CURR} procesos en cola en los ultimos 5 min min - Threshold ${1}\n"
        exit 1
else
	printf "${CURR} procesos en cola en los ultimos 5 min min - Threshold ${1}\n"
	exit 0
fi
