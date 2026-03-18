/*----------------------------------------------------------------------------*/
/*    Copyright (C) 2015 Alexandre Campo                                      */
/*                                                                            */
/*    This file is part of USE Tracker.                                       */
/*                                                                            */
/*    USE Tracker is free software: you can redistribute it and/or modify     */
/*    it under the terms of the GNU General Public License as published by    */
/*    the Free Software Foundation, either version 3 of the License, or       */
/*    (at your option) any later version.                                     */
/*                                                                            */
/*    USE Tracker is distributed in the hope that it will be useful,          */
/*    but WITHOUT ANY WARRANTY; without even the implied warranty of          */
/*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           */
/*    GNU General Public License for more details.                            */
/*                                                                            */
/*    You should have received a copy of the GNU General Public License       */
/*    along with USE Tracker.  If not, see <http://www.gnu.org/licenses/>.    */
/*----------------------------------------------------------------------------*/

#include "AppGui.h"
#include "App.h"

#include <SDL.h>
#include <GL/gl.h>

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"

#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>

#include <portable-file-dialogs.h>

#include <iostream>
#include <algorithm>
#include <chrono>
#include <cstring>
#include <thread>
#include <cmath>

#include "Background.h"
#include "Pipeline.h"
#include "Capture.h"
#include "CaptureVideo.h"
#include "CaptureUSBCamera.h"
#include "CaptureImage.h"
#include "CaptureMultiUSBCamera.h"
#include "CaptureMultiVideo.h"
#include "CaptureDefault.h"

// Plugin headers
#include "plugins/AdaptiveThreshold.h"
#include "plugins/BackgroundDiffGMG.h"
#include "plugins/BackgroundDiffMOG.h"
#include "plugins/BackgroundDiffMOG2.h"
#include "plugins/ColorSegmentation.h"
#include "plugins/Dilation.h"
#include "plugins/Erosion.h"
#include "plugins/ExtractBlobs.h"
#include "plugins/ExtractMotion.h"
#include "plugins/FrameDifference.h"
#include "plugins/GetBlobsAngles.h"
#include "plugins/MovingAverage.h"
#include "plugins/RecordPixels.h"
#include "plugins/RecordVideo.h"
#include "plugins/RemoteControl.h"
#include "plugins/SafeErosion.h"
#include "plugins/SimpleTags.h"
#include "plugins/Stopwatch.h"
#include "plugins/TakeSnapshots.h"
#include "plugins/Tracker.h"
#include "plugins/ZonesOfInterest.h"

#ifdef ARUCO
#include "plugins/Aruco.h"
#endif

#include "plugins/ArucoColor.h"

extern Parameters parameters;
extern std::map<std::string, std::vector<PipelinePlugin*> (*)(cv::FileNode&, unsigned int)> NewPipelinePluginVector;

// Active tab indices
enum TabIndex { TAB_PROCESSING = 0, TAB_BACKGROUND, TAB_CALIBRATION, TAB_PROCFRAME };
static int activeTab = TAB_PROCESSING;

// Helper: input buffer size for string inputs
static const int INPUT_BUF_SIZE = 512;

// ============================================================================
// Constructor / Destructor
// ============================================================================

AppGui::AppGui()
{
    textColor = cv::Scalar(200, 200, 200, 255);

    // Populate available plugin names from the global map
    for (auto& kv : NewPipelinePluginVector)
    {
        availablePluginNames.push_back(kv.first);
    }
    std::sort(availablePluginNames.begin(), availablePluginNames.end());
}

AppGui::~AppGui()
{
    Cleanup();
}

// ============================================================================
// InitSDL
// ============================================================================

bool AppGui::InitSDL()
{
    // Check for display environment before SDL_Init (which may crash without one)
    const char* display = getenv("DISPLAY");
    const char* wayland = getenv("WAYLAND_DISPLAY");
    if (!display && !wayland)
    {
        std::cerr << "Error: No display server found (DISPLAY/WAYLAND_DISPLAY not set). "
                  << "Use -n/--nogui for headless mode." << std::endl;
        return false;
    }

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0)
    {
        std::cerr << "SDL_Init error: " << SDL_GetError() << std::endl;
        return false;
    }

    // OpenGL 3.2 Core
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    // Detect display size
    SDL_DisplayMode dm;
    if (SDL_GetCurrentDisplayMode(0, &dm) == 0)
    {
        // Size window to 80% of the display
        windowWidth = (int)(dm.w * 0.8f);
        windowHeight = (int)(dm.h * 0.8f);
    }

    Uint32 windowFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI;
    window = SDL_CreateWindow(
        "USE Tracker",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        windowWidth, windowHeight,
        windowFlags);

    if (!window)
    {
        std::cerr << "SDL_CreateWindow error: " << SDL_GetError() << std::endl;
        return false;
    }

    gl_context = SDL_GL_CreateContext(window);
    if (!gl_context)
    {
        std::cerr << "SDL_GL_CreateContext error: " << SDL_GetError() << std::endl;
        return false;
    }

    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(1); // vsync

    return true;
}

// ============================================================================
// InitImGui
// ============================================================================

bool AppGui::InitImGui()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();

    // Detect DPI scale
    // Method 1: framebuffer vs window size ratio (works for HiDPI/Retina)
    int ww, wh, fw, fh;
    SDL_GetWindowSize(window, &ww, &wh);
    SDL_GL_GetDrawableSize(window, &fw, &fh);
    float detectedScale = (ww > 0) ? (float)fw / (float)ww : 1.0f;

    // Method 2: SDL display DPI (works for high-res monitors with OS scaling)
    if (detectedScale <= 1.0f)
    {
        float ddpi = 0.0f;
        if (SDL_GetDisplayDPI(0, &ddpi, nullptr, nullptr) == 0 && ddpi > 0)
            detectedScale = ddpi / 96.0f;
    }

    if (detectedScale < 1.0f) detectedScale = 1.0f;
    dpiScale = detectedScale;

    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init("#version 150");

    ApplyUIScale();

    return true;
}

// ============================================================================
// ApplyUIScale — rebuild font and style for current dpiScale
// ============================================================================

void AppGui::ApplyUIScale()
{
    ImGuiIO& io = ImGui::GetIO();

    // Rebuild font at the new scale
    io.Fonts->Clear();
    ImFontConfig fontCfg;
    fontCfg.SizePixels = 13.0f * dpiScale;
    fontCfg.OversampleH = 2;
    fontCfg.OversampleV = 2;
    io.Fonts->AddFontDefault(&fontCfg);
    io.FontGlobalScale = 1.0f;
    io.Fonts->Build();
    ImGui_ImplOpenGL3_DestroyFontsTexture();
    ImGui_ImplOpenGL3_CreateFontsTexture();

    // Reset ALL style values to defaults (StyleColorsDark only resets colors, not sizes)
    ImGui::GetStyle() = ImGuiStyle();
    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    style.FrameRounding = 4.0f;
    style.GrabRounding = 4.0f;
    style.WindowRounding = 6.0f;
    style.ScaleAllSizes(dpiScale);
}

// ============================================================================
// Cleanup
// ============================================================================

void AppGui::Cleanup()
{
    if (videoTexture)
    {
        glDeleteTextures(1, &videoTexture);
        videoTexture = 0;
    }

    for (auto& kv : icons)
    {
        if (kv.second.id)
            glDeleteTextures(1, &kv.second.id);
    }
    icons.clear();

    if (ImGui::GetCurrentContext())
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();
    }

    if (gl_context)
    {
        SDL_GL_DeleteContext(gl_context);
        gl_context = nullptr;
    }
    if (window)
    {
        SDL_DestroyWindow(window);
        window = nullptr;
    }
    if (SDL_WasInit(0))
        SDL_Quit();
}

// ============================================================================
// LoadIconTexture
// ============================================================================

GLuint AppGui::LoadIconTexture(const std::string& path)
{
    cv::Mat img = cv::imread(path, cv::IMREAD_UNCHANGED);
    if (img.empty())
    {
        std::cerr << "Failed to load icon: " << path << std::endl;
        return 0;
    }

    // Convert to RGBA if needed
    cv::Mat rgba;
    if (img.channels() == 4)
        rgba = img;
    else if (img.channels() == 3)
        cv::cvtColor(img, rgba, cv::COLOR_BGR2RGBA);
    else if (img.channels() == 1)
        cv::cvtColor(img, rgba, cv::COLOR_GRAY2RGBA);
    else
        return 0;

    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, rgba.cols, rgba.rows, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, rgba.data);

    IconTex it;
    it.id = tex;
    it.w = rgba.cols;
    it.h = rgba.rows;
    icons[path] = it;

    return tex;
}

// ============================================================================
// UploadVideoTexture
// ============================================================================

void AppGui::UploadVideoTexture(const cv::Mat& frame)
{
    if (frame.empty()) return;

    cv::Mat rgb;
    if (frame.channels() == 3)
        cv::cvtColor(frame, rgb, cv::COLOR_BGR2RGB);
    else if (frame.channels() == 1)
        cv::cvtColor(frame, rgb, cv::COLOR_GRAY2RGB);
    else if (frame.channels() == 4)
        cv::cvtColor(frame, rgb, cv::COLOR_BGRA2RGB);
    else
        return;

    bool needsCreate = (videoTexture == 0) ||
                       (rgb.cols != texWidth) ||
                       (rgb.rows != texHeight);

    if (needsCreate)
    {
        if (videoTexture)
            glDeleteTextures(1, &videoTexture);

        glGenTextures(1, &videoTexture);
        glBindTexture(GL_TEXTURE_2D, videoTexture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, rgb.cols, rgb.rows, 0,
                     GL_RGB, GL_UNSIGNED_BYTE, rgb.data);
        texWidth = rgb.cols;
        texHeight = rgb.rows;
    }
    else
    {
        glBindTexture(GL_TEXTURE_2D, videoTexture);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, rgb.cols, rgb.rows,
                        GL_RGB, GL_UNSIGNED_BYTE, rgb.data);
    }
}

// ============================================================================
// Run - Main loop
// ============================================================================

