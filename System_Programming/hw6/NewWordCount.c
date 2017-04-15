
#include "NewWordCount.h"

static sem_t *semlock;

static word_t *wordArr;
static int *wordArrIndex;
static directory_t *dirArr;
static int *dirArrIndex;
static int *words_and_files;

static int sid, sid2, sid3, sid4, sid5;

void get_sm();
void release_sm();

int countWordsInDirectory(const char *sDirectory, int *iNumOfDirs, int *iNumOfFiles, int *iNumOfUniques){
	int iTotal;
	DIR *pDir;

	if((pDir = opendir(sDirectory)) == NULL)
		return -1;
	closedir(pDir);

	get_sm();

	total_words(sDirectory);

	if(get_named_sem(&semlock, 1) == -1){
		perror("Failed to create named semaphore");
		quit(1);
	}

	while(sem_wait(semlock) == -1)
		if(errno != EINTR){
			perror("Failed to lock semaphore");
			quit(1);
		}

	*iNumOfDirs = *dirArrIndex;
	*iNumOfUniques = *wordArrIndex;

	iTotal = words_and_files[0];
	*iNumOfFiles = words_and_files[1];

	if(sem_post(semlock) == -1){
		perror("Failed to unlock semaphore");
		quit(1);
	}

	create_word_log();
	create_directory_log();

    sem_close(semlock);
    release_sm();
    sem_destroy (semlock);
	sem_unlink(SEMNAME);
	return iTotal;
}

void total_words(const char *sDirectory) {
	struct dirent *pDirent;	/* dirent pointer to walk around in a directory */
	DIR *pDir;				/* directory pointer */

	char sSub[PATH_MAX];	/* a subdirectory or a file path */
	char ** subFiles;
	char ** subDirs;

	int iNumOfFiles = 0;
	int iNumOfDirs = 0;
	int i = 0, j = 0; 		/* loops */
	int iResult = 0;
	int sumOfWords = 0;
	int iStatus = 0;
	pid_t pid;
	clock_t tstart, tend;

	wordcount_t *temp;
	wordcount_t *file_args;

	tstart = clock();

	if((pDir = opendir(sDirectory)) == NULL)
		return ;	/* there is no words if directory cannot open */

	while((pDirent = readdir(pDir)) != NULL){	
		if(dont_look(pDirent->d_name))
			continue;	/* skip root, child and hidden files. */

		sprintf(sSub, "%s/%s", sDirectory, pDirent->d_name);

		if(is_directory(sSub) == 1)
			++iNumOfDirs;
		else
			++iNumOfFiles;
	}

	/* allocate memory to keep subs */
	file_args = (wordcount_t *) malloc(iNumOfFiles * sizeof(wordcount_t));

	subFiles = (char **) malloc(iNumOfFiles * sizeof(char*));
	for(i = 0; i < iNumOfFiles; ++i)
		subFiles[i] = (char *) malloc(PATH_MAX * sizeof(char));

	subDirs = (char **) malloc(iNumOfDirs * sizeof(char*));
	for(i = 0; i < iNumOfDirs; ++i)
		subDirs[i] = (char *) malloc(PATH_MAX * sizeof(char));


	rewinddir(pDir);	/* rewind directory to get subs */
	i = 0, j = 0;
	while((pDirent = readdir(pDir)) != NULL){
		sprintf(sSub, "%s/%s", sDirectory, pDirent->d_name);

		if(dont_look(pDirent->d_name))
			continue;	/* skip root, child and hidden files. */

		if(is_directory(sSub) == 1)
			strcpy(subDirs[i++], sSub);
		else			
			strcpy(subFiles[j++], sSub);
	}

	for(i = 0; i < iNumOfDirs; ++i){	/* finish sub directories first */
		pid = fork();
		if(pid == 0){
			total_words(subDirs[i]);
			exit(0);
		}
	}

	if(get_named_sem(&semlock, 1) == -1){
		perror("Failed to create named semaphore");
		quit(1);
	}

	for(i = 0; i < iNumOfFiles; ++i){	
		if((file_args[i].filedes = open(subFiles[i], O_RDONLY)) < 0){
			fprintf(stderr, "A file cannot open. %s ... \n", subFiles[i]);
			quit(1);
		}
		if(pthread_create(&file_args[i].tid, NULL, scan_file, &file_args[i])){
			fprintf(stderr, "Cannot create threat, at iteration %d\n", i);
			quit(1);
		}
	}

	for(i = 0; i < iNumOfFiles; ++i){
		if(pthread_join(file_args[i].tid, (void **)&temp)){
			fprintf(stderr, "Cannot join thread, at iteration %d\n", i);
			quit(1);
		}
		sumOfWords += temp->iNumOfWords;
		iResult += temp->iNumOfWholeWords;
	}

	/* free everywhere	*/
	free(file_args);

	for(i = 0; i < iNumOfFiles; ++i)
		free(subFiles[i]);
	free(subFiles);

	for(i = 0; i < iNumOfDirs; ++i)
		free(subDirs[i]);
	free(subDirs);

	closedir(pDir);

	tend = clock();

	while(sem_wait(semlock) == -1)
		if(errno != EINTR){
			perror("Failed to lock semaphore");
			quit(1);
		}

	dirArr[(*dirArrIndex)].iWordAmount = sumOfWords;
	strcpy(dirArr[(*dirArrIndex)].directory, sDirectory);
	dirArr[(*dirArrIndex)++].elapsedTime = (double) (tend - tstart) / CLOCKS_PER_SEC * 1000;

	words_and_files[0] += iResult;
	words_and_files[1] += iNumOfFiles;

	if(sem_post(semlock) == -1){
		perror("Failed to unlock semaphore");
		quit(1);
	}

	sem_close(semlock);

	while(wait(&iStatus) > 0) 	/* wait all childs */
		if(iStatus != 0){		/* all childs should exit with 0 */
			perror("Some errors on child processes.");
			quit(1);
		}
}

