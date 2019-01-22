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

#define _ok           IDOK
#define _cancel       IDCANCEL     
#define _left         100      
#define _right        101
#define _more         118
#define _less         119
#define _show         177
#define _states       124     
#define _frame        126
#define _UPLIST       200 // the list of files... 
#define _BLOCKNAME    300  // the name of the block 
#define _FILTER       400  // button for block options
#define _WIPE         500  // button to remove block 
#define _LOLIST       600  // the list of products/genes
#define _ADDFILE      700  // the button to add files
#define _addblock     120 

#define MAFFT  1 
#define MUSCLE 2 
#define USER   3

int blockalignfor , overrideblockspecs ; 
int skip_matrix_creation ; 
extern int dooneseqpersp ;

extern unsigned long int filesizeis ; 
int current_block , blocks_to_align, optcpy_from =-1 ; 
extern double threshold ;
int totfiles_to_do , use_user_params , autoruntnt ;
int show_full_paths = 0 ; 
extern int these_are_files , aligner_is ; 
int passed_through_init = 0 ; 
int first_timer = 1 ; 
char uplistintro[300] ;
char lolistintro[300] ; 
int unnamed_project = 0 ;
int have_something_to_save = 0  ; 

#define FOCUS_ON(x) SetFocus ( GetDlgItem ( hdwnd , x ) ) ; return FALSE ; 
#define SPINACCEL( x , y , z ) \
    { hWho = GetDlgItem ( hdwnd , ( x ) ) ; \
      SpinAccel ( hWho , ( y ) , ( z ) ) ; } 

#define   inva      InvalidateRect ( hwnd, NULL , 1 )
#define VTYPE extern 
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
#include <time.h>
#include <sys\utime.h>

Blocktyp intergen_buf ;
extern int have_open_project ; 
RECT WDim ;
RECT LBDim ; 
RECT CurDim ;
RECT USDim ;
int intlzd = 0 ; 

typedef struct { int w ; int h ; } Siztyp ;

BOOL CALLBACK GetBlockAlignmentFunc ( HWND , UINT , WPARAM , LPARAM ) ;
BOOL CALLBACK GetDoubleGeneFunc ( HWND , UINT , WPARAM , LPARAM ) ;
BOOL CALLBACK ProjectEditFunc ( HWND , UINT , WPARAM , LPARAM ) ;
BOOL CALLBACK OptionsFunc ( HWND , UINT , WPARAM , LPARAM ) ;
#define CTRL_DISABLE( ctrl ) Button_Enable ( GetDlgItem ( uswnd , ctrl ) , FALSE )   
#define CTRL_ENABLE( ctrl ) Button_Enable ( GetDlgItem ( uswnd , ctrl ) , TRUE )   
#define HCTRL(x) GetDlgItem ( uswnd , x )  

static char linend[] = { 13, 10 , '\0' } ; 
extern HWND hwnd ; 
HWND uswnd ;
extern HINSTANCE hInst ;

#define MAXBOX 10
int numboxes , numoption ;
int showfrom , showto ; 
int iamatx ; 
int inbox [ 33 ] ;

int numblocks = 1 , proj_numblocks ; 

int unsaved_data ; 
char ** selthingnames ;
char itemnamespace [ NAMESIZE + MAX_PATH ];
char * itemname ;

int ** edmat ;
int * tmpsellist ;
int have_changes ;

/****  THESE ARE VARIABLES USED IN SCOPE READING FUNCTION, BELOW *************/
#define SELECT_DIALOG StartSelectFun ( hdwnd )  
extern char selthings [ NAMESIZE ] , unselthings [ NAMESIZE ] ;
extern int selnumthings ;
extern int * tmpcurlist ; 
extern char * selectlist ;
/************END OF VARIABLES FOR SCOPE READING FUNCTION ********/

#define SETFLT( x , y ) \
          { sprintf ( junkstr , "%.3f" , ( double ) x / 1000 ) ; \
            SetDlgItemText ( uswnd , y , junkstr ) ; } 
#define GETFLT( x , y ) \ 
          { GetDlgItemText(uswnd, ( y ) , junkstr, 80); \ 
            x = atof(junkstr); } 
#define DERR( x ) { noisy() ; MessageBox ( hdwnd, x, "ERROR", MB_OK | MB_ICONERROR ) ; break ; } 
#define BUTT_CHECK( ctrl ) SendDlgItemMessage( hdwnd,  ctrl , BM_SETCHECK, (WPARAM) BST_CHECKED , 0 )
#define BUTT_UNCHEK( ctrl ) SendDlgItemMessage( hdwnd,  ctrl , BM_SETCHECK, (WPARAM) BST_UNCHECKED , 0 )
#define ON_SPIN case WM_VSCROLL
#define SPIN_DISABLE( ctrl ) \
                   { SetDlgItemText ( hdwnd , GetDlgCtrlID ( SendMessage( GetDlgItem ( hdwnd , ctrl ) , UDM_GETBUDDY, 0, 0) ) , "" ) ; \
                     Button_Enable ( SendMessage( GetDlgItem ( hdwnd , ctrl ) , UDM_GETBUDDY, 0, 0 ) , FALSE ) ; \
                     Button_Enable ( GetDlgItem ( hdwnd , ctrl ) , FALSE ) ; } 
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

int trublocknum ;
int curaligner ; 

int my_spawn( char * argus, ...)
{
    char jstr[(MAX_PATH*2)+256], qqqstr[(MAX_PATH*2)+256];
    STARTUPINFO startinfo;
    PROCESS_INFORMATION pinfo;
    char **cpp = &argus, *cp;
    DWORD how;
    how = 0;
    qqqstr[0] = 0 ; 
    while( cp = * cpp ++ ) { 
            strcat(qqqstr, cp );
            strcat(qqqstr," ");}
        startinfo.cb = sizeof(STARTUPINFO);
        startinfo.lpReserved = NULL;
        startinfo.lpDesktop = NULL;
        if ( curaligner == MAFFT )
               wsprintf(jstr,"Aligning \"%s\" with MAFFT (block %i of %i)" , blocklist[trublocknum].blockname , current_block , blocks_to_align );
        else if ( curaligner == MUSCLE )
                  wsprintf(jstr,"Aligning \"%s\" with MUSCLE (block %i of %i)" , blocklist[trublocknum].blockname , current_block , blocks_to_align );
        else wsprintf(jstr,"Aligning \"%s\"" , blocklist[trublocknum].blockname );
        startinfo.lpTitle = jstr;
        startinfo.dwFlags = STARTF_USEPOSITION | STARTF_USESIZE ;
        startinfo.dwX = 50 ; 
        startinfo.dwXSize = 700 ; 
        startinfo.dwY = 100 ; 
        startinfo.dwYSize = 250 ; 
        startinfo.cbReserved2 = NULL;
        startinfo.wShowWindow = SW_SHOW;
       if(CreateProcess(NULL, qqqstr, NULL, NULL, FALSE ,
            HIGH_PRIORITY_CLASS | 0, NULL, NULL, &startinfo, &pinfo))
        WaitForSingleObject(pinfo.hProcess,INFINITE);
            else {
                MessageBox(hwnd,"Could not spawn!","Spawn error",MB_OK | MB_ICONERROR );
                return(255);
            }
        GetExitCodeProcess(pinfo.hProcess, &how);
        return( how );
}

void shortcpy ( char * ap , char * bp )
{
    int a , chgd = 0 ;
    char * endpt = ( ap + NAMESIZE ) - 1 ;
    while ( * bp && ap < endpt ) 
            * ap ++ = * bp ++ ;
    * ap = '\0' ; 
    return ; 
}

void fillabox ( int which , int withwhat )
{
    int x , y , a , b ;
    int insel ;
    SetDlgItemText ( uswnd , _BLOCKNAME+which , tmpblocklist[withwhat].blockname ) ; 
    junkstr[0] ='\0' ;
    // b = 0 ; 
    for ( a = 0 ; tmpblocklist[withwhat].files[a][0] != '\0' && a < MAXFILES_PER_BLOCK ; ++ a ) {
       // ++ b ; 
       if ( a ) strcat ( junkstr , linend ) ; 
       strcat ( junkstr , tmpblocklist[withwhat].files[a] ) ; }
    SetDlgItemText ( uswnd , _UPLIST+which , junkstr ) ; 
    // if ( !b ) SetDlgItemText ( uswnd , _UPLIST+which , junkstr ) ; 
    junkstr[0] ='\0' ;
    // b = 0 ; 
    for ( a = 0 ; tmpblocklist[withwhat].prod[a][0] != '\0' && a < MAXPRODS_PER_BLOCK ; ++ a ) {
       // ++ b ; 
       if ( a ) strcat ( junkstr , linend ) ; 
       strcat ( junkstr , tmpblocklist[withwhat].prod[a] ) ; }
    SetDlgItemText ( uswnd , _LOLIST+which , junkstr ) ; 
    // if ( !b ) SetDlgItemText ( uswnd , _LOLIST+which , junkstr ) ; 
    return ;
}

void fillallboxes ( void )
{
    int a , b ; 
    for ( a = 0 , b = showfrom ; a < numboxes ; ++ a , ++ b ) fillabox ( a , b ) ; 
    InvalidateRect ( uswnd , NULL , TRUE ) ;
}    

int movmov , movto ;

void chklims ( void )
{
    if ( numblocks == MAXNUMBLOCKS ) CTRL_DISABLE ( _addblock ) ;
    else CTRL_ENABLE ( _addblock ) ;
    if ( numboxes == numblocks || numboxes == MAXBOX ) CTRL_DISABLE ( _more ) ;
    else CTRL_ENABLE ( _more ) ;
    if ( numboxes == 1 ) { CTRL_DISABLE ( _less ) ; CTRL_DISABLE ( _WIPE ) ; }
    else { CTRL_ENABLE ( _less ) ; CTRL_ENABLE ( _WIPE ) ; }
    if ( showfrom > 0 ) { CTRL_ENABLE ( _left ) ; CTRL_ENABLE ( _left+2 ) ; CTRL_ENABLE ( _left+4 ) ; }
    else { CTRL_DISABLE ( _left ) ; CTRL_DISABLE ( _left+2 ) ; CTRL_DISABLE ( _left+4 ) ; }
    if ( showfrom + numboxes < numblocks ) { CTRL_ENABLE ( _right ) ; CTRL_ENABLE ( _right+2 ) ; CTRL_ENABLE ( _right+4 ) ; }
    else { CTRL_DISABLE ( _right ) ; CTRL_DISABLE ( _right+2 ) ; CTRL_DISABLE ( _right+4 ) ; }
    sprintf ( junkstr , "Edit project...   %i TNT blocks" , numblocks ) ; 
    SetWindowText ( uswnd , junkstr ) ;
}    

void MyWR ( HWND hw , RECT * rec )
{
    POINT pt ; 
    GetWindowRect ( hw , rec ) ;
    pt.x = rec->left ;
    pt.y = rec -> top ;
    ScreenToClient ( hw , &pt ) ;
    rec->left = pt.x ;
    rec->top = pt.y ; 
    pt.x = rec-> right;
    pt.y = rec -> bottom ;
    ScreenToClient ( hw , &pt ) ;
    rec->right = pt.x ;
    rec->bottom = pt.y ;
}     

int Hof ( int ctrl )
{
    RECT r ;
    MyWR ( GetDlgItem ( uswnd , ctrl ) , &r ) ;
    return r.bottom - r.top ;
}     

int Wof ( int ctrl )
{
    RECT r ; 
    MyWR ( GetDlgItem ( uswnd , ctrl ) , &r ) ;
    return r.right - r.left ;
}     


void getCsize ( Siztyp * pp , int ctrl )
{
    pp -> w = Wof ( ctrl ) ;
    pp -> h = Hof ( ctrl ) ;
    return ; 
} 


int initlzd = 0 ; 

void hidall ( int which )
{
    ShowWindow ( HCTRL ( ( _BLOCKNAME + which ) ) , SW_HIDE ) ; 
    ShowWindow ( HCTRL ( ( _UPLIST + which ) ) , SW_HIDE ) ; 
    ShowWindow ( HCTRL ( ( _LOLIST + which ) ) , SW_HIDE ) ; 
    ShowWindow ( HCTRL ( ( _FILTER + which ) ) , SW_HIDE ) ; 
    ShowWindow ( HCTRL ( ( _WIPE + which ) ) , SW_HIDE ) ;
    ShowWindow ( HCTRL ( ( _ADDFILE + which ) ) , SW_HIDE ) ;
} 
    
void movall ( int which , Siztyp ls , Siztyp ms , Siztyp txts , int upedge )
{
    int butts = ( iamatx + ( ls.w / 2 ) ) - ( ms.w / 2 ) ; 
    ShowWindow (HCTRL((_BLOCKNAME+which)) , SW_SHOW ) ; 
    ShowWindow (HCTRL((_ADDFILE+which)) , SW_SHOW ) ; 
    ShowWindow (HCTRL((_UPLIST+which)) , SW_SHOW ) ; 
    ShowWindow (HCTRL((_LOLIST+which)) , SW_SHOW ) ; 
    ShowWindow (HCTRL((_FILTER+which)) , SW_SHOW ) ; 
    MoveWindow(HCTRL((_BLOCKNAME+which)),    iamatx , upedge , ls.w , txts.h , TRUE ) ;
    MoveWindow(HCTRL((_ADDFILE+which)), butts - ( ms.w/4) , upedge + 1 + txts.h , ( ms.w * 3 ) / 2 , txts.h , TRUE ) ;
    MoveWindow(HCTRL((_UPLIST+which)), iamatx , upedge + 25 + txts.h , ls.w , ( ls.h / 2 ) - 5 , TRUE ) ;
    MoveWindow(HCTRL((_LOLIST+which)), iamatx , upedge + 25 + txts.h + ((ls.h/2)+5), ls.w , ( ls.h / 2 ) - 5 , TRUE ) ;
    MoveWindow(HCTRL((_FILTER+which)), butts ,  upedge + 26 + txts.h + ls.h, ms.w , ms.h , TRUE ) ;
    iamatx += ( ls.w + 15 ) ;
    return ;
} 
    
void resize_charedit ( int redo )
{
    static Siztyp lowbutt , prev , more , list , lcombo , rcombo , move , statecaption , vertis ;
    RECT tmp ;
    POINT tmpp ;
    POINT pt ; 
    int Hei , Wid , a , chg , listarty ;
    int from , to ;
    int chgh = 0 , chgw = 0 ;
    int i ; 
    /**  Prevent window from getting too small ***/ 
    getCsize ( &vertis , 115 ) ;
    getCsize ( &lowbutt , _ok ) ;    
    GetWindowRect( uswnd , &WDim ) ;
    if ( WDim.bottom - WDim.top < vertis.h * 5 ) chgh = vertis.h * 5 ;
    a = 20 + ( ( ( ( lowbutt.w *  3 ) / 2 ) + 15 ) * numboxes ) ;
    if ( a < ( lowbutt.w + 15 ) *  4 ) a = ( lowbutt.w + 15 ) *  4 ; 
    if ( WDim.right - WDim.left < a )  chgw = a ; 
    if ( chgw || chgh ) {
        if ( !chgh ) chgh = WDim.bottom - WDim.top ;
        if ( !chgw ) chgw = WDim.right - WDim.left ;
        MoveWindow ( uswnd , WDim.left , WDim.top , chgw , chgh , TRUE ) ; }
  
    getCsize ( &lowbutt , _ok ) ;
    getCsize ( &vertis , 115 ) ; 
    if ( !redo ) 
        getCsize ( &statecaption , _BLOCKNAME ) ;
    else {
        Hei = statecaption.h ; 
        getCsize ( &statecaption , _BLOCKNAME ) ;
        statecaption.h = Hei ; }
    if ( !initlzd ) {
       MoveWindow ( uswnd , WDim.left, WDim.top , (lowbutt.w+40)*(numboxes+2) , vertis.h*5 , TRUE ) ;
       initlzd = 1 ; }
    else 
     if ( !redo ) 
        MoveWindow ( uswnd , USDim.left, USDim.top , USDim.right - USDim.left , USDim.bottom - USDim.top , TRUE ) ; 
    MyWR ( uswnd , &WDim ) ;
    getCsize ( &move , _FILTER ) ;
    chg = 0 ; 
    Hei = WDim.bottom ;
    Wid = WDim.right ;

    GetWindowRect ( HCTRL ( _more ) , &tmp ) ;
    tmpp.y = tmp.bottom ;
    tmpp.x = tmp.left ; 
    ScreenToClient ( uswnd , &tmpp ) ;
    from = tmpp.y + 20 ; 
    to = WDim.bottom - ( 20 + lowbutt.h) ; 
    MoveWindow ( HCTRL ( _frame ) , 10 , from , Wid - 20 , to - from , TRUE ) ;

    list.w = ( Wid - ( 40 + ( numboxes * 15 ) ) ) / numboxes ;
    list.h = ( to - from ) - ( 50 + move.h + statecaption.h ) ;
/*
    MoveWindow ( HCTRL ( _left )  , 10 , Hei - ( lowbutt.h + 10 ) , lowbutt.w , lowbutt.h , TRUE ) ; 
    MoveWindow ( HCTRL ( _right ) , 20 + lowbutt.w , Hei - ( lowbutt.h + 10 ) , lowbutt.w , lowbutt.h , TRUE ) ; 
*/
    MoveWindow ( HCTRL ( _left+4 )  , 10 , Hei - ( lowbutt.h + 10 ) , lowbutt.w/4 , lowbutt.h , TRUE ) ; 
    MoveWindow ( HCTRL ( _left+2 )  , 10+(lowbutt.w/4)+2 , Hei - ( lowbutt.h + 10 ) , lowbutt.w/4 , lowbutt.h , TRUE ) ; 
    MoveWindow ( HCTRL ( _left   )  , 10+(lowbutt.w/4)+4+(lowbutt.w/4) , Hei - ( lowbutt.h + 10 ) , 3+(lowbutt.w/3) , lowbutt.h , TRUE ) ; 

    i = 13+(lowbutt.w/4)+6+(lowbutt.w/4)+(lowbutt.w/3) ;  
    MoveWindow ( HCTRL ( _right ) , i , Hei - ( lowbutt.h + 10 ) , 3+(lowbutt.w/3) , lowbutt.h , TRUE ) ; 
    MoveWindow ( HCTRL ( _right+2 ) , i+5+(lowbutt.w/3) , Hei - ( lowbutt.h + 10 ) , lowbutt.w/4 , lowbutt.h , TRUE ) ; 
    MoveWindow ( HCTRL ( _right+4 ) , i+7+(lowbutt.w/3)+(lowbutt.w/4), Hei - ( lowbutt.h + 10 ) , lowbutt.w/4 , lowbutt.h , TRUE ) ; 

    MoveWindow ( HCTRL ( _ok ) , Wid - ( lowbutt.w + 10 ) , Hei - ( lowbutt.h + 10 ) , lowbutt.w , lowbutt.h , TRUE ) ; 
    MoveWindow ( HCTRL ( _cancel )  , Wid - ( ( lowbutt.w + 10 ) * 2 ) , Hei - ( lowbutt.h + 10 ) , lowbutt.w , lowbutt.h , TRUE ) ; 

    /*  Side (vertical) legends */ 
    listarty = from + 35 + statecaption.h ; 
    MoveWindow ( HCTRL ( 115 ) , 15 , listarty , vertis.w , vertis.h , TRUE ) ; 
    listarty += ((list.h/2)+5) ; 
    MoveWindow ( HCTRL ( 116 ) , 15 , listarty , vertis.w , vertis.h , TRUE ) ; 

    iamatx = 40 ;
    for ( a = numboxes ; a < MAXBOX ; ++ a ) hidall ( a ) ; 
    for ( a = 0 ; a < numboxes ; ++ a )
        movall ( a , list, move , statecaption , ( from + 10 ) ) ;
    GetWindowRect ( uswnd , &USDim ) ;
    intlzd = 1 ; 
    InvalidateRect ( uswnd , NULL , 1 ) ;
    return ; 
}

