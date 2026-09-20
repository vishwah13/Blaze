#include <cmath>
#include "tgaimage.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

constexpr TGAColor white   = {255, 255, 255, 255}; // attention, BGRA order
constexpr TGAColor green   = {  0, 255,   0, 255};
constexpr TGAColor red     = {  0,   0, 255, 255};
constexpr TGAColor blue    = {255, 128,  64, 255};
constexpr TGAColor yellow  = {  0, 200, 255, 255};

void line(int ax, int ay, int bx, int by, TGAImage& framebuffer, TGAColor color) {
    bool steep = std::abs(ax - bx) < std::abs(ay - by);
    if (steep) { // if the line is steep, we transpose the image
        std::swap(ax, ay);
        std::swap(bx, by);
    }
    if (ax > bx) { // make it left−to−right
        std::swap(ax, bx);
        std::swap(ay, by);
    }
    int y = ay;
    int ierror = 0;
    for (int x = ax; x <= bx; x++) {
        if (steep) // if transposed, de−transpose
            framebuffer.set(y, x, color);
        else
            framebuffer.set(x, y, color);
        ierror += 2 * std::abs(by - ay);
        if (ierror > bx - ax) {
            y += by > ay ? 1 : -1;
            ierror -= 2 * (bx - ax);
        }
    }
}

int main(int argc, char** argv) {
    constexpr int width  = 800;
    constexpr int height = 600;

    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("SDL_Init failed: %s",SDL_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Blaze", width, height,0);
    SDL_SetWindowSurfaceVSync(window,1);

    TGAImage framebuffer(width, height, TGAImage::RGBA);

    SDL_Surface* fb = SDL_CreateSurfaceFrom(width, height, SDL_PIXELFORMAT_BGRA32,
        framebuffer.buffer(), width * 4);

    int ax =  7, ay =  3;
    int bx = 12, by = 37;
    int cx = 62, cy = 53;

   

    bool running = true;
    while (running)
    {
        // handeling input
        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_EVENT_QUIT) {
                running = false;
            }
        }

        // doing rendering things from here
        line(ax, ay, bx, by, framebuffer, yellow);
        line(bx, by, cx, cy, framebuffer, blue);
        line(cx, cy, ax, ay, framebuffer, red);

        framebuffer.set(ax, ay, white);
        framebuffer.set(bx, by, white);
        framebuffer.set(cx, cy, white);

        SDL_Surface* win = SDL_GetWindowSurface(window);
        if (!SDL_BlitSurfaceScaled(fb, nullptr, win, nullptr, SDL_SCALEMODE_NEAREST))
        {
            SDL_Log("Blit error: %s", SDL_GetError());
            return 1;
        }

        if (!SDL_UpdateWindowSurface(window))
        {
            SDL_Log("SDL update window surface error: %s", SDL_GetError());
            return 1;
        }
    }

    SDL_DestroySurface(fb);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

