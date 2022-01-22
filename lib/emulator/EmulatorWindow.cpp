#include "EmulatorWindow.hpp"

#include <SDL_events.h>
#include <cpu/InstructionBinder.hpp>
#include <emulator/WindowInitializationException.hpp>

#include "SDLChip8KeyMapping.hpp"

chip8::EmulatorWindow::EmulatorWindow(const logging::Logger& logger, chip8::Cpu& cpu, const std::string& programName)
    : logger(logger)
    , cpu(cpu)
{
    logger.logDebug("Initializing emulator window...");
    init(programName);
    logger.logDebug("Emulator window initialized.");
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
    while (processEvents())
    {
        loop();
        clearRenderer();
        drawFrame();
        presentFrame();
    }
}

void chip8::EmulatorWindow::init(const std::string& programName)
{
    const std::string windowTitle = programName + " emulator";

    static constexpr size_t WindowWidth = 640;
    static constexpr size_t WindowHeight = 320;

    window =
        SDL_CreateWindow(windowTitle.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WindowWidth, WindowHeight, SDL_WindowFlags::SDL_WINDOW_SHOWN);

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

    if (SDL_RenderSetLogicalSize(renderer, WindowWidth, WindowHeight) != 0)
    {
        throw WindowInitializationException("Rendering initialization failed: " + std::string(SDL_GetError()));
    }

    chip8ScreenTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, cpu.getWidth(), cpu.getHeight());
    if (chip8ScreenTexture == nullptr)
    {
        throw WindowInitializationException("Cannot create emulator screen texture: " + std::string(SDL_GetError()));
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
        case SDL_KEYDOWN:
            if (chip8::SDLChip8KeyMapping.count(event.key.keysym.scancode) != 0)
            {
                const SDL_Scancode& scancode = event.key.keysym.scancode;
                cpu.onKeyPressed(chip8::SDLChip8KeyMapping.find(scancode)->second);
                logger.logDebug("Key %d pressed", chip8::SDLChip8KeyMapping.find(scancode)->second);
            }
            break;
        case SDL_KEYUP:
            const SDL_Scancode& scancode = event.key.keysym.scancode;
            if (chip8::SDLChip8KeyMapping.count(scancode) != 0)
            {
                cpu.onKeyReleased(chip8::SDLChip8KeyMapping.find(scancode)->second);
                logger.logDebug("Key %d released", chip8::SDLChip8KeyMapping.find(event.key.keysym.scancode)->second);
            }
            break;
        }
    }

    return true;
}

void chip8::EmulatorWindow::loop()
{
    cpu.runClockCycle();

    if (cpu.shouldPlayAudio())
    {
        audioController.play();
    }
    else
    {
        audioController.stop();
    }
}

void chip8::EmulatorWindow::clearRenderer()
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
}

void chip8::EmulatorWindow::drawFrame()
{
    std::vector<uint32_t> screenPixels;
    const auto& frameBuffer = cpu.getFrameBuffer();

    screenPixels.reserve(frameBuffer.size());

    for (const bool& frameBufferPixel : frameBuffer)
    {
        screenPixels.emplace_back(frameBufferPixel ? 0xFFFFFFFF : 0xFF000000);
    }

    SDL_UpdateTexture(chip8ScreenTexture, nullptr, screenPixels.data(), cpu.getWidth() * sizeof(uint32_t));
    SDL_RenderCopy(renderer, chip8ScreenTexture, nullptr, nullptr);
}

void chip8::EmulatorWindow::presentFrame()
{
    SDL_RenderPresent(renderer);
}
