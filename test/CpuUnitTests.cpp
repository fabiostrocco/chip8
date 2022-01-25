#include <cpu/Cpu.hpp>
#include <cpu/CpuExecutionException.hpp>
#include <cpu/IGpu.hpp>
#include <cpu/ITimer.hpp>
#include <cpu/Registers.hpp>
#include <functional>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <iostream>
#include <logging/Logger.hpp>
#include <sstream>
#include <tuple>

class Gpu : public chip8::IGpu
{
  public:
    MOCK_METHOD(void, clear, ());
    MOCK_METHOD(bool, setSprite, (const size_t x, const size_t y, const std::vector<uint8_t>& sprite));
    MOCK_METHOD(const std::vector<bool>&, getFrameBuffer, (), (const));
    MOCK_METHOD(size_t, getWidth, ());
    MOCK_METHOD(size_t, getHeight, ());
};

class Timer : public chip8::ITimer
{
  public:
    void setValue(uint8_t value) override
    {
        counter = value;
    }

    uint8_t getValue() const
    {
        return counter;
    }

  private:
    uint8_t counter = 0;

    MOCK_METHOD(void, updateValue, ());
};

class Logger : public logging::Logger
{
  public:
    Logger()
        : logging::Logger(logging::Severity::Debug)
    {
    }

  protected:
    void logInternal(const logging::Severity severity, const char* format, ...) const override
    {
        va_list args;
        va_start(args, format);
        vprintf(format, args);
        va_end(args);
    }
};

void initTest(uint8_t byte1, uint8_t byte2, chip8::Cpu& cpu)
{
    std::stringstream s;
    s.write(reinterpret_cast<const char*>(&byte1), sizeof(unsigned char));
    s.write(reinterpret_cast<const char*>(&byte2), sizeof(unsigned char));
    cpu.boot(s);
}

namespace chip8::unit_tests
{
    TEST(CpuUnitTests, sys_addr_throws_exception)
    {
        Logger logger;
        Gpu gpu;
        Timer soundTimer;
        Timer delayTimer;
        chip8::Cpu cpu(logger, gpu, soundTimer, delayTimer);
        chip8::Registers& registers(cpu.getRegisters());

        initTest(0x00, 0x00, cpu); // SYS 0

        try
        {
            cpu.runClockCycle();
            EXPECT_TRUE(false);
        }
        catch (chip8::CpuExecutionException& e)
        {
            EXPECT_STREQ(e.what(), "Unsupported legacy SYS instruction");
        }
    }

    TEST(CpuUnitTests, cls_executes_successfuly)
    {
        Logger logger;
        Gpu gpu;
        Timer soundTimer;
        Timer delayTimer;
        chip8::Cpu cpu(logger, gpu, soundTimer, delayTimer);
        chip8::Registers& registers(cpu.getRegisters());

        initTest(0x00, 0xe0, cpu); // cls
        EXPECT_CALL(gpu, clear).Times(1);
        cpu.runClockCycle();
        EXPECT_EQ(*registers.PC, 0x202);
    }

    TEST(CpuUnitTests, ret_executes_successfuly)
    {
        Logger logger;
        Gpu gpu;
        Timer soundTimer;
        Timer delayTimer;
        chip8::Cpu cpu(logger, gpu, soundTimer, delayTimer);
        chip8::Registers& registers(cpu.getRegisters());
        initTest(0x00, 0xee, cpu); // ret
        registers.SP = 2;
        cpu.runClockCycle();
        EXPECT_EQ(*registers.PC, 0x0000);
        EXPECT_EQ(*registers.SP, 0x00);
    }

    TEST(CpuUnitTests, ret_executes_stack_underflow)
    {
        Logger logger;
        Gpu gpu;
        Timer soundTimer;
        Timer delayTimer;
        chip8::Cpu cpu(logger, gpu, soundTimer, delayTimer);
        chip8::Registers& registers(cpu.getRegisters());
        initTest(0x00, 0xee, cpu); // ret

        try
        {
            cpu.runClockCycle();
            EXPECT_TRUE(false);
        }
        catch (chip8::CpuExecutionException& e)
        {
            EXPECT_STREQ(e.what(), "stack underflow");
        }
    }

