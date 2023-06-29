#include <PalmOS.h>

#define MainForm 1000
#define AboutDialog 8000
#define AboutMenu 1001
#define MoneyEntriesList 1020

#define listCount 5


void *getObjectPtr(FormPtr pForm, Int16 resourceNo) {
    UInt16 objIndex = FrmGetObjectIndex(pForm, resourceNo);
    return FrmGetObjectPtr(pForm, objIndex);
}

static void drawList(Int16 i, RectangleType *bounds, Char **items) {
    Char *listString[listCount] = {"one", "two", "three", "four", "five"};
    WinDrawTruncChars (listString[i], StrLen(listString[i]),
            bounds->topLeft.x,
            bounds->topLeft.y,
            bounds->extent.x);
}

static void setupList(int lIndex) {
    FormPtr pForm   = FrmGetActiveForm();
    void    *pList  = getObjectPtr(pForm, lIndex);
    LstSetListChoices (pList, 0, listCount);
    LstSetDrawFunction (pList, (ListDrawDataFuncPtr) drawList);

    // Since the list is already showing, we have to redraw it
    LstDrawList (pList);
}

// Main Form functions

/* Initialize the Main form
 *
 */
static void mainFormInit(FormPtr pForm) {
    setupList(MoneyEntriesList);
}

/*
 * This is the event handler for the main form.  It handles all of
 * the user interactions with the user interface objects (e.g.,
 * buttons, lists, text fields, and such) on the main form.
 */

static Boolean mainFormEventHandler(EventPtr pEvent) {
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
        mainFormInit(pForm);
        handled = true;
        break;


    // *** ADD EVENT HANDLING HERE *** //


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

static Boolean doMainMenu (FormPtr pForm, UInt16 command) {
    Boolean	handled	= false;
    FormType * frmP;
    switch(command)
    {
        case AboutMenu:
            frmP = FrmInitForm (AboutDialog);
	        FrmDoDialog (frmP);
            FrmDeleteForm (frmP);
            break;

    }
    return handled;
}

static Boolean appHandleEvent(EventPtr pEvent) {
    FormPtr	pForm;
    Int16	formId;
    Boolean	handled = false;

    if (pEvent->eType == frmLoadEvent)
    {
        /*
        * Load the resource for the form
        */

        formId	= pEvent->data.frmLoad.formID;
        pForm	= FrmInitForm(formId);

        FrmSetActiveForm(pForm);

        /*
        * install a form-specific event handler
        */

        if (formId == MainForm)
            FrmSetEventHandler (pForm, mainFormEventHandler);


        // *** ADD NEW FORM HANDLING HERE *** //
        handled = true;
    }

    /*
     * If it is a menu item, follow a sub-control path, in this case
     * doMainMenu although remember that you need a seperate menu handler
     * function for each menu bar, and then must include the line to call
     * it in the form handler (i.e. this) for each form that has that menu
     * bar.
     */

    else if (pEvent->eType == menuEvent)
    {
        handled = doMainMenu(pForm, pEvent->data.menu.itemID);
    }
    return handled;
}

UInt32 __attribute__((noinline)) PilotMain(UInt16 cmd, MemPtr cmdPBP, UInt16 launchFlags) {
	EventType event;
    UInt16    error;
	FormType * frmP;

	/* Clear the menu status from the display */
	MenuEraseStatus(0);

	/* Display the About Box. */
	// frmP = FrmInitForm (MainForm);
	// FrmDoDialog (frmP);
	// FrmDeleteForm (frmP);

	if (sysAppLaunchCmdNormalLaunch == cmd) {

        /*
         * FrmGotForm generates a frmLoadEvent that'll get
         * handled as soon as we have an event handler that
         * knows what to do with it.
         */

        FrmGotoForm(MainForm);

		do {
			EvtGetEvent(&event, evtWaitForever);

			/*
             * Then, ask the system, the menu system,
             * and our *OWN* event handlers (one for the
             * application as a whole and one for the
             * current form) to deal with the event.
             */

            if (!SysHandleEvent (&event))
            if (!MenuHandleEvent (0, &event, &error))
            if (!appHandleEvent (&event))
                FrmDispatchEvent (&event);

		} while (event.eType != appStopEvent);

        FrmCloseAllForms();
	}

	return 0;
}

UInt32 __attribute__((section(".vectors"), used)) __Startup__(void);
UInt32 __attribute__((section(".vectors"), used)) __Startup__(void)
{
	SysAppInfoPtr appInfoP;
	void *prevGlobalsP;
	void *globalsP;
	UInt32 ret;

	SysAppStartup(&appInfoP, &prevGlobalsP, &globalsP);
	ret = PilotMain(appInfoP->cmd, appInfoP->cmdPBP, appInfoP->launchFlags);
	SysAppExit(appInfoP, prevGlobalsP, globalsP);

	return ret;
}
