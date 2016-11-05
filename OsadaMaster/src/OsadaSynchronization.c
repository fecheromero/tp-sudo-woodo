/*
 * OsadaSynchronization.c
 *
 *  Created on: 28/10/2016
 *      Author: utnso
 */

#include "OsadaMaster.h"

t_log_level logLevelSync = LOG_LEVEL_DEBUG;
t_log * loggerSync;
pthread_rwlock_t vectorSemaforos[2048];
pthread_mutex_t mapMutex= PTHREAD_MUTEX_INITIALIZER;
void initOsadaSync() {
	loggerSync = log_create("logSems.txt", "PokedexServerSync", true, logLevelSync);
	int i;
	for(i=0;i<2048;i++){
		pthread_rwlock_init(&vectorSemaforos[i],NULL);
	}

}

void waitFileSemaphore(int filePosition, osada_operation operation){

		log_debug(loggerSync, "tomando semaforo: %d",filePosition);
		pthread_mutex_lock(&mapMutex);
			if(operation==READ){
				int j=pthread_rwlock_rdlock(&vectorSemaforos[filePosition]);

			}
			if(operation==WRITE){
				int h=pthread_rwlock_wrlock(&vectorSemaforos[filePosition]);
			}

pthread_mutex_unlock(&mapMutex);

		return;
}

void freeFileSemaphore(int filePosition) {
	log_debug(loggerSync, "liberando semaforo: %d",filePosition);

	pthread_mutex_lock(&mapMutex);
		int i=pthread_rwlock_unlock(&vectorSemaforos[filePosition]);
	pthread_mutex_unlock(&mapMutex);

		printf("%d \n",i);
	return;

}