    TEST(CpuUnitTests, jp_addr_executes_successfuly)
    {
        Logger logger;
        Gpu gpu;
        Timer soundTimer;
        Timer delayTimer;
        chip8::Cpu cpu(logger, gpu, soundTimer, delayTimer);
        chip8::Registers& registers(cpu.getRegisters());
        initTest(0x15, 0x67, cpu); // jp 0x567
        cpu.runClockCycle();
        EXPECT_EQ(*registers.PC, 0x567);
    }

    TEST(CpuUnitTests, call_addr_executes_successfuly)
    {
        Logger logger;
        Gpu gpu;
        Timer soundTimer;
        Timer delayTimer;
        chip8::Cpu cpu(logger, gpu, soundTimer, delayTimer);
        chip8::Registers& registers(cpu.getRegisters());
        initTest(0x25, 0x67, cpu); // call 0x567
        cpu.runClockCycle();
        EXPECT_EQ(*registers.PC, 0x567);
        EXPECT_EQ(*registers.SP, 2);
    }

    TEST(CpuUnitTests, call_addr_executes_stack_overflow)
    {
        Logger logger;
        Gpu gpu;
        Timer soundTimer;
        Timer delayTimer;
        chip8::Cpu cpu(logger, gpu, soundTimer, delayTimer);
        chip8::Registers& registers(cpu.getRegisters());
        initTest(0x25, 0x67, cpu); // call 0x567
        registers.SP = 63;

        try
        {
            cpu.runClockCycle();
            EXPECT_TRUE(false);
        }
        catch (chip8::CpuExecutionException& e)
        {
            EXPECT_STREQ(e.what(), "stack overflow");
        }
    }

    TEST(CpuUnitTests, se_vx_byte_not_skip)
    {
        Logger logger;
        Gpu gpu;
        Timer soundTimer;
        Timer delayTimer;
        chip8::Cpu cpu(logger, gpu, soundTimer, delayTimer);
        chip8::Registers& registers(cpu.getRegisters());
        initTest(0x34, 0x56, cpu); // se v4, 0x56
        cpu.runClockCycle();
        EXPECT_EQ(*registers.PC, 0x202);
    }

    TEST(CpuUnitTests, se_vx_byte_skip)
    {
        Logger logger;
        Gpu gpu;
        Timer soundTimer;
        Timer delayTimer;
        chip8::Cpu cpu(logger, gpu, soundTimer, delayTimer);
        chip8::Registers& registers(cpu.getRegisters());
        initTest(0x34, 0x00, cpu); // se v4, 0x00
        cpu.runClockCycle();
        EXPECT_EQ(*registers.PC, 0x204);
    }

    TEST(CpuUnitTests, sne_vx_byte_not_skip)
    {
        Logger logger;
        Gpu gpu;
        Timer soundTimer;
        Timer delayTimer;
        chip8::Cpu cpu(logger, gpu, soundTimer, delayTimer);
        chip8::Registers& registers(cpu.getRegisters());
        initTest(0x44, 0x00, cpu); // sne v4, 0x00
        cpu.runClockCycle();
        EXPECT_EQ(*registers.PC, 0x202);
    }

    TEST(CpuUnitTests, sne_vx_byte_skip)
    {
        Logger logger;
        Gpu gpu;
        Timer soundTimer;
        Timer delayTimer;
        chip8::Cpu cpu(logger, gpu, soundTimer, delayTimer);
        chip8::Registers& registers(cpu.getRegisters());
        initTest(0x44, 0x56, cpu); // sne v4, 0x56
        cpu.runClockCycle();
        EXPECT_EQ(*registers.PC, 0x204);
    }

