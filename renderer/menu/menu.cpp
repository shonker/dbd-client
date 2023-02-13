#include "menu.hpp"

#include <array>
#include <functional>

#include "../renderer.hpp"

#include "../../engine/engine.hpp"

#include "../../dependencies/rawdata/person.hpp"
#include "../../dependencies/rawdata/skeleton.hpp"
#include "../../dependencies/libs/imgui_tricks.hpp"

#include "../../dependencies/xorstring/xorstring.hpp"

#include "config/config.hpp"

#include "../../settings/settings.hpp"

#include "../../modules/visuals/visuals.hpp"

static auto current_tab = 0;

void Draw_Main( float, float )
{
    ImGui::PushStyleVar( ImGuiStyleVar_ItemSpacing, { 0,0 } );

    ImGui::SetCursorPos( { 24, 235 } );
    ImGui::BeginChild( xs( "Main" ), { 337, 530 }, false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove );
    {
        switch ( current_tab )
        {
        case 0:
            
        break;
        case 1:
            
        break;
        case 2:
            
        break;
        case 3:
            renderer::imgui::elements::functions::information( xs( "Max Distance" ), xs( "sets the max visual distance" ), std::get< int >( menu::config::config_map[features::max_distance ] ), "%i" );

            renderer::imgui::elements::functions::slider_int( "##1", &std::get< int >( menu::config::config_map[features::max_distance] ), 0, 1000, "" );

            renderer::imgui::elements::functions::checkbox( xs( "Corner Box" ), xs( "draws a cornered box on visual units" ), std::get< bool >( menu::config::config_map[features::player_box_esp ] ) );

            renderer::imgui::elements::functions::color_picker( "##2", std::get< engine::structures::vector4_t >( menu::config::config_map[features::player_box_color] ).color );

            renderer::imgui::elements::functions::separator( );

            renderer::imgui::elements::functions::checkbox( xs( "Player Name" ), xs( "draws player's name" ), std::get< bool >( menu::config::config_map[features::player_names] ) );

            renderer::imgui::elements::functions::color_picker( "##3", std::get< engine::structures::vector4_t >( menu::config::config_map[features::player_names_color] ).color );

            renderer::imgui::elements::functions::separator( ); 

            renderer::imgui::elements::functions::checkbox( xs( "Player Distance" ), xs( "draws distance of unit" ), std::get< bool >( menu::config::config_map[features::player_distance_esp] ) );

            renderer::imgui::elements::functions::color_picker( "##4", std::get< engine::structures::vector4_t >( menu::config::config_map[features::player_distance_color] ).color );

            renderer::imgui::elements::functions::separator();

            renderer::imgui::elements::functions::checkbox(xs("Player Items"), xs("draws player items"), std::get< bool >(menu::config::config_map[features::player_item_esp]));

            renderer::imgui::elements::functions::color_picker("##5", std::get< engine::structures::vector4_t >(menu::config::config_map[features::player_item_color]).color);

            renderer::imgui::elements::functions::separator();

            renderer::imgui::elements::functions::checkbox(xs("Generator"), xs("draws generators"), std::get< bool >(menu::config::config_map[features::generator_esp]));

            renderer::imgui::elements::functions::color_picker("##6", std::get< engine::structures::vector4_t >(menu::config::config_map[features::generator_color]).color);

            renderer::imgui::elements::functions::separator();

            renderer::imgui::elements::functions::checkbox(xs("Generator Distance"), xs("draws generator distances"), std::get< bool >(menu::config::config_map[features::generator_distance_esp]));

            renderer::imgui::elements::functions::color_picker("##7", std::get< engine::structures::vector4_t >(menu::config::config_map[features::generator_distance_color]).color);

            renderer::imgui::elements::functions::separator();

            renderer::imgui::elements::functions::checkbox(xs("Generator Percent"), xs("draws generator progress"), std::get< bool >(menu::config::config_map[features::generator_percent_esp]));

            renderer::imgui::elements::functions::color_picker("##8", std::get< engine::structures::vector4_t >(menu::config::config_map[features::generator_percent_color]).color);

            renderer::imgui::elements::functions::separator();

            renderer::imgui::elements::functions::checkbox(xs("Ignore Completed Generators"), xs("ignores generators with 100% progress"), std::get< bool >(menu::config::config_map[features::generator_ignore_repaired]));

            renderer::imgui::elements::functions::separator();

            renderer::imgui::elements::functions::checkbox(xs("Health"), xs("draws health over campers"), std::get< bool >(menu::config::config_map[features::camper_health_esp]));

            renderer::imgui::elements::functions::color_picker("##9", std::get< engine::structures::vector4_t >(menu::config::config_map[features::camper_health_color]).color);

            renderer::imgui::elements::functions::separator();

            renderer::imgui::elements::functions::checkbox(xs("Hatch"), xs("draws the hatch"), std::get< bool >(menu::config::config_map[features::hatch_esp]));

            renderer::imgui::elements::functions::color_picker("##10", std::get< engine::structures::vector4_t >(menu::config::config_map[features::hatch_color]).color);

            renderer::imgui::elements::functions::separator();

            renderer::imgui::elements::functions::checkbox(xs("Hatch Distance"), xs("draws the hatch distance"), std::get< bool >(menu::config::config_map[features::hatch_distance_esp]));

            renderer::imgui::elements::functions::color_picker("##11", std::get< engine::structures::vector4_t >(menu::config::config_map[features::hatch_distance_color]).color);

            renderer::imgui::elements::functions::separator();

            renderer::imgui::elements::functions::checkbox(xs("Enemy Tracers"), xs("draws line towards the player/s considered enemy/es"), std::get< bool >(menu::config::config_map[features::enemy_tracers]));

            renderer::imgui::elements::functions::color_picker("##12", std::get< engine::structures::vector4_t >(menu::config::config_map[features::enemy_tracers_color]).color);
            break;
        case 4:
            renderer::imgui::elements::functions::button( xs( "Save Config" ), { 328, 35 }, renderer::imgui::elements::button_flags::BUTTON_PURPLE );

            ImGui::NewLine( );

            renderer::imgui::elements::functions::button( xs( "Load Config" ), { 328, 35 }, renderer::imgui::elements::button_flags::BUTTON_GRAY );
            break;
        default:
            break;
        }
    }
    ImGui::EndChild( );

    ImGui::PopStyleVar( );
}

