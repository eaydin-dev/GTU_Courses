
#include "Mine.h"

static sub_t *subs = NULL;
static int iSubSize = 10;
static int iSubIndex = 0;
static int iSubFilled = 0;


static word_t *wordArr = NULL;
static int iWordArrIndex = 0;
static int iWordArrSize = 20;

static miner_info_t * miners = NULL;
static int iMinerArrSize = 2;
static int iMinerArrIndex = 0;

static pthread_mutex_t w_lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t s_lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t v_lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t miners_lock = PTHREAD_MUTEX_INITIALIZER;

static int iWorking = 0;
static int iDone = 0;
static int iPending = 0;
static int iLoggerFlag = 0;
static int iDoneFlag = 0;

static int socDescriptor;

static pthread_t pthLogger;
static pthread_t *pthArr = NULL;
static int pthArrIndex = 0;
static int iAliveThreads = 0;


int main(int argc, char **argv){
	int i;
	int client_sock, socSize, *socNew;
    struct sockaddr_in server , client;
	pthread_attr_t attr;
	
	if(argc < 2){
		fprintf(stderr, "usage: %s {path_1} {path_2} ... \n", argv[0]); exit(1);
	}
	else if(argc > MAX_ARGS){
		fprintf(stderr, "please enter less than or equal to %d paths.\n", MAX_ARGS); exit(1);
	}
	
	signal(SIGINT, sig_handler);
	pthread_attr_init(&attr);
	
    init();

	for(i = 1; i < argc; ++i)
		get_subs(argv[i]);

	if(pthread_create(&pthLogger, NULL, logger, 0) < 0){
		perror("could not create logger thread"); quit(1);
	}
	
    if ((socDescriptor = socket(AF_INET , SOCK_STREAM , 0)) == -1){
    	perror("couldn't get socket"); exit(1);
    }
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(S_PORT);
 
    if(bind(socDescriptor, (struct sockaddr *)&server, sizeof(server)) < 0){
        perror("bind failed. Error"); exit(1);
    }

    listen(socDescriptor , 3);
    socSize = sizeof(struct sockaddr_in);

    while((client_sock = accept(socDescriptor, (struct sockaddr *)&client, (socklen_t*)&socSize))){
        fprintf(stderr, "a miner connected.\n");
         
        socNew = malloc(1);
        *socNew = client_sock;
         
        if(pthread_create(&pthArr[pthArrIndex++], &attr, client_handler, socNew) < 0){
            perror("could not create thread"); quit(1);
        }
    }
	
    if(client_sock < 0){
    	perror("error on accept"); quit(1);
    }

    clean();
	return 0;
}

