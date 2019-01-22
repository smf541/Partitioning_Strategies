/*
Copyright (C) 2008, 2011 Pablo A. Goloboff, Santiago Catalano

This program is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the Free
Software Foundation; either version 3 of the License, or (at your option)
any later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
(at http://www.opensource.org/licenses/gpl-3.0.html) for more details.

Email: pablogolo@csnat.edu.ar
Mail:
  Pablo A. Goloboff, INSUE, Instituto Miguel Lillo,
  Miguel Lillo 205, 4000 S.M. de Tucuman, Argentina.

*/

#define   inva      InvalidateRect ( hwnd, NULL , 1 )
#define VTYPE 
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ibm.h"
#include <io.h>
#include "winbase.h"
#include "process.h"
#include <winuser.h>
#include "commdlg.h"
#include "commctrl.h"
#include "wingdi.h"
#include "winsock.h"
#include "ifase.h"
#include "gb2tnt.h"
#include <tchar.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <ctype.h>
#include <unistd.h>
#include <direct.h>
#include <limits.h>

#define MAFFT  1 
#define MUSCLE 2 
#define USER   3

int dooneseqpersp = 1 ; 
int aligner_is = MAFFT , show_rejects = 0 ; 
int query_taxon_identity = 0 , save_accession_numbers = 1 , save_taxonomy = 1 ; 

FILE * logfileis = NULL ; 
extern int unnamed_project , proj_numblocks , show_full_paths , have_something_to_save ; 
char szWinName[] = "MyWin"; 
HCURSOR hNormal, hGlass, hCollapse;
HMENU hmu ;
HFONT hfont ;
CHOOSEFONT cfo;
LOGFONT lf ;
HDC hdc ; 
TEXTMETRIC ourtm;
char filefilter[] = "GB-2-TNT files (*.G2T)\0*.g2t\0ALL files\0*.*\0";
//char logfilefilter[] = "text files (*.txt)\0*.txt\0ALL files\0*.*\0";
void handle_exe_locations ( char * , char * ) ; 
char * squeeze_filename ( char * )  ;

/****  THESE ARE VARIABLES USED IN SCOPE READING FUNCTION, BELOW *************/
#define SELECT_DIALOG StartSelectFun ( hdwnd )  
char selthings [ NAMESIZE ] , unselthings [ NAMESIZE ] ;
char ** selthingnames ;
int selnumthings ;
int * tmpcurlist ; 
char * selectlist ;
char itemnamespace [ NAMESIZE ];
char * itemname;
/************END OF VARIABLES FOR SCOPE READING FUNCTION ********/

#define FOCUS_ON(x) SetFocus ( GetDlgItem ( hdwnd , x ) ) ; return FALSE ; 
#define TOGGLE(x)  x = 1 - x ; 
#define BUTT_CHECK( ctrl ) SendDlgItemMessage( hdwnd,  ctrl , BM_SETCHECK, (WPARAM) BST_CHECKED , 0 )
#define BUTT_UNCHEK( ctrl ) SendDlgItemMessage( hdwnd,  ctrl , BM_SETCHECK, (WPARAM) BST_UNCHECKED , 0 )
#define CTRL_DISABLE( ctrl ) Button_Enable ( GetDlgItem ( hdwnd , ctrl ) , FALSE )   
#define SPIN_DISABLE( ctrl ) \ 
                   { SetDlgItemText ( hdwnd , GetDlgCtrlID ( SendMessage( GetDlgItem ( hdwnd , ctrl ) , UDM_GETBUDDY, 0, 0) ) , "" ) ; \
                     Button_Enable ( SendMessage( GetDlgItem ( hdwnd , ctrl ) , UDM_GETBUDDY, 0, 0 ) , FALSE ) ; \ 
                     Button_Enable ( GetDlgItem ( hdwnd , ctrl ) , FALSE ) ; } 
#define CTRL_ENABLE( ctrl ) Button_Enable ( GetDlgItem ( hdwnd , ctrl ) , TRUE )   
#define SETTEXT( x , y ) \
          { sprintf ( junkstr , "%s" , x ) ; SetDlgItemText ( hdwnd , y , junkstr ) ; } 
#define SETFLT( x , y ) \
          { sprintf ( junkstr , "%.3f" , ( double ) x / 1000 ) ; \
            SetDlgItemText ( hdwnd , y , junkstr ) ; } 
#define GETFLT( x , y ) \ 
          { GetDlgItemText( hdwnd, ( y ) , junkstr, 80); \ 
            x = atof(junkstr); } 
#define SETINT( x , y ) \
          { sprintf ( junkstr , "%i" , x ) ; \
            SetDlgItemText ( hdwnd , y , junkstr ) ; } 
#define GETINT( x , y ) \ 
          { GetDlgItemText(hdwnd, ( y ) , junkstr, 80); \ 
            x = atoi(junkstr); } 
#define SET_UPDOWN( ctrl, from , to , pos ) \ 
          { Button_Enable ( GetDlgItem ( hdwnd , ctrl ) , TRUE ) ; \
            Button_Enable ( SendMessage( GetDlgItem ( hdwnd , ctrl ) , UDM_GETBUDDY, 0, 0 ) , TRUE ) ; \ 
            hWho = SendMessage( GetDlgItem ( hdwnd , ctrl ) , UDM_GETBUDDY, 0, 0) ; \ 
            i = GetDlgCtrlID ( hWho ) ;\
            wsprintf ( junkstr,"%i", pos ) ; \
            SetDlgItemText ( hdwnd , i , junkstr ) ;\
            hWho = GetDlgItem ( hdwnd , ctrl ) ; \
            i = ( to )  | ( ( from )  << 16 ) ; \
            SendMessage((hWho), UDM_SETRANGE, 0,  i ) ; \
            SendMessage((hWho), UDM_SETPOS, 0 , (WPARAM)(int) ( pos )) ; } 
#define ON_SPIN case WM_VSCROLL
#define CHG_UPDOWN( ctrl ) \
            { hWho = GetDlgItem ( hdwnd , ctrl ) ; \
              i = ( SendMessage(  hWho , UDM_GETPOS, 0, 0) ) & 65535 ; \
              wsprintf ( junkstr ,"%i",  i ) ; \
              hWho = SendMessage ( GetDlgItem (hdwnd , ctrl ) , UDM_GETBUDDY, 0 , 0 ) ; \
              i = GetDlgCtrlID ( hWho ) ; \
              SetDlgItemText(hdwnd, i , junkstr ); } 

extern int WINBUFSIZ ;
int text_height = 18 , text_width , maxlinwidth ;
int WinHeight ;
int WinWidth ; 
#define MAXSCREEN_SIZE 150
int linwidth [ MAXSCREEN_SIZE ] ;
int screen_shift = 0 ;

HINSTANCE hInst ;
HMENU hmainmenu ;
OPENFILENAME opbuff;
HWND hwnd , BoxTreehwnd; 

RECT WinDim;
RECT BoxDim ;

char filename[MAX_PATH+3];

int files_to_recall = 0 ;
int recall_more_files = 1 ; 
char last_five_infiles[6][MAX_PATH+3] ;
int mark_treefile = 0 ; 
MENUITEMINFO lastfiles[6] , * ufalala ;
char * newfilename ; 

LRESULT CALLBACK WindowFunc(HWND, UINT, WPARAM, LPARAM);

int blanks_in ( char * txt )
{
    char * cp = txt ;
    while ( * cp != '\0' ) {
         if ( * cp == ' ' ) * cp = '^' ;
         ++ cp ; }
    return 0 ;            
}

void set_font_xfeatures ( LOGFONT * thef ) 
{
    thef->lfEscapement = 0 ;
    thef->lfOrientation = 0 ;
    thef->lfWeight = 400 ;
    thef->lfItalic = 0 ;
    thef->lfUnderline = 0 ;
    thef->lfStrikeOut = 0 ;
    thef->lfCharSet = DEFAULT_CHARSET ;
    thef->lfOutPrecision = 3 ;
    thef->lfClipPrecision = 2 ;
    thef->lfQuality = 1 ;  
}

int WINAPI WinMain(HINSTANCE hThisInst, HINSTANCE hPrevInst, LPSTR lpszArgs, int nWinMode)

{
    MSG msg;
    WNDCLASSEX wcl;
    HACCEL hAccel;
    int i , a, found_semic ;
    clock_t ini ;
    char * pup ;


    GetCurrentDirectory ( MAX_PATH , project_name ) ; 
    add_previous_files () ;
    SetCurrentDirectory ( project_name ) ;


    if ( !mafftexe[0] )
         if ( muscleexe[0] ) aligner_is = MUSCLE ;
         else if ( alignerexe[0] ) aligner_is = USER ;
         else aligner_is = -1 ; 
    
    hGlass = LoadCursor(NULL, IDC_WAIT);
    hCollapse = LoadCursor(hThisInst,"Collapse");

    /* Define */
    wcl.cbSize = sizeof(WNDCLASSEX);
    wcl.hInstance = hThisInst; 
    wcl.lpszClassName = szWinName; 
    wcl.lpfnWndProc = WindowFunc; 
    wcl.style = CS_DBLCLKS; 
    wcl.hIcon = LoadIcon(hThisInst, "GB2TNT"); 
    wcl.hIconSm = NULL; 
    wcl.hCursor = LoadCursor(NULL, IDC_ARROW); 
    hNormal = wcl.hCursor;
    wcl.lpszMenuName = "MyMenu"; 
    wcl.cbClsExtra = 0; 
    wcl.cbWndExtra = 0; 
    wcl.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);

    /* Register */
    if(!RegisterClassEx(&wcl)) return(0);

    sprintf(junkstr, "GenBank to TNT - No project open" );
    initialize_winbufsiz ( 1000 ) ;
    hwnd = CreateWindow(
        szWinName, 
        junkstr, 
        WS_OVERLAPPEDWINDOW | WS_VSCROLL | WS_HSCROLL , 
        CW_USEDEFAULT, 
        CW_USEDEFAULT, 
        CW_USEDEFAULT, 
        CW_USEDEFAULT, 
        HWND_DESKTOP, 
        NULL,
        hThisInst, 
        NULL );
    hInst = hThisInst;
    hAccel = LoadAccelerators(hThisInst, "MyMenu");
    // ShowWindow ( hwnd , SW_MAXIMIZE ) ;
    ShowWindow ( hwnd , SW_RESTORE ) ;
    UpdateWindow(hwnd);
    InitCommonControls();
    InvalidateRect(hwnd,NULL,1);
    hmainmenu = GetMenu(hwnd);

    strcpy(junkstr,"NADA!");

    hmu = GetMenu(hwnd);
    EnableMenuItem( hmu , IDM_FILEMENU , MF_ENABLED);
    EnableMenuItem( hmu , IDM_HELPMENU , MF_ENABLED);
    update_menu_old_files ( ) ; 
    DrawMenuBar ( hwnd ) ; 

    strcpy ( lf.lfFaceName , "Courier" ) ;
    lf.lfHeight = 18 ; 
    lf.lfWeight = FW_DONTCARE ;
    lf.lfCharSet = DEFAULT_CHARSET ;
    lf.lfPitchAndFamily = 49 ;
    set_font_xfeatures ( &lf ) ; 
    hfont = CreateFontIndirect(&lf);
    hdc = GetDC ( hwnd ) ; 
    SelectFont ( hdc , hfont ) ;
    GetTextFace ( hdc , 80 , junkstr ) ;
    if ( strcmp ( junkstr ,"Courier" ) ) {
       strcpy ( lf.lfFaceName , "Terminal" ) ;
       lf.lfHeight = 16 ; 
       lf.lfWeight = FW_DONTCARE ;
       lf.lfCharSet = DEFAULT_CHARSET ;
       lf.lfPitchAndFamily = 49 ; 
       hfont = CreateFontIndirect(&lf);
       hdc = GetDC ( hwnd ) ; 
       SelectFont ( hdc , hfont ) ;
       GetTextFace ( hdc , 80 , junkstr ) ; }

    set_screen_size ( 0 ) ;
    GetClientRect(hwnd, &BoxDim);

    if ( __argc > 1 ) {
       strcpy ( junkstr , __argv[1] ) ;
       if( ! _fullpath( project_name , junkstr , _MAX_PATH ) )
           strcpy ( junkstr , __argv[1] ) ;
       if ( read_the_project ( project_name ) ) {
            sprintf ( junkstr , "GenBank to TNT - %s" , project_name ) ; 
            SetWindowText ( hwnd , junkstr ) ; 
            add_menu_old_file ( project_name ) ;
            myp ( "Read project \"%s\" (%i blocks)" , project_name , proj_numblocks ) ;
            newln () ;
            eofscreen () ; }}

    while(GetMessage(&msg, NULL, 0, 0) ) {
           if(!IsDialogMessage(BoxTreehwnd, &msg)){
              if(!TranslateAccelerator(hwnd, hAccel, &msg)){
                  TranslateMessage(&msg); 
                  DispatchMessage(&msg); }}} 

    ReleaseDC(hwnd, hdc);
    return msg.wParam;
}

