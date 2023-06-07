#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/ip.h>
#include <string.h>

/*--------------------------------------*/
// Defines
#define KLG_MAX_QUEUE 10
#define KLG_MAX_CLIENTS 10


/*-------------------------------------*/
//variables
extern int g_verbose;
static int g_clients[KLG_MAX_CLIENTS];
static int g_serverFD;

/*-------------------------------------*/
//Private Functions
int KLG_indexOfEmptyFD( int *arrFDs, int len );


/*------------------------------------*/


int KLG_networkInit( int port ){
    if ( g_verbose )
        printf( "intitializing network\n" );
    int i;
    for ( i = 0; i < KLG_MAX_CLIENTS; i++ ){
        g_clients[i] = -1;
    }

    g_serverFD = socket( AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0 );
    if( -1 == g_serverFD )
        perror( "Error opening network socket\n" );
  
    if ( g_verbose )
        printf( "server FD is %d\n",g_serverFD );

    int opt = 1;
    if( -1 == setsockopt( g_serverFD, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof( opt ) ))    
        perror( "setsockopt" );    
    
    struct sockaddr_in name;
    name.sin_family = AF_INET;
    name.sin_port = (in_port_t)htons(port);
    name.sin_addr.s_addr = htonl(INADDR_ANY);
    if ( -1 == bind( g_serverFD, ( struct sockaddr *)&name, sizeof( name ) ))
        perror( "Error bind() failed\n" );

    if( listen( g_serverFD, KLG_MAX_QUEUE ) == -1){
        perror("Error listen() failed\n");
    }
    if( g_verbose ){
        printf( "network server configured\n" );
    }
    return 1;
}

int KLG_indexOfEmptyFD( int *arrFDs, int len ){
    int i;
    for ( i = 0; i < len; i++ ){
        if( arrFDs[i] == -1 ){
            return i;
        }
    }
    return -1;
}

void KLG_transmit( char *msg ){
    int index;
    int clientFD;
    while ( 0 <= (index = KLG_indexOfEmptyFD( g_clients, KLG_MAX_CLIENTS )) &&
            0 <= (clientFD = accept(g_serverFD, NULL, NULL))){
       g_clients[index] = clientFD;
        if( g_verbose )
            printf( "accepted socket client with FD %d\n", clientFD );
    } 
    int nTrans = 0;
    int i;
    for( i = 0; i < KLG_MAX_CLIENTS; i++ ){
        if( -1 != g_clients[i] ){
            if( -1 == write( g_clients[i], msg, strlen( msg ) * sizeof (char) )){
                perror( "write error" );
                close( g_clients[i] );
                g_clients[i] = -1;
            }else{
                printf( "wrote to client FD %d\n", g_clients[i] );
                nTrans++;
            }
        }
    }
    if( g_verbose )
        printf("transmitted \"%s\" to %d clients\n", msg, nTrans);
        
}