    TEST(CpuUnitTests, se_vx_vy_not_skip)
    {
        Logger logger;
        Gpu gpu;
        Timer soundTimer;
        Timer delayTimer;
        chip8::Cpu cpu(logger, gpu, soundTimer, delayTimer);
        chip8::Registers& registers(cpu.getRegisters());
        initTest(0x54, 0x50, cpu); // se v4, v5
        registers.V[5] = 1;
        cpu.runClockCycle();
        EXPECT_EQ(*registers.PC, 0x202);
    }

    TEST(CpuUnitTests, se_vx_vy_byte_skip)
    {
        Logger logger;
        Gpu gpu;
        Timer soundTimer;
        Timer delayTimer;
        chip8::Cpu cpu(logger, gpu, soundTimer, delayTimer);
        chip8::Registers& registers(cpu.getRegisters());
        initTest(0x45, 0x55, cpu); // se v4, v5
        cpu.runClockCycle();
        EXPECT_EQ(*registers.PC, 0x204);
    }

    TEST(CpuUnitTests, ld_vx_byte_executes_correctly)
    {
        Logger logger;
        Gpu gpu;
        Timer soundTimer;
        Timer delayTimer;
        chip8::Cpu cpu(logger, gpu, soundTimer, delayTimer);
        chip8::Registers& registers(cpu.getRegisters());
        initTest(0x62, 0x54, cpu); // ld V2, 0x54
        cpu.runClockCycle();
        EXPECT_EQ(*registers.PC, 0x202);
        EXPECT_EQ(registers.V[2], 0x54);
    }

    TEST(CpuUnitTests, add_vx_byte_executes_correctly)
    {
        Logger logger;
        Gpu gpu;
        Timer soundTimer;
        Timer delayTimer;
        chip8::Cpu cpu(logger, gpu, soundTimer, delayTimer);
        chip8::Registers& registers(cpu.getRegisters());
        initTest(0x72, 13, cpu); // add V2, 13
        registers.V[2] = 10;
        cpu.runClockCycle();
        EXPECT_EQ(*registers.PC, 0x202);
        EXPECT_EQ(registers.V[2], 23);
        EXPECT_FALSE(registers.V[0xf]);
    }

    TEST(CpuUnitTests, ld_vx_vy_executes_correctly)
    {
        Logger logger;
        Gpu gpu;
        Timer soundTimer;
        Timer delayTimer;
        chip8::Cpu cpu(logger, gpu, soundTimer, delayTimer);
        chip8::Registers& registers(cpu.getRegisters());
        initTest(0x82, 0x30, cpu); // ld v2, v3
        registers.V[3] = 10;
        cpu.runClockCycle();
        EXPECT_EQ(*registers.PC, 0x202);
        EXPECT_EQ(registers.V[2], 10);
        EXPECT_FALSE(registers.V[0xf]);
    }

    TEST(CpuUnitTests, or_vx_vy_executes_correctly)
    {
        Logger logger;
        Gpu gpu;
        Timer soundTimer;
        Timer delayTimer;
        chip8::Cpu cpu(logger, gpu, soundTimer, delayTimer);
        chip8::Registers& registers(cpu.getRegisters());
        initTest(0x82, 0x31, cpu); // or v2, v3
        registers.V[2] = 0x0f;
        registers.V[3] = 0xf0;
        cpu.runClockCycle();
        EXPECT_EQ(*registers.PC, 0x202);
        EXPECT_EQ(registers.V[2], 0xff);
        EXPECT_FALSE(registers.V[0xf]);
    }

    TEST(CpuUnitTests, and_vx_vy_executes_correctly)
    {
        Logger logger;
        Gpu gpu;
        Timer soundTimer;
        Timer delayTimer;
        chip8::Cpu cpu(logger, gpu, soundTimer, delayTimer);
        chip8::Registers& registers(cpu.getRegisters());
        initTest(0x82, 0x32, cpu); // and v2, v3
        registers.V[2] = 0x0f;
        registers.V[3] = 0xf0;
        cpu.runClockCycle();
        EXPECT_EQ(*registers.PC, 0x202);
        EXPECT_EQ(registers.V[2], 0x00);
        EXPECT_FALSE(registers.V[0xf]);
    }

