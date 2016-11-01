/*
 * OsadaSynchronization.c
 *
 *  Created on: 28/10/2016
 *      Author: utnso
 */

#include "OsadaMaster.h"

t_log_level logLevelSync = LOG_LEVEL_DEBUG;
t_log * loggerSync;
t_dictionary * syncMap;
pthread_mutex_t mapMutex = PTHREAD_MUTEX_INITIALIZER;

void initOsadaSync() {
	loggerSync = log_create("log.txt", "PokedexServerSync", true, logLevelSync);
	syncMap = dictionary_create();
	pthread_mutex_init(&mapMutex, NULL);

}

int internalWaitSemaphore(int file, osada_operation operation){
	char * filePositionString = malloc(200);
		sprintf(filePositionString, "%d", file);
		pthread_mutex_lock(&mapMutex);
		if (dictionary_has_key(syncMap, filePositionString)) {
			osada_sync_struct * syncData = dictionary_get(syncMap,
					filePositionString);
			if (syncData->operation == READ) {
				if (operation == READ) {
					syncData->reading++;
					pthread_mutex_unlock(&mapMutex);
					return 1;
				} else if (operation == WRITE || operation == DELETE) {
					pthread_mutex_unlock(&mapMutex);
					pthread_mutex_lock(syncData->mutex);
					return -1;
				}
			} else if (syncData->operation == FREE) {
				if (operation == READ) {
					pthread_mutex_t fileMutex = PTHREAD_MUTEX_INITIALIZER;
					pthread_mutex_lock(&fileMutex);
					syncData->mutex = &fileMutex;
					syncData->reading = 1;
					syncData->operation = READ;
					pthread_mutex_unlock(&mapMutex);
					return 1;
				} else if (operation == WRITE || operation == DELETE) {
					pthread_mutex_t fileMutex = PTHREAD_MUTEX_INITIALIZER;
					syncData->mutex=&fileMutex;
					pthread_mutex_lock(syncData->mutex);
					syncData->operation = operation;
					pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
					syncData->condition = &cond;
					pthread_cond_init(&cond, NULL);
					pthread_mutex_unlock(&mapMutex);
					return 1;
				}

			} else if (syncData->operation == WRITE) {
				if (operation == READ) {
					pthread_mutex_unlock(&mapMutex);
					pthread_cond_wait(syncData->condition, syncData->mutex);
				} else if (operation == WRITE || operation == DELETE) {
					pthread_mutex_unlock(&mapMutex);
					pthread_mutex_lock(syncData->mutex);
				}
				return -1;
			}else if (syncData->operation == DELETE){
				return -2;
			}
		} else {
			osada_sync_struct * newSyncStruct = calloc(1,sizeof(osada_sync_struct));
			newSyncStruct->operation = FREE;
			newSyncStruct->reading = 0;
			dictionary_put(syncMap, filePositionString, newSyncStruct);
			pthread_mutex_unlock(&mapMutex);
			return -1;
		}
		return -1;
}

int waitFileSemaphore(int file, osada_operation operation) {
	log_debug(loggerSync, "Waiting semaphore");
	int status = -1;
	while(status==-1){
		status = internalWaitSemaphore(file, operation);
	}
	return status;

}



void freeFileSemaphore(int file, osada_operation operation) {
	log_debug(loggerSync, "Releasing Semaphore");
	char * filePositionString = calloc(1,10);
	sprintf(filePositionString, "%d", file);
	pthread_mutex_lock(&mapMutex);
	if (!dictionary_has_key(syncMap, filePositionString)) {
		perror("Falló algo en el diccionario de sincronización");
	} else {
		osada_sync_struct * syncData = dictionary_get(syncMap,
				filePositionString);
		if (syncData->operation != operation) {
			perror(
					"Al liberar mutex la operacion no es la misma que estaba reservada");
			exit(-1);
		}
		if (operation == READ) {
			syncData->reading--;
			if (syncData->reading <= 0) {
				syncData->operation = FREE;
				pthread_mutex_unlock(syncData->mutex);
				pthread_mutex_destroy(syncData->mutex);
			}
		}
		else if (operation == WRITE || operation == DELETE) {
			syncData->operation = FREE;
			pthread_cond_broadcast(syncData->condition);
			pthread_cond_destroy(syncData->condition);
		}
		pthread_mutex_unlock(&mapMutex);
		return;
	}
}
;

