/*
 * ostext.c - UTF-8 language files and Unicode text for Osuwari Multi
 *
 * Same technique as MakMan/2 (unitext.cpp): texts are read from lang\*.txt
 * (UTF-8), drawn with GPI code page 1208, and menu items / dialog texts that
 * the process code page cannot show are owner drawn or drawn by the label
 * control class OsUniText.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "uni.h"
#include "ulang.h"
#include "osmulres.h"
#include "ostext.h"

#define MENU_PELS   16
#define CHECK_W     18
#define PAD          6
#define LABEL_PELS  14
#define BTN_PELS    14
#define NBUILTIN     4

static const char *builtinFaces[NBUILTIN] = {
    "Times New Roman MT 30", "Droid Sans Combined", "Monotype Sans Duospace WT J",
    "Times New Roman WT J"
} ;

/* keys of the language files, in the order of the STR_* ids */
static const char *str_keys[STR_COUNT] = {
    "MENU_OPTION", "MENU_LANGUAGE", "MENU_ABOUT", "MENU_EXIT",
    "SET_TITLE", "SET_MSG", "SET_SND", "SET_POS", "BTN_OK", "BTN_CANCEL",
    "ABOUT_TITLE", "ABOUT_TEXT", "ERR_BITMAPS", "ERR_WINDOWS",
    "B_NADE", "B_FUNI", "B_PSYU", "B_NADE1", "B_NADE2", "B_NADE3", "B_NADE4",
    "B_FUNI1", "B_FUNI2", "B_FUNI3", "B_FUNI4"
} ;

/* used when no lang\*.txt file can be found */
static const char *builtin_en[STR_COUNT] = {
    "~Options...", "~Language", "~About...", "E~xit",
    "Sitting Multi - options", "show messages", "use sounds", "sitting position",
    "OK", "Cancel",
    "Sitting Multi for OS/2 PM, Version 1.07",
    "Original character by Leaf\n  http://leaf.aquaplus.co.jp/",
    "failed to load Bitmaps", "failed to create Windows",
    "*nade*", "*funi*", "*psyuuuuuu-------*",
    "I'm so happy!", "More nade nade me!", "More, More nade nade me!",
    "More, More, More nade nade me!",
    "..Oh! what ?!..", "......", ".......", "........"
} ;

static LANG     langs[MAX_LANGS] ;
static int      nlang = 0, cur = -1, enIdx = -1 ;
static char     langDir[CCHMAXPATH] ;
static char     face[FACESIZE] = "Times New Roman MT 30" ;

static char     trbuf[STR_COUNT][400] ;
static USHORT   trdone[STR_COUNT] ;
static int      trgen = 1 ;

static HWND     hwndPop = NULLHANDLE ;
static int      langItems = 0 ;

/* ------------------------------------------------------------------ */
/* texts                                                               */
/* ------------------------------------------------------------------ */

static const char *get_text(const char *key, int *n)
{
    const char *p = NULL ;

    if (cur >= 0)         p = lang_get(&langs[cur], key, n) ;
    if (!p && enIdx >= 0) p = lang_get(&langs[enIdx], key, n) ;
    return p ;
}

const char *osTrw(int id, int *n)
{
    const char *p = (id >= 0 && id < STR_COUNT) ? get_text(str_keys[id], n) : NULL ;

    if (!p) {
        p = uni_ascii((id >= 0 && id < STR_COUNT) ? builtin_en[id] : "", n) ;
    }
    return p ;
}

/* process code page text; English when the language needs other characters */
const char *osTr(int id)
{
    int n ;
    const char *p ;

    if (id < 0 || id >= STR_COUNT) {
        return "" ;
    }
    if (trdone[id] == trgen) {
        return trbuf[id] ;
    }
    p = osTrw(id, &n) ;
    if (!uls_representable(p, n) && enIdx >= 0 && enIdx != cur) {
        const char *e = lang_get(&langs[enIdx], str_keys[id], &n) ;
        if (e) p = e ;
    }
    uls_to_cp(p, n, trbuf[id], sizeof(trbuf[id])) ;
    trdone[id] = (USHORT) trgen ;
    return trbuf[id] ;
}

const char *osLangCode(void)
{
    return (cur >= 0) ? langs[cur].code : "en" ;
}

int osLangCount(void)
{
    return nlang ;
}

int osLangIndex(void)
{
    return cur ;
}

const char *osFace(void)
{
    return face ;
}

