#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "schedule_station.h"
 
 
typedef struct schedule_station_s
{
    int index;
    char *stopName;
    int stationTime;
    copyListElemFunc cpyFunc;
    freeListElemFunc freeFunc; 
    printListElemFunc printFunc;
}schedule_station_t;

ListElement copyStation(ListElement element)
{
    ScheduleStation station = (ScheduleStation)element;
    ScheduleStation new_station = NULL;
    if (element == NULL)
        return NULL;
    
    new_station = (ScheduleStation)malloc(sizeof(schedule_station_t));
    if (new_station == NULL)
        return NULL;

    new_station->stopName= (char *)malloc((strlen(station->stopName) + 1)*sizeof(char));
    if (new_station->stopName == NULL)
    {
        free(new_station);
        return NULL;
    }
    
    new_station->index = station->index;
    strcpy(new_station->stopName, station->stopName);
    new_station->stationTime = station->stationTime;
    new_station->cpyFunc = station->cpyFunc;
    new_station->freeFunc = station->freeFunc;
    new_station->printFunc = station->printFunc;
    return new_station;
}

void freestation(ListElement element)
{
    ScheduleStation station = (ScheduleStation)element;
    if (station!= NULL)
    {
        free(station->stopName);
        free(station);
    }
}

void printstation(FILE *out,ListElement element)
{
    ScheduleStation station = (ScheduleStation)element;
    if (station == NULL)
        return;
    schedulePrintStation(out, station->stopName, station->stationTime);
}

char *LastFirst(ScheduleStation stat)
{
    char *Name = (char *)malloc((strlen(stat->stopName) + 1) * sizeof(char));
    Name = stat->stopName;
    return Name;
}

int matchLineByDescription(ListElement element, KeyForListElement key)
{
    ScheduleStation stat = (ScheduleStation)element;
    char *name = (char*) key;
    if (element == NULL || key == NULL)
        return 0;
    return strcmp(stat->stopName, name) ==0;
}


int matchStationByIndex(ListElement element, KeyForListElement key)
{
    ScheduleStation station = (ScheduleStation)element;
    int index = *(int *)key; 
    if (element == NULL || key == NULL)
        return 0;
    return station->index == index;
}

int compareStationsByTime(ListElement element1, ListElement element2)
{
    ScheduleStation station1 = (ScheduleStation)element1;
    ScheduleStation station2 = (ScheduleStation)element2;
    if (station1 == NULL || station2 == NULL || station1->stationTime == station2->stationTime)
        return 0;
    return station1->stationTime > station2->stationTime ? 1:-1;
}

ScheduleStation schedule_station_create(int index, const char *station, int time)
{
    ScheduleStation new_station = (ScheduleStation)malloc(1 * sizeof(schedule_station_t));
    if (new_station == NULL)
        return NULL;

    new_station->stopName = (char *)malloc((strlen(station) + 1) * sizeof(char));
    if (new_station->stopName == NULL)
    {
        free(new_station);
        return NULL;
    }

    new_station->index= index;
    strcpy(new_station->stopName, station);
    new_station->stationTime = time;
    new_station->cpyFunc = copyStation;
    new_station->freeFunc = freestation;
    new_station->printFunc = printstation;
    return new_station;
}


