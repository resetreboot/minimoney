#include <PalmOS.h>
#include "db.h"
#include "../utils.h"
#include "../uiResourceIDs.h"
#include "../main.h"


DmOpenRef getDatabaseHandler() {
    DmOpenRef db;
    Err errno;
    UInt32 data;

    if (!(db = globalsSlotVal(GLOBALS_DB_REF))) {
        return 0;
    }
    return db;
}

DmOpenRef DBOpen() {
	Err		    retcode = 0;
    DmOpenRef   db;

    db = getDatabaseHandler();   // Try to retrieve from the features (used as globals here!)

    if (db != 0) {
        return db;
    }

	// Find the application's data file.  If it doesn't exist create it.
    db = DmOpenDatabaseByTypeCreator(DBType, CreatorID, dmModeReadWrite);
	if (db == 0) {
        if ((retcode = DmCreateDatabase(0, DBName, CreatorID, DBType, false))) {
            FrmCustomAlert(ErrorAlert, "Unable to create the dabatase", "", "");
            return NULL;
        }

        db = DmOpenDatabaseByTypeCreator(DBType, CreatorID, dmModeReadWrite);
        if (db == 0) {
            FrmCustomAlert(ErrorAlert, "Unable to open the dabatase after creation", "", "");
            return NULL;
        }

	}

    *globalsSlotPtr(GLOBALS_DB_REF) = db;
    return db;
}

UInt16 NumRecords() {
    DmOpenRef db = DBOpen();

    if (db != NULL) {
        return DmNumRecords(db);
    } else {
        return 0;
    }
}

DBRecordTypePtr SelectRecord(UInt16 index) {
	MemHandle			recH;
	DBRecordTypePtr		recP = NULL;
	UInt16				attr;
	UInt32				uniqueID;
	char				buf[20];
    DmOpenRef           db;

    db = DBOpen();

    if (db == NULL) {
        FrmCustomAlert(ErrorAlert, "Unable to open database", "", "");
    }

	if ((recH = (MemHandle)DmQueryRecord(db, index))) {
		if (DmRecordInfo(db, index, &attr, &uniqueID, NULL)) {
			FrmCustomAlert(ErrorAlert, "Unable to get record info", "", "");
			return NULL;
		}

		recP = (DBRecordTypePtr)MemHandleLock(recH);
		MemHandleUnlock(recH);
	}
    return recP;
}

void UpdateRecord(UInt16 index, DBRecordType dbRecord) {
	MemHandle			recH;
	MemPtr				recP;
    UInt16              attr;
	char				buf[20];
    DmOpenRef           db;

    db = DBOpen();

	if (DmRecordInfo(db, index, &attr, NULL, NULL)) {
		FrmCustomAlert(ErrorAlert, "Unable to get record info", "", "");
		return;
	}

	if ((attr & dmRecAttrBusy) != 0) {
		FrmCustomAlert(ErrorAlert, "Record is locked", "", "");
		return;
	}

	if ((recH = (MemHandle)DmGetRecord(db, index))) {
		recP = MemHandleLock(recH);
		DmWrite(recP, 0, &dbRecord, sizeof(DBRecordType));
		MemPtrUnlock(recP);
		DmReleaseRecord(db, index, true);
	}
	// TODO: else FrmCustomAlert(ErrorAlert, "Unable to get record", "", "");
}


UInt16 DeleteRecord(UInt16 index) {
	UInt16		attr;
	Err			error;
    DmOpenRef           db;

    db = DBOpen();

	if (DmRecordInfo(db, index, &attr, NULL, NULL)) {
		FrmCustomAlert(ErrorAlert, "Unable to get record info", "", "");
        return -1;
	}

	if ((attr & dmRecAttrDelete) == 0) {
		if ((error = DmRemoveRecord(db, index)) != 0) { // delete OK: we need to find a valid "current" rec, forwards OR backwards
            ErrAlert(error);
        }
    } else {
        FrmCustomAlert(ErrorAlert, "Record already deleted", "", "");
    }

    return -1;
}


void LockRecord(Int16 index) {
	MemHandle			recH;
	UInt16		attr;
    DmOpenRef           db;

    db = DBOpen();

	if (DmRecordInfo(db, index, &attr, NULL, NULL)) {
		FrmCustomAlert(ErrorAlert, "Unable to get record info", "", "");
		return;
	}

	if ((attr & dmRecAttrBusy) == 0) {
		if ((recH = (MemHandle)DmGetRecord(db, index))) {
			SelectRecord(index);
        } else {
            ErrAlert(DmGetLastErr());
        }
    } else {
        FrmCustomAlert(ErrorAlert, "Record already locked", "", "");
    }
}


void UnlockRecord(UInt16 index) {
	UInt16		attr;
    DmOpenRef           db;

    db = DBOpen();

	if (DmRecordInfo(db, index, &attr, NULL, NULL)) {
		FrmCustomAlert(ErrorAlert, "Unable to get record info", "", "");
		return;
	}

	if ((attr & dmRecAttrBusy) != 0) {
		if (DmReleaseRecord(db, index, true) == errNone) {
			SelectRecord(index);
        } else {
            FrmCustomAlert(ErrorAlert, "Unable to unlock record", "", "");
        }
    } else {
        FrmCustomAlert(ErrorAlert, "Record not locked", "", "");
    }
}


void InsertRecord(DBRecordType dbRecord) {
	UInt16		index;
	MemHandle 	recH;
	MemPtr		recP;
    DmOpenRef   db;

    db = DBOpen();
    if (db == 0) {
        FrmCustomAlert(ErrorAlert, "Unable to open database", "", "");
        return;
    }

	// Add a new record at the end of the database.
	index = DmNumRecords(db);
	if (!(recH = DmNewRecord(db, &index, sizeof(DBRecordType)))) {
		FrmCustomAlert(ErrorAlert, "Unable to create record", "", "");
		return;
	}

	// StrCopy(dbRecord.name, GetField(InsertForm, InsertName));
	// dbRecord.quantity = StrAToI(GetField(InsertForm, InsertQuantity));

	recP = MemHandleLock(recH);
	DmWrite(recP, 0, &dbRecord, sizeof(DBRecordType));
	MemPtrUnlock(recP);

	DmReleaseRecord(db, index, true);
}
