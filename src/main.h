/*
 * Main header file
 */

#ifndef MAIN_H
#define MAIN_H

// main.c
void *getObjectPtr(FormPtr pForm, Int16 resourceNo);

// appform.c

void appFormInit(FormPtr pForm);
Boolean appFormEventHandler(EventPtr pEvent);
Boolean doAppMenu(FormPtr pForm, UInt16 command);

#endif
