#include "cpu/Cpu.hpp"

#include <cpu/CpuExecutionException.hpp>
#include <cpu/Font.hpp>
#include <cpu/RomLoadFailureException.hpp>
#include <functional>

chip8::Cpu::Cpu(const logging::Logger& logger, chip8::IGpu& gpu, chip8::ITimer& soundTimer, chip8::ITimer& delayTimer)
    : logger(logger)
    , gpu(gpu)
    , memory()
    , soundTimer(soundTimer)
    , delayTimer(delayTimer)
    , randomDevice()
    , randomEngine(randomDevice())
    , playAudioFlag(false)
    , uniformDistrubution(std::numeric_limits<uint8_t>::min(), std::numeric_limits<uint8_t>::max())
    , registers(std::bind(&Cpu::validateMemoryWrite, this, std::placeholders::_1),
                std::bind(&Cpu::validateMemoryRead, this, std::placeholders::_1),
                std::bind(&Cpu::validateStackWrite, this, std::placeholders::_1),
                std::bind(&Cpu::validateStackRead, this, std::placeholders::_1))
    , instructions(std::bind(&chip8::Cpu::execute_sys_addr, this, std::placeholders::_1),
                   std::bind(&chip8::Cpu::execute_cls, this),
                   std::bind(&chip8::Cpu::execute_ret, this),
                   std::bind(&chip8::Cpu::execute_jp_addr, this, std::placeholders::_1),
                   std::bind(&chip8::Cpu::execute_call_addr, this, std::placeholders::_1),
                   std::bind(&chip8::Cpu::execute_se_vx_byte, this, std::placeholders::_1, std::placeholders::_2),
                   std::bind(&chip8::Cpu::execute_sne_vx_byte, this, std::placeholders::_1, std::placeholders::_2),
                   std::bind(&chip8::Cpu::execute_se_vx_vy, this, std::placeholders::_1, std::placeholders::_2),
                   std::bind(&chip8::Cpu::execute_ld_vx_byte, this, std::placeholders::_1, std::placeholders::_2),
                   std::bind(&chip8::Cpu::execute_add_vx_byte, this, std::placeholders::_1, std::placeholders::_2),
                   std::bind(&chip8::Cpu::execute_ld_vx_vy, this, std::placeholders::_1, std::placeholders::_2),
                   std::bind(&chip8::Cpu::execute_or_vx_vy, this, std::placeholders::_1, std::placeholders::_2),
                   std::bind(&chip8::Cpu::execute_and_vx_vy, this, std::placeholders::_1, std::placeholders::_2),
                   std::bind(&chip8::Cpu::execute_xor_vx_vy, this, std::placeholders::_1, std::placeholders::_2),
                   std::bind(&chip8::Cpu::execute_add_vx_vy, this, std::placeholders::_1, std::placeholders::_2),
                   std::bind(&chip8::Cpu::execute_sub_vx_vy, this, std::placeholders::_1, std::placeholders::_2),
                   std::bind(&chip8::Cpu::execute_shr_vx_vy, this, std::placeholders::_1, std::placeholders::_2),
                   std::bind(&chip8::Cpu::execute_subn_vx_vy, this, std::placeholders::_1, std::placeholders::_2),
                   std::bind(&chip8::Cpu::execute_shl_vx_vy, this, std::placeholders::_1, std::placeholders::_2),
                   std::bind(&chip8::Cpu::execute_sne_vx_vy, this, std::placeholders::_1, std::placeholders::_2),
                   std::bind(&chip8::Cpu::execute_ld_i_addr, this, std::placeholders::_1),
                   std::bind(&chip8::Cpu::execute_jp_v0_addr, this, std::placeholders::_1),
                   std::bind(&chip8::Cpu::execute_rnd_vx_byte, this, std::placeholders::_1, std::placeholders::_2),
                   std::bind(&chip8::Cpu::execute_drw_vx_vy_nibble, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3),
                   std::bind(&chip8::Cpu::execute_skp_vx, this, std::placeholders::_1),
                   std::bind(&chip8::Cpu::execute_sknp_vx, this, std::placeholders::_1),
                   std::bind(&chip8::Cpu::execute_ld_vx_dt, this, std::placeholders::_1),
                   std::bind(&chip8::Cpu::execute_ld_vx_k, this, std::placeholders::_1),
                   std::bind(&chip8::Cpu::execute_ld_dt_vx, this, std::placeholders::_1),
                   std::bind(&chip8::Cpu::execute_ld_st_vx, this, std::placeholders::_1),
                   std::bind(&chip8::Cpu::execute_add_i_vx, this, std::placeholders::_1),
                   std::bind(&chip8::Cpu::execute_ld_f_vx, this, std::placeholders::_1),
                   std::bind(&chip8::Cpu::execute_ld_b_vx, this, std::placeholders::_1),
                   std::bind(&chip8::Cpu::execute_ld_idata_vx, this, std::placeholders::_1),
                   std::bind(&chip8::Cpu::execute_ld_vx_idata, this, std::placeholders::_1),
                   std::bind(&chip8::Cpu::onUnmatchedInstruction, this))
{
}