void showjunk ( HDC hdc , char * txt )
{
   TextOut ( hdc , 20 , 20 , txt , strlen ( txt ) ) ;
}   

void set_screen_size ( int inited )
{
    int i ;
    RECT tmpdim ; 
    GetClientRect(hwnd, &WinDim);
    WinHeight = i = WinDim.bottom - WinDim.top ;
    WinWidth = WinDim.right - WinDim.left ; 
    hdc = GetDC ( hwnd ) ; 
    SelectFont ( hdc , hfont ) ; 
    GetTextMetrics(hdc,&ourtm);
    text_height = (int )ourtm.tmHeight + ourtm.tmExternalLeading ;
    i = ( i / text_height ) - 1 ;
    if ( i < 1 ) i = 1 ; 
    if ( i != screen_size ) {
          screen_size = i ;
          if ( screen_size > MAXSCREEN_SIZE ) screen_size = MAXSCREEN_SIZE ; 
          copygil ( 0 ) ; } 
    i = WinDim.right - WinDim.left ; 
    text_width = (int ) ourtm.tmAveCharWidth ;
    screen_width = i / text_width + 1 ;
    InvalidateRect( hwnd , NULL , 1 ) ;
    return ;
} 
    
void reset_screen_shift ( void )
{
    screen_shift = 0 ;
}

void eofscreen ( void )
{
    copygil ( -WINBUFSIZ ) ;
    copygil ( screen_size ) ; 
    reset_screen_shift () ; 
}
    
