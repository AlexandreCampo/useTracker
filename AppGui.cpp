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
#include <sstream>
#include <system_error>

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
#include "plugins/BackgroundDiffGSOC.h"
#include "plugins/BackgroundDiffKNN.h"
#include "plugins/BackgroundDiffMOG.h"
#include "plugins/BackgroundDiffMOG2.h"
#include "plugins/Clahe.h"
#include "plugins/Curves.h"
#include "plugins/WhiteBalance.h"
#include "plugins/YoloDetector.h"
#include "plugins/PatternTracker.h"
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
    // Note: keyboard navigation is deliberately NOT enabled — it would mark the
    // keyboard as captured by the GUI at all times, breaking playback shortcuts

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

    // rows are tightly packed (width * 3 bytes), not 4-byte aligned
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

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
                RequestQuit();

            if (event.type == SDL_WINDOWEVENT &&
                event.window.event == SDL_WINDOWEVENT_CLOSE &&
                event.window.windowID == SDL_GetWindowID(window))
                RequestQuit();

            // Keyboard shortcuts — active unless the user is typing in a text
            // field or navigating the in-app file browser (which uses the
            // arrow / Enter / Esc keys for its own navigation)
            if (event.type == SDL_KEYDOWN && !ImGui::GetIO().WantTextInput &&
                !fileBrowser.visible && !showQuitConfirm)
            {
                bool ctrl = (event.key.keysym.mod & KMOD_CTRL) != 0;
                HandleShortcut(event.key.keysym.sym, ctrl);
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
// HandleShortcut - Playback and application keyboard shortcuts
// ============================================================================

void AppGui::HandleShortcut(SDL_Keycode key, bool ctrl)
{
    if (!ipEngine.capture) return;

    switch (key)
    {
    case SDLK_SPACE:
        // Play / pause
        play = !play;
        if (play)
            ipEngine.capture->Play();
        else
            ipEngine.capture->Pause();
        pipelineDirty = true;
        break;

    case SDLK_RIGHT:
        // Pause and step forward one frame
        play = false;
        ipEngine.capture->Pause();
        ipEngine.GetNextFrame();
        pipelineDirty = true;
        break;

    case SDLK_LEFT:
        // Pause and step backward one frame
        play = false;
        ipEngine.capture->Pause();
        if (ipEngine.capture->GetFrameCount() > 0)
        {
            double t = ipEngine.capture->GetTime();
            double fps = ipEngine.capture->GetFPS();
            if (fps > 0)
            {
                ipEngine.capture->GetFrame(t - 1.0 / fps);
                pipelineDirty = true;
            }
        }
        break;

    case SDLK_EQUALS:  // + or = key
    case SDLK_PLUS:
    case SDLK_KP_PLUS:
        // Accelerate playback
        if (playSpeed < 4) playSpeed++;
        ipEngine.capture->SetPlaySpeed(playSpeed);
        break;

    case SDLK_MINUS:
    case SDLK_KP_MINUS:
        // Slow down playback
        if (playSpeed > -4) playSpeed--;
        ipEngine.capture->SetPlaySpeed(playSpeed);
        break;

    case SDLK_BACKSPACE:
        // Stop: reset to beginning
        ipEngine.capture->Stop();
        play = false;
        pipelineDirty = true;
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
        RequestQuit();
        break;

    default:
        break;
    }
}

// ============================================================================
// UpdateEngine - Frame timing (derived from MainFrame::OnIdle)
// ============================================================================

void AppGui::UpdateEngine()
{
    if (!ipEngine.capture) return;

    // Update pipeline snapshot position
    unsigned int oldSnapshotPos = ipEngine.snapshotPos;
    if (selectedPipelineItem >= 0 &&
        selectedPipelineItem < (int)ipEngine.pipelines[0].plugins.size())
        ipEngine.snapshotPos = selectedPipelineItem;
    else if (!ipEngine.pipelines.empty() && !ipEngine.pipelines[0].plugins.empty())
        ipEngine.snapshotPos = ipEngine.pipelines[0].plugins.size() - 1;
    if (ipEngine.snapshotPos != oldSnapshotPos)
        pipelineDirty = true;

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
                else
                {
                    pipelineDirty = true;
                }
            }
        }
    }

    // Step the processing pipeline, but only when there is new data to process
    // (new frame, seek, plugin parameter change, pipeline edit, ...)
    if (activeTab == TAB_PROCESSING)
    {
        if (pipelineDirty)
        {
            ipEngine.Step(hudVisible);
            pipelineDirty = false;
        }
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

    // In-app file browser (fallback when no native dialog backend is available)
    DrawFileBrowser();

    // Error message popup
    DrawErrorPopup();

    // Quit confirmation
    DrawQuitConfirm();

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
                RequestQuit();
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Help"))
        {
            if (ImGui::MenuItem("About"))
            {
                // OpenPopup cannot be called from inside the menu scope
                // (ID stack mismatch), defer it below
                showAbout = true;
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    if (showAbout)
    {
        ImGui::OpenPopup("AboutPopup");
        showAbout = false;
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
        pipelineDirty = true;
    }

    ImGui::SameLine();

    // Decrease play speed
    if (ImGui::Button("|<##speed_down", ImVec2(30*dpiScale, 24*dpiScale)))
    {
        if (playSpeed > -4) playSpeed--;
        ipEngine.capture->SetPlaySpeed(playSpeed);
    }

    ImGui::SameLine();

    // Step backward (pause and go back one frame)
    if (ImGui::Button("<##step_back", ImVec2(24*dpiScale, 24*dpiScale)))
    {
        play = false;
        ipEngine.capture->Pause();
        if (ipEngine.capture->GetFrameCount() > 0)
        {
            double t = ipEngine.capture->GetTime();
            double fps = ipEngine.capture->GetFPS();
            if (fps > 0)
            {
                ipEngine.capture->GetFrame(t - 1.0 / fps);
                pipelineDirty = true;
            }
        }
    }

    ImGui::SameLine();

    // Play/Pause
    const char* playLabel = play ? "||##play_pause" : ">##play_pause";
    if (ImGui::Button(playLabel, ImVec2(30*dpiScale, 24*dpiScale)))
    {
        play = !play;
        playSpeed = 0;
        ipEngine.capture->SetPlaySpeed(0);
        if (play)
            ipEngine.capture->Play();
        else
            ipEngine.capture->Pause();
        pipelineDirty = true;
    }

    ImGui::SameLine();

    // Step forward (pause and advance one frame)
    if (ImGui::Button(">##step_fwd", ImVec2(24*dpiScale, 24*dpiScale)))
    {
        play = false;
        ipEngine.capture->Pause();
        ipEngine.GetNextFrame();
        pipelineDirty = true;
    }

    ImGui::SameLine();

    // Increase play speed
    if (ImGui::Button(">|##speed_up", ImVec2(30*dpiScale, 24*dpiScale)))
    {
        if (playSpeed < 4) playSpeed++;
        ipEngine.capture->SetPlaySpeed(playSpeed);
    }


    ImGui::SameLine();
    ImGui::Spacing();
    ImGui::SameLine();

    // Video slider
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 250*dpiScale);
    if (ImGui::SliderFloat("##VideoSlider", &videoSliderPos, 0.0f, 1.0f, ""))
    {
        sliderMoving = true;
        if (ipEngine.capture->GetFrameCount() > 0)
        {
            double totalTime = (double)ipEngine.capture->GetFrameCount() / ipEngine.capture->GetFPS();
            ipEngine.capture->GetFrame(videoSliderPos * totalTime);
            pipelineDirty = true;
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
        pipelineDirty = true; // the HUD is drawn during pipeline processing
    }

    ImGui::SameLine();

    // Processing blending slider
    ImGui::SetNextItemWidth(80*dpiScale);
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

    // only blend the processing result in when there is a plugin producing one
    bool anyActivePlugin = false;
    for (auto& pl : ipEngine.pipelines)
    {
        for (auto* pp : pl.plugins)
            if (pp && pp->active) { anyActivePlugin = true; break; }
        if (anyActivePlugin) break;
    }

    if (!oglScreen.empty())
    {
        if (processingBlending > 0.001f && anyActivePlugin &&
            activeTab == TAB_PROCESSING && !ipEngine.pipelineSnapshot.empty())
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
            // displayFrame may still share pixels with the capture frame;
            // copy before drawing on it or we would corrupt the source data
            if (displayFrame.data == oglScreen.data)
                displayFrame = oglScreen.clone();

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

        // Helper: map current mouse position to frame pixel coordinates
        ImVec2 itemMinAbs = ImGui::GetItemRectMin();
        ImVec2 itemSizeAbs = ImGui::GetItemRectSize();
        auto mouseToFrame = [&](cv::Point& out) -> bool
        {
            if (itemSizeAbs.x <= 0 || itemSizeAbs.y <= 0) return false;
            ImVec2 mp = ImGui::GetMousePos();
            float nx = (mp.x - itemMinAbs.x) / itemSizeAbs.x;
            float ny = (mp.y - itemMinAbs.y) / itemSizeAbs.y;
            float uvx = zoomStartX + nx * (zoomEndX - zoomStartX);
            float uvy = zoomStartY + ny * (zoomEndY - zoomStartY);
            out.x = (int)(uvx * texWidth);
            out.y = (int)(uvy * texHeight);
            return (out.x >= 0 && out.x < texWidth && out.y >= 0 && out.y < texHeight);
        };

        // PatternTracker click-to-seed mode
        bool seedMode = (patternSeedPluginIndex >= 0 &&
                         !ipEngine.pipelines.empty() &&
                         patternSeedPluginIndex < (int)ipEngine.pipelines[ipEngine.threadsCount].plugins.size());
        if (seedMode && ImGui::IsItemHovered())
        {
            PatternTracker* pt = dynamic_cast<PatternTracker*>(
                ipEngine.pipelines[ipEngine.threadsCount].plugins[patternSeedPluginIndex]);
            if (pt)
            {
                cv::Point fp;
                if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && mouseToFrame(fp))
                {
                    pt->AddSeed(fp);
                    pipelineDirty = true;
                }
                if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
                {
                    pt->ClearTargets();
                    pipelineDirty = true;
                }
            }
        }

        // WhiteBalance "pick white" mode
        bool pickMode = (whitePickPluginIndex >= 0 &&
                         !ipEngine.pipelines.empty() &&
                         whitePickPluginIndex < (int)ipEngine.pipelines[ipEngine.threadsCount].plugins.size());
        if (pickMode && ImGui::IsItemHovered())
        {
            WhiteBalance* wbp = dynamic_cast<WhiteBalance*>(
                ipEngine.pipelines[ipEngine.threadsCount].plugins[whitePickPluginIndex]);
            cv::Point fp;
            if (wbp && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && mouseToFrame(fp))
            {
                wbp->AddPick(fp);
                whitePickPluginIndex = -1; // single pick, then leave the mode
                pipelineDirty = true;
            }
        }

        // Polygon ROI editing overlay — active when a ZonesOfInterest plugin
        // is selected in the pipeline
        ZonesOfInterest* roiPlugin = nullptr;
        if (selectedPipelineItem >= 0 && !ipEngine.pipelines.empty() &&
            selectedPipelineItem < (int)ipEngine.pipelines[ipEngine.threadsCount].plugins.size())
            roiPlugin = dynamic_cast<ZonesOfInterest*>(
                ipEngine.pipelines[ipEngine.threadsCount].plugins[selectedPipelineItem]);
        roiEditing = (roiPlugin != nullptr);
        if (roiEditing)
            HandleRoiEditing(roiPlugin, itemMinAbs.x, itemMinAbs.y, itemSizeAbs.x, itemSizeAbs.y);

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

            // Double-click to reset zoom (disabled while seeding targets,
            // picking a white point or editing ROIs, where left click is used)
            if (!seedMode && !pickMode && !roiEditing &&
                ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
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

        char speedStr[32] = "1x";
        if (playSpeed > 0)
            snprintf(speedStr, sizeof(speedStr), "%dx", 1 << playSpeed);
        else if (playSpeed < 0)
            snprintf(speedStr, sizeof(speedStr), "1/%dx", 1 << (-playSpeed));

        ImGui::Text("Time: %02d:%02d  Frame: %ld / %ld  FPS: %.1f  Speed: %s",
                     minutes, seconds, frameNum, frameCount,
                     ipEngine.capture->GetFPS(), speedStr);
    }
}

// ============================================================================
// DrawTabs
// ============================================================================

void AppGui::DrawTabs()
{
    int prevTab = activeTab;
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

    // reprocess when coming back to the processing tab
    if (activeTab != prevTab && activeTab == TAB_PROCESSING)
        pipelineDirty = true;
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
            pipelineDirty = true;
        }
        ImGui::SameLine();

        bool isSelected = (selectedPipelineItem == i);
        if (ImGui::Selectable(label.c_str(), isSelected))
        {
            selectedPipelineItem = i;
            ipEngine.takeSnapshot = true;
            pipelineDirty = true;
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
        auto pfv = ipEngine.Erase(selectedPipelineItem);
        for (auto* p : pfv)
            delete p;
        if (selectedPipelineItem < (int)pipelineDialogOpen.size())
            pipelineDialogOpen.erase(pipelineDialogOpen.begin() + selectedPipelineItem);
        if (selectedPipelineItem >= (int)ipEngine.pipelines[0].plugins.size())
            selectedPipelineItem = (int)ipEngine.pipelines[0].plugins.size() - 1;
        pipelineDirty = true;
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
        pipelineDirty = true;
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
        pipelineDirty = true;
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
        OpenFileDialog("Load Background Image", FileBrowser::OPEN,
                       {"Image files", "*.png *.jpg *.bmp *.tif *.tiff",
                        "All files", "*"}, "",
                       [this](const std::string& filename)
        {
            cv::Mat bg = cv::imread(filename);
            if (!bg.empty())
            {
                ipEngine.bgFilename = filename;
                ipEngine.background = bg;
                ipEngine.Reset();
                pipelineDirty = true;
            }
            else
            {
                errorMessage = "Could not load background image:\n" + filename;
            }
        });
    }
    ImGui::SameLine();

    if (ImGui::Button("Save Background"))
    {
        OpenFileDialog("Save Background Image", FileBrowser::SAVE,
                       {"PNG files", "*.png",
                        "All files", "*"}, "background.png",
                       [this](const std::string& filename)
        {
            cv::imwrite(filename, ipEngine.background);
        });
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
            pipelineDirty = true;
        }
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Text("Zones of Interest");

    // Zones file
    char zonesPath[INPUT_BUF_SIZE];
    snprintf(zonesPath, sizeof(zonesPath), "%s", ipEngine.zonesFilename.c_str());
    if (ImGui::InputText("Zones File", zonesPath, INPUT_BUF_SIZE))
        ipEngine.zonesFilename = zonesPath;
    ImGui::SameLine();

    if (ImGui::Button("Browse##Zones"))
    {
        OpenFileDialog("Load Zones Image", FileBrowser::OPEN,
                       {"Image files", "*.png *.jpg *.bmp *.tif",
                        "All files", "*"}, "",
                       [this](const std::string& filename)
        {
            cv::Mat zones = cv::imread(filename, cv::IMREAD_GRAYSCALE);
            if (!zones.empty())
            {
                ipEngine.zonesFilename = filename;
                ipEngine.zoneMap = zones;
                ipEngine.Reset();
                pipelineDirty = true;
            }
            else
            {
                errorMessage = "Could not load zones image:\n" + filename;
            }
        });
    }

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
    if (boardType >= 0 && boardType <= 2)
    {
        if (ImGui::Combo("Board Type", &boardType, boardTypes, 3))
        {
            ipEngine.capture->CalibrationSetBoardType(boardType);
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
// DrawCurveEditor — interactive draggable tone curve
// ============================================================================

bool AppGui::DrawCurveEditor(const char* id, std::vector<cv::Point2f>& pts)
{
    ImGui::PushID(id);
    bool changed = false;

    float sz = 256.0f * dpiScale;
    ImVec2 p0 = ImGui::GetCursorScreenPos();
    ImGui::InvisibleButton("canvas", ImVec2(sz, sz));
    bool hovered = ImGui::IsItemHovered();
    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImVec2 p1(p0.x + sz, p0.y + sz);

    // background, border, grid, identity diagonal
    dl->AddRectFilled(p0, p1, IM_COL32(30, 30, 30, 255));
    for (int i = 1; i < 4; i++)
    {
        float t = i / 4.0f;
        dl->AddLine(ImVec2(p0.x + t * sz, p0.y), ImVec2(p0.x + t * sz, p1.y), IM_COL32(55, 55, 55, 255));
        dl->AddLine(ImVec2(p0.x, p0.y + t * sz), ImVec2(p1.x, p0.y + t * sz), IM_COL32(55, 55, 55, 255));
    }
    dl->AddLine(ImVec2(p0.x, p1.y), ImVec2(p1.x, p0.y), IM_COL32(70, 70, 70, 255));
    dl->AddRect(p0, p1, IM_COL32(90, 90, 90, 255));

    // data (0..255) <-> screen, y is inverted
    auto toScreen = [&](float dx, float dy) {
        return ImVec2(p0.x + (dx / 255.0f) * sz, p1.y - (dy / 255.0f) * sz);
    };
    auto clamp255 = [](float v) { return v < 0.0f ? 0.0f : (v > 255.0f ? 255.0f : v); };

    // the interpolated curve
    unsigned char lut[256];
    Curves::BuildCurveLUT(pts, lut);
    ImVec2 prev = toScreen(0.0f, (float)lut[0]);
    for (int x = 1; x < 256; x++)
    {
        ImVec2 cur = toScreen((float)x, (float)lut[x]);
        dl->AddLine(prev, cur, IM_COL32(235, 235, 130, 255), 2.0f);
        prev = cur;
    }

    // per-widget drag state (survives across frames, isolated by ImGui ID)
    ImGuiStorage* store = ImGui::GetStateStorage();
    ImGuiID dragKey = ImGui::GetID("drag");
    int drag = store->GetInt(dragKey, -1);

    float grabR = 7.0f * dpiScale;
    ImVec2 mouse = ImGui::GetMousePos();

    // points + hit test
    int hoverIdx = -1;
    for (int i = 0; i < (int)pts.size(); i++)
    {
        ImVec2 sp = toScreen(pts[i].x, pts[i].y);
        float dx = mouse.x - sp.x, dy = mouse.y - sp.y;
        bool hot = (dx * dx + dy * dy <= grabR * grabR);
        if (hot) hoverIdx = i;
        float r = (hot ? 6.0f : 4.0f) * dpiScale;
        dl->AddCircleFilled(sp, r, IM_COL32(255, 255, 255, 255));
        dl->AddCircle(sp, r, IM_COL32(0, 0, 0, 255));
    }

    if (hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
    {
        if (hoverIdx >= 0)
            drag = hoverIdx;
        else
        {
            float nx = clamp255((mouse.x - p0.x) / sz * 255.0f);
            float ny = clamp255((p1.y - mouse.y) / sz * 255.0f);
            int ins = 0;
            while (ins < (int)pts.size() && pts[ins].x < nx) ins++;
            pts.insert(pts.begin() + ins, cv::Point2f(nx, ny));
            drag = ins;
            changed = true;
        }
    }

    // right click removes an interior point
    if (hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Right) &&
        hoverIdx > 0 && hoverIdx < (int)pts.size() - 1)
    {
        pts.erase(pts.begin() + hoverIdx);
        changed = true;
    }

    // dragging
    if (drag >= 0 && drag < (int)pts.size() && ImGui::IsMouseDown(ImGuiMouseButton_Left))
    {
        float ny = clamp255((p1.y - mouse.y) / sz * 255.0f);
        float nx = clamp255((mouse.x - p0.x) / sz * 255.0f);
        if (drag == 0)
            pts[0] = cv::Point2f(0.0f, ny);
        else if (drag == (int)pts.size() - 1)
            pts[drag] = cv::Point2f(255.0f, ny);
        else
        {
            float lo = pts[drag - 1].x + 1.0f;
            float hi = pts[drag + 1].x - 1.0f;
            pts[drag] = cv::Point2f(std::min(hi, std::max(lo, nx)), ny);
        }
        changed = true;
    }
    if (!ImGui::IsMouseDown(ImGuiMouseButton_Left)) drag = -1;

    store->SetInt(dragKey, drag);

    ImGui::TextDisabled("click: add/move point   right-click: remove");

    ImGui::PopID();
    return changed;
}

// ============================================================================
// HandleRoiEditing — draw and edit polygon ROIs over the video
// ============================================================================

void AppGui::HandleRoiEditing(ZonesOfInterest* zoi,
                             float minX, float minY, float sizeX, float sizeY)
{
    if (!zoi || sizeX <= 0 || sizeY <= 0 || texWidth <= 0 || texHeight <= 0) return;

    ImDrawList* dl = ImGui::GetWindowDrawList();

    auto frameToScreen = [&](const cv::Point& p) {
        float u = (float)p.x / texWidth;
        float v = (float)p.y / texHeight;
        float nx = (u - zoomStartX) / (zoomEndX - zoomStartX);
        float ny = (v - zoomStartY) / (zoomEndY - zoomStartY);
        return ImVec2(minX + nx * sizeX, minY + ny * sizeY);
    };
    auto mouseToFrame = [&](cv::Point& out) {
        ImVec2 m = ImGui::GetMousePos();
        float nx = (m.x - minX) / sizeX, ny = (m.y - minY) / sizeY;
        float u = zoomStartX + nx * (zoomEndX - zoomStartX);
        float v = zoomStartY + ny * (zoomEndY - zoomStartY);
        out.x = (int)lround(u * texWidth);
        out.y = (int)lround(v * texHeight);
    };
    auto regionColor = [](int r) -> ImU32 {
        static const ImU32 pal[] = {
            IM_COL32(130, 130, 130, 255), IM_COL32(80, 200, 120, 255),
            IM_COL32(90, 160, 255, 255),  IM_COL32(255, 170, 60, 255),
            IM_COL32(230, 90, 220, 255),  IM_COL32(240, 230, 80, 255),
        };
        int n = (int)(sizeof(pal) / sizeof(pal[0]));
        return pal[((r % n) + n) % n];
    };

    auto& polys = zoi->polygons;

    // normalise possibly stale indices (polygons can be removed here or from
    // the plugin dialog) before any of them is used to index the vector
    if (roiActivePolygon >= (int)polys.size()) roiActivePolygon = -1;
    if (roiSelectedPolygon >= (int)polys.size()) roiSelectedPolygon = -1;
    if (roiDragPoly >= (int)polys.size()) { roiDragPoly = -1; roiDragPoint = -1; }
    if (roiDragPoly >= 0 && roiDragPoint >= (int)polys[roiDragPoly].points.size())
    { roiDragPoly = -1; roiDragPoint = -1; }

    // draw all polygons
    for (int pi = 0; pi < (int)polys.size(); pi++)
    {
        auto& poly = polys[pi];
        ImU32 col = regionColor(poly.region);
        int np = (int)poly.points.size();
        bool active = (pi == roiActivePolygon);
        for (int i = 0; i < np; i++)
        {
            ImVec2 a = frameToScreen(poly.points[i]);
            if (i + 1 < np)
                dl->AddLine(a, frameToScreen(poly.points[i + 1]), col, 2.0f);
            else if (!active && np >= 3)
                dl->AddLine(a, frameToScreen(poly.points[0]), col, 2.0f);
        }
        for (int i = 0; i < np; i++)
        {
            ImVec2 a = frameToScreen(poly.points[i]);
            bool sel = (pi == roiSelectedPolygon);
            float r = sel ? 5.0f : 4.0f;
            dl->AddCircleFilled(a, r, IM_COL32(255, 255, 255, 255));
            dl->AddCircle(a, r, col, 0, 2.0f);
        }
        if (np > 0)
        {
            ImVec2 a = frameToScreen(poly.points[0]);
            char lbl[16]; snprintf(lbl, sizeof(lbl), "R%d", poly.region);
            dl->AddText(ImVec2(a.x + 6, a.y - 14), col, lbl);
        }
    }

    ImVec2 mouse = ImGui::GetMousePos();

    // near the first vertex of the polygon being drawn? then a click closes it
    float closeR = 10.0f * dpiScale;
    bool canClose = false;
    if (roiActivePolygon >= 0 && roiActivePolygon < (int)polys.size() &&
        polys[roiActivePolygon].points.size() >= 3)
    {
        ImVec2 first = frameToScreen(polys[roiActivePolygon].points.front());
        float dx = mouse.x - first.x, dy = mouse.y - first.y;
        canClose = (dx * dx + dy * dy <= closeR * closeR);
        if (canClose)
        {
            dl->AddCircle(first, closeR, IM_COL32(255, 255, 255, 255), 0, 2.0f);
            ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
        }
    }

    // rubber-band from the last vertex of the polygon being drawn
    if (roiActivePolygon >= 0 && roiActivePolygon < (int)polys.size() &&
        !polys[roiActivePolygon].points.empty())
    {
        ImVec2 last = frameToScreen(polys[roiActivePolygon].points.back());
        dl->AddLine(last, mouse, regionColor(polys[roiActivePolygon].region), 1.0f);
    }

    if (!ImGui::IsItemHovered())
    {
        if (!ImGui::IsMouseDown(ImGuiMouseButton_Left)) { roiDragPoly = -1; roiDragPoint = -1; }
        return;
    }

    // vertex hit test in screen space
    float grab = 8.0f * dpiScale;
    int hitPoly = -1, hitPt = -1;
    for (int pi = 0; pi < (int)polys.size() && hitPoly < 0; pi++)
        for (int i = 0; i < (int)polys[pi].points.size(); i++)
        {
            ImVec2 s = frameToScreen(polys[pi].points[i]);
            float dx = mouse.x - s.x, dy = mouse.y - s.y;
            if (dx * dx + dy * dy <= grab * grab) { hitPoly = pi; hitPt = i; break; }
        }

    bool changed = false;
    cv::Point fp; mouseToFrame(fp);

    if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
    {
        if (canClose)
        {
            roiActivePolygon = -1; // click near the first vertex closes the polygon
        }
        else if (hitPoly >= 0)
        {
            roiDragPoly = hitPoly; roiDragPoint = hitPt; roiSelectedPolygon = hitPoly;
        }
        else
        {
            if (roiActivePolygon < 0 || roiActivePolygon >= (int)polys.size())
            {
                ZonesOfInterest::Polygon np;
                np.region = 1; // new polygons default to region 1
                polys.push_back(np);
                roiActivePolygon = (int)polys.size() - 1;
                roiSelectedPolygon = roiActivePolygon;
            }
            polys[roiActivePolygon].points.push_back(fp);
            changed = true;
        }
    }

    if (roiDragPoly >= 0 && ImGui::IsMouseDown(ImGuiMouseButton_Left))
    {
        if (roiDragPoly < (int)polys.size() && roiDragPoint < (int)polys[roiDragPoly].points.size())
        {
            polys[roiDragPoly].points[roiDragPoint] = fp;
            changed = true;
        }
    }
    if (!ImGui::IsMouseDown(ImGuiMouseButton_Left)) { roiDragPoly = -1; roiDragPoint = -1; }

    if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
    {
        if (hitPoly >= 0 && hitPoly < (int)polys.size() &&
            hitPt >= 0 && hitPt < (int)polys[hitPoly].points.size())
        {
            // delete the vertex; if the polygon becomes empty, remove it and
            // shift the active/selected indices to stay consistent
            polys[hitPoly].points.erase(polys[hitPoly].points.begin() + hitPt);
            bool removed = polys[hitPoly].points.empty();
            if (removed) polys.erase(polys.begin() + hitPoly);

            auto fixup = [&](int& idx) {
                if (removed) { if (idx == hitPoly) idx = -1; else if (idx > hitPoly) idx--; }
            };
            fixup(roiActivePolygon);
            fixup(roiSelectedPolygon);
            roiDragPoly = -1; roiDragPoint = -1;
            changed = true;
        }
        else
        {
            roiActivePolygon = -1; // finish the current polygon
        }
    }

    if (changed)
    {
        zoi->Rasterize();
        pipelineDirty = true;
    }
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
        pipelineDirty = true;
    }

    bool isOutput = pp->output;
    if (ImGui::Checkbox("Output", &isOutput))
    {
        for (unsigned int p = 0; p <= ipEngine.threadsCount; p++)
            if (ipEngine.pipelines[p].plugins[index])
                ipEngine.pipelines[p].plugins[index]->output = isOutput;
        pipelineDirty = true;
    }

    ImGui::Separator();

    // any widget edit below sets this, triggering a reprocessing of the
    // pipeline so the parameter change is immediately visible
    bool changed = false;

    // Type-specific controls using dynamic_cast

    // --- AdaptiveThreshold ---
    if (AdaptiveThreshold* p = dynamic_cast<AdaptiveThreshold*>(pp))
    {
        int blockSize = p->blockSize;
        if (ImGui::InputInt("Block Size", &blockSize))
        {
            p->SetBlockSize(blockSize);
            changed = true;
        }

        int constant = p->constant;
        if (ImGui::InputInt("Constant (C)", &constant))
        {
            p->SetConstant(constant);
            changed = true;
        }

        int method = p->thresholdMethod;
        const char* methods[] = { "Mean", "Gaussian" };
        int sel = (method == cv::ADAPTIVE_THRESH_GAUSSIAN_C) ? 1 : 0;
        if (ImGui::Combo("Threshold Method", &sel, methods, 2))
        {
            p->SetThresholdMethod(sel == 1 ? cv::ADAPTIVE_THRESH_GAUSSIAN_C
                                            : cv::ADAPTIVE_THRESH_MEAN_C);
            changed = true;
        }

        changed |= ImGui::Checkbox("Additive", &p->additive);
        changed |= ImGui::Checkbox("Invert", &p->invert);
        changed |= ImGui::Checkbox("Restrict to Zone", &p->restrictToZone);
        if (p->restrictToZone)
            changed |= ImGui::InputInt("Zone", &p->zone);

        // Sync to all thread instances
        if (changed)
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
        changed |= ImGui::InputInt("Threshold", &p->threshold);
        changed |= ImGui::Checkbox("Additive", &p->additive);
        changed |= ImGui::Checkbox("Restrict to Zone", &p->restrictToZone);
        if (p->restrictToZone)
            changed |= ImGui::InputInt("Zone", &p->zone);

        // Sync to all thread instances
        if (changed)
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
        {
            p->SetHistory(history);
            changed = true;
        }

        int nMixtures = p->nMixtures;
        if (ImGui::InputInt("Num Mixtures", &nMixtures))
        {
            p->SetNMixtures(nMixtures);
            changed = true;
        }

        float bgRatioF = (float)p->backgroundRatio;
        if (ImGui::InputFloat("Background Ratio", &bgRatioF, 0.01f, 0.1f, "%.3f"))
        {
            p->SetBackgroundRatio(bgRatioF);
            changed = true;
        }

        float noiseSigmaF = (float)p->noiseSigma;
        if (ImGui::InputFloat("Noise Sigma", &noiseSigmaF, 0.1f, 1.0f, "%.2f"))
        {
            p->SetNoiseSigma(noiseSigmaF);
            changed = true;
        }

        float lr = (float)p->learningRate;
        if (ImGui::InputFloat("Learning Rate", &lr, 0.001f, 0.01f, "%.4f"))
        {
            p->learningRate = lr;
            changed = true;
        }

        changed |= ImGui::Checkbox("Additive", &p->additive);
        changed |= ImGui::Checkbox("Restrict to Zone", &p->restrictToZone);
        if (p->restrictToZone)
            changed |= ImGui::InputInt("Zone", &p->zone);

        // Sync to all thread instances (setters are no-ops when unchanged,
        // so this does not reset the background models of the other threads)
        if (changed)
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
        {
            p->SetHistory(history);
            changed = true;
        }

        float thresh = (float)p->threshold;
        if (ImGui::InputFloat("Threshold", &thresh, 0.1f, 1.0f, "%.2f"))
        {
            p->SetThreshold(thresh);
            changed = true;
        }

        bool shadow = p->shadowDetection;
        if (ImGui::Checkbox("Shadow Detection", &shadow))
        {
            p->SetShadowDetection(shadow);
            changed = true;
        }

        float lr = (float)p->learningRate;
        if (ImGui::InputFloat("Learning Rate", &lr, 0.001f, 0.01f, "%.4f"))
        {
            p->learningRate = lr;
            changed = true;
        }

        changed |= ImGui::Checkbox("Additive", &p->additive);
        changed |= ImGui::Checkbox("Restrict to Zone", &p->restrictToZone);
        if (p->restrictToZone)
            changed |= ImGui::InputInt("Zone", &p->zone);

        // Sync to all thread instances (setters are no-ops when unchanged,
        // so this does not reset the background models of the other threads)
        if (changed)
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
        {
            p->learningRate = lr;
            changed = true;
        }

        changed |= ImGui::Checkbox("Additive", &p->additive);
        changed |= ImGui::Checkbox("Restrict to Zone", &p->restrictToZone);
        if (p->restrictToZone)
            changed |= ImGui::InputInt("Zone", &p->zone);

        // Sync to all thread instances
        if (changed)
            for (unsigned int t = 1; t < ipEngine.threadsCount; t++)
                if (auto* tp = dynamic_cast<BackgroundDiffGMG*>(ipEngine.pipelines[t].plugins[index]))
                {
                    tp->learningRate = p->learningRate;
                    tp->additive = p->additive;
                    tp->restrictToZone = p->restrictToZone;
                    tp->zone = p->zone;
                }
    }

    // --- BackgroundDiffGSOC ---
    else if (BackgroundDiffGSOC* p = dynamic_cast<BackgroundDiffGSOC*>(pp))
    {
        int nSamples = p->nSamples;
        if (ImGui::InputInt("Num Samples", &nSamples))
        {
            p->SetNSamples(nSamples);
            changed = true;
        }

        float replaceRate = p->replaceRate;
        if (ImGui::InputFloat("Replace Rate", &replaceRate, 0.001f, 0.01f, "%.4f"))
        {
            p->SetReplaceRate(replaceRate);
            changed = true;
        }

        float propagationRate = p->propagationRate;
        if (ImGui::InputFloat("Propagation Rate", &propagationRate, 0.001f, 0.01f, "%.4f"))
        {
            p->SetPropagationRate(propagationRate);
            changed = true;
        }

        int hitsThreshold = p->hitsThreshold;
        if (ImGui::InputInt("Hits Threshold", &hitsThreshold))
        {
            p->SetHitsThreshold(hitsThreshold);
            changed = true;
        }

        changed |= ImGui::Checkbox("Additive", &p->additive);
        changed |= ImGui::Checkbox("Restrict to Zone", &p->restrictToZone);
        if (p->restrictToZone)
            changed |= ImGui::InputInt("Zone", &p->zone);

        // Sync to all thread instances (setters are no-ops when unchanged,
        // so this does not reset the background models of the other threads)
        if (changed)
            for (unsigned int t = 1; t < ipEngine.threadsCount; t++)
                if (auto* tp = dynamic_cast<BackgroundDiffGSOC*>(ipEngine.pipelines[t].plugins[index]))
                {
                    tp->SetNSamples(p->nSamples);
                    tp->SetReplaceRate(p->replaceRate);
                    tp->SetPropagationRate(p->propagationRate);
                    tp->SetHitsThreshold(p->hitsThreshold);
                    tp->additive = p->additive;
                    tp->restrictToZone = p->restrictToZone;
                    tp->zone = p->zone;
                }
    }

    // --- BackgroundDiffKNN ---
    else if (BackgroundDiffKNN* p = dynamic_cast<BackgroundDiffKNN*>(pp))
    {
        int history = p->history;
        if (ImGui::InputInt("History", &history))
        {
            p->SetHistory(history);
            changed = true;
        }

        float thresh = (float)p->threshold;
        if (ImGui::InputFloat("Dist2 Threshold", &thresh, 10.0f, 100.0f, "%.1f"))
        {
            p->SetThreshold(thresh);
            changed = true;
        }

        bool shadow = p->shadowDetection;
        if (ImGui::Checkbox("Shadow Detection", &shadow))
        {
            p->SetShadowDetection(shadow);
            changed = true;
        }

        float lr = (float)p->learningRate;
        if (ImGui::InputFloat("Learning Rate", &lr, 0.001f, 0.01f, "%.4f"))
        {
            p->learningRate = lr;
            changed = true;
        }
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("-1 = automatic");

        changed |= ImGui::Checkbox("Additive", &p->additive);
        changed |= ImGui::Checkbox("Restrict to Zone", &p->restrictToZone);
        if (p->restrictToZone)
            changed |= ImGui::InputInt("Zone", &p->zone);

        // Sync to all thread instances (KNN parameters change live,
        // the background models are preserved)
        if (changed)
            for (unsigned int t = 1; t < ipEngine.threadsCount; t++)
                if (auto* tp = dynamic_cast<BackgroundDiffKNN*>(ipEngine.pipelines[t].plugins[index]))
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

    // --- Clahe (single threaded, no sync needed) ---
    else if (Clahe* p = dynamic_cast<Clahe*>(pp))
    {
        ImGui::TextWrapped("Contrast enhancement, modifies the frame in place. "
                           "Place it before the detection plugins.");
        ImGui::Spacing();

        float clipLimit = p->clipLimit;
        if (ImGui::InputFloat("Clip Limit", &clipLimit, 0.5f, 1.0f, "%.1f"))
        {
            p->SetClipLimit(clipLimit);
            changed = true;
        }

        int tileSize = p->tileSize;
        if (ImGui::InputInt("Tile Size", &tileSize))
        {
            p->SetTileSize(tileSize);
            changed = true;
        }
    }

    // --- WhiteBalance (single threaded, no sync needed) ---
    else if (WhiteBalance* p = dynamic_cast<WhiteBalance*>(pp))
    {
        ImGui::TextWrapped("Automatic color cast removal, modifies the frame "
                           "in place. Place it before the detection plugins.");
        ImGui::Spacing();

        int type = p->type;
        const char* types[] = { "Gray World (auto)", "Simple (auto)",
                                "Manual / Pick", "Underwater" };
        if (ImGui::Combo("Mode", &type, types, 4))
        {
            p->SetType(type);
            changed = true;
        }

        if (p->type == WhiteBalance::GRAYWORLD)
        {
            float sat = p->saturationThreshold;
            if (ImGui::InputFloat("Saturation Threshold", &sat, 0.05f, 0.1f, "%.2f"))
            {
                p->SetSaturationThreshold(sat);
                changed = true;
            }
        }
        else if (p->type == WhiteBalance::MANUAL)
        {
            bool picking = (whitePickPluginIndex == index);
            if (ImGui::Checkbox("Pick white (click a neutral pixel)", &picking))
                whitePickPluginIndex = picking ? index : -1;
            if (picking)
                ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.2f, 1.0f),
                                   "Click a pixel that should be neutral grey/white");

            changed |= ImGui::SliderFloat("Gain R", &p->gainR, 0.2f, 5.0f, "%.2f");
            changed |= ImGui::SliderFloat("Gain G", &p->gainG, 0.2f, 5.0f, "%.2f");
            changed |= ImGui::SliderFloat("Gain B", &p->gainB, 0.2f, 5.0f, "%.2f");
            if (ImGui::Button("Reset Gains"))
            {
                p->ResetGains();
                changed = true;
            }
        }
        else if (p->type == WhiteBalance::UNDERWATER)
        {
            ImGui::TextWrapped("Restores the red channel absorbed by water, "
                               "then normalises the colour.");
            changed |= ImGui::SliderFloat("Strength", &p->redCompensation, 0.0f, 2.0f, "%.2f");
        }
    }

    // --- Curves (single threaded, no sync needed) ---
    else if (Curves* p = dynamic_cast<Curves*>(pp))
    {
        ImGui::TextWrapped("Per-channel tone/colour curves, applied in place. "
                           "Master affects all channels; R/G/B are added on top.");
        ImGui::Spacing();

        const char* chans[] = { "Master", "Red", "Green", "Blue" };
        ImGui::Combo("Channel", &p->editChannel, chans, 4);

        if (DrawCurveEditor("##curve", p->points[p->editChannel]))
        {
            p->MarkDirty();
            changed = true;
        }

        if (ImGui::Button("Reset Channel"))
        {
            p->SetIdentity(p->editChannel);
            changed = true;
        }
        ImGui::SameLine();
        if (ImGui::Button("Reset All"))
        {
            for (int c = 0; c < Curves::NUM_CHANNELS; c++) p->SetIdentity(c);
            changed = true;
        }
    }

    // --- YoloDetector (single threaded, no sync needed) ---
    else if (YoloDetector* p = dynamic_cast<YoloDetector*>(pp))
    {
        ImGui::TextWrapped("Deep learning detector. Provide an ONNX YOLO model "
                           "(v5 / v8 / v11) and a class names file.");
        ImGui::Spacing();

        // Model file
        char modelBuf[INPUT_BUF_SIZE];
        snprintf(modelBuf, sizeof(modelBuf), "%s", p->modelFilename.c_str());
        if (ImGui::InputText("Model (.onnx)", modelBuf, INPUT_BUF_SIZE))
        {
            p->modelFilename = modelBuf;
            changed = true;
        }
        ImGui::SameLine();
        if (ImGui::Button("Browse##Yolo"))
        {
            OpenFileDialog("Open ONNX Model", FileBrowser::OPEN,
                           {"ONNX models", "*.onnx", "All files", "*"},
                           p->modelFilename,
                           [p, this](const std::string& filename)
            {
                p->modelFilename = filename;
                p->LoadModel();
                pipelineDirty = true;
            });
        }

        // Class names file
        char classBuf[INPUT_BUF_SIZE];
        snprintf(classBuf, sizeof(classBuf), "%s", p->classNamesFilename.c_str());
        if (ImGui::InputText("Class Names", classBuf, INPUT_BUF_SIZE))
        {
            p->classNamesFilename = classBuf;
            changed = true;
        }
        ImGui::SameLine();
        if (ImGui::Button("Browse##YoloNames"))
        {
            OpenFileDialog("Open Class Names File", FileBrowser::OPEN,
                           {"Text files", "*.txt *.names", "All files", "*"},
                           p->classNamesFilename,
                           [p, this](const std::string& filename)
            {
                p->classNamesFilename = filename;
                p->LoadClassNames();
                pipelineDirty = true;
            });
        }

        if (ImGui::Button("Reload Model"))
        {
            p->LoadModel();
            p->LoadClassNames();
            changed = true;
        }
        ImGui::SameLine();
        ImGui::TextWrapped("%s", p->status.c_str());

        ImGui::Separator();

        int modelType = p->modelType;
        const char* modelTypes[] = { "Auto", "YOLOv5", "YOLOv8 / v11" };
        if (ImGui::Combo("Model Type", &modelType, modelTypes, 3))
        {
            p->modelType = modelType;
            changed = true;
        }
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Output layout. Auto suits standard Ultralytics exports.");

        int target = p->target;
        const char* targets[] = { "CPU", "OpenCL (GPU)", "OpenCL FP16 (GPU)", "Vulkan (GPU)" };
        if (ImGui::Combo("Compute Target", &target, targets, 4))
        {
            p->target = target;
            p->ApplyTarget(); // probe the target now, falls back to CPU if it fails
            changed = true;
        }
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("GPU targets need a working OpenCL/Vulkan runtime; "
                              "the plugin falls back to CPU if the target cannot run the model.");
        if (p->netLoaded && p->activeTarget != p->target)
            ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.2f, 1.0f),
                               "Requested target unavailable, running on %s",
                               YoloDetector::TargetName(p->activeTarget).c_str());

        const char* sizes[] = { "320", "416", "512", "640", "1280" };
        const int sizeVals[] = { 320, 416, 512, 640, 1280 };
        int sizeIdx = 3;
        for (int i = 0; i < 5; i++) if (sizeVals[i] == p->inputSize) sizeIdx = i;
        if (ImGui::Combo("Input Size", &sizeIdx, sizes, 5))
        {
            p->inputSize = sizeVals[sizeIdx];
            changed = true;
        }

        if (ImGui::SliderFloat("Confidence", &p->confidenceThreshold, 0.01f, 1.0f, "%.2f"))
            changed = true;
        if (ImGui::SliderFloat("NMS Threshold", &p->nmsThreshold, 0.01f, 1.0f, "%.2f"))
            changed = true;

        char filterBuf[INPUT_BUF_SIZE];
        snprintf(filterBuf, sizeof(filterBuf), "%s", p->classFilter.c_str());
        if (ImGui::InputText("Class Filter", filterBuf, INPUT_BUF_SIZE))
        {
            p->classFilter = filterBuf;
            changed = true;
        }
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Comma separated class names or ids, empty = all classes");

        changed |= ImGui::Checkbox("Additive", &p->additive);

        // Output file
        char outBuf[INPUT_BUF_SIZE];
        snprintf(outBuf, sizeof(outBuf), "%s", p->outputFilename.c_str());
        if (ImGui::InputText("Output File", outBuf, INPUT_BUF_SIZE))
            p->outputFilename = outBuf;
        ImGui::SameLine();
        if (ImGui::Button("Browse##YoloOut"))
        {
            OpenFileDialog("Output File", FileBrowser::SAVE,
                           {"CSV files", "*.csv", "All files", "*"},
                           p->outputFilename,
                           [p](const std::string& filename)
            {
                p->outputFilename = filename;
            });
        }

        // a parameter change must re-run inference even on a paused frame,
        // otherwise the tuning would not be reflected in the view
        if (changed) p->cacheValid = false;
    }

    // --- PatternTracker (single threaded, no sync needed) ---
    else if (PatternTracker* p = dynamic_cast<PatternTracker*>(pp))
    {
        ImGui::TextWrapped("Follows a target by matching its appearance in a "
                           "local window each frame. Place it after Extract "
                           "Blobs if you want automatic seeding.");
        ImGui::Spacing();

        // seed by clicking on the video
        bool seeding = (patternSeedPluginIndex == index);
        if (ImGui::Checkbox("Click on video to add targets", &seeding))
            patternSeedPluginIndex = seeding ? index : -1;
        if (seeding)
            ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.2f, 1.0f),
                               "Left click = add target, right click = clear all");

        ImGui::SameLine();
        if (ImGui::Button("Clear Targets"))
            p->ClearTargets();

        ImGui::Text("Active targets: %d", (int)p->targets.size());

        ImGui::Separator();

        int backend = p->backend;
        const char* backends[] = { "Template match", "CSRT" };
        if (ImGui::Combo("Backend", &backend, backends, 2))
        {
            p->SetBackend(backend);
            changed = true;
        }

        if (ImGui::InputInt("Search Distance (px)", &p->maxDistance))
        {
            if (p->maxDistance < 1) p->maxDistance = 1;
            changed = true;
        }
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Max pixels the target may move between frames");

        if (ImGui::InputInt("Template Size (px)", &p->templateSize))
        {
            if (p->templateSize < 8) p->templateSize = 8;
            changed = true;
        }

        if (p->backend == PatternTracker::TEMPLATE)
        {
            changed |= ImGui::SliderFloat("Match Threshold", &p->matchThreshold, 0.0f, 1.0f, "%.2f");
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("Below this correlation the target is lost");
            changed |= ImGui::SliderFloat("Update Threshold", &p->updateThreshold, 0.0f, 1.0f, "%.2f");
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("Adapt the template only above this confidence");
            changed |= ImGui::SliderFloat("Update Rate", &p->updateRate, 0.0f, 1.0f, "%.2f");
        }

        changed |= ImGui::Checkbox("Predict position", &p->usePrediction);
        changed |= ImGui::InputInt("Max Lost Frames", &p->maxLostFrames);
        changed |= ImGui::InputInt("Max Targets", &p->maxTargets);
        changed |= ImGui::InputInt("Trail Length", &p->trailLength);

        ImGui::Separator();
        changed |= ImGui::Checkbox("Seed from detected blobs", &p->seedFromDetection);
        if (p->seedFromDetection)
            changed |= ImGui::InputInt("Min Blob Size to Seed", &p->minBlobSeedSize);

        changed |= ImGui::Checkbox("Additive", &p->additive);

        char ptOut[INPUT_BUF_SIZE];
        snprintf(ptOut, sizeof(ptOut), "%s", p->outputFilename.c_str());
        if (ImGui::InputText("Output File", ptOut, INPUT_BUF_SIZE))
            p->outputFilename = ptOut;
        ImGui::SameLine();
        if (ImGui::Button("Browse##PTOut"))
        {
            OpenFileDialog("Output File", FileBrowser::SAVE,
                           {"CSV files", "*.csv", "All files", "*"},
                           p->outputFilename,
                           [p](const std::string& filename)
            {
                p->outputFilename = filename;
            });
        }
    }

    // --- ColorSegmentation ---
    else if (ColorSegmentation* p = dynamic_cast<ColorSegmentation*>(pp))
    {
        int type = (int)p->type;
        const char* typeNames[] = { "BGR", "HSV" };
        if (ImGui::Combo("Color Space", &type, typeNames, 2))
        {
            p->type = (ColorSegmentation::Type)type;
            changed = true;
        }

        if (p->type == ColorSegmentation::HSV)
        {
            int minH = p->minHSV[0], minS = p->minHSV[1], minV = p->minHSV[2];
            int maxH = p->maxHSV[0], maxS = p->maxHSV[1], maxV = p->maxHSV[2];

            changed |= ImGui::SliderInt("Min Hue", &minH, 0, 180);
            changed |= ImGui::SliderInt("Max Hue", &maxH, 0, 180);
            changed |= ImGui::SliderInt("Min Saturation", &minS, 0, 255);
            changed |= ImGui::SliderInt("Max Saturation", &maxS, 0, 255);
            changed |= ImGui::SliderInt("Min Value", &minV, 0, 255);
            changed |= ImGui::SliderInt("Max Value", &maxV, 0, 255);

            p->minHSV = cv::Vec3b(minH, minS, minV);
            p->maxHSV = cv::Vec3b(maxH, maxS, maxV);
        }
        else
        {
            int minB = p->minBGR[0], minG = p->minBGR[1], minR = p->minBGR[2];
            int maxB = p->maxBGR[0], maxG = p->maxBGR[1], maxR = p->maxBGR[2];

            changed |= ImGui::SliderInt("Min Blue", &minB, 0, 255);
            changed |= ImGui::SliderInt("Max Blue", &maxB, 0, 255);
            changed |= ImGui::SliderInt("Min Green", &minG, 0, 255);
            changed |= ImGui::SliderInt("Max Green", &maxG, 0, 255);
            changed |= ImGui::SliderInt("Min Red", &minR, 0, 255);
            changed |= ImGui::SliderInt("Max Red", &maxR, 0, 255);

            p->minBGR = cv::Vec3b(minB, minG, minR);
            p->maxBGR = cv::Vec3b(maxB, maxG, maxR);
        }

        changed |= ImGui::Checkbox("Additive", &p->additive);
        changed |= ImGui::Checkbox("Restrict to Zone", &p->restrictToZone);
        if (p->restrictToZone)
            changed |= ImGui::InputInt("Zone", &p->zone);

        // Sync to all thread instances
        if (changed)
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
        {
            p->SetSize(size);
            changed = true;
        }
    }

    // --- Erosion ---
    else if (Erosion* p = dynamic_cast<Erosion*>(pp))
    {
        int size = p->size;
        if (ImGui::InputInt("Size", &size))
        {
            p->SetSize(size);
            changed = true;
        }
    }

    // --- SafeErosion ---
    else if (SafeErosion* p = dynamic_cast<SafeErosion*>(pp))
    {
        changed |= ImGui::InputInt("Size", &p->size);
    }

    // --- ExtractBlobs ---
    else if (ExtractBlobs* p = dynamic_cast<ExtractBlobs*>(pp))
    {
        int minSize = (int)p->minSize;
        int maxSize = (int)p->maxSize;
        changed |= ImGui::InputInt("Min Size", &minSize);
        changed |= ImGui::InputInt("Max Size", &maxSize);
        p->minSize = (unsigned int)std::max(0, minSize);
        p->maxSize = (unsigned int)std::max(0, maxSize);

        char ebOutput[INPUT_BUF_SIZE];
        snprintf(ebOutput, sizeof(ebOutput), "%s", p->outputFilename.c_str());
        if (ImGui::InputText("Output File", ebOutput, INPUT_BUF_SIZE))
            p->outputFilename = ebOutput;
        ImGui::SameLine();
        if (ImGui::Button("Browse##EB"))
        {
            OpenFileDialog("Output File", FileBrowser::SAVE,
                           {"CSV files", "*.csv", "All files", "*"},
                           p->outputFilename,
                           [p](const std::string& filename)
            {
                p->outputFilename = filename;
            });
        }
    }

    // --- FrameDifference ---
    else if (FrameDifference* p = dynamic_cast<FrameDifference*>(pp))
    {
        int threshold = p->threshold;
        if (ImGui::InputInt("Threshold", &threshold))
        {
            p->SetThreshold(threshold);
            changed = true;
        }

        bool usePipeline = p->usePipeline;
        if (ImGui::Checkbox("Use Pipeline", &usePipeline))
        {
            p->SetUsePipeline(usePipeline);
            changed = true;
        }

        changed |= ImGui::Checkbox("Additive", &p->additive);
        changed |= ImGui::Checkbox("Invert", &p->invert);
        changed |= ImGui::Checkbox("Restrict to Zone", &p->restrictToZone);
        if (p->restrictToZone)
            changed |= ImGui::InputInt("Zone", &p->zone);

        // Sync to all thread instances
        if (changed)
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
        {
            p->SetLength((unsigned int)std::max(1, length));
            changed = true;
        }

        int threshold = (int)p->threshold;
        if (ImGui::InputInt("Threshold", &threshold))
        {
            p->SetThreshold((unsigned int)std::max(0, threshold));
            changed = true;
        }

        if (ImGui::Button("Clear History"))
        {
            p->ClearHistory();
            for (unsigned int t = 1; t < ipEngine.threadsCount; t++)
                if (auto* tp = dynamic_cast<MovingAverage*>(ipEngine.pipelines[t].plugins[index]))
                    tp->ClearHistory();
            changed = true;
        }

        // Sync to all thread instances
        if (changed)
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
        {
            p->SetMaxEntities((unsigned int)std::max(1, maxEntities));
            changed = true;
        }

        changed |= ImGui::InputFloat("Min Interdistance", &p->minInterdistance, 1.0f, 10.0f, "%.1f");
        changed |= ImGui::InputFloat("Max Motion/sec", &p->maxMotionPerSecond, 10.0f, 100.0f, "%.1f");
        changed |= ImGui::InputFloat("Extrapolation Decay", &p->extrapolationDecay, 0.01f, 0.1f, "%.2f");

        int meLen = (int)p->motionEstimatorLength;
        if (ImGui::InputInt("Motion Estimator Length", &meLen))
        {
            p->motionEstimatorLength = (unsigned int)std::max(1, meLen);
            changed = true;
        }

        changed |= ImGui::InputFloat("Motion Estimator Timeout", &p->motionEstimatorTimeout, 0.1f, 1.0f, "%.2f");

        changed |= ImGui::Checkbox("Use Virtual Entities", &p->useVirtualEntities);
        if (p->useVirtualEntities)
        {
            changed |= ImGui::InputFloat("Virtual Lifetime", &p->virtualEntitiesLifetime, 0.1f, 1.0f, "%.2f");
            changed |= ImGui::InputFloat("Virtual Delay", &p->virtualEntitiesDelay, 0.1f, 1.0f, "%.2f");
            changed |= ImGui::Checkbox("Virtual Zone", &p->virtualEntitiesZone);
        }

        int trailLen = (int)p->trailLength;
        if (ImGui::InputInt("Trail Length", &trailLen))
        {
            p->trailLength = (unsigned int)std::max(0, trailLen);
            changed = true;
        }

        changed |= ImGui::Checkbox("Replay", &p->replay);

        char trackerOutput[INPUT_BUF_SIZE];
        snprintf(trackerOutput, sizeof(trackerOutput), "%s", p->outputFilename.c_str());
        if (ImGui::InputText("Output File", trackerOutput, INPUT_BUF_SIZE))
            p->outputFilename = trackerOutput;
        ImGui::SameLine();
        if (ImGui::Button("Browse##Tracker"))
        {
            OpenFileDialog("Output File", FileBrowser::SAVE,
                           {"CSV files", "*.csv", "All files", "*"},
                           p->outputFilename,
                           [p](const std::string& filename)
            {
                p->outputFilename = filename;
            });
        }

        if (ImGui::Button("Clear History"))
        {
            p->ClearHistory();
            changed = true;
        }
    }

    // --- RecordVideo ---
    else if (RecordVideo* p = dynamic_cast<RecordVideo*>(pp))
    {
        char rvOutput[INPUT_BUF_SIZE];
        snprintf(rvOutput, sizeof(rvOutput), "%s", p->outputFilename.c_str());
        if (ImGui::InputText("Output File", rvOutput, INPUT_BUF_SIZE))
            p->outputFilename = rvOutput;
        ImGui::SameLine();
        if (ImGui::Button("Browse##RV"))
        {
            OpenFileDialog("Output File", FileBrowser::SAVE,
                           {"Video files", "*.mp4 *.avi *.mkv", "All files", "*"},
                           p->outputFilename,
                           [p](const std::string& filename)
            {
                p->outputFilename = filename;
            });
        }

        char rvPreset[INPUT_BUF_SIZE];
        snprintf(rvPreset, sizeof(rvPreset), "%s", p->preset.c_str());
        if (ImGui::InputText("Preset", rvPreset, INPUT_BUF_SIZE))
            p->preset = rvPreset;

        ImGui::InputInt("Bitrate (kbps)", &p->bitrate);
    }

    // --- RecordPixels ---
    else if (RecordPixels* p = dynamic_cast<RecordPixels*>(pp))
    {
        char rpOutput[INPUT_BUF_SIZE];
        snprintf(rpOutput, sizeof(rpOutput), "%s", p->outputFilename.c_str());
        if (ImGui::InputText("Output File", rpOutput, INPUT_BUF_SIZE))
            p->outputFilename = rpOutput;
        ImGui::SameLine();
        if (ImGui::Button("Browse##RP"))
        {
            OpenFileDialog("Output File", FileBrowser::SAVE,
                           {"CSV files", "*.csv", "All files", "*"},
                           p->outputFilename,
                           [p](const std::string& filename)
            {
                p->outputFilename = filename;
            });
        }
    }

    // --- SimpleTags ---
    else if (SimpleTags* p = dynamic_cast<SimpleTags*>(pp))
    {
        int pwidth = p->pwidth;
        int pheight = p->pheight;
        if (ImGui::InputInt("Tag Width", &pwidth))
        {
            p->SetTagDimensions(pwidth, pheight);
            changed = true;
        }
        if (ImGui::InputInt("Tag Height", &pheight))
        {
            p->SetTagDimensions(pwidth, pheight);
            changed = true;
        }

        char stOutput[INPUT_BUF_SIZE];
        snprintf(stOutput, sizeof(stOutput), "%s", p->outputFilename.c_str());
        if (ImGui::InputText("Output File", stOutput, INPUT_BUF_SIZE))
            p->outputFilename = stOutput;
        ImGui::SameLine();
        if (ImGui::Button("Browse##ST"))
        {
            OpenFileDialog("Output File", FileBrowser::SAVE,
                           {"CSV files", "*.csv", "All files", "*"},
                           p->outputFilename,
                           [p](const std::string& filename)
            {
                p->outputFilename = filename;
            });
        }
    }

    // --- TakeSnapshots ---
    else if (TakeSnapshots* p = dynamic_cast<TakeSnapshots*>(pp))
    {
        char tsOutput[INPUT_BUF_SIZE];
        snprintf(tsOutput, sizeof(tsOutput), "%s", p->outputFilename.c_str());
        if (ImGui::InputText("Output Dir/Pattern", tsOutput, INPUT_BUF_SIZE))
            p->outputFilename = tsOutput;
        ImGui::SameLine();
        if (ImGui::Button("Browse##TS"))
        {
            OpenFileDialog("Select Output Directory", FileBrowser::FOLDER,
                           {}, "",
                           [p](const std::string& folder)
            {
                p->outputFilename = folder;
            });
        }
    }

    // --- Stopwatch ---
    else if (Stopwatch* p = dynamic_cast<Stopwatch*>(pp))
    {
        ImGui::Text("Stopwatch plugin");
        ImGui::Text("Shortcuts: %d", (int)p->shortcuts.size());
        ImGui::Text("Events: %d", (int)p->events.size());

        char swOutput[INPUT_BUF_SIZE];
        snprintf(swOutput, sizeof(swOutput), "%s", p->outputFilename.c_str());
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
        ImGui::TextWrapped("Define regions of interest as polygons drawn on the "
                           "video, and/or from a zone-mask image. Region 0 and "
                           "un-painted areas are ignored by downstream plugins.");
        ImGui::Spacing();

        bool selected = (selectedPipelineItem == index);
        if (selected)
            ImGui::TextColored(ImVec4(0.5f, 1.0f, 0.5f, 1.0f),
                               "Editing active on the video:");
        else
            ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.2f, 1.0f),
                               "Select this plugin in the pipeline to edit on the video.");
        ImGui::BulletText("Click to add points, drag to move");
        ImGui::BulletText("Click the first point (or right-click) to close a polygon");
        ImGui::BulletText("Right-click a point to delete it");
        ImGui::TextDisabled("New polygons default to region 1; change the "
                            "region of a selected polygon below.");

        ImGui::Separator();

        // polygon list
        ImGui::BeginChild("RoiList", ImVec2(0, 90 * dpiScale), true);
        for (int i = 0; i < (int)p->polygons.size(); i++)
        {
            char lbl[64];
            snprintf(lbl, sizeof(lbl), "Polygon %d: region %d, %d points", i,
                     p->polygons[i].region, (int)p->polygons[i].points.size());
            if (ImGui::Selectable(lbl, roiSelectedPolygon == i))
                roiSelectedPolygon = i;
        }
        ImGui::EndChild();

        if (roiSelectedPolygon >= 0 && roiSelectedPolygon < (int)p->polygons.size())
        {
            int reg = p->polygons[roiSelectedPolygon].region;
            ImGui::SetNextItemWidth(120 * dpiScale);
            if (ImGui::InputInt("Selected polygon region", &reg))
            {
                p->polygons[roiSelectedPolygon].region = std::max(0, reg);
                p->Rasterize();
                pipelineDirty = true;
            }
        }

        if (ImGui::Button("Delete Selected"))
        {
            if (roiSelectedPolygon >= 0 && roiSelectedPolygon < (int)p->polygons.size())
            {
                p->polygons.erase(p->polygons.begin() + roiSelectedPolygon);
                roiSelectedPolygon = -1;
                roiActivePolygon = -1;
                p->Rasterize();
                pipelineDirty = true;
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Clear All"))
        {
            p->polygons.clear();
            roiSelectedPolygon = -1;
            roiActivePolygon = -1;
            p->Rasterize();
            pipelineDirty = true;
        }

        if (ImGui::Button("Save ROIs..."))
        {
            OpenFileDialog("Save ROIs", FileBrowser::SAVE,
                           {"Text files", "*.txt", "All files", "*"},
                           p->roiFilename.empty() ? "rois.txt" : p->roiFilename,
                           [p](const std::string& filename) { p->SaveRois(filename); });
        }
        ImGui::SameLine();
        if (ImGui::Button("Load ROIs..."))
        {
            OpenFileDialog("Load ROIs", FileBrowser::OPEN,
                           {"Text files", "*.txt", "All files", "*"}, "",
                           [p, this](const std::string& filename)
            {
                if (p->LoadRois(filename)) { p->Rasterize(); pipelineDirty = true; }
                else errorMessage = "Could not load ROI file:\n" + filename;
            });
        }

        ImGui::Separator();

        // optional zone-mask image
        char zi[INPUT_BUF_SIZE];
        snprintf(zi, sizeof(zi), "%s", p->zonesImageFilename.c_str());
        ImGui::SetNextItemWidth(220 * dpiScale);
        if (ImGui::InputText("Zone image", zi, INPUT_BUF_SIZE))
            p->zonesImageFilename = zi;
        ImGui::SameLine();
        if (ImGui::Button("Browse##ZoneImg"))
        {
            OpenFileDialog("Load Zone Image", FileBrowser::OPEN,
                           {"Image files", "*.png *.jpg *.bmp *.tif", "All files", "*"},
                           "",
                           [p, this](const std::string& filename)
            {
                p->LoadImage(filename);
                p->Rasterize();
                pipelineDirty = true;
            });
        }
    }

    // --- RemoteControl ---
    else if (RemoteControl* p = dynamic_cast<RemoteControl*>(pp))
    {
        ImGui::Text("Remote Control (Bluetooth)");
        ImGui::Text("Address: %s", p->btAddress.c_str());
        changed |= ImGui::InputInt("Corner", &p->corner);
    }

#ifdef ARUCO
    // --- Aruco ---
    else if (Aruco* p = dynamic_cast<Aruco*>(pp))
    {
        float minSz = (float)p->minSize;
        float maxSz = (float)p->maxSize;
        if (ImGui::InputFloat("Min Size", &minSz, 0.001f, 0.01f, "%.4f"))
        {
            p->SetMinSize(minSz);
            changed = true;
        }
        if (ImGui::InputFloat("Max Size", &maxSz, 0.001f, 0.01f, "%.4f"))
        {
            p->SetMaxSize(maxSz);
            changed = true;
        }

        int t1 = p->thresh1;
        int t2 = p->thresh2;
        if (ImGui::InputInt("Threshold 1", &t1))
        {
            p->SetThreshold1(t1);
            changed = true;
        }
        if (ImGui::InputInt("Threshold 2", &t2))
        {
            p->SetThreshold2(t2);
            changed = true;
        }

        int maskShape = p->maskShape;
        const char* maskShapes[] = { "None", "Square", "Disc" };
        if (ImGui::Combo("Mask Shape", &maskShape, maskShapes, 3))
        {
            p->SetMaskShape(maskShape);
            changed = true;
        }

        int maskRadius = p->maskRadius;
        if (ImGui::InputInt("Mask Radius", &maskRadius))
        {
            p->SetMaskRadius(maskRadius);
            changed = true;
        }

        int maskPerspShift = p->maskPerspectiveShift;
        if (ImGui::InputInt("Mask Persp. Shift", &maskPerspShift))
        {
            p->SetMaskPerspectiveShift(maskPerspShift);
            changed = true;
        }

        int maskVal = p->maskValue;
        if (ImGui::InputInt("Mask Value", &maskVal))
        {
            p->SetMaskValue(maskVal);
            changed = true;
        }

        char arucoOutput[INPUT_BUF_SIZE];
        snprintf(arucoOutput, sizeof(arucoOutput), "%s", p->outputFilename.c_str());
        if (ImGui::InputText("Output File", arucoOutput, INPUT_BUF_SIZE))
            p->outputFilename = arucoOutput;
        ImGui::SameLine();
        if (ImGui::Button("Browse##Aruco"))
        {
            OpenFileDialog("Output File", FileBrowser::SAVE,
                           {"CSV files", "*.csv", "All files", "*"},
                           p->outputFilename,
                           [p](const std::string& filename)
            {
                p->outputFilename = filename;
            });
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
        {
            p->SetMarkerCols(markerCols);
            changed = true;
        }
        if (ImGui::InputInt("Marker Rows", &markerRows))
        {
            p->SetMarkerRows(markerRows);
            changed = true;
        }
        if (ImGui::InputInt("Saturation Threshold", &satThresh))
        {
            p->SetSaturationThreshold(satThresh);
            changed = true;
        }
        if (ImGui::InputInt("Value Threshold", &valThresh))
        {
            p->SetValueThreshold(valThresh);
            changed = true;
        }
        if (ImGui::InputInt("AT Block Size", &atBlockSize))
        {
            p->SetAdaptiveThresholdBlockSize(atBlockSize);
            changed = true;
        }
        if (ImGui::InputInt("AT Constant", &atConstant))
        {
            p->SetAdaptiveThresholdConstant(atConstant);
            changed = true;
        }
        if (ImGui::InputInt("Min Marker Area", &minArea))
        {
            p->SetMinMarkerArea(minArea);
            changed = true;
        }
        if (ImGui::InputInt("Max Marker Area", &maxArea))
        {
            p->SetMaxMarkerArea(maxArea);
            changed = true;
        }
        if (ImGui::InputInt("Max Hue Deviation", &maxHueDev))
        {
            p->SetMaxHueDeviation(maxHueDev);
            changed = true;
        }
        if (ImGui::InputInt("Max Marker Range", &maxRange))
        {
            p->SetMaxMarkerRange(maxRange);
            changed = true;
        }

        char acDict[INPUT_BUF_SIZE];
        snprintf(acDict, sizeof(acDict), "%s", p->GetDictionaryString().c_str());
        if (ImGui::InputText("Dictionary", acDict, INPUT_BUF_SIZE))
        {
            p->SetDictionaryString(std::string(acDict));
            changed = true;
        }

        char acHues[INPUT_BUF_SIZE];
        snprintf(acHues, sizeof(acHues), "%s", p->GetReferenceHuesString().c_str());
        if (ImGui::InputText("Reference Hues", acHues, INPUT_BUF_SIZE))
        {
            p->SetReferenceHuesString(std::string(acHues));
            changed = true;
        }

        char acOutput[INPUT_BUF_SIZE];
        snprintf(acOutput, sizeof(acOutput), "%s", p->outputFilename.c_str());
        if (ImGui::InputText("Output File", acOutput, INPUT_BUF_SIZE))
            p->outputFilename = acOutput;
        ImGui::SameLine();
        if (ImGui::Button("Browse##AC"))
        {
            OpenFileDialog("Output File", FileBrowser::SAVE,
                           {"CSV files", "*.csv", "All files", "*"},
                           p->outputFilename,
                           [p](const std::string& filename)
            {
                p->outputFilename = filename;
            });
        }
    }

    // --- Unknown plugin type fallback ---
    else
    {
        ImGui::Text("No specific UI for this plugin type.");
    }

    // any parameter change must be reflected immediately in the video view
    if (changed)
        pipelineDirty = true;

    ImGui::End();
}

// ============================================================================
// OpenSource
// ============================================================================

void AppGui::OpenSource()
{
    play = false;
    if (ipEngine.capture)
        ipEngine.capture->Pause();

    OpenFileDialog("Open Source", FileBrowser::OPEN,
                   {"Video files", "*.avi *.mp4 *.mkv *.mov *.mpg *.mpeg *.wmv *.flv",
                    "Image files", "*.png *.jpg *.jpeg *.bmp *.tif *.tiff",
                    "All files", "*"}, "",
                   [this](const std::string& filename)
    {
        OpenSourceFile(filename);
    });
}

void AppGui::OpenSourceFile(const std::string& filename)
{
    // Determine type and create capture
    Capture* newCapture = nullptr;

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

        // video failed ? try as image
        if (newCapture->type == Capture::NONE)
        {
            delete newCapture;
            newCapture = new CaptureImage(filename);
        }
    }

    // keep the current source untouched if the new one could not be opened
    if (!newCapture || newCapture->type == Capture::NONE)
    {
        delete newCapture;
        errorMessage = "Could not open source:\n" + filename;
        return;
    }

    parameters.inputFilename = filename;
    ChangeCapture(newCapture);
}

