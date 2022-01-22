#pragma once

#include <array>
#include <istream>
#include <logging/Logger.hpp>
#include <memory>
#include <random>

#include "IGpu.hpp"
#include "ITimer.hpp"
#include "InstructionSet.hpp"
#include "Key.hpp"
#include "Registers.hpp"

namespace chip8
{
    class Cpu
    {
      public:
        Cpu(const logging::Logger& logger, chip8::IGpu& gpu, chip8::ITimer& soundTimer, chip8::ITimer& delayTimer);

        // TODO: probably remove ICpu
        void boot(std::istream& stream);
        void runClockCycle();
        void onKeyPressed(const chip8::Key key);
        void onKeyReleased(const chip8::Key key);
        bool shouldPlayAudio() const;

        size_t getWidth() const;
        size_t getHeight() const;
        const std::vector<bool>& getFrameBuffer() const;

        chip8::Registers& getRegisters();

      private:
        static constexpr size_t MemorySize = 4 * 1204; // 4KB
        static constexpr size_t StackSize = 64;
        static constexpr size_t ProgramStartLocation = 0x200;
        static constexpr size_t VF = 0xf;

        const logging::Logger& logger;
        chip8::InstructionSet instructions;

        // ==================== CPU compontents ====================
        chip8::Registers registers;
        std::array<uint8_t, MemorySize> memory;
        std::array<uint8_t, StackSize> stack;
        chip8::IGpu& gpu;
        chip8::ITimer& soundTimer;
        chip8::ITimer& delayTimer;
        std::array<bool, 16> keyPressedStatus;
        std::random_device randomDevice;
        std::default_random_engine randomEngine;
        // random generator for uint8_t. Note: it cannot be uniform_int_distribution<uint8_t> because the effect is undefined for that
        // type (see C++ specifications).
        std::uniform_int_distribution<unsigned long> uniformDistrubution;
        bool playAudioFlag;

        // ==================== Private utility functions ====================
      private:
        void initializeRegisters();
        void initializeMemory();
        void validateMemoryWrite(uint16_t address);
        void validateStackWrite(uint8_t address);
        void validateMemoryRead(uint16_t address);
        void validateStackRead(uint8_t address);

        // ==================== Instruction handling ====================
        void execute_sys_addr(uint16_t addr);
        void execute_cls();
        void execute_ret();
        void execute_jp_addr(uint16_t addr);
        void execute_call_addr(uint16_t addr);
        void execute_se_vx_byte(binding::MatchingPatternType Vx, uint8_t byte);
        void execute_sne_vx_byte(binding::MatchingPatternType Vx, uint8_t byte);
        void execute_se_vx_vy(binding::MatchingPatternType Vx, binding::MatchingPatternType Vy);
        void execute_ld_vx_byte(binding::MatchingPatternType Vx, uint8_t byte);
        void execute_add_vx_byte(binding::MatchingPatternType Vx, uint8_t byte);
        void execute_ld_vx_vy(binding::MatchingPatternType Vx, binding::MatchingPatternType Vy);
        void execute_or_vx_vy(binding::MatchingPatternType Vx, binding::MatchingPatternType Vy);
        void execute_and_vx_vy(binding::MatchingPatternType Vx, binding::MatchingPatternType Vy);
        void execute_xor_vx_vy(binding::MatchingPatternType Vx, binding::MatchingPatternType Vy);
        void execute_add_vx_vy(binding::MatchingPatternType Vx, binding::MatchingPatternType Vy);
        void execute_sub_vx_vy(binding::MatchingPatternType Vx, binding::MatchingPatternType Vy);
        void execute_shr_vx_vy(binding::MatchingPatternType Vx, binding::MatchingPatternType Vy);
        void execute_subn_vx_vy(binding::MatchingPatternType Vx, binding::MatchingPatternType Vy);
        void execute_shl_vx_vy(binding::MatchingPatternType Vx, binding::MatchingPatternType Vy);
        void execute_sne_vx_vy(binding::MatchingPatternType Vx, binding::MatchingPatternType Vy);
        void execute_ld_i_addr(uint16_t addr);
        void execute_jp_v0_addr(uint16_t addr);
        void execute_rnd_vx_byte(binding::MatchingPatternType Vx, uint8_t byte);
        void execute_drw_vx_vy_nibble(binding::MatchingPatternType Vx, binding::MatchingPatternType Vy, binding::MatchingPatternType nibble);
        void execute_skp_vx(binding::MatchingPatternType Vx);
        void execute_sknp_vx(binding::MatchingPatternType Vx);
        void execute_ld_vx_dt(binding::MatchingPatternType Vx);
        void execute_ld_vx_k(binding::MatchingPatternType Vx);
        void execute_ld_dt_vx(binding::MatchingPatternType Vx);
        void execute_ld_st_vx(binding::MatchingPatternType Vx);
        void execute_add_i_vx(binding::MatchingPatternType Vx);
        void execute_ld_f_vx(binding::MatchingPatternType Vx);
        void execute_ld_b_vx(binding::MatchingPatternType Vx);
        void execute_ld_idata_vx(binding::MatchingPatternType Vx);
        void execute_ld_vx_idata(binding::MatchingPatternType Vx);
        void onUnmatchedInstruction();
    };
}
