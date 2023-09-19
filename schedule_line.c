#include <stdio.h>
#include <stdlib.h>
#include <string.h>
  
#include "schedule_line.h"

typedef struct schedule_line_s
{
    ScheduleLineType type;
    int number;
    char *description;
    double price;
    LinkedList stations;
    copyListElemFunc cpyFunc;
    freeListElemFunc freeFunc; 
    printListElemFunc printFunc;
} Schedule_line_t;

ListElement copyLine(ListElement element)
{
    ScheduleLine Line = (ScheduleLine)element;
    ScheduleLine new_line = NULL;
    if (element == NULL)
        return NULL;
    
    new_line = (ScheduleLine)malloc(sizeof(Schedule_line_t));
    if (new_line == NULL)
        return NULL;
    
    ListResult res = linkedListCreate(&(new_line->stations), copyStation ,freestation ,printstation);
    if (res != LIST_SUCCESS)
    {
        free(new_line);
        return NULL;
    }

    new_line->description = (char *)malloc((strlen(Line->description) + 1) * sizeof(char));
    if (new_line->description == NULL)
    {
        free(new_line->stations);
        free(new_line);
        return NULL;
    }
    
    new_line->type = Line->type;
    new_line->number = Line->number;
    strcpy(new_line->description, Line->description);
    new_line->price = Line->price;
    new_line->stations = Line->stations;
    new_line->cpyFunc = Line->cpyFunc;
    new_line->freeFunc = Line->freeFunc;
    new_line->printFunc = Line->printFunc;
    return new_line;    
}

void freeLine(ListElement element)
{
    ScheduleLine Line = (ScheduleLine)element;
    if (Line!= NULL)
    {
        ListResult res = linkedListDestroy(Line->stations);
        if (res !=LIST_SUCCESS)
            return;           
        free(Line->description);
        free(Line);
    }
}

void printLine(FILE *out,ListElement element)
{
    ScheduleLine Line = (ScheduleLine)element;
    if (Line == NULL)
        return;

    ScheduleStation stat =NULL;
    int amount = linkedListGetNumElements(Line->stations);
    if(amount > 0)
    {
        linkedListGoToHead(Line->stations);
        ListResult res = linkedListSortElements(Line->stations, compareStationsByTime);
        if(res != LIST_SUCCESS)
            return;
        linkedListGetCurrent(Line->stations,(ListElement)&stat);
        const char *NameFirst = LastFirst(stat);
        do
        {
            linkedListGetCurrent(Line->stations,(ListElement)&stat);
        } while (linkedListGoToNext(Line->stations) == LIST_SUCCESS);
        const char *NameLast = LastFirst(stat);
        schedulePrintLine(out,Line->type, Line->number, Line->description,NameFirst, NameLast, Line->price);
    }
    else
        schedulePrintLine(out,Line->type, Line->number, Line->description,"N/A", "N/A", Line->price);

    
}

int matchLineByNmuber(ListElement element, KeyForListElement key)
{
    ScheduleLine line = (ScheduleLine)element;
    int number = *(int *)key; 
    if (element == NULL || key == NULL)
        return 0;
    return line->number == number;
}


int matchLineByType(ListElement element, KeyForListElement key)
{
    ScheduleLine line = (ScheduleLine)element;
    ScheduleLineType type = *(ScheduleLineType*)key; 
    if (element == NULL || key == NULL)
        return 0;
    return line->type == type;
}

int compareStationsByPrice(ListElement element1, ListElement element2)
{
    ScheduleLine line1 = (ScheduleLine)element1;
    ScheduleLine line2 = (ScheduleLine)element2;
    if (line1 == NULL || line2 == NULL || line1->price == line2->price)
        return 0;
    return line1->price > line2->price ? 1:-1;
}



ScheduleLine schedule_line_create(ScheduleLineType type, int number, const char *description, double price)
{ 
    ScheduleLine new_line = (ScheduleLine)malloc(sizeof(Schedule_line_t));
    if (new_line == NULL)
        return NULL;
    
    ListResult res = linkedListCreate(&(new_line->stations), copyStation ,freestation ,printstation);
    if (res != LIST_SUCCESS)
    {
        free(new_line);
        return NULL;
    }
    
    new_line->description =(char*)malloc((strlen(description)+1)*sizeof(char));
    if (new_line->description == NULL)
    {
        free(new_line->stations);
        free(new_line);
        return NULL;
    }
    
    new_line->type =type;
    new_line->number = number;
    strcpy(new_line->description,description);
    new_line->price = price;
    new_line->cpyFunc = copyLine;
    new_line->freeFunc = freeLine;
    new_line->printFunc = printLine;
    return new_line;
}

ScheduleLineResult add_station_to_line(ScheduleLine line, const char *station, int time)
{
    int index = linkedListGetNumElements(line->stations);
    ScheduleStation stat = schedule_station_create(index, station,time);
    if (stat == NULL)
        return SCHEDULE_LINE_OUT_OF_MEMORY;
    ListResult res = linkedListInsertLast(line->stations, stat);
    if (res != LIST_SUCCESS)
        return SCHEDULE_LINE_OUT_OF_MEMORY;
    return SCHEDULE_LINE_SUCCESS;
}


ScheduleLineResult remove_station_from_line(ScheduleLine line, int index)
{
    linkedListGoToHead(line->stations);
    if(index == -1)
    {
        while (linkedListGoToNext(line->stations) == LIST_SUCCESS);
    }
    else
    {
        linkedListGoToHead(line->stations);
        ListResult res = linkedListFind(line->stations, &index, matchStationByIndex);  
        if (res != LIST_SUCCESS)
            return SCHEDULEL_STATION_DOESNT_EXIST;
    }
            
    linkedListRemoveCurrent(line->stations);
    return SCHEDULE_LINE_SUCCESS;  
}


ScheduleLineResult print_station_Time(ScheduleLine line)
{
    if(line->stations == NULL)
        return SCHEDULEL_STATION_DOESNT_EXIST;
    line->printFunc(stdout,line);
    int amount = linkedListGetNumElements(line->stations);
    linkedListPrint(line->stations, stdout, amount);
    return SCHEDULE_LINE_SUCCESS;
} 

ScheduleLineResult station_from_to(ScheduleLine line,const char *from,const char *to)
{
    ListResult res;
    if(line->stations != NULL)
    {
        res = linkedListSortElements(line->stations, compareStationsByTime);
        if(res != LIST_SUCCESS)
            return SCHEDULE_LINE_BAD_ARGUMENTS;
        linkedListGoToHead(line->stations);
        do
        {
            res = linkedListFind(line->stations,(KeyForListElement)from,matchLineByDescription);
            if(res == LIST_SUCCESS)
            {
                res = linkedListFind(line->stations,(KeyForListElement)to,matchLineByDescription);
                line->printFunc(stdout, line);
            }
        } while (linkedListGoToNext(line->stations) == LIST_SUCCESS); 
        
        return SCHEDULE_LINE_SUCCESS;
    } 
    return SCHEDULE_LINE_BAD_ARGUMENTS;
}



