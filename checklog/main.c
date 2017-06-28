#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "main.h"

#if SYSTEM == NT4_2000
#include "winlogs.h"
#elif SYSTEM == SUN_OS
#include <thread.h>
#else
#include <pthread.h>
#include <sched.h>
#endif

char **patrons;
int type;
long int idleTime=0;
long int procLinies=1;
char ultimaLiniaFitxer[MAXFILELINE];
int noMatches=1;

#if SYSTEM == NT4_2000
	extern int ready;
#endif

typedef struct {
	int tipus; // AND o OR
	int numSubpatrons;
	char str[MAXPATHCHARS];
	char *subpatrons[MAXSUBPATRONS];
} t_patro;


void usage()
{
	fprintf(stderr,"Uso de checklog:\n");
	fprintf(stderr,"checklog nombre_fichero_de_logs patron1[,patron2,...] [idleTime]\n");
	fprintf(stderr,"\tnombre_fichero_de_logs: nombre del fichero o bien uno de los siguientes logs de Windows (siempre en mayúsculas):\n");
	fprintf(stderr,"\t\tAPPLICATION\n\t\tSYSTEM\n\t\tSECURITY\n");
	fprintf(stderr,"idleTime: tiempo (mseg) de descanso X cada N lineas, para reducir el consumo de CPU. Formato: XmsN, X y N enteros\n");
}

void parsejarPatro(char *str, t_patro *parsejat)
{
	char *p,*q;
	int numSubpatrons;
	int tipus;

	#ifdef DEBUG
		fprintf(stderr,"\nParsejant patró: %s",str);
	#endif

	strcpy(parsejat->str,str);
	p=parsejat->str;
	q=p;
	tipus=SIMPLE;
	numSubpatrons=0;
	while(*p!='\0' && (numSubpatrons < MAXSUBPATRONS)) {
		if(*p=='*') { tipus=OR; *p='\0'; parsejat->subpatrons[numSubpatrons]=q; q=p+1; numSubpatrons++; }
		else if(*p=='·') { tipus=AND; *p='\0'; parsejat->subpatrons[numSubpatrons]=q; q=p+1; numSubpatrons++; }
		p++;
	}
	parsejat->subpatrons[numSubpatrons]=q;
	numSubpatrons++;
	parsejat->numSubpatrons=numSubpatrons;
	parsejat->tipus=tipus;

}

unsigned long buscarPatrons(char **patrons,int numPatrons,FILE *fp,long offset)
{
	char linia[MAXFILELINE];
	int match, fi;
	register i,k;
	t_patro *patroParsejat,**llistaParsejats;
	unsigned long linies;
	char *p;

	llistaParsejats=(t_patro **) malloc(numPatrons*sizeof(t_patro *));
	for(k=0; k<numPatrons; k++) llistaParsejats[k]=(t_patro *) malloc(sizeof(t_patro));
	for(k=0; k<numPatrons; k++) parsejarPatro(patrons[k],llistaParsejats[k]);

	#ifdef DEBUG
		fprintf(stderr,"\nPatrons parsejats:");
		for(k=0; k<numPatrons; k++) {
			fprintf(stderr,"\n\ttipus= %d\n\tnumSubpatrons= %d\n",llistaParsejats[k]->tipus,llistaParsejats[k]->numSubpatrons);
			for(i=0; i<llistaParsejats[k]->numSubpatrons; i++) fprintf(stderr,"\tpatro%d= %s\n",i,llistaParsejats[k]->subpatrons[i]);
		}
	#endif

	linies=0;

	#if SYSTEM == NT4_2000
	if(type == EVENTLOG) {
		if(!ready) textRegistre(linia,MAXFILELINE,(unsigned long) offset,1);
		while(textSeguentRegistre(linia,MAXFILELINE,0,0)) {

			linies++;
			for(k=0; k<numPatrons; k++) {
				patroParsejat=llistaParsejats[k];
				match=0; fi=0;
				while(!match && !fi) {
					for(i=0; i<patroParsejat->numSubpatrons; i++) {
						if(strstr(linia,patroParsejat->subpatrons[i])) {
							if(patroParsejat->tipus==SIMPLE || patroParsejat->tipus==OR) { match=1; break; }
							else if(patroParsejat->tipus==AND) match=1;
						}
						else {
							if(patroParsejat->tipus==SIMPLE) { match=0; break; }
							else if(patroParsejat->tipus==AND) { match=0; break; }
						}
					}
					fi=1;
				}
				if(match) {
					p=linia;
					while(*p) {
						if(*p=='\n' || *p=='\r') *p=' ';
						p++;
					}
					while( (p-- >= linia) && *p==' ');
					*(++p)='\0';
					if(noMatches) fprintf(stdout,"%s",linia);
					else fprintf(stdout,"*-*%s",linia);
					noMatches=0;
				}
			}
			if((linies%procLinies)==0) release(idleTime);
		}
	}
	else {
	#endif
		fseek(fp,offset,SEEK_SET);
		while(!feof(fp)){

			p=fgets(linia,MAXFILELINE,fp);
			if(!p) break;

			linies++;
			for(k=0; k<numPatrons; k++) {
				patroParsejat=llistaParsejats[k];
				match=0; fi=0;
				while(!match && !fi) {
					for(i=0; i<patroParsejat->numSubpatrons; i++) {
						if(strstr(linia,patroParsejat->subpatrons[i])) {
							if(patroParsejat->tipus==SIMPLE || patroParsejat->tipus==OR) { match=1; break; }
							else if(patroParsejat->tipus==AND) match=1;
						}
						else {
							if(patroParsejat->tipus==SIMPLE) { match=0; break; }
							else if(patroParsejat->tipus==AND) { match=0; break; }
						}
					}
					fi=1;
				}
				if(match) {
					p=linia;
					while(*p) {
						if(*p=='\n' || *p=='\r') *p=' ';
						p++;
					}
					while( (p-- >= linia) && *p==' ');
					*(++p)='\0';
					if(noMatches) fprintf(stdout,"%s",linia);
					else fprintf(stdout,"*-*%s",linia);
					noMatches=0;
				}
			}
			#if SYSTEM == NT4_2000
			if((linies%procLinies)==0) release(idleTime);
			#elif SYSTEM == SUN_OS
			if((linies%procLinies)==0) {
				if(idleTime==0) thr_yield();
			}
			#else
			if((linies%procLinies)==0) {
				if(idleTime==0) sched_yield();
			}
			#endif
		}
		strcpy(ultimaLiniaFitxer,linia);
	#if SYSTEM == NT4_2000
	}
	#endif
	if(!noMatches) fputs("\n",stdout);
	return linies;
}

