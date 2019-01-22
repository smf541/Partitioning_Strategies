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
#include<math.h>
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
int this_is_a_TNT_file = 0 ; 
extern int proj_numblocks , query_taxon_identity , save_accession_numbers , save_taxonomy ;
extern HWND hwnd ;
Blocktyp * blk ;
static int mycurfile , headed_synonyms ;

unsigned long int thiseqlen ;

int blockstarts[ MAXNUMBLOCKS ] , blockends[ MAXNUMBLOCKS ] ;

static int errored = 0 ;
static FILE * inpf , * curinput , * opsf = NULL , * mergefile ;
int numinputs = 0 ;
int itsahybrid , unacceptable ;
int max_acceptable_diffs = 5 ;
double threshold = 0.95 ;
unsigned long int dafsize , bytes_read ;
static int madeNWmem = 0 , mademem = 0 ;

int num_unaccepts = 0 , num_forceaccepts = 0 , number_of_passes , num_diff_seqs ;
#define MAX_NUM_UNACCEPTS 100
#define MAX_NUM_FORCEACCEPTS 100
char unaccept[ MAX_NUM_UNACCEPTS ][50] ;
char forceaccept[ MAX_NUM_FORCEACCEPTS ][50] ;
char * curtimestring ( void ) ;

#define MAXSEQS 1000000
char ** names ;
unsigned long int * seqlen ;
signed long int * islike ;
unsigned long int * chooseq ;

unsigned long int * present_in_block ; 

unsigned long int seqsread = 0 ;

typedef struct { int block ; int file ; } Filekeytyp ;
Filekeytyp filekey[ MAXFILES_PER_BLOCK + MAXNUMBLOCKS ] ; 

#define MAXNAMELEN 500
#define MAXSEQLENGTH 300000
char tmpname[MAXNAMELEN] , tmpbuff[MAXNAMELEN];
char tmpseq[MAXSEQLENGTH] ;

typedef struct
      { int up , diag , lef ;
        int min ; } Stringcomptyp ;
Stringcomptyp ** cellcost ;
int gapcost = 1 , gapextcost = 1 ;
int suscost = 1 ;

int sizit ( void )
{
    struct stat buf ;
    fstat ( fileno ( curinput ) , &buf ) ;
    dafsize = buf.st_size ;
    bytes_read = 0 ;
}

/******  Progress reporting .... **********/
extern HINSTANCE hInst ;
extern HWND hProgWnd ;
extern int have_progress_wnd = 0 ;

BOOL CALLBACK MyProgressFunc(HWND hdwnd, UINT message, WPARAM wParam, LPARAM lParam)
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

HWND my_progress_wnd ( void )
{
    HWND bck ;
    bck = CreateDialog( hInst , "ProgressDB", hwnd , (DLGPROC) MyProgressFunc ) ;
    have_progress_wnd = 1 ;
    return ( bck ) ;
}

void endildit ( void )
{
    static int prv = 0 ;
    double fract ;
    unsigned long int ifract ;
    fract = ( ( double ) bytes_read / dafsize ) * 100 ;
    ifract = fract ;
    if ( ifract == prv ) return ;
    prv = ifract ;
    if ( !have_progress_wnd )
         hProgWnd = my_progress_wnd () ;
    sprintf ( junkstr , "checking  %i%%" , ifract ) ;
    SetDlgItemText( hProgWnd , 200 , junkstr );
    sprintf ( junkstr , "Creating matrix (block %i of %i)" , (blk - blocklist)+1 , proj_numblocks ) ;
    SetWindowText ( hProgWnd , junkstr ) ;
    UpdateWindow ( GetDlgItem ( hProgWnd , 200 ) ) ;
    UpdateWindow ( hProgWnd ) ;
}

void mydildit ( void )
{
    static int prv = 0 ;
    double fract ;
    unsigned long int ifract ;
    fract = ( ( double ) bytes_read / dafsize ) * 100 ;
    ifract = fract ;
    if ( ifract == prv ) return ;
    prv = ifract ;
    if ( !have_progress_wnd )
         hProgWnd = my_progress_wnd () ;
    sprintf ( junkstr , "%s  %i%%" , afilname ( blk  -> files[mycurfile] ) , ifract ) ;
    SetDlgItemText( hProgWnd , 200 , junkstr );
    sprintf ( junkstr , "Merging (block %i of %i, pass %i)" , (blk - blocklist)+1 , proj_numblocks , number_of_passes ) ;
    SetWindowText ( hProgWnd , junkstr ) ;
    UpdateWindow ( GetDlgItem ( hProgWnd , 200 ) ) ;
    UpdateWindow ( hProgWnd ) ;
}

int cget ( FILE * where )
{
    ++ bytes_read ;
    return getc ( where ) ;
}

void outer ( char * txt , int i )
{
    sprintf ( junkstr , txt , i ) ;
    MessageBox ( hwnd , junkstr , "FATAL ERROR" , MB_OK ) ;
    errored = 1 ;
    return ;
}

