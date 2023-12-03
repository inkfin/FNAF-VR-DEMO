#pragma once

#include <Windows.h>
#include "JsonConfig.h"

// handle game logic here
namespace Game {

void Init();
// This is the dynamic tick
void UpdateDynamicStep(float deltaTime);
// This is the fixed tick
void UpdateFixedStep();
void Shutdown();

void GeneraCase(float deltaTime);
void DeadCase();

void StartScene();
void DeadLight(int mode);

void EndScene();
void WinCase();
void InitGameStart();
}