int AppGui::Run()
{
    if (!InitSDL()) return 1;
    if (!InitImGui()) return 1;

    // Initialize capture if not already set
    if (!ipEngine.capture)
    {
        if (!parameters.inputFilename.empty())
        {
            // Try opening as video
            CaptureVideo* cv_cap = new CaptureVideo(parameters.inputFilename);
            if (cv_cap->type != Capture::NONE)
            {
                ipEngine.capture = cv_cap;
            }
            else
            {
                // Video failed, try as image
                delete cv_cap;
                CaptureImage* img_cap = new CaptureImage(parameters.inputFilename);
                if (img_cap->type != Capture::NONE)
                {
                    ipEngine.capture = img_cap;
                }
                else
                {
                    delete img_cap;
                    std::cerr << "Warning: Could not open " << parameters.inputFilename
                              << ", starting with empty capture" << std::endl;
                    ipEngine.capture = new CaptureDefault();
                }
            }
        }
        else if (parameters.usbDevice >= 0)
        {
            CaptureUSBCamera* usb = new CaptureUSBCamera(parameters.usbDevice);
            ipEngine.capture = usb;
        }
        else if (parameters.multiVideoCapture && !parameters.inputFilenames.empty())
        {
            CaptureMultiVideo* mv = new CaptureMultiVideo(parameters.inputFilenames);
            ipEngine.capture = mv;
        }
        else if (parameters.multiUSBCapture && !parameters.usbDevices.empty())
        {
            CaptureMultiUSBCamera* mu = new CaptureMultiUSBCamera(parameters.usbDevices);
            ipEngine.capture = mu;
        }
        else
        {
            ipEngine.capture = new CaptureDefault();
        }
    }

    // Set window title
    std::string title = "USE Tracker: " + ipEngine.capture->GetName();
    SDL_SetWindowTitle(window, title.c_str());

    // Initialize engine
    ResetEngine(parameters);

    // Main loop
    while (running)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);

            if (event.type == SDL_QUIT)
                running = false;

            if (event.type == SDL_WINDOWEVENT &&
                event.window.event == SDL_WINDOWEVENT_CLOSE &&
                event.window.windowID == SDL_GetWindowID(window))
                running = false;

            // Keyboard shortcuts
            if (event.type == SDL_KEYDOWN && !ImGui::GetIO().WantCaptureKeyboard)
            {
                bool ctrl = (event.key.keysym.mod & KMOD_CTRL) != 0;
                switch (event.key.keysym.sym)
                {
                case SDLK_SPACE:
                    play = !play;
                    if (play)
                        ipEngine.capture->Play();
                    else
                        ipEngine.capture->Pause();
                    break;

                case SDLK_RIGHT:
                    // Step forward one frame
                    ipEngine.GetNextFrame();
                    ipEngine.Step(hudVisible);
                    break;

                case SDLK_LEFT:
                    // Step backward one frame
                    if (ipEngine.capture && ipEngine.capture->GetFrameCount() > 0)
                    {
                        double t = ipEngine.capture->GetTime();
                        double fps = ipEngine.capture->GetFPS();
                        if (fps > 0)
                        {
                            ipEngine.capture->GetFrame(t - 1.0 / fps);
                            ipEngine.Step(hudVisible);
                        }
                    }
                    break;

                case SDLK_EQUALS:  // + or = key
                case SDLK_PLUS:
                case SDLK_KP_PLUS:
                    // Fast forward — skip 10 frames
                    for (int i = 0; i < 10; i++)
                    {
                        if (!ipEngine.GetNextFrame()) break;
                    }
                    ipEngine.Step(hudVisible);
                    break;

                case SDLK_MINUS:
                case SDLK_KP_MINUS:
                    // Rewind — jump back ~10 frames
                    if (ipEngine.capture && ipEngine.capture->GetFrameCount() > 0)
                    {
                        double t = ipEngine.capture->GetTime();
                        double fps = ipEngine.capture->GetFPS();
                        if (fps > 0)
                        {
                            ipEngine.capture->GetFrame(std::max(0.0, t - 10.0 / fps));
                            ipEngine.Step(hudVisible);
                        }
                    }
                    break;

                case SDLK_BACKSPACE:
                    // Reset to beginning
                    if (ipEngine.capture)
                    {
                        ipEngine.capture->Stop();
                        ipEngine.capture->GetNextFrame();
                        play = false;
                    }
                    break;

                case SDLK_r:
                    if (ctrl)
                    {
                        // Toggle recording/output
                        output = !output;
                        if (output)
                            ipEngine.OpenOutput();
                        else
                            ipEngine.CloseOutput();
                    }
                    break;

                case SDLK_o:
                    if (ctrl)
                        OpenSource();
                    break;

                case SDLK_l:
                    if (ctrl)
                        LoadSettings();
                    break;

                case SDLK_s:
                    if (ctrl)
                        SaveSettings();
                    break;

                case SDLK_ESCAPE:
                    running = false;
                    break;

                default:
                    break;
                }
            }
        }

        UpdateEngine();
        RenderFrame();

        SDL_GL_SwapWindow(window);
    }

    ipEngine.CloseOutput();

    return 0;
}

// ============================================================================
// UpdateEngine - Frame timing (derived from MainFrame::OnIdle)
// ============================================================================

void AppGui::UpdateEngine()
{
    if (!ipEngine.capture) return;

    // Update pipeline snapshot position
    if (selectedPipelineItem >= 0 &&
        selectedPipelineItem < (int)ipEngine.pipelines[0].plugins.size())
        ipEngine.snapshotPos = selectedPipelineItem;
    else if (!ipEngine.pipelines.empty() && !ipEngine.pipelines[0].plugins.empty())
        ipEngine.snapshotPos = ipEngine.pipelines[0].plugins.size() - 1;

    // Estimate time to wait before getting next frame
    bool getNextFrame = true;
    int64_t currentTime = GetUTCTimeUSec();

    int64_t td = ipEngine.capture->GetNextFrameSystemTime() - currentTime - 500;

    if (td > 0)
    {
        if (td < 33000)
        {
            // Short sleep until next frame
            std::this_thread::sleep_for(std::chrono::microseconds(td));
        }
        else
        {
            // Too long to wait, skip frame grab this iteration
            getNextFrame = false;
        }
    }

    // Process frames on the processing tab
    if (activeTab == TAB_PROCESSING)
    {
        if (play)
        {
            if (getNextFrame)
            {
                bool gotFrame = ipEngine.GetNextFrame();
                if (!gotFrame)
                {
                    play = false;
                }
            }
        }
    }

    // Step the processing pipeline
    if (activeTab == TAB_PROCESSING)
    {
        ipEngine.Step(hudVisible);
    }
    else if (activeTab == TAB_CALIBRATION)
    {
        ipEngine.capture->Calibrate();
    }

    // Determine which frame to display
    if (activeTab == TAB_PROCESSING)
        oglScreen = ipEngine.capture->frame;
    else if (activeTab == TAB_BACKGROUND)
        oglScreen = ipEngine.background;
    else if (activeTab == TAB_CALIBRATION)
        oglScreen = ipEngine.capture->CalibrationGetFrame();
    else if (activeTab == TAB_PROCFRAME)
        oglScreen = ipEngine.zoneMap;

    // Update the video slider position
    if (!sliderMoving && ipEngine.capture->GetFrameCount() > 0)
    {
        videoSliderPos = (float)ipEngine.capture->GetFrameNumber() /
                         (float)(ipEngine.capture->GetFrameCount() + 1);
    }
}

// ============================================================================
// RenderFrame
// ============================================================================

void AppGui::RenderFrame()
{
    if (pendingScaleChange)
    {
        ApplyUIScale();
        pendingScaleChange = false;
    }

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    // Full-window dockspace-like layout
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);

    ImGuiWindowFlags hostFlags = ImGuiWindowFlags_NoTitleBar |
                                  ImGuiWindowFlags_NoCollapse |
                                  ImGuiWindowFlags_NoResize |
                                  ImGuiWindowFlags_NoMove |
                                  ImGuiWindowFlags_NoBringToFrontOnFocus |
                                  ImGuiWindowFlags_NoNavFocus |
                                  ImGuiWindowFlags_MenuBar;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(4*dpiScale, 4*dpiScale));

    ImGui::Begin("##MainWindow", nullptr, hostFlags);
    ImGui::PopStyleVar(3);

    DrawMenuBar();
    DrawToolbar();

    // Split: left = video, right = tabs
    float panelWidth = 380.0f * dpiScale;
    ImVec2 contentRegion = ImGui::GetContentRegionAvail();

    // Video display on the left
    ImGui::BeginChild("VideoArea", ImVec2(contentRegion.x - panelWidth, 0), false);
    DrawVideoDisplay();
    ImGui::EndChild();

    ImGui::SameLine();

    // Control panel on the right
    ImGui::BeginChild("ControlPanel", ImVec2(panelWidth, 0), true);
    DrawTabs();
    ImGui::EndChild();

    ImGui::End();

    // Draw plugin dialogs
    for (int i = 0; i < (int)pipelineDialogOpen.size(); i++)
    {
        if (pipelineDialogOpen[i])
            DrawPluginDialog(i);
    }

    // UI Scale buttons — bottom right corner
    {
        ImGuiViewport* vp = ImGui::GetMainViewport();
        float btnW = 28 * dpiScale;
        float btnH = 22 * dpiScale;
        float pad = 4 * dpiScale;
        char scaleBuf[16];
        snprintf(scaleBuf, sizeof(scaleBuf), "%.0f%%", dpiScale * 100.0f);
        float labelW = ImGui::CalcTextSize(scaleBuf).x + pad * 2;
        float totalW = btnW * 2 + labelW + pad * 2;
        ImGui::SetNextWindowPos(ImVec2(vp->WorkPos.x + vp->WorkSize.x - totalW - pad,
                                        vp->WorkPos.y + vp->WorkSize.y - btnH - pad));
        ImGui::SetNextWindowSize(ImVec2(totalW, btnH));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(0, 0));
        if (ImGui::Begin("##UIScale", nullptr,
            ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
            ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_AlwaysAutoResize))
        {
            if (ImGui::Button("-##scale", ImVec2(btnW, btnH)))
            {
                float oldScale = dpiScale;
                dpiScale = std::max(0.5f, dpiScale * 0.9f);
                std::cerr << "Scale -: " << oldScale << " -> " << dpiScale << std::endl;
                pendingScaleChange = true;
            }
            ImGui::SameLine(0, pad);
            ImGui::AlignTextToFramePadding();
            ImGui::Text("%s", scaleBuf);
            ImGui::SameLine(0, pad);
            if (ImGui::Button("+##scale", ImVec2(btnW, btnH)))
            {
                float oldScale = dpiScale;
                dpiScale = std::min(4.0f, dpiScale * 1.1f);
                std::cerr << "Scale +: " << oldScale << " -> " << dpiScale << std::endl;
                pendingScaleChange = true;
            }
        }
        ImGui::End();
        ImGui::PopStyleVar(2);
    }

    // Render
    ImGui::Render();
    int w, h;
    SDL_GetWindowSize(window, &w, &h);
    glViewport(0, 0, w, h);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

// ============================================================================
// DrawMenuBar
// ============================================================================

