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

#ifdef __cplusplus
#include <cstdlib>
#else
#include <stdlib.h>
#endif

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <libgen.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "joystick.hh"

#include <algorithm>
#include <iostream>
using namespace std;

#define PATH_MAX 512

int main ( int argc, char** argv )
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
    
    SDL_Event event;
    
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0){
        std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return 1;
    }
    
    SDL_Window *win = SDL_CreateWindow("Hello World!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 600, 600, SDL_WINDOW_SHOWN);
    if (win == NULL){
        std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        return 1;
    }
    
    SDL_ShowCursor(SDL_DISABLE);
    
    // -------------------------------------------------------
    
    char datadir[PATH_MAX];
    memset(datadir, '\0', PATH_MAX);
    ssize_t len = ::readlink("/proc/self/exe", datadir, sizeof(datadir));
    datadir[len] = '\0';
    const char *resource[4] = {"/piscan0.bmp", "/piscan1.bmp", "/piscan2.bmp", "/piscan3.bmp"};
    
    SDL_Surface *bmp[4];
    for (int i=0; i<4; i++)
    {
        strncat(dirname(datadir), resource[i], sizeof(datadir));
        SDL_Surface *file = SDL_LoadBMP(datadir);
        if (file == NULL){
            std::cout << "SDL_LoadBMP Error: " << SDL_GetError() << std::endl;
            SDL_DestroyWindow(win);
            SDL_Quit();
            return 1;
        }
        bmp[i] = file;
    }
    
    // -------------------------------------------------------
    
    SDL_DisplayMode dm;
    if (SDL_GetDesktopDisplayMode(0, &dm) != 0)
    {
        SDL_Log("SDL_GetDesktopDisplayMode failed: %s", SDL_GetError());
        return 1;
    }
    int w, h;
    w = dm.w;
    h = dm.h;
    
    SDL_Rect location;
    location.x = (w - 600) / 2;
    location.y = (h - 600) / 2;
    location.w = 600;
    location.h = 600;
    
    // =======================================================
    
    int fontHeight = 18;
    
    TTF_Init();
    TTF_Font* font = TTF_OpenFont("source.ttf", fontHeight);
    
    SDL_Color fColor;
    fColor.r = 255;
    fColor.g = 255;
    fColor.b = 255;
    
    SDL_Color bColor;
    bColor.r = 0;
    bColor.g = 0;
    bColor.b = 0;
    
    SDL_Rect fontRect;
    fontRect.x = 20;
    fontRect.y = 20;
    
    // -------------------------------------------------------
    
    {
        static struct ifreq ifreqs[32];
        struct ifconf ifconf;
        memset(&ifconf, 0, sizeof(ifconf));
        ifconf.ifc_req = ifreqs;
        ifconf.ifc_len = sizeof(ifreqs);
        int sd = socket(PF_INET, SOCK_STREAM, 0);
        if(sd >= 0)
        {
            int r = ioctl(sd, SIOCGIFCONF, (char *)&ifconf);
            assert(r == 0);
            if (r == 0)
            {
                char interfaces[256];
                for(int i = 0; i < (int)(ifconf.ifc_len/sizeof(struct ifreq)); ++i)
                {
                    memset(interfaces, 0, sizeof(interfaces));
                    sprintf(interfaces, "%s (%s)", inet_ntoa(((struct sockaddr_in *)&ifreqs[i].ifr_addr)->sin_addr), ifreqs[i].ifr_name);
                    {
                        SDL_Surface* fontSurface = TTF_RenderUTF8_Shaded(font, interfaces, fColor, bColor);
                        SDL_BlitSurface(fontSurface, NULL, SDL_GetWindowSurface(win), &fontRect);
                        SDL_UpdateWindowSurface(win);
                        SDL_FreeSurface(fontSurface);
                        fontRect.y += fontHeight + (fontHeight/3);
                    }
                }
            }
        }
        close(sd);
    }
    
    // =======================================================
    
    Joystick input0(0);
    Joystick input1(1);
    Joystick input2(2);
    Joystick input3(3);
    
    // -------------------------------------------------------
    
    int running = 1;
    int joystick = 0;
    
    srand(time(NULL));
    
    int i = 0;
    while (running && !joystick)
    {
        SDL_BlitSurface(bmp[i], 0, SDL_GetWindowSurface(win), &location);
        SDL_UpdateWindowSurface(win);
        
        // -------------------------------------------------------
        
        if (i == 0)
            usleep(rand() % 240000 + 130000);
        else if (i == 1)
            usleep(8000);
        else if (i == 2)
            usleep(8000);
        else
            usleep(rand() % 240000 + 130000);
        
        // -------------------------------------------------------
        
        while (SDL_PollEvent(&event) != 0) {
            
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
                running = 0;
                break;
                
            }
        }
        
        // -------------------------------------------------------
        
        joystick = input0.isFound()?1:0;
        joystick += input1.isFound()?1:0;
        joystick += input2.isFound()?1:0;
        joystick += input3.isFound()?1:0;
        
        // -------------------------------------------------------
        
        i++;
        i %= 4;
        
    }
    
    // =======================================================
    
    for (int i=0; i<4; i++)
        SDL_FreeSurface(bmp[i]);
    
    SDL_DestroyWindow(win);
    SDL_Quit();
    
    return 0;
}


