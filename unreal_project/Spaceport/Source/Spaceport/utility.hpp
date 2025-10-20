#pragma once

inline constexpr float meters = 100.0f;

template <typename T>
void call_real_soon(FTimerHandle& timer, T* t_ptr, void (T::*member_ptr)())
{
    t_ptr->GetWorldTimerManager().SetTimer(timer, t_ptr, member_ptr, 0.001, false);
}
