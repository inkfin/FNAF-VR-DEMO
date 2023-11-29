#pragma once

// handle game logic here
namespace Game {

void Init();
// This is the dynamic tick
void UpdateDynamicStep(float deltaTime);
// This is the fixed tick
void UpdateFixedStep();
void Shutdown();

}
