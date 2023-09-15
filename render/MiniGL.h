#ifndef _MINIGL_H_
#define _MINIGL_H_

#define GL_SILENCE_DEPRECATION
#include "render/Shader.h"
#include <GLFW/glfw3.h>

namespace CGE
{

class MiniGL
{
public:
    MiniGL();
    void mainLoop();
    void dealMenu();
    void renderCore();

private:
    void init();
    void initShaders();

    Shader m_shader;
    // Shader m_shaderFlat;
    // Shader m_shaderTex;

    GLFWwindow* _window;
    int _display_w, _display_h;
    bool _open_dialog;
};

}

#endif