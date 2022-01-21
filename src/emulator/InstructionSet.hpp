#pragma once

#include <functional>
#include <type_traits>

#include "InstructionBinder.hpp"

namespace chip8
{
    class InstructionSet
    {
      public:
        InstructionSet(std::function<void(uint16_t addr)> sys_addr_callback,
                       std::function<void()> cls_callback,
                       std::function<void()> ret_callback,
                       std::function<void(uint16_t addr)> jp_addr_callback,
                       std::function<void(uint16_t addr)> call_addr_callback,
                       std::function<void(binding::MatchingPatternType Vx, uint8_t byte)> se_vx_byte_callback,
                       std::function<void(binding::MatchingPatternType Vx, uint8_t byte)> sne_vx_byte_callback,
                       std::function<void(binding::MatchingPatternType Vx, chip8::binding::MatchingPatternType Vy)> se_vx_vy_callback,
                       std::function<void(binding::MatchingPatternType Vx, uint8_t byte)> ld_vx_byte_callback,
                       std::function<void(binding::MatchingPatternType Vx, uint8_t byte)> add_vx_byte_callback,
                       std::function<void(binding::MatchingPatternType Vx, chip8::binding::MatchingPatternType Vy)> ld_vx_vy_callback,
                       std::function<void(binding::MatchingPatternType Vx, chip8::binding::MatchingPatternType Vy)> or_vx_vy_callback,
                       std::function<void(binding::MatchingPatternType Vx, chip8::binding::MatchingPatternType Vy)> and_vx_vy_callback,
                       std::function<void(binding::MatchingPatternType Vx, binding::MatchingPatternType Vy)> xor_vx_vy_callback,
                       std::function<void(binding::MatchingPatternType Vx, binding::MatchingPatternType Vy)> add_vx_vy_callback,
                       std::function<void(binding::MatchingPatternType Vx, binding::MatchingPatternType Vy)> sub_vx_vy_callback,
                       std::function<void(binding::MatchingPatternType Vx, binding::MatchingPatternType Vy)> shr_vx_vy_callback,
                       std::function<void(binding::MatchingPatternType Vx, binding::MatchingPatternType Vy)> subn_vx_vy_callback,
                       std::function<void(binding::MatchingPatternType Vx, binding::MatchingPatternType Vy)> shl_vx_vy_callback,
                       std::function<void(binding::MatchingPatternType Vx, binding::MatchingPatternType Vy)> sne_vx_vy_callback,
                       std::function<void(uint16_t addr)> ld_i_addr_callback,
                       std::function<void(uint16_t addr)> jp_v0_addr_callback,
                       std::function<void(binding::MatchingPatternType Vx, uint8_t byte)> rnd_vx_byte_callback,
                       std::function<void(binding::MatchingPatternType Vx, binding::MatchingPatternType Vy, binding::MatchingPatternType nibble)>
                           drw_vx_vy_nibble_callback,
                       std::function<void(binding::MatchingPatternType Vx)> skp_vx_callback,
                       std::function<void(binding::MatchingPatternType Vx)> sknp_vx_callback,
                       std::function<void(binding::MatchingPatternType Vx)> ld_vx_dt_callback,
                       std::function<void(binding::MatchingPatternType Vx)> ld_vx_k_callback,
                       std::function<void(binding::MatchingPatternType Vx)> ld_dt_vx_callback,
                       std::function<void(binding::MatchingPatternType Vx)> ld_st_vx_callback,
                       std::function<void(binding::MatchingPatternType Vx)> add_i_vx_callback,
                       std::function<void(binding::MatchingPatternType Vx)> ld_f_vx_callback,
                       std::function<void(binding::MatchingPatternType Vx)> ld_b_vx_callback,
                       std::function<void(binding::MatchingPatternType Vx)> ld_idata_vx_callback,
                       std::function<void(binding::MatchingPatternType Vx)> ld_vx_idata_callback,
                       std::function<void()> unmatchedInstructionCallback)
        {
            // Instruction opcode->callback mapping.The definition below should match what in doc/Chip8.pdf at page 4
            // For example createInstruction<0x0, n, n, n> for sys_addr_callback reads as
            // Instruction SYS addr is in the format 0nnn where nnn is a 16-bit address

            createInstruction<0x0, 0x0, 0xe, 0x0>(cls_callback);
            createInstruction<0x0, 0x0, 0xe, 0xe>(ret_callback);
            createInstruction<0x0, n, n, n>(bindAddressCallback(sys_addr_callback));
            createInstruction<0x1, n, n, n>(bindAddressCallback(jp_addr_callback));
            createInstruction<0x2, n, n, n>(bindAddressCallback(call_addr_callback));
            createInstruction<0x3, x, k, k>(bindRegisterCallback(se_vx_byte_callback));
            createInstruction<0x4, x, k, k>(bindRegisterCallback(sne_vx_byte_callback));
            createInstruction<0x5, x, y, 0x0>(se_vx_byte_callback);
            createInstruction<0x6, x, k, k>(bindRegisterCallback(ld_vx_byte_callback));
            createInstruction<0x7, x, k, k>(bindRegisterCallback(add_vx_byte_callback));
            createInstruction<0x8, x, y, 0x0>(ld_vx_vy_callback);
            createInstruction<0x8, x, y, 0x1>(or_vx_vy_callback);
            createInstruction<0x8, x, y, 0x2>(and_vx_vy_callback);
            createInstruction<0x8, x, y, 0x3>(xor_vx_vy_callback);
            createInstruction<0x8, x, y, 0x4>(add_vx_vy_callback);
            createInstruction<0x8, x, y, 0x5>(sub_vx_vy_callback);
            createInstruction<0x8, x, y, 0x6>(shr_vx_vy_callback);
            createInstruction<0x8, x, y, 0x7>(subn_vx_vy_callback);
            createInstruction<0x8, x, y, 0xe>(shl_vx_vy_callback);
            createInstruction<0x9, x, y, 0x0>(sne_vx_vy_callback);
            createInstruction<0xa, n, n, n>(bindAddressCallback(ld_i_addr_callback));
            createInstruction<0xb, n, n, n>(bindAddressCallback(jp_v0_addr_callback));
            createInstruction<0xc, x, k, k>(bindRegisterCallback(rnd_vx_byte_callback));
            createInstruction<0xd, x, y, n>(drw_vx_vy_nibble_callback);
            createInstruction<0xe, x, 0x9, 0xe>(skp_vx_callback);
            createInstruction<0xe, x, 0xa, 0x1>(sknp_vx_callback);
            createInstruction<0xf, x, 0x0, 0x7>(ld_vx_dt_callback);
            createInstruction<0xf, x, 0x0, 0xa>(ld_vx_k_callback);
            createInstruction<0xf, x, 0x1, 0x5>(ld_dt_vx_callback);
            createInstruction<0xf, x, 0x1, 0x8>(ld_st_vx_callback);
            createInstruction<0xf, x, 0x1, 0xe>(add_i_vx_callback);
            createInstruction<0xf, x, 0x2, 0x9>(ld_f_vx_callback);
            createInstruction<0xf, x, 0x3, 0x3>(ld_b_vx_callback);
            createInstruction<0xf, x, 0x5, 0x5>(ld_idata_vx_callback);
            createInstruction<0xf, x, 0x6, 0x5>(ld_vx_idata_callback);
            this->unmatchedInstructionCallback = unmatchedInstructionCallback;
        }

