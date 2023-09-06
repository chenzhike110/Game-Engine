#include "visual/Widget.h"

#include "imgui.h"
#include "visual/MiniGL.h"
#include "imgui_internal.h"
#include "visual/MeshUtils.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include "Utils/Logger.h"
#include "Utils/Timing.h"
#include "Utils/FileSystem.h"

#include "Simulation/Simulation.h"
#include "Simulation/SimulationModel.h"

INIT_LOGGING
INIT_TIMING

using namespace PBD;
using namespace Utilities;

MeshWidget::MeshWidget(){}

void MeshWidget::initShaders()
{
	std::string vertFile = m_exePath + "/resources/shaders/vs_smooth.glsl";
	std::string fragFile = m_exePath + "/resources/shaders/fs_smooth.glsl";
	m_shader.compileShaderFile(GL_VERTEX_SHADER, vertFile);
	m_shader.compileShaderFile(GL_FRAGMENT_SHADER, fragFile);
	m_shader.createAndLinkProgram();
	m_shader.begin();
	m_shader.addUniform("modelview_matrix");
	m_shader.addUniform("projection_matrix");
	m_shader.addUniform("surface_color");
	m_shader.addUniform("shininess");
	m_shader.addUniform("specular_factor");
	m_shader.end();

	vertFile = m_exePath + "/resources/shaders/vs_smoothTex.glsl";
	fragFile = m_exePath + "/resources/shaders/fs_smoothTex.glsl";
	m_shaderTex.compileShaderFile(GL_VERTEX_SHADER, vertFile);
	m_shaderTex.compileShaderFile(GL_FRAGMENT_SHADER, fragFile);
	m_shaderTex.createAndLinkProgram();
	m_shaderTex.begin();
	m_shaderTex.addUniform("modelview_matrix");
	m_shaderTex.addUniform("projection_matrix");
	m_shaderTex.addUniform("surface_color");
	m_shaderTex.addUniform("shininess");
	m_shaderTex.addUniform("specular_factor");
	m_shaderTex.end();

	vertFile = m_exePath + "/resources/shaders/vs_flat.glsl";
	std::string geomFile = m_exePath + "/resources/shaders/gs_flat.glsl";
	fragFile = m_exePath + "/resources/shaders/fs_flat.glsl";
	m_shaderFlat.compileShaderFile(GL_VERTEX_SHADER, vertFile);
	m_shaderFlat.compileShaderFile(GL_GEOMETRY_SHADER, geomFile);
	m_shaderFlat.compileShaderFile(GL_FRAGMENT_SHADER, fragFile);
	m_shaderFlat.createAndLinkProgram();
	m_shaderFlat.begin();
	m_shaderFlat.addUniform("modelview_matrix");
	m_shaderFlat.addUniform("projection_matrix");
	m_shaderFlat.addUniform("surface_color");
	m_shaderFlat.addUniform("shininess");
	m_shaderFlat.addUniform("specular_factor");
	m_shaderFlat.end();

}

void MeshWidget::init(int argc, char **argv)
{
    IMGUI_CHECKVERSION();
    ImGuiContext* m_context = ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); 
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    MiniGL::init(argc, argv, 1280, 1024, "Test", false, false);
	MiniGL::initLights();
	MiniGL::initTexture();

    MiniGL::addKeyboardFunc([](int key, int scancode, int action, int mods) -> bool { ImGui_ImplGlfw_KeyCallback(MiniGL::getWindow(), key, scancode, action, mods); return ImGui::GetIO().WantCaptureKeyboard; });
	MiniGL::addCharFunc([](int key, int action) -> bool { ImGui_ImplGlfw_CharCallback(MiniGL::getWindow(), key); return ImGui::GetIO().WantCaptureKeyboard; });
	MiniGL::addMousePressFunc([](int button, int action, int mods) -> bool { ImGui_ImplGlfw_MouseButtonCallback(MiniGL::getWindow(), button, action, mods); return ImGui::GetIO().WantCaptureMouse; });
	MiniGL::addMouseWheelFunc([](int pos, double xoffset, double yoffset) -> bool { ImGui_ImplGlfw_ScrollCallback(MiniGL::getWindow(), xoffset, yoffset); return ImGui::GetIO().WantCaptureMouse; });

	// apply user settings from ini file 

	// ImGuiIO& io = ImGui::GetIO(); (void)io;
	
	// std::string font = Utilities::FileSystem::normalizePath(m_base->getExePath() + "/resources/fonts/Roboto-Medium.ttf");
	// std::string font2 = Utilities::FileSystem::normalizePath(m_base->getExePath() + "/resources/fonts/Cousine-Regular.ttf");

	// m_scales.push_back(1.0f);
	// m_scales.push_back(1.25f);
	// m_scales.push_back(1.5f);
	// m_scales.push_back(1.75f);
	// m_scales.push_back(2.0f);

	// for(int i=0; i < 5; i++)
	// 	m_fonts.push_back(io.Fonts->AddFontFromFileTTF(font.c_str(), m_baseSize * m_scales[i]));
	// for (int i = 0; i < 5; i++)
	// 	m_fonts2.push_back(io.Fonts->AddFontFromFileTTF(font2.c_str(), m_baseSize * m_scales[i]));

	// initStyle();
	if (MiniGL::checkOpenGLVersion(3, 3))
		m_exePath = FileSystem::getProgramPath();

	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(MiniGL::getWindow(), false);
	const char* glsl_version = "#version 330";
	ImGui_ImplOpenGL3_Init(glsl_version);
    
}