    TEST(CpuUnitTests, xor_vx_vy_executes_correctly)
    {
        Logger logger;
        Gpu gpu;
        Timer soundTimer;
        Timer delayTimer;
        chip8::Cpu cpu(logger, gpu, soundTimer, delayTimer);
        chip8::Registers& registers(cpu.getRegisters());
        initTest(0x82, 0x33, cpu); // xor v2, v3
        registers.V[2] = 0x0f;
        registers.V[3] = 0xff;
        cpu.runClockCycle();
        EXPECT_EQ(*registers.PC, 0x202);
        EXPECT_EQ(registers.V[2], 0xf0);
        EXPECT_FALSE(registers.V[0xf]);
    }

    TEST(CpuUnitTests, xor_vx_vy_executes_correctly_no_carry)
    {
        Logger logger;
        Gpu gpu;
        Timer soundTimer;
        Timer delayTimer;
        chip8::Cpu cpu(logger, gpu, soundTimer, delayTimer);
        chip8::Registers& registers(cpu.getRegisters());
        initTest(0x82, 0x34, cpu); // add v2, v3
        registers.V[2] = 10;
        registers.V[3] = 20;
        cpu.runClockCycle();
        EXPECT_EQ(*registers.PC, 0x202);
        EXPECT_EQ(registers.V[2], 30);
        EXPECT_FALSE(registers.V[0xf]);
    }

    TEST(CpuUnitTests, sub_vx_vy_executes_correctly_no_borrow)
    {
        Logger logger;
        Gpu gpu;
        Timer soundTimer;
        Timer delayTimer;
        chip8::Cpu cpu(logger, gpu, soundTimer, delayTimer);
        chip8::Registers& registers(cpu.getRegisters());
        initTest(0x82, 0x35, cpu); // sub v2, v3
        registers.V[2] = 30;
        registers.V[3] = 20;
        cpu.runClockCycle();
        EXPECT_EQ(*registers.PC, 0x202);
        EXPECT_EQ(registers.V[2], 10);
        EXPECT_TRUE(registers.V[0xf]);
    }

    TEST(CpuUnitTests, sub_vx_vy_executes_correctly_borrow)
    {
        Logger logger;
        Gpu gpu;
        Timer soundTimer;
        Timer delayTimer;
        chip8::Cpu cpu(logger, gpu, soundTimer, delayTimer);
        chip8::Registers& registers(cpu.getRegisters());
        initTest(0x82, 0x35, cpu); // sub v2, v3
        const uint8_t v2 = 20;
        const uint8_t v3 = 30;
        const uint8_t result = v2 - v3;
        registers.V[2] = v2;
        registers.V[3] = v3;
        cpu.runClockCycle();
        EXPECT_EQ(*registers.PC, 0x202);
        EXPECT_EQ(registers.V[2], result);
        EXPECT_FALSE(registers.V[0xf]);
    }

    TEST(CpuUnitTests, shr_vx_vy_even_executes_correctly_vf_not_set)
    {
        Logger logger;
        Gpu gpu;
        Timer soundTimer;
        Timer delayTimer;
        chip8::Cpu cpu(logger, gpu, soundTimer, delayTimer);
        chip8::Registers& registers(cpu.getRegisters());
        initTest(0x82, 0x06, cpu); // shr v2
        registers.V[2] = 0b00000010;
        cpu.runClockCycle();
        EXPECT_EQ(*registers.PC, 0x202);
        EXPECT_EQ(registers.V[2], 0b00000001);
        EXPECT_FALSE(registers.V[0xf]);
    }