void chip8::Cpu::boot(std::istream& stream)
{
    initializeRegisters();
    initializeMemory();

    stream.seekg(0, std::ios::end);
    const size_t streamLength = stream.tellg();

    if (streamLength > MemorySize - ProgramStartLocation)
    {
        throw chip8::RomLoadFailureException("Not enough memory to execute the program");
    }

    stream.seekg(0, std::ios::beg);
    stream.read(reinterpret_cast<char*>(memory.data() + ProgramStartLocation), streamLength);
    std::copy(chip8::font.begin(), chip8::font.end(), memory.begin());

    registers.PC = static_cast<uint16_t>(ProgramStartLocation);
}

void chip8::Cpu::runClockCycle()
{
    uint8_t byte1 = memory[*registers.PC];
    uint8_t byte2 = memory[registers.PC + 1];
    registers.PC += 2;

    delayTimer.updateValue();
    soundTimer.updateValue();
    playAudioFlag = soundTimer.getValue() > 0;

    instructions.matchInstruction(byte1, byte2);
}

void chip8::Cpu::onKeyPressed(const chip8::Key key)
{
    keyPressedStatus[static_cast<size_t>(key)] = true;
}

void chip8::Cpu::onKeyReleased(const chip8::Key key)
{
    keyPressedStatus[static_cast<size_t>(key)] = false;
}

bool chip8::Cpu::shouldPlayAudio() const
{
    return playAudioFlag;
}

size_t chip8::Cpu::getWidth() const
{
    return gpu.getWidth();
}

size_t chip8::Cpu::getHeight() const
{
    return gpu.getHeight();
}

const std::vector<bool>& chip8::Cpu::getFrameBuffer() const
{
    return gpu.getFrameBuffer();
}

chip8::Registers& chip8::Cpu::getRegisters()
{
    return registers;
}

void chip8::Cpu::initializeRegisters()
{
    registers.I = 0;
    registers.PC = 0;
    registers.SP = 0;
    std::fill(registers.V.begin(), registers.V.end(), 0);
}

void chip8::Cpu::initializeMemory()
{
    std::fill(memory.begin(), memory.end(), 0);
    std::fill(stack.begin(), stack.end(), 0);
    std::fill(keyPressedStatus.begin(), keyPressedStatus.end(), false);
}

// The validators below allow accessing one extra byte going out of memory.
// This is necessary when for example the stack is full (stack pointer will point to 65-th byte, because it points to the next
// elment to be pushed). However, validation should fail when writing to 65-th byte but it does not.
// Maybe have separate read and write validators.

void chip8::Cpu::validateMemoryWrite(uint16_t address)
{
    if (address > MemorySize)
    {
        throw CpuExecutionException(CpuErrorCode::AddressOutOfBound);
    }
}

void chip8::Cpu::validateStackWrite(uint8_t address)
{
    if (address > StackSize)
    {
        throw CpuExecutionException(CpuErrorCode::StackOverflow);
    }
}

void chip8::Cpu::validateMemoryRead(uint16_t address)
{
    if (address >= MemorySize)
    {
        throw CpuExecutionException(CpuErrorCode::AddressOutOfBound);
    }
}

void chip8::Cpu::validateStackRead(uint8_t address)
{
    if (address >= StackSize)
    {
        throw CpuExecutionException(CpuErrorCode::StackOverflow);
    }
}

void chip8::Cpu::execute_sys_addr(uint16_t addr)
{
    throw chip8::CpuExecutionException(CpuErrorCode::UnsupportedSysInstruction);
}