void * client_handler(void *socket_desc){
    int sock = *(int*)socket_desc;
    int i, tmp, flag = 0, gold = 0;
    int index;
    sem_t *cl_sem;
    struct cl_info_t *cl;
    struct pre_info pre;
    time_t t;
    t = time(NULL); 
	++iAliveThreads;
	
    recv(sock, &pre, sizeof(struct pre_info), 0);
    
    pthread_mutex_lock(&miners_lock);
    index = iMinerArrIndex++;
    if(iMinerArrIndex == iMinerArrSize - 1){
		iMinerArrSize *= 2;
		miners = (miner_info_t *) realloc(miners, iMinerArrSize * sizeof(miner_info_t));
	}
    strcpy(miners[index].s_time, ctime(&t));
    miners[index].pid = pre.pid;
    pthread_mutex_unlock(&miners_lock);
	
    if (get_named_sem(pre.semname, &cl_sem, 1) == -1) {
		perror("Failed to create named semaphore"); quit(1);
	}

	cl = (struct cl_info_t *)shmat(pre.shmId, NULL, 0);
	if (cl == (void *)-1){
		perror("Can't attach in mine"); quit(1);	
	}

    handle_vars(0, 1, 0, index, PENDING);
    
    PEND:
    if(have_subs(pre.n)){		
		i = 0, flag = 0;
		while(1){
			while(sem_wait(cl_sem) == -1)
			if(errno != EINTR){
				perror("Failed to lock semaphore"); quit(1);
			}
			
			if(cl->status == FIRST || cl->status == RECEIVED){
				if(i != pre.n){
					pthread_mutex_lock(&s_lock);
					strcpy(cl->info, subs[iSubIndex++].sub);
					cl->status = SENT;
					++i;
					pthread_mutex_unlock(&s_lock);
				}
				
				else{
					cl->status = FILLED;
					flag = 1;
				}
			}

			if(sem_post(cl_sem) == -1){
				perror("Failed to unlock semaphore"); quit(1);
			}		
			if(flag)
				break;		
		}	
    }
	else{
		sleep(1);
		goto PEND;
	}
	
    handle_vars(1, -1, 0, index, WORKING);
    flag = 0;
    i = 0;

    while(1){

    	while(sem_wait(cl_sem) == -1)
		if(errno != EINTR){
			perror("Failed to lock semaphore"); quit(1);
		}
		if(cl->status == SENT){
	        if(cl->type == WORD){
	        	++i;
	        	tmp = tryadd(cl->info, cl->amount);
	        	cl->gold += tmp;
	        	cl->newword = tmp;
	        	gold = cl->gold;
	        	if(i == 1)
	        		cl->first = tmp;
	        }
	        else if(cl->type == DIRECTORY)
	        	get_subs(cl->info);
			
	        cl->status = RECEIVED;
		}
		else if(cl->status == DONE)
			flag = 1;
		
	    if(sem_post(cl_sem) == -1){
			perror("Failed to unlock semaphore"); quit(1);
		}
		if(flag)
			break;
    }
    
    if (sem_close(cl_sem) == -1)
    	perror("some error while closing named sem");

	t = time(NULL);
	pthread_mutex_lock(&miners_lock);
	strcpy(miners[index].f_time, ctime(&t));
	miners[index].gold = gold;
	pthread_mutex_unlock(&miners_lock);
	
    fflush(stdout);
    shmdt(cl);
    free(socket_desc);
    handle_vars(-1, 0, 1, index, SERVED); 
    
	--iAliveThreads;
	return 0;
}

void clean(){
	if(subs != NULL)
		free(subs);
	if(wordArr != NULL)
		free(wordArr);
	if(miners != NULL)
		free(miners);
	if(pthArr != NULL)
		free(pthArr);

	close(socDescriptor);
}

void init(){
	miners = (miner_info_t *) calloc(iMinerArrSize, sizeof(miner_info_t));
	wordArr = (word_t *) malloc(MAX * sizeof(word_t));
	pthArr = (pthread_t *) calloc(MAX/4, sizeof(pthread_t));
	subs = (sub_t *) calloc(iSubSize, sizeof(sub_t));
}

void wait_threads(){
	int i;
	for(i = 0; i < pthArrIndex; ++i)
		if(pthArr[i] != 0)
			if(pthread_join(pthArr[i], NULL)){
				fprintf(stderr, "Cannot join thread, at iteration %d\n", i);
				quit(1);
			}
	
}

void quit(int val){
    clean();
    fprintf(stderr, "got <CRTL-C> signal.\n");
	exit(val);
}

void getvars(int *w, int *p, int *d){
	pthread_mutex_lock(&v_lock);
	*w = iWorking;
	*p = iPending;
	*d = iDone;
	pthread_mutex_unlock(&v_lock);
}