void renderer::menu::draw( )
{
    modules::visuals::callback( );

    if ( renderer::values::renderer->is_menu_open )
    {
        ImGui::SetNextWindowSize( { 375, 770 }, ImGuiCond_Once );

        ImGui::Begin( xs( "DBD" ), nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove );
        {
            auto DrawList = ImGui::GetWindowDrawList( );
            auto Position = ImGui::GetWindowPos( );

            DrawList->AddText( renderer::imgui::elements::fonts::druk_wide_bold, 30, { Position.x + 24, Position.y + 36 }, ImColor{ 255, 255, 255 }, xs( "Dead By Daylight" ) );

            DrawList->AddRectFilled( { Position.x + 24, Position.y + 73 }, { Position.x + 24 + 187, Position.y + 73 + 2 }, ImColor{ 255, 255, 255, 10 } );
            DrawList->AddRectFilled( { Position.x + 24, Position.y + 73 }, { Position.x + 24 + 88, Position.y + 73 + 2 }, ImColor{ 127, 128, 246 } );

            ImGui::SetCursorPos( { 24, 107 } );

            ImGui::BeginGroup( );
            {
                renderer::imgui::elements::functions::tab( xs( "LEGIT" ), (const char*)ICON_FA_SHIELD_CHECK, current_tab, 0 );

                ImGui::SameLine( );

                renderer::imgui::elements::functions::tab( xs( "RAGE" ), (const char*)ICON_FA_SHIELD, current_tab, 1 );

                ImGui::SameLine( );

                renderer::imgui::elements::functions::tab( xs( "MISC" ), (const char*)ICON_FA_FOLDER, current_tab, 2 );

                ImGui::SameLine( );

                renderer::imgui::elements::functions::tab( xs( "VISUALS" ), (const char*)ICON_FA_BOLT, current_tab, 3 );

                ImGui::SameLine( );

                renderer::imgui::elements::functions::tab( xs( "CFG" ), (const char*)ICON_FA_ARCHIVE, current_tab, 4 );

            }
            ImGui::EndGroup( );

            Draw_Main( 0.f, 0.f );
        }
        ImGui::End( );
    }
    else
    {
        ImGui::Begin( xs( "DBD_Watermark" ), nullptr, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoMove );
        { 
            auto DrawList = ImGui::GetWindowDrawList( );

            ImGui::SetWindowSize( { 220, 40 } );

            ImGui::SetWindowPos( { 0, 0 } );

            DrawList->AddText( renderer::imgui::elements::fonts::druk_wide_bold, 25, { 15, 5 }, ImColor{ 255, 255, 255, 70 }, xs( "loaded" ) );
        }
        ImGui::End( );
    }
}