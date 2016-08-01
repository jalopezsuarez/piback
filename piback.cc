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
#include <string.h>
#include <signal.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "joystick.hh"
#include "INIReader.h"

#include <iostream>
#include <fstream>
using namespace std;

// =======================================================

const char* deviceInput[4];
char** applications;

struct hotkey {
    int hotkey_a;
    int hotkey_b;
    int event_a;
    int event_b;
    int button_a;
    int button_b;
};
struct hotkey hotkeys[4];

// =======================================================

char* strchar(string* str);
int kill(const char *name);

// =======================================================

int main(int argc, char **argv)
{
    // =======================================================
    
    string resource = "piback.ini";
    if (argc > 2 && strcmp(argv[1], "--ini") == 0)
    {
        resource = argv[2];
    }
    INIReader reader(resource);
    
    // -------------------------------------------------------
    
    if (argc < 1 || reader.ParseError() < 0)
    {
        cout << "Usage: " << argv[0] << " --ini <path/file>"
        << "\nOptions:\n"
        << "  --ini <path/file> \t absolute path to application ini file\n"
        << endl;
        exit(1);
    }
    
    // -------------------------------------------------------
    
    string iniDevice0 = reader.Get("device", "device0_input", "/dev/input/js0");
    string iniDevice1 = reader.Get("device", "device1_input", "/dev/input/js1");
    string iniDevice2 = reader.Get("device", "device2_input", "/dev/input/js2");
    string iniDevice3 = reader.Get("device", "device3_input", "/dev/input/js3");
    
    string iniApplications  = reader.Get("application", "application_process", "");
    
    int iniHotkey0ButtonA = reader.GetInteger("hotkey", "hotkey0_button_a", -1);
    int iniHotkey0ButtonB = reader.GetInteger("hotkey", "hotkey0_button_b", -1);
    
    int iniHotkey1ButtonA = reader.GetInteger("hotkey", "hotkey1_button_a", -1);
    int iniHotkey1ButtonB = reader.GetInteger("hotkey", "hotkey1_button_b", -1);
    
    int iniHotkey2ButtonA = reader.GetInteger("hotkey", "hotkey2_button_a", -1);
    int iniHotkey2ButtonB = reader.GetInteger("hotkey", "hotkey2_button_b", -1);
    
    int iniHotkey3ButtonA = reader.GetInteger("hotkey", "hotkey3_button_a", -1);
    int iniHotkey3ButtonB = reader.GetInteger("hotkey", "hotkey3_button_b", -1);
    
    // -------------------------------------------------------
    
    deviceInput[0] = strchar(&iniDevice0);
    deviceInput[1] = strchar(&iniDevice1);
    deviceInput[2] = strchar(&iniDevice2);
    deviceInput[3] = strchar(&iniDevice3);
    
    // -------------------------------------------------------
    
    if (!iniApplications.empty())
    {
        char* stringApplications = strchar(&iniApplications);
        applications = NULL;
        
        int count = 0;
        char *cursor = strtok(stringApplications, ",");
        while(cursor != NULL)
        {
            if (strlen(cursor) > 0)
            {
                applications = (char**) realloc (applications, (count+1) * sizeof(char*));
                applications[count] = (char*)calloc(strlen(cursor) + 1, sizeof(char));
                strcpy(applications[count], cursor);
                count++;
            }
            cursor = strtok(NULL, ",");
        }
        applications = (char**) realloc(applications, (count+1) * sizeof(char*));
        applications[count] = NULL;
    }
    
    // -------------------------------------------------------
    
    hotkeys[0] = {iniHotkey0ButtonA,iniHotkey0ButtonB,0,0,0,0};
    hotkeys[1] = {iniHotkey1ButtonA,iniHotkey1ButtonB,0,0,0,0};
    hotkeys[2] = {iniHotkey2ButtonA,iniHotkey2ButtonB,0,0,0,0};
    hotkeys[3] = {iniHotkey3ButtonA,iniHotkey3ButtonB,0,0,0,0};
    
    // =======================================================
    
    setbuf(stdout, NULL);
    
    system("setterm -clear");
    std::cout << "\x1B[2J\x1B[H";
    cout << "\033[2J\033[1;1H";
    write(1,"\E[H\E[2J",7);
    
    system("setterm -cursor off");
    printf("\e[?25l");
    
    system("setterm -foreground black");
    printf("\033[30m");
    
    // -------------------------------------------------------
    
    Joystick inputs[4];
    int process = 0;
    
    // -------------------------------------------------------
    
    Joystick input0(deviceInput[0]);
    Joystick input1(deviceInput[1]);
    Joystick input2(deviceInput[2]);
    Joystick input3(deviceInput[3]);
    
    inputs[0] = input0;
    inputs[1] = input1;
    inputs[2] = input2;
    inputs[3] = input3;
    
    // -------------------------------------------------------
    
    while (1)
    {
        // -------------------------------------------------------
        
        for (int i = 0; i < 4; i++)
        {
            if (inputs[i].isFound())
            {
                JoystickEvent event;
                if (inputs[i].sample(&event) && event.isButton())
                {
                    if (hotkeys[i].event_a > 0 && hotkeys[i].button_a == event.number)
                    {
                        hotkeys[i].event_a = 0;
                        hotkeys[i].button_a = 0;
                    }
                    if (hotkeys[i].event_b > 0 && hotkeys[i].button_b == event.number)
                    {
                        hotkeys[i].event_b = 0;
                        hotkeys[i].button_b = 0;
                    }
                    if (hotkeys[i].event_a <= 0)
                    {
                        hotkeys[i].event_a = event.value == 0?0:1;
                        hotkeys[i].button_a = event.value == 0?0:event.number;
                    }
                    else if (hotkeys[i].event_b <= 0)
                    {
                        hotkeys[i].event_b = event.value == 0?0:1;
                        hotkeys[i].button_b = event.value == 0?0:event.number;
                    }
                }
            }
            else
            {
                Joystick input(deviceInput[i]);
                inputs[i] = input;
            }
        }
        
        // -------------------------------------------------------
        
        process = 0;
        for (int i = 0; i < 4; i++)
        {
            process += (hotkeys[0].event_a > 0 && hotkeys[0].event_b > 0 && hotkeys[0].button_a == hotkeys[i].hotkey_a && hotkeys[0].button_b == hotkeys[i].hotkey_b)?1:0;
            process += (hotkeys[1].event_a > 0 && hotkeys[1].event_b > 0 && hotkeys[1].button_a == hotkeys[i].hotkey_a && hotkeys[1].button_b == hotkeys[i].hotkey_b)?1:0;
            process += (hotkeys[2].event_a > 0 && hotkeys[2].event_b > 0 && hotkeys[2].button_a == hotkeys[i].hotkey_a && hotkeys[2].button_b == hotkeys[i].hotkey_b)?1:0;
            process += (hotkeys[3].event_a > 0 && hotkeys[3].event_b > 0 && hotkeys[3].button_a == hotkeys[i].hotkey_a && hotkeys[3].button_b == hotkeys[i].hotkey_b)?1:0;
            if (process > 0)
                break;
        }
        
        // -------------------------------------------------------
        
        if (process > 0)
        {
            for (unsigned int i = 0; i < sizeof(applications); i++)
            {
                if (applications[i] != NULL && strlen(applications[i]) > 0)
                    kill(applications[i]);
            }
            for (int i = 0; i < 4; i++)
            {
                hotkeys[i].event_a = 0;
                hotkeys[i].event_b = 0;
                hotkeys[i].button_a = 0;
                hotkeys[i].button_b = 0;
            }
        }
        
        // -------------------------------------------------------
        
        usleep(10000);
    }
    
    // =======================================================
    
    for (unsigned int i = 0; i < sizeof(applications); i++)
    {
        if (applications[i] != NULL && strlen(applications[i]) > 0)
            free(applications[i]);
    }
    free(applications);
    
    // -------------------------------------------------------
    
    return(0);
}