    TEST(CpuUnitTests, shr_vx_vy_odd_executes_correctly_vf_set)
    {
        Logger logger;
        Gpu gpu;
        Timer soundTimer;
        Timer delayTimer;
        chip8::Cpu cpu(logger, gpu, soundTimer, delayTimer);
        chip8::Registers& registers(cpu.getRegisters());
        initTest(0x82, 0x06, cpu); // shr v2
        registers.V[2] = 0b00000101;
        cpu.runClockCycle();
        EXPECT_EQ(*registers.PC, 0x202);
        EXPECT_EQ(registers.V[2], 0b00000010);
        EXPECT_TRUE(registers.V[0xf]);
    }

    TEST(CpuUnitTests, subn_vx_vy_executes_correctly_no_borrow)
    {
        Logger logger;
        Gpu gpu;
        Timer soundTimer;
        Timer delayTimer;
        chip8::Cpu cpu(logger, gpu, soundTimer, delayTimer);
        chip8::Registers& registers(cpu.getRegisters());
        initTest(0x82, 0x37, cpu); // subn v2, v3
        registers.V[2] = 20;
        registers.V[3] = 30;
        cpu.runClockCycle();
        EXPECT_EQ(*registers.PC, 0x202);
        EXPECT_EQ(registers.V[2], 10);
        EXPECT_TRUE(registers.V[0xf]);
    }

    TEST(CpuUnitTests, subn_vx_vy_executes_correctly_borrow)
    {
        Logger logger;
        Gpu gpu;
        Timer soundTimer;
        Timer delayTimer;
        chip8::Cpu cpu(logger, gpu, soundTimer, delayTimer);
        chip8::Registers& registers(cpu.getRegisters());
        initTest(0x82, 0x37, cpu); // subn v2, v3
        const uint8_t v2 = 30;
        const uint8_t v3 = 20;
        const uint8_t result = v3 - v2;
        registers.V[2] = v2;
        registers.V[3] = v3;
        cpu.runClockCycle();
        EXPECT_EQ(*registers.PC, 0x202);
        EXPECT_EQ(registers.V[2], result);
        EXPECT_FALSE(registers.V[0xf]);
    }

    TEST(CpuUnitTests, shl_vx_vy_first_bit_zero_executes_correctly_vf_not_set)
    {
        Logger logger;
        Gpu gpu;
        Timer soundTimer;
        Timer delayTimer;
        chip8::Cpu cpu(logger, gpu, soundTimer, delayTimer);
        chip8::Registers& registers(cpu.getRegisters());
        initTest(0x82, 0x0e, cpu); // shl v2
        registers.V[2] = 0b01000011;
        cpu.runClockCycle();
        EXPECT_EQ(*registers.PC, 0x202);
        EXPECT_EQ(registers.V[2], 0b10000110);
        EXPECT_FALSE(registers.V[0xf]);
    }

    TEST(CpuUnitTests, shl_vx_vy_first_bit_one_executes_correctly_vf_set)
    {
        Logger logger;
        Gpu gpu;
        Timer soundTimer;
        Timer delayTimer;
        chip8::Cpu cpu(logger, gpu, soundTimer, delayTimer);
        chip8::Registers& registers(cpu.getRegisters());
        initTest(0x82, 0x0e, cpu); // shl v2
        registers.V[2] = 0b10000010;
        cpu.runClockCycle();
        EXPECT_EQ(*registers.PC, 0x202);
        EXPECT_EQ(registers.V[2], 0b00000100);
        EXPECT_TRUE(registers.V[0xf]);
    }

    TEST(CpuUnitTests, sne_vx_vy_skip)
    {
        Logger logger;
        Gpu gpu;
        Timer soundTimer;
        Timer delayTimer;
        chip8::Cpu cpu(logger, gpu, soundTimer, delayTimer);
        chip8::Registers& registers(cpu.getRegisters());
        initTest(0x92, 0x30, cpu); // sne v2
        registers.V[2] = 10;
        registers.V[3] = 20;
        cpu.runClockCycle();
        EXPECT_EQ(*registers.PC, 0x204);
    }