// ============================================================================
// ChangeCapture - swap the capture source, preserving the current pipeline
// ============================================================================

void AppGui::ChangeCapture(Capture* newCapture)
{
    // detach the plugins: the engine reset destroys and recreates the
    // pipelines, and we do not want to lose the pipeline built by the user
    std::vector<std::vector<PipelinePlugin*>> savedPlugins;
    if (!ipEngine.pipelines.empty())
        while (!ipEngine.pipelines[0].plugins.empty())
            savedPlugins.push_back(ipEngine.Erase(0));

    delete ipEngine.capture;
    ipEngine.capture = newCapture;

    std::string title = "USE Tracker: " + ipEngine.capture->GetName();
    SDL_SetWindowTitle(window, title.c_str());

    ipEngine.Reset(parameters);

    // reattach the plugins, Reset adapts them to the new frame dimensions
    for (auto& pfv : savedPlugins)
        ipEngine.PushBack(pfv, true);

    hud.create(ipEngine.capture->height, ipEngine.capture->width, CV_8UC4);
    hudApp.create(ipEngine.capture->height, ipEngine.capture->width, CV_8UC4);
    ipEngine.hud = hud;
    ipEngine.takeSnapshot = true;

    play = false;
    pipelineDirty = true;
}

// ============================================================================
// SaveSource
// ============================================================================