// =======================================================

char* strchar(string* str)
{
    return str->empty() ? NULL : &*str->begin();
}

int kill(const char *name)
{
    pid_t p;
    size_t i, j;
    char *s = (char *) malloc(264);
    char buf[128];
    FILE *st;
    DIR *d = opendir("/proc");
    if (d == NULL) {
        free(s);
        return 0;
    }
    struct dirent *f;
    while ((f = readdir(d)) != NULL) {
        if (f-> d_name[0] == '.') continue;
        for (i = 0; isdigit(f-> d_name[i]); i++);
        if (i < strlen(f-> d_name)) continue;
        strcpy(s, "/proc/");
        strcat(s, f-> d_name);
        strcat(s, "/status");
        st = fopen(s, "r");
        if (st == NULL) {
            closedir(d);
            free(s);
            return 0;
        }
        do {
            if (fgets(buf, 128, st) == NULL) {
                fclose(st);
                closedir(d);
                free(s);
                return 0;
            }
        } while (strncmp(buf, "Name:", 5));
        fclose(st);
        for (j = 5; isspace(buf[j]); j++); *strchr(buf, '\n') = 0;
        if (!strcmp( & (buf[j]), name)) {
            sscanf( & (s[6]), "%d", & p);
            kill(p, SIGQUIT);
        }
    }
    closedir(d);
    free(s);
    return 1;
}
