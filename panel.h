#ifndef _EVENTS_H_
#define _EVENTS_H_

#include "graph.h"
#include <windows.h>

void CreateMenu1(HWND parent);
void ProcessMouseClick(HWND parent, unsigned px, unsigned py);
void ProcessButton(HWND parent, UINT button);
void SetTextField(HWND field);

#endif