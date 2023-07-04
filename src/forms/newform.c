#include <PalmOS.h>
#include "../PalmTypes.h"
#include "../uiResourceIDs.h"
#include "../utils.h"
#include "../db/db.h"
#include "../main.h"
#include "newform.h"

DBRecordType readFormData(FormPtr pForm) {
    DBRecordType record;
    void         *pList;

    pList = getObjectPtr(pForm, EntryTypeList);

    StrCopy(record.concept, GetField(NewEntryForm, ConceptField));
    // TODO: Check we have numbers here!
    record.amount = ProcessAmount(GetField(NewEntryForm, AmountField));

    // If type is true, is a deposit, if not, an expense
    record.type = (LstGetSelection(pList) == 1);
    StrCopy(record.date, "01/10/2023");
    StrCopy(record.time, "01:10");

    return record;
}

// New Form functions

/* Initialize the form
 *
 */
void newFormInit(FormPtr pForm) {
    DateTimeType dt;
    char *text = MemPtrNew(sizeof(Int16)*9);

    GetCurrentDateTime(&dt);
    StrPrintF(text, "%02d/%02d/%02d", dt.day, dt.month, dt.year % 100);
    SetField(NewEntryForm, DateField, text);
    MemPtrFree(text);
}

/*
 * This is the event handler for the main form.  It handles all of
 * the user interactions with the user interface objects (e.g.,
 * buttons, lists, text fields, and such) on the main form.
 */

Boolean newFormEventHandler(EventPtr pEvent) {
    Boolean	handled	= false;
    FormPtr	pForm	= FrmGetActiveForm();

    switch (pEvent->eType)
    {

        /*
         * the first event received by a form's event handler is
         * the frmOpenEvent.
         */

        case frmOpenEvent:
            FrmDrawForm(pForm);
            newFormInit(pForm);
            handled = true;
            break;

        // *** ADD EVENT HANDLING HERE *** //
        case ctlSelectEvent:
            switch (pEvent->data.ctlSelect.controlID) {
                case OkNewButton: {
                    DBRecordType rec = readFormData(pForm);
                    InsertRecord(rec);
                    // Now that the record is stored, go back to the
                    // Main app form
                    FrmInitForm(MainForm);
                    FrmGotoForm(MainForm);
                    break;
                }
                case CancelNewButton:
                    // Come back to the main form after not
                    // inserting anything
                    FrmInitForm(MainForm);
                    FrmGotoForm(MainForm);
                    break;
                case DateButton: {
                    DateTimeType datetime;
                    GetCurrentDateTime(&datetime);
                    Int16 month = datetime.month;
                    Int16 day = datetime.day;
                    Int16 year = datetime.year;

                    if(SelectDay(selectDayByDay,
                                 &month,
                                 &day,
                                 &year, "Select date")) {
                        char *text = MemPtrNew(sizeof(Int16)*9);
                        year = year % 100;
                        StrPrintF(text, "%02d/%02d/%02d", day, month, year);
                        SetField(NewEntryForm, DateField, text);
                        MemPtrFree(text);
                    }
                }
            // Add other buttons here!
            }
            break;

        default:
            break;
    }
    return handled;
}

/*
 * This is the menu handler for the main form.  It handles
 doing the users selections. It takes a FormPtr for fast
 access to the form it's 'installed' for, and so that if something
 changes the active form between now and the end of the
 function it still works on the form it's designed for, altough
 that is rare.*/

/*
Boolean doAppMenu(FormPtr pForm, UInt16 command) {
    Boolean	handled	= false;
    FormType * frmP;
    switch(command)
    {
        case AboutMenu:
            frmP = FrmInitForm (AboutDialog);
	        FrmDoDialog (frmP);
            FrmDeleteForm (frmP);
            handled = true;
            break;

        case NewMenu:
            frmP = FrmInitForm(NewEntryForm);
            FrmDoDialog (frmP);
            FrmDeleteForm (frmP);
            handled = true;
            break;

    }
    return handled;
}
*/
