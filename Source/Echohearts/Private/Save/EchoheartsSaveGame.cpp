// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#include "Save/EchoheartsSaveGame.h"

UEchoheartsSaveGame::UEchoheartsSaveGame()
{
    SaveVersion = CurrentSaveVersion;
    SavedAt = FDateTime::Now();
}
