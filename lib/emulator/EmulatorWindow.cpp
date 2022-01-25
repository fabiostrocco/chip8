#include "EmulatorWindow.hpp"

#include <SDL_events.h>
#include <SDL_pixels.h>
#include <SDL_timer.h>
#include <cpu/InstructionBinder.hpp>
#include <emulator/WindowInitializationException.hpp>

#include "EmulatorWindow.hpp"
#include "SDLChip8KeyMapping.hpp"

chip8::EmulatorWindow::EmulatorWindow(const logging::Logger& logger,
                                      chip8::Cpu& cpu,
                                      const std::string& programName,
                                      const std::string& version,
                                      const uint32_t clock)
    : logger(logger)
    , cpu(cpu)
    , clock(clock)
    , needsDraw(false)
    , window(nullptr)
    , renderer(nullptr)
    , chip8ScreenTexture(nullptr)
{
    logger.logInfo("Initializing emulator window...");
    init(programName, version);
    logger.logInfo("Emulator window initialized.");
}

chip8::EmulatorWindow::~EmulatorWindow()
{
    if (window != nullptr)
    {
        SDL_DestroyWindow(window);
    }
}

namespace
{
    Uint32 onInstructionTimerTick(Uint32 interval, void* data);
}

void chip8::EmulatorWindow::run()
{
    // Instruction execution period in seconds T_s = 1 / f (where f is the clock frequency). T in milliseconds is T = (1 / f) * 1000 = 1000 / f
    const uint32_t cpuPeriodMilliseconds = 1000 / clock;

    // Given T, the instruction execution period in milliseconds, set the timer to tick every T milliseconds. However, if T is lower than the minimum
    // allowed timer, set the timer to that value and run multiple instructions at once. This is because setting timers to low intervals (e.g. below 10ms)
    // is not very accurate.
    SDL_AddTimer(cpuPeriodMilliseconds > MinimumInstructionTimerPeriodMilliseconds ? cpuPeriodMilliseconds : MinimumInstructionTimerPeriodMilliseconds,
                 onInstructionTimerTick,
                 &clock);

    while (processEvents())
    {
        if (needsDraw)
        {
            clearRenderer();
            drawFrame();
            presentFrame();
            cpu.onDrawComplete();
            needsDraw = false;
        }
    }
}

void chip8::EmulatorWindow::init(const std::string& programName, const std::string& version)
{
    const std::string windowTitle = programName + " emulator v" + version;

    static constexpr size_t WindowWidth = 640;
    static constexpr size_t WindowHeight = 320;

    window =
        SDL_CreateWindow(windowTitle.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WindowWidth, WindowHeight, SDL_WindowFlags::SDL_WINDOW_RESIZABLE);

    if (window == nullptr)
    {
        throw WindowInitializationException("Cannot create window: " + std::string(SDL_GetError()));
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RendererFlags::SDL_RENDERER_ACCELERATED);

    if (renderer == nullptr)
    {
        logger.logWarning("Cannot use hardware acceleration: using software rendering instead");
        renderer = SDL_CreateRenderer(window, -1, SDL_RendererFlags::SDL_RENDERER_SOFTWARE);
    }
    else
    {
        logger.logInfo("Using hardware acceleration");
    }

    if (renderer == nullptr)
    {
        throw WindowInitializationException("Cannot create window renderer: " + std::string(SDL_GetError()));
    }

    if (SDL_RenderSetLogicalSize(renderer, WindowWidth, WindowHeight) != 0)
    {
        throw WindowInitializationException("Rendering initialization failed: " + std::string(SDL_GetError()));
    }

    chip8ScreenTexture = SDL_CreateTexture(renderer,
                                           SDL_PIXELFORMAT_ARGB8888,
                                           SDL_TextureAccess::SDL_TEXTUREACCESS_STREAMING,
                                           cpu.getWidth(),
                                           cpu.getHeight());

    if (chip8ScreenTexture == nullptr)
    {
        throw WindowInitializationException("Cannot create emulator screen texture: " + std::string(SDL_GetError()));
    }
}

bool chip8::EmulatorWindow::processEvents()
{
    SDL_Event event;
    SDL_Scancode scancode;

    SDL_WaitEvent(&event);

    do
    {
        switch (event.type)
        {
        case SDL_EventType::SDL_QUIT:
            return false;
        case SDL_EventType::SDL_WINDOWEVENT:
            switch (event.window.event)
            {
            case SDL_WindowEventID::SDL_WINDOWEVENT_RESIZED:
                logger.logDebug("Redrawing because window has been resized");
                needsDraw = true;
                break;
            }
        case SDL_KEYDOWN:
            scancode = event.key.keysym.scancode;
            if (chip8::SDLChip8KeyMapping.count(event.key.keysym.scancode) != 0)
            {
                cpu.onKeyPressed(chip8::SDLChip8KeyMapping.find(scancode)->second);
                logger.logDebug("Key %d pressed", chip8::SDLChip8KeyMapping.find(scancode)->second);
            }
            break;
        case SDL_EventType::SDL_KEYUP:
            scancode = event.key.keysym.scancode;
            if (chip8::SDLChip8KeyMapping.count(scancode) != 0)
            {
                cpu.onKeyReleased(chip8::SDLChip8KeyMapping.find(scancode)->second);
                logger.logDebug("Key %d released", chip8::SDLChip8KeyMapping.find(scancode)->second);
            }
            break;
        case SDL_EventType::SDL_USEREVENT:
            runCpuCycle();
            return true;
            break;
        }
    } while (SDL_PollEvent(&event));

    return true;
}

void chip8::EmulatorWindow::runCpuCycle()
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

    if (cpu.getCpuState() == chip8::CpuState::WaitForDraw)
    {
        needsDraw = true;
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
        screenPixels.emplace_back(frameBufferPixel ? 0xFFFFFFF : 0xFF000000);
    }

    SDL_UpdateTexture(chip8ScreenTexture, nullptr, screenPixels.data(), cpu.getWidth() * sizeof(uint32_t));
    SDL_RenderCopy(renderer, chip8ScreenTexture, nullptr, nullptr);
}

void chip8::EmulatorWindow::presentFrame()
{
    SDL_RenderPresent(renderer);
}

namespace
{
    Uint32 onInstructionTimerTick(Uint32 interval, void* data)
    {
        SDL_Event runInstructionEvent;
        SDL_UserEvent runInstructionUserEvent;

        runInstructionUserEvent.type = SDL_USEREVENT;
        runInstructionUserEvent.code = 0;
        runInstructionUserEvent.data1 = nullptr;
        runInstructionUserEvent.data2 = nullptr;

        runInstructionEvent.type = SDL_USEREVENT;
        runInstructionEvent.user = runInstructionUserEvent;

        const uint32_t& clock = *reinterpret_cast<uint32_t*>(data);
        const double instructionsPerMillisecond = static_cast<double>(clock) / 1000.0;
        const uint32_t instructionsPerTimerTick = static_cast<uint32_t>(instructionsPerMillisecond * static_cast<double>(interval));

        for (size_t i = 0; i < instructionsPerTimerTick; i++)
        {
            SDL_PushEvent(&runInstructionEvent);
        }

        return interval;
    }
}