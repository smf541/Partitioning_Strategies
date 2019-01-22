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

#define VTYPE extern
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ctype.h>
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
#include <time.h>

extern int dooneseqpersp ; 
extern HWND hwnd ;
extern int show_rejects , proj_numblocks , max_acceptable_diffs ;
int verb = 0 ;
int use_translation = 0 ;
int dargc , laquiero ;
int showedskipped = 0 ;
char ** dargv ;
static FILE * inpf , * curinput , * outputfile ;
int laschar, order [100] ;
#define CATEGORIES    6
#define TRUE    1
#define FALSE   0
#define MAXUSERACC 1000000
#define MAXSTRINGSIZE 5000


Blocktyp * current_block ;
int current_file, space ;

int look_in_notes = 0 ;
int running_for_first_time = 1 ;
int genometype = 0 ;
int use_string_matching = 0 ;
double string_similarity ;

int stringis ( char * , char * ) ;  //  are both strings the same, case aside ???
int rd ( void ) ;                    //  return next char. from input file; save it in laschar
void myouter ( int , char * txt ) ;   // if first arg. is TRUE, then output txt and exit
void gonln ( void ) ;              //  read all the way to the ENTER
void gotostring ( char * , int ) ;  // find string of first arg, with margin of sec arg characters
void rdto ( int , char * ) ;        //  saves to string every byte it finds before first arg
void stornexline ( void ) ;         // puts in stringsp everything to the ENTER
void * mymalloc ( unsigned long int ) ;  // allocs mem or exits
int gotofirstspace (char * stringsp ) ;
int gotolaspace (char * stringsp ) ;
void parse_intgen ( void ) ;
int istrunc ( char * a ) ;
void effect_complementation (void   ) ;
double doneedwunsch ( char * , char * ) ;

#define MAXNUMCHUNKS 500
typedef struct { int from ; int to ; int iscomp} Chunktyp ;
Chunktyp chunk[MAXNUMCHUNKS] ;
Chunktyp chunk2[MAXNUMCHUNKS] ;
Chunktyp chunksbk[MAXNUMCHUNKS] ;


int numchunks = 0 , numchunks2, whichgen ;
#define MAXSEQLENGTH 500000
char bytestring[MAXSEQLENGTH] ;
char complement_string[MAXSEQLENGTH] ;

unsigned long int accepted = 0 , rejected = 0 ;
char stringsp [ MAXSTRINGSIZE ], headerline [ MAXSTRINGSIZE ],headerline1 [ MAXSTRINGSIZE ], headerline2 [ MAXSTRINGSIZE ] ;
char geneline [ MAXSTRINGSIZE ], stringbk [ MAXSTRINGSIZE ] ;
int genenumber = 0, genenumber2 = 0  ;
#define MAX_USER_DEFINITIONS MAXPRODS_PER_BLOCK
char accnumber [ 100 ] ;


char taxname [ 100 ] ;
char taxonomy [ 1000 ] ;
char ** useraccname ;
int useraccnumber = 0 ;
int loopall = 0 ;

char * genename[MAX_USER_DEFINITIONS ] , * genename2 [MAX_USER_DEFINITIONS] ;
unsigned long int dafsize , bytesread = 0 ;

void makeafullstring (void)
{
 char * cpp, * dde ;
 int i, a, largo=0  ;
 char largesp [ MAXSTRINGSIZE ],sp[1] ;
 sp[0]=32;
 cpp = stringsp  ;
 for(i = 0; i < 50 ; i++ )
    if (stringsp[i] == '"'){
      cpp = stringsp ;
      cpp = cpp + i + 1 ;
      break ; }
  for (i = 0 ; i < 100 ; i++ )
  {
   dde = strrchr (cpp, '"') ; // returns null if second string not contained within first
    if (!i)
      strcpy ( largesp , cpp ) ;
    else {
     a = strlen ( largesp ) ;
     largesp [a] = sp[0] ;
     largesp [a+1] = '\0' ;
     largo  = largo + a  ;
     if (largo >  MAXSTRINGSIZE ) {
       sprintf ( junkstr , "The string is too long (max. %i). Do you want to parse the rest of the file?" , MAXSTRINGSIZE ) ;
       a = MessageBox ( hwnd , junkstr , "Please confirm..." , MB_YESNO | MB_ICONWARNING ) ;
     if ( a == IDYES ){
     save_to_not() ; gotostring ( "ACCESSION" , 0 ) ; }
     }else
       strcat (largesp,cpp) ;
     }
    if (dde != NULL){
      strcpy ( stringsp,largesp) ;
      a = strlen (stringsp) ;
      stringsp [a-1] = '\0' ;
      return ; }
    else
      {
      stornexline () ;

      cpp = stringsp ; }

  }
  strcpy (stringsp, largesp) ;
}

void makeafullstring2 (void)
{
 char * cpp, * dde ;
 int i, a, largo=0   ;
 char largesp [ MAXSTRINGSIZE ],sp[1] ;
 sp[0]=32;
 cpp = stringsp  ;
 strcpy (largesp,stringsp) ;
 while ( 1 ) // returns null if second string not contained within first 
  {
   stornexline () ;
   if (!strrchr (cpp, '/') )
    {
     a = strlen (largesp) ;
     largesp [a+1] = '\0' ;
     largo  = largo + a  ;
     if (largo >  MAXSTRINGSIZE )
      {
       sprintf ( junkstr , "The string is too long (max. %i). Do you want to parse the rest of the file?" , MAXSTRINGSIZE ) ;
        a = MessageBox ( hwnd , junkstr , "Please confirm..." , MB_YESNO | MB_ICONWARNING ) ;
        if ( a == IDYES )
        {
         save_to_not() ; gotostring ( "ACCESSION" , 0 ) ;
        }
      }
     else
     {
       strcat (largesp,cpp) ;
     }
    }
   else{
     strcpy (stringbk,stringsp) ;
     strcpy (stringsp,largesp) ;
     break ;}
  }
}

