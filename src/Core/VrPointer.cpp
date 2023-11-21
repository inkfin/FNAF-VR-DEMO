#include "VrPointer.h"
#include "InitShader.h"
#include "AttriblessRendering.h"
#include <glm/gtc/type_ptr.hpp>
#include <string>

static const std::string vertex_shader("pointer_vs.glsl");
static const std::string fragment_shader("pointer_fs.glsl");

void VrPointer::Init()
{
    mShader = InitShader(vertex_shader.c_str(), fragment_shader.c_str());
}

void VrPointer::SetPose(int hand, const glm::mat4& M)
{
    mMpose[hand] = M;
}


void VrPointer::Display()
{
    if (mEnabled[0] == false && mEnabled[1] == false) return;
    glUseProgram(mShader);

    const int color_loc = 54;
    glProgramUniform3fv(mShader, color_loc, 2, glm::value_ptr(mColor[0]));

    for (int i = 0; i < 2; i++)
    {
        if (mEnabled[i] == false) continue;

        const int M_loc = 0;
        glProgramUniformMatrix4fv(mShader, M_loc, 1, false, glm::value_ptr(mMpose[i]));

        const int hand_loc = 50;
        glUniform1i(hand_loc, i);

        const int pos_loc = 52;
        glm::vec4 pos(0.0f, 0.0f, 0.0f, 1.0f);
        glUniform3fv(pos_loc, 1, glm::value_ptr(pos));

        const int length_loc = 53;
        glUniform1f(length_loc, mLength[i]);

        const int mode_loc = 51;
        const int LINE_MODE = 0;
        const int POINT_MODE = 1;
        const int QUAD_MODE = 2; //Draw thick lines as a quad

        const int mode = QUAD_MODE;

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        if(mode== LINE_MODE)
        {
            glLineWidth(0.75f);
            glUniform1i(mode_loc, LINE_MODE); //LINE_MODE
            bind_attribless_vao();
            glDrawArrays(GL_LINES, 0, 2);
        }
        
        if (mode == QUAD_MODE)
        {
            glUniform1i(mode_loc, QUAD_MODE); //QUAD_MODE
            bind_attribless_vao();
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        }

        //polygon offset applied in shader
        //glUniform1i(mode_loc, POINT_MODE); //POINT_MODE
        //glDrawArrays(GL_POINTS, 0, 1);

        glDisable(GL_BLEND);
    }

   glBindVertexArray(0);
   
}