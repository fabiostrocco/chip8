#pragma once

#include <SDL_keyboard.h>
#include <unordered_map>

#include "Key.hpp"

namespace chip8
{
    const std::unordered_map<SDL_Scancode, chip8::Key> SDLChip8KeyMapping{{SDL_Scancode::SDL_SCANCODE_1, Key::Num1},
                                                                          {SDL_Scancode::SDL_SCANCODE_2, Key::Num2},
                                                                          {SDL_Scancode::SDL_SCANCODE_3, Key::Num3},
                                                                          {SDL_Scancode::SDL_SCANCODE_4, Key::DigitC},

                                                                          {SDL_Scancode::SDL_SCANCODE_Q, Key::Num4},
                                                                          {SDL_Scancode::SDL_SCANCODE_W, Key::Num5},
                                                                          {SDL_Scancode::SDL_SCANCODE_E, Key::Num6},
                                                                          {SDL_Scancode::SDL_SCANCODE_R, Key::DigitD},

                                                                          {SDL_Scancode::SDL_SCANCODE_A, Key::Num7},
                                                                          {SDL_Scancode::SDL_SCANCODE_S, Key::Num8},
                                                                          {SDL_Scancode::SDL_SCANCODE_D, Key::Num9},
                                                                          {SDL_Scancode::SDL_SCANCODE_F, Key::DigitE},

                                                                          {SDL_Scancode::SDL_SCANCODE_Z, Key::DigitA},
                                                                          {SDL_Scancode::SDL_SCANCODE_X, Key::Num0},
                                                                          {SDL_Scancode::SDL_SCANCODE_C, Key::DigitB},
                                                                          {SDL_Scancode::SDL_SCANCODE_V, Key::DigitF}};
}