int find_isinstring ( char * a , char * b )
{
    char * ap = a , x ;
    int lena = strlen ( a ) ;
    int lenb = strlen ( b ) ;
    int at = 0 ;
    if ( !strcmp ( b , "?" ) ) return 1 ;
    while ( * ap ) {
        if ( * ap == * b ) {
           x = ap [ lenb ] ;  // so that it returns where match ends 
           ap [ lenb ] = '\0' ;  
             if ( stringis ( ap , b ) ) {
               ap [ lenb ] = x ;
               return ( at + lenb ) ; }
           ap [ lenb ] = x ; }
        ++ at ;
        ++ ap ;
        if ( at + lenb > lena ) break ; }
    return 0 ;
}


void keep_source (void)
{
    char * cp = stringsp ;
    numchunks = 0 ;
    while (!isdigit ( * cp ) )
       ++ cp ;
    while ( isspace ( * cp ) ) ++ cp ;
    chunk[numchunks].from = 0 ;
    while ( * cp ++ != '.' && * cp ) ;
    ++ cp ;
    chunk[numchunks].to = atoi ( cp ) ;
    while ( isdigit ( * cp ) ) ++ cp ;
}


void * mymalloc ( unsigned long int size )
{
    void * pt = malloc ( size ) ;
    myouter ( pt == NULL , "Not enough memory!" ) ;
    return pt ;
}

/******  Progress reporting .... **********/
HINSTANCE hInst ;
HWND hProgWnd ;
int have_progress_wnd = 0 ;

BOOL CALLBACK ProgressFunc(HWND hdwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static char didis[7] ;
    char a ;
    switch(message) {
      case WM_INITDIALOG :
            return(WM_SETFOCUS);
      case WM_COMMAND :
              switch(LOWORD(wParam)){
                default : break ;
              }
      break ; }
    return 0;
}

HWND progress_wnd ( void )
{
    HWND bck ;
    bck = CreateDialog( hInst , "ProgressDB", hwnd , (DLGPROC) ProgressFunc ) ;
    have_progress_wnd = 1 ;
    return ( bck ) ;
}

void dildit ( void )
{
    static int prv = 0 ;
    double fract ;
    unsigned long int ifract ;
    ++ bytesread ;
    fract = ( ( double ) bytesread / dafsize ) * 100 ;
    ifract = fract ;
    if ( ifract == prv ) return ;
    prv = ifract ;
    if ( !have_progress_wnd )
         hProgWnd = progress_wnd () ;
    sprintf ( junkstr , "%s  %i%%" , afilname ( current_block -> files[current_file] ) , ifract ) ;
    SetDlgItemText( hProgWnd , 200 , junkstr );
    UpdateWindow ( GetDlgItem ( hProgWnd , 200 ) ) ;
    sprintf ( junkstr , "Extracting (block %i of %i)" , (current_block - blocklist)+1 , proj_numblocks ) ;
    SetWindowText ( hProgWnd , junkstr ) ;
    UpdateWindow ( hProgWnd ) ;
}

void undild ( void )
{
    if ( !have_progress_wnd ) return ; 
    have_progress_wnd = 0 ;
    DestroyWindow ( hProgWnd ) ;
}

int eoffound = 0 ;

int rd ( void )
{
    int i ;
    if ( eoffound ) return ( laschar = 10 ) ;
    if ( feof ( curinput ) ) {
        eoffound = 1 ;
        fclose ( curinput ) ;
        newln () ;
        myp ( "       Accepted %i accessions, rejected %i" , accepted , rejected ) ;
        return ( laschar = 10 ) ; }
    dildit () ;
    i = getc ( curinput ) ;
    if ( i == 13 )
        if ( ( i = getc ( curinput ) ) != 10  ) ungetc ( i , curinput ) ;
        else ++ bytesread ;
    return ( laschar = i ) ;
}

int openit ( void )
{
   struct stat buf ;
   char * extisat ;
   curinput = inpf = fopen ( current_block -> files[ current_file ] , "rb" ) ;
   fstat ( fileno ( inpf ) , &buf ) ;
   eoffound = 0 ;
   dafsize = buf.st_size ;
   strcpy ( fn , project_name ) ;
   * ( extisat = find_extension ( fn ) ) = '\0' ;
   sprintf ( extisat , "_bk%i_file%i.tmp" , (current_block-blocklist)+1 , current_file+1 ) ;
   outputfile = fopen ( fn , "wb" ) ;
   if ( outputfile == NULL ) {
       sprintf ( junkstr , "OOPS!!\nCannot open \"%s\" for output!" , fn ) ;
       MessageBox ( hwnd , junkstr , "ERROR" , MB_ICONERROR ) ;
       return 0 ; }
   return 1 ;
}

void myouter ( int doit , char * txt )
{
    if ( !doit ) return ;
    fprintf ( stderr , "%s\n" , txt ) ;
    getc ( stdin ) ;
    exit ( 0 ) ;
}

void gonln ( void )
{
    while ( laschar != 10 ) rd () ;
}

void gotostring ( char * string , int shift )
{
    char * cp ;
    int i ;
    int somenonwhite ;
    strcpy ( stringsp , " " ) ;
    while ( strcmp ( stringsp , string ) && !eoffound ) {
         gonln () ;
         somenonwhite = 0 ;
         if ( shift > 0 ) {
            for ( i = 0 ; i < shift && !somenonwhite ; ++ i )
                if ( rd () != 32 ) somenonwhite = 1 ;
            if ( somenonwhite ) continue ; }
         else {
            while ( isspace ( rd () ) && !eoffound ) ;
            ungetc ( laschar , inpf ) ;
            -- bytesread ; }
         * ( cp = stringsp ) = rd () ;
         if ( * cp ++ != * string ) continue ;
         while ( !isspace( * cp = rd () ) && !eoffound ) {
             cp ++ ;
             if ( cp - stringsp > 98 ) break ; }
         * cp = '\0' ; }
}

void rdaccnumber ( void )
{
    char * cp = accnumber ;
    while ( isspace ( rd () ) && !eoffound ) ;
    while ( !isspace ( laschar ) ) {
         * cp ++ = laschar ;
         rd () ; }
    * cp = '\0' ;
}

