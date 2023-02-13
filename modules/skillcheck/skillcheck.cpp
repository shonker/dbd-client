#include "skillcheck.hpp"

#include <mutex>
#include <cmath>

#include <array>
#include <vector>
#include <format>
#include <map>

#include <unordered_map>
#include <optional>

#include "../../engine/engine.hpp"
#include "../../engine/types/types.hpp"

#include "../../settings/settings.hpp"

#include "../../renderer/renderer.hpp"
#include "../../renderer/menu/config/config.hpp"

#include "../../engine/structures/structures.hpp"

void modules::skillcheck::callback()
{
    static auto lp = engine::types::uworld::get().get_local_player();

    while (true) //this will be rewritten once i get it to work will all the skillcheck types, | the function is supposed to be called once in new thread |
    {
        auto skill_check = lp.get_skill_check();

        if (skill_check.instance && std::get<bool>(menu::config::config_map[features::auto_skill_check]) && skill_check.is_displayed())
        {
            const auto curr_progress = skill_check.get_current_progress();
            const auto success_zone = skill_check.get_success_zone();
            const auto rotation = skill_check.get_rotation();

            auto current_progress_fixed_f = 0.f;
            {
                float current_progress_fixed = 0; *(std::uintptr_t*)&current_progress_fixed = 0;
                std::uint8_t space[10]{};

                if (rotation < 0.f)
                    *(std::uintptr_t*)&current_progress_fixed = 0xBF800000;
                else
                    *(std::uintptr_t*)&current_progress_fixed = 0x3F800000;

                *(float*)&current_progress_fixed = (float)(*(float*)&current_progress_fixed * curr_progress) + driver.read<float>(skill_check.instance + 0x1A4);

                if (*(float*)&current_progress_fixed < 0.f)
                    *(float*)&current_progress_fixed += 1.f;

                current_progress_fixed_f = *(float*)&current_progress_fixed;
            }

            if (rotation < 0.f && current_progress_fixed_f < success_zone)
            {
                INPUT space = { 0 };
                space.type = INPUT_KEYBOARD;
                space.ki.wVk = VK_SPACE;
                SendInput(1, &space, sizeof(INPUT));
                space.ki.dwFlags = KEYEVENTF_KEYUP;
                SendInput(1, &space, sizeof(INPUT));

                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            }

            else if (rotation > 0.f && current_progress_fixed_f > success_zone)
            {
                INPUT space = { 0 };
                space.type = INPUT_KEYBOARD;
                space.ki.wVk = VK_SPACE;
                SendInput(1, &space, sizeof(INPUT));
                space.ki.dwFlags = KEYEVENTF_KEYUP;
                SendInput(1, &space, sizeof(INPUT));

                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            }
        }

        auto wait_time = std::get<bool>(omenu::config::config_map[features::instant_skill_check]) ? 10 : 100;

        std::this_thread::sleep_for(std::chrono::milliseconds(wait_time));
    }
}