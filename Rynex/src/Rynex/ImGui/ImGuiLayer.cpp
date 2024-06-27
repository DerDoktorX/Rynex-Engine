#include "rypch.h"
#include "ImGuiLayer.h"

#include "imgui.h"

#define IMGUI_IMPL_API
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include "Rynex/Core/Application.h"

//TEMPORARY
#include <GLFW/glfw3.h>
#include <glad/glad.h>


#include "ImGuizmo.h"

namespace Rynex {
   
	ImGuiLayer::ImGuiLayer()
		: Layer("ImGuiLayer")
	{
	}

	ImGuiLayer::~ImGuiLayer()
	{
	} 

	void ImGuiLayer::OnAttach()
	{
        IMGUI_CHECKVERSION();
		ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
       
        //io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;

        float fontSize = 17.0f * 1.0f;
        io.Fonts->AddFontFromFileTTF("Resources/fonts/Open_Sans/static/OpenSans-Bold.ttf", fontSize);
        io.FontDefault=io.Fonts->AddFontFromFileTTF("Resources/fonts/Open_Sans/static/OpenSans-Bold.ttf", fontSize);
        
		ImGui::StyleColorsDark();
        //ImGui::StyleColorsClassic();
        ImGuiStyle& style = ImGui::GetStyle();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            style.WindowRounding = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }
        SetDarkThemeColore();
       