void rdto ( int donde , char * stor )
{
    char * cp ;
    int a = 0 ;
    cp = stor ;
    while ( isspace( laschar ) && !eoffound ) rd () ;
    while ( laschar != donde ) {
        if ( a != '_' || laschar != '_' )
           * cp ++ = laschar ;
        a = laschar ;
        rd () ;
        if (eoffound) break ;
        if ( laschar == 32 ) laschar = '_' ;
        if ( laschar == 10 && donde != 10 ) laschar = '_' ; }
    * cp = '\0' ;
}

void rdtaxo ( int donde , char * stor )
{
    char * cp ;
    int a = 0 ;
    cp = stor ;
    while ( isspace( laschar ) && !eoffound ) rd () ;
    while ( laschar != donde ) {
      if (eoffound) break ;
      if (laschar != 32 && laschar != 10 )
        *cp ++ = laschar ;
         rd () ;}
    * cp = '\0' ;
}

void stornexline ( void )
{
    char * cp = stringsp ;
    space= 0  ;
    rd () ;
    while ( isspace ( laschar ) && !eoffound ){ rd () ;  space ++ ; } ;
    while ( laschar != 10 && laschar != 13 ) {
        if (cp - stringsp < ( MAXSTRINGSIZE - 1 ) )
            * cp ++ = laschar ;
        rd () ; }
    * cp = '\0' ;
}


void ** loray ( int wid , int hei , int size )
{
  int a ;
  void ** pp ;
  ( void ** ) pp = ( void ** ) malloc ( wid * sizeof ( void * ) ) ;
  myouter ( ( pp == NULL ) , "Not enough RAM") ;
  for ( a = 0 ; a < wid ; ++ a )
    if ( ( ( void * ) pp [ a ] = ( void * ) malloc ( hei * size ) ) == NULL )
        myouter ( 1 , "Not enough RAM") ;
  return pp ;
}

int stringis ( char * a , char * b )
{
    if ( use_string_matching ) {
        if ( doneedwunsch ( a , b ) >= string_similarity ) return 1 ;
        return 0 ; }
    while ( * a && * b )
        if ( tolower ( * a ++ ) != tolower ( * b ++ ) ) return 0 ;
    return 1 ;
}

int rdliteral ( void )
{
    char * cp = stringsp ;
    rd () ;
    while ( isspace ( laschar ) && !eoffound ) rd () ;
    if ( laschar == '\"' ) {
       * cp ++ = laschar ;
       while ( rd () != '\"' && !eoffound ) * cp ++ = laschar ;
       * cp ++ = laschar ;
       * cp = '\0' ;
       return 1 ; }
    else {
        * cp = laschar ;
        while ( !isspace ( laschar ) && !eoffound ) * ++ cp = rd () ;
        * cp = '\0' ;
        return 0 ; }
}

int istrunc ( char * a )   // is a a truncation of stringsp ??
{
    char * b = stringsp ;
    while ( 1 ) {
        if ( ! * a ) return 1 ;
        if ( * a ++ != * b ++ ) return 0 ; }
}

int isamatch ( char * a , char * b )
{
    while ( 1 ) {
        if ( ! * a ) return 1 ;
        if ( * a ++ != * b ++ ) return 0 ; }
}

