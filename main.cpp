#include <Common/Common.h>
#include <Visualization/MiniGL.h>

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

using namespace PBD;

int main(int argc, char **argv){
    
    IMGUI_CHECKVERSION();
    ImGuiContext* m_context = ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); 
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    MiniGL::init(argc, argv, 1280, 1024, "Test", false, false);
	MiniGL::initLights();
	MiniGL::initTexture();
	MiniGL::setViewport(40.0, 0.1f, 500.0, Vector3r(0.0, 3.0, 8.0), Vector3r(0.0, 0.0, 0.0));

    MiniGL::setClientSceneFunc(render);		

    Vector3r center = {0.0, 0.0, 0.0};
    float m_jointColor[4] = { 0.0f, 0.6f, 0.2f, 1 };

    MiniGL::drawSphere(center, 0.08f, m_jointColor);

    
	

    MiniGL::mainLoop();

    return 0;
}