/*
 * OsadaSynchronization.c
 *
 *  Created on: 28/10/2016
 *      Author: utnso
 */

#include "OsadaMaster.h"

t_dictionary * syncMap;
pthread_mutex_t * mapMutex = PTHREAD_MUTEX_INITIALIZER;

void initOsadaSync() {
	syncMap = dictionary_create();
	pthread_mutex_init(mapMutex, NULL);

}

int internalWaitSemaphore(int file, osada_operation operation){
	char * filePositionString;
		sprintf(filePositionString, "%d", file);
		pthread_mutex_lock(mapMutex);
		if (dictionary_has_key(syncMap, filePositionString)) {
			osada_sync_struct * syncData = dictionary_get(syncMap,
					filePositionString);
			if (syncData->operation == READ) {
				if (operation == READ) {
					syncData->reading++;
					pthread_mutex_unlock(mapMutex);
					return 1;
				} else if (operation == WRITE) {
					pthread_mutex_unlock(mapMutex);
					pthread_mutex_lock(syncData->mutex);
					return -1;
				}
			} else if (syncData->operation == FREE) {
				if (operation == READ) {
					pthread_mutex_t * fileMutex = PTHREAD_MUTEX_INITIALIZER;
					pthread_mutex_lock(fileMutex);
					syncData->mutex = fileMutex;
					syncData->reading = 1;
					syncData->operation = READ;
					pthread_mutex_unlock(mapMutex);
					return 1;
				} else if (operation == WRITE) {
					pthread_mutex_t * fileMutex = PTHREAD_MUTEX_INITIALIZER;
					pthread_mutex_lock(fileMutex);
					syncData->operation = WRITE;
					pthread_cond_t * cond = PTHREAD_COND_INITIALIZER;
					syncData->condition = cond;
					pthread_cond_init(cond, NULL);
					pthread_mutex_unlock(mapMutex);
					return 1;
				}

			} else if (syncData->operation == WRITE) {
				if (operation == READ) {
					pthread_mutex_unlock(mapMutex);
					pthread_cond_wait(syncData->condition, syncData->mutex);
				} else if (operation == WRITE) {
					pthread_mutex_unlock(mapMutex);
					pthread_mutex_lock(syncData->mutex);
				}
				return -1;
			}
		} else {
			osada_sync_struct * newSyncStruct;
			newSyncStruct->operation = FREE;
			newSyncStruct->reading = 0;
			dictionary_put(syncMap, filePositionString, newSyncStruct);
			pthread_mutex_unlock(mapMutex);
			return -1;
		}
		return -1;
}

void waitFileSemaphore(int file, osada_operation operation) {
	while(internalWaitSemaphore(file, operation)==-1){

	}

}



void freeFileSemaphore(int file, osada_operation operation) {
	char * filePositionString;
	sprintf(filePositionString, "%d", file);
	pthread_mutex_lock(mapMutex);
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
			}
		}
		if (operation == WRITE) {
			syncData->operation = FREE;
			pthread_cond_broadcast(syncData->condition);
		}
	}
	pthread_mutex_unlock(mapMutex);
}
;