void * myalloc ( unsigned long int siz )
{
    void * p = malloc ( siz ) ;
    if ( p == NULL ) {
        sprintf ( junkstr , "Ran out of memory, could not allocate RAM for sequence nr. %i" , seqsread ) ;
        MessageBox ( hwnd , junkstr , "ERROR" , MB_OK ) ;
        errored = 1 ;
        return NULL ; }
    return p ;
}

void gotoarrow ( void )
{
    int i = ' ' ;
    while ( isspace ( i ) && !feof ( curinput ) ) i = cget ( curinput ) ;
    if ( feof ( curinput ) ) return ;
    if ( i != '>' ) { outer ( "Expected \">\" for taxon %i" , seqsread ) ; return ; }
}

void stuffit ( int strinlen , int trunamlen )
{
    int i , c , isdiff , somediff ;
    char * ap  , * bp ;
    if ( strinlen < 6 ) strinlen = 6 ;
    islike [ seqsread ] = seqsread ;
    isdiff = 1 ;
    for ( i = 0 ; i < seqsread && isdiff ; ++ i ) {
        if ( islike[i] != i ) continue ;
        ap = tmpname ;
        bp = names[i] ;
        somediff = 0 ;
        for ( c = 0 ; c < trunamlen && !somediff && * ap ; ++ c )
             if ( tolower ( * ap ++ ) != tolower ( * bp ++ ) ) somediff = 1 ;
        if ( !somediff ) { isdiff = 0 ; islike [ seqsread ] = i ; }}
    if ( isdiff )
       islike [ seqsread ] = seqsread ;
}

void skiptaxname ( void )
{
    int i = 'x' ;
    char * cp = tmpname - 1 ;
    while ( !isspace ( i ) )
        * ++ cp = i = cget ( curinput ) ;
    * cp = '\0' ;
    return ;
}

void rdtaxname ( void )
{
    char * cp = tmpname - 1 , * cpp ;
    int i = 'a' , found_cuadruple , x ;
    int numunderscores = 0 , secunderscore ;
    int strinlen , trunamlen , truncated = 0 ;
    char * underscisat , prv = ' ' ;
    while ( !isspace ( i ) ) {
        i = cget ( curinput ) ;
        if ( cp - tmpname > MAXNAMELEN - 1 ) {
            * cp = '\0' ;
            if ( !truncated ) {
                truncated = 1 ;
                sprintf ( junkstr , "Taxon name \"%s\" is too long; it will be truncated" , tmpname ) ;
                if ( query_taxon_identity )
                     MessageBox ( hwnd , junkstr , "ERROR" , MB_OK ) ; }}
        else * ++ cp = i ;
        if ( i == '_' ) {
            if ( !numunderscores ) underscisat = cp ;
            ++ numunderscores ; }
        else
          if ( numunderscores < 4 ) numunderscores = 0 ; }
    if ( numunderscores < 4 ) 
        underscisat = tmpname + strlen ( tmpname ) ; 
    else ++ underscisat ;
    trunamlen = underscisat - tmpname ;
    numunderscores = 0 ;
    * cp ++ = '\0' ;
    ridofstuff ( trunamlen ) ;
    if ( unacceptable || itsahybrid ) return ;
    strinlen = cp - tmpname ;
    stuffit ( cp - tmpname , trunamlen + 3 ) ;
    cp = tmpname ;
    return ;
}

void rdsequence ( void )
{
    char * cp = tmpseq ;
    int i , len ;
    while ( isspace ( i = cget ( curinput ) ) ) ;
    while ( i != '>' && !feof ( curinput ) ) {
        if ( ( cp - tmpseq ) >= MAXSEQLENGTH ) {
            sprintf ( junkstr , "Sequence for species:\n%s\nis too long (max. %i)\n" ,
                                  names[seqsread] , MAXSEQLENGTH ) ;
            MessageBox ( hwnd , junkstr , "ERROR" , MB_OK ) ;
            errored = 1 ;
            return ; }
        if ( !isspace ( i ) )
             * cp ++ = i ;
        i = cget ( curinput ) ; }
    * cp = '\0' ;
    if ( i == '>' ) ungetc ( i , curinput ) ;
    len = cp - tmpseq ;
    fprintf ( mergefile , "%s\n" , tmpseq ) ;
    return ;
}

void skipsequence ( int storlen )
{
    char * cp = tmpseq ;
    int i , len ;
    while ( isspace ( i = cget ( curinput ) ) ) ;
    thiseqlen = 0 ; 
    while ( i != '>' && !feof ( curinput ) ) {
        if ( ( cp - tmpseq ) >= MAXSEQLENGTH ) {
            sprintf ( junkstr , "Sequence for species:\n%s\nis too long (max. %i)\n" ,
                                  tmpname , MAXSEQLENGTH ) ;
            MessageBox ( hwnd , junkstr , "ERROR" , MB_OK ) ;
            errored = 1 ;
            return ; }
        if ( this_is_a_TNT_file && i == '[' ) {
            * cp ++ = i ;
            ++ thiseqlen ; 
            while ( i != ']' ) 
                * cp ++ = i = cget ( curinput ) ; }
        else     
        if ( !isspace ( i ) ) {
             * cp ++ = i ;
             ++ thiseqlen ; }
        i = cget ( curinput ) ; }
    * cp = '\0' ;
    if ( i == '>' ) ungetc ( i , curinput ) ;
    if ( storlen ) seqlen [ seqsread ] = thiseqlen ;
    return ;
}

