
#include "Miner.h"

static sub_t *subs = NULL;

static word_t *wordArr = NULL;
static int iWordArrIndex = 0;
static int iWordArrSize = 10;

static int iNumOfEntities = 0;

static pthread_mutex_t t_lock = PTHREAD_MUTEX_INITIALIZER;
static sem_t *semNamed = NULL;

static int iShmId;
static struct cl_info_t *clShm = NULL;

static char sSemName[MAX];
static char sLogName[MAX];
static FILE *fpLog = NULL;

static pthread_t *pthArr = NULL;
static int iPthArrIndex = 0;

static int iWorkedFiles = 0;


int main(int argc, char **argv){
	int i, j = 0, flag = 0;
	int sock, words = 0, first = 0, k = 0;
    struct sockaddr_in server;
    struct pre_info pre;
	pthread_attr_t attr;

    time_t t;
    t = time(NULL);
	
	if(argc != 2){
    	fprintf(stderr, "Usage: %s [# entities]\n", argv[0]);
    	exit(1);
    }
    iNumOfEntities = atoi(argv[1]);
	
	signal(SIGINT, sig_handler);
	pthread_attr_init(&attr);

    init();

	sprintf(sSemName, "sem.%d", (int) getpid());
    if (get_named_sem(sSemName, &semNamed, 1) == -1) {
		perror("Failed to create named semaphore"); quit(1);
	}
	
    fprintf(fpLog, "miner started at %s\n", ctime(&t));
	strcpy(pre.semname, sSemName);
	
    pre.n = iNumOfEntities;
    pre.shmId = iShmId;
    pre.pid = getpid();
  
    sock = socket(AF_INET , SOCK_STREAM , 0);
    if (sock == -1){
        perror("couldn't create socket"); exit(1);
    }

    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons(S_PORT);

    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0){
        perror("connect failed. Error"); exit(1);
    }

	send(sock, &pre, sizeof(struct pre_info), 0);

	change_status(IGNORE, FIRST);
	i = 0;
	flag = 0;
  
	fprintf(fpLog, "pending entities... mine has not sent entities yet, so this miner will wait until then.\n");
	while(1){
		while(sem_wait(semNamed) == -1)
		if(errno != EINTR){
			perror("Failed to lock semaphore"); quit(1);
		}
		
		if(clShm->status == SENT){
			strcpy(subs[i++].sub, clShm->info);
			clShm->status = RECEIVED;
		}
		else if(clShm->status == FILLED){
			t = time(NULL);
			fprintf(fpLog, "end time of wait: %s\n", ctime(&t));
			flag = 1;
		}
		if(sem_post(semNamed) == -1){
			perror("Failed to unlock semaphore"); quit(1);
		}

		if(flag)
			break;	
	}
	
	for(i = 0; i < pre.n; ++i){
		if(is_directory(subs[i].sub) != 1){
			if(pthread_create(&pthArr[iPthArrIndex++], &attr, scan_file, (void *)subs[i].sub)){
				perror("Cannot create thread"); quit(1);
			}
		}
		else{
			pthread_mutex_lock(&t_lock);
			strcpy(wordArr[iWordArrIndex++].word, subs[i].sub);
			if(iWordArrIndex == iWordArrSize-1){
				iWordArrSize *= 2;
				wordArr = (word_t *) realloc(wordArr, iWordArrSize * sizeof(word_t));
			}
			pthread_mutex_unlock(&t_lock);
		}	
	}
  
	for(i = 0; i < iPthArrIndex; ++i)
	  if(pthread_join(pthArr[i], NULL)){
			fprintf(stderr, "Cannot join thread, at iteration %d\n", i);
			quit(1);
		}

	flag = 0;
	i = 0, j = 0;
	while(1){
		while(sem_wait(semNamed) == -1)
		if(errno != EINTR){
			perror("Failed to lock semaphore"); quit(1);
		}
		
		if(clShm->status == RECEIVED || clShm->status == FILLED){
			if(is_directory(wordArr[i].word) == 1){
				strcpy(clShm->info, wordArr[i++].word);
				clShm->type = DIRECTORY;
				clShm->status = SENT;
			}			

			else{
				++k;
				clShm->gold += (wordArr[i].amount - 1);
				if(k > 1)
					wordArr[j].gold = clShm->newword + wordArr[j].amount - 1;

				j = i;
				if(k == 1)
					first = i;
		
				++words;
				strcpy(clShm->info, wordArr[i].word);
				clShm->amount = wordArr[i++].amount;
				clShm->type = WORD;
				clShm->status = SENT;
			}
		}

		if(sem_post(semNamed) == -1){
			perror("Failed to unlock semaphore"); quit(1);
		}	

		if(i == iWordArrIndex)
			break;
	}
	
	while(change_status(RECEIVED, DONE) == 0) ;
	
	while(sem_wait(semNamed) == -1)
	if(errno != EINTR){
		perror("Failed to lock semaphore"); quit(1);
	}
	
	pthread_mutex_lock(&t_lock);
	wordArr[first].gold = clShm->first + wordArr[first].amount - 1;
	wordArr[j].gold = clShm->newword +  wordArr[j].amount - 1;
	pthread_mutex_unlock(&t_lock);

	if(sem_post(semNamed) == -1){
		perror("Failed to unlock semaphore"); quit(1);
	}
	
	close(sock);

	i = 0, j = 0, flag = 0;
	while(1){
		pthread_mutex_lock(&t_lock);

		if(is_directory(wordArr[i].word) == 0){
			fprintf(fpLog, "%20s - %3d times - %d golds\n", wordArr[i].word, wordArr[i].amount, wordArr[i].gold);
			++j;
		}

		++i;
		if(j == (words+1) || i == iWordArrIndex)
			flag = 1;

		pthread_mutex_unlock(&t_lock);

		if(flag)
			break;
	}

	t = time(NULL);
	fprintf(fpLog, "\nstopped at: %s\n", ctime(&t));
	fprintf(fpLog, "%d golds earned and %d files have been searched.\n", clShm->gold, iWorkedFiles);
	
	clean();
	return 0;
}