void AppGui::SaveSource()
{
    // Save source is typically used for multi-video stitching configs
    OpenFileDialog("Save Source Configuration", FileBrowser::SAVE,
                   {"XML files", "*.xml", "All files", "*"}, "source.xml",
                   [this](const std::string& filename)
    {
        cv::FileStorage fs(filename, cv::FileStorage::WRITE);
        if (fs.isOpened())
        {
            fs << "Source" << "{";
            ipEngine.capture->SaveXML(fs);
            fs << "}";
            fs.release();
        }
        else
        {
            errorMessage = "Could not write file:\n" + filename;
        }
    });
}

// ============================================================================
// LoadSettings
// ============================================================================

void AppGui::LoadSettings()
{
    OpenFileDialog("Load Settings", FileBrowser::OPEN,
                   {"XML files", "*.xml", "All files", "*"}, "",
                   [this](const std::string& filename)
    {
        parameters.parametersFilename = filename;
        parameters.loadXML(filename);

        ResetEngine(parameters);
        pipelineDirty = true;
    });
}

// ============================================================================
// SaveSettings
// ============================================================================

void AppGui::SaveSettings()
{
    OpenFileDialog("Save Settings", FileBrowser::SAVE,
                   {"XML files", "*.xml", "All files", "*"}, "settings.xml",
                   [this](const std::string& filename)
    {
        DoSaveSettings(filename);
    });
}

