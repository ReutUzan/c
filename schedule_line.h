#ifndef __SCHEDULE_LINE_H__
#define __SCHEDULE_LINE_H__

#include "schedule_station.h"
 

typedef struct schedule_line_s *ScheduleLine;
typedef enum
{
    SCHEDULE_LINE_SUCCESS,
    SCHEDULE_LINE_FAIL,
    SCHEDULE_LINE_BAD_ARGUMENTS,
    SCHEDULE_LINE_OUT_OF_MEMORY,
    SCHEDULEL_STATION_DOESNT_EXIST,
} ScheduleLineResult;


ListElement copyLine(ListElement element);
void freeLine(ListElement element);
void printLine(FILE *out,ListElement element);
int matchLineByNmuber(ListElement element, KeyForListElement key);
int matchLineByType(ListElement element, KeyForListElement key);
int compareStationsByPrice(ListElement element1, ListElement element2);
ScheduleLine schedule_line_create(ScheduleLineType type, int number, const char *description, double price);
ScheduleLineResult add_station_to_line(ScheduleLine line,const char *station, int time);
ScheduleLineResult remove_station_from_line(ScheduleLine line, int index);
ScheduleLineResult print_station_Time(ScheduleLine line);
ScheduleLineResult station_from_to(ScheduleLine line,const char *from,const char *to);


#endif