int istruncof ( char * deque , char * que )
{
    int i , j ;
    while ( * que ) {
          i = tolower ( * deque ++ ) ;
          j = tolower ( * que ++ ) ;
          if ( i != j && j != '\0' ) return 0 ; }
    return 1 ;
}

int ispartof ( char * deque , char * que )
{
    int i , j ;
    char * cp , * bp ;
    while ( * deque ) {
       if ( * que == * deque ) {
          cp = que - 1 ;
          bp = deque - 1 ;
          while ( * ++ cp ) {
             i = tolower ( * cp ) ;
             j = tolower ( * ++ bp ) ;
             if ( i != j ) break ; }
          if ( * cp == '\0' ) return 1 ; }
       ++ deque ; }
    return 0 ;
}

void ridofstuff ( int trunamlen )
{
    char * a = tmpbuff ;
    char * cp = tmpname ;
    int i , foundit ;
    int havesomepars = 0 ;
    itsahybrid = unacceptable = 0 ;
    if ( islower ( * cp ) ) { unacceptable = 1 ; return ; }
    /*  Get rid of single quotes  */
    if ( * cp == '\'' ) {
        while ( * cp ) {
            * a ++ = * ++ cp ;
            if ( * cp == '\'' ) a -- ; }
        strcpy ( tmpname , tmpbuff ) ; }
    if (!want_environ)  
      if (istruncof ( cp , "Environmental" ))
          unacceptable = 1 ;
    if (!want_uncult)
     if (istruncof ( cp , "Uncultured" ))
          unacceptable = 1 ;      
    /*  Get rid of parentheses, or _x_ */
    a = tmpbuff ;
    cp = tmpname ;
    havesomepars = 0 ;
    foundit = 0 ;
    if ( num_forceaccepts ) {
       for ( i = 0 ; i < num_forceaccepts && !foundit ; ++ i )
           if ( ispartof ( cp , forceaccept[i] ) ) foundit = 1 ;
       if ( !foundit ) unacceptable = 1 ; }
    while ( * cp && !unacceptable ) {
        for ( i = 0 ; i < num_unaccepts && !unacceptable ; ++ i )
            if ( istruncof ( cp , unaccept[i] ) )
                unacceptable = 1 ;
        if ( * cp == '(' ) {
            havesomepars = 1 ;
            while ( * cp && * cp != ')' ) ++ cp ;
            ++ cp ;
            if ( a > tmpbuff && * ( a - 1 ) == '_' && * cp == '_' ) -- a ; }
        if ( * cp == '_' )
         { 
            if ( cp [3] == '.' && cp - tmpname < trunamlen - 4 ) 
            {
             if (!want_confers) {
                i = cp[5] ;
                cp[5] = '\0' ;
                if ( !strcmp ( cp , "_cf._" ) ) unacceptable = 1 ;
                cp[5] = i ; }
             if ( !want_sp && !unacceptable ) {
                i = cp[5] ;
                cp[5] = '\0' ;
                if ( !strcmp ( cp , "_sp._" ) ) unacceptable = 1 ;
                cp[5] = i ; } 
            }                
            else {
                
              if (cp [4] == '.' && !want_affinis && cp - tmpname < trunamlen - 5 ){
                i = cp[6] ;
                cp[6] = '\0' ;
                if ( !strcmp ( cp , "_aff._" ) ) unacceptable = 1 ;
                cp[6] = i ; }
              else
                {
                if (!want_hybrids)
                 if ( find_isinstring ( cp , "_x_" ) )
                  unacceptable = 1 ; }}
           
         }         
        * a ++ = * cp ++ ; }
    * a = '\0' ;
    if ( unacceptable ) return ;
    if ( havesomepars ) strcpy ( tmpname , tmpbuff ) ;
    return ;
}

void parse_strings_to_trash ( void )
{
    char * cp , * gp ;
    int i ;
    num_unaccepts = 0 ;
    cp = negtaxo - 1 ;
    while ( * ++ cp ) {
        if ( isspace ( * cp ) ) continue ;
        if ( num_unaccepts == MAX_NUM_UNACCEPTS ) {
            sprintf ( junkstr , "Cannot define so many strings to exclude taxa (max=%i)" , MAX_NUM_UNACCEPTS ) ;
            MessageBox ( hwnd , junkstr , "ERROR" , MB_OK ) ;
            errored = 1 ;
            return ; }
        gp = unaccept [ num_unaccepts ++ ] ;
        i = 0 ;
        while ( !isspace ( * cp ) && * cp && i ++ < 48 ) * gp ++ = * cp ++ ;
        * gp = '\0' ; }
    num_forceaccepts = 0 ;
    cp = postaxo - 1 ;
    while ( * ++ cp ) {
        if ( isspace ( * cp ) ) continue ;
        if ( num_forceaccepts == MAX_NUM_FORCEACCEPTS ) {
            sprintf ( junkstr , "Cannot define so many strings to include taxa (max=%i)" , MAX_NUM_FORCEACCEPTS ) ;
            MessageBox ( hwnd , junkstr , "ERROR" , MB_OK ) ;
            errored = 1 ;
            return ; }
        gp = forceaccept [ num_forceaccepts ++ ] ;
        i = 0 ;
        while ( !isspace ( * cp ) && * cp && i ++ < 48 ) * gp ++ = * cp ++ ;
        * gp = '\0' ; }
    return ;
}

