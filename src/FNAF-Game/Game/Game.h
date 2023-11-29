#ifndef __GAME_H__
#define __GAME_H__

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

}

#endif // !__GAME_H__
