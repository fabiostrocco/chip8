#include <functional>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "emulator/InstructionBinder.hpp"

class BinderHandler
{
  public:
    MOCK_METHOD(void, handler, ());
    MOCK_METHOD(void, handler1, (chip8::BindingUnitType));
    MOCK_METHOD(void, handler2, (chip8::BindingUnitType, chip8::BindingUnitType));
    MOCK_METHOD(void, handler3, (chip8::BindingUnitType, chip8::BindingUnitType, chip8::BindingUnitType));
    MOCK_METHOD(void, handler4, (chip8::BindingUnitType, chip8::BindingUnitType, chip8::BindingUnitType, chip8::BindingUnitType));
};

namespace chip8::unit_tests
{
    TEST(InstructionBinderUnitTests, InstructionBinder_NoPlaceholders_BindsSuccessfully)
    {
        constexpr chip8::BindingUnitType u1 = 1, u2 = 2, u3 = 3, u4 = 4;
        BinderHandler binderHandler;
        EXPECT_CALL(binderHandler, handler).Times(1);
        chip8::InstructionBinder<u1, u2, u3, u4> binder([&]() { binderHandler.handler(); });
        binder.match(u1 << 4 | u2, u3 << 4 | u4);
    }

    TEST(InstructionBinderUnitTests, InstructionBinder_NoPlaceholders_BindFails)
    {
        constexpr chip8::BindingUnitType u1 = 1, u2 = 2, u3 = 3, u4 = 4;
        BinderHandler binderHandler;
        EXPECT_CALL(binderHandler, handler).Times(0);
        chip8::InstructionBinder<u1, u2, u3, u4> binder([&]() { binderHandler.handler(); });
        binder.match(0, u3 << 4 | u4);
    }

    TEST(InstructionBinderUnitTests, InstructionBinder_OnePlaceholder_BindsSuccessfully)
    {
        constexpr chip8::BindingUnitType u1 = 1, u2 = 2, u3 = chip8::placeholder, u4 = 4;
        constexpr chip8::BindingUnitType v3 = 0xa;

        BinderHandler binderHandler;
        EXPECT_CALL(binderHandler, handler1(v3)).Times(1);
        chip8::InstructionBinder<u1, u2, u3, u4> binder([&](const chip8::BindingUnitType v) { binderHandler.handler1(v); });
        binder.match(u1 << 4 | u2, v3 << 4 | u4);
    }

    TEST(InstructionBinderUnitTests, InstructionBinder_OnePlaceholder_BindsFails)
    {
        constexpr chip8::BindingUnitType u1 = 1, u2 = 2, u3 = chip8::placeholder, u4 = 4;
        constexpr chip8::BindingUnitType v3 = 0xa;

        BinderHandler binderHandler;
        EXPECT_CALL(binderHandler, handler1).Times(0);
        chip8::InstructionBinder<u1, u2, u3, u4> binder([&](const chip8::BindingUnitType v) { binderHandler.handler1(v); });
        binder.match((u1 + 1) << 4 | u2, v3 << 4 | u4);
    }

    TEST(InstructionBinderUnitTests, InstructionBinder_TwoPlaceholders_BindsSuccessfully)
    {
        constexpr chip8::BindingUnitType u1 = chip8::placeholder, u2 = 2, u3 = chip8::placeholder, u4 = 4;
        constexpr chip8::BindingUnitType v1 = 0xa, v3 = 0xb;

        BinderHandler binderHandler;
        EXPECT_CALL(binderHandler, handler2(v1, v3)).Times(1);
        chip8::InstructionBinder<u1, u2, u3, u4> binder([&](const chip8::BindingUnitType v, const chip8::BindingUnitType vv)
                                                        { binderHandler.handler2(v, vv); });
        binder.match(v1 << 4 | u2, v3 << 4 | u4);
    }

    TEST(InstructionBinderUnitTests, InstructionBinder_TwoPlaceholders_BindFails)
    {
        constexpr chip8::BindingUnitType u1 = chip8::placeholder, u2 = 2, u3 = chip8::placeholder, u4 = 4;
        constexpr chip8::BindingUnitType v2 = 0xa, v3 = 0xb;

        BinderHandler binderHandler;
        EXPECT_CALL(binderHandler, handler2).Times(0);
        chip8::InstructionBinder<u1, u2, u3, u4> binder([&](const chip8::BindingUnitType v, const chip8::BindingUnitType vv)
                                                        { binderHandler.handler2(v, vv); });
        binder.match((u1 + 1) << 4 | v2, v3 << 4 | u4);
    }

    TEST(InstructionBinderUnitTests, InstructionBinder_ThreePlaceholders_BindsSuccessfully)
    {
        constexpr chip8::BindingUnitType u1 = chip8::placeholder, u2 = 2, u3 = chip8::placeholder, u4 = chip8::placeholder;
        constexpr chip8::BindingUnitType v1 = 0xa, v3 = 0xb, v4 = 0xc;

        BinderHandler binderHandler;
        EXPECT_CALL(binderHandler, handler3(v1, v3, v4)).Times(1);
        chip8::InstructionBinder<u1, u2, u3, u4> binder(
            [&](const chip8::BindingUnitType v, const chip8::BindingUnitType vv, const chip8::BindingUnitType vvv)
            { binderHandler.handler3(v, vv, vvv); });
        binder.match((v1 << 4) | u2, (v3 << 4) | v4);
    }

    TEST(InstructionBinderUnitTests, InstructionBinder_ThreePlaceholders_BindFails)
    {
        constexpr chip8::BindingUnitType u1 = chip8::placeholder, u2 = 2, u3 = chip8::placeholder, u4 = chip8::placeholder;
        constexpr chip8::BindingUnitType v1 = 0xa, v3 = 0xb, v4 = 0xc;

        BinderHandler binderHandler;
        EXPECT_CALL(binderHandler, handler3).Times(0);
        chip8::InstructionBinder<u1, u2, u3, u4> binder(
            [&](const chip8::BindingUnitType v, const chip8::BindingUnitType vv, const chip8::BindingUnitType vvv)
            { binderHandler.handler3(v, vv, vvv); });
        binder.match(v1 << 4 | (u2 + 1), v3 << 4 | v4);
    }

    TEST(InstructionBinderUnitTests, InstructionBinder_FourPlaceholders_BindsSuccessfully)
    {
        constexpr chip8::BindingUnitType u1 = chip8::placeholder, u2 = chip8::placeholder, u3 = chip8::placeholder,
                                         u4 = chip8::placeholder;
        constexpr chip8::BindingUnitType v1 = 0xa, v2 = 0xb, v3 = 0xc, v4 = 0xd;

        BinderHandler binderHandler;
        EXPECT_CALL(binderHandler, handler4(v1, v2, v3, v4)).Times(1);
        chip8::InstructionBinder<u1, u2, u3, u4> binder([&](const chip8::BindingUnitType v,
                                                            const chip8::BindingUnitType vv,
                                                            const chip8::BindingUnitType vvv,
                                                            const chip8::BindingUnitType vvvv)
                                                        { binderHandler.handler4(v, vv, vvv, vvvv); });
        binder.match(v1 << 4 | v2, v3 << 4 | v4);
    }
}