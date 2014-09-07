#include <stdio.h>
#include <string.h>
#include <pcre.h>
#include <semaphore.h>
#include <pthread.h>

#define OVECCOUNT 30

void *thread_proc(void *param);
sem_t g_sem;

pcre *list[1024];

struct THREAD_PARAM {
	char line[8092];
	char **idsrules;
	//pcre **list;
};

int main(int argc, char **argv)
{
    	char *logfile;
    	char *parser_line;
    	char *idsfile;
    	char log_line[8092];
    	char *ids_rules[1024];
    	char ids_line[1024];
	const char *error;
	int erroffset;			
    	int i, maxThread, result, semval;
    
    	FILE *log;
    	FILE *ids;
    
    	pthread_t tid;
    	maxThread = 30;
    
    	logfile = argv[1];
    	idsfile = argv[2];
    
    	log = fopen(logfile, "r");
    	if (log == NULL) {
		fprintf(stderr, "Dosya Acilamadi !!!", log);
		exit(EXIT_FAILURE);
    	}
    
    	ids = fopen(idsfile, "r");
    	if (ids == NULL) {
		fprintf(stderr, "Dosya Acilamadi !!!", ids);
		exit(EXIT_FAILURE);
    	}
    
    	i = 0;
    	while ( (fgets(ids_line, sizeof(ids_line), ids) ) != NULL) {
		list[i] = pcre_compile( ids_line, 0, &error, &erroffset, NULL );
                if (list[i] == NULL) {
                	fprintf(stderr, "PCRE compilation: %s failed at offset %d: %s\n", ids_line, erroffset, error);
                        exit(EXIT_FAILURE);
                }
		//ids_rules[i] = (char *)malloc(2048);
		//strncpy(ids_rules[i], ids_line, strlen(ids_line)-1);
		i = i + 1;
    	}
	list[i] = NULL;
	//ids_rules[i] = NULL;
    	fclose(ids);
    
        if (sem_init(&g_sem, 0, maxThread) < 0) {
		perror("sem_init");
		exit(EXIT_FAILURE);
    	}
    
    	while ( (fgets(log_line, sizeof(log_line), log) ) != NULL) {
		struct THREAD_PARAM *th_param;
		th_param = (struct THREAD_PARAM *)malloc(sizeof(struct THREAD_PARAM));
		strncpy(th_param->line, log_line, strlen(log_line)+1);
		//th_param->idsrules = ids_rules;
		//th_param->list = ids_rules;
	
		sem_wait(&g_sem);
		if ((result = pthread_create(&tid, NULL, (void *)thread_proc, (void *)th_param->line)) != 0) {
			fprintf(stderr, "pthread_create: %s\n", strerror(result));
			exit(EXIT_FAILURE);
		}
		pthread_detach(tid);
    	}
  
    	while (sem_getvalue(&g_sem, &semval), semval != maxThread) 
		usleep(100000);

	for(i=0; list[i] != NULL; i++)
	        pcre_free(list[i]);
		//free(ids_rules[i]);
	
    	fclose(log);
	    
	pthread_exit(NULL);
    	return 0;
}


void *thread_proc(void *param)
{
	pcre *re;

	int count, k ;
	//int erroffset, rc;
	int rc;
	int ovector[OVECCOUNT];
	//const char *error;
	char seperator[2] = " ";
	char *token;
	char tmp_line[8092];
	char **ids_rules;

	char *line = (char *)param;
	//struct THREAD_PARAM *params = (struct THREAD_PARAM *)param;
	//ids_rules = params->idsrules;
	//strncpy(tmp_line, params->line, (int)strlen(params->line)+1);

	count = 0;
        //token = strtok(params->line, seperator);
        //token = strtok(params, seperator);
        token = strtok(line, seperator);

	while( token != NULL ) {
	    token = strtok(NULL, seperator);
	    if (count == 5 && token != NULL) {
		for (k = 0; list[k] != NULL; k++) {

	       		//re = pcre_compile( ids_rules[k], 0, &error, &erroffset, NULL );
	        	//if (re == NULL) {
		       	//	fprintf(stderr, "PCRE compilation: %s:%s failed at offset %d: %s\n", ids_rules[k], token, erroffset, error);
		       	//	exit(EXIT_FAILURE);
	            	//}
			    
	            	rc = pcre_exec( list[k], NULL, token, (int)strlen(token), 0, 0, ovector, OVECCOUNT );

			//if (rc < 0) {
        		//	switch (rc) {
            		//		case PCRE_ERROR_NOMATCH:
                	//			break;
            		//		default:
                	//			printf("Error while matching: %d\n", rc);
                	//			break;
        		//	}
    			//} else
	               	//	printf("Match ::%s:: - Regex ::%s::\n", line, ids_rules[k]);
			//
			if (rc > 0)
	               		printf("Match ::%s::\n", line);
	            	//pcre_free(list[k]);
		}
		break;
	    }
	    count = count + 1;
	}

	free(line);
	sem_post(&g_sem);

	pthread_exit(NULL);
}
