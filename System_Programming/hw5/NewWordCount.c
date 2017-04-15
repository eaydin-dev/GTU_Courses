
#include "NewWordCount.h"

static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

static word_t *wordArr;
static int wordArrIndex = 0;
static int wordArrSize = MAX;

static directory_t *dirArr;
static int dirArrIndex = 0;
static int dirArrSize = MAX/2;

int countWordsInDirectory(const char *sDirectory, int *iNumOfDirs, int *iNumOfFiles, int *iNumOfUniques){
	int iTotal;
	DIR *pDir;

	wordArr = (word_t *) malloc(wordArrSize * sizeof(word_t));
	dirArr = (directory_t *) malloc(dirArrSize * sizeof(directory_t));

	if((pDir = opendir(sDirectory)) == NULL)
		return -1;
	closedir(pDir);

	iTotal = total_words(sDirectory, iNumOfFiles);

	*iNumOfDirs = dirArrIndex;
	*iNumOfUniques = wordArrIndex;

	create_word_log();
	create_directory_log();

	free(wordArr);
	free(dirArr);
	return iTotal;
}

int total_words(const char *sDirectory, int *iFiles) {
	struct dirent *pDirent;	/* dirent pointer to walk around in a directory */
	DIR *pDir;				/* directory pointer */

	char sSub[PATH_MAX];	/* a subdirectory or a file path */
	char ** subFiles;
	char ** subDirs;
	char sName[MAX];

	int iNumOfFiles = 0;
	int iNumOfDirs = 0;
	int i = 0, j = 0; 		/* loops */
	int iResult = 0;
	int sumOfWords = 0;
	clock_t tstart, tend;

	wordcount_t *temp;
	wordcount_t *file_args;

	tstart = clock();

	if((pDir = opendir(sDirectory)) == NULL)
		return 0;	/* there is no words if directory cannot open */

	while((pDirent = readdir(pDir)) != NULL){	
		if(dont_look(pDirent->d_name))
			continue;	/* skip root, child and hidden files. */

		sprintf(sSub, "%s/%s", sDirectory, pDirent->d_name);

		if(is_directory(sSub) == 1)
			++iNumOfDirs;
		else
			++iNumOfFiles;
	}

	*iFiles += iNumOfFiles;
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

	for(i = 0; i < iNumOfDirs; ++i)	/* finish sub directories first */
		iResult += total_words(subDirs[i], iFiles);
	

	for(i = 0; i < iNumOfFiles; ++i){	
		if((file_args[i].filedes = open(subFiles[i], O_RDONLY)) < 0){

			fprintf(stderr, "A file cannot open. %s ... \n", subFiles[i]);
			exit(1);
		}
		if(pthread_create(&file_args[i].tid, NULL, scan_file, &file_args[i])){
			fprintf(stderr, "Cannot create threat, at iteration %d\n", i);
			exit(1);
		}
	}

	for(i = 0; i < iNumOfFiles; ++i){
		if(pthread_join(file_args[i].tid, (void **)&temp)){
			fprintf(stderr, "Cannot join thread, at iteration %d\n", i);
			exit(1);
		}
		sumOfWords += temp->iNumOfWords;
		iResult += temp->iNumOfWholeWords;
	}

	dirArr[dirArrIndex].iWordAmount = sumOfWords;

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
	getName(sDirectory, sName);
	strcpy(dirArr[dirArrIndex].directory, sName);
	dirArr[dirArrIndex++].elapsedTime = (double) (tend - tstart) / CLOCKS_PER_SEC * 1000;
	return iResult;
}

int getName(const char *sPath, char *sName){
	int i, j=0, k = strlen(sPath)-1, l;

	for(i = k; i > 0; --i)
		if(sPath[i] != '/')
			++j;
		else
			break;

	l = j;
	for(i = k; i > 0; --i)
		if(sPath[i] != '/')
			sName[--j] = sPath[i];
		else 
			break;

	sName[l] = '\0';
	return l;
}

int dont_look(const char *sPath){
	return (!strcmp(sPath, ".") || !strcmp(sPath, "..") || sPath[strlen(sPath) -1] == '~' || !strcmp(sPath, WORD_LOG_FILE));
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
		exit(1);
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

	check_allocation();
	return info;
}

int is_punctuation(char ch){
	return ch == '.' || ch == ',' || ch == ':' || ch == ';' || ch == '!' || ch == '?';
}

void check_allocation(){
	pthread_mutex_lock(&lock);

	if(wordArrIndex == wordArrSize-1){
		wordArrSize *= 2;
		wordArr = (word_t *) realloc(wordArr, wordArrSize * sizeof(word_t));
	}

	if(dirArrIndex == dirArrSize - 1){
		dirArrSize *= 2;
		dirArr = (directory_t *) realloc(dirArr, dirArrSize * sizeof(directory_t));
	}

	pthread_mutex_unlock(&lock);
}

int create_word_log(){
	int i;
	FILE *fReport;
	fReport = fopen(WORD_LOG_FILE, "w");

	for(i = 0; i < wordArrIndex; ++i)
		fprintf(fReport, "%3d. %20s \t %2d\n", i+1, wordArr[i].word, wordArr[i].amount);	
	
	fclose(fReport);
	return wordArrIndex;
}

int create_directory_log(){
	int i;
	FILE *fReport;
	fReport = fopen(DIR_LOG_FILE, "w");

	fprintf(fReport, "SCANNED DIRECTORIES AND THEIR ELAPSED TIME\n\n");

	for(i = 0; i < dirArrIndex; ++i)
		fprintf(fReport, "%3d. %20s -- Elapsed time: %f ms\n", i+1, dirArr[i].directory, dirArr[i].elapsedTime);	

	fclose(fReport);
	return dirArrIndex;
}

int is_exist(const char *sWord, int *where){
	int i, result = 0;

	for(i = 0; i < wordArrIndex; ++i)
		if(strcmp(wordArr[i].word, sWord) == 0){
			result = 1;
			*where = i;
		}

	return result;
}

int tryadd(const char *sWord){
	int where, returnval;

	pthread_mutex_lock(&lock);

	if(is_exist(sWord, &where) == 0){
		strcpy(wordArr[wordArrIndex].word, sWord);
		wordArr[wordArrIndex++].amount = 1;
		returnval = 1;
	}
	else{
		wordArr[where].amount++;
		returnval = 0;
	}

	pthread_mutex_unlock(&lock);
	return returnval;
}

int is_directory(const char *sPath){

	struct stat statbuf;
	stat(sPath, &statbuf);	/* get info	*/

	/* check info */
	if(S_ISDIR(statbuf.st_mode))
		return 1;

	return 0;
}