#pragma once

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <Mocks/MockBaseEntity.h>
#include <Mocks/MockConfig.h>
#include <Mocks/MockHookContext.h>

#include <Features/Visuals/OutlineGlow/WeaponOutlineGlow/WeaponOutlineGlow.h>
#include <Features/Visuals/OutlineGlow/OutlineGlowConfigVariables.h>

class WeaponOutlineGlowTest : public testing::Test {
protected:
    WeaponOutlineGlowTest()
    {
        EXPECT_CALL(mockHookContext, config()).WillOnce(testing::ReturnRef(mockConfig));
    }

    testing::StrictMock<MockHookContext> mockHookContext;
    testing::StrictMock<MockConfig> mockConfig;
    testing::StrictMock<MockBaseEntity> mockBaseEntity;

    WeaponOutlineGlow<MockHookContext> weaponOutlineGlow{mockHookContext};
};

struct WeaponOutlineGlowConditionTestParam {
    bool enabled{};
    Optional<bool> hasOwner{};
    bool expectWeaponAccess{};
    bool expectGlowApplied{};
};

class WeaponOutlineGlowConditionTest : public WeaponOutlineGlowTest, public testing::WithParamInterface<WeaponOutlineGlowConditionTestParam> {
};

TEST_P(WeaponOutlineGlowConditionTest, GlowIsAppliedWhenExpected) {
    EXPECT_CALL(mockConfig, getVariable(ConfigVariableTypes::indexOf<WeaponOutlineGlowEnabled>())).WillOnce(testing::Return(GetParam().enabled));

    if (GetParam().expectWeaponAccess)
        EXPECT_CALL(mockBaseEntity, hasOwner()).WillOnce(testing::Return(GetParam().hasOwner));

    if (GetParam().expectGlowApplied)
        EXPECT_CALL(mockBaseEntity, applyGlowRecursively(testing::_, outline_glow_params::kWeaponGlowRange));

    weaponOutlineGlow.applyGlowToWeapon(EntityTypeInfo{}, mockBaseEntity);
}

INSTANTIATE_TEST_SUITE_P(, WeaponOutlineGlowConditionTest, testing::ValuesIn(
    std::to_array<WeaponOutlineGlowConditionTestParam>({
        {
            .enabled = false,
            .expectWeaponAccess = false,
            .expectGlowApplied = false
        },
        {
            .enabled = true,
            .hasOwner{true},
            .expectWeaponAccess = true,
            .expectGlowApplied = false
        },
        {
            .enabled = true,
            .hasOwner{false},
            .expectWeaponAccess = true,
            .expectGlowApplied = true
        },
        {
            .enabled = true,
            .hasOwner{std::nullopt},
            .expectWeaponAccess = true,
            .expectGlowApplied = false
        }
    })
));

struct WeaponOutlineGlowColorTestParam {
    EntityTypeInfo entityTypeInfo{};
    cs2::Color expectedColor{0, 0, 0};
};

class WeaponOutlineGlowColorTest : public WeaponOutlineGlowTest, public testing::WithParamInterface<WeaponOutlineGlowColorTestParam> {
};

TEST_P(WeaponOutlineGlowColorTest, CorrectGlowColorIsApplied) {
    EXPECT_CALL(mockConfig, getVariable(ConfigVariableTypes::indexOf<WeaponOutlineGlowEnabled>())).WillOnce(testing::Return(true));
    EXPECT_CALL(mockBaseEntity, hasOwner()).WillOnce(testing::Return(false));
    EXPECT_CALL(mockBaseEntity, applyGlowRecursively(GetParam().expectedColor, outline_glow_params::kWeaponGlowRange));

    weaponOutlineGlow.applyGlowToWeapon(GetParam().entityTypeInfo, mockBaseEntity);
}

INSTANTIATE_TEST_SUITE_P(, WeaponOutlineGlowColorTest, testing::ValuesIn(
    std::to_array<WeaponOutlineGlowColorTestParam>({
        {
            .entityTypeInfo{},
            .expectedColor{outline_glow_params::kDefaultWeaponColor},
        },
        {
            .entityTypeInfo{EntityTypeInfo::indexOf<cs2::C_Flashbang>()},
            .expectedColor{outline_glow_params::kFlashbangColor},
        },
        {
            .entityTypeInfo{EntityTypeInfo::indexOf<cs2::C_HEGrenade>()},
            .expectedColor{outline_glow_params::kHEGrenadeColor},
        },
        {
            .entityTypeInfo{EntityTypeInfo::indexOf<cs2::C_MolotovGrenade>()},
            .expectedColor{outline_glow_params::kMolotovColor},
        },
        {
            .entityTypeInfo{EntityTypeInfo::indexOf<cs2::C_SmokeGrenade>()},
            .expectedColor{outline_glow_params::kSmokeGrenadeColor},
        }
    })
));
