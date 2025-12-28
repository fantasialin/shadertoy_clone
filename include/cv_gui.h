#ifndef CV_APP_GUI_MAIN_H
#define CV_APP_GUI_MAIN_H

#include <iostream>
#include <string>
#include <map>
#include <chrono>
#include <thread>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "Fonts/Fonts.h"

namespace joe_cv_lib{

using namespace std;
using namespace std::placeholders;

using colorf = std::tuple<float, float, float, float>; //RGBA

/// Macro to disable console on Windows
#if defined(_WIN32) && defined(APP_NO_CONSOLE)
    #pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup")
#endif

class app_gui_main{
    typedef std::function<bool(vector<std::string>&)> drop_cb;
    typedef std::function<bool()> draw_main_cb;
  public:
    //default ctor
    app_gui_main(){};
    //ctor
    app_gui_main(std::string title, unsigned int width, unsigned int height);
    //dtor
    ~app_gui_main();

    virtual bool init() = 0;
    virtual bool update() = 0;
    // void handleKeyEvent(int key, int scancode, int action, int mods);
    // void handleMouseButtonEvent(int button, int action, int mods);
    // void handleCursorPosEvent(double xpos, double ypos);
    // void handleScrollEvent(double xoffset, double yoffset);
    // virtual void handleDropEvent(int path_count, const char* paths[]) = 0;
    static app_gui_main* Get(){ return g_instance;};
    static void drop_callback(GLFWwindow* window, int count, const char** paths);
    bool set_drop_callback(drop_cb cb){drop_cb_=cb; return true;};
    bool set_draw_main_func(draw_main_cb fptr){draw_main=fptr; return true;};

    //main loop
    bool run();

    bool load_font(std::string path_file, float font_size, float spacing_x=1.0f, float spacing_y=1.0f);

    GLuint createTextureFromRGBA(int width, int height, vector<unsigned char>& buffer);

    bool get_state();
    bool set_fps(float fps_) {fps = fps_; frame_duration = 1.0 / fps; return true;};
  protected:
    static app_gui_main* g_instance;
    GLFWwindow* window = nullptr;
    colorf backgroundColor;
    std::map<std::string,ImFont*> Fonts;  // font map
  private:
    drop_cb drop_cb_ = {nullptr};
    draw_main_cb draw_main = {nullptr};
    bool im_style = false;
    bool init_state = false;
    bool create_window = false;
    bool init_glew  = false;
    float fps = 30.0;
    double frame_duration = 1.0 / fps;  // in seconds
};

app_gui_main* app_gui_main::g_instance = nullptr;

app_gui_main::app_gui_main(std::string title, unsigned int width, unsigned int height){
    std::cout << __FUNCTION__ << "\n";
    g_instance = this;
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return;
    }
    init_state = true;

#if defined(__APPLE__)
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
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