int separarPatrons(char *llista)
{
	char *p,*q;
	int count=0, i=0, max=0;

	// determinem el nombre de patrons i la mida del més llarg
	p=llista;
	while(*p!='\0') {
		i++;
		if(*p++==',') { count++; max=(i>max)?i-1:max; i=0; }
	}
	max=(i>max)?i:max;
	count++;
	max++; // reservem un espai pel '\0'

	// assignem memoria
	patrons=(char **) malloc(count*sizeof(char *));
	for(i=0; i<count; i++) patrons[i]=(char *) malloc(max*sizeof(char));

	// separem els patrons
	p=llista;
	i=0;
	q=patrons[0];
	while(*p!='\0') {
		*q++=*p++;
		if(*p==',') { *q='\0'; q=patrons[++i]; p++;}
	}
	*q='\0';

	#ifdef DEBUG
		fprintf(stderr,"\nPatrons a buscar: ");
		for(i=0; i< count; i++) fprintf(stderr,"  %s", patrons[i]);
	#endif

	return count;
}

int liniaBuida(char *linia)
{
	char *p;
	p=linia;
	while(*p!='\0') {
		if(*p!=' ' && *p!='\t' && *p!='\r' && *p!='\n') return 0;
		p++;
	}
	return 1;
}

long puntInici(char *nomFitxer, int accio, FILE *fpin)
{
	#if SYSTEM == NT4_2000
	char fitxerInici[MAXPATHCHARS]="c:\\nagios\\";
	#else
	char fitxerInici[MAXPATHCHARS]="/opt/nagios/bin/";
	#endif

	char prefix[MAXPATHCHARS]=PREFIX;
	char linia[MAXFILELINE],linia_aux[MAXFILELINE];
	long offset=0;
	char c, *dp;
	FILE *fp;

	dp=&nomFitxer[strlen(nomFitxer)];
	#if SYSTEM == NT4_2000
	while(*dp!='\\' && dp>=nomFitxer) dp--;
	#else
	while(*dp!='/' && dp>=nomFitxer) dp--;
	#endif
	strcat(fitxerInici,prefix);
	strcat(fitxerInici,++dp);

	switch(accio) {
		case LLEGIR:
			fp=fopen(fitxerInici,"r+b");
			if(fp!=NULL) {
				fgets(linia,100,fp);
				sscanf(linia,"%ld",&offset);
				linia[0]='\0';
				do {
					dp=fgets(linia_aux,MAXFILELINE,fp);
					if(dp) strcat(linia,linia_aux);
				} while (!feof(fp));
				fclose(fp);

				#ifdef DEBUG
					fprintf(stderr,"LLegits:\n\toffset=%ld\n\tlinia=%s\n",offset,linia);
				#endif

				if(type == FILELOG) {
					int length;
					fseek(fpin,0,SEEK_END);
					if(offset > ftell(fpin)) return 0; // el fitxer ha rotat

					fseek(fpin,offset,SEEK_SET);
					fgets(linia_aux,MAXFILELINE,fpin);
					// Els salts de linia al final porten problemes. Els eliminem abans de comparar.
					length=strlen(linia);
					if(linia[length-1]=='\n' || linia[length-1]=='\r') linia[length-1]='\0';
					if(length>=2 && (linia[length-2]=='\n' || linia[length-2]=='\r')) linia[length-2]='\0';
					length=strlen(linia_aux);
					if(linia_aux[length-1]=='\n' || linia_aux[length-1]=='\r') linia_aux[length-1]='\0';
					if(length>=2 && (linia_aux[length-2]=='\n' || linia_aux[length-2]=='\r')) linia_aux[length-2]='\0';


					if(!strcmp(linia,linia_aux)) {

						#ifdef DEBUG
							fprintf(stderr,"Les línies coincideixen\n");
						#endif

						return ftell(fpin); // si les línies coincideixen, el log no ha rotat
					}
					else return 0;
				}
				#if SYSTEM == NT4_2000
				else {
					return offsetInici(offset, linia);
				}
				#endif
			}
			else {
				#if SYSTEM == NT4_2000
					if(type == FILELOG)	return 0;
					else return offsetInici(0, "");
				#else
					return 0;
				#endif
			}
			break;
		case GUARDAR:
			if((fp=fopen(fitxerInici,"wb"))==NULL) {
				fprintf(stdout,"Error al crear el fichero auxiliar %s (no se pudo guardar la ultima linea analizada)",fitxerInici);
				exit(2);
			}

			if(type == FILELOG) {
				unsigned long filePos;
				#ifdef DEBUGMAX
					fputc('\n',stderr);
				#endif

				//obtenim i guardem l'útlima línia i el seu offset
				fseek(fpin,-1,SEEK_END);
				filePos=ftell(fpin);

	next:
//				while((c=fgetc(fpin))=='\x0D' || c=='\x0A') {   // saltem les línies en blanc
				c=fgetc(fpin);
				while(c=='\n' || c=='\r') {	// saltem les línies en blanc
					#ifdef DEBUGMAX
						fprintf(stderr,"Caràcter extrany %x\n",c);
						fprintf(stdout,"\tfilePos=%u\n", filePos);
					#endif

					if(filePos>0) {
						fseek(fpin,-2,SEEK_CUR);
						c=fgetc(fpin);
						filePos--;
					}
					else break;
				}
				while(c!='\n' && c!='\r') {
					#ifdef DEBUGMAX
						fputc(c,stderr);
						fprintf(stdout,"\tfilePos=%u\n", filePos);
					#endif

					if(filePos>0) {
						if(fseek(fpin,-2,SEEK_CUR)) break; // principi del fitxer?
						filePos--;
						c=fgetc(fpin);
					}
					else break;
				}
				if(filePos==0) fseek(fpin,-1,SEEK_CUR);
				offset=ftell(fpin);
				fgets(linia,MAXFILELINE,fpin);

				if(liniaBuida(linia) && filePos>0) { fseek(fpin,offset-1,SEEK_SET); goto next;}
			}
			#if SYSTEM==NT4_2000
			else offsetFinal(&offset,linia);
			#endif

			#ifdef DEBUG
				fprintf(stderr,"Guardats:\n\toffset=%ld\n\tlinia=%s",offset,linia);
			#endif

			// i els guardem al fitxer auxiliar
			fprintf(fp,"%ld\n%s",offset,linia);
			fclose(fp);
			break;
	}

	return offset;
}

