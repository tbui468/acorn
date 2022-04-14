// Dear ImGui: standalone example application for GLFW + OpenGL 3, using programmable pipeline
// (GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan/Metal graphics context creation, etc.)
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#include <iostream>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <glad/glad.h>
#include <GLFW/glfw3.h> // Will drag system OpenGL headers

static int g_ver_div_pos = 1;
static int g_hor_div_pos = 4;
static int g_window_width = 1280;
static int g_window_height = 720;

static bool g_no_hor_animation = false;
static bool g_no_ver_animation = false;

// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

void clamp(int* pos, int left, int right) {
    if (*pos < left) *pos = left;
    if (*pos > right) *pos = right;
}

void shift_ver_div(int shift) {
    g_ver_div_pos += shift;
    clamp(&g_ver_div_pos, 0, 5);
}

void shift_hor_div(int shift) {
    g_hor_div_pos += shift;
    clamp(&g_hor_div_pos, 0, 5);
}

void set_hor_div(int value) {
    g_hor_div_pos = value;
    clamp(&g_hor_div_pos, 0, 5);
}

void focus_explorer() {
    if (g_ver_div_pos == 0)
        shift_ver_div(1);
    ImGui::SetWindowFocus("Explorer");
}

void focus_editor() {
    if (g_ver_div_pos == 5) {
        shift_ver_div(-1);
        set_hor_div(5);
        g_no_hor_animation = true;
    } else if (g_hor_div_pos == 0) {
        shift_hor_div(1);
    }
    ImGui::SetWindowFocus("Editor");
}

void focus_terminal() {
    if (g_ver_div_pos == 5) {
        shift_ver_div(-1);
        set_hor_div(0);
        g_no_hor_animation = true;
    } else if (g_hor_div_pos == 5) {
        shift_hor_div(-1);
    }
    ImGui::SetWindowFocus("Terminal");
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_H && action == GLFW_PRESS && mods == GLFW_MOD_CONTROL)
        shift_ver_div(-1);
    if (key == GLFW_KEY_L && action == GLFW_PRESS && mods == GLFW_MOD_CONTROL)
        shift_ver_div(1);
    if (key == GLFW_KEY_J && action == GLFW_PRESS && mods == GLFW_MOD_CONTROL)
        shift_hor_div(1);
    if (key == GLFW_KEY_K && action == GLFW_PRESS && mods == GLFW_MOD_CONTROL)
        shift_hor_div(-1);
    if (key == GLFW_KEY_B && action == GLFW_PRESS && mods == GLFW_MOD_CONTROL)
        focus_explorer();
    if (key == GLFW_KEY_N && action == GLFW_PRESS && mods == GLFW_MOD_CONTROL)
        focus_editor();
    if (key == GLFW_KEY_M && action == GLFW_PRESS && mods == GLFW_MOD_CONTROL)
        focus_terminal();
}

void window_size_callback(GLFWwindow* window, int width, int height) {
    g_window_width = width;
    g_window_height = height;
}

