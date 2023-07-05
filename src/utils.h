#include "PalmTypes.h"
#ifndef UTILS_H
#define UTILS_H

#define NUM_GLOBALS_SLOTS	8

register void** a5 asm("a5");

static inline void** globalsSlotPtr(UInt8 slotID)	//[0] is reserved
{
	if (!slotID || slotID > NUM_GLOBALS_SLOTS)
		return NULL;

	return a5 + slotID;
}

static inline void* globalsSlotVal(UInt8 slotID)	//[0] is reserved
{
	if (!slotID || slotID > NUM_GLOBALS_SLOTS)
		return NULL;

	return a5[slotID];
}

#define GLOBALS_DB_REF            1
#define GLOBALS_CURRENT_DATE      2


void *getObjectPtr(FormPtr pForm, Int16 resourceNo);
void SetField(UInt16 formID, UInt16 fieldID, MemPtr str);
char* GetField(UInt16 formID, UInt16 fieldID);
void AddPoint(char *result, char *source);
Int32 ProcessAmount(char *s);
void AddCurrency(char *result, char *source);
void GetCurrentDateTime(DateTimeType *datetime);

#endif