double doneedwunsch ( char * ap , char * bp )
{
    int wid , hei , i , j , dacos ;
    char * app , * bpp ;
    char * abecs , * bbecs , * anp , * bnp ;
    double val ;
    int HIGH = 10000000 ;
    wid = strlen ( ap ) ;
    hei = strlen ( bp ) ;
    if ( !madeNWmem ) {
         cellcost = myalloc ( 60 * sizeof ( Stringcomptyp * ) ) ;
         for ( i = 0 ; i < 60 ; ++ i )
            cellcost[i] = myalloc ( 60 * sizeof ( Stringcomptyp ) ) ;
         madeNWmem = 1 ; }
    //  NOTE!!  only the first 60 letters of the name are used
    if ( hei >= 60 ) hei = 59 ;
    if ( wid >= 60 ) wid = 59 ; 
    cellcost[0][0].min = cellcost[0][0].diag = 0 ;
    cellcost[0][0].up = cellcost[0][0].lef = HIGH ;
    bpp = bp ;
    for ( j = 0 ; j < hei ; ++ j ) {
       app = ap ;
       for ( i = 0 ; i < wid ; ++ i ) {
          if ( !i && !j ) {
             continue ; }
          dacos = 0 ;
          if ( * app != * bpp ) dacos = suscost ;
          if ( j ) {
            if ( cellcost[i][j-1].min == cellcost[i][j-1].up )
               cellcost[i][j].up = cellcost[i][j-1].min + gapextcost ;
            else
               cellcost[i][j].up = cellcost[i][j-1].min + gapcost ; }
          else cellcost[i][j].up = cellcost[i][j].diag = HIGH ;
          if ( i ) {
            if ( cellcost[i-1][j].min == cellcost[i-1][j].lef )
               cellcost[i][j].lef = cellcost[i-1][j].min + gapextcost ;
            else
               cellcost[i][j].lef = cellcost[i-1][j].min + gapcost ; }
          else cellcost[i][j].lef = cellcost[i][j].diag = HIGH ;
          if ( i && j ) cellcost[i][j].diag = cellcost[i-1][j-1].min + dacos ;
          dacos = cellcost[i][j].diag ;
          if ( dacos > cellcost[i][j].up ) dacos = cellcost[i][j].up ;
          if ( dacos > cellcost[i][j].lef ) dacos = cellcost[i][j].lef ;
          cellcost[i][j].min = dacos ;
          ++ app ; }
       ++ bpp ; }
    dacos = cellcost[wid-1][hei-1].min ;
    if ( dacos > max_acceptable_diffs ) return 0 ;
    if ( dacos == 0 ) return 1 ;
    if ( wid > hei ) hei = wid ;
    val = ( double ) dacos / ( double ) hei ;
    val = 1 - val ;
    return val ;
}

int open_all_temps ( int blocktodo)
{
    int i ;
    char * extisat ;
    numinputs = 0 ;
    for ( i = 0 ; i < MAXFILES_PER_BLOCK ; ++ i ) {
        if ( blk -> files[i][0] == '\0' ) break ;
        strcpy ( fn , project_name ) ;
        extisat = find_extension ( fn ) ;
        sprintf ( extisat , "_bk%i_file%i.tmp" , blocktodo+1 , i+1 ) ;
        if ( ( inpf = fopen ( fn , "rb" ) ) == NULL ) {
              sprintf ( junkstr , "Cannot open file \"%s\" for input" , fn ) ;
              MessageBox ( hwnd , junkstr , "ERROR" , MB_OK ) ;
              return 0 ; }
        fclose ( inpf ) ;
        filekey [ numinputs].block = blocktodo ;
        filekey [ numinputs].file = i ; 
        ++ numinputs ; }
     return 1 ;
}

int open_all_alns ( void )
{
    int i ;
    char * extisat ;
    numinputs = 0 ;
    for ( i = 0 ; i < proj_numblocks ; ++ i ) {
        if ( listofblocks[i] == -2 ) { numinputs ++ ; continue ; }
        strcpy ( fn , project_name ) ;
        extisat = find_extension ( fn ) ;
        sprintf ( extisat , "_bk%i_%s.aln" , i+1 , blocklist[i].blockname ) ;
        if ( ( inpf = fopen ( fn , "rb" ) ) == NULL ) {
              sprintf ( junkstr , "Cannot open file \"%s\" for input" , fn ) ;
              MessageBox ( hwnd , junkstr , "ERROR" , MB_OK ) ;
              return 0 ; }
        fclose ( inpf ) ;
        numinputs ++ ; }
     return 1 ;
}