/* ------------------------------------------------------------------ */
/* font that really contains the characters of the active language     */
/* ------------------------------------------------------------------ */

static void resolve_face(void)
{
    char cand[NBUILTIN + 8][FACESIZE] ;
    int ncand = 0, i, ns = 0, n, k ;
    char sample[900] ;
    const char *p ;
    char tmp[256], *tok ;
    static const int sampleIds[3] = { STR_MENU_OPTION, STR_SET_MSG, STR_B_NADE1 } ;

    if (cur < 0) {
        return ;
    }
    strncpy(tmp, langs[cur].fonts, sizeof(tmp) - 1) ;
    tmp[sizeof(tmp) - 1] = 0 ;
    for (tok = strtok(tmp, ",") ; tok && ncand < 8 ; tok = strtok(NULL, ",")) {
        while (*tok == ' ') tok++ ;
        if (*tok) {
            strncpy(cand[ncand], tok, FACESIZE - 1) ;
            cand[ncand][FACESIZE - 1] = 0 ;
            ncand++ ;
        }
    }
    for (i = 0 ; i < NBUILTIN ; i++) {
        strcpy(cand[ncand++], builtinFaces[i]) ;
    }

    p = langs[cur].name ;
    if (p) {
        n = utf8_align(p, langs[cur].nameLen, 250) ;
        memcpy(sample, p, n) ;
        ns = n ;
    }
    for (k = 0 ; k < 3 ; k++) {
        p = osTrw(sampleIds[k], &n) ;
        n = utf8_align(p, n, 250) ;
        memcpy(sample + ns, p, n) ;
        ns += n ;
    }
    sample[ns] = 0 ;

    for (i = 0 ; i < ncand ; i++) {
        if (uni_covers(cand[i], sample, ns)) {
            strcpy(face, cand[i]) ;
            return ;
        }
    }
    strcpy(face, cand[0]) ;
}

/* ------------------------------------------------------------------ */
/* labels: "~Mnemonic"                                                 */
/* ------------------------------------------------------------------ */

typedef struct {
    char    t[300] ;
    int     n ;         /* bytes of t   */
    int     mn ;        /* byte offset of the mnemonic, -1 = none */
    int     mnLen ;
} PLABEL ;

static void parse_label(const char *lab, PLABEL *o)
{
    int i = 0, k = 0 ;

    o->mn = -1 ;
    o->mnLen = 0 ;
    while (lab[i]) {
        if (lab[i] == '~' && o->mn < 0 && lab[i + 1]) {
            int used = 0 ;
            utf8_decode(lab + i + 1, (int) strlen(lab + i + 1), &used) ;
            o->mn = k ;
            o->mnLen = used ;
            i++ ;
            continue ;
        }
        if (k < 290) {
            o->t[k++] = lab[i] ;
        }
        i++ ;
    }
    o->t[k] = 0 ;
    o->n = k ;
}

/* the text of a label without mnemonic markers ("~" and "(~X)") */
static int plain_text(const char *lab, int n, char *out, int max)
{
    int i = 0, k = 0 ;

    while (i < n && k < max - 4) {
        if (lab[i] == '(' && i + 3 < n && lab[i + 1] == '~' && lab[i + 3] == ')') {
            i += 4 ;
            continue ;
        }
        if (lab[i] == '~') {
            i++ ;
            continue ;
        }
        out[k++] = lab[i++] ;
    }
    out[k] = 0 ;
    return k ;
}

static const char *label_for(USHORT id, int *n)
{
    if (id >= IDM_LANG_BASE && id < IDM_LANG_BASE + MAX_LANGS) {
        int k = id - IDM_LANG_BASE ;
        if (k < nlang && langs[k].name) {
            if (n) *n = langs[k].nameLen ;
            return langs[k].name ;
        }
        return NULL ;
    }
    switch (id) {
    case IDM_SETUP :        return osTrw(STR_MENU_OPTION,   n) ;
    case IDM_ABOUT :        return osTrw(STR_MENU_ABOUT,    n) ;
    case IDM_EXIT :         return osTrw(STR_MENU_EXIT,     n) ;
    case IDM_SUBMENU_LANG : return osTrw(STR_MENU_LANGUAGE, n) ;
    }
    return NULL ;
}

/* ------------------------------------------------------------------ */
/* popup menu                                                          */
/* ------------------------------------------------------------------ */