        void matchInstruction(uint8_t byte1, uint8_t byte2) const
        {
            for (auto& instruction : instructions)
            {
                if (instruction->match(byte1, byte2))
                {
                    return;
                }
            }

            unmatchedInstructionCallback();
        }

        template <binding::MatchingPatternType u1,
                  binding::MatchingPatternType u2,
                  binding::MatchingPatternType u3,
                  binding::MatchingPatternType u4,
                  typename T>
        void createInstruction(T f)
        {
            auto instruction = std::make_unique<chip8::InstructionBinder<u1, u2, u3, u4>>(f);
            instructions.push_back(std::move(instruction));
        }

      private:
        inline std::function<void(binding::MatchingPatternType, binding::MatchingPatternType, binding::MatchingPatternType)>
        bindAddressCallback(std::function<void(uint16_t)> f)
        {
            return [=](const binding::MatchingPatternType n1, const binding::MatchingPatternType n2, const binding::MatchingPatternType n3)
            {
                f((static_cast<uint16_t>(n1) << 8) | (static_cast<uint16_t>(n2) << 4) | static_cast<uint16_t>(n3));
            };
        }

        inline std::function<void(binding::MatchingPatternType, binding::MatchingPatternType, binding::MatchingPatternType)>
        bindRegisterCallback(std::function<void(binding::MatchingPatternType, uint8_t)> f)
        {
            return [=](const binding::MatchingPatternType n1, const binding::MatchingPatternType n2, const binding::MatchingPatternType n3)
            {
                f(n1, static_cast<uint8_t>((n2 << 4) | n3));
            };
        }

        std::vector<std::unique_ptr<chip8::IBinder>> instructions;
        std::function<void()> unmatchedInstructionCallback;

        static constexpr binding::MatchingPatternType n = binding::placeholder, k = binding::placeholder, x = binding::placeholder, y = binding::placeholder;
    };
}