    TEST(CpuUnitTests, sne_vx_vy_no_skip)
    {
        Logger logger;
        Gpu gpu;
        Timer soundTimer;
        Timer delayTimer;
        chip8::Cpu cpu(logger, gpu, soundTimer, delayTimer);
        chip8::Registers& registers(cpu.getRegisters());
        initTest(0x92, 0x30, cpu); // sne v2
        registers.V[2] = 10;
        registers.V[3] = 10;
        cpu.runClockCycle();
        EXPECT_EQ(*registers.PC, 0x202);
    }

    TEST(CpuUnitTests, ld_i_addr_executes_correctly)
    {
        Logger logger;
        Gpu gpu;
        Timer soundTimer;
        Timer delayTimer;
        chip8::Cpu cpu(logger, gpu, soundTimer, delayTimer);
        chip8::Registers& registers(cpu.getRegisters());
        initTest(0xa1, 0x23, cpu); // ld I, 0x123
        cpu.runClockCycle();
        EXPECT_EQ(*registers.PC, 0x202);
        EXPECT_EQ(*registers.I, 0x123);
    }

    TEST(CpuUnitTests, jp_v0_addr_executes_correctly)
    {
        Logger logger;
        Gpu gpu;
        Timer soundTimer;
        Timer delayTimer;
        chip8::Cpu cpu(logger, gpu, soundTimer, delayTimer);
        chip8::Registers& registers(cpu.getRegisters());
        initTest(0xb1, 0x23, cpu); // jp v0, 0x123
        registers.V[0] = 0x30;
        cpu.runClockCycle();
        EXPECT_EQ(*registers.PC, 0x123 + 0x30);
    }

    // Skipping RND instruction as it is not a good practice to test random generation in a non-determinstic way (not worth).

    TEST(CpuUnitTests, drw_vx_vy_nibble_executes_correctly)
    {
        Logger logger;
        Gpu gpu;
        Timer soundTimer;
        Timer delayTimer;
        chip8::Cpu cpu(logger, gpu, soundTimer, delayTimer);
        chip8::Registers& registers(cpu.getRegisters());
        initTest(0xd1, 0x25, cpu); // drw v1, v2, 5
        registers.V[1] = 60;
        registers.V[2] = 20;
        registers.I = 0x100;
        EXPECT_CALL(gpu, setSprite).Times(1);
        cpu.runClockCycle();
        EXPECT_EQ(*registers.PC, 0x202);
        EXPECT_EQ(*registers.I, 0x100);
        EXPECT_FALSE(registers.V[0xf]);
    }

    TEST(CpuUnitTests, skp_vx_skip)
    {
        Logger logger;
        Gpu gpu;
        Timer soundTimer;
        Timer delayTimer;
        chip8::Cpu cpu(logger, gpu, soundTimer, delayTimer);
        chip8::Registers& registers(cpu.getRegisters());
        initTest(0xe2, 0x9e, cpu); // skp v2
        registers.V[2] = static_cast<uint8_t>(Key::DigitC);
        cpu.onKeyPressed(Key::DigitC);
        cpu.runClockCycle();
        EXPECT_EQ(*registers.PC, 0x204);
    }

    TEST(CpuUnitTests, skp_vx_no_skip)
    {
        Logger logger;
        Gpu gpu;
        Timer soundTimer;
        Timer delayTimer;
        chip8::Cpu cpu(logger, gpu, soundTimer, delayTimer);
        chip8::Registers& registers(cpu.getRegisters());
        initTest(0xe2, 0x9e, cpu); // skp v2
        registers.V[2] = static_cast<uint8_t>(Key::DigitD);
        cpu.onKeyPressed(Key::DigitC);
        cpu.runClockCycle();
        EXPECT_EQ(*registers.PC, 0x202);
    }

