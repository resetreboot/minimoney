#include <PalmOS.h>
#include "../PalmTypes.h"
#include "../uiResourceIDs.h"
#include "../utils.h"
#include "../db/db.h"
#include "../main.h"

static void drawList(Int16 i, RectangleType *bounds, Char **items) {
    DBRecordTypePtr rec = SelectRecord(i);
    if (rec != NULL) {
        WinDrawTruncChars (rec->concept, StrLen(rec->concept),
                bounds->topLeft.x,
                bounds->topLeft.y,
                bounds->extent.x-60);

        char *amount = MemPtrNew(sizeof(Int16) * 10);
        char *amountWithPoint = MemPtrNew(sizeof(Int16) * 11);
        char *amountText = MemPtrNew(sizeof(Int16) * 13);

        if (rec->type) {
            StrIToA(amount, rec->amount);
        } else {
            StrIToA(amount, rec->amount * -1);
        }
        AddPoint(amountWithPoint, amount);
        AddCurrency(amountText, amountWithPoint);

        Int16 width = FntLineWidth(amountText, StrLen(amountText));

        WinDrawTruncChars (amountText, StrLen(amountText),
                bounds->topLeft.x+(156-width),
                bounds->topLeft.y,
                60);
        MemPtrFree(amount);
        MemPtrFree(amountWithPoint);
        MemPtrFree(amountText);
    }
}

static void setupList(int lIndex) {
    FormPtr pForm   = FrmGetActiveForm();
    void    *pList  = getObjectPtr(pForm, lIndex);
    UInt16  numRecords = NumRecords();

    LstSetListChoices (pList, 0, numRecords);
    LstSetDrawFunction (pList, (ListDrawDataFuncPtr) drawList);

    // Since the list is already showing, we have to redraw it
    LstDrawList (pList);
}

/*
 * Make sure we show the totals
 */
void calculateTotals(FormPtr pForm) {
    UInt16  numRecords = NumRecords();
    Int32 total = 0;
    for (UInt16 i=0; i < numRecords; i++) {
        DBRecordTypePtr rec = SelectRecord(i);
        if (rec->type) {
            total += rec->amount;
        } else {
            total -= rec->amount;
        }
    }

    if (total != 0) {
        char *text = MemPtrNew(sizeof(Int16) * 10);
        char *temp = MemPtrNew(sizeof(Int16) * 8);

        StrIToA(text, total);
        AddPoint(temp, text);
        AddCurrency(text, temp);

        SetField(MainForm, TotalField, text);

        MemPtrFree(temp);
        MemPtrFree(text);
    } else {
        SetField(MainForm, TotalField, "0.00");
    }
}

// Main Form functions

/* Initialize the Entries list
 *
 */
void appFormInit(FormPtr pForm) {
    setupList(MoneyEntriesList);
    calculateTotals(pForm);
}

/*
 * This is the event handler for the main form.  It handles all of
 * the user interactions with the user interface objects (e.g.,
 * buttons, lists, text fields, and such) on the main form.
 */

Boolean appFormEventHandler(EventPtr pEvent) {
    Boolean	handled	= false;
    FormPtr	pForm	= FrmGetActiveForm();
    void    *pList  = getObjectPtr(pForm, ConceptList);
    Int16 selection = LstGetSelection(pList);
    // FormType * frmP;
    switch (pEvent->eType)
    {

    /*
     * the first event received by a form's event handler is
     * the frmOpenEvent.
     */

    case frmOpenEvent:
        FrmDrawForm(pForm);
        appFormInit(pForm);
        handled = true;
        break;

    // *** ADD EVENT HANDLING HERE *** //
    case ctlSelectEvent:
        switch (pEvent->data.ctlSelect.controlID) {
            case AppMainNewButton:
                FrmInitForm(NewEntryForm);
                FrmGotoForm(NewEntryForm);
                break;
            case AppMainDeleteButton:
                if (selection != noListSelection) {
                    DeleteRecord(selection);
                    LstSetSelection(pList, noListSelection);
                    LstSetListChoices (pList, 0, NumRecords());
                    LstDrawList(pList);
                    calculateTotals(pForm);
                }
                break;

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