int storefilenamesfrom ( int box , int dablock )
{
  int a , more ;
  int numlins = 0 , havenonblanks ;
  char * cp = junkstr , * cpp , * qp ;
  a = GetDlgItemText( uswnd , box , junkstr, 16000 ) ;
  junkstr[a] = '\0' ;
  while ( * cp ) {
      cpp = cp ;
      more = 0 ; 
      while ( * cpp && * cpp != 13 ) ++ cpp ;
      if ( * cpp == 13 ) { * cpp = '\0' ; more = 1 ; }
      qp = cp ;
      havenonblanks = 0 ; 
      while ( * qp != 13 && * qp != '\0' && !havenonblanks ) { if ( !isspace( * qp ) ) havenonblanks = 1 ; ++ qp ; }
      if ( havenonblanks ) {
          strcpy ( tmpblocklist[dablock].files[numlins] , cp ) ; 
          numlins ++ ;
          if ( numlins < MAXFILES_PER_BLOCK ) tmpblocklist[dablock].files[numlins][0] = '\0' ; }
      if ( more ) cpp += 2 ;
      cp = cpp ; }
  return numlins ; 
}

int countfilenamesfrom ( int box , int dablock )
{
  int a , more ;
  int numlins = 0 , havenonblanks ;
  char * cp = junkstr , * cpp , * qp ; 
  a = GetDlgItemText( uswnd , box , junkstr, 16000 ) ;
  junkstr[a] = '\0' ;
  while ( * cp ) {
      cpp = cp ;
      more = 0 ; 
      while ( * cpp && * cpp != 13 ) ++ cpp ;
      if ( * cpp == 13 ) { * cpp = '\0' ; more = 1 ; }
      qp = cp ;
      havenonblanks = 0 ; 
      while ( * qp != 13 && * qp != '\0' && !havenonblanks ) { if ( !isspace( * qp ) ) havenonblanks = 1 ; ++ qp ; }
      if ( havenonblanks ) numlins ++ ;
      if ( more ) cpp += 2 ;
      cp = cpp ; }
  return numlins ; 
}

int countprodnamesfrom ( int box , int dablock )
{
  int a , more ;
  int numlins = 0 , havenonblanks ;
  char * cp = junkstr , * cpp , * qp ; 
  a = GetDlgItemText( uswnd , box , junkstr, 16000 ) ;
  junkstr[a] = '\0' ;
  while ( * cp ) {
      cpp = cp ;
      more = 0 ; 
      while ( * cpp && * cpp != 13 ) ++ cpp ;
      if ( * cpp == 13 ) { * cpp = '\0' ; more = 1 ; }
      qp = cp ;
      havenonblanks = 0 ; 
      while ( * qp != 13 && * qp != '\0' && !havenonblanks ) { if ( !isspace( * qp ) ) havenonblanks = 1 ; ++ qp ; }
      if ( havenonblanks ) numlins ++ ;
      if ( more ) cpp += 2 ;
      cp = cpp ; }
  return numlins ; 
}

void showfilenamesat ( int box , int dablock )
{
  int a , more ;
  junkstr[0] = '\0' ; 
  for ( a = 0 ; a < MAXFILES_PER_BLOCK && tmpblocklist[dablock].files[a][0] != '\0' ; ++ a ) {
      if ( a ) strcat ( junkstr , linend ) ;
      strcat ( junkstr , tmpblocklist[dablock].files[a] ) ; }
  SetDlgItemText ( uswnd , box , junkstr ) ; 
  return ;  
}

OPENFILENAME opbuff;


void openafile ( int valis )
{
    OPENFILENAME ofn;
    static char dirname[MAX_PATH+3]={ "" } ; 
    char * fp , * otp ;
//    char gbfilter[] = "GB files (*.gb)\0*.gb\0ALL files\0*.*\0";
    char gbfilter[] = "GB files (*.gb)\0*.gb\0FASTA files (*.fas)\0*.fas\0Aligned FASTA (*.aln)\0*.aln\0TNT files (*.tnt)\0*.tnt\0";
    int myblockis = ( valis - _ADDFILE ) + showfrom ;
    int daboxis = _UPLIST + ( valis - _ADDFILE ) ;
    int numfiles , linlen , fileschosen = 0 , longestlin ;
    int catdir = 0 , setnumfilesto , numfileswas ;
    char dirtocat[MAX_PATH] ; 
    fn[0]='\0';
    memset(&opbuff, 0, sizeof(OPENFILENAME));
    opbuff.lStructSize = sizeof(OPENFILENAME);
    opbuff.hwndOwner = uswnd;
    opbuff.lpstrFilter = gbfilter;
    opbuff.nFilterIndex = 1;
    opbuff.lpstrFile = fn;
    opbuff.nMaxFile = sizeof(fn);
    opbuff.lpstrFileTitle = NULL ; // filename;

if ( !unnamed_project ) {
    strcpy ( dirname , project_name ) ; 
    fp = otp = dirname ;
    while ( * otp ) {
        if ( * otp == '\\' ) fp = otp ;
        ++ otp ; }
    * fp = '\0' ; }

    opbuff.lpstrInitialDir = dirname ; 
    opbuff.nMaxFileTitle = 0 ; // sizeof(filename)-1;
    opbuff.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_ALLOWMULTISELECT | OFN_EXPLORER ;
    numfileswas = numfiles = storefilenamesfrom ( daboxis , myblockis ) ;
    if ( numfiles == MAXFILES_PER_BLOCK ) {
        sprintf ( junkstr , "Cannot have more than %i files per block" , MAXFILES_PER_BLOCK) ; 
        MessageBox ( uswnd , junkstr , "Sorry" , MB_ICONERROR ) ;
        return ; }
    sprintf ( junkstr , "GenBank File(s) for Block %i (press ctrl for multiple files)" , myblockis + 1 ) ; 
    opbuff.lpstrTitle = junkstr ;
    setnumfilesto = -1 ; 
    if(GetOpenFileName(&opbuff)) {
            fp = fn ; 
            while ( * fp ) {
               if ( numfiles >= MAXFILES_PER_BLOCK ) { setnumfilesto = numfileswas ; ++ fileschosen ; }
               else 
                 if ( catdir ) {
                     if ( catdir == 1 ) { catdir = 2 ; -- numfiles ; fileschosen = longestlin = 0 ; }
                     ++ fileschosen ;
                     linlen = strlen( fp ) + strlen ( dirtocat ) - 14 ;
                     if ( longestlin < linlen ) longestlin = linlen ; 
                     sprintf ( tmpblocklist[myblockis].files[numfiles++], "%s\\%s" , dirtocat , fp ) ; }
                 else {
                     fileschosen = 1 ; 
                     longestlin = strlen( fp ) - 15 ;
                     if ( longestlin < 0 ) longestlin = 0 ;
                     strcpy ( tmpblocklist[myblockis].files[numfiles++], fp ) ; }
                if ( !catdir ) {
                    catdir = 1 ; 
                    strcpy ( dirtocat , fp ) ; }
                fp = fp + strlen ( fp ) + 1 ; }
            if ( setnumfilesto >= 0 ) {
                 if ( fileschosen > MAXFILES_PER_BLOCK )
                    sprintf ( junkstr , "Cannot choose %i files\nMaximum is %i" , fileschosen , MAXFILES_PER_BLOCK ) ; 
                 else 
                    sprintf ( junkstr , "Cannot choose %i files\nMaximum is %i and you already have %i" , fileschosen , MAXFILES_PER_BLOCK , numfileswas ) ; 
                 MessageBox ( uswnd , junkstr , "ERROR" , MB_ICONERROR ) ;
                 numfiles = setnumfilesto ; }
            if ( numfiles < MAXFILES_PER_BLOCK ) tmpblocklist[myblockis].files[numfiles][0] = '\0' ;
            showfilenamesat ( daboxis , myblockis ) ;
            if ( numfiles == MAXFILES_PER_BLOCK ) CTRL_DISABLE ( ( daboxis - _UPLIST ) + _ADDFILE ) ;
            else CTRL_ENABLE ( ( daboxis - _UPLIST ) + _ADDFILE ) ;
            SendMessage ( GetDlgItem ( uswnd , daboxis ) , EM_LINESCROLL , longestlin , 0 ) ;
            InvalidateRect ( GetDlgItem ( uswnd , daboxis ) , NULL , 1 ) ; }
    return ; 
}

BOOL CALLBACK ProjectEditFunc (HWND hdwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int a , b , insel , ssel , val , to ;
    HWND tmpwnd ;
    HWND hWho ;
    HBITMAP hbmap ; 
    switch(message) {
        case WM_INITDIALOG :
                /*  The vertical legends... */ 
                hbmap = LoadBitmap ( hInst , MAKEINTRESOURCE ( VERT_ONE ) ) ;
                hWho = GetDlgItem ( hdwnd , 115 ) ; 
                SendMessage ( hWho , BM_SETIMAGE , IMAGE_BITMAP , hbmap ) ; 
                hbmap = LoadBitmap ( hInst , MAKEINTRESOURCE ( VERT_TWO ) ) ;
                hWho = GetDlgItem ( hdwnd , 116 ) ; 
                SendMessage ( hWho , BM_SETIMAGE , IMAGE_BITMAP , hbmap ) ; 
                have_changes = 0 ; 
                uswnd = hdwnd ;
                numboxes = ( numblocks < MAXBOX ) ? numblocks : MAXBOX ;
                showfrom = 0 ;
                fillallboxes () ;
                showto = 1 ;
                resize_charedit ( passed_through_init ) ;
                passed_through_init = 1 ;
                resize_charedit ( passed_through_init ) ;
                chklims () ; 
                if ( first_timer ) {
                      SetDlgItemText ( uswnd , _UPLIST , uplistintro ) ; 
                      SetDlgItemText ( uswnd , _LOLIST , lolistintro ) ; }
                FOCUS_ON ( IDOK ) ;
                break;
        case WM_SIZE :
             resize_charedit ( passed_through_init ) ;
             break ; 
        case WM_DESTROY :
                break;
        case WM_COMMAND :

                val = LOWORD ( wParam ) ;
                if ( val >= 700 && val < 800 ) {   //  "Find file"
                    noexplain () ; 
                    openafile ( val ) ; 
                    inva ; 
                    break ; }

                switch( LOWORD(wParam) ){

                    case 200:case 201:case 202:case 203:case 204:case 205:case 206:case 207:case 208:case 209:
                        noexplain () ; 
                        if ( HIWORD ( wParam ) == EN_CHANGE ) {
                             b = ( val - _UPLIST ) - showfrom ;
                             a = countfilenamesfrom ( val , b ) ;
                             if ( a >= MAXFILES_PER_BLOCK ) {
                                  CTRL_DISABLE ( b + _ADDFILE ) ;
                                  if ( a > MAXFILES_PER_BLOCK ) {
                                      sprintf ( junkstr , "Cannot specify more than %i files per block" , MAXFILES_PER_BLOCK ) ; 
                                      MessageBox ( uswnd , junkstr , "Sorry" , MB_OK ) ;
                                      boxretrieve ( val - _UPLIST , showfrom + val - _UPLIST , tmpblocklist ) ;
                                      fillabox ( val - _UPLIST , showfrom + val - _UPLIST ) ; }}
                             else CTRL_ENABLE ( b + _ADDFILE ) ; }
                        break ;  

                    case 600:case 601:case 602:case 603:case 604:case 605:case 606:case 607:case 608:case 609:
                        noexplain () ; 
                        if ( HIWORD ( wParam ) == EN_CHANGE ) {
                             b = ( val - _LOLIST ) - showfrom ;
                             a = countprodnamesfrom ( val , b ) ;
                             if ( a > MAXPRODS_PER_BLOCK ) {
                                  sprintf ( junkstr , "Cannot specify more than %i genes/prods per block" , MAXPRODS_PER_BLOCK ) ; 
                                  MessageBox ( uswnd , junkstr , "Sorry" , MB_OK ) ;
                                  boxretrieve ( val - _LOLIST , showfrom + val - _LOLIST , tmpblocklist ) ;
                                  fillabox ( val - _LOLIST , showfrom + val - _LOLIST ) ; }}
                        break ;  

                    case _addblock:
                         noexplain () ; 
                         sprintf ( tmpblocklist[numblocks].blockname , "BLOCK_%i" , numblocks+1) ; 
                         tmpblocklist[numblocks].files[0][0] = '\0' ; 
                         tmpblocklist[numblocks].prod[0][0] = '\0' ;
                         tmpblocklist[numblocks].maxslen = tmpblocklist[numblocks].minslen = 0 ;
                         tmpblocklist[numblocks].genome = NUCLEAR ;
                         tmpblocklist[numblocks].stringsim = 100 ;
                         tmpblocklist[numblocks].isdna = 1 ;
                         tmpblocklist[numblocks].oneseqpersp = 1 ;
                         tmpblocklist[numblocks].alsonote = 0 ;
                         tmpblocklist[numblocks].category = GENE | CDS ;
                         tmpblocklist[numblocks].complete = 0 ; 
                         tmpblocklist[numblocks].twogen [ 0 ] = 0 ;       
                         tmpblocklist[numblocks].twogen [ 1 ] = 0 ; 
                         tmpblocklist[numblocks].twogen [ 2 ] = 0 ;
                         tmpblocklist[numblocks].voucher = 0 ; 
                         retrieve_all_boxes ( tmpblocklist ) ; 
                         if ( numboxes < MAXBOX ) ++ numboxes ; 
                         ++ numblocks ;
                         showfrom = numblocks - numboxes ;
                         fillallboxes () ; 
                         resize_charedit ( 1 ) ;
                         chklims () ; 
                         break ; 

                    case 400: case 401: case 402: case 403: case 404: case 405: case 406: case 407: case 408: case 409:
                          noexplain () ; 
                          val -= 400 ;
                          numoption = val ;
                          boxretrieve ( numoption , showfrom + numoption , tmpblocklist ) ; 
                          a = DialogBox ( hInst , "OptionsDB" , hdwnd , (DLGPROC) OptionsFunc ) ;

fillallboxes () ; 

                          
                          break ; 

                    case _left: case ( _left+2 ) : case ( _left+4 ) : 
                       noexplain () ; 
                       retrieve_all_boxes ( tmpblocklist ) ;
                       if ( val == _left ) -- showfrom ;
                       else if ( val == _left+2 ) { showfrom -= numboxes ; if ( showfrom < 0 ) showfrom = 0 ; }
                             else showfrom = 0 ; 
                       fillallboxes () ; 
                       chklims () ;
                       break ; 

                    case _right: case ( _right+2) : case ( _right+4) : 
                       noexplain () ; 
                       retrieve_all_boxes ( tmpblocklist ) ; 
                       if ( val == _right ) ++ showfrom ;
                       else if ( val == _right+2 ) { showfrom += numboxes ; if ( showfrom > numblocks - numboxes ) showfrom = numblocks - numboxes ; }
                             else showfrom = numblocks - numboxes ; 
                       fillallboxes () ; 
                       chklims () ;
                       break ; 

                    case _more:
                       noexplain () ; 
                       if ( numboxes < numblocks && numboxes < MAXBOX ) {
                           ++ numboxes ;
                           resize_charedit ( 1 ) ;
                           chklims () ; }
                       break ;

                    case _less:
                        noexplain () ; 
                        if ( numboxes > 1 ) {
                            -- numboxes ;
                            boxretrieve ( numboxes , showfrom + numboxes , tmpblocklist ) ; 
                            resize_charedit ( 1 ) ;
                            chklims () ; }
                        break ; 

                    case IDCANCEL :
                          EndDialog ( hdwnd, 0 ) ; 
                          return 0;
                          
                    case IDOK :
                          noexplain () ;
                          retrieve_all_boxes ( tmpblocklist ) ;
                          if ( !check_source_types ( tmpblocklist ) ) break ; 
                          EndDialog ( hdwnd, 1 ) ;
                          return 1;
                          
                    default : break ; }}
    return 0 ;
}

