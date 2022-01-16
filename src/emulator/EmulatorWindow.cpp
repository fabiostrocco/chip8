#include "EmulatorWindow.hpp"

#include "WindowInitializationException.hpp"
#include "metadata.hpp"
#include "InstructionBinder.hpp"

chip8::EmulatorWindow::EmulatorWindow(const logging::Logger& logger)
    : logger(logger)
    , diagonal(0)
{
}

chip8::EmulatorWindow::~EmulatorWindow()
{
    if (window != nullptr)
    {
        SDL_DestroyWindow(window);
    }
}

void chip8::EmulatorWindow::run()
{
    logger.logDebug("Initializing emulator window...");
    init();
    logger.logDebug("Emulator window initialized.");

    while (processEvents())
    {
        loop();
        clearRenderer();
        drawFrame();
        presentFrame();
    }
}

void chip8::EmulatorWindow::init()
{
    const std::string programName = chip8::metadata::ProgramName;
    const std::string windowTitle = programName + " emulator";

    window = SDL_CreateWindow(windowTitle.c_str(),
                              SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED,
                              640,
                              480,
                              SDL_WindowFlags::SDL_WINDOW_SHOWN);

    if (window == nullptr)
    {
        throw WindowInitializationException("Cannot create window: " + std::string(SDL_GetError()));
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RendererFlags::SDL_RENDERER_ACCELERATED);

    if (renderer == nullptr)
    {
        logger.logDebug("Cannot use hardware acceleration: using software rendering instead");
        renderer = SDL_CreateRenderer(window, -1, SDL_RendererFlags::SDL_RENDERER_SOFTWARE);
    }
    else
    {
        logger.logDebug("Using hardware acceleration");
    }

    if (renderer == nullptr)
    {
        throw WindowInitializationException("Cannot create window renderer: " + std::string(SDL_GetError()));
    }
}

bool chip8::EmulatorWindow::processEvents()
{
    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_QUIT:
            return false;
        }
    }

    return true;
}

void chip8::EmulatorWindow::loop()
{
    diagonal = (diagonal + 1) % 400;
}

void chip8::EmulatorWindow::clearRenderer()
{
    SDL_SetRenderDrawColor(renderer, 200, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
}

void chip8::EmulatorWindow::drawFrame()
{
    SDL_Rect rect;
    rect.x = diagonal;
    rect.y = diagonal;
    rect.w = 100;
    rect.h = 100;
    SDL_SetRenderDrawColor(renderer, 200, 200, 200, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(renderer, &rect);
}

void chip8::EmulatorWindow::presentFrame()
{
    SDL_RenderPresent(renderer);
}
