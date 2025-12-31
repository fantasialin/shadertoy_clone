// ShaderToy-like editor with ImGui - Desktop OpenGL
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <iostream>
#include <fstream>
#include <sstream>

#include <vector>
#include <cmath>
#include <random>

#include <filesystem>  // C++17 standard library
//#include <windows.h>

#include <string>
#include <list>
#include <map>
#include "Fonts/Fonts.h"

using namespace std;

std::map<std::string,ImFont*> Fonts;  // font map

#define ENABLE_DEBUG_LOG 0

static void glfw_error_callback(int error, const char* description) {
    std::cerr << "GLFW Error " << error << ": " << description << std::endl;
}

// Default ShaderToy-style shader (plasma effect)
const char* defaultFragmentShader = R"(
#version 330 core
out vec4 fragColor;

uniform vec3 iResolution;   // viewport resolution (in pixels)
uniform float iTime;        // shader playback time (in seconds)
uniform vec4 iMouse;        // xy: current (if MLB down), zw: click

void main()
{
    vec2 uv = gl_FragCoord.xy / iResolution.xy;
    uv = uv * 2.0 - 1.0;
    uv.x *= iResolution.x / iResolution.y;

    float t = iTime;
    vec3 col = 0.5 + 0.5 * cos(t + uv.xyx + vec3(0,2,4));

    // Add mouse interaction
    if (iMouse.z > 0.0) {
        vec2 mouse = (iMouse.xy / iResolution.xy) * 2.0 - 1.0;
        mouse.x *= iResolution.x / iResolution.y;
        float dist = length(uv - mouse);
        col += smoothstep(0.2, 0.0, dist) * vec3(1.0, 0.8, 0.2);
    }

    fragColor = vec4(col, 1.0);
}
)";

class ShaderFBO {
  public:
    inline ShaderFBO() {}
    inline ~ShaderFBO() { free(); }
    inline operator GLuint() const { return FBO_id; }
    inline GLuint get_tex_id() { return ColorTexture_id; }

    bool init(unsigned int width_, unsigned int height_){
        // Delete old if exists
        if (glIsFramebuffer(FBO_id)) glDeleteFramebuffers(1, &FBO_id);
        if (glIsTexture(ColorTexture_id)) glDeleteTextures(1, &ColorTexture_id);

        width = width_;
        height = height_;
        status_ok = false;

        glGenFramebuffers(1, &FBO_id);
        glGenTextures(1, &ColorTexture_id);

        glBindTexture(GL_TEXTURE_2D, ColorTexture_id);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glBindFramebuffer(GL_FRAMEBUFFER, FBO_id);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ColorTexture_id, 0);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cerr << "FBO creation failed!" << std::endl;
        }
        else{
            status_ok = true;
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return status_ok;
    };

    void free(){
        if(FBO_id){
            end();
            glDeleteFramebuffers(1, &FBO_id);
            FBO_id = 0;
        }
    };

    bool begin(){
        if(FBO_id){
            end();
            glBindFramebuffer(GL_FRAMEBUFFER, FBO_id);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ColorTexture_id, 0);
            status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
            return (status == GL_FRAMEBUFFER_COMPLETE);
        }
        else{
            return false;
        }
    };

    void end(){
        if(FBO_id){
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 0, 0, 0);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            status = 0;
        }
    };

  private:
    GLenum status = 0;
    bool status_ok = {false};//true as ok
    unsigned int width = {0};
    unsigned int height ={0};
    GLuint FBO_id = {0};
    GLuint ColorTexture_id = {0};
};

class ShaderEditor {
public:
    std::string code;
    GLuint program = 0;
    std::string lastError;
    bool needsRecompile = true;

    ShaderEditor() {
        code = defaultFragmentShader;
        compile();
    }

    ~ShaderEditor() {
        if (program) glDeleteProgram(program);
    }

    bool compile() {
        if (!needsRecompile) return true;

        const char* vertexSrc = R"(
        #version 330 core
        layout(location = 0) in vec2 aPos;
        void main() { gl_Position = vec4(aPos, 0.0, 1.0); }
        )";

        GLuint vs = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vs, 1, &vertexSrc, nullptr);
        glCompileShader(vs);

        GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
        const char* codeptr = (const char*)code.c_str();
        glShaderSource(fs, 1, &codeptr, nullptr);
        glCompileShader(fs);

        GLint success;
        char infoLog[512];

        glGetShaderiv(fs, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(fs, 512, nullptr, infoLog);
            lastError = std::string("Fragment shader compilation failed:\n") + infoLog;
            glDeleteShader(vs);
            glDeleteShader(fs);
            needsRecompile = false;
            return false;
        }

        GLuint newProgram = glCreateProgram();
        glAttachShader(newProgram, vs);
        glAttachShader(newProgram, fs);
        glLinkProgram(newProgram);

        glGetProgramiv(newProgram, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(newProgram, 512, nullptr, infoLog);
            lastError = std::string("Program linking failed:\n") + infoLog;
            glDeleteProgram(newProgram);
            glDeleteShader(vs);
            glDeleteShader(fs);
            needsRecompile = false;
            return false;
        }

        if (program) glDeleteProgram(program);
        program = newProgram;

        glDeleteShader(vs);
        glDeleteShader(fs);
        lastError = "Shader compiled successfully!";
        needsRecompile = false;
        return true;
    }
};