int block_is_changed ( Blocktyp * tgt , Blocktyp * src )
{
    int a ;
    if ( tgt -> stringsim != src -> stringsim ) return 1 ;
    if ( tgt -> isdna != src -> isdna ) return 1 ;
    if ( tgt -> genome != src -> genome ) return 1 ;
    if ( tgt -> alsonote != src -> alsonote ) return 1 ;
    if ( tgt -> oneseqpersp != src -> oneseqpersp ) return 1 ;
    if ( tgt -> minslen != src -> minslen ) return 1 ; 
    if ( tgt -> maxslen != src -> maxslen ) return 1 ; 
    if ( strcmp ( tgt -> blockname , src -> blockname ) ) return 1 ;
    if ( tgt -> category != src -> category ) return 1 ; 
    for ( a = 0 ; a < MAXFILES_PER_BLOCK ; ++ a )
         if ( strcmp ( tgt -> files[a] , src -> files[a] ) ) return 1 ; 
    for ( a = 0 ; a < MAXPRODS_PER_BLOCK ; ++ a )
         if ( strcmp ( tgt -> prod[a] , src -> prod[a] ) ) return 1 ;
    if ( tgt -> complete != src -> complete )  return 1 ;
    if ( tgt -> twogen [ 0 ] != src -> twogen [ 0 ] ) return 1 ;         
    if ( tgt -> twogen [ 1 ] != src -> twogen [ 1 ] ) return 1 ;
    if ( tgt -> twogen [ 2 ] != src -> twogen [ 2 ] )  return 1 ;
    if ( tgt -> voucher != src -> voucher)  return 1 ;
    if ( tgt -> missgaps != src -> missgaps ) return 2 ;
    if ( tgt -> aligner != src -> aligner ) return 1 ;
    if ( tgt -> useralignparms != src -> useralignparms ) return 1 ;
    if ( strcmp ( tgt -> alignparms , src -> alignparms ) ) return 1 ;  
    return 0 ; 
}

void copy_a_block ( Blocktyp * tgt , Blocktyp * src )
{
    int a ;
    tgt -> stringsim = src -> stringsim ;
    tgt -> isdna = src -> isdna ;
    tgt -> genome = src -> genome ;
    tgt -> alsonote = src -> alsonote ;
    tgt -> oneseqpersp = src -> oneseqpersp ;
    tgt -> minslen = src -> minslen ; 
    tgt -> maxslen = src -> maxslen ; 
    tgt -> complete = src -> complete ;
    tgt -> voucher = src -> voucher ;
    tgt -> category = src -> category ;
    tgt -> sourcetype = src -> sourcetype ;
    tgt -> missgaps = src -> missgaps ;
    tgt -> aligner = src -> aligner ; 
    tgt -> useralignparms = src -> useralignparms ; 
    strcpy ( tgt -> alignparms , src -> alignparms ) ;  
    if ( optcpy_from < 0  ) {
      strcpy ( tgt -> blockheader , src -> blockheader ) ;   
      tgt -> twogen[0] = src -> twogen[0] ; 
      tgt -> twogen[1] = src -> twogen[1] ; 
      tgt -> twogen[2] = src -> twogen[2] ; 
      strcpy ( tgt -> blockname , src -> blockname ) ;
      for ( a = 0 ; a < MAXFILES_PER_BLOCK ; ++ a )
            strcpy ( tgt -> files[a] , src -> files[a] ) ; 
      for ( a = 0 ; a < MAXPRODS_PER_BLOCK ; ++ a )
           strcpy ( tgt -> prod[a] , src -> prod[a] ) ;
      for ( a = 0 ; a < MAXPRODS_PER_BLOCK ; ++ a )
           strcpy ( tgt -> prod2[a] , src -> prod2[a] ) ; }
} 
    
void delete_a_block ( val )
{
    int a , b ;
    int myblock = val + showfrom ;
    for ( a = myblock ; a < numblocks - 1 ; ++ a ) {
        copy_a_block ( tmpblocklist + a , tmpblocklist + a + 1 ) ;
        b = tmpblocklist[a].blockname[6] ;
        tmpblocklist[a].blockname[6] = '\0' ;
        if ( strcmp ( tmpblocklist[a].blockname , "BLOCK_" ) ) {
            tmpblocklist[a].blockname[6] = b ;
            continue ; }
        tmpblocklist[a].blockname[6] = b ;
        if ( isdigit ( b ) ) sprintf ( tmpblocklist[a].blockname , "BLOCK_%i" , a + 1 ) ; }
    -- numboxes ;
    -- numblocks ; 
    for ( a = val , b = showfrom + val ; a < numboxes ; ++ a , ++ b )
         fillabox ( a , b ) ; 
}

void cpy_block_opt ( int src_blk )
{
  int a ;
  Blocktyp  * tgt_blkpt, * src_blkpt ;
  src_blkpt = tmpblocklist + src_blk ;
  optcpy_from = src_blk ; 
  for ( a = 0 ; a < numblocks ; a ++  )
       if ( listofblocks[a] > 0 && a != src_blk ) {
        tgt_blkpt = tmpblocklist + a ;  
        copy_a_block ( tgt_blkpt, src_blkpt ) ; }
  optcpy_from = -1 ; 
}

void copy_no_blanks ( char * to , char * frm )
{
    char * tp = to -1 , * fp = frm - 1 ; 
    while ( 1 )  {
        ++ tp ;
        ++ fp ; 
        if ( * fp && isspace ( * fp ) ) * tp = '_' ; 
        else * tp = * fp ;
        if ( !* fp ) break ; }
}        
   
void boxretrieve ( int box , int dablock , Blocktyp * dabuf )
{
  int a , more ;
  int numlins , havenonblanks ;
  char * cp = junkstr , * cpp , * qp ;
  /* Block name */
  a = GetDlgItemText( uswnd , box + _BLOCKNAME , junkstr, 16000 ) ;
  if ( a >= MAXBLOCKNAME - 1 ) a = MAXBLOCKNAME - 1 ; 
  junkstr[a] = '\0' ;
  if ( ! a ) sprintf ( dabuf [dablock].blockname , "BLOCK_%i" , dablock ) ;
  else copy_no_blanks ( dabuf [dablock].blockname , junkstr ) ; 
  a = GetDlgItemText( uswnd , box + _UPLIST , junkstr, 16000 ) ;
  junkstr[a] = '\0' ;
  numlins = 0 ; 
  while ( * cp ) {
      cpp = cp ;
      more = 0 ; 
      while ( * cpp && * cpp != 13 ) ++ cpp ;
      if ( * cpp == 13 ) { * cpp = '\0' ; more = 1 ; }
      qp = cp ;
      havenonblanks = 0 ; 
      while ( * qp != 13 && * qp != '\0' && !havenonblanks ) { if ( !isspace( * qp ) ) havenonblanks = 1 ; ++ qp ; }
      if ( havenonblanks ) {
          strcpy ( dabuf [dablock].files[numlins] , cp ) ; 
          numlins ++ ;
          if ( numlins < MAXFILES_PER_BLOCK ) dabuf [dablock].files[numlins][0] = '\0' ; }
      if ( more ) cpp += 2 ;
      cp = cpp ; }
  if ( !numlins ) dabuf[dablock].files[0][0] = '\0' ; 
  a = GetDlgItemText( uswnd , box + _LOLIST , junkstr, 16000 ) ;
  junkstr[a] = '\0' ;
  numlins = 0 ;
  cp = junkstr ; 
  while ( * cp ) {
      cpp = cp ;
      more = 0 ; 
      while ( * cpp && * cpp != 13 ) ++ cpp ;
      if ( * cpp == 13 ) { * cpp = '\0' ; more = 1 ; }
      qp = cp ;
      havenonblanks = 0 ; 
      while ( * qp != 13 && * qp != '\0' && !havenonblanks ) { if ( !isspace( * qp ) ) havenonblanks = 1 ; ++ qp ; }
      if ( havenonblanks ) {
          strcpy ( dabuf [dablock].prod[numlins] , cp ) ; 
          numlins ++ ;
          if ( numlins < MAXPRODS_PER_BLOCK) dabuf [dablock].prod[numlins][0] = '\0' ; }
      if ( more ) cpp += 2 ;
      cp = cpp ; }
  if ( !numlins ) dabuf[dablock].prod[0][0] = '\0' ; 
  return ; 
}

void retrieve_all_boxes ( Blocktyp * dabuf )
{
    int a , b ;
    if ( first_timer ) return ; 
    for ( a = 0 , b = showfrom ; a < numboxes ; ++ a , ++ b )
        boxretrieve ( a , b , dabuf ) ; 
    return ; 
}

void SpinAccel ( HWND hctrl , int frst , int secd , int thrd )
{
    UDACCEL acel[3] ; 
    acel[0].nSec = 0 ; 
    acel[0].nInc = frst ; 
    acel[1].nSec = 2 ; 
    acel[1].nInc = secd ;
    acel[2].nSec = 4 ; 
    acel[2].nInc = thrd ;
    SendMessage( hctrl , UDM_SETACCEL, 3 , acel ) ; 
}


extern int doing_copy_options ;
extern int src_block_for_copy ;

BOOL CALLBACK OptionsFunc(HWND hdwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int i , j , x ; 
    static int a , mytype , mygenom ; 
    static int myminslen , mymaxslen , myalsonote , myoneseqpersp ;
    static int myloc[6] ;
    static char myvouch ;
    static char mycompl ;
    static int tocopy_opt ;
    static int myintgen = 0 ;
    static int mygapstat ;
    static int myblockalign ; 
    HWND hWho ;
    UDACCEL acel[4] ;
    switch(message) {
        case WM_INITDIALOG :
                    if ( !prepare_selmem() ) DestroyWindow ( hdwnd ) ;
                    sprintf ( junkstr , "Options for block %i (%s)" , numoption + showfrom + 1 , tmpblocklist[numoption+showfrom].blockname ) ;
                    SetWindowText ( hdwnd , junkstr ) ; 
                    a = numoption + showfrom ; 
                    if ( tmpblocklist[a].genome == NUCLEAR ) BUTT_CHECK ( 201 ) ; 
                    if ( tmpblocklist[a].genome == MITOCH ) BUTT_CHECK ( 202 ) ; 
                    if ( tmpblocklist[a].genome == CHLORO ) BUTT_CHECK ( 203 ) ;
                    if ( tmpblocklist[a].genome == ANYGENOME ) BUTT_CHECK ( 237 ) ;
                    blockalignfor = a ;
                    myblockalign = 0 ;
                    if ( tmpblocklist[a].aligner || tmpblocklist[a].useralignparms ) myblockalign = 1 ;
                    if ( myblockalign ) BUTT_CHECK ( 312 ) ; 
                    mygenom = tmpblocklist[a].genome ;
                    mytype = 0 ;
                    if ( !tmpblocklist[a].isdna ) mytype = 1 ;
                    x = tmpblocklist[a].category ; 
                    for ( i = 0 , j = 1 ; i < 6 ; ++ i , j <<= 1 ) {
                        if ( ( x & j ) ) {
                            BUTT_CHECK ( ( 231 + i ) ) ;
                            myloc[ i ] = 1 ; }
                        else myloc[ i ] = 0 ; }
                    myvouch = tmpblocklist[a].voucher ;
                    if ( myvouch ) BUTT_CHECK ( 240 ) ;
                    mycompl = tmpblocklist[a].complete ;
                    if ( mycompl ) BUTT_CHECK ( 241 ) ; 
                    myminslen = tmpblocklist[a].minslen ; 
                    mymaxslen = tmpblocklist[a].maxslen ;
                    if ( myminslen ) {
                        BUTT_CHECK ( 206 ) ;
                        SET_UPDOWN ( 210 , 1 , 32767 , myminslen ) ;
                        SpinAccel ( GetDlgItem ( hdwnd , 210 ) , 10, 50 , 100 ) ; }
                    else SPIN_DISABLE ( 210 ) ; 
                    if ( mymaxslen ) {
                        BUTT_CHECK ( 207 ) ;
                        SET_UPDOWN ( 310 , 1 , 32767 , mymaxslen ) ;
                        SpinAccel ( GetDlgItem ( hdwnd , 310 ) , 10, 50 , 100 ) ; }
                    else SPIN_DISABLE ( 310 ) ; 
                    SET_UPDOWN ( 227 , 1 , 100 , tmpblocklist[a].stringsim ) ; 
                    SpinAccel ( GetDlgItem ( hdwnd , 227 ) , 5, 10 , 10 ) ;
                    if ( numblocks < 2 ) Button_Enable ( GetDlgItem ( hdwnd , 199 ) , FALSE ) ; 
                    if ( tmpblocklist[a].isdna )  BUTT_CHECK ( 218 ) ;
                    else BUTT_CHECK ( 219 ) ;
                    myalsonote = tmpblocklist[a].alsonote ;
                    if ( myalsonote ) BUTT_CHECK ( 221 ) ;
                    myoneseqpersp = tmpblocklist[a].oneseqpersp ;
                    if ( myoneseqpersp ) BUTT_CHECK ( 222 ) ; 
                    for ( i = proj_numblocks ; i -- ; ) listofblocks[ i ] = 1 ;
                    myintgen = tmpblocklist[a].twogen[0] ;
                    for ( i = 0 ; tmpblocklist[a].prod[i][0] != '\0' && i < MAXPRODS_PER_BLOCK ; ++ i ) 
                         if ( !strcmp ( tmpblocklist[a].prod[i] , "<>" ) ) myintgen = 1 ; 
                    if ( myintgen ) {
                        mytype = 0 ; 
                        Button_Enable ( GetDlgItem ( hdwnd , 219 ) , FALSE ) ;   
                        BUTT_CHECK( 243 ) ; }
                    if ( mytype ) {
                        Button_Enable ( GetDlgItem ( hdwnd , 243 ) , FALSE ) ;
                        myintgen = 0 ; }  
                    if ( numblocks < 2 ) Button_Enable ( GetDlgItem ( hdwnd , 198 ) , FALSE ) ;
                    if ( ( tmpblocklist[a].missgaps & ALLMISSING ) ) {
                        mygapstat = 1 ; 
                        BUTT_CHECK ( 251 ) ; }
                    else {
                       mygapstat = 0 ; 
                       if ( ( tmpblocklist[a].missgaps & LEADMISSING ) ) { BUTT_CHECK ( 252 ) ; mygapstat |= 2 ; }
                       if ( ( tmpblocklist[a].missgaps & TRAILMISSING ) ) { BUTT_CHECK ( 253 ) ; mygapstat |= 4 ; }}
                    FOCUS_ON (IDOK ) ;
                    break ; 
        case WM_COMMAND :

                i = LOWORD(wParam) ;
                if ( ( i >= 200 && i <= 237 ) || ( i >= 240 && i <= 242 ) ) Button_Enable ( GetDlgItem ( hdwnd , 198 ) , TRUE )  ; 
                switch(LOWORD(wParam)){
                        case 312:
                           myblockalign = 1 - myblockalign ;
                           if ( myblockalign ) {
                                i = DialogBox ( hInst , "GetBlockAlignmentDB" , hdwnd , (DLGPROC) GetBlockAlignmentFunc ) ;
                                if ( !i ) {
                                    myblockalign = 0 ;
                                    BUTT_UNCHEK ( 312 ) ; }}
                           break ; 
                        case 251:
                           if ( ( mygapstat & 1 ) ) mygapstat ^= 1 ;
                           else mygapstat |= 1 ;
                           if ( ( mygapstat & 1 ) ) {
                                 mygapstat = 1 ;
                                 BUTT_UNCHEK ( 252 ) ;
                                 BUTT_UNCHEK ( 253 ) ; }
                           else mygapstat = 0 ; 
                           break ;
                        case 252:
                          mygapstat = ( mygapstat | 1 ) ^ 1 ;
                          if ( ( mygapstat & 2 ) ) mygapstat ^= 2 ;
                          else mygapstat |= 2 ;
                          if ( ( mygapstat & 2 ) ) BUTT_UNCHEK ( 251 ) ; 
                          break ; 
                        case 253:
                          mygapstat = ( mygapstat | 1 ) ^ 1 ;
                          if ( ( mygapstat & 4 ) ) mygapstat ^= 4 ;
                          else mygapstat |= 4 ;
                          if ( ( mygapstat & 4 ) ) BUTT_UNCHEK ( 251 ) ; 
                          break ; 
                        case 243:
                          myintgen = 1 - myintgen ;
                          if ( myintgen ) {
                              //boxretrieve ( numoption , numoption+showfrom , tmpblocklist ) ; 
                              i = DialogBox ( hInst , "GetDoubleGeneDB" , hdwnd , (DLGPROC) GetDoubleGeneFunc ) ;
                              if ( !i ) {
                                  myintgen = 0 ;
                                  BUTT_UNCHEK ( 243 ) ;
                                  Button_Enable ( GetDlgItem ( hdwnd , 219 ) , TRUE ) ; }
                              else {
                                  mytype = 0 ;
                                  Button_Enable ( GetDlgItem ( hdwnd , 219 ) , FALSE ) ; }}
                          else Button_Enable ( GetDlgItem ( hdwnd , 219 ) , TRUE ) ; 
                          break ;                     
                        case 240:
                           myvouch = 1 - myvouch ;
                           break ;
                        case 241:
                           mycompl = 1 - mycompl ;
                           break ; 
                        case 231:case 232:case 233:case 234:case 235:case 236:
                            i = ( LOWORD ( wParam ) - 1 ) - 230 ; 
                            myloc[ i ] = 1 - myloc[ i ] ;
                            break ;
                        case 237:
                           mygenom = ANYGENOME ;
                           break ;  
                        case 221:
                            myalsonote = 1 - myalsonote ;
                            break ;
                        case 222:
                            myoneseqpersp = 1 - myoneseqpersp ;
                            break ; 
                        case 206:
                            if ( myminslen ) { myminslen = 0 ; BUTT_UNCHEK ( 206 ) ; SPIN_DISABLE ( 210 ) ; }
                            else { myminslen = 50 ; SET_UPDOWN ( 210 , 1 , 32767 , 50 ) ; }
                            break ; 
                        case 207:
                            if ( mymaxslen ) { mymaxslen = 0 ; BUTT_UNCHEK ( 207 ) ; SPIN_DISABLE ( 310 ) ; }
                            else { mymaxslen = 30000 ; SET_UPDOWN ( 310 , 1 , 32767 , 30000 ) ; }
                            break ; 
                        case 199: 
                              sprintf ( junkstr , "Are you sure you want to delete block %i" , showfrom + numoption + 1 ) ; 
                              i = MessageBox ( hdwnd , junkstr , "Please confirm" , MB_YESNO ) ;
                              if ( i != IDYES ) break ;
                              retrieve_all_boxes ( tmpblocklist ) ; 
                              delete_a_block ( numoption ) ;
                              resize_charedit ( 1 ) ;
                              chklims () ;
                              EndDialog ( hdwnd , 1 ) ; 
                              break ; 
                        case 201: mygenom = NUCLEAR; break ; 
                        case 202: mygenom = MITOCH ; break ; 
                        case 203: mygenom = CHLORO ; break ;
                        case 218: mytype = 0 ; Button_Enable ( GetDlgItem ( hdwnd , 243 ) , TRUE ) ; break ;
                        case 219: mytype = 1 ; Button_Enable ( GetDlgItem ( hdwnd , 243 ) , FALSE ) ; break ; 

                   case 198:  // copy options to selected blocks
                       these_are_files = 0 ;
                       doing_copy_options = 1 ;
                       src_block_for_copy = a ;
                       retrieve_all_boxes ( tmpblocklist ) ; 
                       for ( i = 0 ; i < numblocks ; ++ i ) allnamespt[i] = tmpblocklist[i].blockname ;
                       sprintf ( junkstr , "Blocks to set as " ) ; 
                       GetDlgItemText( uswnd , ( a - showfrom ) + _BLOCKNAME , junkstr+17 , 16000 ) ;
                       prepare_selection ( junkstr , "Unchanged Blocks", allnamespt , numblocks ) ;
                       for ( i = numblocks ; i-- ; ) selectlist [ i ] = 1 ; 
                       i = SELECT_DIALOG ;
                       if ( i ) {
                           for ( i = numblocks ; i-- ; ) listofblocks[ i ] = selectlist [ i ] ;
                           Button_Enable ( GetDlgItem ( hdwnd , 198 ) , FALSE )  ;
                           tocopy_opt = 1 ; }
                       doing_copy_options = 0 ;
                       break ;
      
                        case IDOK :
                            if ( !myblockalign ) {
                                tmpblocklist[a].aligner = 0 ; 
                                tmpblocklist[a].useralignparms = 0 ; 
                                tmpblocklist[a].alignparms[0] = '\0' ; }
                            first_timer = 0 ; 
                            tmpblocklist[a].oneseqpersp = myoneseqpersp ;
                            tmpblocklist[a].alsonote = myalsonote ;
                            if ( myminslen ) { GETINT ( tmpblocklist[a].minslen , 209 ) ; }
                            else tmpblocklist[a].minslen = 0 ; 
                            if ( mymaxslen ) { GETINT ( tmpblocklist[a].maxslen , 309 ) ; }
                            else tmpblocklist[a].maxslen = 0 ; 
                            tmpblocklist[a].genome = mygenom ;
                            GETINT ( tmpblocklist[a].stringsim , 226 ) ;
                            if ( !mytype ) tmpblocklist[a].isdna = 1 ;
                            else tmpblocklist[a].isdna = 0 ;
                            tmpblocklist[a].category = 0 ;
                            tmpblocklist[a].voucher = myvouch ;
                            tmpblocklist[a].complete = mycompl ;
                            tmpblocklist[a].missgaps = mygapstat ; 
                            for ( i = 0 , j = 1 ; i < 6 ; ++ i , j <<= 1 ) 
                                if ( myloc [ i ] )
                                    tmpblocklist[a].category |= j ;
                            if ( tocopy_opt )   
                              cpy_block_opt ( a ) ; 
                            tocopy_opt = 0 ;
                            if ( myintgen ) {
                                tmpblocklist[a].twogen[0] = 1 ; 
                                for ( i = 0 ; i < MAXPRODS_PER_BLOCK ; ++ i )
                                     strcpy ( tmpblocklist[a].prod[i] , intergen_buf.prod[i] ) ;
                                tmpblocklist[a].twogen[1] = intergen_buf.twogen[1] ; 
                                tmpblocklist[a].twogen[2] = intergen_buf.twogen[2] ; }
                            else {
                                for ( i = 0 ; tmpblocklist[a].prod[i][0] != '\0' && i < MAXPRODS_PER_BLOCK ; ++ i ) 
                                    tmpblocklist[a].prod[i][0] = '\0' ; 
                                tmpblocklist[a].twogen[0] = tmpblocklist[a].twogen[1] = tmpblocklist[a].twogen[2] = 0 ; }
                            EndDialog ( hdwnd, 1 ) ;
                            return 1 ; 
                        case IDCANCEL :
                           EndDialog ( hdwnd , 0 ) ;
                           return 0;
                        default :  break ; }
         break ; }
    return 0;
}

