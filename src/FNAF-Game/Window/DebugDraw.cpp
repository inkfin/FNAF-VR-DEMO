//
// Created by 11096 on 11/20/2023.
//

#include "DebugDraw.h"
#include <GL/glew.h>

void DebugDraw::Init() {
    glGenVertexArrays(1, &VAO_axis);
    glGenBuffers(1, &VBO_axis);

    // 绑定 VAO
    glBindVertexArray(VAO_axis);

    // 绑定 VBO，并传入顶点数据
    glBindBuffer(GL_ARRAY_BUFFER, VBO_axis);
    glBufferData(GL_ARRAY_BUFFER, sizeof(axisVertices), axisVertices, GL_STATIC_DRAW);

    // 设置顶点属性指针
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // 设置颜色属性指针
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // 解绑 VBO 和 VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void DebugDraw::DrawAxis() {
    glBindVertexArray(VAO_axis);
    glDrawArrays(GL_LINES, 0, 6); // 有 6 个顶点，绘制线段
    glBindVertexArray(0);
}