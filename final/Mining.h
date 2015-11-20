/*
	Eyyüp Aydın - 131044038
	
*/

#ifndef MINING
#define MINING

#define _POSIX_C_SOURCE	199309L
#define MAX 4096
#define PERMS (mode_t)(S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
#define FLAGS (O_CREAT | O_EXCL)

#include <stdio.h>
#include <stdlib.h>	
#include <errno.h>
#include <signal.h>

#include <semaphore.h>	
#include <unistd.h>
#include <pthread.h>
#include <dirent.h>	
#include <string.h>
#include <fcntl.h>			
#include <limits.h>
#include <time.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/ipc.h>
#include <sys/sem.h>				
#include <sys/shm.h> 
#include <sys/errno.h> 
#include <sys/time.h>
#include <sys/types.h> 
#include <sys/stat.h>  
#include <sys/wait.h>   			

#define FIRST 1
#define FILLED 2
#define DONE 3
#define RECEIVED 5
#define SENT 6
#define IGNORE -100

#define WORD -1
#define DIRECTORY -2

#define S_PORT 53214

/*
 *	Her bir miner'ın, sokete bağlandığı anda, iletişimin devam etmesini
 *	sağlayacak bilgilerin bulunduğu struct.
 */
struct pre_info{
	/* istenilen entity sayısı */
	int n;
	
	/* miner ile mine'daki bir thread'ın ortak kullanacağı semaphore adı */
	char semname[MAX];
	
	/* kullanılacak shared memory ID'si */
	int shmId;
	
	/* miner'ın pid'si */
	int pid;
};

/*
 *	Bir entity tutan struct.
 */
typedef struct {
	char sub[MAX];
}sub_t;


/*
 *	Bir kelime tutan struct. 
 */
typedef struct {
	/* kelimenin kendisi */
	char word[MAX];
	
	/* kelimenin adedi */
	int amount;
	
	/* bu kelime için kazanılan gold */
	int gold;
} word_t;


/*
 *	Her bir miner'ın, mine ile iletişime geçmesi için açtığı shared memory
 *	struct'ı.
 */
struct cl_info_t{
	/* gönderim/alım durumu */
	int status;
	
	/* gönderilen bilginin tipi (file veya directory) */
	int type;

	/* gönderilen bilgi */
	char info[MAX];
	
	/* eğer gönderilen bilgi kelime ise, adedi */
	int amount;
	
	/* bu miner'ın toplam kazandığı gold */
	int gold;
	
	/* her bir bilgi gönderiminde kazandığı gold */
	int newword;
	
	/* ilk gönderimden kazandığı gold */
	int first;
};


void sig_handler(int sig);

/*
 *	Bir kelimenin, şu an zaten var olup olmadığına bakar.
 *	Eğer varsa, yerini verir "where" parametresi ile.
 *	@param sWord:	aranacak kelime.
 *	@param where:	kelimenin array'deki yeri.
 *	@return: bulunursa 1, bulunmazsa 0.
 */
int is_exist(const char *sWord, int *where);

/*
 *	free işlemlerini yapıp programı sonrandırır.
 *	@param val: exit değeri.
 */
void quit(int val);

/*
 *	allocation işlemleri yapar.
 */
void init();

/*
 *	allocate edilmiş yerleri bulup free'ler.
*/
void clean();

/*
 *	gizli veya root klasörleri ignore eder.
 */
int dont_look(const char *sPath){
	return (!strcmp(sPath, ".") || !strcmp(sPath, "..") || sPath[strlen(sPath) -1] == '~');
}

/*
 * Bir harfin noktalama işareti olup olmadığına bakar.
 */
int is_punctuation(char ch){
	return ch == '.' || ch == ',' || ch == ':' || ch == ';' || ch == '!' || ch == '?';
}

/*
 *	Verilen bir path için; eğer daha önce yok ise yeni bir named semaphore
 *	oluşturur. Eğer daha önce varsa, ona bağlanır.
 *
 *	Bu kodu kitaptan aldım.
 */
int get_named_sem(char *name, sem_t **sem, int val){
	while(((*sem = sem_open(name, FLAGS, PERMS, val)) == SEM_FAILED) && (errno == EINTR))
		;

	if(*sem != SEM_FAILED)
		return 0;
	if(errno != EEXIST)
		return -1;
	while(((*sem = sem_open(name, 0)) == SEM_FAILED) && (errno == EINTR))
		;

	if(*sem != SEM_FAILED)
		return 0;
	return -1;
}

/*
 *	Verilen isimdeki semaphore'u kaldırır.
 *
 *	Bu kod da kitaptan.
 */
int destroynamed(char *name, sem_t *sem) {
	int error = 0;
	if (sem_close(sem) == -1)
		error = errno;
	if ((sem_unlink(name) != -1) && !error)
		return 0;
	if (error)
		/* set errno to first error that occurred */
		errno = error;
	return -1;
}
#endif
