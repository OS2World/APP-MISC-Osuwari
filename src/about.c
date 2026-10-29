/*
 * about.c - about dialog
 */

#include <stdio.h>
#include <stdlib.h>

#define INCL_PM
#include <os2.h>

#include "osmulti2.h"
#include "osmulres.h"

extern void dialogAtMouse(HWND hwndDialog) ;

/*
 * procAbout - dialog procedure for about dialog
 */

static MRESULT EXPENTRY procAbout(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    switch (msg) {

    case WM_INITDLG :
        WinSetWindowText(hwnd, (PSZ) osTr(STR_ABOUT_TITLE)) ;
        osSetLabel(hwnd, IDD_ADESC, STR_ABOUT_TEXT, 0, 0) ;
        dialogAtMouse(hwnd) ;
        return (MRESULT) 0 ;

    case WM_CONTROL :
        if (SHORT2FROMMP(mp1) == BN_PAINT && SHORT1FROMMP(mp1) == DID_OK) {
            osDrawButton((PUSERBUTTON) PVOIDFROMMP(mp2), STR_BTN_OK) ;
        }
        return (MRESULT) 0 ;

    case WM_COMMAND :
        switch (SHORT1FROMMP(mp1)) {
        case DID_OK :
            WinDismissDlg(hwnd, DID_OK) ;
            return (MRESULT) 0 ;
        case DID_CANCEL :
            WinDismissDlg(hwnd, DID_CANCEL) ;
            return (MRESULT) 0 ;
        default :
            return (MRESULT) 0 ;
        }
    }
    return WinDefDlgProc(hwnd, msg, mp1, mp2) ;
}

/*
 * aboutDialog - entry of about dialog
 */

void    aboutDialog(void)
{
    WinDlgBox(HWND_DESKTOP, NULLHANDLE,
                procAbout, NULLHANDLE, IDD_ABOUT, NULL) ;
}