static void walk_menu(HWND hm)
{
    SHORT cnt = SHORT1FROMMR(WinSendMsg(hm, MM_QUERYITEMCOUNT, 0, 0)) ;
    SHORT pos ;

    for (pos = 0 ; pos < cnt ; pos++) {
        SHORT id = SHORT1FROMMR(WinSendMsg(hm, MM_ITEMIDFROMPOSITION, MPFROMSHORT(pos), 0)) ;
        MENUITEM mi ;
        const char *lab ;
        int n ;

        if (!WinSendMsg(hm, MM_QUERYITEM, MPFROM2SHORT(id, FALSE), MPFROMP(&mi))) {
            continue ;
        }
        if (mi.afStyle & MIS_SEPARATOR) {
            continue ;
        }
        lab = label_for((USHORT) id, &n) ;
        if (lab) {
            if (!uls_representable(lab, n)) {
                mi.afStyle = (USHORT) ((mi.afStyle & ~MIS_TEXT) | MIS_OWNERDRAW) ;
                mi.hItem   = (ULONG) lab ;
                WinSendMsg(hm, MM_SETITEM, MPFROM2SHORT(0, FALSE), MPFROMP(&mi)) ;
            } else {
                char txt[300] ;
                if (mi.afStyle & MIS_OWNERDRAW) {
                    mi.afStyle = (USHORT) ((mi.afStyle & ~MIS_OWNERDRAW) | MIS_TEXT) ;
                    mi.hItem   = 0 ;
                    WinSendMsg(hm, MM_SETITEM, MPFROM2SHORT(0, FALSE), MPFROMP(&mi)) ;
                }
                uls_to_cp(lab, n, txt, sizeof(txt)) ;
                WinSendMsg(hm, MM_SETITEMTEXT, MPFROMSHORT(id), MPFROMP(txt)) ;
            }
        }
        if (mi.afStyle & MIS_SUBMENU) {
            walk_menu(mi.hwndSubMenu) ;
        }
    }
}

static void rebuild_language_items(HWND hpop)
{
    MENUITEM mi ;
    HWND sub ;
    int i ;

    if (!WinSendMsg(hpop, MM_QUERYITEM, MPFROM2SHORT(IDM_SUBMENU_LANG, TRUE), MPFROMP(&mi))) {
        return ;
    }
    sub = mi.hwndSubMenu ;
    for (i = 0 ; i < MAX_LANGS ; i++) {
        WinSendMsg(sub, MM_DELETEITEM, MPFROM2SHORT(IDM_LANG_BASE + i, FALSE), 0) ;
    }
    for (i = 0 ; i < nlang ; i++) {
        MENUITEM ni ;
        memset(&ni, 0, sizeof(ni)) ;
        ni.iPosition = MIT_END ;
        ni.afStyle   = MIS_TEXT ;
        ni.id        = (USHORT) (IDM_LANG_BASE + i) ;
        WinSendMsg(sub, MM_INSERTITEM, MPFROMP(&ni), MPFROMP(langs[i].english)) ;
    }
    langItems = 1 ;
}

void osPopupApply(HWND hpop)
{
    int i ;

    if (hpop == NULLHANDLE || nlang == 0) {
        return ;
    }
    hwndPop = hpop ;
    if (!langItems) {
        rebuild_language_items(hpop) ;
    }
    walk_menu(hpop) ;
    for (i = 0 ; i < nlang ; i++) {
        WinSendMsg(hpop, MM_SETITEMATTR, MPFROM2SHORT(IDM_LANG_BASE + i, TRUE),
                   MPFROM2SHORT(MIA_CHECKED, (i == cur) ? MIA_CHECKED : 0)) ;
    }
}