std::vector<std::filesystem::directory_entry> ListDirectory(const std::string& path)
{
    std::vector<std::filesystem::directory_entry> entries;
    for (const auto& entry : std::filesystem::directory_iterator(path)){
        entries.push_back(entry);
    }
    return entries;
}

ifstream& open_file(ifstream& in, const char* filename){
    in.close();
    in.clear();
    in.open(filename);
    return in;
}

std::vector<char> ReadFile(const char* filename)
{
    std::ifstream file(filename, std::ios::binary);
    return std::vector<char>(
        std::istreambuf_iterator<char>(file),
        std::istreambuf_iterator<char>()
    );
}

int main() {
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(1600, 900, "ImGui ShaderToy Clone", nullptr, nullptr);
    if (!window) { glfwTerminate(); return -1; }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW\n";
        return -1;
    }

    int selected_example = -1;
    #define size_shader_code_ 64 * 1024
    
    #if defined(__APPLE__)
    std::string root_path = std::string("./example");
    #else
    std::string root_path = std::string(".\\example");
    #endif
    std::vector<std::filesystem::directory_entry> example_entries = ListDirectory(root_path);
    if(ENABLE_DEBUG_LOG)
        std::cout << "list examples ... " << example_entries.size() << "\n";
    std::vector<std::string> example_;
    std::vector<const char*> example_c_strs;
    example_.reserve(example_entries.size());
    if (example_entries.size() > 1){
        //sorting entries: directories first, then files, both alphabetically
        std::sort(example_entries.begin(), example_entries.end(), [](const std::filesystem::directory_entry& a, const std::filesystem::directory_entry& b) {
            if (a.is_directory() != b.is_directory()){
                return a.is_directory() > b.is_directory();
            }
            return a.path().filename().string() < b.path().filename().string();
        });
        for(auto &f_ : example_entries){
            if (!f_.is_directory()){
                #if defined(__APPLE__)
                std::string tmp = root_path + std::string("/") + f_.path().filename().string();
                #else
                std::string tmp = root_path + std::string("\\") + f_.path().filename().string();
                #endif
                example_.push_back(tmp);
                if(ENABLE_DEBUG_LOG)
                    std::cout << " " << tmp << "\n";            
            }
        }
        example_c_strs.clear();
        example_c_strs.reserve(example_.size());
        for (auto& s : example_)
            example_c_strs.push_back(s.c_str());
        if(example_c_strs.size()>1)     
            selected_example = 0;
    }

    // Setup ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.IniFilename = nullptr; // Disable imgui.ini

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    ImGuiIO* m_io = nullptr;
    m_io = &ImGui::GetIO();
    auto target_font_size = 25.0f;
    ImFontConfig font_cfg;
    font_cfg.FontDataOwnedByAtlas = false;

    std::string font_name("Roboto Regular");
    Fonts[font_name] = m_io->Fonts->AddFontFromMemoryTTF(Roboto_Regular_ttf, Roboto_Regular_ttf_len, target_font_size, &font_cfg);

    // Fullscreen quad
    float quad[] = { -1,-1, 1,-1, -1,1, 1,1 };
    GLuint vao, vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

    ShaderFBO shader_fbo;
    shader_fbo.init(800, 600);

    ShaderEditor editor;

    float time = 0.0f;
    ImVec4 mouse = ImVec4(0,0,0,0); // x,y = pos, z = left button down

    // For text editor
    static bool auto_scroll = true;
    static bool scroll_to_bottom = false;

    float framecount = 0;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        time = (float)glfwGetTime();

        double mx, my;
        glfwGetCursorPos(window, &mx, &my);
        ImVec2 winSize = io.DisplaySize;

        mouse.x = (float)mx;
        mouse.y = winSize.y - (float)my;  // Flip Y
        mouse.z = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS ? 1.0f : 0.0f;

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Set initial positions/sizes (first frame only)
        static bool init_layout = true;
        if (init_layout) {
            ImGui::SetNextWindowPos(ImVec2(0, 0));
            ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x * 0.6f, io.DisplaySize.y));
            init_layout = false;
        }

        ImGui::Begin("Shader Preview", nullptr, ImGuiWindowFlags_NoCollapse);
        ImVec2 previewSize = ImGui::GetWindowSize();
        ImVec2 canvasSize = ImGui::GetContentRegionAvail();
        ImVec2 canvas_pos  = ImGui::GetCursorScreenPos();
        mouse.x = (float)mx-canvas_pos.x;
        mouse.y = canvasSize.y - (float)my + canvas_pos.y;  // Flip Y

        // Make background transparent
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0,0,0,0));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0,0));

        // Resize FBO if canvas size changed significantly
        static ImVec2 lastSize = ImVec2(0,0);
        if (fabs(canvasSize.x - lastSize.x) > 2 || fabs(canvasSize.y - lastSize.y) > 2) {
            if (canvasSize.x > 0 && canvasSize.y > 0) {
                shader_fbo.init((unsigned int)canvasSize.x, (unsigned int)canvasSize.y);
                lastSize = canvasSize;
            }
        }

        // Render shader to FBO
        if (editor.program && editor.compile() && canvasSize.x > 0 && canvasSize.y > 0) {
            shader_fbo.begin();
            glViewport(0, 0, (GLsizei)canvasSize.x, (GLsizei)canvasSize.y);
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            glUseProgram(editor.program);
            glUniform3f(glGetUniformLocation(editor.program, "iResolution"), canvasSize.x, canvasSize.y, 1.0f);
            glUniform1f(glGetUniformLocation(editor.program, "iTime"), time);
            glUniform4f(glGetUniformLocation(editor.program, "iMouse"), mouse.x, mouse.y,
                        mouse.z > 0 ? mouse.x : 0, mouse.z > 0 ? mouse.y : 0);
            glUniform1f(glGetUniformLocation(editor.program, "iFrame"), framecount);

            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

            shader_fbo.end();
        }

        // Display the rendered texture in ImGui
        ImGui::Image((ImTextureID)(intptr_t)shader_fbo.get_tex_id(), canvasSize, ImVec2(0,1), ImVec2(1,0));

        ImGui::PopStyleVar();
        ImGui::PopStyleColor();
        ImGui::End();

        // Editor Window (right side)
        ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.6f, 0));
        ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x * 0.4f, io.DisplaySize.y));

        ImGui::Begin("Fragment Shader Editor");
            static char shaderCode[size_shader_code_] = {0};
            static bool initialized = false;
            if (!initialized) {
                strcpy(shaderCode, defaultFragmentShader);
                editor.code = shaderCode;
                initialized = true;
            }

            if (ImGui::Button("Reset to Default")) {
                strcpy(shaderCode, defaultFragmentShader);
                editor.code = shaderCode;
                editor.needsRecompile = true;
            }
            ImGui::SameLine();
            if (ImGui::Button("Recompile")) {
                editor.code = shaderCode;
                editor.needsRecompile = true;
            }
            ImGui::SameLine();
            if (ImGui::Button("load example")) {
                bool run_ = false;
                if(selected_example >= 0 && selected_example <= example_c_strs.size()-1){
                    std::ifstream in;
                    if(!open_file(in, example_c_strs[selected_example])){
                        std::cout << "failed to open " << example_c_strs[selected_example] << endl;
                    }
                    else{
                        memset(shaderCode,0,size_shader_code_);
                        if(ENABLE_DEBUG_LOG){
                            for(string line; getline(in, line);){
                                std::cout << line << endl;
                            }
                        }
                        auto data = ReadFile(example_c_strs[selected_example]);
                        data.push_back('\0');
                        if(data.size()<(size_shader_code_ -1)){
                            memcpy(shaderCode,data.data(), data.size());
                        }
                        run_ = true;
                    }
                }
                if(run_){
                    editor.code = shaderCode;
                    editor.needsRecompile = true;
                }
            }
            ImGui::Combo("examples",&selected_example, example_c_strs.data(), example_c_strs.size());

            // Fixed InputTextMultiline - uses char buffer
            ImGuiInputTextFlags flags = ImGuiInputTextFlags_AllowTabInput;
            if (ImGui::InputTextMultiline("##source", shaderCode, sizeof(shaderCode),
                                        ImVec2(-1, ImGui::GetTextLineHeight() * 30), flags)) {
                editor.code = shaderCode;
                editor.needsRecompile = true;
            }

            // Compile status
            if (!editor.lastError.empty()) {
                ImVec4 color = editor.lastError.find("success") != std::string::npos ?
                            ImVec4(0, 1, 0, 1) : ImVec4(1, 0.3f, 0.3f, 1);
                ImGui::TextColored(color, "%s", editor.lastError.c_str());
            }

            ImGui::Text("Resolution: %.0f x %.0f | Time: %.2f | FPS: %.1f | mouse x,y %.0f,%.0f | canvas x, y %.0f,%.0f",
                        canvasSize.x, canvasSize.y, time, io.Framerate, mouse.x, mouse.y, canvas_pos.x, canvas_pos.y);
            if (selected_example >= 0)
                ImGui::Text("Selected: %s", example_c_strs[selected_example]);

        ImGui::End();

        // Reset viewport for ImGui
        glViewport(0, 0, (GLsizei)io.DisplaySize.x, (GLsizei)io.DisplaySize.y);
        glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Render ImGui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        framecount += 1;
    }

    // Cleanup
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}