void clean(){
	if(subs != NULL)
		free(subs);
	if(wordArr != NULL)
		free(wordArr);
	if(pthArr != NULL)
		free(pthArr);
	if(semNamed != NULL)
		destroynamed(sSemName, semNamed);
	if(clShm != NULL){
		shmdt(clShm);
		shmctl(iShmId, IPC_RMID, 0);
	}
	if(fpLog != NULL)
		fclose(fpLog);
}

void init(){
	sprintf(sLogName, "%d.log", (int)getpid());
	fpLog = fopen(sLogName, "a");

	iShmId = shmget(IPC_PRIVATE, sizeof(struct cl_info_t), 0666);
    if (iShmId < 0) {
        perror("shmget error (client) ***\n"); exit(1);
    }

	clShm = (struct cl_info_t *)shmat(iShmId, NULL, 0);
	if (clShm == (void *)-1){
		perror("Can't attach in miner"); exit(1);	
	}
	clShm->gold = 0;

	subs = (sub_t *) malloc(iNumOfEntities * sizeof(sub_t));
	pthArr = (pthread_t *) calloc(MAX/4, sizeof(pthread_t));
    wordArr = (word_t *) malloc(iWordArrSize * sizeof(word_t));
}

void quit(int val){
	clean();
	exit(val);
}

void sig_handler(int sig){
	write(1, "Caught signal 11\n", 17);
	fprintf(fpLog, "got <CTRL-C> signal...\n");
	signal(SIGINT, sig_handler);

	quit(1);
}

int change_status(int cond, int stat){
	int ret = 0;

	while(sem_wait(semNamed) == -1)
	if(errno != EINTR){
		perror("Failed to lock semaphore"); quit(1);
	}
	if(cond == IGNORE)
		clShm->status = stat;
	else
		if(clShm->status == cond){
			ret = 1;
			clShm->status = stat;	
		}

	if(sem_post(semNamed) == -1){
		perror("Failed to unlock semaphore"); quit(1);
	}

	return ret;
}

void * scan_file(void * thread_info){
	char temp;
	char sWord[MAX];
	int i = 0;
	char * filename = (char *) thread_info;
	FILE *fCurrentFile;
	
	sleep(3);
	fCurrentFile = fopen(filename, "r");
	if(fCurrentFile != NULL){
	  ++iWorkedFiles;
	  fprintf(fpLog, "thread %d started to work on: %s\n", (int)pthread_self(), filename);
	  while((temp = fgetc(fCurrentFile)) != EOF) {
		  if(temp != ' ' && temp != '\n' && !is_punctuation(temp)){
			  if((temp >= 65 && temp <= 90) || (temp >= 97 && temp <= 122))
				  sWord[i++] = temp;
		  }		
		  else{
			  sWord[i] = '\0';
			  if(i > 1){
				  tryadd(sWord);
			  }
			  i = 0;
		  }
	  }
	  fclose(fCurrentFile);
	}

	return 0;
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

int tryadd(const char *sWord){
	int where, returnval;

	pthread_mutex_lock(&t_lock);

	if(is_exist(sWord, &where) == 0){
		strcpy(wordArr[iWordArrIndex].word, sWord);
		wordArr[iWordArrIndex++].amount = 1;
		if(iWordArrIndex == iWordArrSize-1){
			iWordArrSize *= 2;
			wordArr = (word_t *) realloc(wordArr, iWordArrSize * sizeof(word_t));
		}
		returnval = 1;
	}
	else{
		wordArr[where].amount++;
		returnval = 0;
	}

	pthread_mutex_unlock(&t_lock);
	return returnval;
}

int file_exist (const char *sPath){
    char s[200];
    sprintf(s, "test -e %s", sPath);
    if (system(s) == 0)
        return 1;
    else
        return 0;
}

int is_directory(const char *sPath){

	struct stat statbuf;
	stat(sPath, &statbuf);	/* get info	*/

	/* check info */
	if(S_ISDIR(statbuf.st_mode)){
		if(file_exist(sPath))
			return 1;
	}

	return 0;
}
