#include <PalmOS.h>
#include "uiResourceIDs.h"
#include "main.h"
#include "forms/appform.h"
#include "forms/newform.h"
#include "db/db.h"

#define ROM_VERSION_REQUIRED	0x02000000		// This application requires PalmOS 2.0 or later


/*
 * startApp and stopApp are here for future reference.  They clearly
 * don't do anything for this program, but it's a good idea to do
 * program clean-up and shutdown in these files.  One thing that
 * typically goes here is database opening and closing.
 */

static void startApp() {return;}
static void stopApp() {
    DmOpenRef db = DBOpen();
    if (db != NULL) {
        DmCloseDatabase(db);
    }
    FtrUnregister(CreatorID, featDB);
    FrmCloseAllForms();
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

        switch (formId) {
            case MainForm:
                FrmSetEventHandler (pForm, appFormEventHandler);
                break;

            case NewEntryForm:
                FrmSetEventHandler (pForm, newFormEventHandler);
                break;
        }

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
        pForm = FrmGetActiveForm();
        handled = doAppMenu(pForm, pEvent->data.menu.itemID);
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

        startApp();

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

        stopApp();
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
