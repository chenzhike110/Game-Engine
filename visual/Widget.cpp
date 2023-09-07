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

// namespace{
// typedef void (*ImGuiDemoMarkerCallback)(const char* file, int line, const char* section, void* user_data);
// extern ImGuiDemoMarkerCallback      GImGuiDemoMarkerCallback;
// extern void*                        GImGuiDemoMarkerCallbackUserData;
// ImGuiDemoMarkerCallback             GImGuiDemoMarkerCallback = NULL;
// void*                               GImGuiDemoMarkerCallbackUserData = NULL;
// }
// #define IMGUI_DEMO_MARKER(section)  do { if (GImGuiDemoMarkerCallback != NULL) GImGuiDemoMarkerCallback(__FILE__, __LINE__, section, GImGuiDemoMarkerCallbackUserData); } while (0)

INIT_LOGGING
INIT_TIMING

using namespace PBD;
using namespace Utilities;

MeshWidget::MeshWidget(){
	logger.addSink(std::unique_ptr<ConsoleSink>(new ConsoleSink(LogLevel::INFO)));
	logger.addSink(std::shared_ptr<BufferSink>(new BufferSink(LogLevel::DEBUG)));
}

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
	m_exePath = FileSystem::getProgramPath();

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
	if (MiniGL::checkOpenGLVersion(3, 3))
		initShaders();

	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(MiniGL::getWindow(), false);
	const char* glsl_version = "#version 330";
	ImGui_ImplOpenGL3_Init(glsl_version);
	// MiniGL::setClientSceneFunc([=]{render();});	
    
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
	float staticColor[4] = { 0.5f, 0.5f, 0.5f, 1 };

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
	size.y -= 15.0f;
	pos.y += 15.0f;

	ImGui::SetNextWindowPos(pos);
	ImGui::SetNextWindowSize(size);
	ImGui::SetNextWindowViewport(viewport->ID);
	windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	if (dockspaceFlags & ImGuiDockNodeFlags_PassthruCentralNode) 
		windowFlags |= ImGuiWindowFlags_NoBackground;
	
	ImGui::Begin("DockSpace", nullptr, windowFlags);
	ImGuiID dockspaceID = ImGui::GetID("DockSpace");
	ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), dockspaceFlags);
	static int first = true;
	if (first)
	{
		first = false;
		ImGui::DockBuilderRemoveNode(dockspaceID);
		ImGui::DockBuilderAddNode(dockspaceID, dockspaceFlags | ImGuiDockNodeFlags_DockSpace);
		ImGui::DockBuilderSetNodeSize(dockspaceID, viewport->Size);

		auto dock_id_down = ImGui::DockBuilderSplitNode(dockspaceID, ImGuiDir_Down, 0.3f, nullptr, &dockspaceID);
		auto dock_id_left = ImGui::DockBuilderSplitNode(dockspaceID, ImGuiDir_Left, 0.3f, nullptr, &dockspaceID);
		ImGui::DockBuilderDockWindow("Settings", dock_id_left);
		ImGui::DockBuilderDockWindow("Log", dock_id_down);

		ImGui::DockBuilderFinish(dockspaceID);
	}
	ImGui::End();

	if (ImGui::BeginMainMenuBar()) 
	{
		if (ImGui::BeginMenu("File")) 
		{
			if (ImGui::MenuItem("Create")) { 
			}
			if (ImGui::MenuItem("Open", "Ctrl+O")) { 
			}
			if (ImGui::MenuItem("Save", "Ctrl+S")) {
			}
			if (ImGui::MenuItem("Save as..")) { 
			}
		ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	// Rendering
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}