void finalrdtaxname ( void )
{
    char * cp = tmpname - 1 , * cpp ;
    int i = 'a' , j , found_cuadruple , x ;
    int numunderscores = 0 , oldunderscore ;
    int strinlen , trunamlen ;
    char * old , * nu ;
    char * underscisat , prv = ' ' ;
    int isrepeat , isdiff ;
    while ( !isspace ( i ) ) {
        * ++ cp = i = cget ( curinput ) ;
        if ( i == '_' ) {
            if ( !numunderscores ) underscisat = cp ;
            ++ numunderscores ; }
        else
          if ( numunderscores < 4 ) numunderscores = 0 ; }
    trunamlen = underscisat - tmpname ;
    * cp ++ = '\0' ;
    //*** In case this comes from a TNT matrix with no taxonomy included... 
    if ( numunderscores < 4 ) {
        trunamlen = strlen ( tmpname ) ;
        underscisat = tmpname + trunamlen ; }
    numunderscores = 0 ;
    islike [ seqsread ] = seqsread ;
    chooseq[ seqsread ] = trunamlen = underscisat - tmpname ;
    nu = tmpname ;
    nu [ trunamlen ] = '\0' ;
    for ( i = seqsread ; i -- ; ) {
        if ( islike [ i ] != i ) continue ;
        old = names [ i ] ;
        nu = tmpname ;
        if ( !strcmp ( old , nu ) ) {
            islike [ seqsread ] = islike [ i ] ;
            break ; }
        if ( threshold < 1 ) {
           if ( doneedwunsch ( old , nu ) >= threshold ) {
             if ( query_taxon_identity && islike [ i ] == i ) {
                j = blk - blocklist ;
                x = 0 ;
                while ( j -- )
                    if ( blockstarts[j] <= seqsread && blockends[j] >= seqsread ) x = j ;
                sprintf ( junkstr ,
                   "Found name in block %i\n\n"
                   "       %s\n"
                   "\n" , x+1 , old ) ;
                   sprintf ( junkstr + strlen ( junkstr ) ,
                   "similar to name:\n\n"
                   "       %s\n\n"
                   "in block %i\n"
                   "\nDo you want to consider them the same?" , nu , 1+blk-blocklist ) ;
                 x = MessageBox ( hwnd , junkstr , "Please answer..." , MB_YESNOCANCEL ) ;
                 if ( x == IDCANCEL ) { errored = 1 ; return ; }
                 if ( x == IDYES ) {
                    if ( !headed_synonyms ) {
                          headed_synonyms = 1 ;
                          myp ( "FINAL MATRIX, name equivalencies (at similarity %.2f):\n" , threshold ) ; }
                     myp ( "    %s = %s\n" , nu , old ) ;
                     islike[seqsread] = islike[i] ;
                     break ; }}
              else {
                if ( !headed_synonyms ) {
                      headed_synonyms = 1 ;
                      myp ( "FINAL MATRIX, name equivalencies (at similarity %.2f):\n" , threshold ) ; }
                 myp ( "    %s = %s\n" , nu , old ) ;
                 islike[seqsread] = islike[i] ; }
              break ; }}}
    return ;
}

void free_excess_mem ( void )
{
    int i ;
    for ( i = 0 ; i < seqsread ; ++ i )
       if ( names [ i ] != NULL ) free ( names[i] ) ;
    for ( i = 0 ; i < num_unaccepts ; ++ i )
        free  ( unaccept[i] ) ;
    return ;
}

