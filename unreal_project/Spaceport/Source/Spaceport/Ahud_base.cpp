#include "Ahud_base.h"


Ahud_base::Ahud_base(FObjectInitializer const & init) : AHUD(init) {}

void Ahud_base::saves_list(TArray<FString> const & saves) {}

void Ahud_base::saves_changed(TArray<Ffile_change> const & changes) {}

void Ahud_base::escape_pressed() {}