void process ( void )
{
    char * cp ;
    int i ,lucatit, mygenometype , showed_headerline, a, b, espacio , x,has_voucher  ;
    int showacc_only ;
    int found_translation, make ;

    while ( !eoffound ) { 
        gotostring ( "ACCESSION" , 0 ) ;   if ( eoffound ) break ;
        rdaccnumber () ;
        showacc_only = 0 ;
        has_voucher = 0 ;
        if ( useraccnumber ) {
             for ( i = 0 ; i < useraccnumber && !showacc_only ; ++ i )
                if ( !strcmp ( accnumber , useraccname[i] ) ) showacc_only = 1 ; }
        laquiero = found_translation = 0 ;
        gotostring ( "ORGANISM" , -1 ) ;  if ( eoffound ) break ;
        rdto ( 10 , taxname ) ; if ( eoffound ) break ; 
        rdtaxo ( '.' , taxonomy ) ; if ( eoffound ) break ; 

        gotostring ( "FEATURES" , 0 ) ;  if ( eoffound ) break ;
        mygenometype = NUCLEAR ;


        while ( 1 ) {
          stornexline ( ) ; // puts in stringsp everything to the ENTER
          if ( istrunc ("source") && ( ! genenumber || current_block->complete ) ) // is () a truncation of stringsp ? && si quierotoda la secuencia
           keep_source ( ) ;
          if ( istrunc ( "ORIGIN" )  ) { laquiero = -1 ; break ; }
          if ( istrunc ( "/specimen_voucher=" ) )
            has_voucher = 1 ;
          if ( istrunc ( "/organelle=" ) ) {
              if ( istrunc ( "/organelle=\"mitoc" ) )
                   mygenometype = MITOCH ;
              else if ( istrunc ( "/organelle=\"chlorop" ) )
                   mygenometype = CHLORO ;
              else if ( istrunc ( "/organelle=\"plastid" ) )
                   mygenometype = CHLORO ; }
          if ( istrunc ( "tRNA" ) ) break   ; if ( istrunc ( "rRNA" ) ) break  ; if ( istrunc ( "misc_feature" ) ) break   ;  if ( istrunc ( "intron" ) ) break ; if ( istrunc ( "exon" ) ) break ;
          if ( istrunc ( "intron" ) ) break ; if ( istrunc ( "misc_RNA" ) ) break ; if ( istrunc ( "exon" ) ) break    ;  if ( istrunc ( "gene" ) ) break ;
          if ( istrunc ( "CDS" ) ) {
          if ( use_translation ) laquiero = FALSE ;  break ; } 
          }   /***** End initial parsing... ******/

           if ( mygenometype != current_block->genome )
               laquiero = -1 ;
           if ( current_block -> genome == ANYGENOME ) laquiero = FALSE ;
           if ( (current_block->twogen[0]) && (laquiero == FALSE) )
              laquiero = getintgen ()  ;

           if ( current_block -> voucher && !has_voucher)
              laquiero = - 1 ;

           while ( laquiero == FALSE || ( use_translation && !found_translation ) )
             {
                if ( istrunc ( "ORIGIN" ) && genenumber   ) break ;
                 make = 0 ; lucatit = 0 ;
                if (!genenumber){lucatit = 1 ; laquiero =  1 ;  }
                for ( i = 0 , x = 1 ; i < CATEGORIES ; i++ , x <<= 1 )
                 {
                  if ( ( current_block->category & x ) )
                        if ( istrunc ( cattyp [ i ] )  )
                   {
                   lucatit = 1 ;
                   break ; }
                 }
                 if ( lucatit )
                   {
                     showed_headerline = 0 ;
                     makeafullstring2 ( ) ;
                     makelower ( stringsp ) ;
                     strcpy ( headerline , stringsp ) ;  
                     a = strlen (stringsp ) ;
                     headerline[ a+1 ] = '\0' ;
                     strcpy (  stringsp , stringbk ) ;
                     make = 1 ;
                     espacio = space ;
                     if ( istrunc ( "ORIGIN" ) ) break ;
                     if (! genenumber ) break ;
                     while (1)
                          {
                          if (laquiero && istrunc ( "/translation=" ) && use_translation ) {
                              parse_translation () ;
                              found_translation = 1 ;
                              break ;  }
                          if ( istrunc ( "/gene=" ) && genenumber )
                            {
                             makeafullstring ( ) ;
                             makelower ( stringsp ) ;
                             for ( i = 0 ; i < genenumber && !laquiero ; ++ i )
                               if ( find_isinstring ( stringsp , genename[i] ) )
                                 {
                                     laquiero = 1 ;
                                     if (!use_translation) break ;
                                 }
                            }
                          else
                           if ( istrunc ( "/product=" ) && genenumber )
                             {
                              makeafullstring ( ) ;
                              makelower ( stringsp ) ;
                              for ( i = 0 ; i < genenumber && !laquiero ; ++ i )
                                if ( find_isinstring ( stringsp , genename[i] ) )
                                 {
                                  laquiero = 1 ;
                                  if (!use_translation) break ; }
                             }
                             else
                              if  ( istrunc ( "/note=" ) && genenumber )
                               {
                                makeafullstring ( ) ;
                                makelower ( stringsp ) ;
                                for ( i = 0 ; i < genenumber && !laquiero ; ++ i )
                                 if ( find_isinstring ( stringsp , genename[i] ) )
                                  {
                                   laquiero = 1 ;
                                   if (!use_translation) break ; }}

                          if (laquiero == 1 && !use_translation ) break ;
                          stornexline () ;
                          espacio = space ;
                          if ( istrunc ( "ORIGIN" ) ) break ;
                          if (space == 5) break ;
                         }
                   } /// if (lucatit)
                   else
                     stornexline () ;
                }
       if ( laquiero == TRUE && ( !use_translation || found_translation ) )
        {
         if ( use_translation )
            output_translation () ;
         else {
           if ( current_block->twogen[0] )
               parse_intgen () ;
           else
             if (current_block->complete || !genenumber)
               parse_source () ;
             else
              parsit() ;
        if ( eoffound ) break ; }
        }
        else
         save_to_not () ;
      }     // ---  while ( !feof ( inpf ) )
}

void save_to_not ( void )
{
       if ( !show_rejects ) { ++ rejected ; return ; }
       if ( showedskipped ) myp ( ", " ) ;
       else myp ( "\n       Skipped" ) ;
       if ( rejected && ( rejected % 5 ) == 0 ) myp ( "\n              " ) ;
       ++ rejected ;
       showedskipped = 1 ;
       myp ( "%15s" , accnumber ) ;
}

void makelower ( char * txt )
{
    char * cp = txt ;
    while ( * cp ) {
         * cp = tolower ( *cp ) ;
         ++ cp ; }
    return ;
}

int wrong_location ;

char * storchunk ( char * cp )
{

     if ( * cp == '<' )
       ++ cp ;
     if ( !isdigit ( * cp ) )
        { wrong_location = 1 ; * cp = '0' ; return cp ; }
     if (!current_block->complete)
       chunk[numchunks].from = atoi ( cp ) ;
     while ( * cp ++ != '.' && * cp ) ;
     if ( * cp != '.' ) { wrong_location = 1 ; * cp = '0' ; return cp ; }
     ++ cp ;
     if ( * cp == '>' ) ++ cp ;
     if ( !isdigit ( * cp ) ) { wrong_location = 1 ; * cp = '0' ; return cp ; }
     if (!current_block->complete)
        chunk[numchunks].to = atoi ( cp ) ;
     while ( isdigit ( * cp ) ) ++ cp ;
      ++ numchunks ;
     return cp ;


}

char *  storchunks_jc ( char * cp )
 {
 int open ;

  while ( * cp != '\0' )
    {
     while ( isspace ( * cp ) ) ++ cp ;
     if ( * cp == 'c' )
      {
         chunk[numchunks].iscomp = 1;
         cp = cp + 11 ;
         if ( * cp == '<' ) ++ cp ;
         if (!current_block->complete)
          chunk[numchunks].from = atoi (cp) ;
         while ( * cp ++ != '.' && * cp ) ;
         if ( * cp != '.' ) { wrong_location = 1 ; * cp = '0' ; return cp ; }
         ++ cp ;
         if ( * cp == '>' ) ++ cp ;
         if ( !isdigit ( * cp ) ) { wrong_location = 1 ; * cp = '0' ; return cp ; }
         if (!current_block->complete)
          chunk[numchunks].to = atoi ( cp ) ;
         while ( isdigit ( * cp ) ) ++ cp ;
         cp ++  ;
         ++ numchunks ;
       }
      else
       {
         if ( * cp == ')')
         return cp ;
         chunk[numchunks].iscomp = 0;
         if ( * cp == '<' ) ++ cp ;
         chunk[numchunks].from = atoi (cp) ;
         while ( * cp ++ != '.' && * cp ) ;
         if ( * cp != '.' ) { wrong_location = 1 ; * cp = '0' ; return cp ; }
         ++ cp ;
         if ( * cp == '>' ) ++ cp ;
         if ( !isdigit ( * cp ) ) { wrong_location = 1 ; * cp = '0' ; return cp ; }
         if (!current_block->complete)
          chunk[numchunks].to = atoi ( cp ) ;
         while ( isdigit ( * cp ) ) ++ cp ;
         ++ numchunks ;
        }
   cp++ ;
    }
     return cp ;
}