int effect_fas2fas ( int blocktodo )
{
    double val ;
    int i , j ,a,  trunum = 0 ;
    char * extisat ;
    blk = blocklist + blocktodo ;
    max_acceptable_diffs = 5 ;
    num_diff_seqs = 0 ;
    seqsread = 0 ;
    if ( !open_all_temps ( blocktodo ) ) return 0 ;
    strcpy ( fn , project_name ) ;
    extisat = find_extension ( fn ) ;
    sprintf ( extisat , "_bk%i_%s.fas" , blocktodo+1 , blocklist[blocktodo].blockname ) ;
    mergefile = fopen ( fn , "wb" ) ;
    if ( mergefile == NULL ) {
        sprintf ( junkstr , "Cannot open file \"%s\" for output" , fn ) ;
        MessageBox ( hwnd , junkstr , "ERROR" , MB_OK ) ;
        return 0 ; }
    errored = 0 ;
    if ( !mademem ) {
      seqlen = myalloc ( MAXSEQS * sizeof ( unsigned long int ) ) ; if ( errored ) return 0 ;
      names = myalloc ( MAXSEQS * sizeof ( char * ) ) ; if ( errored ) return 0 ;
      islike = myalloc ( MAXSEQS * sizeof ( signed long int ) ) ; if ( errored ) return 0 ;
      chooseq = myalloc ( MAXSEQS * sizeof ( unsigned long int ) ) ; if ( errored ) return 0 ;
      present_in_block = myalloc ( MAXSEQS * sizeof ( unsigned long int ) ) ; if ( errored ) return 0 ;
      mademem = 1 ; }
    parse_strings_to_trash () ;
    number_of_passes = 1 ;
    /*** First, identify duplicates and their correspondence *******/
    for ( i = 0 ; i < numinputs ; ++ i ) {
        sprintf ( extisat , "_bk%i_file%i.tmp" , filekey[i].block+1 , filekey[i].file+1) ;
        curinput = fopen ( fn , "rb" ) ; 
        sizit () ;
        mycurfile = i ;
        while ( !errored ) {
            gotoarrow( ) ;
            if ( feof ( curinput ) ) break ;
            names [ seqsread ] = NULL ;
            rdtaxname ( ) ;
            ++ trunum ;
            skipsequence ( 1 ) ;
            if ( blk -> minslen )
                 if ( seqlen [ seqsread ] < blk -> minslen ) unacceptable = 1 ;
            if ( blk -> maxslen )
                 if ( seqlen [ seqsread ] > blk -> maxslen ) unacceptable = 1 ;
            if ( itsahybrid ) islike [ seqsread ] = -1 ;
            if ( unacceptable ) islike [ seqsread ] = -1 ;
            if ( islike [ seqsread ] == seqsread && !itsahybrid && !unacceptable ) ++ num_diff_seqs ;
            /** mark the longest duplicate, if sequence is not unique... ***/
            if ( !itsahybrid && !unacceptable ) {
               chooseq [ seqsread ] = seqsread ;
               if ( islike [ seqsread ] != seqsread )
                  if ( seqlen [ islike [ seqsread ] ] < seqlen [ seqsread ] ) {
                    chooseq [ islike [ seqsread ]] = seqsread ;
                    seqlen [ islike [ seqsread ]] = seqlen [ seqsread ] ; }
               names[ seqsread ] = myalloc ( ( strlen ( tmpname ) + 1 ) * sizeof ( char ) ) ;
               strcpy ( names[ seqsread ] , tmpname ) ; }
            ++ seqsread ;
            mydildit () ; }
        fclose ( curinput ) ; }

      /***  Now, parse and save the ones you need *******/
      number_of_passes = 2 ;
      seqsread = 0 ;
      if ( !open_all_temps ( blocktodo ) ) return 0 ;
      for ( i = 0 ; i < numinputs ; ++ i ) {
        sprintf ( extisat , "_bk%i_file%i.tmp" , filekey[i].block+1 , filekey[i].file+1) ;
        curinput = fopen ( fn , "rb" ) ; 
        sizit () ;
        mycurfile = i ;
        while ( !errored ) {
            gotoarrow () ;
            if ( feof ( curinput ) ) break ;
            skiptaxname () ;
            if ( islike [ seqsread ] < 0  && dooneseqpersp )
                 skipsequence ( 0 ) ;
            else {
                if ( seqsread == chooseq [ islike [ seqsread ]] ) {
                    fprintf ( mergefile , ">%s\n" , names [ seqsread ] ) ;
                    rdsequence () ; }
                else skipsequence ( 0 ) ; }
            ++ seqsread ;
            mydildit () ; }
        fclose ( curinput ) ; }
   myp ( "\n    Retained sequences for %i distinct names" , num_diff_seqs ) ;
   fclose ( mergefile ) ;
   free_excess_mem () ;
   if ( errored ) return 0 ;
   if ( num_diff_seqs < 4 ) return 2 ;
   return 1 ;
}