void project_edit ( int existing ) 
{
      int a , b , x , set, bkp , ismod ;
      time_t curtime ; 
      unsigned long int fis ;
      if ( !existing ) {
        time ( &proj_modtime ) ; 
        if ( have_open_project && have_something_to_save ) {
           sprintf ( junkstr , "%s has unsaved modifications\nAre you sure you want to close it?" , project_name ) ;
           a = MessageBox ( hwnd , junkstr , "Please confirm" , MB_YESNO ) ;
           if ( a != IDYES ) return ; 
           initlzd = 0 ; }
        SetWindowText ( hwnd , "GenBank to TNT - Unnamed project..." ) ;
        unnamed_project = first_timer = 1 ;
        if ( first_timer ) {
             strcpy ( uplistintro , "  ( write here the list of source files to" ) ; strcat ( uplistintro , linend ) ;
             strcat ( uplistintro , "    process, one per line, or use the" ) ; strcat ( uplistintro , linend ) ; 
             strcat ( uplistintro , "    \"find file(s)\" button to select files )" ) ; strcat ( uplistintro , linend ) ; 
             strcpy ( lolistintro , "   ( include here the list of products/genes" ) ; strcat ( lolistintro , linend ) ; 
             strcat ( lolistintro , "     to retrieve, one per line; if you leave" ) ; strcat ( lolistintro , linend ) ;  
             strcat ( lolistintro , "     this box blank, all the accessions in" ) ; strcat ( lolistintro , linend ) ; 
             strcat ( lolistintro , "     the file will be retrieved, according to" ) ; strcat ( lolistintro , linend ) ; 
             strcat ( lolistintro , "     the \"Options\" settings )" ) ; strcat ( lolistintro , linend ) ; }
        proj_numblocks = numblocks = 1 ; 
        postaxo[0] = negtaxo[0] = '\0' ;
        want_hybrids = want_confers = want_sp = want_environ = want_uncult = want_affinis = 0 ;
        wipe_before = wipe_after = 0 ;
        threshold = 100 ;
        for ( a = 0 ; a < MAXNUMBLOCKS ; ++ a ) {
          blocklist[a].maxslen = -1 ;  /* to make sure it's always "modified"  */
          tmpblocklist[a].missgaps = ALLMISSING ;
          tmpblocklist[a].aligner = 0 ;
          tmpblocklist[a].useralignparms = 0 ;
          tmpblocklist[a].alignparms[0] = '\0' ; 
          tmpblocklist[a].maxslen = tmpblocklist[a].minslen = 0 ; 
          tmpblocklist[a].genome = NUCLEAR ;
          tmpblocklist[a].stringsim = 100 ;
          tmpblocklist[a].isdna = 1 ;
          tmpblocklist[a].oneseqpersp = 1 ;
          tmpblocklist[a].alsonote = 0 ;
          tmpblocklist[a].complete = 0 ;
          tmpblocklist[a].voucher = 0 ;
          tmpblocklist[a].twogen[0] = tmpblocklist[a].twogen[1] = tmpblocklist[a].twogen[2] = 0 ; 
          tmpblocklist[a].category = GENE | CDS ; 
          sprintf ( tmpblocklist[a].blockname , "BLOCK_%i" , a + 1 ) ; 
          tmpblocklist[a].files[0][0] = '\0' ;
          tmpblocklist[a].prod[0][0] = '\0' ;
          tmpblocklist[a].blockheader[0] = '\0' ; }
        strcpy ( project_name , "Unnamed_project" ) ; }
      else {
          first_timer = 0  ;
          for ( a = 0 ; a < proj_numblocks ; ++ a )
               copy_a_block ( tmpblocklist+a , blocklist+a ) ; 
          numblocks = proj_numblocks ; }

      passed_through_init = 0 ; 
      a = DialogBox ( hInst , "CharDataEditDB" , hwnd , (DLGPROC) ProjectEditFunc ) ;
      if ( !a ) {
          if ( !existing ) {
             have_open_project = have_something_to_save = 0 ;  
             SetWindowText ( hwnd , "GenBank to TNT - No project open" ) ; }}
      else {
          have_open_project = 1 ;
          time ( &curtime ) ;
          newln () ; 
          myp ( "Project with %i TNT blocks: " , numblocks ) ; newln () ;
          if ( numblocks < proj_numblocks ) have_something_to_save = 1 ; 
          for ( a = 0 ; a < numblocks ; ++ a ) {
                // wipe_end_blanks ( a ) ; 
                wipe_dups ( a ) ;
                ismod = block_is_changed ( blocklist+a , tmpblocklist+a ) ; 
                if ( ismod == 1 ) {
                      blocklist[a].modtime = curtime ;
                      have_something_to_save = 1 ; }
                else if ( ismod == 2 ) have_something_to_save = 1 ; // this is when only gaps as missing changed!
                copy_a_block ( blocklist+a , tmpblocklist+a ) ;
                myp ( "     %-10s (%i files, %i prods.) - " , tmpblocklist[a].blockname , count_files( a ) , count_prods ( a ) ) ;
                showmoddate ( blocklist[a].modtime ) ;
                newln () ; }
          proj_numblocks = numblocks ; }
      eofscreen () ; 
      return ; 
} 


void noexplain ( void )
{
   if ( !first_timer ) return ; 
   junkstr[0] = junkstr[1] = '\0' ; SetDlgItemText ( uswnd , _UPLIST , junkstr ) ; 
   junkstr[0] = junkstr[1] = '\0' ; SetDlgItemText ( uswnd , _LOLIST , junkstr ) ;
   first_timer = 0 ;
   return ;
}

void save_a_block ( Blocktyp * src , FILE * fis )
{
    short int a , b ;
    char * l = ( char * ) &a ;
    char * r = l + 1 ;
    char x ; 
    fprintf ( fis , "%c" , src -> category ) ;
    fprintf ( fis , "%c" , src -> complete ) ;
    fprintf ( fis , "%c%c%c" , src -> twogen[0]  , src -> twogen[1] , src -> twogen[2] ) ;
    x = src -> voucher | ( src -> missgaps << 1 ) ;
    x |= ( src->useralignparms << 4 ) | ( src->aligner << 5 ) ; 
    fprintf ( fis , "%c" , x ) ;
    /***  aligner and parms to use for each block... *********/
    if ( src->useralignparms ) {
        a = strlen ( src->alignparms ) + 1 ;
        fprintf ( fis , "%c%c" , * l , * r ) ; 
        while ( a -- ) fprintf ( fis , "%c" , src->alignparms[a] ) ; }
    a = src -> stringsim ; fprintf ( fis , "%c%c" , * l , * r ) ; 
    fprintf ( fis , "%c%c%c%c" , src -> isdna , src -> genome , src -> genome , src -> genome ) ;
    fprintf ( fis , "%c%c" , src->oneseqpersp , src -> alsonote ) ;
    a = src -> minslen ;
    fprintf ( fis , "%c%c" , * l , * r ) ; 
    a = src -> maxslen ;
    fprintf ( fis , "%c%c" , * l , * r ) ;
    r = ( char *) &( src -> modtime ) ;
    for ( a = 0 ; a < sizeof ( time_t ) ; ++ a , ++ r ) fprintf ( fis , "%c" , * r ) ; 
    r = l + 1 ; 
    a = strlen ( src -> blockname ) + 1 ; fprintf ( fis , "%c%c" , * l , * r ) ;
    while ( a -- ) fprintf ( fis , "%c" , src -> blockname [a] ) ;
    for ( b = 0 ; b < MAXFILES_PER_BLOCK ; ++ b ) {
        a = strlen ( src -> files[b]) + 1 ; fprintf ( fis , "%c%c" , * l , * r ) ;
        while ( a -- ) fprintf ( fis , "%c" , src -> files[b][a] ) ; }
    for ( b = 0 ; b < MAXPRODS_PER_BLOCK ; ++ b ) {
        a = strlen ( src -> prod[b]) + 1 ; fprintf ( fis , "%c%c" , * l , * r ) ;
        while ( a -- ) fprintf ( fis , "%c" , src -> prod[b][a] ) ; }
    for ( b = 0 ; b < MAXPRODS_PER_BLOCK ; ++ b ) {
        a = strlen ( src -> prod2[b]) + 1 ; fprintf ( fis , "%c%c" , * l , * r ) ;
        while ( a -- ) fprintf ( fis , "%c" , src -> prod2[b][a] ) ; }
    a = strlen ( src -> blockheader ) + 1 ; fprintf ( fis , "%c%c" , * l , * r ) ;
    while ( a -- ) fprintf ( fis , "%c" , src -> blockheader [a] ) ; 
}

void save_the_project ( char * filnam )
{
    FILE * fis ;
    short int a ;
    char b ; 
    char * cp = ( char * ) &a ;
    int multisp = 1 << 5 ; 
    fis = fopen ( filnam , "wb" ) ;
    if ( fis == NULL ) {
        MessageBox ( hwnd , "Cannot open file for saving" , "ERROR" , MB_ICONERROR ) ;
        return ; }
    a = proj_numblocks ;
    fprintf ( fis , "%c%c" , cp[0] , cp[1] ) ;
    for ( a = 0 ; a < proj_numblocks ; ++ a ) save_a_block ( blocklist+a , fis ) ;
    /** Options common to all blocks ****/
    if ( dooneseqpersp ) multisp = 0 ;
    b = want_hybrids | multisp ; fprintf ( fis , "%c" , b ) ; 
    b = want_confers ; fprintf ( fis , "%c" , b ) ; 
    b = want_sp ; fprintf ( fis , "%c" , b ) ; 
    b = want_environ ; fprintf ( fis , "%c" , b ) ; 
    b = want_uncult ; fprintf ( fis , "%c" , b ) ; 
    b = want_affinis ; fprintf ( fis , "%c" , b ) ; 
    b = wipe_before ; fprintf ( fis , "%c" , b ) ; 
    b = wipe_after ; fprintf ( fis , "%c" , b ) ; 
    a = ( int ) ( threshold * 100 ) ;
    fprintf ( fis , "%c%c" , cp[0] , cp[1] ) ;
    a = strlen ( postaxo ) ;
    fprintf ( fis , "%c%c" , cp[0] , cp[1] ) ;
    while ( a -- ) fprintf ( fis , "%c" , postaxo[a] ) ; 
    a = strlen ( negtaxo ) ;
    fprintf ( fis , "%c%c" , cp[0] , cp[1] ) ;
    while ( a -- ) fprintf ( fis , "%c" , negtaxo[a] ) ;
    cp = ( char *) &( proj_modtime ) ;
    for ( a = 0 ; a < sizeof ( time_t ) ; ++ a , ++ cp ) fprintf ( fis , "%c" , * cp ) ; 
    fclose ( fis ) ;
    have_something_to_save = 0 ; 
    return ; 
}    

void read_a_block ( Blocktyp * src , FILE * fis )
{
    short int a , b ;
    char * l = ( char * ) &a ;
    char * r = l + 1 ;
    unsigned char x ; 
    fscanf ( fis , "%c" , &(src->category ) ) ;
    fscanf ( fis , "%c" , &(src->complete ) ) ;
    fscanf ( fis , "%c%c%c" , &(src->twogen[0]) , &(src->twogen[1]) , &(src->twogen[2]) ) ;  
    fscanf ( fis , "%c" , &x ) ;
    src->voucher = x & 1 ;
    src->missgaps = ( x & 15 ) / 2 ;
    /***  aligner and parms to use for each block... *********/
    src->useralignparms = ( x & ( 1 << 4 ) ) >> 4 ;
    src->aligner = ( x & ( 1 << 5 ) ) >> 5 ;
    if ( src->useralignparms ) {
        fscanf ( fis , "%c%c" , l , r ) ;
        while ( a -- ) fscanf ( fis , "%c" , &(src->alignparms[a]) ) ; }
    fscanf ( fis , "%c%c" , l , r ) ; src -> stringsim = a ; 
    fscanf ( fis , "%c%c%c%c" , &(src -> isdna) , &(src -> genome) , &(src -> genome) , &(src -> genome) ) ;
    fscanf ( fis , "%c%c" , &(src->oneseqpersp) , &(src -> alsonote) ) ;
    fscanf ( fis , "%c%c" , l , r ) ; 
    src -> minslen = a ;
    fscanf ( fis , "%c%c" , l , r ) ;
    src -> maxslen = a ;
    r = ( char *) &( src -> modtime ) ;
    for ( a = 0 ; a < sizeof ( time_t ) ; ++ a , ++ r ) fscanf ( fis , "%c" , r ) ; 
    r = l + 1 ; 
    fscanf ( fis , "%c%c" , l , r ) ;
    while ( a -- ) fscanf ( fis , "%c" , src -> blockname + a ) ;
    for ( b = 0 ; b < MAXFILES_PER_BLOCK ; ++ b ) {
        fscanf ( fis , "%c%c" , l , r ) ;
        while ( a -- ) fscanf ( fis , "%c" , src -> files[b] + a ) ; }
    for ( b = 0 ; b < MAXPRODS_PER_BLOCK ; ++ b ) {
        fscanf ( fis , "%c%c" , l , r ) ;
        while ( a -- ) fscanf ( fis , "%c" , src -> prod[b] + a ) ; }
    for ( b = 0 ; b < MAXPRODS_PER_BLOCK ; ++ b ) {
        fscanf ( fis , "%c%c" , l , r ) ;
        while ( a -- ) fscanf ( fis , "%c" , src -> prod2[b] + a ) ; }
    fscanf ( fis , "%c%c" , l , r ) ;
    while ( a -- ) fscanf ( fis , "%c" , src -> blockheader + a ) ; 
}