void display_bienvenida ( void )
{
    time_t tis ;
    newln () ; 
    myp ( "\n" ) ;
    myp ( "                  G B -> T N T \n" ) ;
    myp ( "                   vers. 0.75 \n\n" ) ;
    myp ( "           %c  P. Goloboff  and  S. Catalano  2011 \n\n" , 169 ) ;
    myp ( "          if publishing results, please cite paper\n" ) ; 
    myp ( "          in Cladistics (forthcoming) as reference\n\n" ) ; 
    myp ( "\n\n" ) ;
    myp ( "Starting Session - " ) ;
    time ( &tis ) ; 
    showmoddate ( tis ) ;
    newln () ;
}    
LRESULT CALLBACK WindowFunc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int a , b , c ;
    char chari ; 
    int response, i,j,k,l, old_xread, skipit ;
    char * cp ;
    PAINTSTRUCT paintstruct;
    char localstr[512], save_path[MAX_PATH];
    RECT rect;
    static SCROLLINFO sinfo ;
    double da ;
    time_t tis ;
    static int wellcomed = 0 ; 
    
    hmainmenu = GetMenu(hwnd);

    switch(message) {

/*
        case WM_CREATE :
                gray_menus () ;
                SendMessage ( hwnd , WM_KEYDOWN , VK_PRIOR , 0 ) ;
                inva ; 
                break;
*/
        case WM_MOUSEWHEEL :
           j = LOWORD ( wParam ) ;
           i = GET_WHEEL_DELTA_WPARAM(wParam);
           if ( !i ) break ;
           if ( ( j & MK_CONTROL ) ) break ; 
           if ( i < 0 ) {
               copygil ( -( (screen_size/2) ) ) ;
               inva ; 
               // SendMessage ( hwnd , WM_KEYDOWN , VK_NEXT , 0 ) ;
               }
           else {
               copygil ( ( (screen_size/2) ) ) ;
               inva ; 
               // SendMessage ( hwnd , WM_KEYDOWN , VK_PRIOR , 0 ) ;
               }
           break ;

        case WM_SIZE:
             set_screen_size ( 1 ) ;
             inva ;
             break ; 

        case WM_MOVE:
                set_screen_size ( 1 ) ;
                inva ;
                break ; 

        case WM_PAINT :
                       gray_menus () ;
                       hdc = BeginPaint(hwnd, &paintstruct);
                       SelectFont ( hdc , hfont ) ; 
                       GetTextMetrics(hdc,&ourtm);
                       if ( !wellcomed ) 
                           display_bienvenida () ;
                       // text_height = (int )ourtm.tmHeight + ourtm.tmExternalLeading ;
                       // text_width = (int ) ourtm.tmAveCharWidth ;
                       // showjunk ( hdc , junkstr ) ; 
                       // SelectFont ( hdc , hfont ) ; 
                       // GetTextMetrics(hdc,&ourtm);
                       text_height = (int )ourtm.tmHeight + ourtm.tmExternalLeading ;
                       text_width = (int ) ourtm.tmAveCharWidth ;
                       maxlinwidth = j = 0 ;
                       for ( i = 0 ; i < linsdone ; ++ i ) { 
                          if ( ( linwidth [ i ] = strlen ( gil [ i ] ) ) > maxlinwidth )
                              maxlinwidth = linwidth [ i ]  ; } 
                       j = screen_shift ;
                       maxlinwidth -= ( screen_width / 2 ) ;
                       if ( maxlinwidth < 0 ) maxlinwidth = 0 ; 
                       if ( j > maxlinwidth ) j = maxlinwidth ; 
                       k = 10 - ( j * text_width ) ;
                       for(i=0;i<linsdone ;++i) {
                          TextOut ( hdc , k , i * text_height, gil[i] , linwidth [ i ] ) ; } 
                       sinfo.cbSize = sizeof ( SCROLLINFO ) ;
                       sinfo.fMask = SIF_POS ;
                       sinfo.nMin = 0 ;
                       sinfo.nMax = 100 ; 
                       j = give_scroll_pos ( ) ;
                       sinfo.nPos = j ;  
                       SetScrollInfo ( hwnd , SB_VERT , &sinfo , TRUE ) ;
                       if ( screen_shift < maxlinwidth ) 
                           sinfo.nPos = ( 100 * screen_shift ) / maxlinwidth ;
                       else sinfo.nPos = 100 ; 
                       SetScrollInfo ( hwnd , SB_HORZ , &sinfo , TRUE ) ; 
                       // UpdateStatus(hwnd);
                       EndPaint(hwnd, &paintstruct);
                       if ( !wellcomed ) SendMessage ( hwnd , WM_KEYDOWN , VK_PRIOR , 0 ) ;
                       wellcomed = 1 ;
                       break;

         case WM_CLOSE :
                        if ( !warn_for_unsaved_data ( ) ) break ;
                        save_menu_old_files () ;
                        PostQuitMessage(0);
                        break;

         case WM_KEYDOWN:
               switch ((char )wParam) {
                    case VK_HOME :
                        copygil ( WINBUFSIZ ) ;
                        screen_shift = 0 ;  
                        InvalidateRect ( hwnd, NULL , 1 ) ; 
                        break ;
                    case VK_END :
                        copygil ( - WINBUFSIZ ) ;
                        copygil ( screen_size ) ; 
                        screen_shift = strlen ( gil [ linsdone - 1 ] ) ;  
                        InvalidateRect ( hwnd, NULL, 1 ) ;
                        break ; 
                    case VK_NEXT :
                        copygil ( -( screen_size - 1 ) ) ;
                        InvalidateRect(hwnd,NULL,1);
                        break;
                    case VK_PRIOR :
                        copygil ( ( screen_size - 1 ) ) ;
                        InvalidateRect(hwnd,NULL,1);
                        break;
                    case VK_UP :
                        copygil ( 1 ) ;
                        InvalidateRect(hwnd,NULL,1);
                        break;
                    case VK_DOWN :
                        copygil ( -1 ) ;
                        InvalidateRect(hwnd,NULL,1);
                        break;
                    case VK_LEFT :
                        if ( screen_shift > maxlinwidth ) screen_shift = maxlinwidth ;  
                        if ( -- screen_shift < 0 ) screen_shift = 0 ;  
                        InvalidateRect(hwnd,NULL,1); 
                        break;
                    case VK_RIGHT :
                        ++ screen_shift ; 
                        if ( screen_shift > maxlinwidth ) screen_shift = maxlinwidth ;  
                        InvalidateRect(hwnd,NULL,1); 
                        break;
                    }
                 break;

         case WM_COMMAND :

             i = LOWORD ( wParam ) ;
             switch( i ){

                    case IDM_EXIT :
                        if ( !warn_for_unsaved_data ( ) ) break ;
                        save_menu_old_files () ;
                        PostQuitMessage(0);
                        break;

case IDM_ONESEQPERSP:
   dooneseqpersp = 1 - dooneseqpersp ;
   time ( &tis) ;
   for ( a = 0 ; a < proj_numblocks ; ++ a ) {
       blocklist[a].modtime = tis ;
       have_something_to_save = 1 ; }
   inva ; 
   break ; 
case IDM_BIOEDITCHECK:
    do_bioedit_check () ;
    inva ; 
    break ; 
case IDM_GETCRITERIA:
   do_inclusion_criteria () ;
   inva ; 
   break ; 
case IDM_TAXONOMYCHANGEHIGHER:
    handle_taxonomic_changes_higher () ;
    inva ; 
    break ; 
case IDM_TAXONOMYCHANGESPECIES:
    handle_taxonomic_changes_lower () ;
    inva ; 
    break ; 
case IDM_NEWPROJECT:
project_edit ( 0 ) ;
inva ;
break ; 
case IDM_MODIFYPROJECT:
project_edit ( 1 ) ;
inva ;
break ;

case IDM_LOCATEBIOEDIT:
    bioeditexe[0] = '\0' ; 
    handle_exe_locations ( bioeditexe , "Specify location of BioEdit binary" ) ;
    inva ;
    break ;
case IDM_LOCATEMAFFT:
    mafftexe[0] = '\0' ; 
    handle_exe_locations ( mafftexe , "Specify location of MAFFT batch file" ) ;
    inva ;
    break ;
case IDM_LOCATEMUSCLE:     
    muscleexe[0] = '\0' ; 
    handle_exe_locations ( muscleexe , "Specify location of MUSCLE binary" ) ; 
    inva ;
    break ;
case IDM_LOCATETNT: 
    tntexe[0] = '\0' ; 
    handle_exe_locations ( tntexe , "Specify location of TNT binary" ) ;
    inva ;
    break ;
case IDM_LOCATEALIGNER: 
    alignerexe[0] = '\0' ; 
    handle_exe_locations ( alignerexe, "Specify location of alternative aligner" ) ; 
    inva ;
    break ;
case IDM_MAFFT :
   aligner_is = MAFFT ;
   myp ( "Setting MAFFT as aligner" ) ; newln () ; eofscreen () ; 
   inva ;
   break ; 
case IDM_MUSCLE :
   aligner_is = MUSCLE ;
   myp ( "Setting MUSCLE as aligner" ) ; newln () ; eofscreen () ; 
   inva ;
   break ; 
case IDM_CHOOSEALIGNER :
   aligner_is = USER ;
   myp ( "Setting %s as aligner" , alignerexe ) ; newln () ; eofscreen () ; 
   inva ;
   break ;
case IDM_QUERYIDENTITY:
    query_taxon_identity = 1 - query_taxon_identity ;
    inva ;
    break ;
case IDM_SAVEACCESSIONS:
    save_accession_numbers = 1 - save_accession_numbers ;
    inva ;
    break ;     
case IDM_SAVETAXONOMY:
    save_taxonomy = 1 - save_taxonomy ;
    inva ;
    break ;
case IDM_ERASEALLTEMPS:
     erase_all_temporary_files () ;
     inva ;
     break ; 

                    case IDM_SHOWDEPENDS:
                        show_all_dependencies ( 1 , 1 , 1 ) ;
                        inva ;
                        break ;

                    case IDM_FULLPATHS:
                         show_full_paths = 1 - show_full_paths ;
                         inva ;
                         break ; 

                    case IDM_SHOWREJECTS:
                         show_rejects = 1 - show_rejects ;
                         inva ;
                         break ; 

                    case IDM_OPEN :
                        fn[0]='\0';
                        memset(&opbuff, 0, sizeof(OPENFILENAME));
                        opbuff.lStructSize = sizeof(OPENFILENAME);
                        opbuff.hwndOwner =hwnd;
                        opbuff.lpstrFilter = filefilter;
                        opbuff.nFilterIndex = 1;
                        opbuff.lpstrFile = fn;
                        opbuff.lpstrTitle = "Open Project File";
                        opbuff.nMaxFile = sizeof(fn);
                        opbuff.lpstrFileTitle = filename;
                        opbuff.nMaxFileTitle = sizeof(filename)-1;
                        opbuff.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY ; 
                        if(GetOpenFileName(&opbuff)){
                                SetCursor(hGlass);
                                if ( read_the_project ( fn ) ) {
                                    strcpy ( project_name , fn ) ;
                                    myp ( "Read project \"%s\" (%i blocks)" , project_name , proj_numblocks ) ;
                                    newln () ; eofscreen () ; 
                                    sprintf ( junkstr , "GenBank to TNT - %s" , fn ) ; 
                                    SetWindowText ( hwnd , junkstr ) ; 
                                    add_menu_old_file ( project_name ) ; }
                                SetCursor(hNormal); }
                       InvalidateRect(hwnd,NULL,1);
                       break ; 

                    case IDM_SAVEPROJECT:
                    case IDM_SAVEPROJECTASS:
                        handle_savings ( i ) ; 
                        inva ; 
                        break;

                    case IDM_CREATE:
                         if ( unnamed_project ) {
                             MessageBox ( hwnd , "To create matrix, you need to save the project" , "NOTE" , MB_OK ) ;
                             handle_savings ( IDM_SAVEPROJECT ) ;
                             if ( unnamed_project ) break ; }
                         handle_matrix_creation () ;
                         inva ;
                         break ; 

                    case IDM_SAVEREPORTS:
                           fn[0]='\0';
                           memset(&opbuff, 0, sizeof(OPENFILENAME));
                           opbuff.lStructSize = sizeof(OPENFILENAME);
                           opbuff.hwndOwner =hwnd;
                           opbuff.lpstrFilter = "text files (*.txt)\0*.txt\0ALL files\0*.*\0"; // logfilefilter;
                           opbuff.nFilterIndex = 1;
                           opbuff.lpstrFile = fn;
                           opbuff.lpstrTitle = "Save reports to...";
                           opbuff.nMaxFile = sizeof(fn);
                           opbuff.lpstrFileTitle = filename;
                           opbuff.nMaxFileTitle = sizeof(filename)-1;
                           opbuff.lpstrDefExt = "txt";
                           opbuff.Flags = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST ;
                           if(GetSaveFileName( &opbuff )) {
                              logfileis = fopen ( fn , "wb" ) ; 
                              if ( logfileis == NULL ) {
                                  MessageBox ( hwnd , "Sorry, can't open file for logging" , "ERROR" , MB_ICONERROR ) ;
                                  break ; }
                              save_txt_buffer () ;
                              myp ( "Logging reports to %s " , fn ) ; newln () ; eofscreen () ; }
                        inva ; 
                        break;

                    case IDM_CLOSEREPORTS :
                           fclose ( logfileis ) ;
                           logfileis = NULL ;
                           inva ;
                           break ; 

                    case 5000 :
                    case 5001 : 
                    case 5002 :
                    case 5003 : 
                    case 5004 :
                        i = LOWORD ( wParam ) - 5000 ; 
                        strcpy ( fn , ( last_five_infiles[i] + 3 ) ) ;
                        if ( read_the_project ( fn ) ) {
                             strcpy ( project_name , fn ) ; 
                             sprintf ( junkstr , "GenBank to TNT - %s" , project_name ) ; 
                             SetWindowText ( hwnd , junkstr ) ; 
                             add_menu_old_file ( project_name ) ;
                             myp ( "Read project \"%s\" (%i blocks)" , project_name , proj_numblocks ) ;
                             newln () ; eofscreen () ; }
                        inva ;
                        break ; 

                     default :
                        MessageBox ( hwnd, "Sooorry, option is not implemented yet", "Ooops. . . ", MB_OK ) ; 
                        return DefWindowProc(hwnd, message, wParam, lParam);
                             
                }
                break;

        case WM_VSCROLL :
                switch (LOWORD(wParam)) {
                    case SB_THUMBPOSITION :
                        sinfo.cbSize = sizeof ( SCROLLINFO ) ;
                        sinfo.fMask = SIF_POS | SIF_TRACKPOS ;
                        sinfo.nMin = 0 ;
                        sinfo.nMax = 100 ;
                        GetScrollInfo ( hwnd , SB_VERT , &sinfo ) ;
                        copygil ( - WINBUFSIZ ) ;
                        j = give_total_lines() ;
                        sinfo.nTrackPos = 100 - sinfo.nTrackPos ; 
                        i = ( sinfo.nTrackPos * j ) / 100 ;
                        copygil ( i ) ;  
                        InvalidateRect(hwnd,NULL,1);
                        break;
                    case SB_THUMBTRACK :
                        sinfo.cbSize = sizeof ( SCROLLINFO ) ;
                        sinfo.fMask = SIF_POS | SIF_TRACKPOS ;
                        sinfo.nMin = 0 ;
                        sinfo.nMax = 100 ;
                        GetScrollInfo ( hwnd , SB_VERT , &sinfo ) ;
                        copygil ( - WINBUFSIZ ) ;
                        j = give_total_lines() ;
                        sinfo.nTrackPos = 100 - sinfo.nTrackPos ; 
                        i = ( sinfo.nTrackPos * j ) / 100 ;
                        copygil ( i ) ;  
                        InvalidateRect(hwnd,NULL,1);
                        break;
                    case SB_PAGEDOWN :
                        copygil( -( screen_size - 1 ) ) ;
                        InvalidateRect(hwnd,NULL,1);
                        break;
                    case SB_PAGEUP :
                        copygil ( ( screen_size - 1 ) ) ;
                        InvalidateRect(hwnd,NULL,1);
                        break;
                    case SB_LINEDOWN :
                        copygil ( -1 ) ; 
                        InvalidateRect(hwnd,NULL,1);
                        break;
                    case SB_LINEUP :
                        copygil ( 1 ) ; 
                        InvalidateRect(hwnd,NULL,1);
                        break;
                    default : break; }
                break;
                
        case WM_HSCROLL :
                switch (LOWORD(wParam)) {
                    case SB_THUMBPOSITION :
                        sinfo.cbSize = sizeof ( SCROLLINFO ) ;
                        sinfo.fMask = SIF_POS | SIF_TRACKPOS ;
                        sinfo.nMin = 0 ;
                        sinfo.nMax = 100 ;
                        GetScrollInfo ( hwnd , SB_HORZ , &sinfo ) ;
                        screen_shift = ( sinfo.nTrackPos * maxlinwidth ) / 100 ; 
                        InvalidateRect(hwnd,NULL,1);
                        break;
                    case SB_THUMBTRACK :
                        sinfo.cbSize = sizeof ( SCROLLINFO ) ;
                        sinfo.fMask = SIF_POS | SIF_TRACKPOS ;
                        sinfo.nMin = 0 ;
                        sinfo.nMax = 100 ;
                        GetScrollInfo ( hwnd , SB_HORZ , &sinfo ) ;
                        screen_shift = ( sinfo.nTrackPos * maxlinwidth ) / 100 ; 
                        InvalidateRect(hwnd,NULL,1);
                        break;
                    case SB_PAGELEFT :
                        if ( screen_shift > maxlinwidth ) screen_shift = maxlinwidth ;  
                        screen_shift -= 30 ;
                        if ( screen_shift < 0 ) screen_shift = 0 ;  
                        InvalidateRect(hwnd,NULL,1);
                        break;
                    case SB_PAGERIGHT :
                        screen_shift += 30 ;
                        if ( screen_shift > maxlinwidth ) screen_shift = maxlinwidth ;  
                        InvalidateRect(hwnd,NULL,1);
                        break;
                    case SB_LINELEFT :
                        if ( screen_shift > maxlinwidth ) screen_shift = maxlinwidth ;  
                        if ( -- screen_shift < 0 ) screen_shift = 0 ;
                        InvalidateRect(hwnd,NULL,1);
                        break;
                    case SB_LINERIGHT :
                        ++ screen_shift ; 
                        if ( screen_shift > maxlinwidth ) screen_shift = maxlinwidth ;  
                        InvalidateRect(hwnd,NULL,1);
                        break;
                    default : break; }
                break;

        default : return DefWindowProc(hwnd, message, wParam, lParam);
    }
    return(0);
}

void getanexe ( FILE * filefile , char * str )
{
    int a , b ;
    char * l , * r ; 
    l = ( char * ) &a ;
    r = l + 1 ; 
    a = strlen ( str ) ;
    fscanf ( filefile , "%c%c" , l , r ) ;
    for ( b = 0 ; b < a ; ++ b ) 
        str [ b ] = getc ( filefile ) ;
    str [ a ] = '\0' ; 
    return ; 
}