int species_read = 0 ;

void parsit ( void )
{
 char * cp = headerline + 15, * ct  ;  // en headerline guarda la info de la location.
 int i , j , a  , atchunk , atpos, shift, jc=0, cj= 0,  dde, length,cambio ;
 char * bytept = bytestring , now, * bytept2 ;
 int complement_it = 0 , isjoin, iscomp  ;

 numchunks =   0 ;
 wrong_location = 0 ;
  while ( isspace ( * cp ) ) ++ cp ;
 isjoin= 0; iscomp= 0 ;

 if (!((strrchr ( cp , ':')) == NULL) ) {   // i.e. if cp contains a colon.  For accessions like U23166
    save_to_not () ;
    return ; }

 if ( * cp == '<' || isdigit ( * cp ) ) {
   storchunk (cp) ;
   chunk[0].iscomp = 0;}
 else
   {
      if ( !((strrchr (cp, 'j'))  == NULL) )
       {  // returns null if second string not within first
        isjoin = 1 ;
            }
      if (!((strrchr (cp, 'c')) == NULL) )
         {
         iscomp = 1 ;
         complement_it = 1 ;
         }
       if ( (iscomp == 0) && (isjoin == 0) ) {
          save_to_not () ; return ; }
       if ( iscomp && !isjoin ){
           cp = cp + 11  ;
           chunk[0].iscomp = 1 ;}

       if ( !iscomp && isjoin )
           cp = cp + 5 ;

       if ( iscomp && isjoin )
         {
         dde = find_isinstring (cp,"t(j") ;
         if (dde) {
           cp = cp + 16 ;
           cj = 1 ;}
          else {
            jc = 1 ;
            cp = cp + 5 ;
            cp = storchunks_jc ( cp ) ;
            }
         }
       while ( * cp != ')' && !wrong_location && !jc ) {
            cp = storchunk ( cp ) ;
            while ( * cp == ',' || isspace ( * cp ) )
            ++ cp ; }
     }
      if (!iscomp && isjoin ){
        for (i = 0 ; i < numchunks ; i++)
          chunk[i].iscomp = 0 ;}



    if (isjoin){
      for (i = 0 ; i < numchunks ; i++){
        chunk2[i].iscomp = chunk[i].iscomp ;
        chunk2[i].from   = chunk[i].from ;
        chunk2[i].to     = chunk[i].to ;}
      cambio= sort_chunks (cp) ;}
    if ( wrong_location ) {
         save_to_not () ;
         return ; }
    gotostring ( "ORIGIN" , 0 ) ;
    rdliteral () ;
    atchunk = atpos = 0  ;
    ++ species_read ;

    while ( 1 ) {
        now = laschar ;
        while ( laschar == 10 || laschar == 13 || laschar == 32 || ( laschar >= '0' && laschar <= '9' ) ) {
            now = laschar ;
            rd () ;
            if ( eoffound ) break ; }
        if ( now == '/' || laschar == '/' )
          return ;
        ++ atpos ;
       if ( chunk[atchunk].from <= atpos  && chunk[atchunk].to >= atpos )
        {
            * bytept ++ = laschar ;
            if ( bytept - bytestring >= MAXSEQLENGTH )
               laschar = atpos ;
            myouter ( ( bytept - bytestring >= MAXSEQLENGTH ) , "OOPS -- sequence is too long!\nChange MAXSEQLENGTH and re-compile" ) ;
        }
        laschar = 32 ;
        if ( atpos == chunk[atchunk].to )
            if ( ++ atchunk == numchunks )
             break ;
    }
   bytept2 = complement_string ;
   length = strlen ( bytestring ) ;
    if ( cj && cambio )
     {
      for (i = 0 ; i < numchunks ; i++) //recorre el chunk2 q es la disp orginal
        for (j = 0 ; j < numchunks ; j++)// recorre el chunk que es la disp en bytestring
         {
          if ( chunk[j].from == chunk2[i].from )
            {
             dde = 0 ;
             for (a = 0 ; a <  j ; a ++ )
              dde = dde + ( chunk[a].to - chunk[a].from + 1 ) ;
              dde = dde  ;
             bytept = &bytestring [dde] ;
             for (a = 0 ; a < (chunk[j].to - chunk[j].from + 1 ) ; a++,bytept ++, bytept2++ )
              * bytept2 = * bytept ;
             break ;
             }

           }
     strcpy (bytestring, complement_string)  ;
     bytestring [length + 1 ] = '\0' ;
     numchunks = 1 ;
     chunk[0].iscomp = 1;
     }
    else
     * bytept = '\0' ;

    ++ accepted ;
    spew_name () ;
    effect_complementation ()  ;
    fprintf ( outputfile , "%s" , bytestring ) ;
    fprintf ( outputfile , "\n" ) ;
    fflush ( outputfile ) ;
}

