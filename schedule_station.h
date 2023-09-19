#ifndef __SCHEDULE_STATION_H__
#define __SCHEDULE_STATION_H__

#include "pr2ex5.h"
#include "linked_list.h"

typedef struct schedule_station_s *ScheduleStation;

typedef enum
{
    SCHEDULE_STATION_SUCCESS,
    SCHEDULE_STATION_FAIL,
    SCHEDULE_STATION_BAD_ARGUMENTS,
    SCHEDULE_STATION_OUT_OF_MEMORY,
} ScheduleStationResult;

ListElement copyStation(ListElement element);
void freestation(ListElement element);
void printstation(FILE *out,ListElement element);
char *LastFirst(ScheduleStation stat);
int matchLineByDescription(ListElement element, KeyForListElement key);
int matchStationByIndex(ListElement element, KeyForListElement key);
int compareStationsByTime(ListElement element1, ListElement element2);
ScheduleStation schedule_station_create(int index, const char *station, int time);


#endif