void add_previous_files ( void )
{
    /*****  THIS IS STUFF TO ADD LAST 5 FILES OPENED TO THE "FILE" MENU *****/
int chr ;
int a , b , i , e, j ;
unsigned long int x , y ; 
FILE * filefile ; 
ufalala = lastfiles ;
for ( a = 0 ; a < 6 ; ++a ) {
    lastfiles[a].cbSize = sizeof ( MENUITEMINFO ) ;
    lastfiles[a].fMask = MIIM_ID | MIIM_STATE | MIIM_TYPE ; 
    lastfiles[a].fType = MFT_STRING ; 
    lastfiles[a].fState = MFS_ENABLED ; 
    lastfiles[a].wID = 5000 + a ; 
    lastfiles[a].hSubMenu = NULL ; 
    lastfiles[a].hbmpChecked = NULL ; 
    lastfiles[a].hbmpUnchecked = NULL ; 
    lastfiles[a].dwItemData = 0 ; 
    lastfiles[a].dwTypeData = last_five_infiles[a] ; 
    lastfiles[a].cch = 0 ; }
lastfiles[5].fType = MFT_SEPARATOR ; 
files_to_recall = 0 ;
GetWindowsDirectory ( filename , MAX_PATH ) ;
strcat ( filename,"\\gb2tnt.opt" ) ;
filefile = fopen ( filename , "rb" ) ;
i = a = files_to_recall = 0 ;
if ( filefile != NULL ) {
    getanexe ( filefile , mafftexe ) ; 
    getanexe ( filefile , muscleexe ) ; 
    getanexe ( filefile , tntexe ) ; 
    getanexe ( filefile , alignerexe ) ;
    getanexe ( filefile , bioeditexe ) ; 
    aligner_is = getc ( filefile ) ; 
    files_to_recall = getc ( filefile ) ; 
    chr = getc( filefile ) ;
    while ( !feof(filefile ) ) {
        if ( !i ) {
            last_five_infiles[a][0] = '&' ;
            last_five_infiles[a][1] = '1' + a ;
            last_five_infiles[a][2] = ' ' ;
            i = 3 ; }
        if ( chr == '^' ) chr = ' ' ; 
        last_five_infiles[a][i++] = chr ;
        chr = getc ( filefile ) ; 
        if ( isspace ( chr ) || feof ( filefile ) )
               { last_five_infiles[a][i] = '\0'; ++ a ; i = 0 ;
                 while ( isspace ( chr ) && ! feof ( filefile ) )
                        chr = getc( filefile ) ; }
        if ( a == 5 ) break ; }}
else {  // first time running, ask user to specify location of exes
    sprintf ( junkstr ,
        "You must specify where binaries for\n"
        "aligners and TNT are.\n"
        "\n"
        "If you do not plan on using a binary,\n"
        "cancel the corresponding request.\n"
        "\n"
        "You can subsequently add (or change)\n"
        "any location at any time by selecting\n"
        "Options/Location of binaries\"" ) ; 
    MessageBox ( hwnd , junkstr , "Please note..." , MB_OK ) ; 
    mafftexe[0] = '\0' ; 
    handle_exe_locations ( mafftexe , "Specify location of MAFFT batch file" ) ; 
    muscleexe[0] = '\0' ; 
    handle_exe_locations ( muscleexe , "Specify location of MUSCLE binary" ) ; 
    tntexe[0] = '\0' ;
    MessageBox ( hwnd , "Now, you will be asked to specify the location of TNT.\nMake sure you have downloaded a recent copy of TNT,\nsince earlier versions can't process taxonomic information" , "Please note..." , MB_OK ) ; 
    handle_exe_locations ( tntexe , "Specify location of TNT binary" ) ; 
    alignerexe[0] = '\0' ; 
    handle_exe_locations ( alignerexe , "Specify location of alternative aligner" ) ;
    bioeditexe[0] = '\0' ;
    handle_exe_locations ( bioeditexe , "Specify location of BioEdit binary" ) ; }
        
DrawMenuBar ( hwnd ) ;
fclose ( filefile ) ;
return ; 
} 

int eqfilename ( char * a , char * b )
{
    a += 3 ;  
    while ( tolower ( * a ) == tolower ( * b ) && * a ) { ++a ; ++b ; }
    if ( !*a && !*b ) return 1 ; 
    return 0 ;
} 

void saveanexe ( FILE * filefile , char * str )
{
    int a , b ;
    char * l , * r ; 
    l = ( char * ) &a ;
    r = l + 1 ; 
    a = strlen ( str ) ;
    fprintf ( filefile , "%c%c" , *l , * r ) ;
    fprintf ( filefile , "%s" , str ) ;
    return ; 
}

void save_menu_old_files ( void )
{ FILE * filefile ;
  char * at ; 
  int a , b ;
  char *l , *r , *t , *c ; 
  unsigned long int status ;
  // if ( !files_to_recall ) return ; 
  GetWindowsDirectory ( filename , MAX_PATH ) ;
  strcat ( filename,"\\gb2tnt.opt") ;
  filefile = fopen ( filename,"wb") ;
  if ( filefile == NULL ) return ;
  saveanexe ( filefile , mafftexe ) ; 
  saveanexe ( filefile , muscleexe ) ; 
  saveanexe ( filefile , tntexe ) ; 
  saveanexe ( filefile , alignerexe ) ;
  saveanexe ( filefile , bioeditexe ) ;
  fprintf ( filefile , "%c" , aligner_is ) ; 
  fprintf ( filefile , "%c" , files_to_recall ) ;
  for ( a = 0 ; a < files_to_recall ; ++a ) {
      at = last_five_infiles[a] + 3 ;
      strcpy ( junkstr , at ) ;
      blanks_in ( junkstr ) ; 
      fprintf( filefile, "%s ", junkstr ) ; }
  fprintf( filefile, " " ) ;
  fclose ( filefile ) ;
  return ; 
}

void add_menu_old_file ( char * ptrnam )
{
    int a ;
    char nam[MAX_PATH] ; 
    int isdup = files_to_recall ;
    if ( !recall_more_files ) return ; 
    strcpy ( nam , ptrnam ) ; 
    for ( a = files_to_recall ; a-- ; ) {
        if ( eqfilename ( last_five_infiles[a] , nam ) ) { isdup = a ; break ; }}
    for ( a = isdup ; a ; a -- ) { 
              strcpy ( last_five_infiles[a] , last_five_infiles[a-1] ) ;
              ++ last_five_infiles[a][1] ; }
    last_five_infiles[0][0] = '&' ; 
    last_five_infiles[0][1] = '1' ; 
    last_five_infiles[0][2] = ' ' ; 
    last_five_infiles[0][3] = '\0' ;
    strcat ( last_five_infiles[0], nam ) ; 
    if ( files_to_recall < 5 && isdup == files_to_recall ) ++ files_to_recall ; 
    clear_menu_old_files () ;
    update_menu_old_files () ;
    mark_treefile = 0 ; 
    DrawMenuBar ( hwnd ) ; 
}         

void clear_menu_old_files ( void )
{   int a; 
    hmu = GetMenu ( hwnd ) ;
    a = 0 ;
    while ( a < files_to_recall ) { 
        DeleteMenu ( hmu , 5000 + a , MF_BYCOMMAND ) ;
        ++ a ; }
    if ( files_to_recall ) 
       DeleteMenu ( hmu , 5005 , MF_BYCOMMAND ) ;
} 
    
void update_menu_old_files( void )
{ int a;
  hmu = GetMenu(hwnd);
  if ( !files_to_recall ) return ; 
  a = 0 ;
  ufalala = lastfiles ; 
  while ( a < files_to_recall ) { 
    InsertMenuItem ( hmu , IDM_EXIT , FALSE , ufalala ) ;
    ++ ufalala ; 
    ++ a ; }
  if ( files_to_recall ) {
      ufalala = lastfiles + 5 ; 
      InsertMenuItem ( hmu , IDM_EXIT , FALSE , ufalala ) ; }
} 


HMENU pupu ; 
    
void blackit ( BOOL condit, UINT idis )
{
    if ( condit == FALSE )
        EnableMenuItem( pupu, idis, MF_GRAYED );
    else 
        EnableMenuItem( pupu, idis, MF_ENABLED );
} 

void markit ( BOOL condit, UINT idis )
{
    if ( condit == FALSE )
          CheckMenuItem( pupu , idis, MF_UNCHECKED );
    else CheckMenuItem( pupu , idis, MF_CHECKED );
}

int have_open_project = 0 ; 

void gray_menus ( void )
{ 
   pupu = GetMenu ( hwnd ) ;
   blackit ( have_open_project , IDM_ONESEQPERSP ) ; 
   markit ( dooneseqpersp , IDM_ONESEQPERSP ) ; 
   blackit ( ( have_open_project && have_something_to_save ) , IDM_SAVEPROJECT ) ; 
   blackit ( have_open_project , IDM_SAVEPROJECTASS ) ;
   blackit ( have_open_project , IDM_MODIFYPROJECT ) ;
   blackit ( have_open_project , IDM_CREATEMENU ) ;
   blackit ( have_open_project , IDM_GRALTAXONOMYOPTIONS ) ;
   blackit ( ( logfileis == NULL ) , IDM_SAVEREPORTS ) ;
   blackit ( ( logfileis != NULL ) , IDM_CLOSEREPORTS ) ;
   blackit ( bioeditexe[0] , IDM_BIOEDITCHECK ) ; 
   blackit ( mafftexe[0] , IDM_MAFFT ) ; 
   blackit ( muscleexe[0] , IDM_MUSCLE ) ; 
   blackit ( alignerexe[0] , IDM_CHOOSEALIGNER ) ;
   blackit ( have_open_project , IDM_ERASEALLTEMPS ) ;
   blackit ( have_open_project , IDM_TAXONOMYOPTIONS ) ; 
   markit ( ( aligner_is == MAFFT ) , IDM_MAFFT ) ; 
   markit ( ( aligner_is == MUSCLE ) , IDM_MUSCLE ) ; 
   markit ( ( aligner_is == USER ) , IDM_CHOOSEALIGNER ) ;
   markit ( show_full_paths , IDM_FULLPATHS ) ;
   markit ( show_rejects , IDM_SHOWREJECTS ) ;
   markit ( query_taxon_identity , IDM_QUERYIDENTITY ) ;
   markit ( save_accession_numbers , IDM_SAVEACCESSIONS ) ; 
   markit ( save_taxonomy , IDM_SAVETAXONOMY ) ; 
   DrawMenuBar ( hwnd ) ;
}

/** Output system **********/
#define Spewsize 1024 
static char vertbuff[ Spewsize ] , * nextvert = vertbuff ;

char * namov( char * from , char * to )
{
        while( * to ++ = * from ++ ) ;
        return( to - 1 ) ;
}

void reset_intvert ( void )
{
    nextvert = vertbuff ;
} 

char charvertbuf[3] ; 

char * charvert ( int x )
{
    charvertbuf[0]= x ;
    charvertbuf[1]='\0' ;
    return charvertbuf ;     
}

char * intvert( int x )
{
        char poo[ 60 ] , * zult ;
        itoa( x , poo , 10 ) ;
        nextvert = namov( poo , zult = nextvert ) + 1 ;
        if ( ( nextvert - vertbuff ) > Spewsize - 15 )
                nextvert = vertbuff ; 
        return( zult ) ;
}

