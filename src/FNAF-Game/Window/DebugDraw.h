//
// Created by 11096 on 11/20/2023.
//

#pragma once

namespace {

unsigned int VBO_axis, VAO_axis;

}

namespace DebugDraw {

const static float axisVertices[] = {
    // X轴，红色
    0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
    1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,

    // Y轴，绿色
    0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

    // Z轴，蓝色
    0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f
};

void Init();

void DrawAxis();

}