int read_the_project ( char * filnam )
{
    FILE * fis ;
    short int a ;
    char * l = ( char * ) &a ;
    char * r = l + 1 ;
    char b ; 
    if ( have_open_project && have_something_to_save ) {
       sprintf ( junkstr , "%s has unsaved modifications\nAre you sure you want to close it?" , project_name ) ;
       a = MessageBox ( hwnd , junkstr , "Please confirm" , MB_YESNO ) ;
       if ( a != IDYES ) return 0 ; }
    fis = fopen ( filnam , "rb" ) ;
    if ( fis == NULL ) {
        MessageBox ( hwnd , "Cannot open file for reading" , "ERROR" , MB_ICONERROR ) ;
        return 0 ; }
    fscanf ( fis , "%c%c" , l , r ) ; 
    proj_numblocks = numblocks = a ;
    for ( a = 0 ; a < proj_numblocks ; ++ a ) read_a_block ( blocklist+a , fis ) ;
    /*** Options common to all blocks ******/
    fscanf ( fis , "%c" , &b ) ;
    want_hybrids = b & 1 ;
    dooneseqpersp = 1 ;
    if ( ( b & ( 1 << 5 ) ) ) dooneseqpersp = 0 ;
    fscanf ( fis , "%c" , &b ) ; want_confers = b ; 
    fscanf ( fis , "%c" , &b ) ; want_sp = b ; 
    fscanf ( fis , "%c" , &b ) ; want_environ = b ; 
    fscanf ( fis , "%c" , &b ) ; want_uncult = b ; 
    fscanf ( fis , "%c" , &b ) ; want_affinis = b ; 
    fscanf ( fis , "%c" , &b ) ; wipe_before = b ; 
    fscanf ( fis , "%c" , &b ) ; wipe_after = b ; 
    fscanf ( fis , "%c%c" , l , r ) ; 
    threshold = ( double ) a / 100 ;
    fscanf ( fis , "%c%c" , l , r ) ;
    while ( a -- ) fscanf ( fis , "%c" , &(postaxo[a]) ) ; 
    fscanf ( fis , "%c%c" , l , r ) ;
    while ( a -- ) fscanf ( fis , "%c" , &(negtaxo[a]) ) ;
    r = ( char *) &( proj_modtime ) ;
    for ( a = 0 ; a < sizeof ( time_t ) ; ++ a , ++ r ) fscanf ( fis , "%c" , r ) ; 
    fclose ( fis ) ;
    initlzd = 0 ; 
    have_open_project = 1 ;
    unnamed_project = have_something_to_save = 0 ;
    check_source_types ( blocklist ) ; 
    return 1 ; 
}        
    
void wipe_dups ( int whichblock )   // wipes duplicate file/product names
{
    int a , b , c ;
    int lastc ; 
    Blocktyp * blk = tmpblocklist + whichblock ; 
    for ( a = 0 ; a < MAXFILES_PER_BLOCK - 1 ; ++ a ) {
        if ( blk -> files[a][0] == '\0' ) break ; 
        for ( b = a + 1 ; b < MAXFILES_PER_BLOCK ; ++ b ) {
            if ( blk -> files[b][0] == '\0' ) break ; 
            if ( !strcmp ( blk -> files[a] , blk -> files[b] ) ) {
                lastc = b ; 
                for ( c = b + 1 ; c < MAXFILES_PER_BLOCK ; ++ c ) {
                   if ( blk -> files[c][0] == '\0' ) break ;
                   lastc = c ; 
                   strcpy ( blk -> files[b] , blk -> files[c] ) ; }
                blk -> files[lastc][0] = '\0' ;
                -- b ; }}}
    for ( a = 0 ; a < MAXPRODS_PER_BLOCK - 1 ; ++ a ) {
        if ( blk -> prod[a][0] == '\0' ) break ; 
        for ( b = a + 1 ; b < MAXPRODS_PER_BLOCK ; ++ b ) {
            if ( blk -> prod[b][0] == '\0' ) break ; 
            if ( !strcmp ( blk -> prod[a] , blk -> prod[b] ) ) {
                lastc = b ; 
                for ( c = b + 1 ; c < MAXPRODS_PER_BLOCK ; ++ c ) {
                   if ( blk -> prod[c][0] == '\0' ) break ;
                   lastc = c ; 
                   strcpy ( blk -> prod[b] , blk -> prod[c] ) ; }
                blk -> prod[lastc][0] = '\0' ;
                -- b ; }}}
   return ;
}

void wipe_end_blanks ( int whichblock )   // gets rid of blanks at the end of the string
{
    int a , b , c ;
    int lastc ;
    char * cp , * sp ; 
    Blocktyp * blk = tmpblocklist + whichblock ; 
    for ( a = 0 ; a < MAXPRODS_PER_BLOCK ; ++ a ) {
        if ( blk -> prod[a][0] == '\0' ) break ;
        sp = blk -> prod[a] ;
        while ( isspace ( * sp ) ) ++ sp ; 
        b = strlen ( sp ) - 1 ;
        cp = sp + b ;
        while ( isspace ( * cp ) && cp > sp ) -- cp ;
        * ( ++ cp ) = '\0' ;
        strcpy ( blk -> prod[a] , sp ) ; }
    for ( a = 0 ; a < MAXFILES_PER_BLOCK ; ++ a ) {
        if ( blk -> files[a][0] == '\0' ) break ;
        sp = blk -> files[a] ;
        while ( isspace ( * sp ) ) ++ sp ; 
        b = strlen ( sp ) - 1 ;
        cp = sp + b ;
        while ( isspace ( * cp ) && cp > sp ) -- cp ;
        * ( ++ cp ) = '\0' ;
        strcpy ( blk -> files[a] , sp ) ; }
   return ;
}
        
int count_files ( int blk )
{
    int a = 0 ;
    Blocktyp * bp = blocklist + blk ; 
    for ( a = 0 ; a < MAXFILES_PER_BLOCK ; ++ a )
        if ( bp -> files[a][0] == '\0' ) return a ;
    return MAXFILES_PER_BLOCK ;
}

int count_prods ( int blk )
{
    int a = 0 ;
    Blocktyp * bp = blocklist + blk ; 
    for ( a = 0 ; a < MAXPRODS_PER_BLOCK ; ++ a )
        if ( bp -> prod[a][0] == '\0' ) return a ;
    return MAXPRODS_PER_BLOCK ;
}

char * squeeze_filename ( char * nam )  // a pointer to where the actual file name begins... 
{
    char * cp = nam + strlen ( nam ) ;
    while ( * cp != '\\' && cp > nam ) -- cp ;
    if ( * cp == '\\' ) return ( cp + 1 ) ;
    return cp ; 
}

char * afilname ( char * nam )  // a pointer to where the actual file name begins... 
{
    char * cp ;
    if ( show_full_paths ) return nam ; 
    cp = nam + strlen ( nam ) ;
    while ( * cp != '\\' && cp > nam ) -- cp ;
    if ( * cp == '\\' ) return ( cp + 1 ) ;
    return cp ; 
}

char * find_extension ( char * nam )
{
    char * cp = nam + strlen ( nam ) ;
    while ( * cp != '.' && cp > nam ) -- cp ;
    return cp ;
}

int filedepends , aligndepends ;
extern int skip_some_source_files ; 


BOOL CALLBACK CreateMatrixFunc (HWND hdwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int a , b , c , insel , ssel , val , to , i ;
    HWND tmpwnd ;
    HWND hWho ;
    HBITMAP hbmap ;
    double nuthrheshold ; 
    static int myfiledepends , myaligndepends ;
    static int myfileall , myalignall , mydefparms ;
    static int mydiscardlo , mydiscardhi ; 
    switch(message) {
        case WM_INITDIALOG :
             strcpy ( fn , squeeze_filename ( project_name ) ) ;
             sprintf ( find_extension ( fn ) , ".tnt" ) ; 
             sprintf ( junkstr , "Create TNT matrix \"%s\"..." , fn ) ;
             SetWindowText ( hdwnd , junkstr ) ; 
             if ( !prepare_selmem() ) DestroyWindow ( hdwnd ) ;
             BUTT_CHECK ( 200 ) ;
             BUTT_CHECK ( 204 ) ;
             mydiscardlo = mydiscardhi = 0 ; 
             SPIN_DISABLE ( 219 ) ; 
             SPIN_DISABLE ( 222 ) ; 
             if ( aligner_is != USER ) {
                 BUTT_CHECK ( 208 ) ;
                 mydefparms = 1 ;
                 Button_Enable ( GetDlgItem ( hdwnd , 210 ) , FALSE ) ; }
             else {
                 mydefparms = 0 ;
                 strcpy ( junkstr , "specify parameters (with $infile and $outfile):" ) ;
                 SetDlgItemText ( hdwnd , 209 , junkstr ) ;  
                 // Button_Enable ( GetDlgItem ( hdwnd , 208 ) , FALSE ) ; 
                 BUTT_CHECK ( 209 ) ; }
             myfileall = myalignall = 0 ; 
             myfiledepends = myaligndepends = 1 ;
             skip_matrix_creation = 0 ;
             BUTT_CHECK ( 217 ) ;
             autoruntnt = 1 ;
             BUTT_CHECK ( 218 ) ; 
             FOCUS_ON ( IDOK ) ;
             break;
        case WM_SIZE :
             break ; 
        case WM_DESTROY :
                break;
        case WM_COMMAND :
                val = LOWORD(wParam) ; 
                switch( LOWORD(wParam) ){

case 217:
   skip_matrix_creation = 1 - skip_matrix_creation ;
   if ( skip_matrix_creation ) {
       autoruntnt = 0 ;
       BUTT_UNCHEK ( 218 ) ; }
   break ;
case 218:
    autoruntnt = 1 - autoruntnt ;
    if ( autoruntnt ) {
        skip_matrix_creation = 0 ;
        BUTT_CHECK ( 217 ) ; }
    break ; 
case 209:
   Button_Enable ( GetDlgItem ( hdwnd , 210 ) , TRUE ) ; 
   mydefparms = 0 ; 
   sprintf ( junkstr , "" ) ;
   SetDlgItemText ( hdwnd , 210 , junkstr ) ;
   break ;
case 208:
   mydefparms = 1 ; 
   sprintf ( junkstr , "  (type parameters here)" ) ;
   SetDlgItemText ( hdwnd , 210 , junkstr ) ;
   Button_Enable ( GetDlgItem ( hdwnd , 210 ) , FALSE ) ; 
   break ;
case 201:
    myfileall = 1 ;
    myfiledepends = 0 ;
    break ;
case 205:
    myalignall = 1 ;
    myaligndepends = 0 ;
    break ;
case 200:
    show_all_dependencies ( 0 , 1 , 0 ) ; 
    myfileall = 0 ;
    myfiledepends = 1 ;
    break ;
case 204:
    show_all_dependencies ( 0 , 0 , 1 ) ; 
    myalignall = 0 ;
    myaligndepends = 1 ;
    break ;
                    
                    case 202 :   // extract selected files
                       myfileall = myfiledepends = 0 ; 
                       these_are_files = 1 ;
                       b = 0 ;
                       skip_some_source_files = FASTA | ALIGNED | TNT ; 
                       for ( a = 0 ; a < proj_numblocks ; ++ a )
                          for ( c = 0 ; c < MAXFILES_PER_BLOCK ; ++ c )
                               if ( blocklist[a].files[c][0] == '\0' ) break ;
                               else allnamespt[b++] = blocklist[a].files[c] ; 
                       prepare_selection ( "Files to Extract", "Unselected Files", allnamespt , b ) ;
                       for ( a = b ; a -- ; ) selectlist [ a ] = listoffiles[ a ] ; 
                       a = SELECT_DIALOG ;
                       skip_some_source_files = 0 ;
                       if ( a ) 
                           for ( a = b ; a-- ; ) listoffiles[ a ] = selectlist [ a ] ;
                               
                       break ;
                    case 206 :   // align selected files
                       myalignall = myaligndepends = 0 ; 
                       these_are_files = 0 ;
                       for ( a = 0 ; a < proj_numblocks ; ++ a ) {
                           allnamespt[a] = blocklist[a].blockname ;
                           if ( ( blocklist[a].sourcetype & ( ALIGNED | TNT ) ) ) allnamespt[a] = NULL ; }
                       prepare_selection ( "Blocks to Align", "Unselected Blocks", allnamespt , proj_numblocks ) ;
                       for ( a = proj_numblocks ; a-- ; ) selectlist [ a ] = listofblocks[ a ] ; 
                       a = SELECT_DIALOG ;
                       if ( a ) 
                           for ( a = proj_numblocks ; a-- ; ) listofblocks[ a ] = selectlist [ a ] ;
                       break ;

                       
                    case IDCANCEL :
                          EndDialog ( hdwnd, 0 ) ; 
                          return 0;
                    case IDOK :
                          if ( ( use_user_params = ( mydefparms == 0 ) ) )
                              GetDlgItemText ( hdwnd , 210 , userparams , MAX_PATH * 2 ) ;
                          if ( myfileall ) 
                               for ( a = 0 ; a < totfiles_to_do ; ++ a ) listoffiles[a] = 1 ;
                          else if ( !myfiledepends )
                                 for ( a = 0 ; a < totfiles_to_do ; ++ a )
                                      if ( listoffiles[a] < 1 ) listoffiles [ a ] = -1 ;
                          if ( myalignall )
                               for ( a = 0 ; a < proj_numblocks ; ++ a ) listofblocks[a] = listoftemps[a] = 1 ;
                          else if ( !myaligndepends )
                                 for ( a = 0 ; a < proj_numblocks ; ++ a )
                                      if ( listofblocks[a] < 1 ) listofblocks [ a ] = -1 ;
                          aligndepends = myaligndepends ;
                          filedepends = myfiledepends ;

    for ( a = c = 0 ; a < proj_numblocks ; ++ a ) {
        if ( listofblocks[a] < 1 ) continue ; 
        if ( blocklist[a].sourcetype == TNT ) continue ; 
        if ( blocklist[a].sourcetype == ALIGNED ) continue ; 
        if ( num_distinct_sequences ( a ) < 4 ) continue ; 
        if ( blocklist[a].aligner != aligner_is && blocklist[a].aligner ) ++ c ;
        if ( blocklist[a].useralignparms && use_user_params ) ++ c ; }
        overrideblockspecs = 0 ; 
        if ( c ) {
             c = MessageBox ( hdwnd , "Current alignment specification differs from the one\n"
                                      "previously chosen for some individual blocks...\n\n"
                                      "   Use the current specification anyway (yes),\n"
                                      "   specification for each individual block (no)\n"
                                      "   or cancel?" , "Ooops!" , MB_ICONWARNING | MB_YESNOCANCEL ) ;
             if ( c == IDCANCEL ) break ;
             if ( c == IDYES ) overrideblockspecs = 1 ; }
                          
                          EndDialog ( hdwnd, 1 ) ;
                          return 1;
                    default : break ; }}
    return 0 ;
}

int show_all_dependencies ( int display , int setextracts , int setaligns )
{
    int a , b , c , somemissingfiles , cancreateit = 1 ;
    int nowatfile = 0 ;
    int mustrealign ; 
    time_t ref , is , alndate , tmpdate ;
    char * extisat ;
    int allisok = 1 ; 
    totfiles_to_do = 0 ;  
    for ( a = 0 ; a < proj_numblocks ; ++ a )
       for ( c = 0 ; c < MAXFILES_PER_BLOCK ; ++ c )
           if ( blocklist[a].files[c][0] == '\0' ) break ;
           else allnamespt[ totfiles_to_do ++] = blocklist[a].files[c] ; 
    nowatfile = 0 ;
    if ( display ) {
        newln () ;
        myp ( "DEPENDENCIES FOR PROJECT \"%s\":" , afilname ( project_name ) ) ;
        newln () ; }
    for ( a = 0 ; a < proj_numblocks ; ++ a ) {
         somemissingfiles = 0 ;
         listoftemps[ a ] = 0 ; 
         if ( setaligns ) listofblocks [ a ] = 0 ; 
         mustrealign = 0 ;
         if ( display ) {
             newln () ; 
             myp ( "Block %i (\"%s\"), modified " , a+1 , blocklist[a].blockname ) ;
             showmoddate ( blocklist[a].modtime ) ;
             newln () ; }
         strcpy ( fn , project_name ) ; 
         * ( extisat = find_extension ( fn ) ) = '\0' ;
         sprintf ( extisat , "_bk%i_%s.fas" , a+1 , blocklist[a].blockname ) ;
         tmpdate = getfilemodtime ( fn ) ;
         sprintf ( extisat , "_bk%i_%s.aln" , a+1 , blocklist[a].blockname ) ;
         alndate = getfilemodtime ( fn ) ;
         if ( difftime ( blocklist[a].modtime , alndate ) > 0 ) mustrealign = 1 ;
         if ( alndate == -1 ) mustrealign = 1 ;
         if ( tmpdate == -1 ) { mustrealign = 1 ; listoftemps[ a ] = 1 ; if ( blocklist[a].sourcetype < ALIGNED ) allisok = 0 ; }
         for ( b = 0 ; b < MAXFILES_PER_BLOCK ; ++ b ) {
             if ( blocklist[a].files[b][0] == '\0' ) break ;
             if ( !setextracts && setaligns ) {
                 if ( listoffiles[nowatfile] > 0 )
                     allisok = 0 ;
                     listoftemps[a] = 1 ; 
                     mustrealign = 1 ;
                 ++ nowatfile ;
                 continue ; } 
             listoffiles [ nowatfile ] = 0 ; 
             if ( display ) myp ( "     File \"%s\": " , afilname ( blocklist[a].files[b] ) ) ;
             ref = getfilemodtime ( blocklist[a].files[b] ) ;
             if ( ref == -1 ) {
                 if ( display ) {
                    myp ( "not found!" ) ; newln () ; }
                 somemissingfiles = 1 ;
                 if ( setextracts ) listoffiles [ nowatfile ] = 2 ;
                 allisok = 0 ;
                 nowatfile ++ ; 
                 continue ; }
             sprintf ( extisat , "_bk%i_file%i.tmp" , a+1 , b+1 ) ;
             is = getfilemodtime ( fn ) ;
             if ( is == -1 ) {
                 if ( blocklist[a].sourcetype != TNT ) {
                    if ( display ) 
                       myp ( "not extracted yet (\"%s\" doesn't exist)" , afilname ( fn ) ) ;
                    mustrealign = 1 ;
                    allisok = 0 ; 
                    listoftemps [ a ] = 1 ; 
                    if ( setextracts ) { listoffiles[nowatfile] = 1 ; allisok = 0 ; }}
                 else {
                     if ( alndate == -1 || difftime ( ref , alndate ) > 0 ) {
                         listoffiles[ nowatfile ] = 1 ;
                         allisok = 0 ;
                         if ( display ) 
                            myp ( "will directly copy file onto *.aln" ) ; }
                     else
                      if ( display ) 
                        myp ( "not needed (native TNT format)" ) ; }}
             else {
                if ( difftime ( is , alndate ) > 0 ) { mustrealign = 1 ; allisok = 0 ; }
                if ( difftime ( is , tmpdate ) > 0 ) { mustrealign = listoftemps [ a ] = 1 ; allisok = 0 ; }
                if ( difftime ( is , blocklist[a].modtime ) > 0 && difftime ( is , ref ) > 0 ) {
                    if ( display ) 
                        myp ( "OK (\"%s\" up-to-date)" , afilname ( fn ) ) ; }
                else {
                    if ( setextracts ) listoffiles[nowatfile] = 1 ; 
                    mustrealign = 1 ;
                    listoftemps [ a ] = 1 ;
                    allisok = 0 ; 
                    if ( display ) 
                        myp ( "must be re-extracted (\"%s\" is old)" , afilname ( fn ) ) ; }}
             if ( display ) newln () ;
             ++ nowatfile ; }
         if ( somemissingfiles ) {
             if ( display ) 
                 myp ( "     ---> Block cannot be evaluated (some source files missing)\n" ) ;
             allisok = 0 ;     
             cancreateit = 0 ; 
             continue ; }
         if ( display && blocklist[a].sourcetype < ALIGNED ) {
                sprintf ( extisat , "_bk%i_%s.fas" , a+1 , blocklist[a].blockname ) ;
                if ( listoftemps[a] )
                    if ( tmpdate == -1 )
                          myp ( "     ---> Temporary FASTA file for block needed (\"%s\" doesn't exist)\n" , afilname ( fn ) ) ;
                     else myp ( "     ---> Temporary FASTA file for block needed (\"%s\" is old)\n" , afilname ( fn ) ) ;
                else myp ( "     ---> Temporary FASTA file for block is up-to-date\n" ) ; }
         sprintf ( extisat , "_bk%i_%s.aln" , a+1 , blocklist[a].blockname ) ;
         if ( mustrealign && setaligns ) {
                listofblocks[a] = 1 ;
                if ( blocklist[a].sourcetype < ALIGNED ) allisok = 0 ; 
                if ( display ) 
                  if ( !( blocklist[a].sourcetype & ( ALIGNED | TNT ) ) ) {
                    if ( alndate != -1 ) {
                       if ( listoftemps[a] )
                             myp ( "     ---> Block must be re-aligned (\"%s\" is older than temporary FASTA file)\n" , afilname ( fn ) ) ; 
                       else myp ( "     ---> Block must be re-aligned (\"%s\" is old)\n" , afilname ( fn ) ) ; }
                    else myp ( "     ---> Block must be re-aligned (\"%s\" doesn't exist)\n" , afilname ( fn ) ) ; }}
         else {
               if ( difftime ( tmpdate , alndate ) > 0 ) {
                    listofblocks[a] = 1 ;
                    if ( blocklist[a].sourcetype < ALIGNED ) allisok = 0 ; 
                    if ( display )
                        myp ( "     ---> Block must be re-aligned (\"%s\" is older than temporary FASTA file)\n" , afilname ( fn ) ) ; }
               else 
               if ( display ) 
                   myp ( "     ---> Block is OK (\"%s\" up-to-date)\n" , afilname ( fn ) ) ; }
        }

        if ( display ) {
            if ( !cancreateit ) {
              newln () ; 
              myp ( "The following source files are missing:" ) ; newln () ;
              totfiles_to_do  = 0 ;
              for ( a = 0 ; a < proj_numblocks ; ++ a )
                 for ( c = 0 ; c < MAXFILES_PER_BLOCK ; ++ c ) {
                     if ( blocklist[a].files[c][0] == '\0' ) break ;
                     if ( listoffiles[totfiles_to_do ++] == 2 ) {
                         myp ( "   Block %s, file %i: %s" , blocklist[a].blockname , c + 1 , afilname ( blocklist[a].files[c] ) ) ;
                         newln () ; }}}
            else if ( allisok ) myp ( "\nPROJECT IS READY FOR BUILDING TNT MATRIX...\n" ) ; 
            newln () ;
            eofscreen () ; }
        return cancreateit ; 
}             
             