/* owner drawn items; PM sends the messages to the owner of the menu */
MRESULT osOwnerMsg(ULONG msg, MPARAM mp2, BOOL *handled)
{
    *handled = FALSE ;
    switch (msg) {
    case WM_MEASUREITEM : {
        POWNERITEM poi = (POWNERITEM) PVOIDFROMMP(mp2) ;
        const char *lab = (const char *) poi->hItem ;
        HPS hps ;
        ULONG ocp ;
        PLABEL pl ;
        int w = 0, h = MENU_PELS + 6 ;

        if (!lab) break ;
        parse_label(lab, &pl) ;
        hps = WinGetPS(poi->hwnd) ;
        if (uni_begin(hps, face, MENU_PELS, &ocp)) {
            w = uni_width(hps, pl.t, pl.n) ;
            h = uni_height(hps) + 4 ;
        }
        uni_end(hps, ocp) ;
        WinReleasePS(hps) ;
        w += 2 * PAD + CHECK_W ;
        poi->rclItem.xLeft = 0 ;
        poi->rclItem.yBottom = 0 ;
        poi->rclItem.xRight = w ;
        poi->rclItem.yTop = h ;
        *handled = TRUE ;
        return MRFROMSHORT((SHORT) h) ;
    }

    case WM_DRAWITEM : {
        POWNERITEM poi = (POWNERITEM) PVOIDFROMMP(mp2) ;
        const char *lab = (const char *) poi->hItem ;
        HPS hps ;
        ULONG ocp ;
        LONG bg, fg ;
        PLABEL pl ;
        POINTL pt ;
        int hil, chk, dis, itemH ;

        if (!lab) break ;
        parse_label(lab, &pl) ;
        hil = (poi->fsAttribute & MIA_HILITED) != 0 ;
        chk = (poi->fsAttribute & MIA_CHECKED) != 0 ;
        dis = (poi->fsAttribute & MIA_DISABLED) != 0 ;
        bg = hil ? SYSCLR_MENUHILITEBGND : SYSCLR_MENU ;
        fg = dis ? SYSCLR_MENUDISABLEDTEXT : (hil ? SYSCLR_MENUHILITE : SYSCLR_MENUTEXT) ;

        hps = WinGetPS(poi->hwnd) ;
        WinFillRect(hps, &poi->rclItem, bg) ;
        GpiSetColor(hps, fg) ;
        itemH = (int) (poi->rclItem.yTop - poi->rclItem.yBottom) ;
        if (uni_begin(hps, face, MENU_PELS, &ocp)) {
            long tx, by ;
            int lineH = uni_height(hps), desc = uni_descender(hps) ;

            tx = poi->rclItem.xLeft + PAD + CHECK_W ;
            by = poi->rclItem.yBottom + (itemH - lineH) / 2 + desc ;
            uni_draw(hps, tx, by, pl.t, pl.n) ;
            if (pl.mn >= 0) {                       /* underline the mnemonic */
                long ux = tx + uni_width(hps, pl.t, pl.mn) ;
                long uw = uni_width(hps, pl.t + pl.mn, pl.mnLen) ;
                pt.x = ux ;      pt.y = by - 2 ; GpiMove(hps, &pt) ;
                pt.x = ux + uw ; pt.y = by - 2 ; GpiLine(hps, &pt) ;
            }
        }
        uni_end(hps, ocp) ;
        if (chk) {                                  /* check mark */
            long x = poi->rclItem.xLeft + 4, y = poi->rclItem.yBottom + itemH / 2 ;
            pt.x = x ;      pt.y = y ;     GpiMove(hps, &pt) ;
            pt.x = x + 4 ;  pt.y = y - 5 ; GpiLine(hps, &pt) ;
            pt.x = x + 11 ; pt.y = y + 5 ; GpiLine(hps, &pt) ;
        }
        WinReleasePS(hps) ;
        poi->fsAttribute    &= ~(MIA_CHECKED | MIA_HILITED | MIA_FRAMED | MIA_DISABLED) ;
        poi->fsAttributeOld &= ~(MIA_CHECKED | MIA_HILITED | MIA_FRAMED | MIA_DISABLED) ;
        *handled = TRUE ;
        return MRFROMLONG(TRUE) ;
    }
    }
    return 0 ;
}

/* ------------------------------------------------------------------ */
/* dialog texts: OsUniText controls and owner drawn buttons            */
/* ------------------------------------------------------------------ */

#define MAXLINES 24

