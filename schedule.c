#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "schedule.h"
#include "schedule_line.h"

typedef struct schedule_s
{
    LinkedList Lines; 
} Schedule_t;


Schedule scheduleCreate()
{ 
    Schedule schedule = (Schedule)malloc(sizeof(Schedule_t));
    if (schedule == NULL)
        return  NULL;
    
    ListResult res = linkedListCreate(&(schedule->Lines), copyLine, freeLine, printLine);    
    if (res != LIST_SUCCESS)
    {
        free(schedule);
        return NULL;
    }
    return schedule;
}


void scheduleDestroy(Schedule schedule)
{
    if (schedule!= NULL)
    {

        ListResult res = linkedListDestroy(schedule->Lines);
        if (res !=LIST_SUCCESS)
                return; 
        free(schedule);
    }
}

ScheduleResult scheduleAddLine(Schedule schedule, ScheduleLineType type, int number, const char *description, double price)
{
    if (schedule == NULL || description == NULL)
        return SCHEDULE_NULL_ARG;

    if (type !=SCHEDULE_LINE_BUS && type != SCHEDULE_LINE_TRAIN && type!=SCHEDULE_LINE_METRO)
        return SCHEDULE_INVALID_LINE_TYPE;

    if (number < 0 || number > 9999)
        return SCHEDULE_INVALID_LINE_NUMBER;

    if (type == SCHEDULE_LINE_TRAIN || type == SCHEDULE_LINE_METRO)
    {
        if ((strcmp(description, "Suburban")!= 0) && (strcmp(description, "Intercity") != 0) && (strcmp(description, "Direct") != 0))
            return SCHEDULE_INVALID_TRAIN_TYPE;
    }
     
    if (price < 0)
        return SCHEDULE_INVALID_PRICE;
    
    if (linkedListFind(schedule->Lines, &number, matchLineByNmuber) == LIST_SUCCESS) 
        return SCHEDULE_LINE_ALREADY_EXISTS; 

    ScheduleLine line = schedule_line_create(type, number, description, price);
    if (line == NULL)
        return SCHEDULE_OUT_OF_MEMORY;
    ListResult res = linkedListInsertLast(schedule->Lines, line);
    if (res != LIST_SUCCESS)
        return SCHEDULE_OUT_OF_MEMORY;
    
    free(line);
    return SCHEDULE_SUCCESS;
}


ScheduleResult scheduleRemoveLine(Schedule schedule, int number)
{
    if(schedule == NULL)
        return SCHEDULE_NULL_ARG;
    
    if (number < 0 || number > 9999)
        return SCHEDULE_INVALID_LINE_NUMBER;
    
    linkedListGoToHead(schedule->Lines);

    if (linkedListFind(schedule->Lines, &number, matchLineByNmuber) == LIST_SUCCESS)  
        linkedListRemoveCurrent(schedule->Lines);
    
    else
        return SCHEDULE_LINE_DOESNT_EXIST;
    return SCHEDULE_SUCCESS;  
}

ScheduleResult scheduleAddStationToLine(Schedule schedule, int number, const char *station, int time)
{
    if (schedule == NULL || station == NULL)
        return SCHEDULE_NULL_ARG;
    
    if (number < 0 || number > 9999)
        return SCHEDULE_INVALID_LINE_NUMBER;
    
    if (time < 0)
        return SCHEDULE_INVALID_TIME;
    
    ScheduleLine line= NULL;
    linkedListGoToHead(schedule->Lines);
    if (linkedListFind(schedule->Lines, &number, matchLineByNmuber) == LIST_SUCCESS)  
    {
        linkedListGetCurrent(schedule->Lines,(ListElement*)&line);
        if(add_station_to_line(line, station, time) != SCHEDULE_LINE_SUCCESS)
            return SCHEDULE_OUT_OF_MEMORY;
    }
    else
        return SCHEDULE_LINE_DOESNT_EXIST;
    return SCHEDULE_SUCCESS;
}

