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
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <setjmp.h>
#include <ctype.h>
#include <windows.h>
#include <windowsx.h>
#include <io.h>
#include <conio.h>
#include <winbase.h>
#include <winuser.h>
#include <commctrl.h>
#include <commdlg.h>
#include <process.h>
#include"gb2tnt.h"

extern FILE * logfileis ; 

extern HWND hwnd ; 
#define MAXSCREEN_SIZE 150
int WINBUFSIZ ; 
int first_linout , last_linout ; 
int scrollpos ;
int wrapdbuf ; 
int maxlines ;
char * xat , * gilend ;
int atlin ;
int starlin ; 
int starbufis ;

int pressbuff = 0 ;

char * linbuf ; 
static char thestr [ 26 ] ;
static int didasearch = 0 ;

int buflinlen ( char * tx )
{
    int len = 0 ;
    char * cp = tx ; 
    if ( !pressbuff ) return strlen ( tx ) ;
    while ( * cp ) {
           if ( * cp == 1 ) len += * ++ cp ;
           else ++ len ; 
           ++ cp ; }
    return len ;
} 

void clear_text_buffer ( void )
{
      gilspace [ 0 ] = xat = bgilspace ; 
      atlin = starlin = linsdone = 0 ;
      scrollpos = starbufis = last_linout = first_linout = 0 ; 
      WINBUFSIZ = 0 ; 
} 

int give_scroll_line ( void )
{  int a , b ;
   if ( ( a = give_total_lines () - scrollpos ) < 0 ) a = 0 ; 
   return a ; 
} 

int give_first_line ( void )
{
    return ( first_linout ) ; 
}     

int give_last_line ( void )
{ 
   return ( last_linout ) ;
} 

int give_scroll_pos ( void )
{ int a , b ;
  b = give_total_lines () ;
  if ( b > screen_size ) b -= ( screen_size ) ; // this needed for thumb to reach bottom
  if ( !b ) return 0 ;
  a = b + scrollpos ;
  if ( a < 0 ) a = 0 ;
  return ( ( ( b - a ) * 100 ) / b ) ; 
}

int give_total_lines ( void )
{
    return ( atlin - starlin ) ; 
}

int rescuing = 0 ;

void initialize_winbufsiz ( int val )
{   int i ;
    int mxlin ; 

    val = val * 1000 ; 
    if ( val < 50000 ) val = 50000 ;
    mxlin = val / 10 ;

    maxlines = mxlin ;
    val += 2 ;
    mxlin += 2 ;
    while ( ( val % 4 ) ) ++ val ;
    bgilspace = malloc ( val * sizeof ( char ) ) ;
    gilspace = ( char ** ) malloc ( mxlin * sizeof ( char * ) ) ;
    gil = ( char ** ) malloc ( MAXSCREEN_SIZE * sizeof ( char * ) ) ;
    if ( gil == NULL || bgilspace == NULL || gilspace == NULL ) {
        free ( gil ) ;
        free ( bgilspace ) ;
        free ( gilspace ) ;
        if ( !rescuing ) {
           MessageBox ( hwnd , "Not enough memory to allocate display buffer.\nWill attempt to allocate a smaller buffer." , "ERROR" , MB_OK | MB_ICONWARNING ) ;
           rescuing = 1 ; 
           initialize_winbufsiz ( 50 ) ;
           rescuing = 0 ; }
        else
           MessageBox ( hwnd , "Can't allocate small buffer.\nFree memory and re-try." , "ERROR" , MB_OK | MB_ICONERROR ) ;
        return ; } 
    gilend = bgilspace + ( ( val * sizeof ( char ) ) - 2 ) ; 
    atlin = starlin = linsdone = 0 ;
    starbufis = scrollpos = WINBUFSIZ = 0 ;
    last_linout = first_linout = 0 ;
    gilspace [ 0 ] = xat = bgilspace ;
    return ; 
}

int give_display_buffer_size ( void )
{
    return ( gilend - bgilspace ) / 1000 ; 
}

int give_free_buffer_bytes ( void ) 
{ 
  return gilend - xat ; 
} 