void AppGui::DoSaveSettings(const std::string& result)
{
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
// OpenFileDialog - native dialog when available, in-app fallback otherwise
// ============================================================================

void AppGui::OpenFileDialog(const std::string& title, FileBrowser::Mode mode,
                            const std::vector<std::string>& filters,
                            const std::string& defaultName,
                            std::function<void(const std::string&)> onSelect)
{
    // use the native dialogs if a backend (zenity, kdialog, ...) is installed
    if (pfd::settings::available())
    {
        if (mode == FileBrowser::OPEN)
        {
            auto result = pfd::open_file(title, ".", filters).result();
            if (!result.empty())
                onSelect(result[0]);
        }
        else if (mode == FileBrowser::SAVE)
        {
            auto result = pfd::save_file(title, defaultName, filters).result();
            if (!result.empty())
                onSelect(result);
        }
        else
        {
            auto result = pfd::select_folder(title, ".").result();
            if (!result.empty())
                onSelect(result);
        }
        return;
    }

    // no backend available : use the in-app fallback browser
    fileBrowser.visible = true;
    fileBrowser.mode = mode;
    fileBrowser.title = title;
    fileBrowser.onSelect = onSelect;
    fileBrowser.error.clear();

    // extract the allowed extensions from the pfd-style filters
    // (pairs of "label", "*.ext1 *.ext2" strings)
    fileBrowser.extensions.clear();
    for (size_t i = 1; i < filters.size(); i += 2)
    {
        std::istringstream ss(filters[i]);
        std::string pattern;
        while (ss >> pattern)
        {
            if (pattern.rfind("*.", 0) == 0 && pattern != "*.*")
            {
                std::string ext = pattern.substr(1); // keep the dot
                std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
                fileBrowser.extensions.push_back(ext);
            }
        }
    }

    // start in the directory of the current source, else keep the last one
    if (fileBrowser.dir.empty())
    {
        std::error_code ec;
        if (!parameters.inputFilename.empty())
            fileBrowser.dir = std::filesystem::absolute(
                std::filesystem::path(parameters.inputFilename), ec).parent_path();
        if (fileBrowser.dir.empty() || !std::filesystem::is_directory(fileBrowser.dir, ec))
            fileBrowser.dir = std::filesystem::current_path(ec);
    }

    snprintf(fileBrowser.nameBuf, sizeof(fileBrowser.nameBuf), "%s",
             std::filesystem::path(defaultName).filename().string().c_str());

    RefreshFileBrowser();
}

void AppGui::RefreshFileBrowser()
{
    fileBrowser.entries.clear();
    fileBrowser.error.clear();

    try
    {
        for (auto& entry : std::filesystem::directory_iterator(fileBrowser.dir))
        {
            std::string name = entry.path().filename().string();
            bool isDir = entry.is_directory();

            if (!isDir && !fileBrowser.extensions.empty())
            {
                std::string ext = entry.path().extension().string();
                std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
                if (std::find(fileBrowser.extensions.begin(),
                              fileBrowser.extensions.end(), ext)
                    == fileBrowser.extensions.end())
                    continue;
            }

            fileBrowser.entries.push_back({name, isDir});
        }
    }
    catch (const std::exception& ex)
    {
        fileBrowser.error = ex.what();
    }

    std::sort(fileBrowser.entries.begin(), fileBrowser.entries.end(),
              [](const std::pair<std::string, bool>& a,
                 const std::pair<std::string, bool>& b)
    {
        if (a.second != b.second) return a.second; // directories first
        return a.first < b.first;
    });

    // default keyboard selection: the file matching nameBuf, else the first row
    fileBrowser.selected = fileBrowser.entries.empty() ? -1 : 0;
    for (int i = 0; i < (int)fileBrowser.entries.size(); i++)
        if (!fileBrowser.entries[i].second &&
            fileBrowser.entries[i].first == fileBrowser.nameBuf)
        { fileBrowser.selected = i; break; }
    fileBrowser.scrollToSelected = true;
}

void AppGui::DrawFileBrowser()
{
    if (!fileBrowser.visible) return;

    std::string popupId = fileBrowser.title + "###FileBrowser";

    if (!ImGui::IsPopupOpen(popupId.c_str()))
        ImGui::OpenPopup(popupId.c_str());

    ImGui::SetNextWindowSize(ImVec2(560*dpiScale, 480*dpiScale), ImGuiCond_Appearing);
    bool open = true;
    if (!ImGui::BeginPopupModal(popupId.c_str(), &open))
    {
        if (!open) fileBrowser.visible = false;
        return;
    }

    // Escape cancels the browser (and must not fall through to quitting the app)
    if (ImGui::IsKeyPressed(ImGuiKey_Escape))
    {
        fileBrowser.visible = false;
        fileBrowser.onSelect = nullptr;
        ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
        return;
    }

    bool confirm = false;
    int n = (int)fileBrowser.entries.size();

    // ---- keyboard handling (manual: ImGui nav stays off so the video
    //      shortcuts keep working). Skipped while typing in the name field. ----
    bool typing = ImGui::GetIO().WantTextInput;
    if (!typing && n > 0)
    {
        bool moved = false;
        if (ImGui::IsKeyPressed(ImGuiKey_DownArrow))
        {
            fileBrowser.selected = (fileBrowser.selected < 0)
                                 ? 0 : std::min(fileBrowser.selected + 1, n - 1);
            moved = true;
        }
        if (ImGui::IsKeyPressed(ImGuiKey_UpArrow))
        {
            fileBrowser.selected = (fileBrowser.selected < 0)
                                 ? 0 : std::max(fileBrowser.selected - 1, 0);
            moved = true;
        }
        // on an actual move, scroll to it and reflect a file into the name field
        if (moved)
        {
            fileBrowser.scrollToSelected = true;
            if (fileBrowser.selected >= 0 && fileBrowser.selected < n &&
                !fileBrowser.entries[fileBrowser.selected].second)
                snprintf(fileBrowser.nameBuf, sizeof(fileBrowser.nameBuf), "%s",
                         fileBrowser.entries[fileBrowser.selected].first.c_str());
        }

        // Enter: open the highlighted folder, or confirm the highlighted file
        if (ImGui::IsKeyPressed(ImGuiKey_Enter) || ImGui::IsKeyPressed(ImGuiKey_KeypadEnter))
        {
            if (fileBrowser.selected >= 0 && fileBrowser.selected < n)
            {
                if (fileBrowser.entries[fileBrowser.selected].second)
                {
                    fileBrowser.dir /= fileBrowser.entries[fileBrowser.selected].first;
                    RefreshFileBrowser();
                }
                else
                    confirm = true;
            }
            else if (fileBrowser.nameBuf[0] != '\0')
                confirm = true;
        }

        // Tab: jump to the file name field (where Enter also confirms)
        if (ImGui::IsKeyPressed(ImGuiKey_Tab) && fileBrowser.mode != FileBrowser::FOLDER)
            fileBrowser.focusName = true;
    }

    // current directory + navigation
    if (ImGui::Button("Up"))
    {
        std::filesystem::path parent = fileBrowser.dir.parent_path();
        if (!parent.empty() && parent != fileBrowser.dir)
        {
            fileBrowser.dir = parent;
            RefreshFileBrowser();
        }
    }
    ImGui::SameLine();
    ImGui::TextWrapped("%s", fileBrowser.dir.string().c_str());

    if (!fileBrowser.error.empty())
        ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.4f, 1.0f), "%s", fileBrowser.error.c_str());
    else
        ImGui::TextDisabled("Up/Down: select   Enter: open/pick   Tab: name field");

    // entries list
    float footerHeight = ImGui::GetFrameHeightWithSpacing() *
                         (fileBrowser.mode == FileBrowser::FOLDER ? 1.4f : 2.6f);
    ImGui::BeginChild("##FileList", ImVec2(0, -footerHeight), true);
    for (int i = 0; i < (int)fileBrowser.entries.size(); i++)
    {
        const std::string& name = fileBrowser.entries[i].first;
        bool isDir = fileBrowser.entries[i].second;

        std::string label = isDir ? name + "/" : name;

        ImGui::PushID(i);
        if (ImGui::Selectable(label.c_str(), fileBrowser.selected == i,
                              ImGuiSelectableFlags_AllowDoubleClick))
        {
            fileBrowser.selected = i;
            if (!isDir)
                snprintf(fileBrowser.nameBuf, sizeof(fileBrowser.nameBuf), "%s", name.c_str());

            if (ImGui::IsMouseDoubleClicked(0))
            {
                if (isDir)
                {
                    fileBrowser.dir /= name;
                    RefreshFileBrowser();
                    ImGui::PopID();
                    break; // entries were rebuilt, stop iterating
                }
                else
                    confirm = true;
            }
        }

        // auto-scroll to keep the keyboard selection visible
        if (fileBrowser.scrollToSelected && fileBrowser.selected == i)
            ImGui::SetScrollHereY(0.5f);

        ImGui::PopID();
    }
    ImGui::EndChild();
    fileBrowser.scrollToSelected = false;

    // file name input
    if (fileBrowser.mode != FileBrowser::FOLDER)
    {
        if (fileBrowser.focusName)
        {
            ImGui::SetKeyboardFocusHere();
            fileBrowser.focusName = false;
        }
        ImGui::SetNextItemWidth(-1);
        if (ImGui::InputText("##FileName", fileBrowser.nameBuf,
                             sizeof(fileBrowser.nameBuf),
                             ImGuiInputTextFlags_EnterReturnsTrue))
            confirm = true;
    }

    const char* okLabel = (fileBrowser.mode == FileBrowser::SAVE) ? "Save"
                        : (fileBrowser.mode == FileBrowser::FOLDER) ? "Select This Folder"
                        : "Open";
    if (ImGui::Button(okLabel, ImVec2(160*dpiScale, 0)))
        confirm = true;
    ImGui::SameLine();
    if (ImGui::Button("Cancel", ImVec2(100*dpiScale, 0)))
    {
        fileBrowser.visible = false;
        ImGui::CloseCurrentPopup();
        confirm = false;
    }

    if (confirm)
    {
        std::string result;
        if (fileBrowser.mode == FileBrowser::FOLDER)
            result = fileBrowser.dir.string();
        else if (fileBrowser.nameBuf[0] != '\0')
            result = (fileBrowser.dir / fileBrowser.nameBuf).string();

        if (!result.empty())
        {
            fileBrowser.visible = false;
            ImGui::CloseCurrentPopup();

            // copy the callback: it may open another dialog
            auto callback = fileBrowser.onSelect;
            fileBrowser.onSelect = nullptr;
            if (callback)
                callback(result);
        }
    }

    ImGui::EndPopup();

    if (!open)
        fileBrowser.visible = false;
}