int create_tnt_matrix ( int runtnt )
{
    double val ;
    int i , j , k, atleast = 0 ;
    char * extisat , * taxois , * myname , bak , * pttax ;
    unsigned long int tot_numchars = 0 ;
    char countedchunk ;
    unsigned long int * isinblock , taxa_in_this_block ;
    int havesomeprots ;
    num_diff_seqs = 0 ;
    headed_synonyms = 0 ;
    seqsread = 0 ;
    if ( !mademem ) {
      seqlen = myalloc ( MAXSEQS * sizeof ( unsigned long int ) ) ; if ( errored ) return 0 ;
      names = myalloc ( MAXSEQS * sizeof ( char * ) ) ; if ( errored ) return 0 ;
      islike = myalloc ( MAXSEQS * sizeof ( unsigned long int ) ) ; if ( errored ) return 0 ;
      chooseq = myalloc ( MAXSEQS * sizeof ( unsigned long int ) ) ; if ( errored ) return 0 ;
      present_in_block = myalloc ( MAXSEQS * sizeof ( unsigned long int ) ) ; if ( errored ) return 0 ;
      mademem = 1 ; }
    if ( !open_all_alns ( ) ) return 0 ;
    errored = 0 ;
    number_of_passes = 1 ;
    isinblock = seqlen ;
    for ( i = 0; i < proj_numblocks ; ++ i  ) 
        if ( ! (listofblocks[i] == -2 ) ) atleast ++ ;
    if ( atleast == 0  ){
      sprintf ( junkstr , "There is no block with more than three sequences" ) ;
      MessageBox ( hwnd , junkstr , "ERROR" , MB_OK ) ;
      undild () ;
      return 0 ; }
    strcpy ( fn , project_name ) ;
    extisat = find_extension ( fn ) ; 
    /*** First, identify duplicates and their correspondence *******/
    for ( i = 0 , blk = blocklist ; i < numinputs ; ++ i , ++ blk ) {
        if ( listofblocks[i] == -2 ) continue ;
        sprintf ( extisat , "_bk%i_%s.aln" , i+1 , blocklist[i].blockname ) ;
        curinput = fopen ( fn , "rb" ) ; 
        sizit () ;
        mycurfile = i ;
        countedchunk = 0 ;
        blockstarts[ i ] = seqsread ;
        blockends[ i ] = seqsread + 1 ;
        for ( j = 0 ; j < blockends[ i ] ; ++ j ) isinblock [ j ] = 0 ;
        for ( j = 0 ; j < MAXSEQS ; ++ j ) present_in_block [ j ] = 0 ;
        taxa_in_this_block = 0 ;
        this_is_a_TNT_file = 0 ; 
        if ( blocklist[i].sourcetype == TNT ) this_is_a_TNT_file = 1 ; 
        while ( !errored ) {
            gotoarrow( ) ;
            if ( feof ( curinput ) ) break ;
            finalrdtaxname ( ) ;
            if ( errored ) break ;
            if ( feof ( curinput ) ) break ;
            skipsequence ( 0 ) ;
            if ( !countedchunk ) {
                 countedchunk = 1 ;
                 tot_numchars += thiseqlen ; }
            if ( ! present_in_block [ islike [ seqsread ] ] ++ ) ++ taxa_in_this_block ; 
            if ( islike [ seqsread ] == seqsread ) {
                ++ num_diff_seqs ; }
            tmpname [ chooseq [ seqsread ]] = '\0' ;
            names[seqsread] = myalloc ( ( chooseq [ seqsread ] + 1 ) * sizeof ( char ) ) ;
            strcpy ( names[seqsread] , tmpname ) ;
            ++ isinblock[ islike [ seqsread ]] ;
            ++ seqsread ;
            blockends [ i ] = seqsread ; }
        if ( taxa_in_this_block < 4 ) {
            listofblocks[i] = -3 ;
            tot_numchars -= thiseqlen ; }
        fclose ( curinput ) ; }
       endildit () ;
      if ( errored ) {
          undild () ;
          myp ( "\nAborted matrix creation..." ) ; newln () ;
          fclose ( mergefile ) ;
          return 0 ; }

   /***  Now, parse and save the ones you need *******/
   strcpy ( fn , project_name ) ;
   extisat = find_extension ( fn ) ;
   sprintf ( extisat , ".tnt" ) ;
   mergefile = fopen ( fn , "wb" ) ;

   if ( mergefile == NULL ) {
      sprintf ( junkstr , "Cannot open file \"%s\" for output" , fn ) ;
      MessageBox ( hwnd , junkstr , "ERROR" , MB_OK ) ;
      return 0 ; }

      number_of_passes = 2 ;
      seqsread = 0 ;
      if ( !open_all_alns ( ) ) {
          fclose ( mergefile ) ;
          return 0 ; }
      j = havesomeprots = 0 ;
      for ( i = 0 , blk = blocklist ; i < proj_numblocks && !j ; ++ i  , ++ blk )
           if ( ! blk -> isdna ) j = 1 ;
      if ( !j ) fprintf ( mergefile , "nstates dna ;\n" ) ;
      else { fprintf ( mergefile , "nstates prot ;\n" ) ; havesomeprots = 1 ; }
      j = 120 ;
      j = ( j * num_diff_seqs ) / 5000 ;
      j = ( j * tot_numchars ) / 2000 ;
      j += 2 ;
      if ( havesomeprots ) j *= 3 ; 
      if ( j < 32 ) j = 32 ;
      if ( j > 1220 ) j = 1220 ; 
      fprintf ( mergefile , "mxram %i ;\ntaxonomy=;\ncost < ;\n" , j ) ;
      fprintf ( mergefile , "xwipe;\ntaxname+300;\ntaxname[!;\n" ) ;
      fprintf ( mergefile , "xread\n'Matrix created by GB2TNT, %s'\n%i %i\n" , curtimestring () , tot_numchars , num_diff_seqs ) ;

      for ( i = 0 , blk = blocklist ; i < numinputs ; ++ i , ++ blk ) {
        if ( listofblocks[i] == -2 ) continue ;
        sprintf ( extisat , "_bk%i_%s.aln" , i+1 , blocklist[i].blockname ) ;
        curinput = fopen ( fn , "rb" ) ; 
        sizit () ;
        mycurfile = i ;
        if ( listofblocks[i] != -3 ) 
           if ( numinputs > 1 || blk -> sourcetype == TNT )
              if ( blk -> sourcetype == TNT ) {
                  if ( blk -> blockheader[0] ) fprintf ( mergefile , "&[ %s ]\n" , blk->blockheader ) ;
                  else fprintf ( mergefile , "&[ num ]\n" ) ; }
              else {
                  if ( blk -> isdna ) fprintf ( mergefile , "&[dna " ) ;
                  else fprintf ( mergefile , "&[prot " ) ;
                  if ( ( blk -> missgaps & 1 ) ) fprintf ( mergefile , "nogaps " ) ;
                  else {
                      fprintf ( mergefile , "gaps " ) ; 
                      if ( ( blk -> missgaps & 2 ) ) fprintf ( mergefile , "trimhead " ) ;
                      if (  ( blk -> missgaps & 4 ) ) fprintf ( mergefile , "trimtail " ) ; }
                  fprintf ( mergefile , "]\n" ) ; }
        for ( j = 0 ; j < blockends[ i ] ; ++ j ) isinblock [ j ] = 0 ;
        while ( !errored ) {
            gotoarrow () ;
            if ( feof ( curinput ) ) break ;
            skiptaxname () ;
            if ( isinblock [ islike [ seqsread ] ] )
                 skipsequence ( 0 ) ;
            else {
               ++ isinblock [ islike [ seqsread ] ] ;
               if ( listofblocks[i] == -3 ) skipsequence ( 0 ) ;
               else {
                   myname = names [ islike [ seqsread ]] ;
                   fprintf ( mergefile , "%s" , myname ) ;
                   taxois = tmpname + chooseq [ seqsread ] ;
                   if ( !save_accession_numbers ) {
                          while ( * taxois != '@' && * taxois ) ++ taxois ;
                          if ( save_taxonomy ) {
                              fprintf ( mergefile , "____" ) ;
                              if ( wipe_before ) {
                                  fprintf ( mergefile , "@" ) ; 
                                  j = wipe_before ;
                                  while ( j && * taxois ) 
                                      if ( * taxois ++ == '_' ) -- j ; }
                              fprintf ( mergefile , "%s  " , taxois ) ; }
                          else fprintf ( mergefile , "  " ) ; }
                   else
                     if ( !save_taxonomy ) {
                         while ( * taxois != '@' && * taxois )
                                fprintf ( mergefile , "%c" , * taxois ++ ) ;
                         fprintf ( mergefile , "  " ) ; }
                     else {
                         while ( * taxois != '@' && * taxois )
                             fprintf ( mergefile , "%c" , * taxois ++ ) ;
                         if ( wipe_before ) {
                             if ( * taxois == '@' ) fprintf ( mergefile , "%c" , * taxois ++ ) ; 
                             j = wipe_before ;
                             while ( j && * taxois ) 
                                 if ( * taxois ++ == '_' ) -- j ; }
                         if ( wipe_after > wipe_before ) {
                             j = wipe_before ;
                             pttax = taxois ;
                             while ( j < wipe_after ) 
                                 if ( * ++ pttax == '_' ) ++ j ;
                             * pttax = '\0' ; }
                         fprintf ( mergefile , "%s  " , taxois ) ; }
                   rdsequence () ; }}
            ++ seqsread ; }
        fclose ( curinput ) ; }
    endildit () ;
   fprintf ( mergefile , "\n;\n" ) ; 
   for ( i = k = j = 0 , blk = blocklist ; i < proj_numblocks ; ++ i , ++ blk ) {
        if ( listofblocks[i] == -2 || listofblocks[i] == -3 ) continue ;
        ++ k ;
        if (proj_numblocks > 1) {
            if ( !j ) fprintf ( mergefile , "cname\n" ) ;
            j = 1 ; 
            fprintf ( mergefile , "[%i %s;\n" , k , blk -> blockname ) ; }}
   if ( j ) fprintf ( mergefile , ";\n" ) ; 
   fprintf ( mergefile , "outgroup[!;\ntaxonomy];\n" ) ; 
   fprintf ( mergefile , ";\nproc/;\n" ) ;
   undild () ;
   if ( k == 0 ) {
      sprintf ( junkstr , "Matrix contains only %i distinct taxa (minimum=4)" , num_diff_seqs ) ;
      MessageBox ( hwnd , junkstr , "ERROR" , MB_OK ) ;
       undild () ;
       return 0 ;
       fclose ( mergefile ) ;}
   strcpy ( fn , project_name ) ;
   extisat = find_extension ( fn ) ;
   sprintf ( extisat , ".tnt" ) ;
   if ( !errored ) {
      myp ( "\nCREATED MATRIX \"%s\" WITH %i TAXA AND %i CHARS." , fn , num_diff_seqs , tot_numchars ) ;
      if ( k != proj_numblocks ) 
         myp ( "\n   Note: %i blocks were empty according to filtering criteria in effect!" , proj_numblocks - k ) ; }
   fclose ( mergefile ) ;
   free_excess_mem () ;
   if ( errored ) return 0 ;
   return 1 ;
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
           x = ap [ lenb ] ;  // esto es para q devuelva el valor dde termina el match
           ap [ lenb ] = '\0' ;  // le da final al string con = largo q el string b q es el que va a comparar
             if ( stringis ( ap , b ) ) {
               ap [ lenb ] = x ;
               return ( at + lenb ) ; }
           ap [ lenb ] = x ; }
        ++ at ;
        ++ ap ;
        if ( at + lenb > lena ) break ; }
    return 0 ;
}



