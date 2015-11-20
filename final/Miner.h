
#ifndef MINER_H
#define MINER_H

#include "Mining.h"
/*
 *	Bir dosyayı inceleyip, kelimelerini ayıklayan thread fonksiyonu.
 *	@param thread_info:	dosya adı.
 */
void * scan_file(void * thread_info);

/*
 *	Eğer status istenilen durumdaysa, onu değiştirir.
 *	@param cond:	istenilen durum
 *	@param stat:	yeni atanacak durum
 *	@return eğer status değiştirilirse 1, diğer durumda 0. 
 */
int change_status(int cond, int stat);

/*
 *	Bir path'in var olup olmadığını kontrol eder.
 *	@param sPath:	kontrol edilecek path.
 *	@return varsa 1, yoksa 0.
 */
int file_exist (const char *sPath);

/*
 *	Verilen bir path'in klasör olup olmadığına bakar.
 *	@param sPath:	incelenecek path.
 *	@return eğer klasörse 1, değilse 0.
 */
int is_directory(const char *sPath);

/*
 *	Verilen bir kelimeyi kelime array'ine koymaya çalışır.
 *	@param sWord:	eklenmeye çalışılacak kelime.
 *	@return eğer kelime bu fonksiyon çalışılmadan önce array'de varsa 0,
 			bu kelime için bu fonksiyon ilk kez çağırılıyosa 1.
 */
int tryadd(const char *sWord);
#endif
