/*
 * OsadaSynchronization.c
 *
 *  Created on: 28/10/2016
 *      Author: utnso
 */

#include "OsadaMaster.h"

t_log_level logLevelSync = LOG_LEVEL_DEBUG;
t_log * loggerSync;
pthread_rwlock_t* vectorSemaforos[2048];
pthread_mutex_t mapMutex; // = PTHREAD_MUTEX_INITIALIZER; hacer esto o hacer el init es lo mismo hasta donde yo se

void initOsadaSync() {
	loggerSync = log_create("logSems.txt", "PokedexServerSync", true, logLevelSync);
	pthread_mutex_init(&mapMutex, NULL);

}

void waitFileSemaphore(int filePosition, osada_operation operation){

		log_debug(loggerSync, "tomando semaforo: %d",filePosition);
		pthread_mutex_lock(&mapMutex);
		if (vectorSemaforos[filePosition]!=NULL) {
			pthread_rwlock_t*  semArchivo = vectorSemaforos[filePosition];
				pthread_mutex_unlock(&mapMutex);
			if(operation==READ){
				pthread_rwlock_rdlock(semArchivo);

			}if(operation==WRITE){
				pthread_rwlock_wrlock(semArchivo);
			}
			} else {
				pthread_rwlock_t* newSem;
				pthread_rwlock_init(newSem,NULL);
				vectorSemaforos[filePosition]=newSem;
			pthread_rwlock_rdlock(newSem);
			pthread_mutex_unlock(&mapMutex);
			}
		return;
}

void freeFileSemaphore(int filePosition) {
	log_debug(loggerSync, "liberando semaforo: %d",filePosition);

	pthread_mutex_lock(&mapMutex);
	if (vectorSemaforos[filePosition]==NULL) {
		perror("Falló algo en el diccionario de sincronización");
	} else {
		pthread_mutex_unlock(&mapMutex);
		pthread_rwlock_t* semaforo=vectorSemaforos[filePosition];
		pthread_rwlock_unlock(semaforo);
	};
	return;

}

