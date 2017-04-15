
#include "NewWordCount.h"

void recursive_directory(const char *sDir, const char *sWordFile, int *pipeArr) {
	struct dirent *pDirent;	/* dirent pointer to walk around in a directory */
	DIR *pDir;				/* directory pointer */
	FILE *fCurrentFile;		/* file that will be read */
	pid_t pid;				/* pid_t to use fork function */

	char sSub[MAX];			/* a subdirectory or a file path */
	char ** subs;			/* keeps all subs */
	int iNumOfSubs = 0;
	int iNumOfFiles = 0;
	int iNumOfDirs = 0;
	int i = 0; 				/* loops */
	int iStatus; 			/* exit status of childs */

	if((pDir = opendir(sDir)) == NULL)
		return ;	/* there is no words if directory cannot open */

	while((pDirent = readdir(pDir)) != NULL){
		if(!strcmp(pDirent->d_name, ".") || !strcmp(pDirent->d_name, "..") || !strcmp(pDirent->d_name, "Words.log") || pDirent->d_name[strlen(pDirent->d_name) -1] == '~')
			continue;	/* skip root, child and hidden files. */
		++iNumOfSubs;
	}
	
	/* allocate memory to keep subs */
	subs = (char **) malloc(iNumOfSubs * sizeof(char*));
	for(i = 0; i < iNumOfSubs; ++i)
		subs[i] = (char *) malloc(MAX * sizeof(char));

	rewinddir(pDir);	/* rewind directory to get subs */
	i = 0;
	while((pDirent = readdir(pDir)) != NULL){
		if(!strcmp(pDirent->d_name, ".") || !strcmp(pDirent->d_name, "..") || !strcmp(pDirent->d_name, "Words.log") || pDirent->d_name[strlen(pDirent->d_name) -1] == '~')
			continue;	/* skip root, child and hidden files. */
		strcpy(subs[i++], pDirent->d_name);
	}

	for(i = 0; i < iNumOfSubs; ++i){	/* fork for all subs */
		pid = fork();
		if(pid == 0){	/* child should count words */
			sprintf(sSub, "%s/%s", sDir, subs[i]);

			if(is_directory(sSub) == 1){
				scan_nums(&iNumOfDirs, &iNumOfFiles, pipeArr);
				++iNumOfDirs;
				write_nums(iNumOfDirs, iNumOfFiles, pipeArr);
				recursive_directory(sSub, sWordFile, pipeArr);
			}
			else {
				fCurrentFile = fopen(sSub, "r");
				if(fCurrentFile != NULL){
					scan_file(fCurrentFile, sWordFile);

					scan_nums(&iNumOfDirs, &iNumOfFiles, pipeArr);
					++iNumOfFiles;
					write_nums(iNumOfDirs, iNumOfFiles, pipeArr);
				}
				fclose(fCurrentFile);
			}
			exit(0);		
		}

	}

	/* free everywhere	*/
	for(i = 0; i < iNumOfSubs; ++i)
		free(subs[i]);
	free(subs);

	closedir(pDir);

	while(wait(&iStatus) > 0){ 	/* wait all childs */
		if(iStatus != 0){		/* all childs should exit with 0 */
			fprintf(stderr, "Some errors on child processes.\n");
			exit(1);
		}
	}	
	return ;
}

void scan_nums(int *iDir, int *iFile, int *pipeArr){
	if(read(pipeArr[0], iDir, sizeof(*iDir)) < 0 || read(pipeArr[0], iFile, sizeof(*iFile)) < 0){
		fprintf(stderr, "Error while reading from pipe.\n");
		exit(1);
	}
}

void write_nums(int iDir, int iFile, int *pipeArr){
	if(write(pipeArr[1], &iDir, sizeof(iDir)) < 0 || write(pipeArr[1], &iFile, sizeof(iFile)) < 0){
		fprintf(stderr, "Error while writing to pipe.\n");
		exit(1);
	}	
}

int scan_file(FILE *fCurrentFile, const char *sWordFile){
	int iTemp = 0;
	int iResult = 0;
	char *sDelim = " ., ':;!";
	char *sToken;
	char sLine[MAX];
	FILE* fWordFile;
	fWordFile = fopen(sWordFile, "a");
	
	/* write num of words and num of childs to file */
	if(fWordFile != NULL){
		while(fgets(sLine, sizeof(sLine), fCurrentFile)) {
			sToken = strtok(sLine, sDelim);
			while(sToken != NULL){
				iTemp = sscanf(sToken, "%d", &iTemp);
				if(iTemp == 0){	/* check if it's a number or not	*/
					fprintf(fWordFile, "%s\n", sToken);
					/*printf("scan_file -> %s\n", sToken);*/
					++iResult;
				}
				sToken = strtok(NULL, sDelim);
			}
		}
		fclose(fWordFile);
	}
	else{
		fprintf(stderr, "Error scan_file.\n");
		exit(1);
	}
	return iResult;
}

int count_word(const char * sWord, const char *sWordFile){
	int iResult = 0;
	char sToken[MAX];
	FILE *fWordFile;
	fWordFile = fopen(sWordFile, "r");

	if(fWordFile != NULL){
		while(fscanf(fWordFile, "%s", sToken) != EOF) {
			if(strcmp(sToken, sWord) == 0)
				++iResult;
		}
	}
	else{
		fprintf(stderr, "Error on count_word.\n");
		exit(1);
	}
	fclose(fWordFile);
	/*printf("  %d\n", iResult);*/
	return iResult;
}

int create_report(const char *sPath){

	char sWord[MAX];
	char sLog[MAX];
	char sWordFile[MAX];

	int iTotalInTemp = 0;
	FILE *fWordFile;
	FILE *fReport;
	int iSingleWords = 0;

	sprintf(sWordFile, "%s/%s", sPath, "word_data");
	sprintf(sLog, "%s/%s", sPath, "Words.log");

	fWordFile = fopen(sWordFile, "r");
	fReport = fopen(sLog, "w");
	fclose(fReport);

	while(fscanf(fWordFile, "%s", sWord) != EOF){
		/*printf("c_r: %s ", sWord);*/
		if(is_exist(sWord, sLog) != 1){
			++iSingleWords;
			iTotalInTemp = count_word(sWord, sWordFile);

			fReport = fopen(sLog, "a");
			fseek(fReport, 0, SEEK_END);
			fprintf(fReport, "%15s \t %2d\n", sWord, iTotalInTemp);
			fclose(fReport);
		}		
	}

	fclose(fWordFile);
	return iSingleWords;
}

int is_exist(const char *sWord, const char *sLogFile){
	char sToken[MAX];
	FILE *fReport;
	fReport = fopen(sLogFile, "r");

	if(fReport != NULL){
		
		while(fgets(sToken, sizeof(sToken), fReport) != NULL) {
			/*printf("  null");*/
			if(strstr(sToken, sWord) != NULL){
				/*printf("  y");*/
				fclose(fReport);
				return 1;
			}
		}
	}
	else{
		fprintf(stderr, "Error on is exist.\n");
		exit(1);
	}

	fclose(fReport);
	/*printf(" n");*/
	return 0;	/* not exist */
}

int is_directory(const char *sPath){

	struct stat statbuf;
	stat(sPath, &statbuf);	/* get info	*/

	/* check info */
	if(S_ISDIR(statbuf.st_mode))
		return 1;

	return 0;
}
