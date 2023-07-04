#define	CreatorID				'RSET'
#define DBName					"MiniMoneyDB"
#define DBType					'DATA'
#define featDB                  100
#define featDBID                101
#define featDBCard              102

#ifndef DB_H
#define DB_H

typedef struct DBRecordType {
    char  concept[40];
    Int32 amount;
    Boolean type;
    char date[10];
    char time[5];
} DBRecordType;

typedef DBRecordType* DBRecordTypePtr;

DmOpenRef DBOpen();

UInt16 NumRecords();

DBRecordTypePtr SelectRecord(UInt16 index);

void UpdateRecord(UInt16 index, DBRecordType dbRecord);

UInt16 DeleteRecord(UInt16 index);

void LockRecord(Int16 index);

void UnlockRecord(UInt16 index);

void InsertRecord(DBRecordType dbRecord);

#endif