char * floatvert ( int width , int precision, double num )
{
    char * p = nextvert ; 
    sprintf ( p , "%%%i.%if" , width , precision ) ;
    nextvert += ( strlen ( p ) + 1 ) ;
    sprintf ( nextvert , p , num ) ;
    p = nextvert ; 
    nextvert += strlen ( nextvert ) ; 
    return p ;
} 

char * wintvert( int width, int num )
{
    int a =0, b;
    char excha = 0 ; 
    char *p;
    if ( width < 0 ) { excha = 1; width = -width; } 
    p = intvert(num);
    while( p[a] ) ++a;
    b = width - a;
    if ( b <= 0 ) return ( p ) ; 
    p[a+b] = 0; 
    if ( ! excha ) { 
       while( a-- ) p[a+b] = p[a];
       while( b ) p[--b] = ' '; }
    else while( b--) p[a++] = ' '; 
    return ( p ) ; 
}     

static char spewbuff[ Spewsize + 64 ] , * nextspew = spewbuff ;

void spewer( void )
{
    if ( logfileis != NULL ) fprintf ( logfileis , "%s" , spewbuff ) ; 
    spewtogil ( spewbuff ) ;
}     

void spewout()
{
        if( nextspew > spewbuff )
        {
            if( nextspew[ -1 ] != '\n' ) * nextspew ++ = '\n' ;
            * nextspew = 0 ;
            spewer(); 
            nextspew = spewbuff ;
        }
        nextvert = vertbuff ;
}

char * strend( char * a )
{
    while( * a ) ++ a ;
    return( a ) ;
}

int  strlength( char * a)
{
    return( strend( a ) - a ) ;
}

char * tospew( char * a ) 
{
        if( nextspew <= spewbuff + Spewsize - strlength( a ) )
        {
            nextspew = namov( a , nextspew ) ;
            * nextspew ++ = ' ' ; 
        }
        return( nextspew ) ;
}

char * tojoin( char * a ) 
{
        if( nextspew <= spewbuff + Spewsize - strlength( a ) )
            nextspew = namov( a , nextspew ) ;

else
 ++ a  ;
            
        return( nextspew ) ;
}

void spewjn( void )  
{
        if( nextspew > spewbuff )
        {
            * nextspew = 0 ;
            spewer(); 
            nextspew = spewbuff ;
        }
        nextvert = vertbuff ;
}

void joinem( char * a , ... )
{
        char ** cpp = & a , * cp ;
        while( cp = * cpp ++ ) tojoin( cp ) ;
        spewjn() ;
}

void newln( void )
{
    joinem(" \n", NULL );
}

void myp ( void * i , ... )
{
    void ** ip = &i ;
    void * ipp = * ip ;
    int a ;
    // Assume this never happens... 
/*
    for ( a = 0 ; a < 10 ; ++ a )
        if ( ip [ a ] == junkstr ) {
               MessageBox( hwnd , "An error occured when calling MYP().\nMYP called to print JUNKSTR" "\nWould you please notify Pablo ?" , "OOPS!!!" , MB_ICONERROR ) ;
               return ; }
*/
    ipp = * ip ; 
    sprintf ( junkstr , ip [ 0 ]  , ip [ 1 ] , ip [ 2 ] , ip [ 3 ] , ip [ 4 ] , ip [ 5 ] , ip [ 6 ] , ip [ 7 ] , ip [ 8 ] , ip [ 9 ] ) ;
    joinem ( junkstr , 0 ) ;
    spewout () ; 
    return ; 
}

void spewem( char * a , ... )
{
        char ** cpp = & a , * cp ;
        while( cp = * cpp ++ ) tospew( cp ) ;
        spewout() ;
}

unsigned long int filesizeis ;

time_t getfilemodtime( char * nam )
{
   struct stat buf ;
   FILE * inpf = fopen ( nam , "rb" ) ;
   if ( inpf == NULL ) return -1 ; 
   fstat ( fileno ( inpf ) , &buf ) ;
   fclose ( inpf ) ;
   filesizeis = buf.st_size ;
   return buf.st_mtime ;
}

char damonth[12][4] =
      {
          { 'J','a','n','\0'  } , 
          { 'F','e','b','\0'  } , 
          { 'M','a','r','\0'  } , 
          { 'A','p','r','\0'  } , 
          { 'M','a','y','\0'  } , 
          { 'J','u','n','\0'  } , 
          { 'J','u','l','\0'  } , 
          { 'A','u','g','\0'  } , 
          { 'S','e','p','\0'  } , 
          { 'O','c','t','\0'  } , 
          { 'N','o','v','\0'  } , 
          { 'D','e','c','\0'  } } ;  

char daday[7][4] = { "Sun" , "Mon" , "Tue" , "Wed" , "Thu" , "Fri" , "Sat" } ; 
     
void showmoddate ( time_t dat )
{
  struct tm * gentim ;
   /*  tm members: 
     int    tm_sec   seconds [0,61]
     int    tm_min   minutes [0,59]
     int    tm_hour  hour [0,23]
     int    tm_mday  day of month [1,31]
     int    tm_mon   month of year [0,11]
     int    tm_year  years since 1900
     int    tm_wday  day of week [0,6] (Sunday = 0)
     int    tm_yday  day of year [0,365]
     int    tm_isdst daylight savings flag */ 
   gentim =  localtime ( &(dat) ) ;
    /*  E.g.:
         Mon  Jan 3 (12:55:22), 1989   */ 
   myp ( "%s, %s %i " , 
          daday [ gentim -> tm_wday ] , 
          damonth [ gentim -> tm_mon ] ,
          gentim -> tm_mday ) ; 
   myp ( "%i (" , gentim -> tm_year + 1900 ) ;
   if ( gentim -> tm_hour < 10 ) myp ( "0" ) ;
   myp ( "%i:" , gentim -> tm_hour ) ; 
   if ( gentim -> tm_min < 10 ) myp ( "0" ) ;
   myp ( "%i:" , gentim -> tm_min ) ; 
   if ( gentim -> tm_sec < 10 ) myp ( "0" ) ;
   myp ( "%i)" , gentim -> tm_sec ) ; 
   return ;
}    

/*****  THIS A SCOPE READING FUNCTION ... goes all the way to "end of scope reading"  ****/
void * winloram( unsigned long int size )  
{
        char * temp ;
        temp = malloc ( size ) ;
        return temp ; 
}

void markselgroups ( int * mygroups , char * mylist , short int * myscope , int mynum )
{   int a , b , cel ;
    unsigned long int flg ; 
    for ( a = 32 ; a-- ; ) {
        if ( !mylist [ a ] ) continue ; 
        flg = 1 << a ;
        for ( b = mynum ; b -- ; )
           if ( ( mygroups [ b ] & flg ) ) myscope [ b ] = 1 ; } 
} 

void nametheitem ( int a )
{
    if ( selthingnames && selthingnames[ a ] ) itemname = selthingnames[a] ;
    else {
        itemname = itemnamespace ;
        strcpy ( itemname , intvert ( a ) ) ;
        reset_intvert () ; } 
}

int prepare_selmem ()
{ static int a = 0 ;
  if ( a ) return 1 ; 
  a = MAXNUMBLOCKS ;
  if ( ( tmpcurlist = winloram ( a * sizeof ( int ) ) ) == NULL ) return 0 ; 
  if ( ( selectlist = winloram ( a * sizeof ( char ) ) ) == NULL ) return 0 ;
  return 1 ; 
} 

int invert_nodsel ; 

void prepare_selection ( char * thgs , char * unthgs , char ** thgnams, int numthgs )
{   int a ; 
    strcpy ( selthings, thgs ) ;
    strcpy ( unselthings , unthgs ) ; 
    selthingnames = thgnams ;
    selnumthings = numthgs ;
    for ( a = numthgs ; a-- ; ) tmpcurlist [ a ] = selectlist [ a ] = 0 ;
    invert_nodsel = 0 ; 
}

int namtrun ( char * a , char * b )
{  // is A a trunc of B ??
   while ( tolower ( * a ) == tolower ( * b ) && * a ) { ++ a ; ++ b ; }
   if ( * a ) return 0 ;
   return 1 ; 
}

int these_are_files ;
int hide_paths = 1 ;
int doing_copy_options = 0 ;
int src_block_for_copy ; 
int skip_some_source_files = 0 ; 

