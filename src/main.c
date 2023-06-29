#include <PalmOS.h>
#include "uiResourceIDs.h"
#include "main.h"


void *getObjectPtr(FormPtr pForm, Int16 resourceNo) {
    UInt16 objIndex = FrmGetObjectIndex(pForm, resourceNo);
    return FrmGetObjectPtr(pForm, objIndex);
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
            FrmSetEventHandler (pForm, appFormEventHandler);


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
