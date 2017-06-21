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
	echo "$0 <mount_point> <threshold_warning> <threshold_critical>"
	exit 3
fi


################ Ejecucion monitor #################

################################################################
# DETERMINA COMANDO BDF SEGUN EL TIPO DE UNIX
################################################################
SYSTEM=`uname -s`
case $SYSTEM in 
        HP-UX)  DF_CMD="bdf";AWK_CMD="awk";;
        SCO_SV) DF_CMD="df -B";AWK_CMD="awk";;
        SunOS)  DF_CMD="df -k";AWK_CMD="/usr/xpg4/bin/awk";;
        Linux)  DF_CMD="df -k";AWK_CMD="awk";;
esac
################################################################

CURR=`${DF_CMD} | grep " $1\$"  | cut -d% -f1 | awk '{print $(NF)}'` 

#Si la longitud de la cadena CURR es 0, es decir, esta vacia salimos.
if [ -z "${CURR}" ]
then
        printf "No existe el filesystem $1 \n"
        exit 3
fi

################ Envio de resultado #################

if [ ${CURR} -ge ${3} ]
then 
	printf "Fs: '$1' Used: (${CURR}%%) - Threshold ${3}%%|$1\n"
	exit 2
elif [ ${CURR} -ge ${2} ]
then
        printf "Fs: '$1' Used: (${CURR}%%) - Threshold ${2}%%|$1\n"
        exit 1
else
	printf "Fs: '$1' Used: (${CURR}%%) - Threshold ${2}%%|$1\n"
	exit 0
fi
