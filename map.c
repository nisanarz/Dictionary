/*
 * map.c
 *
 *  Created on: 12 бреб 2013
 *      Author: Nisan
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "map.h"

typedef struct node_t {
	MapKeyElement key;
	MapDataElement data;
	struct node_t* nextNode;
} *Node;

struct Map_t {
	Node currentNode;
	Node firstNode;
	copyMapKeyElements copyMapKey;
	copyMapDataElements copyMapData;
	freeMapKeyElements freeMapKey;
	freeMapDataElements freeMapData;
	compareMapKeyElements compareKeys;
};

#define CREATE_NODE(ptr) ptr = malloc(sizeof(*(ptr))); \
			if (ptr == NULL) \
				return MAP_OUT_OF_MEMORY; \
			ptr->key = map->copyMapKey(keyElement);\
			ptr->data = map->copyMapData(dataElement);\
			ptr->nextNode = NULL; \
			if (ptr->key == NULL) { \
				map->freeMapKey(ptr->key); \
			return MAP_OUT_OF_MEMORY;} \
			if (ptr->key == NULL) { \
				map->freeMapData(ptr->data); \
			return MAP_OUT_OF_MEMORY;}


Map mapCreate(copyMapDataElements copyDataElement,
		copyMapKeyElements copyKeyElement,freeMapDataElements freeDataElement,
		freeMapKeyElements freeKeyElement,
		compareMapKeyElements compareKeyElements){
	if (!copyDataElement || !copyKeyElement || !freeDataElement
			|| !freeKeyElement || !compareKeyElements){
		return NULL;
	}
	Map newMap=malloc(sizeof(*newMap));
	if (!newMap){
		return NULL;
	}
	newMap->firstNode=NULL;
	newMap->currentNode=NULL;
	newMap->copyMapKey=copyKeyElement;
	newMap->copyMapData=copyDataElement;
	newMap->freeMapKey=freeKeyElement;
	newMap->freeMapData=freeDataElement;
	newMap->compareKeys=compareKeyElements;
	return newMap;
}

void mapDestroy(Map map){
	if (!map){
		return;
	}
	mapClear(map);
	free(map);
}

Map mapCopy(Map map){
	if (!map){
		return NULL;
	}
	Map newMapCopy=mapCreate(map->copyMapData,map->copyMapKey,map->freeMapData,
			map->freeMapKey,map->compareKeys);
	if (!newMapCopy){
		return NULL;
	}
	map->currentNode=map->firstNode;
	while (map->currentNode){
		mapPut(newMapCopy,map->currentNode->key,map->currentNode->data);
		newMapCopy->currentNode=newMapCopy->currentNode->nextNode;
		map->currentNode=map->currentNode->nextNode;
	}
	return newMapCopy;
}

int mapGetSize(Map map){
	if (!map){
		return -1;
	}
	if (map->firstNode == NULL){
		return 0;
	}
	int count=0;
	map->currentNode=map->firstNode;
	while (map->currentNode != NULL){
		count++;
		map->currentNode=map->currentNode->nextNode;
	}
	return count;
}

bool mapContains(Map map, MapKeyElement element){
	if (!map || !element){
		return false;
	}
	map->currentNode=map->firstNode;
	while (map->currentNode){
		if (map->compareKeys(map->currentNode->key,element) == 0){
			return true;
		}
		map->currentNode=map->currentNode->nextNode;
	}
	return false;
}

MapResult mapPut(Map map, MapKeyElement keyElement, MapDataElement dataElement){
	if (!map || !keyElement || !dataElement){
		return MAP_NULL_ARGUMENT;
	}
	if (map->firstNode == NULL){
		CREATE_NODE(map->firstNode);
		map->currentNode=map->firstNode;
		return MAP_SUCCESS_ITEM_ADDED;
	}
	map->currentNode=map->firstNode;
	while (map->currentNode){
		if (map->compareKeys(map->currentNode->key, keyElement) == 0){
			map->freeMapData(map->currentNode->data);
			map->currentNode->data=map->copyMapData(dataElement);
			if (map->currentNode->data == NULL){
				return MAP_OUT_OF_MEMORY;
			}
			return MAP_SUCCESS;
		}
		map->currentNode=map->currentNode->nextNode;
	}
	map->currentNode=map->firstNode;
	while(map->currentNode->nextNode){
		map->currentNode=map->currentNode->nextNode;
	}
	CREATE_NODE(map->currentNode->nextNode);
	return MAP_SUCCESS_ITEM_ADDED;
}

MapDataElement mapGet(Map map, MapKeyElement keyElement){
	if (!map || !keyElement){
		return NULL;
	}
	map->currentNode=map->firstNode;
	while (map->currentNode){
		if (map->compareKeys(map->currentNode->key,keyElement) == 0){
			return map->copyMapData(map->currentNode->data);
		}
		map->currentNode=map->currentNode->nextNode;
	}
	return NULL;
}

MapResult mapRemove(Map map, MapKeyElement keyElement){
	if (!map){
		return MAP_NULL_ARGUMENT;
	}
	if (!keyElement){
		return MAP_NULL_ARGUMENT;
	}
	if (map->firstNode == NULL){
		return MAP_ITEM_DOES_NOT_EXIST;
	}
	Node tmp;
	if (map->compareKeys(map->firstNode->key,keyElement) == 0){
		tmp=map->firstNode->nextNode;
		map->freeMapKey(map->firstNode->key);
		map->freeMapData(map->firstNode->data);
		free(map->firstNode);
		map->firstNode=tmp;
		return MAP_SUCCESS;
	}
	Node savePrevious;
	map->currentNode=map->firstNode;
	while (map->currentNode){
		if(map->compareKeys(map->currentNode->key,keyElement) == 0){
			savePrevious->nextNode=map->currentNode->nextNode;
			map->freeMapKey(map->currentNode->key);
			map->freeMapData(map->currentNode->data);
			free(map->currentNode);
			map->currentNode=NULL;
			return MAP_SUCCESS;
		}
		savePrevious=map->currentNode;
		map->currentNode=map->currentNode->nextNode;
	}
	return MAP_ITEM_DOES_NOT_EXIST;
}

MapKeyElement mapGetFirst(Map map, MapResult* result){
	if (!result){
		return NULL;
	}
	if (!map){
		*result=MAP_NULL_ARGUMENT;
		return NULL;
	}
	if (map->firstNode == NULL){
		*result=MAP_SUCCESS;
		return NULL;
	}
	map->currentNode=map->firstNode;
	MapKeyElement nextCopy=map->copyMapKey(map->firstNode->key);
	if (!nextCopy){
		*result=MAP_OUT_OF_MEMORY;
		return NULL;
	}
	*result=MAP_SUCCESS;
	return nextCopy;
}

MapKeyElement mapGetNext(Map map, MapResult* result){
	if (!result){
		return NULL;
	}
	if (!map){
		*result=MAP_NULL_ARGUMENT;
		return NULL;
	}
	if (map->currentNode != NULL){
		if (map->currentNode->nextNode != NULL){
			map->currentNode=map->currentNode->nextNode;
			MapKeyElement nextCopy=map->copyMapKey(map->currentNode->key);
			if (!nextCopy){
				*result=MAP_OUT_OF_MEMORY;
				return NULL;
			}
			*result=MAP_SUCCESS;
			return nextCopy;
		}
	}
	*result=MAP_SUCCESS;
	return NULL;
}

MapResult mapClear(Map map){
	if (!map){
		return MAP_NULL_ARGUMENT;
	}
	if(map->firstNode == NULL){
		return MAP_SUCCESS;
	}
	Node tmp;
	while(map->firstNode){
		tmp=map->firstNode;
		map->firstNode=map->firstNode->nextNode;
		map->freeMapKey(tmp->key);
		map->freeMapData(tmp->data);
		free(tmp);
	}
	return MAP_SUCCESS;
}