void chip8::Cpu::execute_cls()
{
    gpu.clear();
}

void chip8::Cpu::execute_ret()
{
    uint8_t byte2 = stack[registers.SP - 1];
    uint8_t byte1 = stack[registers.SP - 2];
    registers.PC = (byte1 << 8) | byte2;
    registers.SP -= 2;
}

void chip8::Cpu::execute_jp_addr(uint16_t addr)
{
    registers.PC = addr;
}

void chip8::Cpu::execute_call_addr(uint16_t addr)
{
    stack[*registers.SP] = static_cast<uint8_t>((*registers.PC & 0xFF00) >> 8);
    stack[registers.SP + 1] = static_cast<uint8_t>(*registers.PC & 0x00FF);
    registers.SP += 2;
    registers.PC = addr;
}

void chip8::Cpu::execute_se_vx_byte(binding::MatchingPatternType Vx, uint8_t byte)
{
    if (registers.V[Vx] == byte)
    {
        registers.PC += 2;
    }
}

void chip8::Cpu::execute_sne_vx_byte(binding::MatchingPatternType Vx, uint8_t byte)
{
    if (registers.V[Vx] != byte)
    {
        registers.PC += 2;
    }
}

void chip8::Cpu::execute_se_vx_vy(binding::MatchingPatternType Vx, binding::MatchingPatternType Vy)
{
    if (registers.V[Vx] == registers.V[Vy])
    {
        registers.PC += 2;
    }
}

void chip8::Cpu::execute_ld_vx_byte(binding::MatchingPatternType Vx, uint8_t byte)
{
    registers.V[Vx] = byte;
}

void chip8::Cpu::execute_add_vx_byte(binding::MatchingPatternType Vx, uint8_t byte)
{
    registers.V[Vx] = registers.V[Vx] + byte;
}

void chip8::Cpu::execute_ld_vx_vy(binding::MatchingPatternType Vx, binding::MatchingPatternType Vy)
{
    registers.V[Vx] = registers.V[Vy];
}

void chip8::Cpu::execute_or_vx_vy(binding::MatchingPatternType Vx, binding::MatchingPatternType Vy)
{
    registers.V[Vx] = registers.V[Vx] | registers.V[Vy];
}

void chip8::Cpu::execute_and_vx_vy(binding::MatchingPatternType Vx, binding::MatchingPatternType Vy)
{
    registers.V[Vx] = registers.V[Vx] & registers.V[Vy];
}

void chip8::Cpu::execute_xor_vx_vy(binding::MatchingPatternType Vx, binding::MatchingPatternType Vy)
{
    registers.V[Vx] = registers.V[Vx] ^ registers.V[Vy];
}

void chip8::Cpu::execute_add_vx_vy(binding::MatchingPatternType Vx, binding::MatchingPatternType Vy)
{
    uint8_t originalVx = registers.V[Vx];
    registers.V[Vx] = registers.V[Vx] + registers.V[Vy];
    registers.V[VF] = (registers.V[Vx] < originalVx);
}

void chip8::Cpu::execute_sub_vx_vy(binding::MatchingPatternType Vx, binding::MatchingPatternType Vy)
{
    uint8_t originalVx = registers.V[Vx];
    registers.V[Vx] = registers.V[Vx] - registers.V[Vy];
    registers.V[VF] = (registers.V[Vx] <= originalVx);
}

void chip8::Cpu::execute_shr_vx_vy(binding::MatchingPatternType Vx, binding::MatchingPatternType Vy)
{
    registers.V[VF] = ((registers.V[Vx] & 0b00000001) == 1);
    registers.V[Vx] = registers.V[Vx] >> 1;
}

void chip8::Cpu::execute_subn_vx_vy(binding::MatchingPatternType Vx, binding::MatchingPatternType Vy)
{
    uint8_t originalVy = registers.V[Vy];
    registers.V[Vx] = registers.V[Vy] - registers.V[Vx];
    registers.V[VF] = (registers.V[Vx] <= originalVy);
}

void chip8::Cpu::execute_shl_vx_vy(binding::MatchingPatternType Vx, binding::MatchingPatternType Vy)
{
    registers.V[VF] = ((registers.V[Vx] & 0b10000000) == 0b10000000);
    registers.V[Vx] = registers.V[Vx] << 1;
}