    window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }
    create_window = true;

    //register callback
    glfwSetWindowUserPointer(window, static_cast<void*>(this));
    // glfwSetKeyCallback(m_window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
    //     { static_cast<App*>(glfwGetWindowUserPointer(window))->handleKeyEvent(key, scancode, action, mods); });
    // glfwSetMouseButtonCallback(m_window, [](GLFWwindow* window, int button, int action, int mods)
    //     { static_cast<App*>(glfwGetWindowUserPointer(window))->handleMouseButtonEvent(button, action, mods); });
    // glfwSetCursorPosCallback(m_window, [](GLFWwindow* window, double xpos, double ypos)
    //     { static_cast<App*>(glfwGetWindowUserPointer(window))->handleCursorPosEvent(xpos, ypos); });
    // glfwSetScrollCallback(m_window, [](GLFWwindow* window, double xoffset, double yoffset)
    //     { static_cast<App*>(glfwGetWindowUserPointer(window))->handleScrollEvent(xoffset, yoffset); });
    // glfwSetDropCallback(window, [](GLFWwindow* window, int path_count, const char* paths[])
    //     { static_cast<app_gui_main*>(glfwGetWindowUserPointer(window))->handleDropEvent(path_count, paths); });
    glfwSetDropCallback(window, &drop_callback);//for static function

    // Make the window's context current
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return;
    }
    init_glew = true;

    // setup ImGui binding @@
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
    ImGui::StyleColorsDark();

    backgroundColor = {0.15f, 0.16f, 0.21f, 1.00f}; //{0.45f, 0.55f, 0.60f, 1.00f}

    //set font
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->Clear();

    ImFontConfig font_cfg;
    font_cfg.FontDataOwnedByAtlas = false;

    ImFontConfig icons_config;
    icons_config.MergeMode = true;
    icons_config.PixelSnapH = true;
    icons_config.GlyphMinAdvanceX = 14.0f;
    icons_config.GlyphOffset = ImVec2(0, 0);
    icons_config.OversampleH = 1;
    icons_config.OversampleV = 1;
    icons_config.FontDataOwnedByAtlas = false;

    static const ImWchar fa_ranges[] = {ICON_MIN_FA, ICON_MAX_FA, 0};

    auto target_font_size = 30.0f;
    auto target_icon_size = 14.0f;

    if (im_style) {
        io.Fonts->AddFontDefault();
        io.Fonts->AddFontFromMemoryTTF(fa_solid_900_ttf, fa_solid_900_ttf_len, target_icon_size, &icons_config, fa_ranges);
    }

    if(true){
        std::string font_name("Roboto Bold");
        //assert(font_name.size() > 40-1);
        memcpy(font_cfg.Name, font_name.c_str(), font_name.size());
        Fonts[font_name] = io.Fonts->AddFontFromMemoryTTF(Roboto_Bold_ttf, Roboto_Bold_ttf_len, target_font_size, &font_cfg);
        io.Fonts->AddFontFromMemoryTTF(fa_solid_900_ttf, fa_solid_900_ttf_len, target_icon_size, &icons_config, fa_ranges);
    }

    if(true){
        std::string font_name("Roboto Italic");
        //assert(font_name.size() > 40-1);
        memcpy(font_cfg.Name, font_name.c_str(), font_name.size());
        Fonts[font_name] = io.Fonts->AddFontFromMemoryTTF(Roboto_Italic_ttf, Roboto_Italic_ttf_len, target_font_size, &font_cfg);
    }

    if(true){
        std::string font_name("Roboto Regular");
        //assert(font_name.size() > 40-1);
        Fonts[font_name] = io.Fonts->AddFontFromMemoryTTF(Roboto_Regular_ttf, Roboto_Regular_ttf_len, target_font_size, &font_cfg);
    }

    if(false){
        std::string font_name("Roboto Mono Bold");
        //assert(font_name.size() > 40-1);
        Fonts[font_name] = io.Fonts->AddFontFromMemoryTTF(RobotoMono_Bold_ttf, RobotoMono_Bold_ttf_len, target_font_size, &font_cfg);
    }

    if(false){
        std::string font_name("Roboto Mono Italic");
        //assert(font_name.size() > 40-1);
        Fonts[font_name] = io.Fonts->AddFontFromMemoryTTF(RobotoMono_Italic_ttf, RobotoMono_Italic_ttf_len, target_font_size, &font_cfg);
    }

    if(false){
        std::string font_name("Roboto Mono Regular");
        //assert(font_name.size() > 40-1);
        Fonts[font_name] = io.Fonts->AddFontFromMemoryTTF(RobotoMono_Regular_ttf, RobotoMono_Regular_ttf_len, target_font_size, &font_cfg);
    }
    std::cout << " font map size : " << Fonts.size() << "\n";
    //load_font(std::string("font/Roboto-Regular.ttf"), 40);
    // Rebuild the font atlas
    unsigned char* tex_pixels = nullptr;
    int tex_width, tex_height;
    io.Fonts->GetTexDataAsRGBA32(&tex_pixels, &tex_width, &tex_height);
    std::cout << " font text date --  tex width : " << tex_width << "   tex height : " << tex_height << "\n";

    // Upload texture to graphics system
    GLuint tex_id;
    glGenTextures(1, &tex_id);
    glBindTexture(GL_TEXTURE_2D, tex_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex_width, tex_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex_pixels);

    // Store our identifier
    io.Fonts->TexID = (void*)(intptr_t)tex_id;

    return;
}

app_gui_main::~app_gui_main(){
    std::cout << __FUNCTION__ << "\n";
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    return;
}

inline void app_gui_main::drop_callback(GLFWwindow *window, int count, const char **paths)
{
    std::cout << __FUNCTION__ << "\n";
    vector<std::string> drop_files;
    for (int i = 0; i < count; i++)
    {
        printf("Dropped file: %s\n", paths[i]);
        // You can push into a queue or store it somewhere for ImGui
        drop_files.push_back(std::string(paths[i]));
    }
    if(nullptr!=Get()->drop_cb_)
        Get()->drop_cb_(drop_files);
}

bool app_gui_main::run()
{
    init();
    auto [R, G, B, A] = backgroundColor;
    using Clock = std::chrono::high_resolution_clock;
    auto last_time = Clock::now();
    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        update();//menu UI or subwindows
        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(R, G, B, A);
        glClear(GL_COLOR_BUFFER_BIT);

        //this port is main window.. 
        if(draw_main)
            draw_main();

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);

        auto now = Clock::now();
        std::chrono::duration<double> elapsed = now - last_time;
        if (elapsed.count() < frame_duration) {
            std::this_thread::sleep_for(std::chrono::duration<double>(frame_duration - elapsed.count()));
        }
        last_time = Clock::now();
    }
    return true;
}

inline bool app_gui_main::load_font(std::string path_file, float font_size, float spacing_x, float spacing_y)
{
    ImGuiIO& io = ImGui::GetIO();
    ImFontConfig config;
    config.SizePixels = font_size;

    // load font
    io.Fonts->AddFontFromFileTTF(path_file.c_str(), font_size, &config);
    
    // spacing 
    config.GlyphExtraSpacing.x = spacing_x;
    config.GlyphExtraSpacing.y = spacing_y;
    return true;
}

inline GLuint app_gui_main::createTextureFromRGBA(int width, int height, vector<unsigned char> &buffer)
{
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, buffer.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    return texture;
}

inline bool app_gui_main::get_state()
{
    return (init_state && create_window && init_glew);
}

}//namespace joe_cv_lib

#endif //CV_APP_GUI_MAIN_H