    TEST(CpuUnitTests, sknp_vx_skip)
    {
        Logger logger;
        Gpu gpu;
        Timer soundTimer;
        Timer delayTimer;
        chip8::Cpu cpu(logger, gpu, soundTimer, delayTimer);
        chip8::Registers& registers(cpu.getRegisters());
        initTest(0xe2, 0xa1, cpu); // sknp v2
        registers.V[2] = static_cast<uint8_t>(Key::DigitC);
        cpu.onKeyReleased(Key::DigitC);
        cpu.runClockCycle();
        EXPECT_EQ(*registers.PC, 0x204);
    }

    TEST(CpuUnitTests, sknp_vx_no_skip)
    {
        Logger logger;
        Gpu gpu;
        Timer soundTimer;
        Timer delayTimer;
        chip8::Cpu cpu(logger, gpu, soundTimer, delayTimer);
        chip8::Registers& registers(cpu.getRegisters());
        initTest(0xe2, 0xa1, cpu); // sknp v2
        registers.V[2] = static_cast<uint8_t>(Key::DigitC);
        cpu.onKeyPressed(Key::DigitC);
        cpu.runClockCycle();
        EXPECT_EQ(*registers.PC, 0x202);
    }

    TEST(CpuUnitTests, ld_vx_dt_executes_correctly)
    {
        Logger logger;
        Gpu gpu;
        Timer soundTimer;
        Timer delayTimer;
        chip8::Cpu cpu(logger, gpu, soundTimer, delayTimer);
        chip8::Registers& registers(cpu.getRegisters());
        initTest(0xf2, 0x07, cpu); // ld v2, DT
        delayTimer.setValue(50);
        cpu.runClockCycle();
        EXPECT_EQ(*registers.PC, 0x202);
        EXPECT_EQ(registers.V[2], 50);
    }

    TEST(CpuUnitTests, ld_vx_k_executes_correctly)
    {
        Logger logger;
        Gpu gpu;
        Timer soundTimer;
        Timer delayTimer;
        chip8::Cpu cpu(logger, gpu, soundTimer, delayTimer);
        chip8::Registers& registers(cpu.getRegisters());
        initTest(0xf2, 0x0a, cpu); // ld v2, K
        cpu.onKeyPressed(Key::Num3);
        cpu.runClockCycle();
        EXPECT_EQ(*registers.PC, 0x202);
        EXPECT_EQ(registers.V[2], static_cast<uint8_t>(Key::Num3));
    }

    TEST(CpuUnitTests, ld_vx_k_executes_correctly_two_attempts)
    {
        Logger logger;
        Gpu gpu;
        Timer soundTimer;
        Timer delayTimer;
        chip8::Cpu cpu(logger, gpu, soundTimer, delayTimer);
        chip8::Registers& registers(cpu.getRegisters());
        initTest(0xf2, 0x0a, cpu); // ld v2, K
        cpu.runClockCycle();
        EXPECT_EQ(*registers.PC, 0x200);
        cpu.onKeyPressed(Key::Num3);
        cpu.runClockCycle();
        EXPECT_EQ(*registers.PC, 0x202);
        EXPECT_EQ(registers.V[2], static_cast<uint8_t>(Key::Num3));
    }

    TEST(CpuUnitTests, ld_dt_vx_executes_correctly)
    {
        Logger logger;
        Gpu gpu;
        Timer soundTimer;
        Timer delayTimer;
        chip8::Cpu cpu(logger, gpu, soundTimer, delayTimer);
        chip8::Registers& registers(cpu.getRegisters());
        initTest(0xf2, 0x15, cpu); // ld DT, v2
        registers.V[2] = 50;
        cpu.runClockCycle();
        EXPECT_EQ(*registers.PC, 0x202);
        EXPECT_EQ(delayTimer.getValue(), 50);
    }