int dont_look(const char *sPath){
	return (!strcmp(sPath, ".") || !strcmp(sPath, "..") || sPath[strlen(sPath) -1] == '~' || !strcmp(sPath, WORD_LOG_FILE) || !strcmp(sPath, DIR_LOG_FILE));
}

void * scan_file(void * thread_info){
	char temp;
	char sWord[MAX];
	int iUniqueAmount = 0;
	int iWholeAmount = 0;
	int i = 0;
	
	wordcount_t *info;
	FILE *fCurrentFile;

	info = (wordcount_t *) thread_info;
	fCurrentFile = fdopen(info->filedes, "r");
	if(fCurrentFile == NULL){
		fprintf(stderr, "Cannot open file with fdopen\n");
		quit(1);
	}

	while((temp = fgetc(fCurrentFile)) != EOF) {
		if(temp != ' ' && temp != '\n' && !is_punctuation(temp)){
			if(temp > 57 || temp < 48)
				sWord[i++] = temp;
		}		
		else{
			sWord[i] = '\0';
			if(i > 1){
				if(tryadd(sWord))
					++iUniqueAmount;
				++iWholeAmount;
			}
			i = 0;
		}
	}
	fclose(fCurrentFile);

	info->tid = pthread_self();
	info->iNumOfWords = iUniqueAmount;
	info->iNumOfWholeWords = iWholeAmount;
	return info;
}

int is_punctuation(char ch){
	return ch == '.' || ch == ',' || ch == ':' || ch == ';' || ch == '!' || ch == '?';
}

void create_word_log(){
	int i;
	FILE *fReport;
	fReport = fopen(WORD_LOG_FILE, "w");

	while(sem_wait(semlock) == -1)
		if(errno != EINTR){
			perror("Failed to lock semaphore");
			exit(1);
		}

	for(i = 0; i < *wordArrIndex; ++i)
		fprintf(fReport, "%3d. %25s \t %3d\n", i+1, wordArr[i].word, wordArr[i].amount);	
	
	if(sem_post(semlock) == -1){
		perror("Failed to unlock semaphore");
		exit(1);
	}

	fclose(fReport);
}

