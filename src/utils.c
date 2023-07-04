#include <PalmOS.h>
#include "PalmTypes.h"
#include "utils.h"

void *getObjectPtr(FormPtr pForm, Int16 resourceNo) {
    UInt16 objIndex = FrmGetObjectIndex(pForm, resourceNo);
    return FrmGetObjectPtr(pForm, objIndex);
}

void SetField(UInt16 formID, UInt16 fieldID, MemPtr str) {
    FormPtr 	frm;
	UInt16		obj;
	void	    *fld;
	char*		p;
	MemHandle	h;

	frm = FrmGetFormPtr(formID);
	obj = FrmGetObjectIndex(frm, fieldID);
	fld = FrmGetObjectPtr(frm, obj);
	h = (MemHandle)FldGetTextHandle(fld);

	if (h == NULL)
	{
		h = MemHandleNew (FldGetMaxChars(fld)+1);
		ErrFatalDisplayIf(!h, "No Memory");
	}

	p = (char *)MemHandleLock(h);
	StrCopy(p, str);
	MemHandleUnlock(h);

	FldSetTextHandle(fld, h);
    FldDrawField(fld);
}

char* GetField(UInt16 formID, UInt16 fieldID) {
    FormPtr 	frm;
	FieldPtr	fld;
	UInt16		obj;
	Char		*p;

	frm = FrmGetFormPtr(formID);
	obj = FrmGetObjectIndex(frm, fieldID);
	fld = (FieldPtr)FrmGetObjectPtr(frm, obj);
	p = FldGetTextPtr(fld);
	return (p ? p : "");
}

void AddPoint(char *result, char *source) {
    Int16 length = StrLen(source);
    Int16 i = 0;

    while (i<=length) {
        if (i < length - 2) {
            result[i] = source[i];
        } else if (i == length - 2) {
            result[i] = '.';
            result[i+1] = source[i];
        } else {
            result[i+1] = source[i];
        }
        i++;
    }
}

void RemovePoint(char *dest, char *source) {
    Int16 length = StrLen(source);
    Boolean pointFound = false;
    Int16 i = 0;

    while(i < length) {
        if (!pointFound) {
            if (source[i] != '.') {
                dest[i] = source[i];
            } else {
                pointFound = true;
            }
        } else {
            dest[i-1] = source[i];
        }
        i++;
    }
}

Int32 ProcessAmount(char *s) {
    char *processed = MemPtrNew(sizeof(Int16) * StrLen(s));
    RemovePoint(processed, s);
    Int32 multiplier = 1;
    if (StrLen(processed) == StrLen(s)) {
        // We didn't need to remove the decimal point
        // So we multiply by 100
        multiplier = 100;
    }
    Int32 res = StrAToI(processed) * multiplier;
    MemPtrFree(processed);
    return res;
}

// TODO: Preferences should give us the currency sign
void AddCurrency(char *result, char *source) {
    StrCopy(result, source);
    StrCat(result, " $");
}

void GetCurrentDateTime(DateTimeType *datetime) {
    Int32 timestamp = TimGetSeconds();
    TimSecondsToDateTime(timestamp, datetime);
}