void MeshWidget::shaderFlatBegin(const float* col)
{
	m_shaderFlat.begin();
	glUniform1f(m_shaderFlat.getUniform("shininess"), 5.0f);
	glUniform1f(m_shaderFlat.getUniform("specular_factor"), 0.2f);

	GLfloat matrix[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, matrix);
	glUniformMatrix4fv(m_shaderFlat.getUniform("modelview_matrix"), 1, GL_FALSE, matrix);
	GLfloat pmatrix[16];
	glGetFloatv(GL_PROJECTION_MATRIX, pmatrix);
	glUniformMatrix4fv(m_shaderFlat.getUniform("projection_matrix"), 1, GL_FALSE, pmatrix);
	glUniform3fv(m_shaderFlat.getUniform("surface_color"), 1, col);
}

void MeshWidget::shaderFlatEnd()
{
	m_shaderFlat.end();
}

void MeshWidget::shaderBegin(const float *col)
{
	m_shader.begin();
	glUniform1f(m_shader.getUniform("shininess"), 5.0f);
	glUniform1f(m_shader.getUniform("specular_factor"), 0.2f);

	GLfloat matrix[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, matrix);
	glUniformMatrix4fv(m_shader.getUniform("modelview_matrix"), 1, GL_FALSE, matrix);
	GLfloat pmatrix[16];
	glGetFloatv(GL_PROJECTION_MATRIX, pmatrix);
	glUniformMatrix4fv(m_shader.getUniform("projection_matrix"), 1, GL_FALSE, pmatrix);
	glUniform3fv(m_shader.getUniform("surface_color"), 1, col);
}

void MeshWidget::shaderEnd()
{
	m_shader.end();
}

void MeshWidget::renderRigidBodys()
{
	SimulationModel *model = Simulation::getCurrent()->getModel();
	SimulationModel::RigidBodyVector &rb = model->getRigidBodies();

	float surfaceColor[4] = { 0.1f, 0.4f, 0.7f, 1 };
	float staticColor[4] = { 0.5f, 0.5f, 0.5f, 0.5 };

	for (size_t i = 0; i < rb.size(); i++)
	{
		const VertexData &vd = rb[i]->getGeometry().getVertexData();
		const IndexedFaceMesh &mesh = rb[i]->getGeometry().getMesh();

		if (mesh.getFlatShading())
			shaderFlatBegin(staticColor);
		else
			shaderBegin(staticColor);

		if (rb[i]->getMass() == 0.0)
		{
			glUniform3fv(m_shader.getUniform("surface_color"), 1, staticColor);
			MESH::drawMesh(vd, mesh, 0, staticColor);
		}
		else
		{
			glUniform3fv(m_shader.getUniform("surface_color"), 1, surfaceColor);
			MESH::drawMesh(vd, mesh, 0, surfaceColor);
		}

		if (mesh.getFlatShading())
			shaderFlatEnd();
		else
			shaderEnd();
	}
}

void MeshWidget::render()
{
	float gridColor[4] = { 0.2f, 0.2f, 0.2f, 1.0f };
	MiniGL::drawGrid_xz(gridColor);
	MiniGL::coordinateSystem();

	// Draw sim model	
	SimulationModel *model = Simulation::getCurrent()->getModel();
	if (model == nullptr)
	{
		update();
		return;
	}
	renderRigidBodys();
	update();

}

void MeshWidget::update()
{
	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	// init dock space
	static ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_PassthruCentralNode | ImGuiDockNodeFlags_NoDockingInCentralNode | ImGuiDockNodeFlags_AutoHideTabBar;
	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	ImGuiViewport* viewport = ImGui::GetMainViewport();

	// add enough space for the menubar
	ImVec2 pos = viewport->Pos;
	ImVec2 size = viewport->Size;

	ImGui::SetNextWindowPos(pos);
	ImGui::SetNextWindowSize(size);
	ImGui::SetNextWindowViewport(viewport->ID);
	windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	if (dockspaceFlags & ImGuiDockNodeFlags_PassthruCentralNode) 
		windowFlags |= ImGuiWindowFlags_NoBackground;

	// ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	// ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	// ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

	// ImGui::PopStyleVar(3);

	// ImGui::PopFont();

	// Rendering
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}