int warn_for_unsaved_data ( void )
{  int i ; 
   if ( have_open_project && have_something_to_save ) {
     sprintf ( junkstr , "You have unsaved modifications to \"%s\"\nDo you want to save the project before quitting?" , project_name ) ;
     i = MessageBox ( hwnd , junkstr , "Warning" , MB_ICONWARNING | MB_YESNOCANCEL ) ;
     if ( i == IDCANCEL ) return 0 ; 
     if ( i == IDYES )
         handle_savings ( IDM_SAVEPROJECT ) ;
     return 1 ; }
   return 1 ;
}

void erase_all_temporary_files ( void )
{
    int a , b ;
    char * extisat ; 
    strcpy ( fn , project_name ) ;
    * ( extisat = find_extension ( fn ) ) = '\0' ;
    sprintf ( junkstr , "Erasing all temporary files will require re-extraction\nand re-alignment of all source files in the project...\n\nAre you sure you wish to erase them?" ) ;
    a = MessageBox ( hwnd , junkstr , "Please confirm" , MB_YESNO ) ;
    if ( a == IDNO ) return ;
    for ( a = 0 ; a < proj_numblocks ; ++ a ) {
        for ( b = 0 ; b < MAXFILES_PER_BLOCK ; ++ b ) {
            if ( blocklist[a].files[b][0] == '\0' ) break ;
            sprintf ( extisat , "_bk%i_file%i.tmp" , a+1 , b+1 ) ;
            DeleteFile ( fn ) ; }
        sprintf ( extisat , "_bk%i_%s.fas" , a+1 , blocklist[a].blockname ) ;
        DeleteFile ( fn ) ; 
        sprintf ( extisat , "_bk%i_%s.aln" , a+1 , blocklist[a].blockname ) ;
        DeleteFile ( fn ) ; }
    myp ( "Erasing all temporary files for project \"%s\"" , afilname ( project_name ) ) ;
    newln () ;
    eofscreen () ;
    inva ; 
    return ; 
}

BOOL CALLBACK GetDoubleGeneFunc (HWND hdwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int a , b , insel , ssel , val , to ;
    HWND tmpwnd ;
    HWND hWho ;
    HBITMAP hbmap ;
    static Blocktyp * myblock , * yosoy ;
    int cualwin ;
    static inclfrom , inclto ;
    char * cp , * cpp , * qp ;
    int more , numlins , havenonblanks ; 
    switch(message) {
        case WM_INITDIALOG:
            myblock = tmpblocklist + ( numoption + showfrom ) ;
            yosoy = &intergen_buf ;
            cualwin = 201 ; 
            junkstr[0] ='\0' ;
            for ( a = b = 0 ; myblock -> prod[a][0] != '\0' && a < MAXPRODS_PER_BLOCK ; ++ a ) {
               if ( b ++ ) strcat ( junkstr , linend ) ;
               if ( !strcmp ( myblock->prod[a] , "<>" ) ) {
                   SetDlgItemText ( hdwnd , cualwin , junkstr ) ;
                   junkstr[0] = '\0' ; 
                   cualwin = 202 ;
                   b = 0 ; }
               else strcat ( junkstr , myblock -> prod[a] ) ; }
            inclfrom = inclto = 0 ; 
            if ( myblock -> twogen[1] == 1 ) { inclfrom = 1 ; BUTT_CHECK ( 205 ) ; }
            if ( myblock -> twogen[2] == 1 ) { inclto = 1 ; BUTT_CHECK ( 206 ) ; }
            SetDlgItemText ( hdwnd , cualwin , junkstr ) ;
            FOCUS_ON ( IDOK ) ;
            break;
        case WM_COMMAND:
             switch ( LOWORD ( wParam ) ) {
                 case 205:
                    inclfrom = 1 - inclfrom ;
                    break ;
                 case 206:
                    inclto = 1 - inclto ;
                    break ; 
                 case IDOK:
                   a = GetDlgItemText( hdwnd , 201 , junkstr, 16000 ) ;
                   junkstr[a] = '\0' ;
                   numlins = 0 ;
                   cp = junkstr ; 
                   while ( * cp ) {
                       cpp = cp ;
                       more = 0 ; 
                       while ( * cpp && * cpp != 13 ) ++ cpp ;
                       if ( * cpp == 13 ) { * cpp = '\0' ; more = 1 ; }
                       qp = cp ;
                       havenonblanks = 0 ; 
                       while ( * qp != 13 && * qp != '\0' && !havenonblanks ) { if ( !isspace( * qp ) ) havenonblanks = 1 ; ++ qp ; }
                       if ( havenonblanks ) {
                           strcpy ( yosoy ->prod[numlins] , cp ) ; 
                           numlins ++ ; }
                       if ( more ) cpp += 2 ;
                       cp = cpp ; }
                   if ( numlins < MAXPRODS_PER_BLOCK ) strcpy ( yosoy ->prod[numlins++] , "<>" ) ;
                   if ( numlins < MAXPRODS_PER_BLOCK ) yosoy ->prod[numlins][0] = '\0' ; 
                   a = GetDlgItemText( hdwnd , 202 , junkstr, 16000 ) ;
                   junkstr[a] = '\0' ;
                   cp = junkstr ; 
                   while ( * cp ) {
                       cpp = cp ;
                       more = 0 ; 
                       while ( * cpp && * cpp != 13 ) ++ cpp ;
                       if ( * cpp == 13 ) { * cpp = '\0' ; more = 1 ; }
                       qp = cp ;
                       havenonblanks = 0 ; 
                       while ( * qp != 13 && * qp != '\0' && !havenonblanks ) { if ( !isspace( * qp ) ) havenonblanks = 1 ; ++ qp ; }
                       if ( havenonblanks ) {
                           strcpy ( yosoy->prod[numlins] , cp ) ; 
                           numlins ++ ;
                           if ( numlins < MAXPRODS_PER_BLOCK ) yosoy->prod[numlins][0] = '\0' ; }
                       if ( more ) cpp += 2 ;
                       cp = cpp ; }
                    junkstr[0] ='\0' ;
                    for ( a = 0 ; yosoy->prod[a][0] != '\0' && a < MAXPRODS_PER_BLOCK ; ++ a ) {
                        if ( a ) strcat ( junkstr , linend ) ; 
                        strcat ( junkstr , yosoy->prod[a] ) ; }
                    SetDlgItemText ( uswnd , _LOLIST+numoption , junkstr ) ; 
                    EndDialog ( hdwnd , 1 ) ;
                    yosoy -> twogen[1] = 0 ;
                    if ( inclfrom ) yosoy -> twogen[1] = 1 ;
                    yosoy -> twogen[2] = 0 ;
                    if ( inclto ) yosoy -> twogen[2] = 1 ;
                    return 1 ; 
                    break ;
                 case IDCANCEL:
                    EndDialog ( hdwnd , 0 ) ;
                    break ;
                 default: break ; }
        default: break ; }
   return 0 ; 
}

extern double threshold ;

BOOL CALLBACK InclusionCriteriaFunc (HWND hdwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HWND tmpwnd ;
    HWND hWho ;
    int i , j ; 
    static int mypostaxo , mynegtaxo ;
    static int mythreshold ;
    double tmp ;
    static int mywipe_before , mywipe_after ;
    static int myhyb , mysp , myaff , mycf , myenv , myuncul ;
    int changemodtime ; 
    switch(message) {
        case WM_INITDIALOG:
            mypostaxo = ( postaxo[0] != '\0' ) ; 
            mynegtaxo = ( negtaxo[0] != '\0' ) ;
            if ( !mypostaxo ) {
                   SetDlgItemText ( hdwnd , 214 , "(taxon list - blank separated)" ) ; 
                   Button_Enable ( GetDlgItem ( hdwnd , 214 ) , FALSE ) ; }
            else {
                SetDlgItemText ( hdwnd , 214 , postaxo ) ;
                BUTT_CHECK ( 213 ) ; }
            if ( !mynegtaxo ) {
                   SetDlgItemText ( hdwnd , 216 , "(taxon list - blank separated)" ) ; 
                   Button_Enable ( GetDlgItem ( hdwnd , 216 ) , FALSE ) ; }
            else {
                SetDlgItemText ( hdwnd , 216 , negtaxo ) ;
                BUTT_CHECK ( 215 ) ; }
            tmp = ( threshold * 100 ) ;
            mythreshold = ( int ) tmp ; 
            SET_UPDOWN ( 203 , 1 , 100 , mythreshold ) ;
            mywipe_before = ( wipe_before > 0 ) ; 
            SET_UPDOWN ( 208 , 0 , 100 , wipe_before ) ; 
            mywipe_after = ( wipe_after > 0 ) ; 
            SET_UPDOWN ( 211 , 0 , 100 , wipe_after ) ; 
            myhyb = want_hybrids ;
            mysp = want_sp ;
            myenv = want_environ ;
            myaff = want_affinis ;
            mycf = want_confers ;
            myuncul = want_uncult ;
            if ( !myhyb ) BUTT_CHECK ( 218 ) ;
            if ( !mysp ) BUTT_CHECK ( 221 ) ;
            if ( !myenv ) BUTT_CHECK ( 222 ) ;
            if ( !myaff ) BUTT_CHECK ( 220 ) ;
            if ( !mycf ) BUTT_CHECK ( 219 ) ;
            if ( !myuncul ) BUTT_CHECK ( 223 ) ;
            if ( wipe_before ) BUTT_CHECK ( 206 ) ;
            if ( wipe_after ) BUTT_CHECK ( 209 ) ;
            FOCUS_ON ( IDOK ) ;
            break;
        case WM_COMMAND:
             switch ( LOWORD ( wParam ) ) {

                case 218 :  myhyb = 1 - myhyb ; break ;
                case 219 :  mycf = 1 - mycf ; break ;
                case 220 :  myaff = 1 - myaff ; break ;
                case 221 :  mysp = 1 - mysp ; break ;
                case 222 :  myenv = 1 - myenv ; break ;
                case 223 :  myuncul = 1 - myuncul ; break ; 
                case 206:
                    mywipe_before = 1 - mywipe_before ;
                    break ;
                case 209:
                    mywipe_after = 1 - mywipe_after ;
                    break ;
                case 213:  // pos taxo
                   mypostaxo = 1 - mypostaxo ;
                   if ( mypostaxo ) {
                       Button_Enable ( GetDlgItem ( hdwnd , 214 ) , TRUE ) ;
                       SetDlgItemText ( hdwnd , 214 , postaxo ) ; }
                   else {
                       SetDlgItemText ( hdwnd , 214 , "(taxon list - blank separated)" ) ; 
                       Button_Enable ( GetDlgItem ( hdwnd , 214 ) , FALSE ) ; }
                   break ; 
                case 215:  // neg taxo
                   mynegtaxo = 1 - mynegtaxo ;
                   if ( mynegtaxo ) {
                       Button_Enable ( GetDlgItem ( hdwnd , 216 ) , TRUE ) ;
                       SetDlgItemText ( hdwnd , 216 , negtaxo ) ; }
                   else {
                       SetDlgItemText ( hdwnd , 216 , "(taxon list -blank separated)" ) ; 
                       Button_Enable ( GetDlgItem ( hdwnd , 216 ) , FALSE ) ; }
                   break ;
                 case IDOK:
                    if ( mywipe_after && mywipe_before ) {
                        GETINT ( i , 207 ) ; GETINT ( j , 210 ) ;
                        if ( i >= j ) {
                            MessageBox ( hdwnd , "Categories to discard *before* must be \nsmaller than categories to discard *after*" , "ERROR" , MB_OK ) ;
                            break ; }}
                    changemodtime = 0 ;
                    strcpy ( junkstr , postaxo ) ; 
                    if ( mypostaxo ) GetDlgItemText ( hdwnd , 214 , postaxo , MAXTAXO ) ;
                    else postaxo[0] = '\0' ;
                    if ( strcmp ( junkstr , postaxo ) ) changemodtime = 1 ; 
                    strcpy ( junkstr , negtaxo ) ; 
                    if ( mynegtaxo ) GetDlgItemText ( hdwnd , 216 , negtaxo , MAXTAXO ) ; 
                    else negtaxo[0] = '\0' ;
                    if ( strcmp ( junkstr , negtaxo ) ) changemodtime = 1 ;
                    if ( mywipe_before ) GETINT ( wipe_before , 207 )  else wipe_before = 0 ;
                    if ( mywipe_after ) GETINT ( wipe_after , 210 )  else wipe_after = 0 ;
                    GETINT ( i , 202 ) ;
                    threshold = ( double ) i / 100 ; 
                    have_something_to_save = 1 ;
                    if ( want_hybrids != myhyb ) { want_hybrids = myhyb ; changemodtime = 1 ; }
                    if ( want_confers != mycf ) { want_confers = mycf ; changemodtime = 1 ; }
                    if ( want_sp != mysp ) { want_sp = mysp ; changemodtime = 1 ; }
                    if ( want_environ != myenv ) { want_environ = myenv ; changemodtime = 1 ; }
                    if ( want_uncult != myuncul ) { want_uncult = myuncul ; changemodtime = 1 ; }
                    if ( want_affinis != myaff ) { want_affinis = myaff ; changemodtime = 1 ; }
                    time ( &proj_modtime ) ;
                    if ( changemodtime ) touch_all_fastas () ; 
                    EndDialog ( hdwnd , 1 ) ;
                    break ;
                 case IDCANCEL:
                    EndDialog ( hdwnd , 0 ) ;
                    break ;
                 default: break ; }
        default: break ; }
   return 0 ; 
}

void do_inclusion_criteria ( void )
{
  DialogBox ( hInst , "InclusionCriteriaDB" , hwnd , (DLGPROC) InclusionCriteriaFunc ) ;
}  


char mybioeditselection[ MAX_PATH ] ;

void nametheitem_4_bioedit ( int a )
{
    char * extisat ; 
    itemname = itemnamespace ;
    strcpy ( itemname , project_name ) ;
    * ( extisat = find_extension ( itemname ) ) = '\0' ;
    sprintf ( extisat , "_bk%i_%s.aln" , a+1 , blocklist[a].blockname ) ;
}


