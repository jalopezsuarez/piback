// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <linux/input.h>
#include <linux/uinput.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <fcntl.h>

#include "joystick.hh"
#include "INIReader.h"

#include <iostream>
#include <fstream>
using namespace std;

#define PROG_NAME "c2h2_spi_kbd"

int fire_key(__u16);
int setup_uinputfd(const char *);
int close_uinputfd();
void write_uinput(__u16, __u16, __s32);

/*fd for uinput, we do need kernel to support uinput */
static int uinputfd = -1;

int main(int argc, char **argv)
{
    // =======================================================
    {
        setbuf(stdout, NULL);
        write(1,"\E[H\E[2J",7);
        cout << "\033[H\033[2J";
        cout << "\033[2J\033[1;1H";
        cout << "\x1B[2J\x1B[H";
        cout << "\x1B[2J";
        cout << "\x1B[0;0H";
    }
    // =======================================================
    
    Joystick input0(0);
    
    while (1)
    {
        usleep(1000);
        
        if (input0.isFound())
        {
            JoystickEvent event;
            if (input0.sample(&event) && event.isButton() && !event.isInitialState())
            {
                if (event.value > 0)
                {
                    switch (event.number)
                    {
                        case 4:
                            // ps3 up
                            fire_key(KEY_UP);
                            break;
                        case 6:
                            // ps3 down
                            fire_key(KEY_DOWN);
                            break;
                        case 7:
                            // ps3 left
                            fire_key(KEY_LEFT);
                            break;
                        case 5:
                            // ps3 right
                            fire_key(KEY_RIGHT);
                            break;
                        case 0:
                            // ps3 select
                            fire_key(KEY_ESC);
                            break;
                        case 3:
                            // ps3 start
                            fire_key(KEY_ENTER);
                            break;
                        default:
                            break;
                            
                    }
                }
                
            }
        }
    }
    
    // =======================================================
    
    exit(0);
}

// ~ Methods
// =======================================================

int fire_key(__u16 key){
    uinputfd = setup_uinputfd("pikeys");
    if(uinputfd != -1)
        write_uinput(EV_KEY, key, 1);
    close_uinputfd();
    return 0;
}

void write_uinput(__u16 type, __u16 code, __s32 value){
    struct input_event ie;
    
    //usleep(10000);
    
    memset(&ie, 0, sizeof(ie));
    ie.type = type;
    ie.code = code;
    ie.value = value;
    
    if(write(uinputfd, &ie, sizeof(ie)) != sizeof(ie)) puts("ERR1");
    
    memset(&ie, 0, sizeof(ie));
    ie.type = EV_SYN;
    ie.code = SYN_REPORT;
    ie.value = 0;
    
    if(write(uinputfd, &ie, sizeof(ie)) != sizeof(ie)) puts("ERR2");
}

int close_uinputfd(){
    close(uinputfd);
    return 0;
}

int setup_uinputfd(const char *name){
    int fd;
    int key;
    struct uinput_user_dev dev;
    
    fd = open("/dev/input/uinput", O_RDWR);
    if (fd == -1) {
        fd = open("/dev/uinput", O_RDWR);
        if (fd == -1) {
            fd = open("/dev/misc/uinput", O_RDWR);
            if (fd == -1) {
                fprintf(stderr, "could not open %s\n", "uinput");
                perror(NULL);
                return -1;
            }
        }
    }
    memset(&dev, 0, sizeof(dev));
    strncpy(dev.name, name, sizeof(dev.name));
    dev.name[sizeof(dev.name) - 1] = 0;
    if (write(fd, &dev, sizeof(dev)) != sizeof(dev) ||
        ioctl(fd, UI_SET_EVBIT, EV_KEY) != 0
        || ioctl(fd, UI_SET_EVBIT, EV_REP) != 0) {
        goto setup_error;
    }
    
    for (key = KEY_RESERVED; key <= KEY_UNKNOWN; key++) {
        if (ioctl(fd, UI_SET_KEYBIT, key) != 0) {
            goto setup_error;
        }
    }
    
    if (ioctl(fd, UI_DEV_CREATE) != 0) {
        goto setup_error;
    }
    return fd;
    
setup_error:
    fprintf(stderr, "could not setup %s\n", "uinput");
    perror(NULL);
    close(fd);
    return -1;
}
