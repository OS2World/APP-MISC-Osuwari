/*
 * ostext.h - UTF-8 language files and Unicode text for Osuwari Multi
 *
 * The texts come from lang\*.txt next to the program (see ulang.c); they are
 * drawn through GPI code page 1208 (see uni.c).  Same technique as MakMan/2.
 * Include after <os2.h>.
 */

#ifndef _OSTEXT
#define _OSTEXT

/* text ids; the order must match str_keys[] in ostext.c */
enum {
    STR_MENU_OPTION = 0,
    STR_MENU_LANGUAGE,
    STR_MENU_ABOUT,
    STR_MENU_EXIT,
    STR_SET_TITLE,
    STR_SET_MSG,
    STR_SET_SND,
    STR_SET_POS,
    STR_BTN_OK,
    STR_BTN_CANCEL,
    STR_ABOUT_TITLE,
    STR_ABOUT_TEXT,
    STR_ERR_BITMAPS,
    STR_ERR_WINDOWS,
    STR_B_NADE,
    STR_B_FUNI,
    STR_B_PSYU,
    STR_B_NADE1,
    STR_B_NADE2,
    STR_B_NADE3,
    STR_B_NADE4,
    STR_B_FUNI1,
    STR_B_FUNI2,
    STR_B_FUNI3,
    STR_B_FUNI4,
    STR_COUNT
};

/* class of the text controls used in the dialogs */
#define OS_LABEL_CLASS  "OsUniText"

/*
 * Loads lang\*.txt and selects the language: a language code on the command
 * line (ja, en, ...), else the saved one, else one that fits the code page.
 */
void        osLangInit(HAB hab, const char *saved, int ac, char *av[]) ;
void        osLangDone(void) ;

/* text in the process code page (English when it cannot show the language) */
const char *osTr(int id) ;
/* text as UTF-8, *n = bytes */
const char *osTrw(int id, int *n) ;

const char *osLangCode(void) ;          /* "en", "ja", ... */
int         osLangCount(void) ;
int         osLangIndex(void) ;
void        osLangSelect(int idx) ;     /* then call osPopupApply() */
const char *osFace(void) ;              /* font that has the glyphs of the language */

/* put the texts into the popup menu (items, Language submenu, check marks) */
void        osPopupApply(HWND hwndPopup) ;
/* WM_MEASUREITEM / WM_DRAWITEM for owner drawn menu items */
MRESULT     osOwnerMsg(ULONG msg, MPARAM mp2, BOOL *handled) ;

/* BN_PAINT of an owner drawn button */
void        osDrawButton(PUSERBUTTON ub, int strId) ;
/* give a label control (class OS_LABEL_CLASS) its text; buddy = control that
   gets a click on the label (0 = none) */
void        osSetLabel(HWND hwndDlg, USHORT id, int strId, int center, USHORT buddy) ;

#endif  /* _OSTEXT */