void * logger(void * nothing){
	int w, p, d, i, x = 0;
	FILE *lf;
	char *status;
	char *serve = "served";
	char *work = "working";
	char *pend = "pending";
	time_t t;
     
	sleep(1);
	while(1){
		lf = fopen(LOGNAME, LOG_FILE_TYPE);
		if(lf == NULL){
			perror("cant open log file"); quit(1);
		}
		
		if(iLoggerFlag){
			t = time(NULL);
			fprintf(lf, "<CRTL-C> signal caught at: %s\n", ctime(&t));
			fclose(lf);
			break;
		}
		
		pthread_mutex_lock(&miners_lock);
		pthread_mutex_lock(&w_lock);
		
		getvars(&w, &p, &d);
		fprintf(lf, "# of served miners: %d\n", d);
		
		for(i = 0; i < iMinerArrIndex; ++i){
			if(miners[i].stat == SERVED)
				fprintf(lf, "miner %d earned %d golds\n", 
					miners[i].pid, miners[i].gold);
		}
		fprintf(lf, "\n# of pending miners: %d\n", p);
		fprintf(lf, "# of currently working miners: %d\n\n", w);
		
		for(i = 0; i < iMinerArrIndex; ++i){
			if(miners[i].stat == SERVED)
				status = serve;
			else if(miners[i].stat == WORKING)
				status = work;
			else 
				status = pend;
				
			fprintf(lf, "miner %d (%s) start time: %s",
				miners[i].pid, status, miners[i].s_time);			
		}
		
		fprintf(lf, "\n");
		for(i = 0; i < iMinerArrIndex; ++i)
			if(miners[i].stat == SERVED)
				fprintf(lf, "miner %d stop time: %s",
					miners[i].pid, miners[i].f_time);
		
		fprintf(lf, "\n");
		for(i = 0; i < iWordArrIndex; ++i)
			fprintf(lf, "%20s - %3d\n", wordArr[i].word, wordArr[i].amount);
		
		fprintf(lf, "--------------------------------------------\n\n");
		pthread_mutex_unlock(&w_lock);
		pthread_mutex_unlock(&miners_lock);
		++x;
		fclose(lf);
		sleep(LOG_REFRESH_TIME);
	}
	iDoneFlag = 1;
	return 0;
}

int have_subs(int n){
	int ret = 0;
	
	pthread_mutex_lock(&s_lock);
	if(n <= (iSubFilled - iSubIndex))
		ret = 1;
	pthread_mutex_unlock(&s_lock);
	
	return ret;
}

void handle_vars(int w, int p, int d, int m_i, int m_s){
	pthread_mutex_lock(&v_lock);
	iWorking += w;
	iPending += p;
	iDone += d;
	pthread_mutex_unlock(&v_lock);
	
	pthread_mutex_lock(&miners_lock);
	miners[m_i].stat = m_s;
	pthread_mutex_unlock(&miners_lock);
}

int tryadd(const char *sWord, int amount){
	int where, returnval;
	pthread_mutex_lock(&w_lock);

	if(is_exist(sWord, &where) == 0){
		strcpy(wordArr[iWordArrIndex].word, sWord);
		wordArr[iWordArrIndex++].amount = amount;
		if(iWordArrIndex == iWordArrSize-1){
			iWordArrSize *= 2;
			wordArr = (word_t *) realloc(wordArr, iWordArrSize * sizeof(word_t));
		}
		returnval = 10;
	}
	else{
		wordArr[where].amount += amount;
		returnval = 1;
	}

	pthread_mutex_unlock(&w_lock);
	return returnval;
}

int is_exist(const char *sWord, int *where){
	int i, result = 0;
	for(i = 0; i < iWordArrIndex; ++i)
		if(strcmp(wordArr[i].word, sWord) == 0){
			result = 1;
			*where = i;
		}
	return result;
}

void sig_handler(int sig){
  int i;
	write(1, "Caught signal 11\n", 17);
	signal(SIGINT, sig_handler);
	
	iLoggerFlag = 1;
	if(pthread_join(pthLogger, NULL)){
		  fprintf(stderr, "Cannot join thread, logger\n");
		  quit(1);
	}
	
	for(i = 0; i < iMinerArrIndex; ++i)
	  kill(miners[i].pid, SIGINT);
	
	if(iAliveThreads == 0)
	  wait_threads();
	quit(1);
}

void get_subs(const char *path){
	struct dirent *pDirent;	/* dirent pointer to walk around in a directory */
	char sSub[MAX];
	static DIR *pDir;
	if((pDir = opendir(path)) == NULL){
		fprintf(stderr, "folder NULL: %s\n", path); quit(1);
	}
	
	while((pDirent = readdir(pDir)) != NULL){
		sprintf(sSub, "%s/%s", path, pDirent->d_name);
		
		if(dont_look(pDirent->d_name))
			continue;	/* skip root, child and hidden files. */

		pthread_mutex_lock(&s_lock);
		strcpy(subs[iSubFilled++].sub, sSub);
		if(iSubFilled == iSubSize - 1){
			iSubSize *= 2;
			subs = (sub_t *) realloc(subs, iSubSize * sizeof(sub_t));
		}
		pthread_mutex_unlock(&s_lock);
	}
	
	closedir(pDir);
}
