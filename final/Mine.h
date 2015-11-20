
#ifndef MINE_H
#define MINE_H

#include "Mining.h"

/* Log dosyasının güncellenme parametreleri burdan değiştirilebilir. */
#define LOGNAME "mine.log"
#define LOG_REFRESH_TIME 2
#define LOG_FILE_TYPE "w"

#define SERVED 10
#define PENDING 20
#define WORKING 30
#define MAX_ARGS 10


/*
 *	Her bir miner için bilgi tutan struct.
 */
typedef struct{
	/* miner'ın start ve finish zamanları */
	char s_time[100];
	char f_time[100];
	
	/* kazandığı gold */
	int gold;
	
	/* şu andaki durumu (working, pending veya served) */
	int stat;
	
	/* pid'si */
	int pid;
} miner_info_t;

/*
 *	bir kelimeyi array'a koyar.
 *	@param sWord:	eklenecek kelime
 *	@param amount:	adedi
 *	@return: kazanılan gold
 */
int tryadd(const char *sWord, int amount);

/*
 *	gelen miner'lar ile ilgilenecek thread fonksiyonu
 *	@param: socket_desc:	miner'ın pre_info_t struct'ını yollayacağı socket descriptor'ı.	
 */
void * client_handler(void *socket_desc);

/*
 *	global working, pending ve done(served) değerlerini günceller. ayrıca miner array'indeki
 *	bir miner'ın durumunu da günceller.
 *	@param w:	working'in arttırılacağı değer.
 *	@param p:	pending'in arttırılacağı değer.
 *	@param d:	done değerinin arttırılacağı değer.
 *	@param m_i:	miner'ın array'deki index'i.
 *	@param m_s:	miner'ın güncel status'u.
 */
void handle_vars(int w, int p, int d, int m_i, int m_s);

/*
 *	Verilen bir path'in sub-pathlerini alıp, global array'de saklar.
 */
void get_subs(const char *path);

/*
 *	Bir miner'ın istediği kadar entity şu an array'de var mı yok mu diye bakar.
 *	@param n:	istenilen entity sayısı.
 *	@return eğer yeterliyse 1, değilse 0.
 */
int have_subs(int n);

/*
 *	sinyal gelmesi durumunda miner'lar için açılan threadleri bekler
 */
void wait_threads();

/*
 *	Log dosyasını sürekli güncelleyen thread fonksiyonu. Ne aralıklarla ve hangi
 *	modda güncelleceği, bu header file'ın başında ayarlanabilir.
 */
void * logger(void * nothing);

/*
 *	Güncel global working, pending ve done(served) değerlerini verir.
 */
void getvars(int *w, int *p, int *d);

#endif
