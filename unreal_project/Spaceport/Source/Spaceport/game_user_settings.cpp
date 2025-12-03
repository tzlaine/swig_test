#include "Aplayer_controller_base.h"
#include "audio_assets.h"
#include "game_user_settings.h"
#include "utility.hpp"

#include <AudioDevice.h>
#include <Engine/Engine.h>
#include <HAL/IConsoleManager.h>


namespace {
    int current_aa_method()
    {
        IConsoleVariable * var = IConsoleManager::Get().FindConsoleVariable(
            TEXT("r.AntiAliasingMethod"));
        if (var)
            return var->GetInt();
        int const default_aa_method = AAM_TSR;
        return default_aa_method;
    }
}

Ugame_user_settings::Ugame_user_settings(FObjectInitializer const & init) :
    Super(init),
    texture_quality(Super::GetTextureQuality()),
    aa_method(current_aa_method()),
    aa_quality(Super::GetAntiAliasingQuality()),
    planet_detail(3)
{}

void Ugame_user_settings::apply_console_settings(Aplayer_controller_base & pc)
{
    IConsoleVariable * aa_method_var =
        IConsoleManager::Get().FindConsoleVariable(
            TEXT("r.AntiAliasingMethod"));
    if (ensure(aa_method_var)) {
        aa_method_var->Set(aa_method, ECVF_SetByGameSetting);
        aa_method_var->Set(aa_method, ECVF_SetByProjectSetting);
    }
    pc.ConsoleCommand(
        FString::Printf(TEXT("r.AntiAliasingMethod %d"), aa_method));

    switch (aa_quality) {
    case 0:
        pc.ConsoleCommand(TEXT("r.FXAA.Quality=0"));
        pc.ConsoleCommand(TEXT("r.TemporalAA.Quality=0"));
        pc.ConsoleCommand(TEXT("r.TSR.History.R11G11B10=1"));
        pc.ConsoleCommand(TEXT("r.TSR.History.ScreenPercentage=100"));
        pc.ConsoleCommand(TEXT("r.TSR.History.UpdateQuality=0"));
        pc.ConsoleCommand(TEXT("r.TSR.History.GrandReprojection=0"));
        pc.ConsoleCommand(TEXT("r.TSR.ShadingRejection.Flickering=0"));
        pc.ConsoleCommand(TEXT("r.TSR.Velocity.Extrapolation=0"));
        pc.ConsoleCommand(TEXT("r.TSR.RejectionAntiAliasingQuality=0"));
        break;
    case 1:
        pc.ConsoleCommand(TEXT("r.FXAA.Quality=1"));
        pc.ConsoleCommand(TEXT("r.TemporalAA.Quality=1"));
        pc.ConsoleCommand(TEXT("r.TSR.History.R11G11B10=1"));
        pc.ConsoleCommand(TEXT("r.TSR.History.ScreenPercentage=100"));
        pc.ConsoleCommand(TEXT("r.TSR.History.UpdateQuality=1"));
        pc.ConsoleCommand(TEXT("r.TSR.History.GrandReprojection=0"));
        pc.ConsoleCommand(TEXT("r.TSR.ShadingRejection.Flickering=0"));
        pc.ConsoleCommand(TEXT("r.TSR.Velocity.Extrapolation=0"));
        pc.ConsoleCommand(TEXT("r.TSR.RejectionAntiAliasingQuality=1"));
        break;
    case 2:
        pc.ConsoleCommand(TEXT("r.FXAA.Quality=3"));
        pc.ConsoleCommand(TEXT("r.TemporalAA.Quality=1"));
        pc.ConsoleCommand(TEXT("r.TSR.History.R11G11B10=1"));
        pc.ConsoleCommand(TEXT("r.TSR.History.ScreenPercentage=100"));
        pc.ConsoleCommand(TEXT("r.TSR.History.UpdateQuality=2"));
        pc.ConsoleCommand(TEXT("r.TSR.History.GrandReprojection=0"));
        pc.ConsoleCommand(TEXT("r.TSR.ShadingRejection.Flickering=1"));
        pc.ConsoleCommand(TEXT("r.TSR.Velocity.Extrapolation=0"));
        pc.ConsoleCommand(TEXT("r.TSR.RejectionAntiAliasingQuality=1"));
        break;
    default:
    case 3:
        pc.ConsoleCommand(TEXT("r.FXAA.Quality=4"));
        pc.ConsoleCommand(TEXT("r.TemporalAA.Quality=2"));
        pc.ConsoleCommand(TEXT("r.TSR.History.R11G11B10=1"));
        pc.ConsoleCommand(TEXT("r.TSR.History.ScreenPercentage=200"));
        pc.ConsoleCommand(TEXT("r.TSR.History.UpdateQuality=3"));
        pc.ConsoleCommand(TEXT("r.TSR.History.GrandReprojection=0"));
        pc.ConsoleCommand(TEXT("r.TSR.ShadingRejection.Flickering=1"));
        pc.ConsoleCommand(TEXT("r.TSR.Velocity.Extrapolation=1"));
        pc.ConsoleCommand(TEXT("r.TSR.RejectionAntiAliasingQuality=2"));
        break;
    case 4:
        pc.ConsoleCommand(TEXT("r.FXAA.Quality=5"));
        pc.ConsoleCommand(TEXT("r.TemporalAA.Quality=2"));
        pc.ConsoleCommand(TEXT("r.TSR.History.R11G11B10=0"));
        pc.ConsoleCommand(TEXT("r.TSR.History.ScreenPercentage=200"));
        pc.ConsoleCommand(TEXT("r.TSR.History.UpdateQuality=3"));
        pc.ConsoleCommand(TEXT("r.TSR.History.GrandReprojection=1"));
        pc.ConsoleCommand(TEXT("r.TSR.ShadingRejection.Flickering=1"));
        pc.ConsoleCommand(TEXT("r.TSR.Velocity.Extrapolation=1"));
        pc.ConsoleCommand(TEXT("r.TSR.RejectionAntiAliasingQuality=2"));
        break;
    }
}

void Ugame_user_settings::apply_volume_settings()
{
    if (unapplied_volume_settings_)
        apply_volume_settings_impl();
}

void Ugame_user_settings::LoadSettings(bool b)
{
    Super::LoadSettings(b);
    apply_impl();
}

void Ugame_user_settings::ApplySettings(bool b)
{
    apply_impl();
    Super::ApplySettings(b);
}

Ugame_user_settings * Ugame_user_settings::get()
{
    return Cast<Ugame_user_settings>(UGameUserSettings::GetGameUserSettings());
}

void Ugame_user_settings::apply_impl()
{
    Super::SetTextureQuality(texture_quality);
    if (auto * w = ::world()) {
        if (auto * pc = player_controller_base()) {
            apply_console_settings(*pc);
        }
    }
    apply_volume_settings_impl();
}

void Ugame_user_settings::apply_volume_settings_impl()
{
    FAudioDeviceHandle device = GEngine->GetActiveAudioDevice();
    if (!device || !have_audio_assets()) {
        // At LoadSettings time, there is no world, nor a player controller;
        // apply these later.
        unapplied_volume_settings_ = true;
        return;
    }

    for (auto const & [name, mix] : audio_assets().sound_mixes_) {
        if (!volume_levels.Contains(name))
            volume_levels.Add(name, 1.0f);
        device->SetSoundMixClassOverride(
            mix, sound_class_of(mix), volume_levels[name], 1.0f, 0.0f, true);
    }

    unapplied_volume_settings_ = false;
}
