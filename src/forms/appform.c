#include <PalmOS.h>
#include "../uiResourceIDs.h"
#include "../main.h"

#define listCount 5

static void drawList(Int16 i, RectangleType *bounds, Char **items) {
    WinDrawTruncChars ("test", 4,
            bounds->topLeft.x,
            bounds->topLeft.y,
            bounds->extent.x);
}

static void setupList(int lIndex) {
    FormPtr pForm   = FrmGetActiveForm();
    void    *pList  = getObjectPtr(pForm, lIndex);
    LstSetListChoices (pList, 0, 4);
    LstSetDrawFunction (pList, (ListDrawDataFuncPtr) drawList);

    // Since the list is already showing, we have to redraw it
    LstDrawList (pList);
}

// Main Form functions

/* Initialize the Main form
 *
 */
void appFormInit(FormPtr pForm) {
    setupList(MoneyEntriesList);
}

/*
 * This is the event handler for the main form.  It handles all of
 * the user interactions with the user interface objects (e.g.,
 * buttons, lists, text fields, and such) on the main form.
 */

Boolean appFormEventHandler(EventPtr pEvent) {
    Boolean	handled	= false;
    FormPtr	pForm	= FrmGetActiveForm();
    FormType * frmP;
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
                frmP = FrmInitForm(NewEntryForm);
                FrmDoDialog (frmP);
                FrmDeleteForm (frmP);
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
