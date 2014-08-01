/*
    This file is part of 3d'oh, a multiplatform 3do emulator written by Gabriel Ernesto Cabral.

    3d'oh is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    3d'oh is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with 3d'oh.  If not, see <http://www.gnu.org/licenses/>.

*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>


	FILE* config;
	long fsize;
	int readcount;


void configOpen(char *file)
{


	printf("INFO: openning config file in %s\n",file);
	config=fopen(file,"rb");
	if (config==NULL)printf("ERROR: Config load error\n");
	printf("INFO: Config file opened\n");
    fseek (config , 0 , SEEK_END);
	fsize = ftell(config);
	rewind (config);



}


void configClose()
{

	fclose(config);

}



int configReadInt(char *section,char *name)
{

	char value[20];
	char temp[20];
	char stemp[20];
//	char ẗemp[20];
	int sectionfound=0;

	//search section

	memset(stemp,0,sizeof(stemp));
	memset(temp,0,sizeof(temp));
//	fgets(temp,20,config);

//	while((!feof(config))&&(!sectionfound))
//	{
	while(fscanf(config,"%[^\n]",temp)==2)
	{
	
		sprintf(stemp,"[%s]",section);
		printf(stemp);

		//if section found
		if(strcmp(stemp,temp)==0)
		{
			printf("encontrada sección %s %s %d\n",temp,stemp,strcmp(stemp,temp));
			sectionfound=1;

		}


	}

	//search value
	if(sectionfound)
	{

		while(!feof(config))
		{

			fgets(temp,20,config);

			//buscar etiqueta
			fscanf(config,"%[^=]%[^\n]",temp,value);

				if(strcmp(temp,name)==0)
				{
					printf("encontrada etiqueta %s %s\n",temp, value);

					return 1;
				}


		}


	}else{
	
		printf("Section not found\n");

	}
	rewind (config);
	return 0;

}


char *configReadString(char *section,char *name)
{

	char *value;
	char temp[20];
	char stemp[20];
//	char ẗemp[20];
	int sectionfound=0;

	//search section

	value=(char *)malloc(sizeof(char)*20);
	memset(stemp,0,sizeof(stemp));
	memset(temp,0,sizeof(temp));
//	fgets(temp,20,config);

	printf("searching section %s value %s\n",section,name);

	while((!feof(config))&&(!sectionfound))
//	while(fscanf(config,"%[^\n]",temp)==2)
	{
//		fscanf(config,"%[^\n] ",temp);
		fgets(temp,20,config);
		sprintf(stemp,"[%s]",section);

		printf("found: %s\n",temp);
		printf("match: %s\n",stemp);
		

		//if section found
		if(strcmp(stemp,temp)==0)
		{
			printf("encontrada sección %s\n",temp);
			sectionfound=1;
			break;

		}
		fflush(stdout);
	//	fscanf(config," ",temp);


	}

	//search value
	if(sectionfound)
	{
		memset(temp,0,sizeof(temp));
		while(!feof(config))
		{

		//	fgets(temp,20,config);
//			fscanf(config,"%*[\n]",temp);
			fgets(temp,20,config);
			printf("value found: %s\n",temp);

			//buscar etiqueta
			fscanf(config,"%[^=]=%[^\n]",temp,value);

				if(strcmp(temp,name)==0)
				{
					printf("encontrada etiqueta %s %s\n",temp, value);
					rewind (config);
					return value;
				}


		}


	}
	rewind (config);
	return "JOY_BUTTON0";

}



void configWrite()
{




}


void configInitDefaults()
{





}