int main(int, char**)
{
    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100
    const char* glsl_version = "#version 100";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
    // GL 3.2 + GLSL 150
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

    //glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);

    // Create window with graphics context
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Acorn Editor", NULL, NULL);
    if (window == NULL)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);
    glfwSetWindowSizeCallback(window, window_size_callback);
    glfwSwapInterval(1); // Enable vsync


    //load using glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "[ERROR] Couldn't initialize GLAD" << std::endl;
        if (window != NULL) { glfwDestroyWindow(window); }
        glfwTerminate();
        return -1;
    }
    else
    {
        std::cout << "[INFO] GLAD initialized\n";
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != NULL);
    
    //ImFont* font = io.Fonts->AddFontDefault();
    

    // Our state
    bool show_demo_window = false;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    float font_size = 16.0f;
    float vert_padding = 8.0f;
    ImFont* font = io.Fonts->AddFontFromFileTTF("EffectsEighty.ttf", font_size);
    ImVec2 frame_padding = ImVec2(8.0f, vert_padding);
    float decoration_height = font_size + vert_padding * 2.0f;
    int current_sidebar_width = g_ver_div_pos * (g_window_width / 5);
    int current_editor_height = g_hor_div_pos * (g_window_height / 5);

    // Main loop
    while (!glfwWindowShouldClose(window))
    {

        //animate vertical bar
        int target_width = g_ver_div_pos * (g_window_width / 5);
        if (g_no_ver_animation) {
            current_sidebar_width = target_width;
            g_no_ver_animation = false;
        } else {
            if (current_sidebar_width != target_width) {
                if (abs(current_sidebar_width - target_width) < 5)
                    if (current_sidebar_width - target_width > 0)
                        current_sidebar_width--;
                    else
                        current_sidebar_width++;
                else {
                    current_sidebar_width += (target_width - current_sidebar_width) / 5;
                }
            }
        }

        //animate horizontal bar
        int target_height = g_hor_div_pos * (g_window_height / 5);
        if (g_no_hor_animation) {
            current_editor_height = target_height;
            g_no_hor_animation = false;
        } else {
            if (current_editor_height != target_height) {
                if (abs(current_editor_height - target_height) < 5)
                    if (current_editor_height - target_height > 0)
                        current_editor_height--;
                    else
                        current_editor_height++;
                else {
                    current_editor_height += (target_height - current_editor_height) / 5;
                }
            }
        }

        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::PushFont(font);

        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, frame_padding);

        bool explorer_focused = false;
        bool editor_focused = false;
        bool terminal_focused = false;

        {
            ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
            //ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
            ImGui::SetNextWindowSize(ImVec2(current_sidebar_width, ImGui::GetIO().DisplaySize.y));
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.1f, 0.1f, 0.1f, 1.0f)); // Set window background to grey
            //ImGui::Begin("tet", &open_window, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize);
            ImGui::Begin("Explorer", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
            explorer_focused = ImGui::IsWindowFocused();
            ImGui::Text("File tree goes here!");
            ImGui::End();

            ImGui::PopStyleColor();
            ImGui::PopStyleVar(1);
        }

        if (!show_demo_window) {
            ImGui::SetNextWindowPos(ImVec2(current_sidebar_width, 0.0f));
            ImGui::SetNextWindowBgAlpha(1.0f); // Transparent background
            //ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
            ImGui::SetNextWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x - current_sidebar_width, current_editor_height));
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));  
            ImGui::Begin("Editor", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
            editor_focused = ImGui::IsWindowFocused();
            //ImGui::Begin("Editor", &open_window, ImGuiWindowFlags_NoResize);
            static char text[1024 * 16] =
                "print(\"Hello world\")\n"
                "a = 24\n";

            static ImGuiInputTextFlags flags = ImGuiInputTextFlags_AllowTabInput;
            //ImGui::InputTextMultiline("##source", text, IM_ARRAYSIZE(text), ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 16), flags);
            ImGui::InputTextMultiline("##source", text, IM_ARRAYSIZE(text), ImVec2(-FLT_MIN, current_editor_height - decoration_height), flags);
            ImGui::End();
            ImGui::PopStyleVar(2);
        }

        {
            ImGui::SetNextWindowPos(ImVec2(current_sidebar_width, current_editor_height));
            //ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
            ImGui::SetNextWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x - current_sidebar_width, ImGui::GetIO().DisplaySize.y));
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.1f, 0.1f, 0.1f, 1.0f)); // Set window background to grey
            //ImGui::Begin("tet", &open_window, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize);
            ImGui::Begin("Terminal", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
            terminal_focused = ImGui::IsWindowFocused();
            ImGui::Text("User can type terminal commands here!");
            ImGui::End();
            ImGui::PopStyleColor();
            ImGui::PopStyleVar(1);
        }

        ///////THIS NEEDS TO BE REDONE WITH NESTED CONDITIONS/////////////
        
        switch (g_hor_div_pos) {
            case 0: //editor completely hidden
                switch (g_ver_div_pos) {
                    case 0:
                        //only terminal showing
                        ImGui::SetWindowFocus("Terminal");
                        break;
                    case 5:
                        //only explorer showing
                        ImGui::SetWindowFocus("Explorer");
                        break;
                    default:
                        //both terminal and explorer showing
                        ImGui::SetWindowFocus("Explorer");
                        ImGui::SetWindowFocus("Terminal");
                        if (explorer_focused) ImGui::SetWindowFocus("Explorer");
                        break;
                }
                break;
            case 5: //terminal completely hidden
                switch (g_ver_div_pos) {
                    case 0:
                        //only editor showing
                        ImGui::SetWindowFocus("Editor");
                        break;
                    case 5:
                        //only explorer showing
                        ImGui::SetWindowFocus("Explorer");
                        break;
                    default:
                        //both editor and explorer showing
                        ImGui::SetWindowFocus("Explorer");
                        ImGui::SetWindowFocus("Editor");
                        if (explorer_focused) ImGui::SetWindowFocus("Explorer");
                        break;
                }
                break;
            default: //both editor and terminal possibly visible
                switch (g_ver_div_pos) {
                    case 0:
                        //explorer hidden
                        ImGui::SetWindowFocus("Terminal");
                        ImGui::SetWindowFocus("Editor");
                        if (terminal_focused) ImGui::SetWindowFocus("Terminal");
                        break;
                    case 5:
                        //only explorer showing
                        ImGui::SetWindowFocus("Explorer");
                        break;
                    default:
                        //all three windows showing
                        break;
                }
                break;
        }


        ImGui::PopFont();
        ImGui::PopStyleVar(1); //Framepadding

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