BOOL CALLBACK SelectFunc(HWND hdwnd, UINT message, WPARAM wParam, LPARAM lParam )  
{
  static HWND hselwnd , hunselwnd , hWho ;
  int a , b , insel, inunsel ;
  int selitems , x , y ;
  char * strg ;
  switch(message) {
          case WM_INITDIALOG :

hide_paths = 1 - show_full_paths ; 
if ( !these_are_files ) {
    ShowWindow ( GetDlgItem ( hdwnd , 113 ) , FALSE ) ; 
    ShowWindow ( GetDlgItem ( hdwnd , 114 ) , FALSE ) ; }
else 
    if ( !hide_paths ) BUTT_CHECK ( 113 ) ; 
          
             hselwnd= GetDlgItem ( hdwnd , 105 ) ; 
             hunselwnd= GetDlgItem ( hdwnd , 106 ) ;
             insel = inunsel = 0 ;
             for ( a = 0 ; a < selnumthings ; a ++ ) {
                 if ( doing_copy_options && a == src_block_for_copy ) continue ;
                 nametheitem ( a ) ; 
                 if ( ( skip_some_source_files & FASTA ) && isfile_of_type ( itemname , "fas" ) ) continue ; 
                 if ( ( skip_some_source_files & ALIGNED ) && isfile_of_type ( itemname , "aln" ) ) continue ; 
                 if ( ( skip_some_source_files & TNT ) && isfile_of_type ( itemname , "tnt" ) ) continue ; 
                 if ( selthingnames[a] == NULL ) continue ;  // this can only happen for lists of blocks containging ALN or TNT as source... 
                 if ( !selectlist [ a ] ) {
                         if ( hide_paths ) ListBox_InsertString ( hunselwnd , inunsel , squeeze_filename ( itemname ) ) ;
                         else                          
                         ListBox_InsertString ( hunselwnd , inunsel , itemname ) ;
                         ListBox_SetItemData ( hunselwnd , inunsel , a ) ; 
                         inunsel ++ ; }
                 if ( selectlist [ a ] == 1 ) { 
                         if ( hide_paths ) ListBox_InsertString ( hselwnd , insel , squeeze_filename ( itemname ) ) ;
                         else                          
                         ListBox_InsertString ( hselwnd , insel , itemname ) ;
                         ListBox_SetItemData ( hselwnd , insel , a ) ; 
                         insel ++ ; }}
             skip_some_source_files = 0 ; 
             wsprintf( junkstr, "%i %s", insel, selthings ) ; 
             SetDlgItemText(hdwnd, 107, junkstr);
             wsprintf ( junkstr, "%i %s", inunsel, unselthings ) ; 
             SetDlgItemText(hdwnd, 108, junkstr);
             SetWindowRedraw ( hunselwnd , TRUE ) ; 
             SetWindowRedraw ( hselwnd , TRUE ) ;
             InvalidateRect ( hunselwnd , NULL , TRUE ) ; 
             InvalidateRect ( hselwnd , NULL , TRUE ) ;
             FOCUS_ON (IDOK ) ; 
             break;

          case WM_COMMAND :

                 switch ( HIWORD ( wParam )) {  
                    case LBN_DBLCLK : //   first move to "selected", then to "unselected"
                           selitems = SendMessage ( hunselwnd , LB_GETSELITEMS , selnumthings, tmpcurlist ) ;
                           SetWindowRedraw ( hunselwnd , FALSE ) ; 
                           SetWindowRedraw ( hselwnd , FALSE ) ;
                           y = ListBox_GetCount ( hselwnd ) ;
                           b = 0 ; 
                           for ( a = 0 ; a < selitems ; a ++ ) {
                                 x = ListBox_GetItemData ( hunselwnd , tmpcurlist[a] ) ;
                                 nametheitem ( x ) ; 
                                 for (  ; b < y ; ++ b ) 
                                       if ( x < ListBox_GetItemData ( hselwnd , b ) ) break ;

if ( hide_paths ) ListBox_InsertString ( hselwnd , b , squeeze_filename ( itemname ) ) ;
else                          
                                 ListBox_InsertString ( hselwnd , b , itemname ) ;
                                 ListBox_SetItemData ( hselwnd , b , x ) ;
                                 ++ y ;  ++b ; }
                           for ( a = selitems ; a-- ; ) 
                                 ListBox_DeleteString ( hunselwnd , tmpcurlist[a] ) ; 
                           selitems = SendMessage ( hselwnd , LB_GETSELITEMS , selnumthings, tmpcurlist ) ;
                           y = ListBox_GetCount ( hunselwnd ) ;
                           b = 0 ; 
                           for ( a = 0 ; a < selitems ; a ++ ) {
                                 x = ListBox_GetItemData ( hselwnd , tmpcurlist[a] ) ;
                                 nametheitem ( x ) ; 
                                 for (  ; b < y ; ++ b ) 
                                       if ( x < ListBox_GetItemData ( hunselwnd , b ) ) break ;

if ( hide_paths ) ListBox_InsertString ( hunselwnd , b , squeeze_filename ( itemname ) ) ;
else                          
                                 ListBox_InsertString ( hunselwnd , b , itemname ) ;
                                 ListBox_SetItemData ( hunselwnd , b , x ) ;
                                 ++ y ;  ++b ; }
                           for ( a = selitems ; a-- ; ) 
                                 ListBox_DeleteString ( hselwnd , tmpcurlist[a] ) ; 
                           SetWindowRedraw ( hunselwnd , TRUE ) ; 
                           SetWindowRedraw ( hselwnd , TRUE ) ;
                           insel = ListBox_GetCount ( hselwnd ) ;
                           inunsel = ListBox_GetCount ( hunselwnd ) ;
                           InvalidateRect ( hunselwnd , NULL , TRUE ) ; 
                           InvalidateRect ( hselwnd , NULL , TRUE ) ;
                           wsprintf( junkstr, "%i %s", insel, selthings ) ; 
                           SetDlgItemText(hdwnd, 107, junkstr);
                           wsprintf ( junkstr, "%i %s", inunsel, unselthings ) ; 
                           SetDlgItemText(hdwnd, 108, junkstr);
                           break ;
                      default: break ; } 
              switch(LOWORD(wParam)){


case 113:
  hide_paths = 1 - hide_paths ; 
  SetWindowRedraw ( hunselwnd , FALSE ) ; 
  SetWindowRedraw ( hselwnd , FALSE ) ;
  selitems = ListBox_GetCount ( hselwnd ) ; 
  for ( a = selitems ; a -- ; )  
      ListBox_DeleteString ( hselwnd , 0 ) ;
  selitems = ListBox_GetCount ( hunselwnd ) ; 
  for ( a = selitems ; a -- ; )  
      ListBox_DeleteString ( hunselwnd , 0 ) ;

             insel = inunsel = 0 ;
             for ( a = 0 ; a < selnumthings ; a ++ ) {
                 if ( !selectlist [ a ] ) {
                         nametheitem ( a ) ;

if ( hide_paths ) ListBox_InsertString ( hunselwnd , inunsel , squeeze_filename ( itemname ) ) ;
else                          
                         ListBox_InsertString ( hunselwnd , inunsel , itemname ) ;
                         ListBox_SetItemData ( hunselwnd , inunsel , a ) ; 
                         inunsel ++ ; }
                 if ( selectlist [ a ] == 1 ) { 
                         nametheitem ( a ) ;

if ( hide_paths ) ListBox_InsertString ( hselwnd , insel , squeeze_filename ( itemname ) ) ;
else                          
                         ListBox_InsertString ( hselwnd , insel , itemname ) ;
                         ListBox_SetItemData ( hselwnd , insel , a ) ; 
                         insel ++ ; }}
  SetWindowRedraw ( hunselwnd , TRUE ) ; 
  SetWindowRedraw ( hselwnd , TRUE ) ;
  InvalidateRect ( hunselwnd , NULL , TRUE ) ; 
  InvalidateRect ( hselwnd , NULL , TRUE ) ;
  break ; 
                  
                        case 112 : // invert selection
                           SetWindowRedraw ( hunselwnd , FALSE ) ; 
                           SetWindowRedraw ( hselwnd , FALSE ) ;
                           selitems = ListBox_GetCount ( hselwnd ) ; 
                           y = ListBox_GetCount ( hunselwnd ) ;
                           for ( a = 0 ; a < selitems ; a ++ ) {
                                 x = ListBox_GetItemData ( hselwnd , a ) ;
                                 nametheitem ( x ) ; 
if ( hide_paths ) ListBox_InsertString ( hunselwnd , a , squeeze_filename ( itemname ) ) ;
else                          
                                 ListBox_InsertString ( hunselwnd , a , itemname ) ;
                                 ListBox_SetItemData ( hunselwnd , a , x ) ; } 
                           for ( a = selitems ; a -- ; )  
                                 ListBox_DeleteString ( hselwnd , 0 ) ;
                           b = selitems ;       
                           for ( a = 0 ; a < y ; a ++ ) {
                                 x = ListBox_GetItemData ( hunselwnd , b ) ;
                                 nametheitem ( x ) ; 
if ( hide_paths ) ListBox_InsertString ( hselwnd , a , squeeze_filename ( itemname ) ) ;
else                          
                                 ListBox_InsertString ( hselwnd , a , itemname ) ;
                                 ListBox_SetItemData ( hselwnd , a , x ) ;
                                 ++ b ; } 
                           for ( a = y ; a -- ; ) 
                                 ListBox_DeleteString ( hunselwnd , selitems ) ;
                           SetWindowRedraw ( hunselwnd , TRUE ) ; 
                           SetWindowRedraw ( hselwnd , TRUE ) ;
                           insel = ListBox_GetCount ( hselwnd ) ;
                           inunsel = ListBox_GetCount ( hunselwnd ) ;
                           InvalidateRect ( hunselwnd , NULL , TRUE ) ; 
                           InvalidateRect ( hselwnd , NULL , TRUE ) ;
                           wsprintf( junkstr, "%i %s", insel, selthings ) ; 
                           SetDlgItemText(hdwnd, 107, junkstr);
                           wsprintf ( junkstr, "%i %s", inunsel, unselthings ) ; 
                           SetDlgItemText(hdwnd, 108, junkstr);
                           break ; 

                        case 110 : // Move to "selected"
                           GetDlgItemText ( hdwnd , 111 , junkstr , NAMESIZE ) ;
                           if ( !strlen ( junkstr )) 
                                selitems = SendMessage ( hunselwnd , LB_GETSELITEMS , selnumthings, tmpcurlist ) ;
                           else {
                               y = ListBox_GetCount ( hunselwnd ) ;
                               if ( isdigit ( junkstr [ 0 ] ) ) {
                                   a = atoi ( junkstr ) ;
                                   if ( a >= selnumthings ) {
                                          wsprintf ( junkstr , "Only %i items (0-%i) to select from!", selnumthings , selnumthings-1 ) ; 
                                          MessageBox(hdwnd, junkstr ,"ERROR",MB_OK | MB_ICONERROR ); break ; }
                                   selitems = 0 ; 
                                   for ( b = y ; b -- ; ) if ( ListBox_GetItemData ( hunselwnd , b ) == a ) { selitems = 1 ; break ; }
                                   if ( selitems ) tmpcurlist [ 0 ] = b ;
                                   else { wsprintf ( junkstr , "Item %i is already selected", a ) ; 
                                           MessageBox(hdwnd, junkstr ,"ERROR",MB_OK | MB_ICONERROR ); break ; }}
                               else {
                                   selitems = 0 ; 
                                   for ( b = 0 ; b < y ; ++ b ) {
                                       x = ListBox_GetItemData ( hunselwnd, b ) ;
                                       nametheitem ( x ) ;
                                       if ( namtrun ( junkstr , itemname ) )
                                                tmpcurlist [ selitems ++ ] = b ; }
                                   if ( !selitems ) {
                                      MessageBox(hdwnd, "Name doesn't match any unselected item" ,"ERROR",MB_OK | MB_ICONERROR );
                                      break ; }}
                              SetDlgItemText(hdwnd, 111, ""); }
                           SetWindowRedraw ( hunselwnd , FALSE ) ; 
                           SetWindowRedraw ( hselwnd , FALSE ) ;
                           y = ListBox_GetCount ( hselwnd ) ;
                           b = 0 ; 
                           for ( a = 0 ; a < selitems ; a ++ ) {
                                 x = ListBox_GetItemData ( hunselwnd , tmpcurlist[a] ) ;
                                 nametheitem ( x ) ; 
                                 for (  ; b < y ; ++ b ) 
                                       if ( x < ListBox_GetItemData ( hselwnd , b ) ) break ;

if ( hide_paths ) ListBox_InsertString ( hselwnd , b , squeeze_filename ( itemname ) ) ;
else                          
                                 ListBox_InsertString ( hselwnd , b , itemname ) ;
                                 ListBox_SetItemData ( hselwnd , b , x ) ;
                                 ++ y ;  ++b ; }
                           for ( a = selitems ; a-- ; ) 
                                 ListBox_DeleteString ( hunselwnd , tmpcurlist[a] ) ; 
                           SetWindowRedraw ( hunselwnd , TRUE ) ; 
                           SetWindowRedraw ( hselwnd , TRUE ) ;
                           insel = ListBox_GetCount ( hselwnd ) ;
                           inunsel = ListBox_GetCount ( hunselwnd ) ;
                           InvalidateRect ( hunselwnd , NULL , TRUE ) ; 
                           InvalidateRect ( hselwnd , NULL , TRUE ) ;
                           wsprintf( junkstr, "%i %s", insel, selthings ) ; 
                           SetDlgItemText(hdwnd, 107, junkstr);
                           wsprintf ( junkstr, "%i %s", inunsel, unselthings ) ; 
                           SetDlgItemText(hdwnd, 108, junkstr);
                           break ; 

                        case 109 : // Move to "un-selected"
                           GetDlgItemText ( hdwnd , 111 , junkstr , NAMESIZE ) ;
                           if ( !strlen ( junkstr )) 
                                selitems = SendMessage ( hselwnd , LB_GETSELITEMS , selnumthings, tmpcurlist ) ;
                           else {
                               y = ListBox_GetCount ( hselwnd ) ;
                               if ( isdigit ( junkstr [ 0 ] ) ) {
                                   a = atoi ( junkstr ) ;
                                   if ( a >= selnumthings ) {
                                          wsprintf ( junkstr , "Only %i items (0-%i) to select from!", selnumthings , selnumthings-1 ) ; 
                                          MessageBox(hdwnd, junkstr ,"ERROR",MB_OK | MB_ICONERROR ); break ; }
                                   selitems = 0 ; 
                                   for ( b = y ; b -- ; ) if ( ListBox_GetItemData ( hselwnd , b ) == a ) { selitems = 1 ; break ; }
                                   if ( selitems ) tmpcurlist [ 0 ] = b ;
                                   else { wsprintf ( junkstr , "Item %i is already selected", a ) ; 
                                           MessageBox(hdwnd, junkstr ,"ERROR",MB_OK | MB_ICONERROR ); break ; }}
                               else {
                                   selitems = 0 ; 
                                   for ( b = 0 ; b < y ; ++ b ) {
                                       x = ListBox_GetItemData ( hselwnd, b ) ;
                                       nametheitem ( x ) ;
                                       if ( namtrun ( junkstr , itemname ) )
                                                tmpcurlist [ selitems ++ ] = b ; }
                                   if ( !selitems ) {
                                      MessageBox(hdwnd, "Name doesn't match any selected item" ,"ERROR",MB_OK | MB_ICONERROR );
                                      break ; }}
                               SetDlgItemText(hdwnd, 111, ""); }
                           SetWindowRedraw ( hunselwnd , FALSE ) ; 
                           SetWindowRedraw ( hselwnd , FALSE ) ;
                           y = ListBox_GetCount ( hunselwnd ) ;
                           b = 0 ; 
                           for ( a = 0 ; a < selitems ; a ++ ) {
                                 x = ListBox_GetItemData ( hselwnd , tmpcurlist[a] ) ;
                                 nametheitem ( x ) ; 
                                 for (  ; b < y ; ++ b ) 
                                       if ( x < ListBox_GetItemData ( hunselwnd , b ) ) break ;
if ( hide_paths ) ListBox_InsertString ( hunselwnd , b , squeeze_filename ( itemname ) ) ;
else                          
                                 ListBox_InsertString ( hunselwnd , b , itemname ) ;
                                 ListBox_SetItemData ( hunselwnd , b , x ) ;
                                 ++ y ;  ++b ; }
                           for ( a = selitems ; a-- ; ) 
                                 ListBox_DeleteString ( hselwnd , tmpcurlist[a] ) ; 
                           SetWindowRedraw ( hunselwnd , TRUE ) ; 
                           SetWindowRedraw ( hselwnd , TRUE ) ;
                           insel = ListBox_GetCount ( hselwnd ) ;
                           inunsel = ListBox_GetCount ( hunselwnd ) ;
                           InvalidateRect ( hunselwnd , NULL , TRUE ) ; 
                           InvalidateRect ( hselwnd , NULL , TRUE ) ;
                           wsprintf( junkstr, "%i %s", insel, selthings ) ; 
                           SetDlgItemText(hdwnd, 107, junkstr);
                           wsprintf ( junkstr, "%i %s", inunsel, unselthings ) ; 
                           SetDlgItemText(hdwnd, 108, junkstr);
                           break ; 
                        case IDOK :
                           if ( these_are_files ) show_full_paths = 1 - hide_paths ; 
                           insel = ListBox_GetCount ( hselwnd ) ;
                           for ( a = selnumthings ; a-- ; ) selectlist [ a ] = 0 ; 
                           for ( b = insel ; b -- ; ) 
                               selectlist [ ListBox_GetItemData ( hselwnd , b ) ] = 1 ;  
                           EndDialog ( hdwnd, 1 ) ;
                           break ; 
                        case IDCANCEL: 
                           EndDialog ( hdwnd, 0 ) ;
                           break ; 
                        default :
                           break;
                }
         break; }
    return 0;
}

