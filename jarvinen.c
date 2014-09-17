#include <stdio.h>
#include <string.h>
#include <pcre.h>
#include <semaphore.h>
#include <pthread.h>
#include <getopt.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include "jarvinen.h"


int main(int argc, char *argv[])
{
    	pthread_t tid;

    	char *logfile;
    	char *idsfile;
	char *line; 
	char log_line[LOG_LINE];
	const char *error;
    	int maxThread, erroffset, result, semval, check, i, k;

	char *targ;
	int lflag = 0;
	int fflag = 0;    
	int tflag = 0;
	int ch;

    	FILE *log; 

	while ((ch = getopt(argc, argv, "l:f:t:")) != -1)
		switch (ch) {
			case 'l':
				lflag = 1;
				logfile = optarg;
				break;
			case 'f':
				fflag = 1;
				idsfile = optarg;
				break;
			case 't':
				tflag = 1;
				targ = optarg;
				break;
			case '?':
				fprintf(stderr, "invalid argument: '%c'\n", optopt);
				exit(EXIT_FAILURE);
		}

	if (!tflag) {
		fprintf(stderr, "-t switch must be specified!\n");
		exit(EXIT_FAILURE);
	} else if (!lflag) {
		fprintf(stderr, "-l switch must be specified!\n");
                exit(EXIT_FAILURE);
	} else if (!fflag) {
		fprintf(stderr, "-f switch must be specified!\n");
                exit(EXIT_FAILURE);
	}

	maxThread = (int) strtol(targ, NULL, 10);

	parseDoc (idsfile, idsxml);
	for(i=0; idsxml[i] != NULL; i++) {
		list[i] = pcre_compile( (char *)idsxml[i]->rule, 0, &error, &erroffset, NULL );
		if (list[i] == NULL) {
                        fprintf(stderr, "PCRE compilation: %s failed at offset %d: %s\n", (char *)idsxml[i]->rule, erroffset, error);
                        exit(EXIT_FAILURE);
                }
        }
	list[i] = NULL;

    	log = fopen(logfile, "r");
    	if (log == NULL) {
		fprintf(stderr, "\"%s\" Dosyasi Acilamadi !\n", logfile);
		exit(EXIT_FAILURE);
    	}
    
    	if (sem_init(&g_sem, 0, maxThread) < 0) {
		perror("sem_init");
		exit(EXIT_FAILURE);
    	}
    
    	while ( (fgets(log_line, sizeof(log_line), log) ) != NULL) {
		line = (char *)malloc(LOG_LINE);
		strncpy(line, log_line, strlen(log_line)+1);
		sem_wait(&g_sem);
		if ((result = pthread_create(&tid, NULL, (void *)apache_parser, (void *)line)) != 0) {
			fprintf(stderr, "pthread_create: %s\n", strerror(result));
			exit(EXIT_FAILURE);
		}
		pthread_detach(tid);
    	}
	fclose(log);
  
    	while (sem_getvalue(&g_sem, &semval), semval != maxThread) 
		usleep(100000);

	for (i=0; idsxml[i] != NULL; i++) { 
	    xmlFree(idsxml[i]->id);
	    xmlFree(idsxml[i]->rule);
	    xmlFree(idsxml[i]->description);
	    xmlFree(idsxml[i]->impact);
	    for(k=0; idsxml[i]->tag[k] != NULL; k++)
		xmlFree(idsxml[i]->tag[k]);
	    free(idsxml[i]);
	    pcre_free(list[i]);
	}
    	   
	pthread_exit(NULL);
	
    	return 0;
}

void *apache_parser(void *param)
{
	pcre *re;
	int rc,count,i,k;
	int ovector[OVECCOUNT];
	char seperator[2] = " ";
	char *token;
	char timestamp[1024];
	char url[2048];

	char *log_line = (char *)param;

	count = 0;
        token = strtok(log_line, seperator);

	while( token != NULL ) {
	    token = strtok(NULL, seperator);
	    if (count == 2 && token != NULL) {
		token = token + 1;
		strncpy(timestamp, token, strlen(token) + 1);
	    } else if  (count == 5 && token != NULL) {
		strncpy(url, token, strlen(token) + 1);	
		for (k = 0; list[k] != NULL; k++) {
	            	rc = pcre_exec( list[k], NULL, token, (int)strlen(token), 0, 0, ovector, OVECCOUNT );
			if (rc > 0) {
				for (i=0; idsxml[k]->tag[i] != NULL; i++) {
				    if (idsxml[k]->tag[i+1] == NULL)
					printf("%s - ", idsxml[k]->tag[i]);
				    else
					printf("%s,", idsxml[k]->tag[i]);
				}
				printf("%s - %s - %s\n", url, log_line, timestamp);  
			}	
		}
		break;
	    }
	    count = count + 1;
	}

	free(log_line);
	sem_post(&g_sem);

	pthread_exit(NULL);
}


static void parseDoc(char *docname, struct IDS_XML_NODE **idsxml) {
        xmlDocPtr doc;
        xmlNodePtr cur;
        int i;

        doc = xmlParseFile(docname);
        if (doc == NULL ) {
                fprintf(stderr,"Document not parsed successfully. \n");
                exit(EXIT_FAILURE);
        }

        cur = xmlDocGetRootElement(doc);
        if (cur == NULL) {
                fprintf(stderr,"empty document\n");
                xmlFreeDoc(doc);
                exit(EXIT_FAILURE);
        }

        if (xmlStrcmp(cur->name, (const xmlChar *) "filters")) {
                fprintf(stderr,"document of the wrong type, root node != filters");
                xmlFreeDoc(doc);
                exit(EXIT_FAILURE);
        }

        i = 0;
        cur = cur->xmlChildrenNode;
        while (cur != NULL) {
                if ((!xmlStrcmp(cur->name, (const xmlChar *)"filter"))){
                        idsxml[i] = (struct IDS_XML_NODE *)malloc(sizeof(struct IDS_XML_NODE));
                        parse_node(doc,cur, idsxml[i]);
                        i = i + 1;
                }
                cur = cur->next;
        }
        idsxml[i] = NULL;

        xmlFreeDoc(doc);

        return;
}


void parse_node (xmlDocPtr doc, xmlNodePtr cur, struct IDS_XML_NODE *idsxml) {
        xmlNodePtr tmp_cur;
        int count = 0;

        cur = cur->xmlChildrenNode;
        while (cur != NULL) {
                if ((!xmlStrcmp(cur->name, (const xmlChar *)"id"))) {
                        idsxml->id = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
                } else if ((!xmlStrcmp(cur->name, (const xmlChar *)"rule"))) {
                        idsxml->rule = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
                } else if ((!xmlStrcmp(cur->name, (const xmlChar *)"description"))) {
                        idsxml->description = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
                } else if ((!xmlStrcmp(cur->name, (const xmlChar *)"tags"))) {
                        tmp_cur = cur;
                        cur = cur->xmlChildrenNode;
                        while(cur != NULL) {
                                if ((!xmlStrcmp(cur->name, (const xmlChar *)"tag"))) {
                                        idsxml->tag[count] = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
                                        count = count + 1;
                                }
                                cur = cur->next;
                        }
                        idsxml->tag[count] = NULL;
                        cur = tmp_cur;
                } else if ((!xmlStrcmp(cur->name, (const xmlChar *)"impact"))) {
                        idsxml->impact = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
                }

                cur = cur->next;
        }

        return;
}

