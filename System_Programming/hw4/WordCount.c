
#include "WordCount.h"

int countWordsInDirectory(const char *sDirectory, int *iNumOfDirs, int *iNumOfFiles){
	int iTotal;
	nums_t nums;
	DIR *pDir;

	if((pDir = opendir(sDirectory)) == NULL)
		return -1;

	closedir(pDir);

	nums.files = 0;
	nums.directories = 0;

	iTotal = total_words(sDirectory, &nums);

	*iNumOfDirs = nums.directories;
	*iNumOfFiles = nums.files;
	return iTotal;
}

int total_words(const char *sDirectory, nums_t * nums) {
	struct dirent *pDirent;	/* dirent pointer to walk around in a directory */
	DIR *pDir;				/* directory pointer */

	char sSub[MAX];			/* a subdirectory or a file path */
	char ** subFiles;
	char ** subDirs;

	int iNumOfFiles = 0;
	int iNumOfDirs = 0;
	int i = 0, j = 0; 		/* loops */
	int iResult = 0;
	int *iTemp;

	wordcount_t *file_args;

	if((pDir = opendir(sDirectory)) == NULL)
		return 0;	/* there is no words if directory cannot open */

	while((pDirent = readdir(pDir)) != NULL){
		sprintf(sSub, "%s/%s", sDirectory, pDirent->d_name);

		if(!strcmp(pDirent->d_name, ".") || !strcmp(pDirent->d_name, "..") || pDirent->d_name[strlen(pDirent->d_name) -1] == '~')
			continue;	/* skip root, child and hidden files. */

		if(is_directory(sSub) == 1)
			++iNumOfDirs;

		else
			++iNumOfFiles;
	}
	
	nums->files += iNumOfFiles;
	nums->directories += iNumOfDirs;

	/* allocate memory to keep subs */
	file_args = (wordcount_t *) malloc(iNumOfFiles * sizeof(wordcount_t));
	subFiles = (char **) malloc(iNumOfFiles * sizeof(char*));
	
	for(i = 0; i < iNumOfFiles; ++i)
		subFiles[i] = (char *) malloc(MAX * sizeof(char));

	subDirs = (char **) malloc(iNumOfDirs * sizeof(char*));
	for(i = 0; i < iNumOfDirs; ++i)
		subDirs[i] = (char *) malloc(MAX * sizeof(char));


	rewinddir(pDir);	/* rewind directory to get subs */
	i = 0;
	while((pDirent = readdir(pDir)) != NULL){
		sprintf(sSub, "%s/%s", sDirectory, pDirent->d_name);

		if(!strcmp(pDirent->d_name, ".") || !strcmp(pDirent->d_name, "..") || pDirent->d_name[strlen(pDirent->d_name) -1] == '~')
			continue;	/* skip root, child and hidden files. */

		if(is_directory(sSub) == 1)
			strcpy(subDirs[i++], sSub);
		else			
			strcpy(subFiles[j++], sSub);
	}

	for(i = 0; i < iNumOfDirs; ++i)
		iResult += total_words(subDirs[i], nums);
	

	for(i = 0; i < iNumOfFiles; ++i){	
		if((file_args[i].args[0] = open(subFiles[i], O_RDONLY)) < 0){
			fprintf(stderr, "A file cannot open.\n");
			exit(1);
		}

		if(pthread_create(&file_args[i].tid, NULL, scan_file, file_args[i].args)){
			fprintf(stderr, "Cannot create threat, at iteration %d\n", i);
			exit(1);
		}
	}

	for(i = 0; i < iNumOfFiles; ++i){
		if(pthread_join(file_args[i].tid, (void **)&iTemp)){
			fprintf(stderr, "Cannot read from thread, at iteration %d\n", i);
			exit(1);
		}
		if(iTemp == NULL){
			fprintf(stderr, "Thread %d failed to return.\n", i);
			exit(1);
		}
		iResult += *iTemp;
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
	return iResult;
}

void * scan_file(void * file_and_return){
	int iTemp = 0;
	int iResult = 0;
	char *sDelim = " ., ':;!";
	char *sToken;
	char sLine[MAX];

	int *args;
	FILE * fCurrentFile;

	args = (int *) file_and_return;
	fCurrentFile = fdopen(args[0], "r");
	if(fCurrentFile == NULL){
		fprintf(stderr, "Cannot open file with fdopen\n");
		exit(1);
	}

	while(fgets(sLine, sizeof(sLine), fCurrentFile)) {
		sToken = strtok(sLine, sDelim);
		while(sToken != NULL){
			iTemp = sscanf(sToken, "%d", &iTemp);
			if(iTemp == 0)/* check if it's a number or not	*/
				++iResult;
							
			sToken = strtok(NULL, sDelim);
		}
	}
	close(args[0]);
	fclose(fCurrentFile);
	args[1] = iResult;

	return args + 1;
}

int is_directory(const char *sPath){

	struct stat statbuf;
	stat(sPath, &statbuf);	/* get info	*/

	/* check info */
	if(S_ISDIR(statbuf.st_mode))
		return 1;

	return 0;
}