int StartSelectFun( HWND hdwnd )
{
    int setto , a , b ;
    short int * vt ;
    a = DialogBox ( hInst, "Selectdb", hdwnd , (DLGPROC) SelectFunc ) ;
    return ;
} 
/********* end of scope reading function ***********/ 


void handle_savings ( int i )
{
    if ( ( i == IDM_SAVEPROJECTASS ) || unnamed_project ) {
       fn[0]='\0';
       memset(&opbuff, 0, sizeof(OPENFILENAME));
       opbuff.lStructSize = sizeof(OPENFILENAME);
       opbuff.hwndOwner =hwnd;
       opbuff.lpstrFilter = filefilter;
       opbuff.nFilterIndex = 1;
       opbuff.lpstrFile = fn;
       opbuff.lpstrTitle = "Save Project as...";
       opbuff.nMaxFile = sizeof(fn);
       opbuff.lpstrFileTitle = filename;
       opbuff.nMaxFileTitle = sizeof(filename)-1;
       opbuff.lpstrDefExt = "g2t";
       opbuff.Flags = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST ;
       //if(GetOpenFileName(&opbuff)) {
       if(GetSaveFileName( &opbuff )) { 
          SetCursor(hGlass);
          sprintf ( junkstr , "GenBank to TNT - %s" , fn ) ; 
          SetWindowText ( hwnd , junkstr ) ; 
          save_the_project ( fn ) ;
          add_menu_old_file ( fn ) ;
          strcpy ( project_name , fn ) ;
          myp ( "Saved %s (%i blocks)" , project_name , proj_numblocks ) ; newln () ; eofscreen () ;
          unnamed_project = 0 ; 
          SetCursor(hNormal); }}
    else {
        save_the_project ( project_name ) ;
        myp ( "Saved %s (%i blocks)" , project_name , proj_numblocks ) ; newln () ; eofscreen () ; }
    return ; 
}

void handle_exe_locations ( char * xnam , char * tit ) 
{
    fn[0]='\0';
    memset(&opbuff, 0, sizeof(OPENFILENAME));
    opbuff.lStructSize = sizeof(OPENFILENAME);
    opbuff.hwndOwner =hwnd;
    if ( xnam == mafftexe ) 
        opbuff.lpstrFilter = "batch files (*.bat)\0*.bat\0";
    else opbuff.lpstrFilter = "executable files (*.exe)\0*.exe\0";
    opbuff.nFilterIndex = 1;
    opbuff.lpstrFile = fn;
    opbuff.lpstrTitle = tit ;
    opbuff.nMaxFile = sizeof(fn);
    opbuff.lpstrFileTitle = filename;
    opbuff.nMaxFileTitle = sizeof(filename)-1;
    opbuff.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY ;
    * xnam ++ = '\"' ; 
    if(GetOpenFileName(&opbuff))
         strcpy ( xnam , fn ) ; 
    strcat ( xnam , "\"" ) ; 
    InvalidateRect(hwnd,NULL,1);
    return ; 
}

char dadate[ 100 ] ; 
char * curtimestring ( void ) 
{
   struct tm * gentim ;
   time_t dat ;
   time ( &dat ) ; 
   gentim =  localtime ( &(dat) ) ;
   sprintf ( dadate , "%s, %s %i (" , 
          daday [ gentim -> tm_wday ] , 
          damonth [ gentim -> tm_mon ] ,
          gentim -> tm_mday ) ; 
   if ( gentim -> tm_hour < 10 ) strcat ( dadate , "0" ) ;
   sprintf ( dadate + strlen ( dadate ) , "%i:" , gentim -> tm_hour ) ; 
   if ( gentim -> tm_min < 10 ) strcat ( dadate , "0" ) ;
   sprintf ( dadate + strlen ( dadate ) , "%i:" , gentim -> tm_min ) ; 
   if ( gentim -> tm_sec < 10 ) strcat ( dadate , "0" ) ;
   sprintf ( dadate + strlen ( dadate ) , "%i)" , gentim -> tm_sec ) ; 
   sprintf ( dadate + strlen ( dadate ) , " %i" , gentim -> tm_year + 1900 ) ;
   return dadate ;
}    

BOOL CALLBACK GetTaxonomyChangesFunc (HWND hdwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int i ;
    char * cp ; 
    switch(message) {
        case WM_INITDIALOG :
           cp = project_name + strlen ( project_name ) ;
           while ( cp > project_name && * cp != '\\' ) -- cp ; 
           i = * cp ;
           if ( * cp == '\\' ) * cp = '\0' ;
           sprintf ( junkstr , "Change all GB files now in %s" , project_name ) ;
           if ( i == '\\' ) * cp = i ;
           SetWindowText ( hdwnd , junkstr ) ; 
           break ;
        case WM_COMMAND:
           switch ( LOWORD ( wParam ) ) {
               case IDOK:
                 GetDlgItemText( hdwnd, 201, junkstr , 1000 ) ;
                 cp = junkstr + strlen ( junkstr ) + 1 ;
                 GetDlgItemText( hdwnd, 202 , cp , 1000 ) ;
                 EndDialog ( hdwnd , 1 ) ; return 1 ; break ;
               case IDCANCEL: 
                 EndDialog ( hdwnd , 0 ) ; return 0 ; break ;
               default: break ; }
            break ;
        default: break ; }
    return 0 ;
}

static char * frombuf ;
static char * fromgenus , * togenus ; 

int isgbfile ( char * what )
{
    char * cp = what + strlen ( what ) ;
    while ( cp > what && * cp != '.' ) cp -- ;
    if ( strlen ( cp ) != 3 ) return 0 ;
    if ( tolower ( cp[1] ) != 'g' || tolower ( cp[2] ) != 'b' ) return 0 ;
    return 1 ; 
}

char outbuf[258] , * outbufis , * outbuflim ; 
char inpbuf[BUFSIZ] , outpbuf[BUFSIZ] ; 

void substitute_in_this_file ( char * fnam , char * from , char * to )
{
    FILE * fp , * op ;
    char x ;
    int fromlen = strlen ( from ) ;
    int changes = 0 ;
    char * cp , * fromlim , * bp ; 
    fp = fopen ( fnam , "rb" ) ;
    op = fopen ( "gb2tnt.tmp" , "wb" ) ;
    if ( fp == NULL ) {
        myp ( "CAN'T OPEN FILE %s FOR INPUT\n" , fnam ) ; return ; }
    if ( op == NULL ) {
        myp ( "CAN'T OPEN TEMP FILE FOR OUTPUT\n" ) ; return ; }
    setbuf ( fp , inpbuf ) ;
    setbuf ( op , outpbuf ) ; 
    fromlim = frombuf + fromlen ;
    outbufis = outbuf ;
    outbuflim = outbuf + 254 ; 
    while ( !feof ( fp ) ) {
         x = getc ( fp ) ;
         if ( feof ( fp ) ) break ; 
         if ( x == * from ) {
              * outbufis = '\0' ;
              * ( cp = frombuf ) = x ;
              cp ++ ;
              bp = from + 1 ; 
              while ( !feof ( fp ) && cp < fromlim ) {
                   * cp = getc ( fp ) ;
                   if ( * cp ++ != * bp ++ ) break ; }
              * cp = '\0' ; 
              if ( !strcmp ( frombuf , from ) ) {
                   ++ changes ;
                   fprintf ( op , "%s%s" , outbuf , to ) ; }
              else fprintf ( op , "%s%s" , outbuf , frombuf ) ;
              outbufis = outbuf ; }
         else {
             * outbufis ++ = x ;
             if ( outbufis == outbuflim ) {
                 * outbufis = '\0' ;
                 fprintf ( op , "%s" , outbuf ) ;
                 outbufis = outbuf ; }}}
    if ( outbufis != outbuf ) {
         * outbufis = '\0' ;
         fprintf ( op , "%s" , outbuf ) ; }
    fflush ( op ) ; 
    fclose ( fp ) ; 
    fclose ( op ) ;
    /***  Now, read back and copy... *******/ 
    if ( changes )  {
        fp = fopen ( fnam , "wb" ) ;
        op = fopen ( "gb2tnt.tmp" , "rb" ) ;
        setbuf ( fp , inpbuf ) ;
        setbuf ( op , outpbuf ) ; 
        outbufis = outbuf ;
        outbuflim = outbuf + 254 ; 
        while ( !feof ( op ) ) {
             x = fgets( outbuf , 255 , op ) ;
             fputs ( outbuf , fp ) ; }
        fflush ( fp ) ; 
        fclose ( fp ) ; 
        fclose ( op ) ; } 
    myp ( "   ...%i substitutions made to %s\n" , changes , fnam ) ;
    return ; 
}