        Application& app = Application::Get();
        GLFWwindow* window = static_cast<GLFWwindow*>(app.GetWindow().GetNativeWindow());

        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 410");
	}

	void ImGuiLayer::OnDetach()
	{
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
	}

    void ImGuiLayer::OnEvent(Event& e)
    {
        if (m_BlockEvents)
        {
            ImGuiIO& io = ImGui::GetIO();
            e.Handled |= e.IsInCategory(EventCategoryMouse) & io.WantCaptureMouse;
            e.Handled |= e.IsInCategory(EventCategoryKeyBord) & io.WantCaptureKeyboard;
        }
    }

	//void ImGuiLayer::OnUpdate()
	//{
    //    ImGuiIO& io = ImGui::GetIO();
    //    Application& app = Application::Get();
    //    io.DisplaySize = ImVec2(app.GetWindow().GetWidth(), app.GetWindow().GetHeight());
    //
    //    float time = (float)glfwGetTime();
    //    io.DeltaTime = m_Time > 0.0f ? (time - m_Time) : (1.0f /60.0f) ;
    //    m_Time = time;
    //
    //    ImGui_ImplOpenGL3_NewFrame();
    //    ImGui::NewFrame();
    //
    //    static bool show = true;
    //    ImGui::ShowDemoWindow(&show);
    //
    //    ImGui::Render();
    //    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	//}

    void ImGuiLayer::Begin()
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGuizmo::BeginFrame();
    }

    void ImGuiLayer::End()
    {
        ImGuiIO& io = ImGui::GetIO();
        Application& app = Application::Get();
        io.DisplaySize = ImVec2(app.GetWindow().GetWidth(), app.GetWindow().GetHeight());

        //Rendering
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }
       
    }

    void ImGuiLayer::SetDarkThemeColore()
    {
        auto& colors = ImGui::GetStyle().Colors;

        colors[ImGuiCol_WindowBg] = ImVec4{ 0.1f, 0.105f, 0.11f, 1.0f };

        // Heders
        colors[ImGuiCol_Header] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
        colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
        colors[ImGuiCol_HeaderActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

        // Buttons
        colors[ImGuiCol_Button] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
        colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
        colors[ImGuiCol_ButtonActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

        // Frag BG
        colors[ImGuiCol_FrameBg] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
        colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
        colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

        // Tabs
        colors[ImGuiCol_Tab] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
        colors[ImGuiCol_TabHovered] = ImVec4{ 0.38f, 0.2805f, 0.281f, 1.0f };
        colors[ImGuiCol_TabActive] = ImVec4{ 0.28f, 0.1505f, 0.151f, 1.0f };
        colors[ImGuiCol_TabUnfocused] = ImVec4{ 0.15f, 0.205f, 0.151f, 1.0f };
        colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };


        // Title
        colors[ImGuiCol_TitleBg] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
        colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
        colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.95f, 0.1505f, 0.951f, 1.0f };

    }

    void ImGuiLayer::OnImGuiRender()
    {
#if IM_GUI_DEMO_WINDOW
        static bool show = true;
        ImGui::ShowDemoWindow(&show);
#endif // IM_GUI_DEMO_WINDOW

       

        //ImGui::Begin();
    }

	//void ImGuiLayer::OnEvent(Event& event)
	//{
    //    EventDispatcher dispatcher(event);
    //    //RY_CORE_INFO(event.ToString());
    //    dispatcher.Dispatch<MouseButtenPressedEvent>(RY_BIND_EVENT_FN(ImGuiLayer::OnMouseButtenPressedEvent));
    //    dispatcher.Dispatch<MouseButtenRealsedEvent>(RY_BIND_EVENT_FN(ImGuiLayer::OnMouseButtenRealsedEvent));
    //    dispatcher.Dispatch<MouseSrolledEvent>(RY_BIND_EVENT_FN(ImGuiLayer::OnMouseSrolledEvent));
    //    dispatcher.Dispatch<MouseMoveEvent>(RY_BIND_EVENT_FN(ImGuiLayer::OnMouseMoveEvent));
    //
    //    dispatcher.Dispatch<KeyPressedEvent>(RY_BIND_EVENT_FN(ImGuiLayer::OnKeyPressedEvent));
    //    dispatcher.Dispatch<KeyReleasdEvent>(RY_BIND_EVENT_FN(ImGuiLayer::OnKeyReleasdEvent));
    //
    //    dispatcher.Dispatch<KeyTypedEvent>(RY_BIND_EVENT_FN(ImGuiLayer::OnKeyTypedEvent));
    //    dispatcher.Dispatch<WindowResizeEvent>(RY_BIND_EVENT_FN(ImGuiLayer::OnWindowResizeEvent));
	//}
    //bool ImGuiLayer::OnMouseButtenPressedEvent(MouseButtenPressedEvent& e)
    //{
    //    ImGuiIO& io  = ImGui::GetIO();
    //    io.MouseDown[e.GetMouseButton()] = true;
    //    return false;
    //}
    //bool ImGuiLayer::OnMouseButtenRealsedEvent(MouseButtenRealsedEvent& e)
    //{
    //    ImGuiIO& io = ImGui::GetIO();
    //    io.MouseDown[e.GetMouseButton()] = false;
    //    return false;
    //}
    //bool ImGuiLayer::OnMouseSrolledEvent(MouseSrolledEvent& e)
    //{
    //    ImGuiIO& io = ImGui::GetIO();
    //    io.MouseWheelH += e.GetXOffset();
    //    io.MouseWheel += e.GetYOffset();
    //    return false;
    //}
    //bool ImGuiLayer::OnMouseMoveEvent(MouseMoveEvent& e)
    //{
    //    ImGuiIO& io = ImGui::GetIO();
    //    io.MousePos = ImVec2(e.GetX(), e.GetY());
    //    return false;
    //}
    //bool ImGuiLayer::OnKeyPressedEvent(KeyPressedEvent& e)
    //{
    //
    //    ImGuiIO& io = ImGui::GetIO();
    //    io.KeysDown[e.GetKeyCode()] = true;
    //
    //    io.KeyCtrl = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
    //    io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
    //    io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
    //    io.KeySuper = io.KeysDown[GLFW_KEY_LEFT_SUPER] || io.KeysDown[GLFW_KEY_RIGHT_SUPER];
    //    return false;
    //}
    //bool ImGuiLayer::OnKeyReleasdEvent(KeyReleasdEvent& e)
    //{
    //    ImGuiIO& io = ImGui::GetIO();
    //    io.KeysDown[e.GetKeyCode()] = false;
    //    return false;
    //}
    //bool ImGuiLayer::OnKeyTypedEvent(KeyTypedEvent& e)
    //{
    //    ImGuiIO& io = ImGui::GetIO();
    //    int keyCode = e.GetKeyCode();
    //    if (keyCode > 0 && keyCode < 0x10000)
    //        io.AddInputCharacter((unsigned short)keyCode);
    //
    //    return false;
    //}
    //bool ImGuiLayer::OnWindowResizeEvent(WindowResizeEvent& e)
    //{
    //    ImGuiIO& io = ImGui::GetIO();
    //    io.DisplaySize = ImVec2(e.GetWidth(), e.GetHeight());
    //    io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
    //    glViewport(0, 0, e.GetWidth(), e.GetHeight());
    //    return false;
    //}

   
}