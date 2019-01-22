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

#define NAMESIZE _MAX_PATH

VTYPE char junkstr[16005] ;

#define MAXFILES_PER_BLOCK 25
#define MAXPRODS_PER_BLOCK 25 
#define MAXPRODNAME 50
#define MAXNUMBLOCKS 150
#define MAXBLOCKNAME 100
#define MAXTAXO 1000
#define MAX_BLOCK_HEADER 40 

#define NUCLEAR 1
#define MITOCH  2
#define PLASTID 3
#define CHLORO  4
#define ANYGENOME 5 

#define GENE      1
#define CDS       2
#define RRNA      4
#define TRNA      8
#define MISCFEAT  16
#define MISCRNA   32

char cattyp[6][13] =
           { "gene " , "CDS" , "rRNA" , "tRNA" , "misc_feature" , "misc_RNA" } ; 

#define GBANK   1
#define FASTA   2
#define ALIGNED 4
#define TNT     8 

#define ALLMISSING   1
#define LEADMISSING  2
#define TRAILMISSING 4

typedef struct {
     char useralignparms , aligner , alignparms[_MAX_PATH] ; 
     char blockname[ MAXBLOCKNAME ] ; 
     char files[MAXFILES_PER_BLOCK][_MAX_PATH] ;
     char prod[MAXPRODS_PER_BLOCK][MAXPRODNAME];
     char prod2[MAXPRODS_PER_BLOCK][MAXPRODNAME];
     char genome ;
     char stringsim ;
     char isdna ;
     unsigned long int maxslen , minslen ;
     char oneseqpersp , alsonote ;
     char category ; 
     char complete ;
     char twogen [ 3 ];
     char voucher ;
     char blockheader[ MAX_BLOCK_HEADER ] ; 
     time_t modtime ;
     int sourcetype ;
     int missgaps ; }
     Blocktyp ;

VTYPE Blocktyp tmpblocklist[MAXNUMBLOCKS] ;
VTYPE Blocktyp blocklist[MAXNUMBLOCKS] ;

VTYPE char project_name[ _MAX_PATH ] ;

VTYPE time_t getfilemodtime( char * ) ;

VTYPE char fn[MAX_PATH*MAXFILES_PER_BLOCK] ;
VTYPE char quotedfn[MAX_PATH*MAXFILES_PER_BLOCK] ;

#define XTRA_EXT ".tmp"
#define PROC_EXT ".fas"
#define ALIGN_EXT ".aln"
 
VTYPE char * bgilspace ; 
VTYPE char ** gilspace ; 
VTYPE char **  gil ; 
VTYPE int linsdone ;
VTYPE int screen_size ;
VTYPE int screen_width ;

extern void myp ( void * , ... ) ;    /* prints args , using "printf" format */

void showmoddate ( time_t ) ; 

signed char listoffiles[MAXNUMBLOCKS*MAXFILES_PER_BLOCK];
signed char listofblocks[MAXNUMBLOCKS];
char * allnamespt[MAXNUMBLOCKS*MAXFILES_PER_BLOCK];
signed char listoftemps[MAXNUMBLOCKS];
     
VTYPE char mafftexe[ MAX_PATH ] ;
VTYPE char muscleexe[ MAX_PATH ] ;
VTYPE char alignerexe[ MAX_PATH ] ;
VTYPE char tntexe[ MAX_PATH ] ;
VTYPE char mafftparams[ MAX_PATH ] = { "--auto " } ; 
VTYPE char muscleparams[ MAX_PATH ] = { " " } ;
VTYPE char userparams[ MAX_PATH * 2 ] ;
VTYPE char parmlist[ MAX_PATH * 3 ] ;
VTYPE char bioeditexe[ MAX_PATH ] ;
VTYPE char bioeditparams[ MAX_PATH ] ;

VTYPE char postaxo [ MAXTAXO ] ;
VTYPE char negtaxo [ MAXTAXO ] ;

VTYPE int want_hybrids , want_confers , want_sp , want_environ , want_uncult , want_affinis ;
VTYPE int wipe_before , wipe_after ; 

VTYPE time_t proj_modtime ; 