int main(int argc, char *argv[])
{
	char *dp;
	char *nomFitxer;
	char *llistaPatrons;
	char sysCommand[1024];

	int numPatrons=0;
	long offset=0;
	int fitxerTemporal=0;
	unsigned long liniesProcessades=0;
	int i;

	#if TIMING
	time_t inici,final;
	clock_t start,finish;
	#endif

	FILE *fpin;


#ifndef SYSTEM
	// cal especificar el sistema a l'hora de compilar
	fprintf(stderr,"\nSYSTEM no definido.\nRecompilar utilizando el parametro -DSYSTEM=<target_OS>\n<target_OS>:\n\tHP_UX\n\tSUN_OS\n\tLINUX\n\tNT4_2000\n");
	exit(2);
#else

	if(argc!=3 && argc!=4) {
		fprintf(stderr,"Número de argumentos incorrecto\n");
		usage();
		exit(2);
	}

	nomFitxer=argv[1];
	llistaPatrons=argv[2];

	if(argc==4) {
		#if SYSTEM==NT4_2000
		sscanf(argv[3],"%ldms%ld",&idleTime,&procLinies);
		#else
		sscanf(argv[3],"%ldms%ld",&idleTime,&procLinies);
		idleTime=0;
		#endif
	}
	else { idleTime=0; procLinies=1; }


	#ifdef TIMING
	start=clock();
	time(&inici);
	#endif

	dp=&nomFitxer[strlen(nomFitxer)];
	// conversió dels fitxers binaris a text
	#if SYSTEM == HP_UX
		while(*dp!='/' && dp>=nomFitxer) dp--;
		++dp;
		if(!strcmp(dp,"wtmp")) {
			sprintf(sysCommand,"/usr/sbin/acct/fwtmp < %s > /opt/nagios/bin/tmp_checklog.tmp",nomFitxer);
			system(sysCommand);
			fitxerTemporal=1;
		}
		type=FILELOG;
	#elif SYSTEM == SUN_OS
		while(*dp!='/' && dp>=nomFitxer) dp--;
		++dp;
		if(!strcmp(dp,"wtmpx")) {
			sprintf(sysCommand,"/usr/lib/acct/fwtmp < %s > /opt/nagios/bin/tmp_checklog.tmp",nomFitxer);
			system(sysCommand);
			fitxerTemporal=1;
		}
		type=FILELOG;
	#elif SYSTEM == LINUX
		while(*dp!='/' && dp>=nomFitxer) dp--;
		++dp;
		if(!strcmp(dp,"wtmp")) {
			sprintf(sysCommand,"/usr/bin/last -f %s > /opt/nagios/bin/tmp_checklog.tmp",nomFitxer);
			system(sysCommand);
			fitxerTemporal=1;
		}
		type=FILELOG;
	#elif SYSTEM == NT4_2000
		// no s'aplica a windows
		//DisplayEntries(nomFitxer);
		//return 0;
		setPriority();
		if(!strcmp(nomFitxer,"APPLICATION") ||
		   !strcmp(nomFitxer,"SYSTEM") ||
		   !strcmp(nomFitxer,"SECURITY") ) type=EVENTLOG;
		else type=FILELOG;
	#else
		fprintf(stderr,"\nWARNING! SYSTEM definido sin valor.\nRecompilar usando el parámetro -DSYSTEM=<target_OS>\n<target_OS>:\n\tHP_UX\n\tSUN_OS\n\tLINUX\n\tNT4_2000\n");
	#endif

	// obrir el fitxer de logs
	#if SYSTEM == NT4_2000
		if(type==EVENTLOG) {
			if(!obrirEventLog(nomFitxer)) {
				//fprintf(stderr, "No se pudo abrir el log siguiente: %s\n", nomFitxer);
				fprintf(stdout,"ERROR. El fichero %s no existe",nomFitxer);
				exit(2);
			}
		}
		else if((fpin=fopen(nomFitxer,"rb"))==NULL) {
			//fprintf(stderr,"No existe el fichero %s\n",nomFitxer);
			fprintf(stdout,"ERROR. El fichero %s no existe",nomFitxer);
			exit(2);
		}
	#else
		if(fitxerTemporal==1) {
			if((fpin=fopen("/opt/nagios/bin/tmp_checklog.tmp","rb"))==NULL) {
				//fprintf(stderr,"No se ha creado el fichero %s (probablemente el fichero de logs binario no se convirtió a texto)\n",nomFitxer);
				fprintf(stdout,"ERROR. El fichero /opt/nagios/bin/tmp_checklog.tmp no existe");
				exit(2);
			}
		}
		else if((fpin=fopen(nomFitxer,"rb"))==NULL) {
			//fprintf(stderr,"No existe el fichero %s\n",nomFitxer);
			fprintf(stdout,"ERROR. El fichero %s no existe",nomFitxer);
			exit(2);
		}
		//setvbuf(fpin,NULL,_IOFBF,32768);
	#endif

	// en principi no cal analitzar el fitxer sencer
	offset=puntInici(nomFitxer,LLEGIR,fpin);

	// separa els patrons
	numPatrons=separarPatrons(llistaPatrons);

	// busquem tots els patrons
	liniesProcessades=buscarPatrons(patrons,numPatrons,fpin,offset);

	// guardem el punt on començarem a analitzar la próxima vegada
	if(liniesProcessades>0) puntInici(nomFitxer,GUARDAR,fpin);

	#if SYSTEM == NT4_2000
		if(type == EVENTLOG) tancarEventLog();
		else fclose(fpin);
	#else
		fclose(fpin);
	#endif

	#ifdef TIMING
	time(&final);
	finish=clock();
	fprintf(stderr,"\n%lu linies processades en %lu segons",liniesProcessades, (unsigned long)(final-inici));
	fprintf(stderr,"\nCàlcul alternatiu: %f segons", (double)(finish-start)/CLOCKS_PER_SEC );
	#endif

	if(noMatches) fputs("0 patrones encontrados",stdout);
	else return 1;

	return 0;

#endif


}