BOOL CALLBACK BioEditCheckFunc ( HWND hdwnd, UINT message, WPARAM wParam, LPARAM lParam ) 
{
    static HWND hselwnd ;
    int a , b , c , insel , firstvalid ;
    static int totnumfiles ;
    FILE * tst ; 
    switch(message) {
          case WM_INITDIALOG :
                if ( !prepare_selmem() ) DestroyWindow ( hdwnd ) ;
                hselwnd = GetDlgItem ( hdwnd , 200 ) ;
                b = 0 ;
                for ( a = 0 ; a < proj_numblocks ; ++ a )
                    allnamespt[b++] = blocklist[a].blockname ; 
                prepare_selection ( "Files to Extract", "Unselected Files", allnamespt , b ) ;
                totnumfiles = b ; 
                insel = 0 ;
                firstvalid = -1; 
                for ( a = 0 ; a < proj_numblocks ; a ++ ) {
                     nametheitem_4_bioedit ( a ) ;
                     tst = fopen ( itemname , "r" ) ;
                     if ( tst == NULL ) continue ;
                     if ( firstvalid < 0 ) firstvalid = a ; 
                     fclose ( tst ) ; 
                     ComboBox_InsertString ( hselwnd , insel , squeeze_filename ( itemname ) ) ;
                     ComboBox_SetItemData ( hselwnd , insel , a ) ;
                     ++ insel ; }
                if ( insel == 0 ) {
                    MessageBox ( hdwnd , "No aligned files have been produced yet!\nYou must extract and align before checking with BioEdit" , "ERROR" , MB_OK ) ;
                    EndDialog ( hdwnd , 0) ;
                    break ; }
                nametheitem_4_bioedit ( firstvalid ) ; 
                ComboBox_SelectString ( hselwnd , firstvalid , squeeze_filename ( itemname ) ) ;
                SetWindowRedraw ( hselwnd , TRUE ) ;
                InvalidateRect ( hselwnd , NULL , TRUE ) ;
                FOCUS_ON (IDOK ) ; 
                break;
          case WM_COMMAND :
              switch(LOWORD(wParam)){
                        case IDOK :
                              GetDlgItemText ( hdwnd , 200 , junkstr , MAX_PATH ) ;
                              insel = ComboBox_GetCurSel ( hselwnd ) ;
                              if ( insel == CB_ERR ) {
                                  MessageBox ( hdwnd , "Wrong file name" , "ERROR" , MB_OK ) ;
                                  break ; }
                              a = ComboBox_GetItemData ( hselwnd , insel ) ;
                              nametheitem_4_bioedit ( a ) ; 
                              strcpy ( mybioeditselection , itemname ) ; 
                              EndDialog ( hdwnd, 1 ) ;
                           break ; 
                        case IDCANCEL :
                           EndDialog ( hdwnd, 0 ) ;
                           break ; 
                        default :
                                break;
                }
         break; }
    return 0;
}
  
void do_bioedit_check ( void )
{
    time_t datwas , datis ;
    FILE * in , * out ;
    char * extisat ; 
    int a = DialogBox ( hInst , "BioEditCheckDB" , hwnd , (DLGPROC) BioEditCheckFunc ) ;
    if ( !a ) return ;
    datwas = getfilemodtime ( mybioeditselection ) ;
    strcpy ( fn , "\"" ) ;
    strcat ( fn , mybioeditselection ) ;
    strcat ( fn , "\"" ) ; 
    my_spawn ( bioeditexe , fn , NULL ) ;
    datis = getfilemodtime ( mybioeditselection ) ;
    if ( datis == datwas ) return ;
    sprintf ( junkstr , "File \"%s\" has changed.\nDo you want to trim gaps for re-aligning?\n(answering \"no\" will use file as is)" , afilname ( mybioeditselection ) ) ; 
    a = MessageBox ( hwnd , junkstr , "File changed..." , MB_YESNO ) ;
    if ( a == IDYES ) {
        myp ( "File \"%s\" has been edited, and will be copied onto *.fas for re-alignment" , afilname ( mybioeditselection ) ) ; 
        in = fopen ( mybioeditselection , "rb" ) ;
        sprintf ( extisat = find_extension ( mybioeditselection ) , ".fas" ) ; 
        out = fopen ( mybioeditselection , "wb" ) ;
        a = getc ( in ) ; 
        while ( !feof ( in ) ) {
            if ( a == '-' ) { a = getc ( in ) ; continue ; }
            putc ( a , out ) ;
            if ( a == '>' ) {
                while ( a != 10 && !feof ( in ) ) 
                    putc ( ( a = getc ( in ) ) , out ) ; }
            a = getc ( in ) ; }
        fclose ( in ) ;
        fclose ( out ) ; }
    else 
        myp ( "File \"%s\" has been edited with BioEdit, and will be used \"as is\"" , afilname ( mybioeditselection ) ) ; 
    SetFocus ( hwnd ) ; // come back!
    newln () ;         
    eofscreen () ;
    inva ; 
    return ; 
}

void handle_matrix_creation ( void )
{
    int a , b , c ;
    int errorfree = 1 ; 
    char * extisat , * exe_ptr ;
    char * qextisat ; 
    char * cp , * xp ;
    char didin , didout ; 
    FILE * tstfil ;
    time_t datwas , datis ; 
    if ( !dooneseqpersp && proj_numblocks > 1 ) {
        a = MessageBox ( hwnd , "With multiple TNT blocks, saving multiple\nsequences per species is strongly discouraged\n\nContinue anyway?" , "Warning" , MB_YESNO ) ;
        if ( a == IDNO ) return ; }        
    for ( a = 0 ; a < proj_numblocks ; ++ a ) {
         totfiles_to_do  = 0 ;
         for ( c = 0 ; c < MAXFILES_PER_BLOCK ; ++ c ) {
              if ( blocklist[a].files[c][0] == '\0' ) break ;
              totfiles_to_do ++ ; }
         if ( !totfiles_to_do ) {
             sprintf ( junkstr , "Some blocks (e.g. block %i) have no input files defined\nPlease delete those empty blocks, or specify input files\n" , a+1 ) ;
             MessageBox ( hwnd , junkstr , "ERROR" , MB_OK ) ;
             return ; }}
    if ( have_filename_conflict () ) return ; 
    if ( !show_all_dependencies ( 0 , 1 , 1 ) ) {
        MessageBox ( hwnd , "Cannot create matrix --some files missing\nSelect \"Matrix/ShowDependendencies\" to see missing files" , "ERROR" , MB_ICONERROR ) ;
        return ; }
    filedepends = aligndepends = 0 ; 
    a = DialogBox ( hInst , "CreateMatrixDB" , hwnd , (DLGPROC) CreateMatrixFunc ) ;
    // aqui hay que pasar de nuevo por el calculo de dependencias, si extract/align es autodepend!
    if ( !a ) return ;

    newln () ; 
    myp ( "CREATING MATRIX FOR TNT:\n\nEXTRACTION:" ) ;

    if ( aligndepends && !filedepends )
         show_all_dependencies ( 0 , 0 , 1 ) ;

    /*** EXTRACTION FROM GEN-BANK FILES **********/
    for ( a = totfiles_to_do = 0 ; a < proj_numblocks && errorfree ; ++ a ) {
       newln () ; 
       myp ( "  Block %i (\"%s\")" , a+1 , blocklist[a].blockname ) ;
       for ( c = 0 ; c < MAXFILES_PER_BLOCK ; ++ c ) {
           if ( blocklist[a].files[c][0] == '\0' ) break ;
           if ( listoffiles [ totfiles_to_do ] < 1 ) {
               newln () ;
               if ( !listoffiles [ totfiles_to_do ] ) 
                    myp ( "     file %i (\"%s\")\n       Up-to-Date" , c+1 , afilname ( blocklist[a].files[c] ) ) ;
               else myp ( "     file %i (\"%s\")\n       Not Selected; skipping" , c+1 , afilname ( blocklist[a].files[c] ) ) ;
               ++ totfiles_to_do ;
               continue ; }
           newln () ; 
           myp ( "     file %i (\"%s\")... " , c+1 , afilname ( blocklist[a].files[c] ) ) ; 
           if ( blocklist[a].sourcetype == FASTA ) {
               strcpy ( fn , project_name ) ;
               sprintf ( extisat = find_extension ( fn ) , "_bk%i_file%i.tmp" , a+1 , c+1 ) ; 
               myp ( "copying onto temporary file \"%s\"" , afilname ( fn ) ) ; newln () ;
               totfiles_to_do ++ ; 
               if ( !copy_between_files ( fn , blocklist[a].files[c] ) ) { errorfree = 0 ; break ; }
               continue ; }
           if ( blocklist[a].sourcetype == ALIGNED ) continue ;
           if ( blocklist[a].sourcetype == TNT ) {
               if ( !extract_tnt_file ( a ) ) errorfree = 0 ; 
               continue ; }
           extract_a_file ( a , c ) ;
           totfiles_to_do ++ ; }}
    undild () ;    //  Kill progress report...
    newln () ; 
    if ( !errorfree ) return ; 

    /****  FUSION OF GEN-BANK FILES TO ELIMINATE DUPS. AND FILTER TAXONOMY *****/
    if ( !dooneseqpersp )
        myp ( "\n\nFILTERING SEQUENCES (duplicate taxa will be retained!):\n" ) ;
    else 
       myp ( "\n\nFILTERING DUPLICATES:\n" ) ;
    for ( a = 0 ; a < proj_numblocks && errorfree ; ++ a ) {
        if ( blocklist[a].sourcetype >= ALIGNED ) continue ; 
        if ( dooneseqpersp ) {
            if ( listoftemps[a] < 1 ) {
               if ( !aligndepends )
                  myp ( "    Block %i (\"%s\") not selected for filtering duplicate taxa\n" , a+1 , blocklist[a].blockname ) ;
               else 
                  myp ( "    Block %i (\"%s\") needs no filter for duplicate taxa (up-to-date)\n" , a+1 , blocklist[a].blockname ) ;
               continue ; }
            myp ( "    Filtering sequences in block %i (\"%s\") for duplicate taxa" , a+1 , blocklist[a].blockname ) ; }
        else {
            if ( listoftemps[a] < 1 ) {
               if ( !aligndepends )
                  myp ( "    Block %i (\"%s\") not selected for filtering\n" , a+1 , blocklist[a].blockname ) ;
               else 
                  myp ( "    Block %i (\"%s\") needs no filter (up-to-date)\n" , a+1 , blocklist[a].blockname ) ;
               continue ; }
            myp ( "    Filtering sequences in block %i (\"%s\")" , a+1 , blocklist[a].blockname ) ; }
        if ( !( c = effect_fas2fas ( a ) ) ) errorfree = 0 ;
        if ( c == 2 ) listofblocks[ a ] = -2 ; 
        newln () ; }

    undild () ;
    if ( !errorfree ) return ; 

    /****  ALIGNMENT **************/
    myp ( "\n\nALIGNMENT:\n" ) ; 
    strcpy ( fn , project_name ) ;
    strcpy ( quotedfn , "\"" ) ; 
    strcpy ( quotedfn + 1 , project_name ) ;
    qextisat = find_extension ( quotedfn ) ;
    extisat = find_extension ( fn ) ;
    if ( aligner_is == MAFFT ) exe_ptr = mafftexe ; 
    if ( aligner_is == MUSCLE ) {
        exe_ptr = muscleexe ;
        strcpy ( parmlist , muscleparams ) ; }
    if ( aligner_is == USER ) {
        exe_ptr = alignerexe ;
        strcpy ( parmlist , userparams ) ; }
    current_block = 1 ;
    blocks_to_align = 0 ;
    for ( a = 0 ; a < proj_numblocks ; ++ a ) 
        if ( listofblocks[a] > 0 ) ++ blocks_to_align ; 
    for ( a = 0 ; a < proj_numblocks && errorfree ; ++ a ) {
        if ( listofblocks[a] < 1 ) {
           if ( blocklist[a].sourcetype > FASTA ) myp ( "    Block %i (\"%s\") is up-to-date\n" , a+1 , blocklist[a].blockname ) ;
           else 
           if ( listofblocks[a] == -2 )
              myp ( "    Block %i (\"%s\") produced fewer than 4 sequences with filtering criteria in effect\n" , a+1 , blocklist[a].blockname ) ;
           else 
           if ( !aligndepends )
              myp ( "    Block %i (\"%s\") not selected for alignment\n" , a+1 , blocklist[a].blockname ) ;
           else 
              myp ( "     Block %i (\"%s\") needs no alignment (up-to-date)\n" , a+1 , blocklist[a].blockname ) ;
           continue ; }
        if ( blocklist[a].sourcetype == TNT ) continue ; 
        if ( blocklist[a].sourcetype == ALIGNED ) {
               strcpy ( fn , project_name ) ;
               sprintf ( extisat = find_extension ( fn ) , "_bk%i_%s.aln" , a+1 , blocklist[a].blockname ) ;
               myp ( "    Block %i (\"%s\"): copying onto temporary file \"%s\"" , a+1 , blocklist[a].blockname , afilname ( fn ) ) ; newln () ;
               if ( !copy_between_files ( fn , blocklist[a].files[0] ) ) { errorfree = 0 ; break ; }
               ++ current_block ; 
               continue ; }
        if ( num_distinct_sequences ( a ) < 4 ) {
            myp ( "    Skipping block %i (\"%s\"): too few sequences\n" , a+1 , blocklist[a].blockname ) ;
            listofblocks[a] = -2 ;
            continue ; }
        myp ( "    Aligning block %i (\"%s\")\n" , a+1 , blocklist[a].blockname ) ;
        if ( ( aligner_is == MAFFT && ( overrideblockspecs || !blocklist[a].aligner ) ) || ( blocklist[a].aligner == MAFFT && !overrideblockspecs ) ) {
           curaligner = MAFFT ; 
           if ( !mafftexe[0] ) {
               MessageBox ( hwnd , "Location of MAFFT executable has not been specified" , "ERROR" , MB_OK ) ;
               errorfree = 0 ;
               break ; }
           strcpy ( parmlist , mafftparams ) ;
           if ( use_user_params && ( overrideblockspecs || !blocklist[a].useralignparms ) ) strcpy ( parmlist , userparams ) ;
           else if ( blocklist[a].useralignparms ) strcpy ( parmlist , blocklist[a].alignparms ) ; 
           sprintf ( qextisat , "_bk%i_%s.fas\"" , a+1 , blocklist[a].blockname ) ;
           sprintf ( ( parmlist + strlen ( parmlist ) ) , "%s > " , quotedfn ) ; 
           sprintf ( qextisat , "_bk%i_%s.aln\"" , a+1 , blocklist[a].blockname ) ; 
           sprintf ( extisat , "_bk%i_%s.aln" , a+1 , blocklist[a].blockname ) ; 
           sprintf ( ( parmlist + strlen ( parmlist ) ) , "%s" , quotedfn ) ;
           datwas = getfilemodtime ( fn ) ; }
        if ( ( aligner_is == MUSCLE && ( overrideblockspecs || !blocklist[a].aligner ) ) || ( blocklist[a].aligner == MUSCLE && !overrideblockspecs ) ) {
           curaligner = MUSCLE ; 
           if ( !muscleexe[0] ) {
               MessageBox ( hwnd , "Location of MUSCLE executable has not been specified" , "ERROR" , MB_OK ) ;
               errorfree = 0 ;
               break ; }
           strcpy ( parmlist , muscleparams ) ;
           if ( use_user_params && ( overrideblockspecs || !blocklist[a].useralignparms ) ) strcpy ( parmlist , userparams ) ;
           else if ( blocklist[a].useralignparms ) strcpy ( parmlist , blocklist[a].alignparms ) ; 
           sprintf ( qextisat , "_bk%i_%s.fas\"" , a+1 , blocklist[a].blockname ) ;
           sprintf ( ( parmlist + strlen ( parmlist ) ) , " -in %s " , quotedfn ) ; 
           sprintf ( qextisat , "_bk%i_%s.aln\"" , a+1 , blocklist[a].blockname ) ; 
           sprintf ( extisat , "_bk%i_%s.aln" , a+1 , blocklist[a].blockname ) ; 
           sprintf ( ( parmlist + strlen ( parmlist ) ) , "-out %s" , quotedfn ) ;
           datwas = getfilemodtime ( fn ) ; }
        if ( ( aligner_is == USER && ( overrideblockspecs || !blocklist[a].aligner ) ) || ( blocklist[a].aligner == USER && !overrideblockspecs ) ) {
            curaligner = USER ; 
           if ( !alignerexe[0] ) {
               MessageBox ( hwnd , "Location of Aligner executable has not been specified" , "ERROR" , MB_OK ) ;
               errorfree = 0 ;
               break ; }
           strcpy ( parmlist , userparams ) ;
           if ( ( !overrideblockspecs ) ) {
               if ( blocklist[a].useralignparms ) strcpy ( parmlist , blocklist[a].alignparms ) ;
               else {
                  MessageBox ( hwnd , "Aligners other than MAFFT/MUSCLE require specification of parameters!" , "ERROR" , MB_OK ) ;
                  errorfree = 0 ;
                  break ; }}
           strcpy ( junkstr , parmlist ) ;
           c = 0 ;
           xp = parmlist ;
           didin = didout = 0 ; 
           while ( junkstr[ c ] ) {
               if ( junkstr [ c ] == '$' ) {
                   cp = junkstr + c + 1 ;
                   b = cp [ 6 ] ; 
                   cp [ 6 ] = '\0' ;
                   if ( !strcmp ( cp , "infile" ) ) {
                       sprintf ( qextisat , "_bk%i_%s.fas\"" , a+1 , blocklist[a].blockname ) ;
                       sprintf ( xp , "%s" , quotedfn ) ;
                       xp = parmlist + strlen ( parmlist ) ; 
                       cp [ 6 ] = b ;
                       c += 7 ;
                       didin = 1 ; 
                       continue ; }
                    else {
                       cp [ 6 ] = b ;
                       b = cp [ 7 ] ;
                       cp [ 7 ] = '\0' ;
                       if ( !strcmp ( cp , "outfile" ) ) {
                           sprintf ( qextisat , "_bk%i_%s.aln\"" , a+1 , blocklist[a].blockname ) ;
                           sprintf ( xp , "%s" , quotedfn ) ;
                           xp = parmlist + strlen ( parmlist ) ; 
                           cp [ 7 ] = b ;
                           c += 8 ;
                           didout = 1 ; 
                           continue ; }
                       else cp [ 7 ] = b ; }}
               * xp ++ = junkstr [ c ++ ] ; }
            * xp = '\0' ;
            if ( !didin || !didout ) {
                  MessageBox ( hwnd , "For using an aligner other than MAFFT/MUSCLE,\nyou have to specify both $infile and $outfile" , "ERROR" , MB_OK ) ;
                  errorfree = 0 ;
                  break ; }
           datwas = getfilemodtime ( fn ) ; }
        trublocknum = a ; 
        my_spawn ( exe_ptr , parmlist , NULL ) ;
        /*** Make sure file was created ********/
        datis = getfilemodtime ( fn ) ;
        if ( datis == -1 ) {
            myp ( "    OOPS! ...alignment program failed to create \"%s\"...\n" , fn ) ; 
            sprintf ( junkstr , "Some error occured.\n\nAlignment program did not create \"%s\"" , fn ) ;
            MessageBox ( hwnd , junkstr , "ERROR" , MB_OK ) ;
            errorfree = 0 ;
            break ; }
        if ( datis == datwas ) {
            myp ( "    OOPS! ...alignment program failed to modify \"%s\"...\n" , fn ) ; 
            sprintf ( junkstr , "Some error seems to have occured.\n\nFile \"%s\" was not modified by the aligner!" , fn ) ;
            MessageBox ( hwnd , junkstr , "ERROR" , MB_OK ) ;
            errorfree = 0 ;
            break ; }
        if ( filesizeis == 0 ) {
            myp ( "    OOPS! ...alignment program created an empty file (\"%s\")...\n" , fn ) ; 
            sprintf ( junkstr , "Some error occured.\n\nAlignment program created an empty file (\"%s\")" , fn ) ;
            MessageBox ( hwnd , junkstr , "ERROR" , MB_OK ) ;
            errorfree = 0 ;
            sprintf ( extisat , "_bk%i_%s.fas\"" , a+1 , blocklist[a].blockname ) ;
            utime ( fn , NULL ) ; 
            break ; }
        current_block ++ ; }

    /*** CREATE TNT MATRIX ************/
    if ( errorfree && !skip_matrix_creation ) 
       if ( !create_tnt_matrix ( autoruntnt ) ) errorfree = 0 ; 

    if ( autoruntnt && errorfree && !skip_matrix_creation ) {
        strcpy ( fn , "\"" ) ; 
        strcpy ( fn + 1 , project_name ) ;
        extisat = find_extension ( fn ) ;
        sprintf ( extisat , ".tnt\"" ) ; 
        my_spawn ( tntexe , fn , NULL ) ; }

    SetFocus ( hwnd ) ;   // come back to program 
    newln () ;
    eofscreen () ;
    inva ; 
}

