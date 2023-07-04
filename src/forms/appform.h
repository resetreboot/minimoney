/*
 * AppForm header file
 */

#ifndef APPFORM_H
#define APPFORM_H

void appFormInit(FormPtr pForm);
Boolean appFormEventHandler(EventPtr pEvent);
Boolean doAppMenu(FormPtr pForm, UInt16 command);

#endif