void chip8::Cpu::execute_sne_vx_vy(binding::MatchingPatternType Vx, binding::MatchingPatternType Vy)
{
    if (registers.V[Vx] != registers.V[Vy])
    {
        registers.PC += 2;
    }
}

void chip8::Cpu::execute_ld_i_addr(uint16_t addr)
{
    registers.I = addr;
}

void chip8::Cpu::execute_jp_v0_addr(uint16_t addr)
{
    registers.PC = registers.V[0] + addr;
}

void chip8::Cpu::execute_rnd_vx_byte(binding::MatchingPatternType Vx, uint8_t byte)
{
    const uint8_t randomNumber = static_cast<uint8_t>(uniformDistrubution(randomEngine));
    registers.V[Vx] = randomNumber & byte;
}

void chip8::Cpu::execute_drw_vx_vy_nibble(binding::MatchingPatternType Vx, binding::MatchingPatternType Vy, binding::MatchingPatternType nibble)
{
    const size_t spriteStartLocation = *registers.I;
    const size_t spriteEndLocation = registers.I + nibble;
    std::vector<uint8_t> sprite(memory.begin() + spriteStartLocation, memory.begin() + spriteEndLocation);
    registers.V[VF] = gpu.setSprite(registers.V[Vx], registers.V[Vy], sprite);
}

void chip8::Cpu::execute_skp_vx(binding::MatchingPatternType Vx)
{
    if (registers.V[Vx] >= keyPressedStatus.size())
    {
        throw CpuExecutionException(CpuErrorCode::BadKeyboardIndex);
    }

    if (keyPressedStatus[registers.V[Vx]])
    {
        registers.PC += 2;
    }
}

void chip8::Cpu::execute_sknp_vx(binding::MatchingPatternType Vx)
{
    if (registers.V[Vx] >= keyPressedStatus.size())
    {
        throw CpuExecutionException(CpuErrorCode::BadKeyboardIndex);
    }

    if (!keyPressedStatus[registers.V[Vx]])
    {
        registers.PC += 2;
    }
}

void chip8::Cpu::execute_ld_vx_dt(binding::MatchingPatternType Vx)
{
    registers.V[Vx] = delayTimer.getValue();
}

void chip8::Cpu::execute_ld_vx_k(binding::MatchingPatternType Vx)
{
    // Find a key pressed.
    for (size_t i = 0; i < keyPressedStatus.size(); i++)
    {
        // If any, store its index into Vx
        if (keyPressedStatus[i])
        {
            registers.V[Vx] = static_cast<uint8_t>(i);
            keyPressedStatus[i] = false;
            return;
        }
    }

    // If no key is found, try again on next step.
    registers.PC -= 2;
}

void chip8::Cpu::execute_ld_dt_vx(binding::MatchingPatternType Vx)
{
    delayTimer.setValue(registers.V[Vx]);
}

void chip8::Cpu::execute_ld_st_vx(binding::MatchingPatternType Vx)
{
    soundTimer.setValue(registers.V[Vx]);
}

void chip8::Cpu::execute_add_i_vx(binding::MatchingPatternType Vx)
{
    registers.I += registers.V[Vx];
}

void chip8::Cpu::execute_ld_f_vx(binding::MatchingPatternType Vx)
{
    registers.I = registers.V[Vx] * chip8::FontCharacterHeight; // I = Vx * 5
}

void chip8::Cpu::execute_ld_b_vx(binding::MatchingPatternType Vx)
{
    memory[*registers.I] = registers.V[Vx] / 100;
    memory[registers.I + 1] = (registers.V[Vx] / 10) % 10;
    memory[registers.I + 2] = registers.V[Vx] % 10;
}

void chip8::Cpu::execute_ld_idata_vx(binding::MatchingPatternType Vx)
{
    for (size_t i = 0; i <= Vx; i++)
    {
        memory[*registers.I] = registers.V[i];
        registers.I += 1;
    }
}

void chip8::Cpu::execute_ld_vx_idata(binding::MatchingPatternType Vx)
{
    for (size_t i = 0; i <= Vx; i++)
    {
        registers.V[i] = memory[*registers.I];
        registers.I += 1;
    }
}

void chip8::Cpu::onUnmatchedInstruction()
{
    throw CpuExecutionException(CpuErrorCode::UnmatchedInstruction);
}