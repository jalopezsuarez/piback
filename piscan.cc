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
#include <unistd.h>
#include <SDL2/SDL.h>

#include <iostream>
#include "joystick.hh"

#define PATH_MAX 512

int main ( int argc, char** argv )
{
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
    
    char datadir[PATH_MAX];
    
    memset(datadir, '\0', PATH_MAX);
    strncpy(datadir, argv[0], sizeof(datadir));
    strncat(datadir, "/piscan0.bmp", sizeof(datadir));
    SDL_Surface *bmp0 = SDL_LoadBMP(datadir);
    if (bmp0 == NULL){
        std::cout << "SDL_LoadBMP Error: " << SDL_GetError() << std::endl;
        return 1;
    }
    
    memset(datadir, '\0', PATH_MAX);
    strncpy(datadir, argv[0], sizeof(datadir));
    strncat(datadir, "/piscan1.bmp", sizeof(datadir));
    SDL_Surface *bmp1 = SDL_LoadBMP(datadir);
    if (bmp1 == NULL){
        std::cout << "SDL_LoadBMP Error: " << SDL_GetError() << std::endl;
        return 1;
    }
    
    memset(datadir, '\0', PATH_MAX);
    strncpy(datadir, argv[0], sizeof(datadir));
    strncat(datadir, "/piscan2.bmp", sizeof(datadir));
    SDL_Surface *bmp2 = SDL_LoadBMP(datadir);
    if (bmp2 == NULL){
        std::cout << "SDL_LoadBMP Error: " << SDL_GetError() << std::endl;
        return 1;
    }
    
    memset(datadir, '\0', PATH_MAX);
    strncpy(datadir, argv[0], sizeof(datadir));
    strncat(datadir, "/piscan3.bmp", sizeof(datadir));
    SDL_Surface *bmp3 = SDL_LoadBMP(datadir);
    if (bmp3 == NULL){
        std::cout << "SDL_LoadBMP Error: " << SDL_GetError() << std::endl;
        return 1;
    }
    
    SDL_Surface *bmp[4];
    bmp[0] = bmp0;
    bmp[1] = bmp1;
    bmp[2] = bmp2;
    bmp[3] = bmp3;
    
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
        
        Joystick input0(0);
        Joystick input1(1);
        Joystick input2(2);
        Joystick input3(3);
        
        joystick = input0.isFound()?1:0;
        joystick += input1.isFound()?1:0;
        joystick += input2.isFound()?1:0;
        joystick += input3.isFound()?1:0;
        
        // -------------------------------------------------------
        
        i++;
        i %= 4;
        
    }
    
    SDL_FreeSurface( bmp0 );
    SDL_FreeSurface( bmp1 );
    SDL_FreeSurface( bmp2 );
    SDL_FreeSurface( bmp3 );
    
    SDL_DestroyWindow(win);
    SDL_Quit();
    
    return 0;
}