void reset_winbufsiz ( int newval , int ojo )
{
   int i = IDYES ;
   if ( linsdone > 0 && ojo ) {
      i = MessageBox ( hwnd, "Resetting size of display buffer requires clearing it.\n\nProceed anyway?", "WARNING", MB_YESNO | MB_ICONWARNING ) ;  }
   if ( i == IDNO ) return ;
   clear_text_buffer () ; 
   free ( bgilspace ) ; 
   free ( gil ) ;
   free ( gilspace ) ;
   initialize_winbufsiz ( newval ) ;
   return ; 
}     
    
void save_txt_buffer ( void )
{
    int a , i , from , to ;
    char * cp ; 
    int scrwas = screen_size ;
    from = 0 ;
    to = atlin - starlin ;
    if ( starlin == atlin ) return ;
    for ( i = starlin + from ; i <= starlin + to ; ++ i )
         fprintf ( logfileis , "%s\n", gilspace [ i ] ) ;
}     

void copygil ( int scrolldiff )
{
    int i , j = 0 ;
    scrollpos += scrolldiff ;
    if ( scrollpos > 0 ) scrollpos = 0 ;
    if ( scrollpos < - ( ( atlin - starlin ) )  ) 
        scrollpos = - ( ( atlin - starlin ) ) ; 

    while ( ( starlin - scrollpos ) < 0 ) -- scrollpos ;
    first_linout = - scrollpos ;
    for ( i = starlin - scrollpos ; i < atlin && j < screen_size ; ++ i )
            gil [ j ++ ] = gilspace [ i ] ;
    last_linout = ( ( - scrollpos ) + j ) - 1 ; 
    linsdone = j ; 
    return ;
}

int prvbuf = 0 ; 
int spccnt = 0 ;
int markdspc = 0 ;

spewtogil ( char * spewbuff )
{
   int a , i ;
   char * pt = spewbuff;
   char * pa = xat ;
   int zerit = 0 ;
   int starbufwas ;
   wrapdbuf = 0 ;
   if ( gilspace [ starlin ] > bgilspace ) wrapdbuf = 1 ;  
   while ( * pt ) {
       if ( pressbuff ) { 
         if ( * pt == ' ' 
            && prvbuf == ' ' && spccnt < 125 ) {
             if ( !markdspc ) { 
                    * ( pa - 1 ) = 1 ;
                    spccnt = 1 ; 
                    ++ pa ; } 
             markdspc = 1 ; 
             * ( pa - 1 ) = ++ spccnt ;
             ++ pt ;
             continue ; } 
         prvbuf = * pt ;
         spccnt = 0 ; 
         markdspc = 0 ; } 
       if ( * pt == '\n' ) {
           ++ pt ; * pa ++ = '\0' ;
           ++ atlin ; 
           if ( atlin >= maxlines ) {
               if ( !starlin ) {
                   clear_text_buffer () ; 
                   spewem ( "NOTE: wrapped lines before wrapping buffer contents!" , NULL ) ; } 
               a = 0 ;
               for ( i = starlin ; i < maxlines ; ++ i ) 
                   gilspace [ a ++ ] = gilspace [ i ] ;
               atlin = a ;
               starlin = 0 ;
               wrapdbuf = 0 ; } 
           gilspace [ atlin ] = pa ; zerit = 0 ; }
       else { * pa ++ = * pt ++ ; zerit = 1 ; }
       if ( pa >= gilend ) {
           * pa = '\0' ; strcpy ( bgilspace , gilspace [ atlin ] ) ;
           gilspace [ atlin ] [ 0 ] = '\0' ; 
           pa = bgilspace + ( pa - gilspace [ atlin ] ) ; 
           gilspace [ atlin ] = bgilspace ;  
           wrapdbuf = 2 ; starbufwas = starbufis ; starbufis = atlin ; } 
       }
   if ( zerit ) * pa = '\0' ;
   if ( wrapdbuf ) {
       a = ++ starlin ;
       if ( wrapdbuf == 2 && starbufwas > starlin ) a = starbufwas + 1 ; 
       while ( gilspace [ a ] < pa && gilspace [ a ] != bgilspace && a < atlin ) ++ a ; 
       if ( a < atlin ) 
             starlin = a ; }
   WINBUFSIZ = atlin - starlin ; 
   xat = pa ;
   return ;
}


