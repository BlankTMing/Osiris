#pragma once

#include <string_view>

#include <CS2/Constants/SoundNames.h>

struct BombBeepSound {
    static constexpr auto kFadeAwayStart = 0.0f;
    static constexpr auto kFadeAwayDuration = 0.3f;
    static constexpr auto kMinScale = 0.4f;

    [[nodiscard]] static constexpr bool isSound(std::string_view soundName) noexcept
    {
        return soundName.starts_with(cs2::kBombSoundsPath) && std::string_view{ soundName.data() + cs2::kBombSoundsPath.length(), soundName.length() - cs2::kBombSoundsPath.length() }.starts_with(cs2::kBombBeepSoundsPrefix);
    }
};
