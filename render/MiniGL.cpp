#include <iostream>

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "utils/ImGuiFileDialog.h"
#include "utils/utils.h"
#include "render/MiniGL.h"

#define IMGUI_HAS_VIEWPORT

using namespace CGE;

static void glfw_error_callback(int error, const char* description)
{
    std::cout << "GLFW Error" << error << ":" << description <<std::endl;
}

MiniGL::MiniGL():
    _display_w(1280), 
    _display_h(720),
    _open_dialog(false)
{
    init();
}

void MiniGL::init()
{
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) return;
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    // Create window with graphics context
    _window = glfwCreateWindow(1280, 720, "Game Engine 1.0", nullptr, nullptr);
    if (_window == nullptr) return;
    glfwMakeContextCurrent(_window);
    glfwSwapInterval(1); // Enable vsync

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(_window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // shaders for geometry
    // initShaders();
}

void MiniGL::initShaders()
{
    m_shader = Shader::Find("./resources/shader/unlit");

}

void test()
{
    // ImGuiIO& io = ImGui::GetIO();
    // ImGui::Begin("Menu");
    // ImGui::Text("Dear ImGui %s Backend Checker", ImGui::GetVersion());
    // ImGui::Text("io.BackendPlatformName: %s", io.BackendPlatformName ? io.BackendPlatformName : "NULL");
    // ImGui::Text("io.BackendRendererName: %s", io.BackendRendererName ? io.BackendRendererName : "NULL");
    // ImGui::Separator();
    
    // if (ImGui::TreeNode("0001: Renderer: Large Mesh Support"))
    // {
    //     ImDrawList* draw_list = ImGui::GetWindowDrawList();
    //     {
    //         static int vtx_count = 60000;
    //         ImGui::SliderInt("VtxCount##1", &vtx_count, 0, 100000);
    //         ImVec2 p = ImGui::GetCursorScreenPos();
    //         for (int n = 0; n < vtx_count / 4; n++)
    //         {
    //             float off_x = (float)(n % 100) * 3.0f;
    //             float off_y = (float)(n % 100) * 1.0f;
    //             ImU32 col = IM_COL32(((n * 17) & 255), ((n * 59) & 255), ((n * 83) & 255), 255);
    //             draw_list->AddRectFilled(ImVec2(p.x + off_x, p.y + off_y), ImVec2(p.x + off_x + 50, p.y + off_y + 50), col);
    //         }
    //         ImGui::Dummy(ImVec2(300 + 50, 100 + 50));
    //         ImGui::Text("VtxBuffer.Size = %d", draw_list->VtxBuffer.Size);
    //     }
    //     {
    //         static int vtx_count = 60000;
    //         ImGui::SliderInt("VtxCount##2", &vtx_count, 0, 100000);
    //         ImVec2 p = ImGui::GetCursorScreenPos();
    //         for (int n = 0; n < vtx_count / (10*4); n++)
    //         {
    //             float off_x = (float)(n % 100) * 3.0f;
    //             float off_y = (float)(n % 100) * 1.0f;
    //             ImU32 col = IM_COL32(((n * 17) & 255), ((n * 59) & 255), ((n * 83) & 255), 255);
    //             draw_list->AddText(ImVec2(p.x + off_x, p.y + off_y), col, "ABCDEFGHIJ");
    //         }
    //         ImGui::Dummy(ImVec2(300 + 50, 100 + 20));
    //         ImGui::Text("VtxBuffer.Size = %d", draw_list->VtxBuffer.Size);
    //     }
    //     ImGui::TreePop();
    // }
    // ImGui::End();
}

void MiniGL::dealMenu()
{
    if (_open_dialog) {
            ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File", ".*", ".");

            // display
            if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey", ImGuiWindowFlags_NoCollapse,  ImVec2(_display_w/4, _display_h/4), ImVec2(_display_w, _display_h), &_open_dialog)) 
            {
                // action if OK
                if (ImGuiFileDialog::Instance()->IsOk())
                {
                    std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
                    std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
                // action
                }
                // close
                ImGuiFileDialog::Instance()->Close();
            }
        }
}

void MiniGL::renderCore()
{
    // core
    ImGui::Begin("Core");

    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    // draw_list->AddCallback();

    ImGui::End();
}

void MiniGL::mainLoop()
{
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    while (!glfwWindowShouldClose(_window))
    {
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // menu bar
        if (ImGui::BeginMainMenuBar()) 
        {
            if (ImGui::BeginMenu("File")) 
            {
                if (ImGui::MenuItem("Open", "Ctrl+O"))
                { 
                   _open_dialog = true;
                }
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }
        dealMenu();

        // test();
        renderCore();
        
        ImGui::Render();
        glfwGetFramebufferSize(_window, &_display_w, &_display_h);
        glViewport(0, 0, _display_w, _display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
        glfwSwapBuffers(_window);
    }

}