    TEST(CpuUnitTests, ld_st_vx_executes_correctly)
    {
        Logger logger;
        Gpu gpu;
        Timer soundTimer;
        Timer delayTimer;
        chip8::Cpu cpu(logger, gpu, soundTimer, delayTimer);
        chip8::Registers& registers(cpu.getRegisters());
        initTest(0xf2, 0x18, cpu); // ld ST, v2
        registers.V[2] = 50;
        cpu.runClockCycle();
        EXPECT_EQ(*registers.PC, 0x202);
        EXPECT_EQ(soundTimer.getValue(), 50);
    }

    TEST(CpuUnitTests, add_i_vx_executes_correctly)
    {
        Logger logger;
        Gpu gpu;
        Timer soundTimer;
        Timer delayTimer;
        chip8::Cpu cpu(logger, gpu, soundTimer, delayTimer);
        chip8::Registers& registers(cpu.getRegisters());
        initTest(0xf2, 0x1e, cpu); // add I, V2
        registers.V[2] = 0x30;
        registers.I = 0x100;
        cpu.runClockCycle();
        EXPECT_EQ(*registers.PC, 0x202);
        EXPECT_EQ(*registers.I, 0x100 + 0x30);
    }

    TEST(CpuUnitTests, ld_f_vx_executes_correctly)
    {
        Logger logger;
        Gpu gpu;
        Timer soundTimer;
        Timer delayTimer;
        chip8::Cpu cpu(logger, gpu, soundTimer, delayTimer);
        chip8::Registers& registers(cpu.getRegisters());
        initTest(0xf2, 0x29, cpu); // ld F, vx
        registers.V[2] = 0xa;
        cpu.runClockCycle();
        EXPECT_EQ(*registers.PC, 0x202);
        EXPECT_EQ(*registers.I, 0xa * 5);
    }

    TEST(CpuUnitTests, ld_b_vx_executes_correctly)
    {
        Logger logger;
        Gpu gpu;
        Timer soundTimer;
        Timer delayTimer;
        chip8::Cpu cpu(logger, gpu, soundTimer, delayTimer);
        chip8::Registers& registers(cpu.getRegisters());
        initTest(0xf2, 0x33, cpu); // ld B, v2
        registers.V[2] = 123;
        cpu.runClockCycle();
        EXPECT_EQ(*registers.PC, 0x202);
    }

    TEST(CpuUnitTests, ld_ivalue_vx_executes_correctly)
    {
        Logger logger;
        Gpu gpu;
        Timer soundTimer;
        Timer delayTimer;
        chip8::Cpu cpu(logger, gpu, soundTimer, delayTimer);
        chip8::Registers& registers(cpu.getRegisters());
        initTest(0xf4, 0x55, cpu); // ld [I], v4
        registers.I = 10;
        cpu.runClockCycle();
        EXPECT_EQ(*registers.PC, 0x202);
        EXPECT_EQ(*registers.I, 10 + 5);
    }

    TEST(CpuUnitTests, ld_vx_ivalue_executes_correctly)
    {
        Logger logger;
        Gpu gpu;
        Timer soundTimer;
        Timer delayTimer;
        chip8::Cpu cpu(logger, gpu, soundTimer, delayTimer);
        chip8::Registers& registers(cpu.getRegisters());
        initTest(0xf4, 0x65, cpu); // ld v4, [I]
        registers.I = 10;
        cpu.runClockCycle();
        EXPECT_EQ(*registers.PC, 0x202);
        EXPECT_EQ(*registers.I, 10 + 5);
    }

    TEST(CpuUnitTests, unsupported_instruction_throws)
    {
        Logger logger;
        Gpu gpu;
        Timer soundTimer;
        Timer delayTimer;
        chip8::Cpu cpu(logger, gpu, soundTimer, delayTimer);
        chip8::Registers& registers(cpu.getRegisters());
        initTest(0xff, 0xff, cpu); // <unsupported instruction>

        try
        {
            cpu.runClockCycle();
            EXPECT_TRUE(false);
        }
        catch (CpuExecutionException& e)
        {
            EXPECT_STREQ(e.what(), "invalid instruction");
        }
    }
}
