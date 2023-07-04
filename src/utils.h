
#ifndef UTILS_H
#define UTILS_H

void *getObjectPtr(FormPtr pForm, Int16 resourceNo);
void SetField(UInt16 formID, UInt16 fieldID, MemPtr str);
char* GetField(UInt16 formID, UInt16 fieldID);
void AddPoint(char *result, char *source);
Int32 ProcessAmount(char *s);
void AddCurrency(char *result, char *source);

#endif