void handle_taxonomic_changes_higher ( void )
{
    int i ;
    struct dirent * direntp ;
    DIR * dirp ;
    char dname[MAX_PATH], * dbufp = dname ; 
    unsigned long int maxsize = 0 , thissize ;
    char * from , * to ;
    char * fromis , * tois ; 
    HWND hProgWnd ;
    i = DialogBox ( hInst , "GetTaxonomyChangesDB" , hwnd , (DLGPROC) GetTaxonomyChangesFunc ) ;
    if ( !i ) return ;
    from = to = junkstr ;
    while ( * to ) ++ to ;
    ++ to ;
    fromis = malloc ( ( strlen ( from ) + 1 ) * sizeof ( char ) ) ; 
    tois = malloc ( ( strlen ( to ) + 1 ) * sizeof ( char ) ) ;
    strcpy ( fromis , from ) ;
    strcpy ( tois , to ) ; 
    strcpy ( dname , project_name ) ;
    dbufp = dname + strlen ( dname ) ;
    while ( * dbufp != '\\' && dbufp > dname ) -- dbufp ;
    if ( * dbufp == '\\' ) * dbufp = '\0' ;
    else strcpy ( dbufp , "\\" ) ; 
    SetCurrentDirectory ( dname ) ;     
    getcwd ( dbufp , MAX_PATH ) ;
    dirp = opendir ( dbufp ) ;
    if ( dirp == NULL ) {
      sprintf ( junkstr , "No GenBank file(s) found in directory \"%s\"" , dname ) ; 
      MessageBox ( hwnd , junkstr , "ERROR" , MB_OK ) ;
      return ; }
    hProgWnd = progress_wnd () ;
    myp ( "\nSubstituting...\n" ) ;
    SetWindowText ( hProgWnd , "Substituting...please wait" ) ; 
    /*  Now, process  */ 
    frombuf = malloc ( ( strlen ( from ) + 5 ) * sizeof ( char ) ) ;
    for ( direntp = readdir ( dirp ) ; direntp != NULL ; direntp = readdir ( dirp ) ) 
      if ( isgbfile ( direntp->d_name ) ) {
          SetDlgItemText( hProgWnd , 200 , direntp -> d_name ) ;
          UpdateWindow ( hProgWnd ) ; 
          substitute_in_this_file ( direntp -> d_name , fromis , tois ) ; }
    DestroyWindow ( hProgWnd ) ;
    closedir ( dirp ) ;
    free ( frombuf ) ;
    free ( fromis ) ;
    free ( tois ) ; 
    eofscreen () ; 
    return ;
}

void handle_species_in_this_file ( char * fnam , char * from , char * to )
{
    FILE * fp , * op ;
    char x ;
    int fromlen = strlen ( from ) ;
    int changes = 0 ;
    char * cp , * fromlim , * bp ;
    fp = fopen ( fnam , "rb" ) ;
    op = fopen ( "gb2tnt.tmp" , "wb" ) ;
    if ( fp == NULL ) {
        myp ( "CAN'T OPEN FILE %s FOR INPUT\n" , fnam ) ; return ; }
    if ( op == NULL ) {
        myp ( "CAN'T OPEN TEMP FILE FOR OUTPUT\n" ) ; return ; }
    setbuf ( fp , inpbuf ) ;
    setbuf ( op , outpbuf ) ; 
    fromlim = frombuf + fromlen ;
    outbufis = outbuf ;
    outbuflim = outbuf + 254 ; 
    while ( !feof ( fp ) ) {
         x = getc ( fp ) ;
         if ( feof ( fp ) )
             break ;
         if ( x == * from ) {
              * outbufis = '\0' ;
              * ( cp = frombuf ) = x ;
              bp = from + 1 ; 
              cp ++ ; 
              while ( !feof ( fp ) && cp < fromlim ) {
                   * cp = getc ( fp ) ;
                   if ( * cp ++ != * bp ++ ) break ; }
              * cp = '\0' ; 
              if ( !strcmp ( frombuf , from ) ) {
                   ++ changes ;
                   fprintf ( op , "%s%s" , outbuf , to ) ;
                   if ( strcmp ( fromgenus , togenus ) ) {
                       cp = junkstr ;
                       x = ' ' ; 
                       while ( x != '.' && cp - junkstr < 5000 ) 
                           * cp ++ = x = getc ( fp ) ;
                       if ( * cp == '.' ) cp -- ;
                       while ( isspace ( * cp ) ) -- cp ;  // just in case there's a space between the genus name and the period... is it possible??? 
                       while ( !isspace ( * cp ) ) -- cp ;  // skip the last name
                       * ++ cp = '\0' ;
                       fprintf ( op , "%s%s." , junkstr , togenus ) ; }}
              else fprintf ( op , "%s%s" , outbuf , frombuf ) ; 
              outbufis = outbuf ; }
         else {
             * outbufis ++ = x ;
             if ( outbufis == outbuflim ) {
                 * outbufis = '\0' ;
                 fprintf ( op , "%s" , outbuf ) ;
                 outbufis = outbuf ; }}}
    if ( outbufis != outbuf ) {
         * outbufis = '\0' ;
         fprintf ( op , "%s" , outbuf ) ; }
    fflush ( op ) ; 
    fclose ( fp ) ; 
    fclose ( op ) ;
    /***  Now, read back and copy... *******/
    if ( changes ) {
        fp = fopen ( fnam , "wb" ) ;
        op = fopen ( "gb2tnt.tmp" , "rb" ) ;
        if ( fp == NULL ) {
            myp ( "CAN'T OPEN FILE %s FOR OUTPUT\n" , fnam ) ; return ; }
        if ( op == NULL ) {
            myp ( "CAN'T OPEN TEMP FILE FOR INPUT\n" ) ; return ; }
        setbuf ( fp , inpbuf ) ;
        setbuf ( op , outpbuf ) ; 
        outbufis = outbuf ;
        outbuflim = outbuf + 254 ; 
        while ( !feof ( op ) ) {
             x = fgets( outbuf , 255 , op ) ;
             fputs ( outbuf , fp ) ; } 
        fflush ( fp ) ; 
        fclose ( fp ) ; 
        fclose ( op ) ; } 
    myp ( "   ...%i substitutions made to %s\n" , changes , fnam ) ;
    return ; 
}

char * find_n_alloc_genus_name ( char * where )
{
    char * cp = where ;
    char * op = junkstr ;
    char * mloc ; 
    while ( isspace ( * cp ) && * cp ) ++ cp ;
    while ( !isspace ( * cp ) && * cp ) cp ++ ;  // SKIP ORGANISM... it's always there!
    while ( isspace ( * cp ) && * cp ) ++ cp ;
    while ( !isspace ( * cp ) && * cp ) * op ++ = * cp ++ ;
    * op ++ = '\0' ;
    while ( isspace ( * cp ) && * cp ) ++ cp ;
    if ( !* cp ) {
        MessageBox ( hwnd , "Invalid species name!\nSpecies names must be binominals..." , "ERROR" , MB_OK ) ; 
        return NULL ; }
    while ( !isspace ( * cp ) && * cp ) cp ++ ;
    while ( * cp ) {
        if ( !isspace ( * cp ) ) {
        MessageBox ( hwnd , "Invalid species name!\nSpecies names must be binominals..." , "ERROR" , MB_OK ) ; 
        return NULL ; }
        ++ cp ; }
    mloc = malloc ( ( op - junkstr ) * sizeof ( char ) ) ;
    strcpy ( mloc , junkstr ) ; 
    return mloc ; 
}    

void handle_taxonomic_changes_lower ( void )
{
    int i ;
    struct dirent * direntp ;
    DIR * dirp ;
    char dname[MAX_PATH], * dbufp = dname ; 
    unsigned long int maxsize = 0 , thissize ;
    char * from , * to ;
    char * fromis , * tois ; 
    HWND hProgWnd ;
    i = DialogBox ( hInst , "GetTaxonomyChangesDB" , hwnd , (DLGPROC) GetTaxonomyChangesFunc ) ;
    if ( !i ) return ;
    from = to = junkstr ;
    while ( * to ) ++ to ;
    ++ to ;
    fromis = malloc ( ( strlen ( from ) + 12 ) * sizeof ( char ) ) ; 
    tois = malloc ( ( strlen ( to ) + 12 ) * sizeof ( char ) ) ;
    strcpy ( fromis , "ORGANISM  " ) ; 
    strcpy ( tois , "ORGANISM  " ) ; 
    strcat ( fromis , from ) ;
    strcat ( tois , to ) ; 
    if ( ( fromgenus = find_n_alloc_genus_name ( fromis ) ) == NULL ) return ; 
    if ( ( togenus = find_n_alloc_genus_name ( tois ) ) == NULL ) return ;  
    strcpy ( dname , project_name ) ;
    dbufp = dname + strlen ( dname ) ;
    while ( * dbufp != '\\' && dbufp > dname ) -- dbufp ;
    if ( * dbufp == '\\' ) * dbufp = '\0' ;
    else strcpy ( dbufp , "\\" ) ; 
    SetCurrentDirectory ( dname ) ;     
    getcwd ( dbufp , MAX_PATH ) ;
    dirp = opendir ( dbufp ) ;
    if ( dirp == NULL ) {
      sprintf ( junkstr , "No GenBank file(s) found in directory \"%s\"" , dname ) ; 
      MessageBox ( hwnd , junkstr , "ERROR" , MB_OK ) ;
      return ; }
    hProgWnd = progress_wnd () ;
    myp ( "\nSubstituting...\n" ) ;
    SetWindowText ( hProgWnd , "Substituting...please wait" ) ; 
    /*  Now, process  */ 
    frombuf = malloc ( ( strlen ( fromis ) + 5 ) * sizeof ( char ) ) ;
    for ( direntp = readdir ( dirp ) ; direntp != NULL ; direntp = readdir ( dirp ) ) 
      if ( isgbfile ( direntp->d_name ) ) {
          SetDlgItemText( hProgWnd , 200 , direntp -> d_name ) ;
          UpdateWindow ( hProgWnd ) ; 
          handle_species_in_this_file ( direntp -> d_name , fromis , tois ) ; }
    DestroyWindow ( hProgWnd ) ;
    closedir ( dirp ) ;
    free ( frombuf ) ;
    free ( fromis ) ;
    free ( tois ) ;
    free ( fromgenus ) ;
    free ( togenus ) ; 
    eofscreen () ; 
    return ;
}

