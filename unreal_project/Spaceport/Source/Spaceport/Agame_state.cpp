#include "Agame_state.h"
#include "Ahud_t.h"
#include "utility.hpp"

#include <Engine/World.h>
#include <Net/UnrealNetwork.h>


namespace {
    struct state_transtion
    {
        FString name_;
        std::function<void()> f_;
    };
    static_assert((int)play_state::ended + 1 == 7);
    state_transtion const remove_all{TEXT("remove_all_widgets"), [] {
                                         if (auto * hud = ::hud())
                                             hud->remove_all_widgets();
                                     }};
    state_transtion const show_main_menu{TEXT("show_main_menu"), [] {
                                             if (auto * hud = ::hud()) {
                                                 hud->remove_all_widgets();
                                                 hud->show_main_menu(false);
                                             }
                                         }};

    // Anything that needs to be done -- especially to the UI -- on a
    // play_state transition should go in the table below.  Note that
    // transitions like start_menu -> setup and setup -> generating do not,
    // because each simply pushes a modal (UCommonActivatableWidget) widget
    // onto the stack, hiding the previous widget(s).

    // clang-format off
    std::array<
        std::array<state_transtion, (int)play_state::ended + 1>,
        (int)play_state::ended + 1>
        g_state_transitions = {{
// from:       to:  start_menu        setup  waiting generating playing       paused        ended
/* start_menu  */ {{{show_main_menu}, {},    {},     {},        {remove_all}, {remove_all}, {}}},
/* setup       */ {{{show_main_menu}, {},    {},     {},        {remove_all}, {remove_all}, {}}},
/* waiting_... */ {{{show_main_menu}, {},    {},     {},        {remove_all}, {remove_all}, {}}},
/* generating  */ {{{show_main_menu}, {},    {},     {},        {remove_all}, {remove_all}, {}}},
/* playing     */ {{{show_main_menu}, {},    {},     {},        {remove_all}, {remove_all}, {}}},
/* paused      */ {{{show_main_menu}, {},    {},     {},        {remove_all}, {remove_all}, {}}},
/* ended       */ {{{show_main_menu}, {},    {},     {},        {remove_all}, {remove_all}, {}}}
        }};
    // clang-format on
}

Agame_state::Agame_state()
{
    bReplicates = true;
    bAlwaysRelevant = true;
    bOnlyRelevantToOwner = false;
}

bool Agame_state::playing_or_paused() const
{
    return ::playing_or_paused(play_state_);
}

void Agame_state::saves_changed()
{
    UE_LOG(
        LogTemp,
        Warning,
        TEXT("List of saves changed to: %s"),
        *FString::Join(saves_, TEXT(", ")));

    if (auto * const hud = ::hud())
        hud->saves_list(saves_);
}

void Agame_state::save_file_changes_changed()
{
    UE_LOG(LogTemp, Warning, TEXT("List of save file changes changed"));

    if (auto * const hud = ::hud())
        hud->saves_changed(save_file_changes_);
}

void Agame_state::play_state_changed()
{
    UE_LOG(
        LogTemp,
        Warning,
        TEXT("Client: play state changed, %s -> %s"),
        *UEnum::GetValueAsString(prev_play_state_),
        *UEnum::GetValueAsString(play_state_));

    if (auto const & transition =
            g_state_transitions[(int)prev_play_state_][(int)play_state_];
        transition.f_) {
        UE_LOG(
            LogTemp,
            Log,
            TEXT("Executing associated transition '%s'..."),
            *transition.name_);
        transition.f_();
    }
}

void Agame_state::play_speed_changed()
{
    UE_LOG(LogTemp, Warning, TEXT("Client: play speed=%d"), (int)play_speed_);
}

void Agame_state::GetLifetimeReplicatedProps(
    TArray<FLifetimeProperty> & OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(Agame_state, saves_);
    DOREPLIFETIME(Agame_state, save_file_changes_);
    DOREPLIFETIME(Agame_state, prev_play_state_);
    DOREPLIFETIME(Agame_state, play_state_);
    DOREPLIFETIME(Agame_state, play_speed_);
}