void parse_intgen ( void )
{
 char * cp,  * ct  ;  
 int i ,a ,b,  atchunk , atpos, shift, jc=0, cj= 0,  dde  ;
 char * bytept = bytestring , now ;
 int complement_it = 0 , isjoin, iscomp, nucha  ;
 numchunks = nucha = 0 ;
 wrong_location = 0 ;

 isjoin = 0 ; iscomp = 0 ;


 for ( a = 1  ; a < 3 ; a++ ){
   whichgen = a ;
   numchunks = 0 ;
   isjoin= 0 ;
   iscomp= 0 ;
   if ( a == 1 )
    cp = headerline1 + 15 ;
   else
     cp = headerline2 + 15 ;
   while ( isspace ( * cp ) ) ++ cp ;
   if (!((strrchr ( cp , ':')) == NULL) ) {   // i.e. if cp contains a colon.  For accessions like U23166
     save_to_not () ;
     return ; }
   if ( * cp == '<' || isdigit ( * cp ) ) {
     storchunk (cp) ;
     chunk[0].iscomp = 0; }
  else
   {
      if ( !((strrchr (cp, 'j'))  == NULL) )
         isjoin = 1 ;
      if (!((strrchr (cp, 'c')) == NULL) )
         { iscomp = 1 ; complement_it = 1 ; }
       if ( (iscomp == 0) && (isjoin == 0) ) {
          save_to_not () ; return ; }
       if ( iscomp && !isjoin ){
           cp = cp + 11  ;
             chunk[0].iscomp = 1 ; }
       if ( !iscomp && isjoin )
           cp = cp + 5 ;
       if ( iscomp && isjoin ){
         dde = find_isinstring (cp,"t(j") ;
         if (dde) {
           cp = cp + 16 ;
           cj = 1 ;}
          else {
            jc = 1 ;
            cp = cp + 5 ;
            cp = storchunks_jc ( cp ) ;}}
        while ( * cp != ')' && !wrong_location && !jc ) {
            cp = storchunk ( cp ) ;
            while ( * cp == ',' || isspace ( * cp ) )
            ++ cp ; }
     }
      if (!iscomp && isjoin ){
        for (i = 0 ; i < numchunks ; i++)
             chunk[i].iscomp = 0 ;
     if (cj)
      chunk[0].iscomp = 2 ;

   
      if (isjoin)
        sort_chunks (cp) ;
      if ( wrong_location ) {
         save_to_not () ;
         return ; }}
     if (a ==1 )
      for (b = 0 ; b < numchunks ; ++b ){
         chunksbk [b].from =   chunk [b].from ;
         chunksbk [b].to   =   chunk [b].to ;
         nucha = numchunks ; }
}
         for (b = 0 ; b < numchunks ; ++b ){
          chunk2 [b].from   = chunk [b].from   ;
          chunk2 [b].to     = chunk [b].to     ;}
         for (b = 0 ; b < nucha ; ++b ){
            chunk [b].from   = chunksbk [b].from   ;
            chunk [b].to     = chunksbk [b].to     ;}


if ( (chunk[0].from < chunk2[0].from ) && (chunk[numchunks-1].to > chunk2[0].from ) )
  { save_to_not () ; return ;}
if ( (chunk2[0].from < chunk[0].from ) && (chunk2[numchunks-1].to > chunk[0].from ) )
  {save_to_not () ;  return ; }
if ( (chunk[0].from < chunk2[0].from ) && (chunk[numchunks-1].to > chunk2[0].to ) )
 {save_to_not () ;  return ; }
if ( (chunk[0].from > chunk2[0].from ) && (chunk[numchunks-1].to < chunk2[0].to ) )
 {save_to_not () ;  return ; }


 if ( chunk[0].from > chunk2[ numchunks-1 ].from ) {
   for (b = 0 ; b < nucha ; ++b ){
     chunk[ b ].from = chunk2[ b ].from ;
     chunk[ b ].to   = chunk2[ b ].to   ;}
   for (b = 0 ; b < numchunks ; ++b ){
     chunk2[ b ].from = chunksbk[ b ].from ;
     chunk2[ b ].to   = chunksbk[ b ].to   ;}
   chunk[0].iscomp =1 ;
   a = nucha ; nucha = numchunks; numchunks = a ;
   }
 else
   chunk[0].iscomp =0 ;


  if (current_block->twogen[1] == 0  )
    chunk [ 0 ].from = ( chunk  [ nucha - 1 ].to + 1 ) ;
  if (current_block->twogen[ 2 ] == 0  )
    chunk [ 0 ].to = ( chunk2 [ 0 ].from - 1 );
   else
    chunk [ 0 ].to = chunk2 [ numchunks - 1 ].to;
  if ( chunk [ 0 ].to <= chunk [ 0 ].from)
   {save_to_not () ;  return ; }



   numchunks = 1 ;
   gotostring ( "ORIGIN" , 0 ) ;
   rdliteral () ;
   atchunk = atpos = 0  ;
   while ( 1 ) {
        now = laschar ;
        while ( laschar == 10 || laschar == 13 || laschar == 32 || ( laschar >= '0' && laschar <= '9' ) ) {
            now = laschar ;
            rd () ;
            if ( eoffound ) break ; }
        if ( now == '/' || laschar == '/' )
          return ;
        ++ atpos ;
       if ( chunk[atchunk].from <= atpos  && chunk[atchunk].to >= atpos )
        {
            * bytept ++ = laschar ;
            if ( bytept - bytestring >= MAXSEQLENGTH )
               laschar = atpos ;
            myouter ( ( bytept - bytestring >= MAXSEQLENGTH ) , "OOPS -- sequence is too long!\nChange MAXSEQLENGTH and re-compile" ) ;
        }
        laschar = 32 ;
        if ( atpos == chunk[atchunk].to )
            if ( ++ atchunk == numchunks )
             break ;
    }
    ++ species_read ;
    * bytept = '\0' ;
    effect_complementation ()  ;
    ++ accepted ;
    spew_name () ;
    fprintf ( outputfile , "%s" , bytestring ) ;
    fprintf ( outputfile , "\n" ) ;
    fflush ( outputfile ) ;
}

void parse_translation ( void )
{
    char * cp , * bytept ;
    cp = stringsp + 14 ;
    bytept = bytestring ;
    while ( * cp != '\"' && * cp ) {
           if ( !isspace ( * cp ) ) * bytept ++ = * cp ++ ;
           if ( * cp == 10 || * cp == 13 || * cp == '\0' ) {
                 stornexline () ;
                 cp = stringsp ;
                 while ( isspace ( * cp ) ) ++ cp ; }}
    * bytept = '\0' ;
}