void AppGui::DrawMenuBar()
{
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Open Source..."))
                OpenSource();
            if (ImGui::MenuItem("Save Source..."))
                SaveSource();
            ImGui::Separator();
            if (ImGui::MenuItem("Load Settings..."))
                LoadSettings();
            if (ImGui::MenuItem("Save Settings..."))
                SaveSettings();
            ImGui::Separator();
            if (ImGui::MenuItem("Quit"))
                running = false;
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Help"))
        {
            if (ImGui::MenuItem("About"))
            {
                // Show about popup
                ImGui::OpenPopup("AboutPopup");
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    // About popup
    if (ImGui::BeginPopupModal("AboutPopup", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("USE Tracker");
        ImGui::Separator();
        ImGui::Text("Universal Simultaneous Event Tracker");
        ImGui::Text("Built with SDL2 + Dear ImGui + OpenCV");
        ImGui::Text("Copyright (C) 2015 Alexandre Campo");
        ImGui::Text("Licensed under GNU GPL v3");
        ImGui::Separator();
        if (ImGui::Button("OK", ImVec2(120*dpiScale, 0)))
            ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
    }
}

// ============================================================================
// DrawToolbar
// ============================================================================

void AppGui::DrawToolbar()
{
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4*dpiScale, 4*dpiScale));

    // Record button
    bool isOutputting = output;
    if (isOutputting)
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.2f, 0.2f, 1.0f));
    if (ImGui::Button(isOutputting ? "REC*" : "REC", ImVec2(40*dpiScale, 24*dpiScale)))
    {
        output = !output;
        if (output)
            ipEngine.OpenOutput();
        else
            ipEngine.CloseOutput();
    }
    if (isOutputting)
        ImGui::PopStyleColor();

    ImGui::SameLine();

    // Stop
    if (ImGui::Button("Stop", ImVec2(40*dpiScale, 24*dpiScale)))
    {
        ipEngine.capture->Stop();
        play = false;
        output = false;
        ipEngine.CloseOutput();
    }

    ImGui::SameLine();

    // Rewind
    if (ImGui::Button("|<", ImVec2(30*dpiScale, 24*dpiScale)))
    {
        ipEngine.capture->Stop();
        ipEngine.capture->GetNextFrame();
        play = false;
    }

    ImGui::SameLine();

    // Step backward
    if (ImGui::Button("<", ImVec2(24*dpiScale, 24*dpiScale)))
    {
        if (ipEngine.capture->GetFrameCount() > 0)
        {
            double t = ipEngine.capture->GetTime();
            double fps = ipEngine.capture->GetFPS();
            if (fps > 0)
            {
                ipEngine.capture->GetFrame(t - 1.0 / fps);
                ipEngine.Step(hudVisible);
            }
        }
    }

    ImGui::SameLine();

    // Play/Pause
    const char* playLabel = play ? "||" : ">";
    if (ImGui::Button(playLabel, ImVec2(30*dpiScale, 24*dpiScale)))
    {
        play = !play;
        if (play)
            ipEngine.capture->Play();
        else
            ipEngine.capture->Pause();
    }

    ImGui::SameLine();

    // Step forward
    if (ImGui::Button(">", ImVec2(24*dpiScale, 24*dpiScale)))
    {
        ipEngine.GetNextFrame();
        ipEngine.Step(hudVisible);
    }

    ImGui::SameLine();

    // Fast forward
    if (ImGui::Button(">|", ImVec2(30*dpiScale, 24*dpiScale)))
    {
        // Jump forward (skip 10 frames)
        for (int i = 0; i < 10; i++)
        {
            if (!ipEngine.GetNextFrame()) break;
        }
        ipEngine.Step(hudVisible);
    }

    ImGui::SameLine();
    ImGui::Spacing();
    ImGui::SameLine();

    // Video slider
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 250);
    float prevSliderPos = videoSliderPos;
    if (ImGui::SliderFloat("##VideoSlider", &videoSliderPos, 0.0f, 1.0f, ""))
    {
        sliderMoving = true;
        if (ipEngine.capture->GetFrameCount() > 0)
        {
            double totalTime = (double)ipEngine.capture->GetFrameCount() / ipEngine.capture->GetFPS();
            ipEngine.capture->GetFrame(videoSliderPos * totalTime);
            ipEngine.Step(hudVisible);
        }
    }
    if (ImGui::IsItemDeactivatedAfterEdit() || (!ImGui::IsItemActive() && sliderMoving))
    {
        sliderMoving = false;
    }

    ImGui::SameLine();

    // HUD toggle
    if (ImGui::Checkbox("HUD", &hudVisible))
    {
        // state toggled
    }

    ImGui::SameLine();

    // Processing blending slider
    ImGui::SetNextItemWidth(80);
    ImGui::SliderFloat("##Blend", &processingBlending, 0.0f, 1.0f, "%.1f");
    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Processing blending");

    ImGui::PopStyleVar();
}

// ============================================================================
// DrawVideoDisplay
// ============================================================================

void AppGui::DrawVideoDisplay()
{
    // Blend source frame with processing snapshot if needed
    cv::Mat displayFrame;

    if (!oglScreen.empty())
    {
        if (processingBlending > 0.001f && !ipEngine.pipelineSnapshot.empty())
        {
            cv::Mat snapshot3;
            if (ipEngine.pipelineSnapshot.channels() == 1)
                cv::cvtColor(ipEngine.pipelineSnapshot, snapshot3, cv::COLOR_GRAY2BGR);
            else
                snapshot3 = ipEngine.pipelineSnapshot;

            if (snapshot3.size() == oglScreen.size())
            {
                cv::addWeighted(oglScreen, 1.0 - processingBlending,
                                snapshot3, processingBlending, 0, displayFrame);
            }
            else
            {
                displayFrame = oglScreen;
            }
        }
        else
        {
            displayFrame = oglScreen;
        }

        // Overlay HUD if visible
        if (hudVisible && !ipEngine.hud.empty() && ipEngine.hud.size() == displayFrame.size())
        {
            // The HUD is BGRA with alpha channel
            for (int y = 0; y < ipEngine.hud.rows; y++)
            {
                for (int x = 0; x < ipEngine.hud.cols; x++)
                {
                    cv::Vec4b hudPixel = ipEngine.hud.at<cv::Vec4b>(y, x);
                    if (hudPixel[3] > 0)
                    {
                        float alpha = hudPixel[3] / 255.0f;
                        cv::Vec3b& dst = displayFrame.at<cv::Vec3b>(y, x);
                        dst[0] = (unsigned char)(dst[0] * (1 - alpha) + hudPixel[0] * alpha);
                        dst[1] = (unsigned char)(dst[1] * (1 - alpha) + hudPixel[1] * alpha);
                        dst[2] = (unsigned char)(dst[2] * (1 - alpha) + hudPixel[2] * alpha);
                    }
                }
            }
        }

        UploadVideoTexture(displayFrame);
    }

    if (videoTexture != 0 && texWidth > 0 && texHeight > 0)
    {
        ImVec2 avail = ImGui::GetContentRegionAvail();

        // Maintain aspect ratio
        float aspect = (float)texWidth / (float)texHeight;
        float displayW = avail.x;
        float displayH = displayW / aspect;
        if (displayH > avail.y)
        {
            displayH = avail.y;
            displayW = displayH * aspect;
        }

        // UV coordinates for zoom/pan
        ImVec2 uv0(zoomStartX, zoomStartY);
        ImVec2 uv1(zoomEndX, zoomEndY);

        ImGui::Image((ImTextureID)(intptr_t)videoTexture,
                     ImVec2(displayW, displayH),
                     uv0, uv1);

        // Zoom/pan with mouse
        if (ImGui::IsItemHovered())
        {
            float scroll = ImGui::GetIO().MouseWheel;
            if (scroll != 0.0f)
            {
                // Zoom in/out centered on mouse
                ImVec2 mousePos = ImGui::GetMousePos();
                ImVec2 itemMin = ImGui::GetItemRectMin();
                ImVec2 itemSize = ImGui::GetItemRectSize();

                float mx = (mousePos.x - itemMin.x) / itemSize.x; // 0..1 in display
                float my = (mousePos.y - itemMin.y) / itemSize.y;

                // Convert to UV space
                float uvX = zoomStartX + mx * (zoomEndX - zoomStartX);
                float uvY = zoomStartY + my * (zoomEndY - zoomStartY);

                float zoomFactor = (scroll > 0) ? 0.9f : 1.1f;

                float newWidth = (zoomEndX - zoomStartX) * zoomFactor;
                float newHeight = (zoomEndY - zoomStartY) * zoomFactor;

                // Clamp to valid range
                newWidth = std::max(0.01f, std::min(1.0f, newWidth));
                newHeight = std::max(0.01f, std::min(1.0f, newHeight));

                zoomStartX = uvX - mx * newWidth;
                zoomStartY = uvY - my * newHeight;
                zoomEndX = zoomStartX + newWidth;
                zoomEndY = zoomStartY + newHeight;

                // Clamp bounds
                if (zoomStartX < 0.0f) { zoomEndX -= zoomStartX; zoomStartX = 0.0f; }
                if (zoomStartY < 0.0f) { zoomEndY -= zoomStartY; zoomStartY = 0.0f; }
                if (zoomEndX > 1.0f) { zoomStartX -= (zoomEndX - 1.0f); zoomEndX = 1.0f; }
                if (zoomEndY > 1.0f) { zoomStartY -= (zoomEndY - 1.0f); zoomEndY = 1.0f; }

                zoomStartX = std::max(0.0f, zoomStartX);
                zoomStartY = std::max(0.0f, zoomStartY);
            }

            // Pan with middle mouse button
            if (ImGui::IsMouseDragging(ImGuiMouseButton_Middle))
            {
                ImVec2 delta = ImGui::GetIO().MouseDelta;
                ImVec2 itemSize = ImGui::GetItemRectSize();

                float dx = -delta.x / itemSize.x * (zoomEndX - zoomStartX);
                float dy = -delta.y / itemSize.y * (zoomEndY - zoomStartY);

                zoomStartX += dx;
                zoomEndX += dx;
                zoomStartY += dy;
                zoomEndY += dy;

                // Clamp
                if (zoomStartX < 0.0f) { zoomEndX -= zoomStartX; zoomStartX = 0.0f; }
                if (zoomStartY < 0.0f) { zoomEndY -= zoomStartY; zoomStartY = 0.0f; }
                if (zoomEndX > 1.0f) { zoomStartX -= (zoomEndX - 1.0f); zoomEndX = 1.0f; }
                if (zoomEndY > 1.0f) { zoomStartY -= (zoomEndY - 1.0f); zoomEndY = 1.0f; }

                zoomStartX = std::max(0.0f, zoomStartX);
                zoomStartY = std::max(0.0f, zoomStartY);
            }

            // Double-click to reset zoom
            if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
            {
                zoomStartX = 0.0f; zoomStartY = 0.0f;
                zoomEndX = 1.0f; zoomEndY = 1.0f;
            }
        }
    }
    else
    {
        ImGui::Text("No video source loaded.");
    }

    // Status line
    if (ipEngine.capture)
    {
        int totalSec = (int)ipEngine.capture->GetTime();
        int minutes = totalSec / 60;
        int seconds = totalSec % 60;
        long frameNum = ipEngine.capture->GetFrameNumber();
        long frameCount = ipEngine.capture->GetFrameCount();

        ImGui::Text("Time: %02d:%02d  Frame: %ld / %ld  FPS: %.1f",
                     minutes, seconds, frameNum, frameCount,
                     ipEngine.capture->GetFPS());
    }
}