// ============================================================================
// DrawErrorPopup
// ============================================================================

void AppGui::RequestQuit()
{
    if (showQuitConfirm) return;
    showQuitConfirm = true;
    quitConfirmJustOpened = true;
}

void AppGui::DrawQuitConfirm()
{
    if (!showQuitConfirm) return;

    if (!ImGui::IsPopupOpen("Quit###QuitConfirm"))
        ImGui::OpenPopup("Quit###QuitConfirm");

    // centre the modal
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    if (ImGui::BeginPopupModal("Quit###QuitConfirm", nullptr,
                               ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Are you sure you want to leave?");
        ImGui::TextDisabled("Any unsaved changes will be lost.");
        ImGui::Spacing();

        if (ImGui::Button("Quit", ImVec2(120 * dpiScale, 0)))
        {
            running = false;
            showQuitConfirm = false;
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120 * dpiScale, 0)))
        {
            showQuitConfirm = false;
            ImGui::CloseCurrentPopup();
        }

        // Escape cancels — but not on the frame the modal opened (that same
        // Escape press is what asked to quit)
        if (!quitConfirmJustOpened && ImGui::IsKeyPressed(ImGuiKey_Escape))
        {
            showQuitConfirm = false;
            ImGui::CloseCurrentPopup();
        }
        quitConfirmJustOpened = false;

        ImGui::EndPopup();
    }
}

void AppGui::DrawErrorPopup()
{
    if (!errorMessage.empty() && !ImGui::IsPopupOpen("Error###ErrorPopup"))
        ImGui::OpenPopup("Error###ErrorPopup");

    if (ImGui::BeginPopupModal("Error###ErrorPopup", nullptr,
                               ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::TextWrapped("%s", errorMessage.c_str());
        ImGui::Spacing();
        if (ImGui::Button("OK", ImVec2(120*dpiScale, 0)))
        {
            errorMessage.clear();
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
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

    pipelineDirty = true;

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
