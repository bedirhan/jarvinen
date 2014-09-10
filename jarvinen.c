#include <stdio.h>
#include <string.h>
#include <pcre.h>
#include <semaphore.h>
#include <pthread.h>
#include <getopt.h>

#define OVECCOUNT 30
#define IDS_COUNT 1024

void *apache_parser(void *param);

sem_t g_sem;
pcre *list[IDS_COUNT];
char *idslist[IDS_COUNT];

int main(int argc, char *argv[])
{
    	pthread_t tid;

    	char *logfile;
    	char *parser_line;
    	char *idsfile;
    	char log_line[8092];
    	char *ids_rules[1024];
    	char ids_line[1024];
	char *line;
	const char *error;
    	int maxThread, erroffset, result, semval, i;

	char *targ;
	char *sarg;
	int lflag = 0;
	int xflag = 0;    
	int tflag = 0;
	int sflag = 0;
	int ch;

    	FILE *log;
    	FILE *ids;
    
	while ((ch = getopt(argc, argv, "l:x:s:t:")) != -1)
		switch (ch) {
			case 'l':
				lflag = 1;
				logfile = optarg;
				break;
			case 'x':
				xflag = 1;
				idsfile = optarg;
				break;
			case 's':
				sflag = 1;
				sarg = optarg;
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
	} else if (!xflag) {
		fprintf(stderr, "-x switch must be specified!\n");
                exit(EXIT_FAILURE);
	} else if (!sflag) {
		fprintf(stderr, "-s switch must be specified!\n");
                exit(EXIT_FAILURE);
	}

	maxThread = (int) strtol(targ, NULL, 10);

    	log = fopen(logfile, "r");
    	if (log == NULL) {
		fprintf(stderr, "\"%s\" Dosyasi Acilamadi !\n", logfile);
		exit(EXIT_FAILURE);
    	}
    
    	ids = fopen(idsfile, "r");
    	if (ids == NULL) {
		fprintf(stderr, "\"%s\" Dosyasi Acilamadi !\n", idsfile);
		exit(EXIT_FAILURE);
    	}
    
    	i = 0;
    	while ( (fgets(ids_line, sizeof(ids_line), ids) ) != NULL) {
		idslist[i] = (char *)malloc(IDS_COUNT);
		strncpy(idslist[i], ids_line, strlen(ids_line)+1);
		list[i] = pcre_compile( ids_line, 0, &error, &erroffset, NULL );
                if (list[i] == NULL) {
                	fprintf(stderr, "PCRE compilation: %s failed at offset %d: %s\n", ids_line, erroffset, error);
                        exit(EXIT_FAILURE);
                }
		i = i + 1;
    	}
	idslist[i] = NULL;
	list[i] = NULL;
    	fclose(ids);
    
        if (sem_init(&g_sem, 0, maxThread) < 0) {
		perror("sem_init");
		exit(EXIT_FAILURE);
    	}
    
    	while ( (fgets(log_line, sizeof(log_line), log) ) != NULL) {
		line = (char *)malloc(1024);
		strncpy(line, log_line, strlen(log_line)+1);
	
		sem_wait(&g_sem);
		if ((result = pthread_create(&tid, NULL, (void *)apache_parser, (void *)line)) != 0) {
			fprintf(stderr, "pthread_create: %s\n", strerror(result));
			exit(EXIT_FAILURE);
		}
		pthread_detach(tid);
    	}
  
    	while (sem_getvalue(&g_sem, &semval), semval != maxThread) 
		usleep(100000);

	for(i=0; list[i] != NULL; i++) {
		idslist[i];
	        pcre_free(list[i]);
	}
	
    	fclose(log);
	    
	pthread_exit(NULL);
    	return 0;
}

void *apache_parser(void *param)
{
	pcre *re;

	int k,rc,count;
	int ovector[OVECCOUNT];
	char seperator[2] = " ";
	char *token;
	char **ids_rules;
	char tmp_line[1024];

	char *line = (char *)param;

	count = 0;
        token = strtok(line, seperator);

	while( token != NULL ) {
	    token = strtok(NULL, seperator);
	    if (count == 5 && token != NULL) {
		strncpy(tmp_line, token, strlen(token) + 1);	
		for (k = 0; list[k] != NULL; k++) {
	            	rc = pcre_exec( list[k], NULL, token, (int)strlen(token), 0, 0, ovector, OVECCOUNT );
			if (rc > 0)
	               		printf("Match: %s <=> Regex: %s -\n", tmp_line, idslist[k]);
		}
		break;
	    }
	    count = count + 1;
	}

	free(line);
	sem_post(&g_sem);

	pthread_exit(NULL);
}