// ============================================================================
// DrawTabs
// ============================================================================

void AppGui::DrawTabs()
{
    if (ImGui::BeginTabBar("##MainTabs"))
    {
        if (ImGui::BeginTabItem("Processing"))
        {
            activeTab = TAB_PROCESSING;
            DrawProcessingTab();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Background"))
        {
            activeTab = TAB_BACKGROUND;
            DrawBackgroundTab();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Calibration"))
        {
            activeTab = TAB_CALIBRATION;
            DrawCalibrationTab();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Processing Frame"))
        {
            activeTab = TAB_PROCFRAME;
            DrawProcessingFrameTab();
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
}

// ============================================================================
// DrawProcessingTab
// ============================================================================

void AppGui::DrawProcessingTab()
{
    ImGui::Text("Pipeline");
    ImGui::Separator();

    // Pipeline list
    int pipelineSize = 0;
    if (!ipEngine.pipelines.empty())
        pipelineSize = (int)ipEngine.pipelines[0].plugins.size();

    ImGui::BeginChild("PipelineList", ImVec2(0, 200*dpiScale), true);
    for (int i = 0; i < pipelineSize; i++)
    {
        // Get the plugin pointer (may be null in threaded pipeline, use single-threaded)
        PipelinePlugin* pp = ipEngine.pipelines[0].plugins[i];
        if (!pp) pp = ipEngine.pipelines[ipEngine.threadsCount].plugins[i];

        if (!pp) continue;

        bool isActive = pp->active;
        std::string label = std::to_string(i) + ": " +
                           CamelCaseToText(pp->registryName);

        // Checkbox for active state
        ImGui::PushID(i);
        if (ImGui::Checkbox("##active", &isActive))
        {
            // Set active on all pipeline copies
            for (unsigned int p = 0; p <= ipEngine.threadsCount; p++)
            {
                if (ipEngine.pipelines[p].plugins[i])
                    ipEngine.pipelines[p].plugins[i]->active = isActive;
            }
        }
        ImGui::SameLine();

        bool isSelected = (selectedPipelineItem == i);
        if (ImGui::Selectable(label.c_str(), isSelected))
        {
            selectedPipelineItem = i;
            ipEngine.takeSnapshot = true;
        }

        // Double-click to open dialog
        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
        {
            if (i < (int)pipelineDialogOpen.size())
                pipelineDialogOpen[i] = true;
        }
        ImGui::PopID();
    }
    ImGui::EndChild();

    // Pipeline control buttons
    if (ImGui::Button("Settings...") && selectedPipelineItem >= 0 &&
        selectedPipelineItem < (int)pipelineDialogOpen.size())
    {
        pipelineDialogOpen[selectedPipelineItem] = true;
    }
    ImGui::SameLine();

    if (ImGui::Button("Remove") && selectedPipelineItem >= 0 &&
        selectedPipelineItem < pipelineSize)
    {
        ipEngine.Erase(selectedPipelineItem);
        if (selectedPipelineItem < (int)pipelineDialogOpen.size())
            pipelineDialogOpen.erase(pipelineDialogOpen.begin() + selectedPipelineItem);
        if (selectedPipelineItem >= (int)ipEngine.pipelines[0].plugins.size())
            selectedPipelineItem = (int)ipEngine.pipelines[0].plugins.size() - 1;
    }
    ImGui::SameLine();

    if (ImGui::Button("Up") && selectedPipelineItem > 0 &&
        selectedPipelineItem < pipelineSize)
    {
        auto pfv = ipEngine.Erase(selectedPipelineItem);
        bool dialogState = false;
        if (selectedPipelineItem < (int)pipelineDialogOpen.size())
        {
            dialogState = pipelineDialogOpen[selectedPipelineItem];
            pipelineDialogOpen.erase(pipelineDialogOpen.begin() + selectedPipelineItem);
        }
        selectedPipelineItem--;
        ipEngine.Insert(selectedPipelineItem, pfv, false);
        pipelineDialogOpen.insert(pipelineDialogOpen.begin() + selectedPipelineItem, dialogState);
    }
    ImGui::SameLine();

    if (ImGui::Button("Down") && selectedPipelineItem >= 0 &&
        selectedPipelineItem < pipelineSize - 1)
    {
        auto pfv = ipEngine.Erase(selectedPipelineItem);
        bool dialogState = false;
        if (selectedPipelineItem < (int)pipelineDialogOpen.size())
        {
            dialogState = pipelineDialogOpen[selectedPipelineItem];
            pipelineDialogOpen.erase(pipelineDialogOpen.begin() + selectedPipelineItem);
        }
        selectedPipelineItem++;
        ipEngine.Insert(selectedPipelineItem, pfv, false);
        pipelineDialogOpen.insert(pipelineDialogOpen.begin() + selectedPipelineItem, dialogState);
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Text("Available Plugins");

    // Available plugins list
    ImGui::BeginChild("AvailablePlugins", ImVec2(0, 150*dpiScale), true);
    for (int i = 0; i < (int)availablePluginNames.size(); i++)
    {
        std::string displayName = CamelCaseToText(availablePluginNames[i]);
        bool isSelected = (selectedAvailablePlugin == i);
        if (ImGui::Selectable(displayName.c_str(), isSelected))
        {
            selectedAvailablePlugin = i;
        }
        // Double-click to add
        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
        {
            cv::FileNode fn;
            AddPipelinePlugin(availablePluginNames[i], fn);
        }
    }
    ImGui::EndChild();

    if (ImGui::Button("Add Plugin") && selectedAvailablePlugin >= 0 &&
        selectedAvailablePlugin < (int)availablePluginNames.size())
    {
        cv::FileNode fn;
        AddPipelinePlugin(availablePluginNames[selectedAvailablePlugin], fn);
    }
}

// ============================================================================
// DrawBackgroundTab
// ============================================================================

void AppGui::DrawBackgroundTab()
{
    ImGui::Text("Background Calculation");
    ImGui::Separator();

    // Method selection
    int bgMethod = (int)ipEngine.bgCalcType;
    ImGui::Text("Method:");
    ImGui::RadioButton("Mean", &bgMethod, (int)ImageProcessingEngine::BG_MEAN);
    ImGui::SameLine();
    ImGui::RadioButton("Median", &bgMethod, (int)ImageProcessingEngine::BG_MEDIAN);
    ipEngine.bgCalcType = (ImageProcessingEngine::BgCalcType)bgMethod;

    ImGui::Spacing();

    // Parameters
    ImGui::InputInt("Frames", &ipEngine.bgFrames);
    if (ipEngine.bgFrames < 1) ipEngine.bgFrames = 1;

    ImGui::InputFloat("Start Time", &ipEngine.bgStartTime, 0.1f, 1.0f, "%.1f");
    ImGui::InputFloat("End Time", &ipEngine.bgEndTime, 0.1f, 1.0f, "%.1f");

    int lowThresh = ipEngine.bgLowThreshold;
    int highThresh = ipEngine.bgHighThreshold;
    ImGui::InputInt("Low Threshold", &lowThresh);
    ImGui::InputInt("High Threshold", &highThresh);
    ipEngine.bgLowThreshold = (unsigned char)std::max(0, std::min(255, lowThresh));
    ipEngine.bgHighThreshold = (unsigned char)std::max(0, std::min(255, highThresh));

    ImGui::Checkbox("Recalculate", &ipEngine.bgRecalculate);

    ImGui::Spacing();
    ImGui::Separator();

    // Load/Save/Calculate background
    if (ImGui::Button("Load Background"))
    {
        auto f = pfd::open_file("Load Background Image", ".",
                                {"Image files", "*.png *.jpg *.bmp *.tif *.tiff",
                                 "All files", "*"});
        auto result = f.result();
        if (!result.empty())
        {
            ipEngine.bgFilename = result[0];
            ipEngine.background = cv::imread(ipEngine.bgFilename);
            if (!ipEngine.background.empty())
                ipEngine.Reset();
        }
    }
    ImGui::SameLine();

    if (ImGui::Button("Save Background"))
    {
        auto f = pfd::save_file("Save Background Image", "background.png",
                                {"PNG files", "*.png",
                                 "All files", "*"});
        auto result = f.result();
        if (!result.empty())
        {
            cv::imwrite(result, ipEngine.background);
        }
    }

    ImGui::Spacing();

    if (ImGui::Button("Calculate Background"))
    {
        if (ipEngine.capture)
        {
            if (ipEngine.bgCalcType == ImageProcessingEngine::BG_MEAN)
            {
                ipEngine.background = CalculateBackgroundMean(
                    ipEngine.capture,
                    ipEngine.bgStartTime, ipEngine.bgEndTime,
                    ipEngine.bgFrames,
                    ipEngine.bgLowThreshold, ipEngine.bgHighThreshold);
            }
            else
            {
                ipEngine.background = CalculateBackgroundMedian(
                    ipEngine.capture,
                    ipEngine.bgStartTime, ipEngine.bgEndTime,
                    ipEngine.bgFrames,
                    ipEngine.bgLowThreshold, ipEngine.bgHighThreshold);
            }
            ipEngine.Reset();
        }
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Text("Zones of Interest");

    // Zones file
    static char zonesPath[INPUT_BUF_SIZE] = "";
    if (zonesPath[0] == '\0' && !ipEngine.zonesFilename.empty())
        strncpy(zonesPath, ipEngine.zonesFilename.c_str(), INPUT_BUF_SIZE - 1);

    ImGui::InputText("Zones File", zonesPath, INPUT_BUF_SIZE);
    ImGui::SameLine();

    if (ImGui::Button("Browse##Zones"))
    {
        auto f = pfd::open_file("Load Zones Image", ".",
                                {"Image files", "*.png *.jpg *.bmp *.tif",
                                 "All files", "*"});
        auto result = f.result();
        if (!result.empty())
        {
            strncpy(zonesPath, result[0].c_str(), INPUT_BUF_SIZE - 1);
            ipEngine.zonesFilename = result[0];
            cv::Mat zones = cv::imread(ipEngine.zonesFilename, cv::IMREAD_GRAYSCALE);
            if (!zones.empty())
            {
                ipEngine.zoneMap = zones;
                ipEngine.Reset();
            }
        }
    }

    ipEngine.zonesFilename = zonesPath;
}

// ============================================================================
// DrawCalibrationTab
// ============================================================================

void AppGui::DrawCalibrationTab()
{
    if (!ipEngine.capture)
    {
        ImGui::Text("No capture device loaded.");
        return;
    }

    ImGui::Text("Camera Calibration");
    ImGui::Separator();

    // Board type
    int boardType = ipEngine.capture->CalibrationGetBoardType();
    const char* boardTypes[] = { "Chessboard", "Circles Grid", "Asymmetric Circles Grid" };
    if (boardType >= 1 && boardType <= 3)
    {
        int sel = boardType - 1;
        if (ImGui::Combo("Board Type", &sel, boardTypes, 3))
        {
            ipEngine.capture->CalibrationSetBoardType(sel + 1);
        }
    }

    // Board dimensions
    int boardWidth = ipEngine.capture->CalibrationGetBoardWidth();
    int boardHeight = ipEngine.capture->CalibrationGetBoardHeight();
    int squareSize = ipEngine.capture->CalibrationGetSquareSize();
    int framesCount = ipEngine.capture->CalibrationGetFramesCount();

    if (ImGui::InputInt("Board Width", &boardWidth))
        ipEngine.capture->CalibrationSetBoardWidth(boardWidth);
    if (ImGui::InputInt("Board Height", &boardHeight))
        ipEngine.capture->CalibrationSetBoardHeight(boardHeight);
    if (ImGui::InputInt("Square Size", &squareSize))
        ipEngine.capture->CalibrationSetSquareSize(squareSize);
    if (ImGui::InputInt("Frames Count", &framesCount))
        ipEngine.capture->CalibrationSetFramesCount(framesCount);

    // Aspect ratio
    int aspectNum = ipEngine.capture->CalibrationGetAspectNum();
    int aspectDen = ipEngine.capture->CalibrationGetAspectDen();
    if (ImGui::InputInt("Aspect Numerator", &aspectNum))
    {
        if (aspectDen > 0)
            ipEngine.capture->CalibrationSetAspect((float)aspectNum / (float)aspectDen);
    }
    if (ImGui::InputInt("Aspect Denominator", &aspectDen))
    {
        if (aspectDen > 0)
            ipEngine.capture->CalibrationSetAspect((float)aspectNum / (float)aspectDen);
    }

    // Frame delay
    float frameDelay = ipEngine.capture->CalibrationGetFrameDelay();
    if (ImGui::InputFloat("Frame Delay", &frameDelay, 0.1f, 1.0f, "%.1f"))
        ipEngine.capture->CalibrationSetFrameDelay(frameDelay);

    // Flags
    bool zeroTangent = ipEngine.capture->CalibrationGetZeroTangentDist();
    bool fixPrincipal = ipEngine.capture->CalibrationGetFixPrincipalPoint();
    bool flipVert = ipEngine.capture->CalibrationGetFlipVertical();

    if (ImGui::Checkbox("Zero Tangent Distortion", &zeroTangent))
        ipEngine.capture->CalibrationSetZeroTangentDist(zeroTangent);
    if (ImGui::Checkbox("Fix Principal Point", &fixPrincipal))
        ipEngine.capture->CalibrationSetFixPrincipalPoint(fixPrincipal);
    if (ImGui::Checkbox("Flip Vertical", &flipVert))
        ipEngine.capture->CalibrationSetFlipVertical(flipVert);

    ImGui::Spacing();
    ImGui::Separator();

    // Calibration controls
    if (ImGui::Button("Start Calibration"))
    {
        ipEngine.capture->CalibrationStart();
    }
    ImGui::SameLine();
    if (ImGui::Button("Reset Calibration"))
    {
        ipEngine.capture->CalibrationReset();
    }

    // Multi-source subdevice selection
    bool isMulti = (ipEngine.capture->type == Capture::MULTI_USB_CAMERA ||
                    ipEngine.capture->type == Capture::MULTI_VIDEO);
    if (isMulti)
    {
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Text("Subdevice Calibration");

        if (ipEngine.capture->type == Capture::MULTI_USB_CAMERA)
        {
            CaptureMultiUSBCamera* c = dynamic_cast<CaptureMultiUSBCamera*>(ipEngine.capture);
            if (c)
            {
                unsigned int n = c->GetDeviceCount();
                int selected = c->GetDeviceToCalibrate();
                for (unsigned int i = 0; i < n; i++)
                {
                    ImGui::RadioButton(c->GetDeviceName(i).c_str(), &selected, i);
                }
                if (selected != (int)c->GetDeviceToCalibrate())
                    c->SetDeviceToCalibrate(selected);
            }
        }
        else if (ipEngine.capture->type == Capture::MULTI_VIDEO)
        {
            CaptureMultiVideo* c = dynamic_cast<CaptureMultiVideo*>(ipEngine.capture);
            if (c)
            {
                unsigned int n = c->GetDeviceCount();
                int selected = c->GetDeviceToCalibrate();
                for (unsigned int i = 0; i < n; i++)
                {
                    std::string name = c->GetDeviceName(i);
                    if (name.length() > 23)
                        name = name.substr(0, 20) + "...";
                    ImGui::RadioButton(name.c_str(), &selected, i);
                }
                if (selected != (int)c->GetDeviceToCalibrate())
                    c->SetDeviceToCalibrate(selected);
            }
        }
    }
}

// ============================================================================
// DrawProcessingFrameTab
// ============================================================================

void AppGui::DrawProcessingFrameTab()
{
    ImGui::Text("Processing Frame Settings");
    ImGui::Separator();

    ImGui::InputFloat("Start Time", &ipEngine.startTime, 0.1f, 1.0f, "%.2f");
    ImGui::InputFloat("Duration", &ipEngine.durationTime, 0.1f, 1.0f, "%.2f");
    ImGui::InputFloat("Timestep", &ipEngine.timestep, 0.001f, 0.01f, "%.3f");
    ImGui::Checkbox("Use Time Boundaries", &ipEngine.useTimeBoundaries);
}

// ============================================================================
// DrawPluginDialog
// ============================================================================

void AppGui::DrawPluginDialog(int index)
{
    if (index < 0 || ipEngine.pipelines.empty()) return;
    if (index >= (int)ipEngine.pipelines[0].plugins.size()) return;

    // Get the plugin pointer from the first available pipeline
    PipelinePlugin* pp = ipEngine.pipelines[0].plugins[index];
    if (!pp) pp = ipEngine.pipelines[ipEngine.threadsCount].plugins[index];
    if (!pp) return;

    std::string title = CamelCaseToText(pp->registryName) +
                        " [" + std::to_string(index) + "]###PluginDlg" + std::to_string(index);
    bool open = pipelineDialogOpen[index];

    ImGui::SetNextWindowSize(ImVec2(400*dpiScale, 350*dpiScale), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin(title.c_str(), &open))
    {
        pipelineDialogOpen[index] = open;
        ImGui::End();
        return;
    }
    pipelineDialogOpen[index] = open;

    // Common controls
    bool isActive = pp->active;
    if (ImGui::Checkbox("Active", &isActive))
    {
        for (unsigned int p = 0; p <= ipEngine.threadsCount; p++)
            if (ipEngine.pipelines[p].plugins[index])
                ipEngine.pipelines[p].plugins[index]->active = isActive;
    }

    bool isOutput = pp->output;
    if (ImGui::Checkbox("Output", &isOutput))
    {
        for (unsigned int p = 0; p <= ipEngine.threadsCount; p++)
            if (ipEngine.pipelines[p].plugins[index])
                ipEngine.pipelines[p].plugins[index]->output = isOutput;
    }

    ImGui::Separator();

    // Type-specific controls using dynamic_cast

    // --- AdaptiveThreshold ---
    if (AdaptiveThreshold* p = dynamic_cast<AdaptiveThreshold*>(pp))
    {
        int blockSize = p->blockSize;
        if (ImGui::InputInt("Block Size", &blockSize))
            p->SetBlockSize(blockSize);

        int constant = p->constant;
        if (ImGui::InputInt("Constant (C)", &constant))
            p->SetConstant(constant);

        int method = p->thresholdMethod;
        const char* methods[] = { "Mean", "Gaussian" };
        int sel = (method == cv::ADAPTIVE_THRESH_GAUSSIAN_C) ? 1 : 0;
        if (ImGui::Combo("Threshold Method", &sel, methods, 2))
        {
            p->SetThresholdMethod(sel == 1 ? cv::ADAPTIVE_THRESH_GAUSSIAN_C
                                            : cv::ADAPTIVE_THRESH_MEAN_C);
        }

        ImGui::Checkbox("Additive", &p->additive);
        ImGui::Checkbox("Invert", &p->invert);
        ImGui::Checkbox("Restrict to Zone", &p->restrictToZone);
        if (p->restrictToZone)
            ImGui::InputInt("Zone", &p->zone);

        // Sync to all thread instances
        for (unsigned int t = 1; t < ipEngine.threadsCount; t++)
            if (auto* tp = dynamic_cast<AdaptiveThreshold*>(ipEngine.pipelines[t].plugins[index]))
            {
                tp->SetBlockSize(p->blockSize);
                tp->SetConstant(p->constant);
                tp->SetThresholdMethod(p->thresholdMethod);
                tp->additive = p->additive;
                tp->invert = p->invert;
                tp->restrictToZone = p->restrictToZone;
                tp->zone = p->zone;
            }
    }

    // --- ExtractMotion (background difference) ---
    else if (ExtractMotion* p = dynamic_cast<ExtractMotion*>(pp))
    {
        ImGui::InputInt("Threshold", &p->threshold);
        ImGui::Checkbox("Additive", &p->additive);
        ImGui::Checkbox("Restrict to Zone", &p->restrictToZone);
        if (p->restrictToZone)
            ImGui::InputInt("Zone", &p->zone);

        // Sync to all thread instances
        for (unsigned int t = 1; t < ipEngine.threadsCount; t++)
            if (auto* tp = dynamic_cast<ExtractMotion*>(ipEngine.pipelines[t].plugins[index]))
            {
                tp->threshold = p->threshold;
                tp->additive = p->additive;
                tp->restrictToZone = p->restrictToZone;
                tp->zone = p->zone;
            }
    }

    // --- BackgroundDiffMOG ---
    else if (BackgroundDiffMOG* p = dynamic_cast<BackgroundDiffMOG*>(pp))
    {
        int history = p->history;
        if (ImGui::InputInt("History", &history))
            p->SetHistory(history);

        int nMixtures = p->nMixtures;
        if (ImGui::InputInt("Num Mixtures", &nMixtures))
            p->SetNMixtures(nMixtures);

        double bgRatio = p->backgroundRatio;
        float bgRatioF = (float)bgRatio;
        if (ImGui::InputFloat("Background Ratio", &bgRatioF, 0.01f, 0.1f, "%.3f"))
            p->SetBackgroundRatio(bgRatioF);

        double noiseSigma = p->noiseSigma;
        float noiseSigmaF = (float)noiseSigma;
        if (ImGui::InputFloat("Noise Sigma", &noiseSigmaF, 0.1f, 1.0f, "%.2f"))
            p->SetNoiseSigma(noiseSigmaF);

        float lr = (float)p->learningRate;
        if (ImGui::InputFloat("Learning Rate", &lr, 0.001f, 0.01f, "%.4f"))
            p->learningRate = lr;

        ImGui::Checkbox("Additive", &p->additive);
        ImGui::Checkbox("Restrict to Zone", &p->restrictToZone);
        if (p->restrictToZone)
            ImGui::InputInt("Zone", &p->zone);

        // Sync to all thread instances
        for (unsigned int t = 1; t < ipEngine.threadsCount; t++)
            if (auto* tp = dynamic_cast<BackgroundDiffMOG*>(ipEngine.pipelines[t].plugins[index]))
            {
                tp->SetHistory(p->history);
                tp->SetNMixtures(p->nMixtures);
                tp->SetBackgroundRatio(p->backgroundRatio);
                tp->SetNoiseSigma(p->noiseSigma);
                tp->learningRate = p->learningRate;
                tp->additive = p->additive;
                tp->restrictToZone = p->restrictToZone;
                tp->zone = p->zone;
            }
    }

    // --- BackgroundDiffMOG2 ---
    else if (BackgroundDiffMOG2* p = dynamic_cast<BackgroundDiffMOG2*>(pp))
    {
        int history = p->history;
        if (ImGui::InputInt("History", &history))
            p->SetHistory(history);

        float thresh = (float)p->threshold;
        if (ImGui::InputFloat("Threshold", &thresh, 0.1f, 1.0f, "%.2f"))
            p->SetThreshold(thresh);

        bool shadow = p->shadowDetection;
        if (ImGui::Checkbox("Shadow Detection", &shadow))
            p->SetShadowDetection(shadow);

        float lr = (float)p->learningRate;
        if (ImGui::InputFloat("Learning Rate", &lr, 0.001f, 0.01f, "%.4f"))
            p->learningRate = lr;

        ImGui::Checkbox("Additive", &p->additive);
        ImGui::Checkbox("Restrict to Zone", &p->restrictToZone);
        if (p->restrictToZone)
            ImGui::InputInt("Zone", &p->zone);

        // Sync to all thread instances
        for (unsigned int t = 1; t < ipEngine.threadsCount; t++)
            if (auto* tp = dynamic_cast<BackgroundDiffMOG2*>(ipEngine.pipelines[t].plugins[index]))
            {
                tp->SetHistory(p->history);
                tp->SetThreshold(p->threshold);
                tp->SetShadowDetection(p->shadowDetection);
                tp->learningRate = p->learningRate;
                tp->additive = p->additive;
                tp->restrictToZone = p->restrictToZone;
                tp->zone = p->zone;
            }
    }

    // --- BackgroundDiffGMG ---
    else if (BackgroundDiffGMG* p = dynamic_cast<BackgroundDiffGMG*>(pp))
    {
        float lr = (float)p->learningRate;
        if (ImGui::InputFloat("Learning Rate", &lr, 0.001f, 0.01f, "%.4f"))
            p->learningRate = lr;

        ImGui::Checkbox("Additive", &p->additive);
        ImGui::Checkbox("Restrict to Zone", &p->restrictToZone);
        if (p->restrictToZone)
            ImGui::InputInt("Zone", &p->zone);

        // Sync to all thread instances
        for (unsigned int t = 1; t < ipEngine.threadsCount; t++)
            if (auto* tp = dynamic_cast<BackgroundDiffGMG*>(ipEngine.pipelines[t].plugins[index]))
            {
                tp->learningRate = p->learningRate;
                tp->additive = p->additive;
                tp->restrictToZone = p->restrictToZone;
                tp->zone = p->zone;
            }
    }

    // --- ColorSegmentation ---
    else if (ColorSegmentation* p = dynamic_cast<ColorSegmentation*>(pp))
    {
        int type = (int)p->type;
        const char* typeNames[] = { "BGR", "HSV" };
        if (ImGui::Combo("Color Space", &type, typeNames, 2))
            p->type = (ColorSegmentation::Type)type;

        if (p->type == ColorSegmentation::HSV)
        {
            int minH = p->minHSV[0], minS = p->minHSV[1], minV = p->minHSV[2];
            int maxH = p->maxHSV[0], maxS = p->maxHSV[1], maxV = p->maxHSV[2];

            ImGui::SliderInt("Min Hue", &minH, 0, 180);
            ImGui::SliderInt("Max Hue", &maxH, 0, 180);
            ImGui::SliderInt("Min Saturation", &minS, 0, 255);
            ImGui::SliderInt("Max Saturation", &maxS, 0, 255);
            ImGui::SliderInt("Min Value", &minV, 0, 255);
            ImGui::SliderInt("Max Value", &maxV, 0, 255);

            p->minHSV = cv::Vec3b(minH, minS, minV);
            p->maxHSV = cv::Vec3b(maxH, maxS, maxV);
        }
        else
        {
            int minB = p->minBGR[0], minG = p->minBGR[1], minR = p->minBGR[2];
            int maxB = p->maxBGR[0], maxG = p->maxBGR[1], maxR = p->maxBGR[2];

            ImGui::SliderInt("Min Blue", &minB, 0, 255);
            ImGui::SliderInt("Max Blue", &maxB, 0, 255);
            ImGui::SliderInt("Min Green", &minG, 0, 255);
            ImGui::SliderInt("Max Green", &maxG, 0, 255);
            ImGui::SliderInt("Min Red", &minR, 0, 255);
            ImGui::SliderInt("Max Red", &maxR, 0, 255);

            p->minBGR = cv::Vec3b(minB, minG, minR);
            p->maxBGR = cv::Vec3b(maxB, maxG, maxR);
        }

        ImGui::Checkbox("Additive", &p->additive);
        ImGui::Checkbox("Restrict to Zone", &p->restrictToZone);
        if (p->restrictToZone)
            ImGui::InputInt("Zone", &p->zone);

        // Sync to all thread instances
        for (unsigned int t = 1; t < ipEngine.threadsCount; t++)
            if (auto* tp = dynamic_cast<ColorSegmentation*>(ipEngine.pipelines[t].plugins[index]))
            {
                tp->type = p->type;
                tp->minHSV = p->minHSV;
                tp->maxHSV = p->maxHSV;
                tp->minBGR = p->minBGR;
                tp->maxBGR = p->maxBGR;
                tp->additive = p->additive;
                tp->restrictToZone = p->restrictToZone;
                tp->zone = p->zone;
            }
    }

    // --- Dilation ---
    else if (Dilation* p = dynamic_cast<Dilation*>(pp))
    {
        int size = p->size;
        if (ImGui::InputInt("Size", &size))
            p->SetSize(size);
    }

    // --- Erosion ---
    else if (Erosion* p = dynamic_cast<Erosion*>(pp))
    {
        int size = p->size;
        if (ImGui::InputInt("Size", &size))
            p->SetSize(size);
    }

    // --- SafeErosion ---
    else if (SafeErosion* p = dynamic_cast<SafeErosion*>(pp))
    {
        ImGui::InputInt("Size", &p->size);
    }

    // --- ExtractBlobs ---
    else if (ExtractBlobs* p = dynamic_cast<ExtractBlobs*>(pp))
    {
        int minSize = (int)p->minSize;
        int maxSize = (int)p->maxSize;
        ImGui::InputInt("Min Size", &minSize);
        ImGui::InputInt("Max Size", &maxSize);
        p->minSize = (unsigned int)std::max(0, minSize);
        p->maxSize = (unsigned int)std::max(0, maxSize);

        static char ebOutput[INPUT_BUF_SIZE] = "";
        if (ebOutput[0] == '\0' && !p->outputFilename.empty())
            strncpy(ebOutput, p->outputFilename.c_str(), INPUT_BUF_SIZE - 1);
        if (ImGui::InputText("Output File", ebOutput, INPUT_BUF_SIZE))
            p->outputFilename = ebOutput;
        ImGui::SameLine();
        if (ImGui::Button("Browse##EB"))
        {
            auto f = pfd::save_file("Output File", p->outputFilename,
                                    {"CSV files", "*.csv", "All files", "*"});
            auto result = f.result();
            if (!result.empty())
            {
                strncpy(ebOutput, result.c_str(), INPUT_BUF_SIZE - 1);
                p->outputFilename = result;
            }
        }
    }

    // --- FrameDifference ---
    else if (FrameDifference* p = dynamic_cast<FrameDifference*>(pp))
    {
        int threshold = p->threshold;
        if (ImGui::InputInt("Threshold", &threshold))
            p->SetThreshold(threshold);

        bool usePipeline = p->usePipeline;
        if (ImGui::Checkbox("Use Pipeline", &usePipeline))
            p->SetUsePipeline(usePipeline);

        ImGui::Checkbox("Additive", &p->additive);
        ImGui::Checkbox("Invert", &p->invert);
        ImGui::Checkbox("Restrict to Zone", &p->restrictToZone);
        if (p->restrictToZone)
            ImGui::InputInt("Zone", &p->zone);

        // Sync to all thread instances
        for (unsigned int t = 1; t < ipEngine.threadsCount; t++)
            if (auto* tp = dynamic_cast<FrameDifference*>(ipEngine.pipelines[t].plugins[index]))
            {
                tp->SetThreshold(p->threshold);
                tp->SetUsePipeline(p->usePipeline);
                tp->additive = p->additive;
                tp->invert = p->invert;
                tp->restrictToZone = p->restrictToZone;
                tp->zone = p->zone;
            }
    }

    // --- MovingAverage ---
    else if (MovingAverage* p = dynamic_cast<MovingAverage*>(pp))
    {
        int length = (int)p->length;
        if (ImGui::InputInt("Length", &length))
            p->SetLength((unsigned int)std::max(1, length));

        int threshold = (int)p->threshold;
        if (ImGui::InputInt("Threshold", &threshold))
            p->SetThreshold((unsigned int)std::max(0, threshold));

        if (ImGui::Button("Clear History"))
        {
            p->ClearHistory();
            for (unsigned int t = 1; t < ipEngine.threadsCount; t++)
                if (auto* tp = dynamic_cast<MovingAverage*>(ipEngine.pipelines[t].plugins[index]))
                    tp->ClearHistory();
        }

        // Sync to all thread instances
        for (unsigned int t = 1; t < ipEngine.threadsCount; t++)
            if (auto* tp = dynamic_cast<MovingAverage*>(ipEngine.pipelines[t].plugins[index]))
            {
                tp->SetLength(p->length);
                tp->SetThreshold(p->threshold);
            }
    }

    // --- Tracker ---
    else if (Tracker* p = dynamic_cast<Tracker*>(pp))
    {
        int maxEntities = (int)p->entitiesCount;
        if (ImGui::InputInt("Max Entities", &maxEntities))
            p->SetMaxEntities((unsigned int)std::max(1, maxEntities));

        ImGui::InputFloat("Min Interdistance", &p->minInterdistance, 1.0f, 10.0f, "%.1f");
        ImGui::InputFloat("Max Motion/sec", &p->maxMotionPerSecond, 10.0f, 100.0f, "%.1f");
        ImGui::InputFloat("Extrapolation Decay", &p->extrapolationDecay, 0.01f, 0.1f, "%.2f");

        int meLen = (int)p->motionEstimatorLength;
        if (ImGui::InputInt("Motion Estimator Length", &meLen))
            p->motionEstimatorLength = (unsigned int)std::max(1, meLen);

        ImGui::InputFloat("Motion Estimator Timeout", &p->motionEstimatorTimeout, 0.1f, 1.0f, "%.2f");

        ImGui::Checkbox("Use Virtual Entities", &p->useVirtualEntities);
        if (p->useVirtualEntities)
        {
            ImGui::InputFloat("Virtual Lifetime", &p->virtualEntitiesLifetime, 0.1f, 1.0f, "%.2f");
            ImGui::InputFloat("Virtual Delay", &p->virtualEntitiesDelay, 0.1f, 1.0f, "%.2f");
            ImGui::Checkbox("Virtual Zone", &p->virtualEntitiesZone);
        }

        int trailLen = (int)p->trailLength;
        if (ImGui::InputInt("Trail Length", &trailLen))
            p->trailLength = (unsigned int)std::max(0, trailLen);

        ImGui::Checkbox("Replay", &p->replay);

        static char trackerOutput[INPUT_BUF_SIZE] = "";
        if (trackerOutput[0] == '\0' && !p->outputFilename.empty())
            strncpy(trackerOutput, p->outputFilename.c_str(), INPUT_BUF_SIZE - 1);
        if (ImGui::InputText("Output File", trackerOutput, INPUT_BUF_SIZE))
            p->outputFilename = trackerOutput;
        ImGui::SameLine();
        if (ImGui::Button("Browse##Tracker"))
        {
            auto f = pfd::save_file("Output File", p->outputFilename,
                                    {"CSV files", "*.csv", "All files", "*"});
            auto result = f.result();
            if (!result.empty())
            {
                strncpy(trackerOutput, result.c_str(), INPUT_BUF_SIZE - 1);
                p->outputFilename = result;
            }
        }

        if (ImGui::Button("Clear History"))
            p->ClearHistory();
    }

    // --- RecordVideo ---
    else if (RecordVideo* p = dynamic_cast<RecordVideo*>(pp))
    {
        static char rvOutput[INPUT_BUF_SIZE] = "";
        if (rvOutput[0] == '\0' && !p->outputFilename.empty())
            strncpy(rvOutput, p->outputFilename.c_str(), INPUT_BUF_SIZE - 1);
        if (ImGui::InputText("Output File", rvOutput, INPUT_BUF_SIZE))
            p->outputFilename = rvOutput;
        ImGui::SameLine();
        if (ImGui::Button("Browse##RV"))
        {
            auto f = pfd::save_file("Output File", p->outputFilename,
                                    {"Video files", "*.mp4 *.avi *.mkv", "All files", "*"});
            auto result = f.result();
            if (!result.empty())
            {
                strncpy(rvOutput, result.c_str(), INPUT_BUF_SIZE - 1);
                p->outputFilename = result;
            }
        }

        static char rvPreset[INPUT_BUF_SIZE] = "";
        if (rvPreset[0] == '\0' && !p->preset.empty())
            strncpy(rvPreset, p->preset.c_str(), INPUT_BUF_SIZE - 1);
        if (ImGui::InputText("Preset", rvPreset, INPUT_BUF_SIZE))
            p->preset = rvPreset;

        ImGui::InputInt("Bitrate (kbps)", &p->bitrate);
    }

    // --- RecordPixels ---
    else if (RecordPixels* p = dynamic_cast<RecordPixels*>(pp))
    {
        static char rpOutput[INPUT_BUF_SIZE] = "";
        if (rpOutput[0] == '\0' && !p->outputFilename.empty())
            strncpy(rpOutput, p->outputFilename.c_str(), INPUT_BUF_SIZE - 1);
        if (ImGui::InputText("Output File", rpOutput, INPUT_BUF_SIZE))
            p->outputFilename = rpOutput;
        ImGui::SameLine();
        if (ImGui::Button("Browse##RP"))
        {
            auto f = pfd::save_file("Output File", p->outputFilename,
                                    {"CSV files", "*.csv", "All files", "*"});
            auto result = f.result();
            if (!result.empty())
            {
                strncpy(rpOutput, result.c_str(), INPUT_BUF_SIZE - 1);
                p->outputFilename = result;
            }
        }
    }

    // --- SimpleTags ---
    else if (SimpleTags* p = dynamic_cast<SimpleTags*>(pp))
    {
        int pwidth = p->pwidth;
        int pheight = p->pheight;
        if (ImGui::InputInt("Tag Width", &pwidth))
            p->SetTagDimensions(pwidth, pheight);
        if (ImGui::InputInt("Tag Height", &pheight))
            p->SetTagDimensions(pwidth, pheight);

        static char stOutput[INPUT_BUF_SIZE] = "";
        if (stOutput[0] == '\0' && !p->outputFilename.empty())
            strncpy(stOutput, p->outputFilename.c_str(), INPUT_BUF_SIZE - 1);
        if (ImGui::InputText("Output File", stOutput, INPUT_BUF_SIZE))
            p->outputFilename = stOutput;
        ImGui::SameLine();
        if (ImGui::Button("Browse##ST"))
        {
            auto f = pfd::save_file("Output File", p->outputFilename,
                                    {"CSV files", "*.csv", "All files", "*"});
            auto result = f.result();
            if (!result.empty())
            {
                strncpy(stOutput, result.c_str(), INPUT_BUF_SIZE - 1);
                p->outputFilename = result;
            }
        }
    }

    // --- TakeSnapshots ---
    else if (TakeSnapshots* p = dynamic_cast<TakeSnapshots*>(pp))
    {
        static char tsOutput[INPUT_BUF_SIZE] = "";
        if (tsOutput[0] == '\0' && !p->outputFilename.empty())
            strncpy(tsOutput, p->outputFilename.c_str(), INPUT_BUF_SIZE - 1);
        if (ImGui::InputText("Output Dir/Pattern", tsOutput, INPUT_BUF_SIZE))
            p->outputFilename = tsOutput;
        ImGui::SameLine();
        if (ImGui::Button("Browse##TS"))
        {
            auto f = pfd::select_folder("Select Output Directory", ".");
            auto result = f.result();
            if (!result.empty())
            {
                strncpy(tsOutput, result.c_str(), INPUT_BUF_SIZE - 1);
                p->outputFilename = result;
            }
        }
    }

    // --- Stopwatch ---
    else if (Stopwatch* p = dynamic_cast<Stopwatch*>(pp))
    {
        ImGui::Text("Stopwatch plugin");
        ImGui::Text("Shortcuts: %d", (int)p->shortcuts.size());
        ImGui::Text("Events: %d", (int)p->events.size());

        static char swOutput[INPUT_BUF_SIZE] = "";
        if (swOutput[0] == '\0' && !p->outputFilename.empty())
            strncpy(swOutput, p->outputFilename.c_str(), INPUT_BUF_SIZE - 1);
        if (ImGui::InputText("Output File", swOutput, INPUT_BUF_SIZE))
            p->outputFilename = swOutput;
    }

    // --- GetBlobsAngles ---
    else if (GetBlobsAngles* p = dynamic_cast<GetBlobsAngles*>(pp))
    {
        ImGui::Text("Get Blobs Angles plugin");
        ImGui::Text("No additional parameters.");
    }

    // --- ZonesOfInterest ---
    else if (ZonesOfInterest* p = dynamic_cast<ZonesOfInterest*>(pp))
    {
        ImGui::Text("Zones of Interest plugin");
        ImGui::Text("Zones are loaded via the Background tab.");
    }

    // --- RemoteControl ---
    else if (RemoteControl* p = dynamic_cast<RemoteControl*>(pp))
    {
        ImGui::Text("Remote Control (Bluetooth)");
        ImGui::Text("Address: %s", p->btAddress.c_str());
        ImGui::InputInt("Corner", &p->corner);
    }

#ifdef ARUCO
    // --- Aruco ---
    else if (Aruco* p = dynamic_cast<Aruco*>(pp))
    {
        float minSz = (float)p->minSize;
        float maxSz = (float)p->maxSize;
        if (ImGui::InputFloat("Min Size", &minSz, 0.001f, 0.01f, "%.4f"))
            p->SetMinSize(minSz);
        if (ImGui::InputFloat("Max Size", &maxSz, 0.001f, 0.01f, "%.4f"))
            p->SetMaxSize(maxSz);

        int t1 = p->thresh1;
        int t2 = p->thresh2;
        if (ImGui::InputInt("Threshold 1", &t1))
            p->SetThreshold1(t1);
        if (ImGui::InputInt("Threshold 2", &t2))
            p->SetThreshold2(t2);

        int maskShape = p->maskShape;
        const char* maskShapes[] = { "None", "Square", "Disc" };
        if (ImGui::Combo("Mask Shape", &maskShape, maskShapes, 3))
            p->SetMaskShape(maskShape);

        int maskRadius = p->maskRadius;
        if (ImGui::InputInt("Mask Radius", &maskRadius))
            p->SetMaskRadius(maskRadius);

        int maskPerspShift = p->maskPerspectiveShift;
        if (ImGui::InputInt("Mask Persp. Shift", &maskPerspShift))
            p->SetMaskPerspectiveShift(maskPerspShift);

        int maskVal = p->maskValue;
        if (ImGui::InputInt("Mask Value", &maskVal))
            p->SetMaskValue(maskVal);

        static char arucoOutput[INPUT_BUF_SIZE] = "";
        if (arucoOutput[0] == '\0' && !p->outputFilename.empty())
            strncpy(arucoOutput, p->outputFilename.c_str(), INPUT_BUF_SIZE - 1);
        if (ImGui::InputText("Output File", arucoOutput, INPUT_BUF_SIZE))
            p->outputFilename = arucoOutput;
        ImGui::SameLine();
        if (ImGui::Button("Browse##Aruco"))
        {
            auto f = pfd::save_file("Output File", p->outputFilename,
                                    {"CSV files", "*.csv", "All files", "*"});
            auto result = f.result();
            if (!result.empty())
            {
                strncpy(arucoOutput, result.c_str(), INPUT_BUF_SIZE - 1);
                p->outputFilename = result;
            }
        }
    }
#endif

    // --- ArucoColor ---
    else if (ArucoColor* p = dynamic_cast<ArucoColor*>(pp))
    {
        int markerCols = p->GetMarkerCols();
        int markerRows = p->GetMarkerRows();
        int satThresh = p->GetSaturationThreshold();
        int valThresh = p->GetValueThreshold();
        int atBlockSize = p->GetAdaptiveThresholdBlockSize();
        int atConstant = p->GetAdaptiveThresholdConstant();
        int minArea = p->GetMinMarkerArea();
        int maxArea = p->GetMaxMarkerArea();
        int maxHueDev = p->GetMaxHueDeviation();
        int maxRange = p->GetMaxMarkerRange();

        if (ImGui::InputInt("Marker Cols", &markerCols))
            p->SetMarkerCols(markerCols);
        if (ImGui::InputInt("Marker Rows", &markerRows))
            p->SetMarkerRows(markerRows);
        if (ImGui::InputInt("Saturation Threshold", &satThresh))
            p->SetSaturationThreshold(satThresh);
        if (ImGui::InputInt("Value Threshold", &valThresh))
            p->SetValueThreshold(valThresh);
        if (ImGui::InputInt("AT Block Size", &atBlockSize))
            p->SetAdaptiveThresholdBlockSize(atBlockSize);
        if (ImGui::InputInt("AT Constant", &atConstant))
            p->SetAdaptiveThresholdConstant(atConstant);
        if (ImGui::InputInt("Min Marker Area", &minArea))
            p->SetMinMarkerArea(minArea);
        if (ImGui::InputInt("Max Marker Area", &maxArea))
            p->SetMaxMarkerArea(maxArea);
        if (ImGui::InputInt("Max Hue Deviation", &maxHueDev))
            p->SetMaxHueDeviation(maxHueDev);
        if (ImGui::InputInt("Max Marker Range", &maxRange))
            p->SetMaxMarkerRange(maxRange);

        std::string dictStr = p->GetDictionaryString();
        static char acDict[INPUT_BUF_SIZE] = "";
        if (acDict[0] == '\0' && !dictStr.empty())
            strncpy(acDict, dictStr.c_str(), INPUT_BUF_SIZE - 1);
        if (ImGui::InputText("Dictionary", acDict, INPUT_BUF_SIZE))
            p->SetDictionaryString(std::string(acDict));

        std::string refHues = p->GetReferenceHuesString();
        static char acHues[INPUT_BUF_SIZE] = "";
        if (acHues[0] == '\0' && !refHues.empty())
            strncpy(acHues, refHues.c_str(), INPUT_BUF_SIZE - 1);
        if (ImGui::InputText("Reference Hues", acHues, INPUT_BUF_SIZE))
            p->SetReferenceHuesString(std::string(acHues));

        static char acOutput[INPUT_BUF_SIZE] = "";
        if (acOutput[0] == '\0' && !p->outputFilename.empty())
            strncpy(acOutput, p->outputFilename.c_str(), INPUT_BUF_SIZE - 1);
        if (ImGui::InputText("Output File", acOutput, INPUT_BUF_SIZE))
            p->outputFilename = acOutput;
        ImGui::SameLine();
        if (ImGui::Button("Browse##AC"))
        {
            auto f = pfd::save_file("Output File", p->outputFilename,
                                    {"CSV files", "*.csv", "All files", "*"});
            auto result = f.result();
            if (!result.empty())
            {
                strncpy(acOutput, result.c_str(), INPUT_BUF_SIZE - 1);
                p->outputFilename = result;
            }
        }
    }

    // --- Unknown plugin type fallback ---
    else
    {
        ImGui::Text("No specific UI for this plugin type.");
    }

    ImGui::End();
}

// ============================================================================
// OpenSource
// ============================================================================

void AppGui::OpenSource()
{
    play = false;
    if (ipEngine.capture)
        ipEngine.capture->Stop();

    auto f = pfd::open_file("Open Source", ".",
                            {"Video files", "*.avi *.mp4 *.mkv *.mov *.mpg *.mpeg *.wmv *.flv",
                             "Image files", "*.png *.jpg *.jpeg *.bmp *.tif *.tiff",
                             "All files", "*"});
    auto result = f.result();
    if (result.empty()) return;

    std::string filename = result[0];

    // Determine type and create capture
    Capture* newCapture = nullptr;

    // Try as image first (check extension)
    std::string ext = filename.substr(filename.find_last_of('.') + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    if (ext == "png" || ext == "jpg" || ext == "jpeg" ||
        ext == "bmp" || ext == "tif" || ext == "tiff")
    {
        newCapture = new CaptureImage(filename);
    }
    else
    {
        newCapture = new CaptureVideo(filename);
    }

    if (newCapture)
    {
        delete ipEngine.capture;
        ipEngine.capture = newCapture;
        parameters.inputFilename = filename;

        std::string title = "USE Tracker: " + ipEngine.capture->GetName();
        SDL_SetWindowTitle(window, title.c_str());

        ResetEngine(parameters);
    }
}

// ============================================================================
// SaveSource
// ============================================================================

void AppGui::SaveSource()
{
    // Save source is typically used for multi-video stitching configs
    auto f = pfd::save_file("Save Source Configuration", "source.xml",
                            {"XML files", "*.xml", "All files", "*"});
    auto result = f.result();
    if (result.empty()) return;

    cv::FileStorage fs(result, cv::FileStorage::WRITE);
    if (fs.isOpened())
    {
        fs << "Source" << "{";
        ipEngine.capture->SaveXML(fs);
        fs << "}";
        fs.release();
    }
}

// ============================================================================
// LoadSettings
// ============================================================================

void AppGui::LoadSettings()
{
    auto f = pfd::open_file("Load Settings", ".",
                            {"XML files", "*.xml", "All files", "*"});
    auto result = f.result();
    if (result.empty()) return;

    parameters.parametersFilename = result[0];
    parameters.loadXML(result[0]);

    ResetEngine(parameters);
}

// ============================================================================
// SaveSettings
// ============================================================================

void AppGui::SaveSettings()
{
    auto f = pfd::save_file("Save Settings", "settings.xml",
                            {"XML files", "*.xml", "All files", "*"});
    auto result = f.result();
    if (result.empty()) return;

    // Release any open file handle so we can overwrite
    if (parameters.file.isOpened())
        parameters.file.release();

    cv::FileStorage fs(result, cv::FileStorage::WRITE);
    if (fs.isOpened())
    {
        fs << "Configuration" << "{";

        ipEngine.SaveXML(fs);

        // Save pipeline
        fs << "Pipeline" << "{";

        for (unsigned int i = 0; i < ipEngine.pipelines[0].plugins.size(); i++)
        {
            // Get plugin from thread 0 or the single-thread pipeline
            PipelinePlugin* pp = ipEngine.pipelines[0].plugins[i];
            if (!pp) pp = ipEngine.pipelines[ipEngine.threadsCount].plugins[i];
            if (!pp) continue;

            // Use registry name (matches NewPipelinePluginVector keys)
            std::string pluginName = pp->registryName;

            fs << std::string("Plugin_") + std::to_string(i) << "{"
               << pluginName << "{";

            pp->SaveXML(fs);

            fs << "}" << "}";
        }

        fs << "}"; // Pipeline
        fs << "}"; // Configuration
        fs.release();

        // Re-open the saved file as the current parameters file
        parameters.loadXML(result);
    }
}

// ============================================================================
// ResetEngine
// ============================================================================

void AppGui::ResetEngine()
{
    // Regenerate HUD
    if (ipEngine.capture)
    {
        hud.create(ipEngine.capture->height, ipEngine.capture->width, CV_8UC4);
        hudApp.create(ipEngine.capture->height, ipEngine.capture->width, CV_8UC4);
        ipEngine.hud = hud;
        ipEngine.Reset();
    }
}

void AppGui::ResetEngine(Parameters& params)
{
    ipEngine.Reset(params);

    if (ipEngine.capture)
    {
        hud.create(ipEngine.capture->height, ipEngine.capture->width, CV_8UC4);
        hudApp.create(ipEngine.capture->height, ipEngine.capture->width, CV_8UC4);
        ipEngine.hud = hud;
        ipEngine.takeSnapshot = true;
    }

    // Clean up pipeline dialogs
    pipelineDialogOpen.clear();

    // Reload plugins from parameters
    cv::FileNode fn = params.rootNode["Pipeline"];
    if (!fn.empty())
    {
        cv::FileNodeIterator it = fn.begin(), it_end = fn.end();
        for (; it != it_end; ++it)
        {
            cv::FileNode pluginNode = *((*it).begin()); // ugly hack for duplicate key bug
            AddPipelinePlugin(pluginNode.name(), pluginNode);
        }
    }
}

// ============================================================================
// AddPipelinePlugin
// ============================================================================

bool AppGui::AddPipelinePlugin(const std::string& name, cv::FileNode& fn, int pos)
{
    // try direct lookup first, then convert from display text
    auto it = NewPipelinePluginVector.find(name);
    if (it == NewPipelinePluginVector.end())
        it = NewPipelinePluginVector.find(TextToCamelCase(name));
    if (it == NewPipelinePluginVector.end())
    {
        std::cerr << "Unknown plugin name: " << name << std::endl;
        return false;
    }

    auto pfv = it->second(fn, ipEngine.threadsCount);
    for (auto* p : pfv)
        if (p) p->registryName = it->first;

    if (pos < 0)
    {
        ipEngine.PushBack(pfv, true);
        pipelineDialogOpen.push_back(false);
    }
    else
    {
        ipEngine.Insert(pos, pfv, true);
        pipelineDialogOpen.insert(pipelineDialogOpen.begin() + pos, false);
    }

    return true;
}

// ============================================================================
// CamelCaseToText / TextToCamelCase
// ============================================================================

std::string AppGui::CamelCaseToText(const std::string& txt)
{
    std::string res;

    for (unsigned int i = 0; i < txt.size(); i++)
    {
        char c = tolower(txt[i]);

        // If it was a capital and not the first char, add space
        if (c != txt[i] && i != 0) res += " ";

        res += c;
    }
    return res;
}

std::string AppGui::TextToCamelCase(const std::string& txt)
{
    std::string text = txt;
    std::string res;

    std::transform(text.begin(), text.end(), text.begin(),
                   [](unsigned char c) { return std::tolower(c); });

    bool upNext = true;
    for (unsigned int i = 0; i < text.size(); i++)
    {
        if (std::isalnum(text[i]))
        {
            if (upNext) res += std::toupper(text[i]);
            else res += text[i];
            upNext = false;
        }
        else
        {
            upNext = true;
        }
    }
    return res;
}