/* draws the (multi line) text s into rc, smaller until it fits */
static void draw_text(HPS hps, RECTL *rc, const char *s, int n, int center, int dis)
{
    char txt[2400] ;
    int tn, start[MAXLINES], len[MAXLINES], nl = 0, i, pels ;
    ULONG ocp ;
    long cw = rc->xRight - rc->xLeft, ch = rc->yTop - rc->yBottom ;

    tn = plain_text(s, n, txt, sizeof(txt)) ;
    for (i = 0, start[0] = 0 ; i <= tn && nl < MAXLINES ; i++) {
        if (i == tn || txt[i] == '\n') {
            len[nl] = i - start[nl] ;
            nl++ ;
            if (i < tn && nl < MAXLINES) {
                start[nl] = i + 1 ;
            }
        }
    }
    GpiSetColor(hps, dis ? SYSCLR_MENUDISABLEDTEXT : SYSCLR_WINDOWSTATICTEXT) ;
    for (pels = LABEL_PELS ; pels >= 9 ; pels-- ) {
        int ok = uni_begin(hps, face, pels, &ocp) ;

        if (ok) {
            int lh = uni_height(hps), desc = uni_descender(hps), maxw = 0, k ;

            for (k = 0 ; k < nl ; k++) {
                int w = uni_width(hps, txt + start[k], len[k]) ;
                if (w > maxw) maxw = w ;
            }
            if ((maxw <= cw && lh * nl <= ch) || pels == 9) {
                for (k = 0 ; k < nl ; k++) {
                    long x = 0, y ;
                    if (center) {
                        x = (cw - uni_width(hps, txt + start[k], len[k])) / 2 ;
                        if (x < 0) x = 0 ;
                    }
                    if (nl == 1) {
                        y = (ch - lh) / 2 + desc ;
                    } else {
                        y = ch - (k + 1) * lh + desc ;
                    }
                    uni_draw(hps, x, y, txt + start[k], len[k]) ;
                }
                uni_end(hps, ocp) ;
                return ;
            }
        }
        uni_end(hps, ocp) ;
        if (!ok) break ;
    }
}

/* window words: 0 = STR id + 1 (0 = none), 4 = centered, 8 = buddy control id */
static MRESULT EXPENTRY LabelProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    if (msg == WM_PAINT) {
        HPS hps = WinBeginPaint(hwnd, NULLHANDLE, NULL) ;
        RECTL rc ;
        ULONG id = WinQueryWindowULong(hwnd, 0) ;

        WinQueryWindowRect(hwnd, &rc) ;
        WinFillRect(hps, &rc, SYSCLR_DIALOGBACKGROUND) ;
        if (id > 0 && id <= STR_COUNT) {
            int n ;
            const char *p = osTrw((int) id - 1, &n) ;
            draw_text(hps, &rc, p, n, (int) WinQueryWindowULong(hwnd, 4),
                      !WinIsWindowEnabled(hwnd)) ;
        }
        WinEndPaint(hps) ;
        return 0 ;
    }
    if (msg == WM_BUTTON1DOWN) {
        ULONG buddy = WinQueryWindowULong(hwnd, 8) ;
        if (buddy) {
            HWND hb = WinWindowFromID(WinQueryWindow(hwnd, QW_PARENT), (USHORT) buddy) ;
            if (hb && WinIsWindowEnabled(hb)) {
                WinSendMsg(hb, BM_CLICK, MPFROMSHORT(TRUE), 0) ;
            }
        }
        return MRFROMLONG(TRUE) ;
    }
    return WinDefWindowProc(hwnd, msg, mp1, mp2) ;
}

void osSetLabel(HWND hwndDlg, USHORT id, int strId, int center, USHORT buddy)
{
    HWND h = WinWindowFromID(hwndDlg, id) ;

    if (!h) {
        return ;
    }
    WinSetWindowULong(h, 0, (ULONG) (strId + 1)) ;
    WinSetWindowULong(h, 4, (ULONG) center) ;
    WinSetWindowULong(h, 8, (ULONG) buddy) ;
    WinInvalidateRect(h, NULL, FALSE) ;
}

