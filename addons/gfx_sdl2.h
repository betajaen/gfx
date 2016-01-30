// gfx
// 
// Copyright (c) 2016 Robin Southern -- github.com/betajaen/gfx
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef GFX_SDL2_H
#define GFX_SDL2_H

#include "gfx.h"

#include <SDL2/sdl.h>
#include <SDL2/SDL_syswm.h>

#include <bgfx/bgfxplatform.h>

namespace GFX_NS
{
  class SDL2App
  {
  public:
    friend int ::main(int argc, char *argv[]);
  protected:

    SDL_Window* window;
    bool quit;
    uint32_t windowWidth, windowHeight;

    SDL2App(const char* name = "App", uint32_t windowWidth_ = 1280, uint32_t windowHeight_ = 720, int initFlags = SDL_INIT_VIDEO)
      : windowWidth(windowWidth_),
        windowHeight(windowHeight_)
    {
      SDL_Init(initFlags);
      window = SDL_CreateWindow(name, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, windowWidth, windowHeight, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
      
      SDL_SysWMinfo wmi = { 0 };
      SDL_VERSION(&wmi.version);
      SDL_GetWindowWMInfo(window, &wmi);

      bgfx::PlatformData platformData;
      platformData.ndt = nullptr;
      platformData.nwh = wmi.info.win.window;
      platformData.context = nullptr;
      platformData.backBuffer = nullptr;
      platformData.backBufferDS = nullptr;

      bgfx::setPlatformData(platformData);

      bgfx::init();
      bgfx::reset(windowWidth, windowHeight, BGFX_RESET_VSYNC);

    }

    void run()
    {
      quit = false;
      while(quit == false)
      {
        update();
        draw();
        gfx::frame();
      }
    }

    virtual void setup() {}

    virtual void update() {}

    virtual void draw() {}
  };
}

#define GFX_SDL_APP(APP_NAME) int main(int argc, char *argv[]) { auto app = new APP_NAME(); app->setup(); app->run(); delete app; return 0; }

#endif