void spew_name ( void )
{
    char * cp = taxonomy , * here , * begg ;
    begg = here =  cp  ;
    while ( * cp ) {
        if ( * cp == ';' ){
         * here ++ = '_' ;
         cp ++ ; }
         else {
        * here ++ = * cp ++ ; }}
   * here = '\0' ;
   if ( !dooneseqpersp )
       fprintf ( outputfile , ">%s_%s_@%s\n" , taxname , accnumber , begg ) ;
   else 
       fprintf ( outputfile , ">%s____%s_@%s\n" , taxname , accnumber , begg ) ;
}

void output_translation ( void )
{
    ++ accepted ;
    spew_name () ;
    fprintf ( outputfile , "%s" , bytestring ) ;
    fprintf ( outputfile , "\n" ) ;
    fflush ( outputfile ) ;
}

char tmpmask[256] ;
char makeit[9] ;
char antimask[16] ;

void effect_complementation ( void )
{
    int i , j , k , l, * pt, a, start, finish ;
    int bit ;
    int x , y ;
    static int inited = 0 ;
    if ( !inited ) {
        for ( i = 0 ; i < 256 ; ++ i ) tmpmask[i] = 0 ;
        tmpmask [ 'a' ] = tmpmask [ 'A' ] = 1 ; // tmpmask [ '0' ] ;
        tmpmask [ 'g' ] = tmpmask [ 'G' ] = 2 ; // tmpmask [ '1' ] ;
        tmpmask [ 'c' ] = tmpmask [ 'C' ] = 4 ; // tmpmask [ '2' ] ;
        tmpmask [ 't' ] = tmpmask [ 'T' ] = 8 ; // tmpmask [ '3' ] ;
        tmpmask [ 'R' ] = tmpmask [ 'r' ] = tmpmask [ 'a' ] | tmpmask [ 'g' ] ;
        tmpmask [ 'Y' ] = tmpmask [ 'y' ] = tmpmask [ 't' ] | tmpmask [ 'c' ] ;
        tmpmask [ 'W' ] = tmpmask [ 'w' ] = tmpmask [ 'a' ] | tmpmask [ 't' ] ;
        tmpmask [ 'S' ] = tmpmask [ 's' ] = tmpmask [ 'c' ] | tmpmask [ 'g' ] ;
        tmpmask [ 'M' ] = tmpmask [ 'm' ] = tmpmask [ 'a' ] | tmpmask [ 'c' ] ;
        tmpmask [ 'K' ] = tmpmask [ 'k' ] = tmpmask [ 'g' ] | tmpmask [ 't' ] ;
        tmpmask [ 'B' ] = tmpmask [ 'b' ] = tmpmask [ 'c' ] | tmpmask [ 'g' ] | tmpmask [ 't' ] ;
        tmpmask [ 'D' ] = tmpmask [ 'd' ] = tmpmask [ 'a' ] | tmpmask [ 'g' ] | tmpmask [ 't' ] ;
        tmpmask [ 'H' ] = tmpmask [ 'h' ] = tmpmask [ 'a' ] | tmpmask [ 'c' ] | tmpmask [ 't' ] ;
        tmpmask [ 'V' ] = tmpmask [ 'v' ] = tmpmask [ 'a' ] | tmpmask [ 'c' ] | tmpmask [ 'g' ] ;
        tmpmask [ 'N' ] = tmpmask [ 'n' ] = 1 | 2 | 4 | 8 ;
        makeit[tmpmask['a']] = tmpmask['t'] ;
        makeit[tmpmask['c']] = tmpmask['g'] ;
        makeit[tmpmask['g']] = tmpmask['c'] ;
        makeit[tmpmask['t']] = tmpmask['a'] ;
        for ( i = 0 ; i < 256 ; ++ i )
            if ( tmpmask[i] )
                 antimask[ tmpmask[i] ] = i ; 
        inited = 1 ; }

    j = strlen ( bytestring ) ;
    if ( numchunks == 1 )
    {
    if (chunk[0].iscomp == 0 ) return  ;
    for ( i = 0 ; i < j ; ++ i )
       complement_string[i] = bytestring[i] ;
    for ( i = 0 , k = j - 1 ; i < j ; ++ i , -- k ) {
        x = tmpmask [ complement_string [i]] ;
        y = 0 ;
        for ( l = 0 , bit = 1 ; l < 4 ; ++ l , bit <<= 1 )
            if ( ( bit & x ) )
                y |= makeit[ bit ] ;
        bytestring [ k ] = antimask [ y ] ; }
    return ;
    }
    else {
      j = finish = chunk[0].to - chunk[0].from  ;
      start = 0 ;
      for   (a = 0 ; a < numchunks ; a ++){
         if (a != 0) {
         start =  finish ;
         finish =  start + (chunk[a].to - chunk[a].from + 1)  ;
         j = finish ; }
         if (chunk[a].iscomp != 1  )
               continue ;
         for ( i = start ; i < j ; ++ i )
               complement_string[i] = bytestring[i] ;
            for ( i = start , k = j - 1 ; i < j ; ++ i , -- k ) {
                x = tmpmask [ complement_string [i]] ;
                y = 0 ;
                for ( l = 0 , bit = 1 ; l < 4 ; ++ l , bit <<= 1 )
                    if ( ( bit & x ) )
                        y |= makeit[ bit ] ;
                bytestring [ k ] = antimask [ y ] ; }
       }
    }
}

int gotolaspace (char * stringsp )
{
int i ;
    for(i = 0; i < 100 ; i++ )
      if ( stringsp[i] != 32 )
        return (i) ;
}
int gotofirstspace (char * stringsp )
{
    int i ;
    for(i = 0; i < 100 ; i++ )
      if ( stringsp[i] == 32 )
        return (i) ;
}