void osDrawButton(PUSERBUTTON ub, int strId)
{
    HPS hps = ub->hps ;
    RECTL rc ;
    POINTL pt ;
    ULONG ocp ;
    int hil = (ub->fsState & BDS_HILITED) != 0 ;
    int dis = (ub->fsState & BDS_DISABLED) != 0 ;
    int def = (ub->fsState & BDS_DEFAULT) != 0 ;
    const char *p ;
    char txt[300] ;
    int n, tn ;

    WinQueryWindowRect(ub->hwnd, &rc) ;
    WinFillRect(hps, &rc, SYSCLR_BUTTONMIDDLE) ;
    GpiSetColor(hps, hil ? SYSCLR_BUTTONDARK : SYSCLR_BUTTONLIGHT) ;        /* top and left edge */
    pt.x = rc.xLeft ;       pt.y = rc.yBottom ;  GpiMove(hps, &pt) ;
    pt.x = rc.xLeft ;       pt.y = rc.yTop - 1 ; GpiLine(hps, &pt) ;
    pt.x = rc.xRight - 1 ;  pt.y = rc.yTop - 1 ; GpiLine(hps, &pt) ;
    GpiSetColor(hps, hil ? SYSCLR_BUTTONLIGHT : SYSCLR_BUTTONDARK) ;        /* bottom and right edge */
    pt.x = rc.xRight - 1 ;  pt.y = rc.yBottom ;  GpiLine(hps, &pt) ;
    pt.x = rc.xLeft ;       pt.y = rc.yBottom ;  GpiLine(hps, &pt) ;
    if (def) {                                                              /* default button frame */
        GpiSetColor(hps, CLR_BLACK) ;
        pt.x = rc.xLeft + 1 ;   pt.y = rc.yBottom + 1 ; GpiMove(hps, &pt) ;
        pt.x = rc.xRight - 2 ;  pt.y = rc.yTop - 2 ;
        GpiBox(hps, DRO_OUTLINE, &pt, 0, 0) ;
    }
    p = osTrw(strId, &n) ;
    tn = plain_text(p, n, txt, sizeof(txt)) ;
    if (uni_begin(hps, face, BTN_PELS, &ocp)) {
        int lh = uni_height(hps), desc = uni_descender(hps), w = uni_width(hps, txt, tn) ;
        long x = (rc.xRight - rc.xLeft - w) / 2 + (hil ? 1 : 0) ;
        long y = (rc.yTop - rc.yBottom - lh) / 2 + desc - (hil ? 1 : 0) ;
        GpiSetColor(hps, dis ? SYSCLR_MENUDISABLEDTEXT : CLR_BLACK) ;
        uni_draw(hps, x, y, txt, tn) ;
    }
    uni_end(hps, ocp) ;
}

/* ------------------------------------------------------------------ */
/* start / stop                                                        */
/* ------------------------------------------------------------------ */

static int find_code(const char *code)
{
    int i ;

    for (i = 0 ; code && code[0] && i < nlang ; i++) {
        if (!stricmp(langs[i].code, code)) {
            return i ;
        }
    }
    return -1 ;
}

/* language that fits the code page of the process */
static int default_index(void)
{
    const char *code = "en" ;
    int i ;

    switch (uls_codepage()) {
    case 932 : case 942 : case 943 : code = "ja" ;    break ;
    case 949 :                       code = "ko" ;    break ;
    case 936 :                       code = "zh_CN" ; break ;
    case 950 :                       code = "zh_TW" ; break ;
    case 855 : case 866 :            code = "ru" ;    break ;
    case 869 :                       code = "el" ;    break ;
    }
    i = find_code(code) ;
    return (i >= 0) ? i : enIdx ;
}

void osLangInit(HAB hab, const char *saved, int ac, char *av[])
{
    PTIB ptib ;
    PPIB ppib ;
    char *p ;
    int i, idx = -1 ;

    uls_init() ;
    WinRegisterClass(hab, OS_LABEL_CLASS, (PFNWP) LabelProc, CS_SIZEREDRAW, 12) ;

    langDir[0] = 0 ;
    DosGetInfoBlocks(&ptib, &ppib) ;
    if (DosQueryModuleName(ppib->pib_hmte, sizeof(langDir), langDir) == 0) {
        p = strrchr(langDir, '\\') ;
        if (p) p[1] = 0 ; else langDir[0] = 0 ;
    }
    strcat(langDir, "lang\\") ;
    nlang = lang_scan(langDir, langs, MAX_LANGS) ;
    for (i = 0 ; i < nlang ; i++) {
        if (!stricmp(langs[i].code, "en")) enIdx = i ;
    }

    for (i = 1 ; i < ac && idx < 0 ; i++) {         /* command line wins */
        idx = find_code(av[i]) ;
    }
    if (idx < 0) idx = find_code(saved) ;
    if (idx < 0) idx = default_index() ;
    cur = idx ;
    trgen++ ;
    resolve_face() ;
}

void osLangSelect(int idx)
{
    if (nlang == 0) {
        return ;
    }
    if (idx < 0 || idx >= nlang) {
        idx = (enIdx >= 0) ? enIdx : 0 ;
    }
    cur = idx ;
    trgen++ ;
    resolve_face() ;
}

void osLangDone(void)
{
    lang_free(langs, nlang) ;
    nlang = 0 ;
    uni_done() ;
}