ScheduleResult scheduleRemoveStationFromLine(Schedule schedule, int number, int index)
{
    if (schedule == NULL)
        return SCHEDULE_NULL_ARG; 
    
    if (number < 0 || number > 9999)
        return SCHEDULE_INVALID_LINE_NUMBER;
    
    ScheduleLine line= NULL;

    linkedListGoToHead(schedule->Lines);
    if (linkedListFind(schedule->Lines, &number, matchLineByNmuber) == LIST_SUCCESS)  
    {
        linkedListGetCurrent(schedule->Lines,(ListElement*)&line);
        ScheduleLineResult res = remove_station_from_line(line, index);
        if (res != SCHEDULE_LINE_SUCCESS)
            return SCHEDULE_STATION_DOESNT_EXIST;   
    }

    else
        return SCHEDULE_LINE_DOESNT_EXIST;
    return SCHEDULE_SUCCESS;
}

ScheduleResult scheduleReportStationsForLine(Schedule schedule, int number)
{
    if(schedule == NULL)
        return SCHEDULE_NULL_ARG; 
    
    if (number < 0 || number > 9999)
        return SCHEDULE_INVALID_LINE_NUMBER;


    ScheduleLine line= NULL;
    linkedListGoToHead(schedule->Lines);
    if (linkedListFind(schedule->Lines, &number, matchLineByNmuber) == LIST_SUCCESS)  
    {
        linkedListGetCurrent(schedule->Lines,(ListElement*)&line);
        ScheduleLineResult res = print_station_Time(line);
        if (res!= SCHEDULE_LINE_SUCCESS)
            return SCHEDULE_NULL_ARG;        
    }
    else
        return SCHEDULE_LINE_DOESNT_EXIST; 
    return SCHEDULE_SUCCESS;
    
}

ScheduleResult scheduleReportLines(Schedule schedule,ScheduleLineType type)
{
    if (schedule == NULL)
        return SCHEDULE_NULL_ARG;
    
    if (type !=SCHEDULE_LINE_BUS && type != SCHEDULE_LINE_TRAIN && type !=SCHEDULE_LINE_METRO && type !=SCHEDULE_LINE_ALL)
        return SCHEDULE_INVALID_LINE_TYPE;
    
    int amount;
    amount = linkedListGetNumElements(schedule->Lines);
    if(amount == 0)
        return SCHEDULE_NO_LINES;
    
    ListResult res;
    res = linkedListSortElements(schedule->Lines, compareStationsByPrice);
    if(res != LIST_SUCCESS)
            return SCHEDULE_NULL_ARG;
    
    if (type == SCHEDULE_LINE_ALL)
    {
        res = linkedListPrint(schedule->Lines, stdout,amount);
        if (res != LIST_SUCCESS)
            return SCHEDULE_NULL_ARG;
        return SCHEDULE_SUCCESS;
    }
    else
    {
        Schedule new_schedule = scheduleCreate();
        if(linkedListFilterElements(schedule->Lines, &(new_schedule->Lines), matchLineByType, (KeyForListElement)&type) != LIST_SUCCESS)
        {
            scheduleDestroy(new_schedule);
            return SCHEDULE_NULL_ARG;
        }
        amount = linkedListGetNumElements(new_schedule->Lines);
        res = linkedListPrint(new_schedule->Lines, stdout,amount);
        if (res != LIST_SUCCESS)
            return SCHEDULE_NULL_ARG;
        free(new_schedule);
    }
        return SCHEDULE_SUCCESS;
    
}

ScheduleResult scheduleReportLinesBetweenStations(Schedule schedule, const char *from,const char *to)
{
    int amount =0;
    if(schedule == NULL)
        return SCHEDULE_NULL_ARG;
    ScheduleLine line = NULL;
    if(schedule->Lines != NULL)
    {
        linkedListGoToHead(schedule->Lines);
        amount = linkedListGetNumElements(schedule->Lines);
        do
        {
            linkedListGetCurrent(schedule->Lines,(ListElement*)&line);
            ScheduleLineResult res = station_from_to(line ,from, to);
            if(res == SCHEDULE_LINE_BAD_ARGUMENTS)
                amount --;
        }while (linkedListGoToNext(schedule->Lines) == LIST_SUCCESS);
        
        return SCHEDULE_SUCCESS;
        
        if (amount == 0)
            return SCHEDULE_NO_LINES;
    }
    return SCHEDULE_NULL_ARG;
}