void create_directory_log(){
	int i;
	FILE *fReport;
	fReport = fopen(DIR_LOG_FILE, "w");

	fprintf(fReport, "SCANNED DIRECTORIES AND THEIR ELAPSED TIME\n\n");

	while(sem_wait(semlock) == -1)
		if(errno != EINTR){
			perror("Failed to lock semaphore");
			exit(1);
		}

	for(i = 0; i < *dirArrIndex; ++i)
		fprintf(fReport, "%3d. %s --- Elapsed time: %f ms\n", i+1, dirArr[i].directory, dirArr[i].elapsedTime);	

	if(sem_post(semlock) == -1){
		perror("Failed to unlock semaphore");
		exit(1);
	}

	fclose(fReport);
}

int is_exist(const char *sWord, int *where){
	int i, result = 0;

	for(i = 0; i < *wordArrIndex; ++i)
		if(strcmp(wordArr[i].word, sWord) == 0){
			result = 1;
			*where = i;
		}

	return result;
}

int tryadd(const char *sWord){
	int where, returnval;

	while(sem_wait(semlock) == -1)
		if(errno != EINTR){
			perror("Failed to lock semaphore");
			exit(1);
		}

	if(is_exist(sWord, &where) == 0){
		strcpy(wordArr[(*wordArrIndex)].word, sWord);
		wordArr[(*wordArrIndex)++].amount = 1;
		returnval = 1;
	}
	else{
		wordArr[where].amount++;
		returnval = 0;
	}

	if(sem_post(semlock) == -1){
		perror("Failed to unlock semaphore");
		exit(1);
	}

	return returnval;
}

void quit(int val){
	release_sm();
	exit(val);
}

void get_sm(){

	sid5 = shmget (IPC_PRIVATE, 2 * sizeof(int), 0644 | IPC_CREAT);
    if (sid5 < 0){                           /* shared memory error check */
        perror ("shmget\n");
        exit (1);
    }
    words_and_files = (int *) shmat(sid5, NULL, 0);
    words_and_files[0] = 0; words_and_files[1] = 0;


    sid = shmget (IPC_PRIVATE, MAX * sizeof(word_t), 0644 | IPC_CREAT);
    if (sid < 0){                           /* shared memory error check */
        perror ("shmget\n");
        quit(1);
    }
    wordArr = (word_t *) shmat(sid, NULL, 0);

    sid3 = shmget (IPC_PRIVATE, MAX/2 * sizeof(directory_t), 0644 | IPC_CREAT);
    if (sid3 < 0){                           /* shared memory error check */
        perror ("shmget\n");
        quit(1);
    }
    dirArr = (directory_t *) shmat(sid3, NULL, 0);


    sid2 = shmget (IPC_PRIVATE, sizeof (int), 0644 | IPC_CREAT);
    if (sid2 < 0){                           /* shared memory error check */
        perror ("shmget\n");
        quit(1);
    }

    wordArrIndex = (int *) shmat (sid2, NULL, 0);   /* attach p to shared memory */
    *wordArrIndex = 0;

    sid4 = shmget (IPC_PRIVATE, sizeof (int), 0644 | IPC_CREAT);
    if (sid4 < 0){                           /* shared memory error check */
        perror ("shmget\n");
        quit(1);
    }

    dirArrIndex = (int *) shmat (sid4, NULL, 0);   /* attach p to shared memory */
    *dirArrIndex = 0;
}

void release_sm(){
	shmdt (wordArr);
	shmdt (dirArr);
	shmdt(wordArrIndex);
	shmdt(dirArrIndex);
	shmdt(words_and_files);

    shmctl (sid, IPC_RMID, 0);
    shmctl (sid2, IPC_RMID, 0);
    shmctl (sid3, IPC_RMID, 0);
    shmctl (sid4, IPC_RMID, 0);
    shmctl (sid5, IPC_RMID, 0);
}

int get_named_sem(sem_t **sem, int val){
	while(((*sem = sem_open(SEMNAME, FLAGS, PERMS, val)) == SEM_FAILED) && (errno == EINTR))
		;

	if(*sem != SEM_FAILED)
		return 0;
	if(errno != EEXIST)
		return -1;
	while(((*sem = sem_open(SEMNAME, 0)) == SEM_FAILED) && (errno == EINTR))
		;

	if(*sem != SEM_FAILED)
		return 0;
	return -1;
}

int is_directory(const char *sPath){

	struct stat statbuf;
	stat(sPath, &statbuf);	/* get info	*/

	/* check info */
	if(S_ISDIR(statbuf.st_mode))
		return 1;

	return 0;
}