int getintgen ( void )
{
 int ihvfst = 0, ihvnd = 0, i, a, make,espacio, gotit = 0   ;
 while (1) {
   if ( istrunc ("gene ") )
    {
     makeafullstring2 ( ) ;
     makelower ( stringsp ) ;
     strcpy ( headerline , stringsp ) ;
     a = strlen (stringsp ) ;
     headerline[ a+1 ] = '\0' ;
     strcpy (  stringsp , stringbk ) ;
     makelower ( stringsp ) ;
     make = 1 ;
     espacio = space ;
     if ( istrunc ( "ORIGIN" ) ) break ;
     while ( 1 )
       {
        if (!ihvfst)
          for ( i = 0 ; i < genenumber ; ++ i )
            if ( find_isinstring ( stringsp , genename[i] ) )
             { ihvfst ++ ;
              strcpy (headerline1, headerline);
              gotit ++ ;
              break ;
             }
        if (!ihvnd && !gotit)
          for ( i = 0 ; i < genenumber2 ; ++ i )
            if ( find_isinstring ( stringsp , genename2[i] ) )
            {
             ihvnd ++ ;
             strcpy (headerline2, headerline);
             break ; }
        gotit = 0 ;
       if ( ( ihvfst ) && ( ihvnd ) )
         return (1) ;
       stornexline () ;
       espacio = space ;
       if ( istrunc ( "ORIGIN" ) ) break ;
       if (space == 5) break ;
     }
   }
   else {
    stornexline () ;
    if ( istrunc ( "ORIGIN" ) ) return (-1 )  ;}
 }
}




void setopts ( void ) // aca
{
    int i , c ;
    loopall = 0 ;
    showedskipped = 0 ;
    genenumber = 0 ;
    genenumber2 = 0 ;
    if ( current_block -> stringsim < 100  ) { // para ponerle un valor de stringmatch para el NW
         use_string_matching = 0 ;
         string_similarity = ( double ) current_block -> stringsim / 100 ; }
    else {
       use_string_matching = 1 ;
       string_similarity = 1 ; }
    if ( ! current_block -> isdna ) use_translation = 1 ; // define que tome los valores de AA
    else use_translation = 0 ;
    for ( c = 0 ; c < MAXPRODS_PER_BLOCK ; ++ c ) {
        if ( current_block -> prod[c][0] == '\0' ) break ;
        if (( current_block -> prod[c][0] == '<') && (current_block -> prod[c][1] == '>' ))
          { c++ ; current_block ->twogen[0] = 1 ; break ;}
        else {
           genename [ genenumber ] = current_block -> prod[c] ;
           makelower ( genename [ genenumber ++ ] ) ; }}
    if (current_block -> twogen[0] == 1)    {
     for ( i = c ; i < MAXPRODS_PER_BLOCK ; ++ i ) {
        if ( current_block -> prod[i][0] == '\0' ) break ;
         genename2 [ genenumber2 ] = current_block -> prod[i] ;
         makelower ( genename2 [ genenumber2 ++ ] ) ; }}


    if ( current_block -> genome == MITOCH ) genometype = MITOCH ;
    if ( current_block -> genome == NUCLEAR ) genometype = NUCLEAR ;
    if ( current_block -> genome == PLASTID ) genometype = CHLORO ;
    if ( current_block -> genome == CHLORO ) genometype = CHLORO ;
    look_in_notes = current_block -> alsonote ;



}

int extract_a_file ( int block , int file )
{
    max_acceptable_diffs = 3 ;
    current_block = blocklist + block ;
    current_file = file ;
    accepted = rejected = bytesread = 0 ;
    setopts () ;
    if ( !openit () ) return 0 ;
    bytesread = 0 ;
    dildit () ;
    process () ;
    fclose ( outputfile ) ;
    return 1 ;
}

void parse_source (void)
{
    char * cp = headerline + 15, * ct  ;  // en headerline guarda la info de la location.
    int i , atchunk , atpos ;
    char * bytept = bytestring , now ;
    numchunks = 0 ;
    gotostring ( "ORIGIN" , 0 ) ;
    rdliteral () ;
    atchunk = atpos = 0 ;
    ++ species_read ;
    while ( 1 ) {
        now = laschar ;
        while ( laschar == 10 || laschar == 13 || laschar == 32 || ( laschar >= '0' && laschar <= '9' ) ) {
            now = laschar ;
            rd () ;
            if ( eoffound ) break ; }
        if ( now == '/' || laschar == '/' )
          return ;
        ++ atpos ;
        if ( chunk[atchunk].from <= atpos && chunk[atchunk].to >= atpos ) {
            * bytept ++ = laschar ;
         if  ( bytept - bytestring >= MAXSEQLENGTH )
             atpos = laschar ;
            myouter ( ( bytept - bytestring >= MAXSEQLENGTH ) , "OOPS -- sequence is too long!\nChange MAXSEQLENGTH and re-compile" ) ; }
        laschar = 32 ;
        if ( atpos == chunk[atchunk].to )
               break ;
     }
    * bytept = '\0' ;
    ++ accepted ;
    spew_name () ;
    fprintf ( outputfile , "%s" , bytestring ) ;
    fprintf ( outputfile , "\n" ) ;
    fflush ( outputfile ) ;
}

int sort_chunks (char *cp )
{
 int bien = 0 , j = 0, tmp, change = 0  ;
  while ( 1 )
   {
      if (chunk[j].to < chunk[j+1].to)
      {
        bien ++ ; if ( bien == (numchunks - 1) ) break ;
        if (j != ( numchunks - 2))
          j++ ;
        else{
         j = 0; bien = 0 ; }
      }
      else
         {
          change = 1 ;
          tmp = chunk[j].to  ;  chunk[j].to = chunk[j+1].to ;  chunk[j+1].to = tmp ;
          tmp = chunk[j].from ; chunk[j].from = chunk[j+1].from ; chunk[j+1].from = tmp ;
          tmp = chunk[j].iscomp ; chunk[j].iscomp = chunk[j+1].iscomp ; chunk[j+1].iscomp = tmp ;
          bien = 0 ;
          if (j != (numchunks - 2))
            j++ ;
          else
           j = 0;
         }
   }
return (change) ;
}