signed int isfile_of_type ( char * full , char * ext )
{
    char * cp = full + strlen ( full ) ;
    char *a , *b ; 
    while ( cp > full && * cp != '.' ) -- cp ;
    if ( * cp != '.' ) return 0 ;
    a = ++ cp ;
    b = ext ; 
    while ( tolower ( * a ) == tolower ( * b ) && * a ) { ++ a ; ++ b ; }
    if ( * a == * b ) return 1 ;
    return 0 ;
}

signed int filetypesin ( Blocktyp * blk )
{ 
    int i , num = 0 ;
    int x = 0 ; 
    for ( i = 0 ; i < MAXFILES_PER_BLOCK ; ++ i ) {
        if ( blk -> files [ i ] [ 0 ]  == '\0' ) break ;
        ++ num ; 
        if ( isfile_of_type ( blk->files[i] , "fas" ) ) x |= FASTA ;
        else if ( isfile_of_type ( blk->files[i] , "gb" ) ) x |= GBANK ;
        else if ( isfile_of_type ( blk->files[i] , "aln" ) ) x |= ALIGNED ;
        else if ( isfile_of_type ( blk->files[i] , "tnt" ) ) x |= TNT ;
        else {
            sprintf ( junkstr , "Unknown file type: %s" , afilname ( blk -> files[i] ) ) ; 
            MessageBox ( hwnd , junkstr , "ERROR" , MB_OK ) ;
            return -1 ; }}
    if ( ( x == ALIGNED || x == TNT ) && num > 1 ) {
            sprintf ( junkstr , "Each block can include at most ONE aligned file" ) ; 
            MessageBox ( hwnd , junkstr , "ERROR" , MB_OK ) ;
            return -1 ; }
    return x ;
}

int check_source_types ( Blocktyp * blk )
{
    signed int x ;
    int i ;
    signed int u , v ; 
    for ( i = 0 ; i < numblocks ; ++ i , ++ blk ) {
        x = filetypesin ( blk ) ;
        if ( x < 0 ) return 0 ;
        for ( v = 0 , u = 1 ; u <= TNT ; u <<= 1 ) if ( ( x & u ) ) ++ v ;
        if ( v > 1 ) {
            sprintf ( junkstr , "Cannot mix file types!\nBlock %i has files of type\n" , i+1 ) ;
            if ( ( x & GBANK ) ) strcat ( junkstr , " GenBank" ) ; 
            if ( ( x & FASTA ) ) strcat ( junkstr , " FASTA" ) ; 
            if ( ( x & ALIGNED ) ) strcat ( junkstr , " ALN" ) ;
            if ( ( x & TNT ) ) strcat ( junkstr , " TNT" ) ;
            MessageBox ( hwnd , junkstr , "ERROR" , MB_OK ) ;
            return 0 ; }
        blk -> sourcetype = x ; }
    return 1 ;         
}

int some_source_is ( char * what )
{
    int i , k ;
    char * a , * b ; 
    for ( i = 0 ; i < proj_numblocks ; ++ i ) {
       for ( k = 0 ; k < MAXFILES_PER_BLOCK ; ++ k ) {
           if ( blocklist[i].files[k][0] == '\0' ) break ;
           a = blocklist[i].files[k] ;
           b = what ;
           while ( tolower ( * a ) == tolower ( * b ) && * a ) { ++ a ; ++ b ; }
           if ( * a == * b && ! * a ) {
               sprintf ( junkstr , "Source file \"%s\" (block %i):\n\n       this is not a valid name for an input file\n       (it would be overwritten as part of the pipeline)\n       Rename it, or choose another file" , afilname ( blocklist[i].files[k] ) , i+1 ) ; 
               MessageBox ( hwnd , junkstr , "ERROR" , MB_OK ) ; 
               return 1 ; }}}
    return 0 ; 
}            

int have_filename_conflict ( void )
{
    int i , j , k ;
    char * extat ; 
    strcpy ( fn , project_name ) ;
    extat = find_extension ( fn ) ; 
    for ( i = 0 ; i < proj_numblocks ; ++ i ) {
        for ( j = 0 ; j < MAXFILES_PER_BLOCK ; ++ j ) {
            sprintf ( extat , "_bk%i_file%i.tmp" , i+1 , j+1 ) ;
            if ( some_source_is ( fn ) ) return 1 ; }
        sprintf ( extat , "_bk%i_%s.fas" , i+1 , blocklist[i].blockname ) ;
        if ( some_source_is ( fn ) ) return 1 ; 
        sprintf ( extat , "_bk%i_%s.aln" , i+1 , blocklist[i].blockname ) ;
        if ( some_source_is ( fn ) ) return 1 ; }
    return 0 ;
}

int copy_between_files ( char * ton , char * fromn )
{
    char a , b ;
    FILE * t , * f ;
    t = fopen ( ton , "wb" ) ;
    if ( t == NULL ) {
        sprintf ( junkstr , "Can't open output file \"%s\"" , afilname ( ton ) ) ; 
        MessageBox ( hwnd , junkstr , "ERROR" , MB_OK ) ; 
        return 0 ; }
    f = fopen ( fromn , "rb" ) ;
    if ( f == NULL ) {
        fclose ( t ) ;
        sprintf ( junkstr , "Can't open input file \"%s\"" , afilname ( fromn ) ) ; 
        MessageBox ( hwnd , junkstr , "ERROR" , MB_OK ) ; 
        return 0 ; }
    a = getc ( f ) ; 
    while ( ! feof ( f ) ) {
        putc ( a , t ) ;
        a = getc ( f ) ; }
    fclose ( t ) ;
    fclose ( f ) ;
}    
        
void touch_all_fastas ( void )
{
    int i ;
    char * extat ;
    strcpy ( fn , project_name ) ;
    extat = find_extension ( fn ) ;
    for ( i = 0 ; i < proj_numblocks ; ++ i ) {
        sprintf ( extat , "_bk%i_%s.fas" , i+1 , blocklist[i].blockname ) ;
        utime ( fn , NULL ) ; }
    return ;
}

int is_a_single_block ( int a )  // is TNT file for block "a" composed of a single block?
{
    FILE * fp ;
    char tmstring[100] ;
    int mync , mynt , b ;
    int blocksinfile = 0 ; 
    char * bp = blocklist[a].blockheader ; 
    fp = fopen ( blocklist[a].files[0] , "rb" ) ;
    tmstring[0] = '\0' ; 
    while ( !feof ( fp ) && strcmp ( tmstring , "xread" ) ) fscanf ( fp , " %s" , tmstring ) ;
    b = getc ( fp ) ; while ( isspace ( b ) ) b = getc ( fp ) ;
    if ( b == '\'' ) while ( ( b = getc ( fp ) ) != '\'' ) ;
    else ungetc ( b , fp ) ; 
    fscanf ( fp , " %i %i" , &mync , &mynt ) ;
    strcpy ( junkstr , bp ) ; 
    while ( b != ';' && !feof ( fp ) ) {
        b = getc ( fp ) ;
        if ( b == '&' ) {
            if ( blocksinfile ++ ) {
                sprintf ( junkstr , "File \"%s\" contains multiple blocks.\nOnly one block per TNT file is allowed." , afilname ( blocklist[a].files[0] ) ) ;
                MessageBox ( hwnd , junkstr , "ERROR" , MB_OK ) ;
                fclose ( fp ) ; 
                return 0 ; }
            while ( isspace ( b = getc ( fp ) ) ) ;
            if ( b == '[' ) {
                b = getc ( fp ) ; 
                while ( b != ']' && bp - blocklist[a].blockheader < MAX_BLOCK_HEADER - 1 ) {
                    * bp ++ = b ; 
                    b = getc ( fp ) ; }
                * bp = '\0' ; }}}
    if ( strcmp ( junkstr , blocklist[a].blockheader ) ) have_something_to_save = 1 ;   
    return 1 ;     
}

int translate_tnt_file ( int a )  // is TNT file for block "a" composed of a single block?
{
    FILE * fp , * of ;
    char tmstring[100] ;
    int mync , mynt , b , c , n ; 
    int blocksinfile = 0 ; 
    char * bp = blocklist[a].blockheader ;
    char * extisat ; 
    fp = fopen ( blocklist[a].files[0] , "rb" ) ;
    strcpy ( fn , project_name ) ;
    extisat = find_extension ( fn ) ;
    sprintf ( extisat , "_bk%i_%s.aln" , a+1 , blocklist[a].blockname ) ;
    of = fopen ( fn , "wb" ) ;
    if ( of == NULL ) {
        sprintf ( junkstr , "Cannot open temporary file \"%s\" for output" , afilname ( fn ) ) ;
        MessageBox ( hwnd , junkstr , "ERROR" , MB_OK ) ;
        fclose ( fp ) ; 
        return 0 ; }
    myp ( "copying TNT file onto temporary file \"%s\"" , afilname ( fn ) ) ; newln () ;
    tmstring[0] = '\0' ; 
    while ( !feof ( fp ) && strcmp ( tmstring , "xread" ) ) fscanf ( fp , " %s" , tmstring ) ;
    b = getc ( fp ) ; while ( isspace ( b ) ) b = getc ( fp ) ;
    if ( b == '\'' ) while ( ( b = getc ( fp ) ) != '\'' ) ;
    else ungetc ( b , fp ) ; 
    fscanf ( fp , " %i %i" , &mync , &mynt ) ;
    strcpy ( junkstr , bp ) ;
    b = ' ' ; 
    while ( isspace ( b ) && !feof ( fp ) ) {
        b = getc ( fp ) ;
        if ( b == '&' ) {
            while ( isspace ( b = getc ( fp ) ) ) ;
            if ( b == '[' ) {
                b = getc ( fp ) ; 
                while ( b != ']' && bp - blocklist[a].blockheader < MAX_BLOCK_HEADER - 1 ) {
                    * bp ++ = b ; 
                    b = getc ( fp ) ; }
                * bp = '\0' ; }
        else if ( !isspace ( b ) ) ungetc ( b , fp ) ; }}
    for ( n = 0 ; n < mynt ; ++ n ) {
        while ( isspace ( b ) ) b = getc ( fp ) ;
        fprintf ( of , "\n>" ) ;
        while ( !isspace(b) ) {
            putc ( b , of ) ;
            b = getc ( fp ) ; }
        fprintf ( of , "\n" ) ;
        while ( isspace ( b ) ) b = getc ( fp ) ; 
        c = 0 ;
        while ( c < mync ) {
            if ( b == ';' ) {
                sprintf ( junkstr , "Unexpected semicolon in TNT matrix \"%s\"" , afilname ( blocklist[a].files[0] ) ) ;
                MessageBox ( hwnd , junkstr , "ERROR" , MB_OK ) ;
                fclose ( of ) ;
                fclose ( fp ) ; 
                return 0 ; }
            if ( b == '[' ) {
                ++ c ; 
                putc ( b , of ) ;
                while ( b != ']' ) {
                    b = getc ( fp ) ;
                    if ( !isspace ( b ) ) putc ( b , of ) ; }}
            else 
            if ( !isspace ( b ) ) { putc ( b , of ) ; ++ c ; }
            b = getc ( fp ) ; }}
    fprintf ( of , "\n" ) ;
    fclose ( of ) ;
    fclose ( fp ) ;
    return 1 ; 
}

int extract_tnt_file ( int a )
{
    if ( !is_a_single_block ( a ) ) return 0 ;
    if ( !translate_tnt_file ( a ) ) return 0 ; 
    return 1 ;     
}    

int num_distinct_sequences ( int a )
{
    char * extat ;
    FILE * fp ;
    int i , numdistinct = 0 ; 
    strcpy ( fn , project_name ) ;
    sprintf ( find_extension ( fn ) , "_bk%i_%s.fas" , a+1 , blocklist[a].blockname ) ;
    fp = fopen ( fn , "rb" ) ;
    if ( fp == NULL ) return 0 ;
    i = 10 ; 
    while ( !feof ( fp ) ) {
        if ( i == 10 )
           if ( getc ( fp ) == '>' ) ++ numdistinct ;
        i = getc ( fp ) ; }
    fclose ( fp ) ; 
    return numdistinct ; 
}

BOOL CALLBACK GetBlockAlignmentFunc (HWND hdwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HWND tmpwnd ;
    HWND hWho ;
    static int maligner , mparms ; 
    switch(message) {
        case WM_INITDIALOG:
            if ( !tmpblocklist[blockalignfor].aligner ) maligner = aligner_is ;
            else maligner = tmpblocklist[blockalignfor].aligner ;
            if ( maligner == MAFFT ) BUTT_CHECK ( 201 ) ; 
            if ( maligner == MUSCLE ) BUTT_CHECK ( 202 ) ; 
            mparms = tmpblocklist[blockalignfor].useralignparms ;
            if ( maligner == USER ) {
                BUTT_CHECK ( 203 ) ;
                sprintf ( junkstr , "user-defined (indicate $infile and $outfile):" ) ;
                mparms = 1 ; 
                Button_Enable ( GetDlgItem ( hdwnd , 205 ) , FALSE ) ; 
                SetDlgItemText ( hdwnd , 206 , junkstr ) ; }
            if ( !mparms ) {
                BUTT_CHECK ( 205 ) ;
                Button_Enable ( GetDlgItem ( hdwnd , 207 ) , FALSE ) ; 
                sprintf ( junkstr , "(type parameters here)" ) ; }
            else {
                BUTT_CHECK ( 206 ) ;
                strcpy ( junkstr , tmpblocklist[blockalignfor].alignparms ) ; }
            SetDlgItemText ( hdwnd , 207 , junkstr ) ; 
            FOCUS_ON ( IDOK ) ;
            break;
        case WM_COMMAND:
             switch ( LOWORD ( wParam ) ) {
                 case 201:
                     maligner = MAFFT ;
                     sprintf ( junkstr , "user-defined:" ) ;
                     Button_Enable ( GetDlgItem ( hdwnd , 205 ) , TRUE ) ; 
                     SetDlgItemText ( hdwnd , 206 , junkstr ) ; 
                     break ; 
                 case 202:
                     maligner = MUSCLE ;
                     sprintf ( junkstr , "user-defined:" ) ;
                     Button_Enable ( GetDlgItem ( hdwnd , 205 ) , TRUE ) ; 
                     SetDlgItemText ( hdwnd , 206 , junkstr ) ; 
                     break ; 
                 case 203:
                     maligner = USER ;
                     sprintf ( junkstr , "user-defined (indicate $infile and $outfile):" ) ;
                     SetDlgItemText ( hdwnd , 206 , junkstr ) ;
                     mparms = 1 ;
                     BUTT_UNCHEK ( 205 ) ; 
                     BUTT_CHECK ( 206 ) ; 
                     Button_Enable ( GetDlgItem ( hdwnd , 205 ) , FALSE ) ; 
                     Button_Enable ( GetDlgItem ( hdwnd , 207 ) , TRUE ) ; 
                     strcpy ( junkstr , tmpblocklist[blockalignfor].alignparms ) ;
                     SetDlgItemText ( hdwnd , 207 , junkstr ) ;
                     break ;
                 case 205:
                    mparms = 0 ;
                    Button_Enable ( GetDlgItem ( hdwnd , 207 ) , FALSE ) ; 
                    sprintf ( junkstr , "(type parameters here)" ) ; 
                    SetDlgItemText ( hdwnd , 207 , junkstr ) ;
                    break ;
                case 206:
                    mparms = 1 ;
                    Button_Enable ( GetDlgItem ( hdwnd , 207 ) , TRUE ) ;
                    strcpy ( junkstr , tmpblocklist[blockalignfor].alignparms ) ;
                    SetDlgItemText ( hdwnd , 207 , junkstr ) ;
                    break ; 
                 case IDOK:
                    tmpblocklist[blockalignfor].aligner = maligner ;
                    tmpblocklist[blockalignfor].useralignparms = mparms ;
                    if ( mparms ) {
                        GetDlgItemText(hdwnd, ( 207 ) , junkstr , _MAX_PATH ) ;
                        junkstr[ _MAX_PATH - 1 ] = '\0' ;
                        strcpy ( tmpblocklist[blockalignfor].alignparms , junkstr ) ; }
                    EndDialog ( hdwnd , 1 ) ;
                    break ;
                 case IDCANCEL:
                    EndDialog ( hdwnd , 0 ) ;
                    break ;
                 default: break ; }
        default: break ; }
   return 0 ; 
}

