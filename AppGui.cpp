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
#include "ui/Theme.h"

#include <SDL.h>
#include <GL/gl.h>

#include "imgui.h"
#include "imgui_internal.h" // custom persistence for workspace size and splitters
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
#include <fstream>
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
#include "plugins/Denoise.h"
#include "plugins/TemporalDenoise.h"
#include "plugins/Sharpen.h"
#include "plugins/Dehaze.h"
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
    const char* driver = getenv("SDL_VIDEODRIVER");
    if (!display && !wayland && (!driver || std::strcmp(driver, "offscreen") != 0))
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
        windowWidth = std::max(960, std::min(1600, (int)(dm.w * 0.8f)));
        windowHeight = std::max(640, std::min(1000, (int)(dm.h * 0.8f)));
    }

    Uint32 windowFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI;
    // SDL's offscreen surface cannot grow after creation. Allocate enough for
    // every scripted size so framebuffer screenshots also exercise large layouts.
    if (driver && std::strcmp(driver, "offscreen")==0 && !parameters.testScript.empty())
    {
        std::ifstream script(parameters.testScript);
        std::string line;
        while (std::getline(script,line))
        {
            std::istringstream command(line);
            std::string op; int w=0,h=0;
            if (command >> op >> w >> h && op=="resize")
            { windowWidth=std::max(windowWidth,w); windowHeight=std::max(windowHeight,h); }
        }
    }
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

    SDL_SetWindowMinimumSize(window, 960, 640);
    SDL_EventState(SDL_DROPFILE, SDL_ENABLE);
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

    // Keep UI tests deterministic and separate from the user's saved workspace.
    if (!parameters.testScript.empty()) ImGui::GetIO().IniFilename = nullptr;
    ImGuiSettingsHandler settings;
    settings.TypeName = "useTracker";
    settings.TypeHash = ImHashStr(settings.TypeName);
    settings.UserData = this;
    settings.ReadOpenFn = [](ImGuiContext*, ImGuiSettingsHandler* h, const char* name) -> void* {
        // Preserve divider and scale preferences saved by the first redesign.
        return std::strcmp(name,"Workspace")==0 || std::strcmp(name,"Darkroom")==0 ? h->UserData : nullptr;
    };
    settings.ReadLineFn = [](ImGuiContext*, ImGuiSettingsHandler*, void* entry, const char* line) {
        auto* app=static_cast<AppGui*>(entry);
        float value=0;
        if (sscanf(line,"Scale=%f",&value)==1 && std::isfinite(value)) app->dpiScale=std::clamp(value,.75f,2.f);
        if (sscanf(line,"Panel=%f",&value)==1 && std::isfinite(value)) app->controlPanelWidth=std::clamp(value,280.f,900.f);
        if (sscanf(line,"Pipeline=%f",&value)==1 && std::isfinite(value)) app->pipelineListHeight=std::clamp(value,90.f,900.f);
        if (std::strcmp(line,"Theme=light")==0) app->darkMode=false;
        if (std::strcmp(line,"Theme=dark")==0) app->darkMode=true;
    };
    settings.WriteAllFn = [](ImGuiContext*, ImGuiSettingsHandler* h, ImGuiTextBuffer* out) {
        auto* app=static_cast<AppGui*>(h->UserData);
        out->appendf("[useTracker][Workspace]\nTheme=%s\nScale=%.2f\nPanel=%.1f\nPipeline=%.1f\n\n",
                     app->darkMode ? "dark" : "light",app->dpiScale,app->controlPanelWidth,app->pipelineListHeight);
    };
    ImGui::AddSettingsHandler(&settings);

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
    if (ImGui::GetIO().IniFilename) ImGui::LoadIniSettingsFromDisk(ImGui::GetIO().IniFilename);

    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init("#version 150");

    ApplyUIScale();

    return true;
}

// ============================================================================
// ApplyUIScale — rebuild font and style for current dpiScale
// ============================================================================

float AppGui::MaxUIScale() const
{
    int w=0,h=0;
    SDL_GetWindowSize(window,&w,&h);
    return std::clamp(std::min(w/960.f,h/640.f),.75f,2.f);
}

void AppGui::ApplyUIScale()
{
    dpiScale=std::clamp(dpiScale,.75f,MaxUIScale());
    TrackerUI::Apply(dpiScale, darkMode ? TrackerUI::Mode::Dark : TrackerUI::Mode::Light);
    ImGui_ImplOpenGL3_DestroyFontsTexture();
    ImGui_ImplOpenGL3_CreateFontsTexture();
}

void AppGui::SetDarkMode(bool enabled)
{
    if (darkMode==enabled) return;
    darkMode=enabled;
    pendingThemeChange=true;
    ImGui::MarkIniSettingsDirty();
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
    if (parameters.inputScale > 0.0f && parameters.inputScale <= 1.0f)
        ipEngine.inputScale = parameters.inputScale;
    ResetEngine(parameters);

    // Test harness: load the input script if one was given
    if (!parameters.testScript.empty())
        LoadTestScript(parameters.testScript);

    // Main loop
    while (running)
    {
        // scripted input for automated GUI tests
        TestAdvance();

        auto handleEvent = [&](SDL_Event& event)
        {
            ImGui_ImplSDL2_ProcessEvent(&event);

            if (event.type == SDL_QUIT)
                RequestQuit();

            if (event.type == SDL_DROPFILE)
            {
                if (!fileBrowser.visible && !showQuitConfirm &&
                    !ImGui::IsPopupOpen(nullptr, ImGuiPopupFlags_AnyPopupId | ImGuiPopupFlags_AnyPopupLevel))
                    OpenSourceFile(event.drop.file);
                SDL_free(event.drop.file);
            }

            if (event.type == SDL_WINDOWEVENT &&
                event.window.event == SDL_WINDOWEVENT_CLOSE &&
                event.window.windowID == SDL_GetWindowID(window))
                RequestQuit();

            // Keyboard shortcuts — active unless the user is typing in a text
            // field or navigating the in-app file browser (which uses the
            // arrow / Enter / Esc keys for its own navigation)
            if (event.type == SDL_KEYDOWN && !ImGui::GetIO().WantTextInput &&
                !fileBrowser.visible && !showQuitConfirm && !ImGui::IsAnyItemActive() &&
                !ImGui::IsPopupOpen(nullptr, ImGuiPopupFlags_AnyPopupId | ImGuiPopupFlags_AnyPopupLevel))
            {
                bool ctrl = (event.key.keysym.mod & KMOD_CTRL) != 0;
                HandleShortcut(event.key.keysym.sym, ctrl);
            }
        };

        // Idle throttling: when nothing is animating (paused, no pending work,
        // not scripted) block until an event arrives instead of busy-spinning,
        // which keeps CPU near zero while paused. Otherwise poll and run full
        // speed (vsync-capped).
        bool busy = play || pendingRewind || ipEngine.refilling ||
                    pipelineDirty || videoDirty || pendingThemeChange || pendingScaleChange || testMode ||
                    ImGui::GetIO().WantTextInput || activeTab == TAB_CALIBRATION;

        SDL_Event event;
        if (!busy)
        {
            // wake at least every 200 ms so ImGui animations still tick
            if (SDL_WaitEventTimeout(&event, 200))
                handleEvent(event);
        }
        while (SDL_PollEvent(&event))
            handleEvent(event);

        UpdateEngine();
        RenderFrame();

        // capture a scripted screenshot from the rendered back buffer
        if (!testShotPath.empty())
        {
            CaptureScreenshot(testShotPath);
            testShotPath.clear();
        }

        SDL_GL_SwapWindow(window);
    }

    ipEngine.CloseOutput();

    return testFailed ? 1 : 0;
}

// ============================================================================
// HandleShortcut - Playback and application keyboard shortcuts
// ============================================================================

void AppGui::HandleShortcut(SDL_Keycode key, bool ctrl)
{
    if (!HasSource() && !(ctrl && (key==SDLK_o || key==SDLK_l || key==SDLK_s)) && key!=SDLK_ESCAPE)
        return;
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
        // Pause and step backward one frame (cached, or a deferred chunk re-read)
        RequestStepBackward();
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
        ipEngine.RefreshCurrentFrame();
        play = false;
        output = false;
        ipEngine.CloseOutput();
        pendingRewind = false;
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

    // Deferred backward chunk re-read: the first pass just shows the loading
    // indicator (drawn this frame), the second pass does the actual (blocking)
    // refill, so the user sees feedback rather than a silent freeze.
    if (pendingRewind)
    {
        // Progressive backward chunk re-read: kick it off, then decode a small
        // batch each frame so the loading bar shows real progress. When the
        // chunk is ready the process head lands on the previous frame.
        if (!ipEngine.refilling)
        {
            long tgt = ipEngine.GetProcessFrameNumber() - 1;
            if (tgt < 0) { pendingRewind = false; return; }
            ipEngine.BeginRefillBackward(tgt);
        }
        bool done = ipEngine.PumpRefill(6);
        rewindLoadingTicks++;
        if (done)
        {
            pendingRewind = false;
            rewindLoadingTicks = 0;
            pipelineDirty = true;
        }
        return;   // don't run normal playback/step while rebuilding the buffer
    }

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
                    // loop playback: jump back to the loop start once the
                    // playhead reaches the loop end
                    if (loopEnabled && loopStart >= 0 && loopEnd > loopStart &&
                        ipEngine.GetPresentTime() >= loopEnd)
                    {
                        ipEngine.SeekTime(loopStart);
                    }
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
            videoDirty = true;   // the processed frame changed -> redraw texture
        }
    }
    else if (activeTab == TAB_CALIBRATION)
    {
        ipEngine.capture->Calibrate();
        videoDirty = true;       // calibration view updates continuously
    }

    // Determine which frame to display
    if (activeTab == TAB_PROCESSING)
        oglScreen = ipEngine.GetPresentImage();   // process head, or delayed for centered plugins
    else if (activeTab == TAB_BACKGROUND)
        oglScreen = ipEngine.background;
    else if (activeTab == TAB_CALIBRATION)
        oglScreen = ipEngine.capture->CalibrationGetFrame();
    else if (activeTab == TAB_PROCFRAME)
        oglScreen = ipEngine.zoneMap;

    // switching the displayed view changes the source image
    static int lastVideoTab = -1;
    if (activeTab != lastVideoTab) { videoDirty = true; lastVideoTab = activeTab; }

    // Update the video slider position
    if (!sliderMoving && ipEngine.capture->GetFrameCount() > 0)
    {
        videoSliderPos = (float)ipEngine.GetPresentFrameNumber() /
                         (float)(ipEngine.capture->GetFrameCount() + 1);
    }
}

// ============================================================================
// RenderFrame
// ============================================================================

void AppGui::RenderFrame()
{
    if (dpiScale>MaxUIScale()) pendingScaleChange=true;
    if (pendingScaleChange)
    {
        ApplyUIScale();
        pendingScaleChange = false;
    }
    if (pendingThemeChange)
    {
        // Apply between frames so open dialogs and custom widgets change together.
        // A palette switch leaves fonts, playback and pipeline state intact.
        TrackerUI::ApplyColors(darkMode ? TrackerUI::Mode::Dark : TrackerUI::Mode::Light);
        pendingThemeChange=false;
    }
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    if (testMode) TestInjectInput();
    ImGui::NewFrame();

    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGuiWindowFlags hostFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoSavedSettings;
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8*dpiScale, 8*dpiScale));
    ImGui::Begin("##MainWindow", nullptr, hostFlags);
    ImGui::PopStyleVar(3);
    DrawMenuBar();

    ImVec2 region = ImGui::GetContentRegionAvail();
    const float gap = 7*dpiScale;
    const float height = std::max(120.f, region.y - ImGui::GetFrameHeightWithSpacing() - 8*dpiScale);
    // The inspector remains useful on a laptop; the canvas gets extra width on a large screen.
    const float maxPanel = std::max(280*dpiScale, region.x - 400*dpiScale - gap);
    const float minPanel = std::min(360*dpiScale, maxPanel);
    float panelWidth = std::clamp(controlPanelWidth*dpiScale, minPanel, maxPanel);
    const float videoWidth = std::max(100.f, region.x-panelWidth-gap);

    ImGui::BeginChild("VideoWorkspace", ImVec2(videoWidth, height), ImGuiChildFlags_Borders,
                      ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
    DrawVideoTools();
    const float transportHeight = 2*ImGui::GetFrameHeightWithSpacing() +
        ImGui::GetTextLineHeightWithSpacing() + 10*dpiScale;
    float canvasHeight = std::max(60.f, ImGui::GetContentRegionAvail().y - transportHeight);
    ImGui::PushStyleColor(ImGuiCol_ChildBg, TrackerUI::Colors.Canvas);
    ImGui::BeginChild("VideoCanvas", ImVec2(0, canvasHeight), ImGuiChildFlags_Borders,
                      ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
    DrawVideoDisplay();
    ImGui::EndChild();
    ImGui::PopStyleColor();
    DrawToolbar();
    ImGui::EndChild();

    ImGui::SameLine(0, 0);
    ImGui::InvisibleButton("##panelSplitter", ImVec2(gap, height));
    if (ImGui::IsItemHovered() || ImGui::IsItemActive())
    {
        ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
        auto a=ImGui::GetItemRectMin(), b=ImGui::GetItemRectMax();
        ImGui::GetWindowDrawList()->AddLine(ImVec2((a.x+b.x)/2,a.y+8*dpiScale),
            ImVec2((a.x+b.x)/2,b.y-8*dpiScale), ImGui::GetColorU32(TrackerUI::Colors.Accent));
    }
    if (ImGui::IsItemActive())
        controlPanelWidth = std::clamp(panelWidth/dpiScale - ImGui::GetIO().MouseDelta.x/dpiScale,
                                       minPanel/dpiScale, maxPanel/dpiScale);
    ImGui::SameLine(0, 0);
    ImGui::BeginChild("ControlPanel", ImVec2(panelWidth, height), ImGuiChildFlags_Borders);
    TrackerUI::Label("ANALYSIS");
    ImGui::SameLine(ImGui::GetWindowWidth()-159*dpiScale);
    if (ImGui::Button("Load...", ImVec2(66*dpiScale,0))) LoadSettings();
    TrackerUI::Hint("Load a pipeline and analysis settings (Ctrl+L)");
    ImGui::SameLine();
    if (ImGui::Button("Save...", ImVec2(66*dpiScale,0))) SaveSettings();
    TrackerUI::Hint("Save a reproducible XML configuration (Ctrl+S)");
    DrawTabs();
    ImGui::EndChild();
    ImGui::Separator();
    DrawDownscaleControl();
    ImGui::End();

    for (int i=0; i<(int)pipelineDialogOpen.size(); ++i)
        if (pipelineDialogOpen[i]) DrawPluginDialog(i);
    DrawFileBrowser();
    DrawErrorPopup();
    DrawQuitConfirm();

    ImGui::Render();
    int w, h;
    SDL_GL_GetDrawableSize(window, &w, &h);
    glViewport(0, 0, w, h);
    glClearColor(TrackerUI::Colors.Paper.x, TrackerUI::Colors.Paper.y, TrackerUI::Colors.Paper.z, 1);
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
        ImVec2 mark = ImGui::GetCursorScreenPos();
        mark.y += 3*dpiScale;
        TrackerUI::Mark(mark, 15*dpiScale);
        ImGui::Dummy(ImVec2(21*dpiScale, 20*dpiScale));
        ImGui::PushFont(TrackerUI::Mono);
        ImGui::TextUnformatted("useTracker");
        ImGui::PopFont();
        ImGui::Dummy(ImVec2(16*dpiScale, 0));
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Open video or image...", "Ctrl+O")) OpenSource();
            if (ImGui::MenuItem("Save source configuration...")) SaveSource();
            ImGui::Separator();
            if (ImGui::MenuItem("Load analysis...", "Ctrl+L")) LoadSettings();
            if (ImGui::MenuItem("Save analysis...", "Ctrl+S")) SaveSettings();
            ImGui::Separator();
            if (ImGui::MenuItem("Quit", "Esc")) RequestQuit();
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("View"))
        {
            if (ImGui::MenuItem("Dark mode", nullptr, darkMode)) SetDarkMode(true);
            if (ImGui::MenuItem("Light mode", nullptr, !darkMode)) SetDarkMode(false);
            ImGui::Separator();
            if (ImGui::MenuItem("Fit image")) { zoomStartX=zoomStartY=0; zoomEndX=zoomEndY=1; }
            if (ImGui::MenuItem("Tracking overlays", nullptr, &hudVisible))
                pipelineDirty = videoDirty = true;
            ImGui::Separator();
            ImGui::TextDisabled("Interface size");
            for (float scale : {0.85f, 1.f, 1.15f, 1.25f, 1.5f, 1.75f, 2.f})
            {
                char text[24]; snprintf(text, sizeof(text), "%.0f%%", scale*100);
                ImGui::BeginDisabled(scale>MaxUIScale());
                if (ImGui::MenuItem(text, nullptr, std::fabs(dpiScale-scale)<.01f))
                { dpiScale=scale; pendingScaleChange=true; }
                ImGui::EndDisabled();
                if (scale>MaxUIScale()) TrackerUI::Hint("Enlarge the window to use this interface size.");
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Help"))
        {
            if (ImGui::MenuItem("Keyboard & mouse")) showShortcuts=true;
            if (ImGui::MenuItem("About useTracker")) showAbout=true;
            ImGui::EndMenu();
        }
        if (ImGui::GetWindowWidth()>800*dpiScale)
        {
            ImGui::SameLine(ImGui::GetWindowWidth()-226*dpiScale);
            TrackerUI::Label("v" USETRACKER_VERSION);
        }
        ImGui::SameLine(ImGui::GetWindowWidth()-124*dpiScale);
        if (ImGui::Button(darkMode ? "Light mode###ThemeToggle" : "Dark mode###ThemeToggle",
                          ImVec2(112*dpiScale,20*dpiScale)))
            SetDarkMode(!darkMode);
        TrackerUI::Hint(darkMode ? "Switch to Light mode. Your preference is saved." :
                                  "Switch to Dark mode. Your preference is saved.");
        ImGui::EndMenuBar();
    }
    if (showAbout) { ImGui::OpenPopup("About useTracker"); showAbout=false; }
    if (ImGui::BeginPopupModal("About useTracker", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::PushFont(TrackerUI::Heading);
        ImGui::TextUnformatted("useTracker");
        ImGui::PopFont();
        TrackerUI::Label("UNIVERSAL SIMULTANEOUS EVENT TRACKER");
        ImGui::Separator();
        ImGui::TextUnformatted("Open source tools for video analysis and tracking.");
        ImGui::Text("Version %s  /  %s mode", USETRACKER_VERSION, darkMode ? "Dark" : "Light");
        ImGui::TextDisabled("SDL2 / Dear ImGui / OpenCV / FFmpeg");
        ImGui::TextUnformatted("Copyright (C) 2015 Alexandre Campo. GNU GPL v3.");
        ImGui::TextDisabled("Liberation Sans & Mono: SIL Open Font License 1.1.");
        ImGui::Separator();
        if (TrackerUI::AccentButton("Close", ImVec2(100*dpiScale,0))) ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
    }
    if (showShortcuts) { ImGui::OpenPopup("Keyboard & mouse"); showShortcuts=false; }
    if (ImGui::BeginPopupModal("Keyboard & mouse", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        const char* rows[][2] = {
            {"Space", "Play / pause"}, {"Left / Right", "Previous / next frame"},
            {"Backspace", "Stop and return to the beginning"}, {"- / +", "Slower / faster playback"},
            {"Ctrl+O", "Open a video or image"}, {"Ctrl+L / Ctrl+S", "Load / save analysis"},
            {"Ctrl+R", "Enable / disable configured outputs"}, {"Mouse wheel", "Zoom at the pointer"},
            {"Drag image", "Pan when zoomed"}, {"Double-click image", "Fit image"},
            {"Ctrl+click timeline", "Add bookmark"}, {"Shift+click timeline", "Set loop start / end"},
            {"Right-click marker", "Remove timeline marker"}, {"Double-click stage", "Edit stage parameters"}};
        if (ImGui::BeginTable("Shortcuts", 2, ImGuiTableFlags_RowBg, ImVec2(520*dpiScale,0)))
        {
            ImGui::TableSetupColumn("Key", ImGuiTableColumnFlags_WidthFixed, 190*dpiScale);
            ImGui::TableSetupColumn("Action");
            for (auto& row : rows)
            {
                ImGui::TableNextRow(); ImGui::TableNextColumn();
                ImGui::PushFont(TrackerUI::Mono); ImGui::TextUnformatted(row[0]); ImGui::PopFont();
                ImGui::TableNextColumn(); ImGui::TextUnformatted(row[1]);
            }
            ImGui::EndTable();
        }
        if (ImGui::Button("Close", ImVec2(100*dpiScale,0))) ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
    }
}

// ============================================================================
// DrawToolbar
// ============================================================================

bool AppGui::HasSource() const
{
    return ipEngine.capture && ipEngine.capture->type != Capture::NONE &&
        dynamic_cast<CaptureDefault*>(ipEngine.capture) == nullptr;
}

void AppGui::DrawVideoTools()
{
    const bool source = HasSource();
    const std::string fullName = source ? ipEngine.capture->GetName() : "No source loaded";
    const std::string name = source ? std::filesystem::path(fullName).filename().string() : fullName;
    const float openWidth = 77*dpiScale;
    // Clip long names independently so the Open button stays in reach.
    ImGui::BeginChild("SourceName", ImVec2(std::max(30.f,ImGui::GetContentRegionAvail().x-openWidth-7*dpiScale),
                      ImGui::GetFrameHeight()), ImGuiChildFlags_None,
                      ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
    ImGui::AlignTextToFramePadding();
    ImGui::TextUnformatted(name.c_str());
    TrackerUI::Hint(fullName.c_str());
    ImGui::EndChild();
    ImGui::SameLine();
    if (ImGui::Button("Open...", ImVec2(openWidth,0))) OpenSource();
    TrackerUI::Hint("Open a video or image (Ctrl+O). You can also drop a file on the window.");
    if (source)
    {
        ImGui::PushFont(TrackerUI::Mono);
        if (ipEngine.capture->type==Capture::IMAGE)
            ImGui::TextColored(TrackerUI::Colors.Muted,"%d x %d  /  STILL IMAGE",ipEngine.capture->width,ipEngine.capture->height);
        else
            ImGui::TextColored(TrackerUI::Colors.Muted, "%d x %d  /  %.2f fps", ipEngine.capture->width,
                                ipEngine.capture->height, ipEngine.capture->GetFPS());
        ImGui::PopFont();
    }
    else TrackerUI::Label("VIDEO / IMAGE / LIVE CAPTURE");
    ImGui::Separator();
    ImGui::BeginDisabled(!source);
    int mode = processingBlending<.001f ? 0 : (processingBlending>.999f ? 2 : 1);
    const char* modes[] = {"Image", "Blend", "Mask"};
    ImGui::BeginDisabled(activeTab!=TAB_PROCESSING);
    ImGui::SetNextItemWidth(101*dpiScale);
    if (ImGui::Combo("##ViewMode", &mode, modes, 3))
    { processingBlending = mode==0 ? 0.f : (mode==2 ? 1.f : .5f); videoDirty=true; }
    TrackerUI::Hint("Image: video with enhancement plugins applied.\nBlend / Mask: inspect the mask at the selected pipeline stage.");
    ImGui::EndDisabled();
    ImGui::SameLine();
    if (ImGui::Checkbox("Overlays", &hudVisible)) pipelineDirty = videoDirty = true;
    TrackerUI::Hint("Show tracking labels, detections and other plugin overlays.");
    ImGui::SameLine();
    if (TrackerUI::IconButton("##fit", TrackerUI::Icon::Fit, "Fit image (or double-click the image)", dpiScale))
    { zoomStartX=zoomStartY=0; zoomEndX=zoomEndY=1; }
    ImGui::SameLine();
    if (ImGui::Button("Tools")) ImGui::OpenPopup("ImageTools");
    if (ImGui::BeginPopup("ImageTools"))
    {
        if (ImGui::Checkbox("Measure on image", &rulerActive))
        { if (!rulerActive) { rulerMeasurements.clear(); rulerAnchored=false; } }
        TrackerUI::Hint("Click two points on the image. Right-click cancels or removes the last measurement.");
        if (rulerActive)
        {
            ImGui::SetNextItemWidth(160*dpiScale);
            ImGui::Combo("Shape", &rulerShape, "Line\0Rectangle\0");
            if (ImGui::Button("Clear measurements")) { rulerMeasurements.clear(); rulerAnchored=false; }
        }
        ImGui::Separator();
        ImGui::SetNextItemWidth(180*dpiScale);
        if (ImGui::SliderFloat("Blend", &processingBlending, 0.f, 1.f, "%.2f")) videoDirty=true;
        ImGui::EndPopup();
    }
    ImGui::EndDisabled();
    if (source && ImGui::GetWindowWidth()>600*dpiScale)
    {
        ImGui::SameLine(ImGui::GetWindowWidth()-168*dpiScale);
        ImGui::AlignTextToFramePadding();
        ImGui::PushFont(TrackerUI::Mono);
        if (activeTab==TAB_PROCESSING && PluginAt(selectedPipelineItem))
            ImGui::TextColored(TrackerUI::Colors.Accent,"Preview / stage %02d",selectedPipelineItem+1);
        else ImGui::TextDisabled("%s",activeTab==TAB_BACKGROUND ? "BACKGROUND" :
            (activeTab==TAB_CALIBRATION ? "CALIBRATION" : (activeTab==TAB_PROCFRAME ? "ZONE MAP" : "SOURCE IMAGE")));
        ImGui::PopFont();
    }
}

void AppGui::DrawWelcome()
{
    ImVec2 room = ImGui::GetContentRegionAvail();
    float width = std::min(370*dpiScale, room.x);
    float left = ImGui::GetCursorPosX() + std::max(0.f, (room.x-width)*.5f);
    ImGui::SetCursorPos(ImVec2(left, ImGui::GetCursorPosY()+std::max(0.f,(room.y-240*dpiScale)*.44f)));
    TrackerUI::Mark(ImGui::GetCursorScreenPos(), 34*dpiScale);
    ImGui::Dummy(ImVec2(34*dpiScale, 47*dpiScale));
    ImGui::SetCursorPosX(left);
    TrackerUI::Label("OBSERVE / PROCESS / TRACK");
    ImGui::SetCursorPosX(left);
    ImGui::PushFont(TrackerUI::Heading);
    ImGui::TextUnformatted("Start with a source.");
    ImGui::PopFont();
    ImGui::SetCursorPosX(left);
    ImGui::PushTextWrapPos(left+width);
    ImGui::TextColored(TrackerUI::Colors.Muted, "Open a video or image, then build an analysis pipeline in the panel on the right.");
    ImGui::PopTextWrapPos();
    ImGui::Spacing();
    ImGui::SetCursorPosX(left);
    if (TrackerUI::AccentButton("Open video or image...", ImVec2(std::min(width,210*dpiScale), 36*dpiScale))) OpenSource();
    ImGui::SetCursorPosX(left);
    ImGui::TextDisabled("or drop a file here  /  Ctrl+O");
}

void AppGui::DrawToolbar()
{
    const bool source = HasSource();
    const bool seekable = source && ipEngine.capture->GetFrameCount()>1;
    ImGui::BeginDisabled(!seekable);
    DrawSeekBar(ImGui::GetContentRegionAvail().x);
    ImGui::EndDisabled();
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(5*dpiScale, 5*dpiScale));
    ImGui::BeginDisabled(!source || ipEngine.capture->type==Capture::IMAGE);
    if (TrackerUI::IconButton("##stop", TrackerUI::Icon::Stop, "Stop and return to start (Backspace)", dpiScale))
        HandleShortcut(SDLK_BACKSPACE, false);
    ImGui::SameLine();
    ImGui::BeginDisabled(!seekable);
    if (TrackerUI::IconButton("##previous", TrackerUI::Icon::Previous, "Previous frame (Left arrow)", dpiScale))
        RequestStepBackward();
    ImGui::EndDisabled();
    ImGui::SameLine();
    if (TrackerUI::AccentButton(play ? "Pause" : "Play", ImVec2(65*dpiScale,0))) HandleShortcut(SDLK_SPACE, false);
    TrackerUI::Hint("Play / pause (Space)");
    ImGui::SameLine();
    if (TrackerUI::IconButton("##next", TrackerUI::Icon::Next, "Next frame (Right arrow)", dpiScale))
        HandleShortcut(SDLK_RIGHT, false);
    ImGui::SameLine();
    const char* speeds[] = {"1/16x", "1/8x", "1/4x", "1/2x", "1x", "2x", "4x", "8x", "16x"};
    int speed=playSpeed+4;
    ImGui::SetNextItemWidth(68*dpiScale);
    if (ImGui::Combo("##speed", &speed, speeds, 9))
    { playSpeed=speed-4; ipEngine.capture->SetPlaySpeed(playSpeed); }
    TrackerUI::Hint("Playback speed (- / +). The source frame rate is shown above the image.");
    ImGui::EndDisabled();
    ImGui::SameLine();
    ImGui::BeginDisabled(!seekable);
    if (ImGui::Button("Marks")) ImGui::OpenPopup("TimelineMarks");
    if (ImGui::BeginPopup("TimelineMarks"))
    {
        if (ImGui::MenuItem("Add bookmark", "Ctrl+click bar")) bookmarks.push_back(ipEngine.GetPresentTime());
        if (ImGui::MenuItem(loopStart<0 ? "Set loop start" : (loopEnd<0 ? "Set loop end" : "Start a new loop"), "Shift+click bar"))
            AddLoopPoint(ipEngine.GetPresentTime());
        if (ImGui::MenuItem("Clear all marks")) { bookmarks.clear(); loopStart=loopEnd=-1; loopEnabled=false; }
        ImGui::EndPopup();
    }
    ImGui::SameLine();
    ImGui::BeginDisabled(loopStart<0 || loopEnd<=loopStart);
    ImGui::Checkbox("Loop", &loopEnabled);
    TrackerUI::Hint("Set two loop points from Marks or Shift+click the timeline, then enable looping.");
    ImGui::EndDisabled();
    ImGui::EndDisabled();
    ImGui::PopStyleVar();

    ImGui::PushFont(TrackerUI::Mono);
    if (source)
    {
        double time = std::max(0., ipEngine.GetPresentTime());
        long count = ipEngine.capture->GetFrameCount();
        ImGui::TextColored(play ? TrackerUI::Colors.Green : TrackerUI::Colors.Muted, "%s  %02d:%02d.%03d",
            play ? "PLAY" : "HOLD", (int)time/60, (int)time%60, (int)(time*1000)%1000);
        if (ImGui::GetContentRegionAvail().x>120*dpiScale)
        {
            ImGui::SameLine();
            if (count>0) ImGui::TextDisabled(" /  %ld of %ld", ipEngine.GetPresentFrameNumber(), count);
            else ImGui::TextDisabled(" /  frame %ld", ipEngine.GetPresentFrameNumber());
        }
    }
    else ImGui::TextDisabled("Ready when you are.");
    ImGui::PopFont();
}

// A reserved status bar: controls never cover the image or the plugin library.
void AppGui::DrawDownscaleControl()
{
    ImGui::AlignTextToFramePadding();
    TrackerUI::Label("PROCESSING");
    ImGui::SameLine();
    ImGui::BeginDisabled(!HasSource());
    DottedScaleSlider();
    TrackerUI::Hint("Processing resolution. Smaller frames make tuning faster; output coordinates stay at source resolution.");
    ImGui::EndDisabled();
    ImGui::SameLine();
    ImGui::AlignTextToFramePadding();
    ImGui::PushFont(TrackerUI::Mono);
    if (HasSource()) ImGui::TextDisabled("%d x %d", ipEngine.ProcWidth(), ipEngine.ProcHeight());
    else ImGui::TextDisabled("No source");
    ImGui::PopFont();
    ImGui::SameLine(0, 18*dpiScale);
    ImGui::BeginDisabled(!HasSource());
    const bool wasOutput = output;
    if (wasOutput) ImGui::PushStyleColor(ImGuiCol_Text, TrackerUI::Colors.Red);
    if (ImGui::Button(output ? "Outputs enabled" : "Write outputs", ImVec2(136*dpiScale,0)))
        HandleShortcut(SDLK_r, true);
    if (wasOutput) ImGui::PopStyleColor();
    TrackerUI::Hint("Enable / disable files and other outputs configured in active pipeline stages (Ctrl+R).");
    ImGui::EndDisabled();
    const float uiWidth=163*dpiScale;
    if (ImGui::GetContentRegionAvail().x>uiWidth)
        ImGui::SameLine(ImGui::GetWindowWidth()-uiWidth-8*dpiScale);
    else ImGui::SameLine();
    ImGui::AlignTextToFramePadding();
    ImGui::TextDisabled("UI");
    ImGui::SameLine();
    if (ImGui::Button("-##uizoom", ImVec2(26*dpiScale,0)))
    { dpiScale=std::max(.75f,dpiScale-.1f); pendingScaleChange=true; }
    ImGui::SameLine();
    ImGui::TextDisabled("%.0f%%", dpiScale*100);
    ImGui::SameLine();
    ImGui::BeginDisabled(dpiScale>=MaxUIScale()-.01f);
    if (ImGui::Button("+##uizoom", ImVec2(26*dpiScale,0)))
    { dpiScale=std::min(2.f,dpiScale+.1f); pendingScaleChange=true; }
    ImGui::EndDisabled();
    TrackerUI::Hint("Larger interface text. Enlarge the window for more room.");
}

// A slim slider with big dots at 1:1, 1/2, 1/4, 1/8. The axis is the downscale
// exponent (log2 of the divisor) so those ratios are evenly spaced. Dragging is
// continuous but snaps to a dot when near it; the (heavy) engine rebuild is
// applied only on release.
void AppGui::DottedScaleSlider()
{
    const float EMAX = 3.3219f;             // log2(1 / 0.1): min scale 10%
    const int   NDOTS = 4;                  // 1:1, 1/2, 1/4, 1/8  -> exponents 0..3
    const char* dotLabels[NDOTS] = {"1:1", "1/2", "1/4", "1/8"};

    float w = 168 * dpiScale;
    float h = ImGui::GetFrameHeight();

    if (!draggingScale)
        downscaleUI = ipEngine.inputScale;

    ImVec2 p0 = ImGui::GetCursorScreenPos();
    ImGui::InvisibleButton("##dottedscale", ImVec2(w, h));
    bool active = ImGui::IsItemActive();
    bool hovered = ImGui::IsItemHovered();

    float x0 = p0.x + 10*dpiScale;
    float x1 = p0.x + w - 10*dpiScale;
    float ty = p0.y + h * 0.42f;

    auto scaleToX = [&](float scale) {
        float e = log2f(1.0f / std::max(0.05f, std::min(1.0f, scale)));
        return x0 + (e / EMAX) * (x1 - x0);
    };

    if (active)
    {
        float t = (ImGui::GetIO().MousePos.x - x0) / std::max(1.0f, (x1 - x0));
        t = std::max(0.0f, std::min(1.0f, t));
        float e = t * EMAX;
        float nearest = std::min((float)(NDOTS - 1), std::round(e));
        if (std::abs(e - nearest) < 0.18f) e = nearest;   // snap to a dot
        downscaleUI = 1.0f / powf(2.0f, e);
        downscaleUI = std::max(0.1f, std::min(1.0f, downscaleUI));
        draggingScale = true;
    }
    else if (draggingScale)                                // released -> apply
    {
        draggingScale = false;
        ipEngine.SetInputScale(downscaleUI);
        SyncHudSize();
        pipelineDirty = true;
    }

    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImU32 colTrack = ImGui::GetColorU32(ImGuiCol_FrameBg);
    ImU32 colDot   = ImGui::GetColorU32(ImGuiCol_SliderGrab);
    ImU32 colActive= ImGui::GetColorU32(ImGuiCol_SliderGrabActive);
    ImU32 colText  = ImGui::GetColorU32(ImGuiCol_TextDisabled);

    dl->AddLine(ImVec2(x0, ty), ImVec2(x1, ty), colTrack, 3*dpiScale);

    float curE = log2f(1.0f / std::max(0.1f, std::min(1.0f, downscaleUI)));
    for (int i = 0; i < NDOTS; i++)
    {
        float dx = x0 + ((float)i / EMAX) * (x1 - x0);
        bool onDot = std::abs(curE - i) < 0.05f;
        dl->AddCircleFilled(ImVec2(dx, ty), (onDot ? 6.0f : 4.0f) * dpiScale,
                            onDot ? colActive : colDot);
        ImVec2 ts = ImGui::CalcTextSize(dotLabels[i]);
        dl->AddText(ImVec2(dx - ts.x * 0.5f, p0.y + h - ts.y * 0.5f),
                    colText, dotLabels[i]);
    }

    // draggable handle
    float hx = scaleToX(downscaleUI);
    dl->AddCircleFilled(ImVec2(hx, ty), (active || hovered ? 7.0f : 5.0f) * dpiScale,
                        colActive);
}

// Ruler / measure tool overlay. Draws saved and in-progress measurements over
// the video and reports their size in SOURCE pixels (with the processing-space
// value in parentheses when the input is downscaled). imgMin/imgSize are the
// on-screen rectangle of the video image; the current zoom UV window is used so
// measurements track pan/zoom. Must be called right after the video ImGui::Image
// so IsItemHovered()/mouse tests still refer to it.
void AppGui::DrawRulerOverlay(float imgMinX, float imgMinY, float imgSizeX, float imgSizeY)
{
    if (!ipEngine.capture) return;
    float W = (float)ipEngine.capture->width;
    float H = (float)ipEngine.capture->height;
    if (W <= 0 || H <= 0 || imgSizeX <= 0 || imgSizeY <= 0) return;

    auto screenToSource = [&](ImVec2 s, cv::Point2f& out) -> bool
    {
        float nx = (s.x - imgMinX) / imgSizeX;
        float ny = (s.y - imgMinY) / imgSizeY;
        out.x = (zoomStartX + nx * (zoomEndX - zoomStartX)) * W;
        out.y = (zoomStartY + ny * (zoomEndY - zoomStartY)) * H;
        return (nx >= 0 && nx <= 1 && ny >= 0 && ny <= 1);
    };
    auto sourceToScreen = [&](cv::Point2f p) -> ImVec2
    {
        float nx = (p.x / W - zoomStartX) / std::max(1e-6f, (zoomEndX - zoomStartX));
        float ny = (p.y / H - zoomStartY) / std::max(1e-6f, (zoomEndY - zoomStartY));
        return ImVec2(imgMinX + nx * imgSizeX, imgMinY + ny * imgSizeY);
    };

    ImDrawList* dl = ImGui::GetWindowDrawList();
    const ImU32 col = IM_COL32(255, 220, 40, 255);

    auto drawLabel = [&](ImVec2 at, const char* txt)
    {
        ImVec2 ts = ImGui::CalcTextSize(txt);
        ImVec2 p(at.x + 8 * dpiScale, at.y + 6 * dpiScale);
        dl->AddRectFilled(ImVec2(p.x - 3, p.y - 2), ImVec2(p.x + ts.x + 3, p.y + ts.y + 2),
                          IM_COL32(0, 0, 0, 170), 3.0f);
        dl->AddText(p, col, txt);
    };

    auto drawMeasure = [&](cv::Point2f a, cv::Point2f b, int shape)
    {
        ImVec2 sa = sourceToScreen(a), sb = sourceToScreen(b);
        float sc = ipEngine.inputScale;
        char buf[160];
        if (shape == 0) // line
        {
            dl->AddLine(sa, sb, col, 2.0f * dpiScale);
            dl->AddCircleFilled(sa, 3.5f * dpiScale, col);
            dl->AddCircleFilled(sb, 3.5f * dpiScale, col);
            float dx = b.x - a.x, dy = b.y - a.y;
            float len = std::sqrt(dx * dx + dy * dy);
            if (sc < 0.999f)
                snprintf(buf, sizeof(buf), "%.0f px  (proc %.0f)   dx %.0f  dy %.0f",
                         len, len * sc, std::fabs(dx), std::fabs(dy));
            else
                snprintf(buf, sizeof(buf), "%.0f px   dx %.0f  dy %.0f",
                         len, std::fabs(dx), std::fabs(dy));
            drawLabel(ImVec2((sa.x + sb.x) * 0.5f, (sa.y + sb.y) * 0.5f), buf);
        }
        else // rectangle
        {
            ImVec2 r0(std::min(sa.x, sb.x), std::min(sa.y, sb.y));
            ImVec2 r1(std::max(sa.x, sb.x), std::max(sa.y, sb.y));
            dl->AddRect(r0, r1, col, 0, 0, 2.0f * dpiScale);
            float w = std::fabs(b.x - a.x), h = std::fabs(b.y - a.y);
            if (sc < 0.999f)
                snprintf(buf, sizeof(buf), "%.0f x %.0f px  (proc %.0f x %.0f)",
                         w, h, w * sc, h * sc);
            else
                snprintf(buf, sizeof(buf), "%.0f x %.0f px", w, h);
            drawLabel(ImVec2(r0.x, r0.y - ImGui::GetTextLineHeight() - 4 * dpiScale), buf);
        }
    };

    // saved measurements
    for (auto& m : rulerMeasurements)
        drawMeasure(m.p1, m.p2, m.shape);

    if (!ImGui::IsItemHovered()) return;

    cv::Point2f cur;
    bool inside = screenToSource(ImGui::GetMousePos(), cur);

    // live cursor read-out
    char posbuf[48];
    snprintf(posbuf, sizeof(posbuf), "%.0f, %.0f", cur.x, cur.y);
    drawLabel(ImGui::GetMousePos(), posbuf);

    // in-progress measurement follows the cursor
    if (rulerAnchored)
        drawMeasure(rulerP1, cur, rulerShape);

    // left click: place first point, then finalize; right click: cancel / undo
    if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && inside)
    {
        if (!rulerAnchored) { rulerP1 = cur; rulerAnchored = true; }
        else { rulerMeasurements.push_back({rulerP1, cur, rulerShape}); rulerAnchored = false; }
    }
    if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
    {
        if (rulerAnchored) rulerAnchored = false;
        else if (!rulerMeasurements.empty()) rulerMeasurements.pop_back();
    }
}

// Step back one frame. If the previous frame is cached it is instant; if the
// buffer limit is hit it needs a (slower) chunk re-read, which is deferred one
// frame so a loading indicator can be shown first.
void AppGui::RequestStepBackward()
{
    play = false;
    if (ipEngine.capture) ipEngine.capture->Pause();
    if (!ipEngine.capture || ipEngine.capture->GetFrameCount() <= 0) return;

    if (ipEngine.CanStepBackwardCached())
    {
        ipEngine.StepBackward();
        pipelineDirty = true;
    }
    else
    {
        pendingRewind = true;   // heavy: handled in UpdateEngine after showing the bar
    }
}

void AppGui::AddLoopPoint(double t)
{
    if (loopStart < 0)      loopStart = t;                 // first point = start
    else if (loopEnd < 0)                                   // second = end
    {
        loopEnd = t;
        if (loopEnd < loopStart) std::swap(loopStart, loopEnd);
    }
    else { loopStart = t; loopEnd = -1.0; }                 // restart a fresh range
}

// Custom video seek bar: shows a hover read-out (frame + time) so a precise
// time can be found, draws timeline markers (cyan bookmarks, orange loop
// region), and supports Ctrl+click to bookmark and Shift+click to set loop
// points. Plain click/drag seeks; right-click removes the nearest marker.
void AppGui::DrawSeekBar(float width)
{
    if (!ipEngine.capture) return;
    double fps = ipEngine.capture->GetFPS();
    long fcount = ipEngine.capture->GetFrameCount();
    double totalTime = (fps > 0 && fcount > 0) ? (double)fcount / fps : 0.0;

    if (width < 60 * dpiScale) width = 60 * dpiScale;
    float h = ImGui::GetFrameHeight();
    ImVec2 p0 = ImGui::GetCursorScreenPos();
    ImGui::InvisibleButton("##seekbar", ImVec2(width, h));
    bool hovered = ImGui::IsItemHovered();
    bool active  = ImGui::IsItemActive();
    ImGuiIO& io = ImGui::GetIO();

    float x0 = p0.x, x1 = p0.x + width;
    float ymid = p0.y + h * 0.5f;
    float trackTop = ymid - 3 * dpiScale, trackBot = ymid + 3 * dpiScale;

    auto timeToX = [&](double t){ return x0 + (totalTime > 0 ? (float)(t / totalTime) : 0.f) * width; };
    auto xToTime = [&](float x){ float u = (x - x0) / width; u = std::max(0.f, std::min(1.f, u)); return totalTime * u; };
    auto fmtTime = [&](char* buf, size_t n, double t){
        long f = (fps > 0) ? (long)(t * fps + 0.5) : 0;
        int mm = (int)(t / 60.0), ss = (int)t % 60, ms = (int)((t - (long)t) * 1000);
        snprintf(buf, n, "frame %ld   %02d:%02d.%03d", f, mm, ss, ms);
    };

    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImU32 colTrack = ImGui::GetColorU32(ImGuiCol_FrameBg);
    ImU32 colFill  = ImGui::GetColorU32(ImGuiCol_SliderGrab);
    ImU32 colHead  = ImGui::GetColorU32(ImGuiCol_SliderGrabActive);
    ImU32 colBook  = ImGui::GetColorU32(TrackerUI::Colors.Bookmark);
    ImU32 colLoop  = ImGui::GetColorU32(TrackerUI::Colors.Accent);

    dl->AddRectFilled(ImVec2(x0, trackTop), ImVec2(x1, trackBot), colTrack, 2 * dpiScale);

    // loop region shading
    if (loopStart >= 0 && loopEnd > loopStart)
        dl->AddRectFilled(ImVec2(timeToX(loopStart), p0.y), ImVec2(timeToX(loopEnd), p0.y + h),
                          ImGui::GetColorU32(ImGuiCol_CheckMark, loopEnabled ? .28f : .12f));

    // progress fill up to the playhead
    float px = x0 + videoSliderPos * width;
    dl->AddRectFilled(ImVec2(x0, trackTop), ImVec2(px, trackBot), colFill, 2 * dpiScale);

    // bookmark ticks
    for (double t : bookmarks)
    {
        float mx = timeToX(t);
        dl->AddLine(ImVec2(mx, p0.y), ImVec2(mx, p0.y + h), colBook, 2 * dpiScale);
    }
    // loop-point ticks
    if (loopStart >= 0) { float mx = timeToX(loopStart); dl->AddLine(ImVec2(mx, p0.y), ImVec2(mx, p0.y + h), colLoop, 2 * dpiScale); }
    if (loopEnd   >= 0) { float mx = timeToX(loopEnd);   dl->AddLine(ImVec2(mx, p0.y), ImVec2(mx, p0.y + h), colLoop, 2 * dpiScale); }

    // playhead
    dl->AddCircleFilled(ImVec2(px, ymid), 5 * dpiScale, colHead);

    auto seekToX = [&](float x){
        float u = (x - x0) / width; u = std::max(0.f, std::min(1.f, u));
        videoSliderPos = u; sliderMoving = true;
        if (fcount > 0) { ipEngine.SeekTime(u * totalTime); pipelineDirty = true; }
    };

    if (hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
    {
        double t = xToTime(io.MousePos.x);
        if (io.KeyCtrl)       bookmarks.push_back(t);
        else if (io.KeyShift) AddLoopPoint(t);
        else                  seekToX(io.MousePos.x);
    }
    if (active && !io.KeyCtrl && !io.KeyShift && ImGui::IsMouseDown(ImGuiMouseButton_Left))
        seekToX(io.MousePos.x);
    if (sliderMoving && !ImGui::IsMouseDown(ImGuiMouseButton_Left))
        sliderMoving = false;

    // right-click removes the nearest marker
    if (hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
    {
        float mxm = io.MousePos.x, best = 8 * dpiScale;
        int which = 0, bi = -1;   // which: 1=loopStart, 2=loopEnd, 3=bookmark
        for (int i = 0; i < (int)bookmarks.size(); i++)
        { float d = std::fabs(timeToX(bookmarks[i]) - mxm); if (d < best) { best = d; which = 3; bi = i; } }
        if (loopStart >= 0) { float d = std::fabs(timeToX(loopStart) - mxm); if (d < best) { best = d; which = 1; } }
        if (loopEnd   >= 0) { float d = std::fabs(timeToX(loopEnd)   - mxm); if (d < best) { best = d; which = 2; } }
        if (which == 1) loopStart = -1.0;
        else if (which == 2) loopEnd = -1.0;
        else if (which == 3 && bi >= 0) bookmarks.erase(bookmarks.begin() + bi);
    }

    // hover read-out: frame number + time, plus a guide line at the cursor
    if (hovered)
    {
        char buf[64]; fmtTime(buf, sizeof(buf), xToTime(io.MousePos.x));
        ImGui::SetTooltip("%s", buf);
        dl->AddLine(ImVec2(io.MousePos.x, p0.y), ImVec2(io.MousePos.x, p0.y + h),
                    ImGui::GetColorU32(ImGuiCol_Text, .35f), 1.0f);
    }
}

// ============================================================================
// DrawVideoDisplay
// ============================================================================

void AppGui::DrawVideoDisplay()
{
    if (!HasSource()) { DrawWelcome(); return; }
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

    // Only rebuild the composited texture when the image actually changed:
    // the HUD alpha-blend is a full-frame per-pixel loop and the upload is a
    // GPU transfer, both wasteful to repeat every frame while paused.
    if (!oglScreen.empty() && (videoDirty || videoTexture == 0))
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

        // Overlay HUD if visible. The HUD is at source resolution; upscale the
        // (possibly downscaled) frame to match so the overlay stays crisp and a
        // fixed size on screen regardless of the processing downscale.
        if (hudVisible && !ipEngine.hud.empty())
        {
            // displayFrame may still share pixels with the capture frame;
            // copy before drawing on it or we would corrupt the source data
            if (displayFrame.data == oglScreen.data)
                displayFrame = oglScreen.clone();

            // some views (zone map) are single channel; the HUD blend below
            // writes Vec3b, so make sure the frame is 3-channel BGR first
            if (displayFrame.channels() == 1)
                cv::cvtColor(displayFrame, displayFrame, cv::COLOR_GRAY2BGR);

            if (displayFrame.size() != ipEngine.hud.size())
                cv::resize(displayFrame, displayFrame, ipEngine.hud.size(),
                           0, 0, cv::INTER_LINEAR);

            // The HUD is BGRA with alpha channel (row-pointer access — much
            // faster than per-pixel .at<>())
            for (int y = 0; y < ipEngine.hud.rows; y++)
            {
                const cv::Vec4b* hrow = ipEngine.hud.ptr<cv::Vec4b>(y);
                cv::Vec3b* drow = displayFrame.ptr<cv::Vec3b>(y);
                for (int x = 0; x < ipEngine.hud.cols; x++)
                {
                    unsigned char a = hrow[x][3];
                    if (a > 0)
                    {
                        int ia = 255 - a;
                        drow[x][0] = (unsigned char)((drow[x][0] * ia + hrow[x][0] * a) / 255);
                        drow[x][1] = (unsigned char)((drow[x][1] * ia + hrow[x][1] * a) / 255);
                        drow[x][2] = (unsigned char)((drow[x][2] * ia + hrow[x][2] * a) / 255);
                    }
                }
            }
        }

        UploadVideoTexture(displayFrame);
        videoDirty = false;
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

        ImVec2 cursor = ImGui::GetCursorPos();
        ImGui::SetCursorPos(ImVec2(cursor.x + (avail.x-displayW)*.5f,
                                   cursor.y + (avail.y-displayH)*.5f));
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

        // Ruler / measure tool (takes over mouse interaction while active)
        if (rulerActive)
            DrawRulerOverlay(itemMinAbs.x, itemMinAbs.y, itemSizeAbs.x, itemSizeAbs.y);

        // Loading indicator while a backward chunk is being re-read from disk
        if (pendingRewind)
        {
            ImDrawList* dl = ImGui::GetWindowDrawList();
            float bw = 240 * dpiScale, bh = 46 * dpiScale;
            ImVec2 c(itemMinAbs.x + itemSizeAbs.x * 0.5f, itemMinAbs.y + itemSizeAbs.y * 0.5f);
            ImVec2 r0(c.x - bw * 0.5f, c.y - bh * 0.5f), r1(c.x + bw * 0.5f, c.y + bh * 0.5f);
            dl->AddRectFilled(r0, r1, IM_COL32(0, 0, 0, 200), 6 * dpiScale);
            dl->AddRect(r0, r1, IM_COL32(255, 220, 40, 220), 6 * dpiScale, 0, 1.5f);
            float prog = ipEngine.RefillProgress();
            char txt[48];
            snprintf(txt, sizeof(txt), "Loading frames... %d%%", (int)(prog * 100.0f + 0.5f));
            ImVec2 ts = ImGui::CalcTextSize(txt);
            dl->AddText(ImVec2(c.x - ts.x * 0.5f, r0.y + 6 * dpiScale), IM_COL32(255, 255, 255, 255), txt);
            // real progress bar
            float by = r1.y - 12 * dpiScale, bxl = r0.x + 12 * dpiScale, bxr = r1.x - 12 * dpiScale;
            dl->AddRectFilled(ImVec2(bxl, by), ImVec2(bxr, by + 6 * dpiScale), IM_COL32(70, 70, 70, 255), 2);
            dl->AddRectFilled(ImVec2(bxl, by), ImVec2(bxl + (bxr - bxl) * prog, by + 6 * dpiScale),
                              IM_COL32(255, 220, 40, 255), 2);
        }

        // PatternTracker click-to-seed mode
        bool seedMode = (!rulerActive && patternSeedPluginIndex >= 0 &&
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

            // Pan by dragging: middle button always, or left button when
            // zoomed in and no tool is using the left click (ruler, pattern
            // seeding, white-point pick, ROI editing)
            bool zoomedIn = (zoomEndX - zoomStartX < 0.999f) ||
                            (zoomEndY - zoomStartY < 0.999f);
            bool leftPan = zoomedIn && !rulerActive && !seedMode && !pickMode &&
                           !roiEditing && ImGui::IsMouseDragging(ImGuiMouseButton_Left);
            if (ImGui::IsMouseDragging(ImGuiMouseButton_Middle) || leftPan)
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

                ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeAll);
            }
            else if (zoomedIn && !rulerActive && !seedMode && !pickMode && !roiEditing)
            {
                // hint that the image can be grabbed to pan
                ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
            }

            // Double-click to reset zoom (disabled while a tool uses the left
            // click: ruler, seeding targets, white-point pick, ROI editing)
            if (!rulerActive && !seedMode && !pickMode && !roiEditing &&
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

}

// ============================================================================
// DrawTabs
// ============================================================================

void AppGui::DrawTabs()
{
    int prevTab = activeTab;
    if (ImGui::BeginTabBar("##MainTabs", ImGuiTabBarFlags_FittingPolicyScroll))
    {
        if (ImGui::BeginTabItem("Pipeline"))
        {
            activeTab = TAB_PROCESSING;
            DrawProcessingTab();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Background"))
        {
            activeTab = TAB_BACKGROUND;
            ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x*.52f);
            DrawBackgroundTab();
            ImGui::PopItemWidth();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Calibration"))
        {
            activeTab = TAB_CALIBRATION;
            ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x*.52f);
            DrawCalibrationTab();
            ImGui::PopItemWidth();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Timing"))
        {
            activeTab = TAB_PROCFRAME;
            ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x*.52f);
            DrawProcessingFrameTab();
            ImGui::PopItemWidth();
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

namespace
{
std::string PluginTitle(const std::string& key)
{
    static const std::map<std::string, std::string> titles = {
        {"Clahe", "Local contrast (CLAHE)"}, {"BackgroundDiffKnn", "Background / KNN"},
        {"BackgroundDiffMog", "Background / MOG"}, {"BackgroundDiffMog2", "Background / MOG2"},
        {"BackgroundDiffGmg", "Background / GMG"}, {"BackgroundDiffGsoc", "Background / GSOC"},
        {"YoloDetector", "YOLO detector"}, {"Aruco", "ArUco markers"}, {"ArucoColor", "Color ArUco markers"},
        {"ZonesOfInterest", "Zones of interest"}, {"GetBlobsAngles", "Blob orientation"}};
    auto found=titles.find(key);
    if (found!=titles.end()) return found->second;
    std::string result;
    for (unsigned i=0; i<key.size(); ++i)
    {
        unsigned char c=key[i];
        if (i && std::isupper(c)) { result+=' '; result+=(char)std::tolower(c); }
        else result+=(char)c;
    }
    return result;
}
const char* PluginSummary(const std::string& key)
{
    static const std::map<std::string, const char*> text = {
        {"Clahe", "Recover local contrast"}, {"Curves", "Adjust tones and color channels"},
        {"WhiteBalance", "Correct a color cast"}, {"Denoise", "Reduce spatial noise"},
        {"TemporalDenoise", "Reduce noise across frames"}, {"Sharpen", "Bring out edges and detail"},
        {"Dehaze", "Reduce haze and backscatter"}, {"BackgroundDifference", "Compare with a reference background"},
        {"FrameDifference", "Find changes between frames"}, {"MovingAverage", "Estimate a rolling background"},
        {"BackgroundDiffKnn", "Separate moving objects with KNN"}, {"BackgroundDiffMog", "Model a changing background"},
        {"BackgroundDiffMog2", "Adaptive background and shadow detection"},
        {"BackgroundDiffGmg", "Statistical background subtraction"}, {"BackgroundDiffGsoc", "Adaptive foreground extraction"},
        {"AdaptiveThreshold", "Segment using local brightness"}, {"ColorSegmentation", "Select a range of colors"},
        {"Erosion", "Remove small foreground regions"}, {"Dilation", "Expand and connect foreground regions"},
        {"SafeErosion", "Shrink regions while preserving blobs"}, {"ExtractBlobs", "Turn a mask into detected objects"},
        {"GetBlobsAngles", "Estimate object orientation"}, {"PatternTracker", "Follow visual patterns across frames"},
        {"TrackBlobs", "Link detected objects into trajectories"}, {"YoloDetector", "Detect objects with an ONNX model"},
        {"ZonesOfInterest", "Limit analysis to regions of interest"}, {"Aruco", "Locate and identify fiducial markers"},
        {"ArucoColor", "Identify colored fiducial markers"}, {"SimpleTags", "Detect simple visual tags"},
        {"RecordVideo", "Write the processed video"}, {"RecordPixels", "Export pixel data"},
        {"TakeSnapshots", "Save individual frames"}, {"Stopwatch", "Measure events over time"},
        {"RemoteControl", "Send events to external hardware"}};
    auto found=text.find(key);
    return found==text.end() ? "Configure this processing stage" : found->second;
}
}

PipelinePlugin* AppGui::PluginAt(int index)
{
    if (index<0 || ipEngine.pipelines.empty() || index>=(int)ipEngine.pipelines[0].plugins.size()) return nullptr;
    for (auto& pipeline : ipEngine.pipelines)
        if (pipeline.plugins[index]) return pipeline.plugins[index];
    return nullptr;
}

void AppGui::MovePipelinePlugin(int from, int to)
{
    if (!PluginAt(from) || !PluginAt(to) || from==to) return;
    auto plugins=ipEngine.Erase(from);
    ipEngine.Insert(to, plugins, false);
    for (auto* states : {&pipelineDialogOpen, &pipelineHelpOpen})
    {
        bool value=(*states)[from];
        states->erase(states->begin()+from);
        states->insert(states->begin()+to, value);
    }
    auto remap=[&](int& index) {
        if (index==from) index=to;
        else if (from<to && index>from && index<=to) --index;
        else if (from>to && index>=to && index<from) ++index;
    };
    remap(selectedPipelineItem);
    remap(patternSeedPluginIndex);
    remap(whitePickPluginIndex);
    pipelineDirty=true;
    ipEngine.takeSnapshot=true;
    scrollToPipelineSelection=true;
}

void AppGui::RemovePipelinePlugin(int index)
{
    if (!PluginAt(index)) return;
    auto plugins=ipEngine.Erase(index);
    for (auto* plugin : plugins)
        if (plugin) { plugin->CloseOutput(); delete plugin; }
    pipelineDialogOpen.erase(pipelineDialogOpen.begin()+index);
    pipelineHelpOpen.erase(pipelineHelpOpen.begin()+index);
    auto remap=[&](int& value) { if (value==index) value=-1; else if (value>index) --value; };
    remap(patternSeedPluginIndex);
    remap(whitePickPluginIndex);
    if (selectedPipelineItem==index)
    {
        selectedPipelineItem=std::min(index,(int)pipelineDialogOpen.size()-1);
        roiEditing=false; roiActivePolygon=roiSelectedPolygon=roiDragPoly=roiDragPoint=-1;
    }
    else if (selectedPipelineItem>index) --selectedPipelineItem;
    pipelineDirty=true;
    ipEngine.takeSnapshot=true;
}

void AppGui::DrawProcessingTab()
{
    int count=ipEngine.pipelines.empty() ? 0 : (int)ipEngine.pipelines[0].plugins.size();
    int enabled=0;
    for (int i=0; i<count; ++i) if (auto* p=PluginAt(i)) enabled+=p->active;
    ImGui::AlignTextToFramePadding();
    TrackerUI::Label("PIPELINE");
    ImGui::SameLine();
    ImGui::TextDisabled("%d stages / %d active", count, enabled);

    float total=ImGui::GetContentRegionAvail().y;
    float maxHeight=std::max(90*dpiScale,total-238*dpiScale);
    float ph=std::clamp((count ? pipelineListHeight : 154.f)*dpiScale, 90*dpiScale, maxHeight);
    ImGui::PushStyleColor(ImGuiCol_ChildBg, TrackerUI::Colors.Paper);
    ImGui::BeginChild("PipelineList", ImVec2(0,ph), ImGuiChildFlags_Borders);
    if (count==0)
    {
        ImGui::Dummy(ImVec2(0,10*dpiScale));
        ImGui::TextUnformatted("Build your analysis, stage by stage.");
        ImGui::TextWrapped("Find a plugin below and add it to the pipeline. Stages run from top to bottom.");
        ImGui::Spacing();
        ImGui::TextDisabled("Already have a configuration?");
        if (ImGui::Button("Load analysis...")) LoadSettings();
    }
    int moveFrom=-1, moveTo=-1;
    for (int i=0; i<count; ++i)
    {
        auto* pp=PluginAt(i);
        if (!pp) continue;
        ImGui::PushID(i);
        ImVec2 row=ImGui::GetCursorPos();
        ImGui::SetCursorPosY(row.y+7*dpiScale);
        bool active=pp->active;
        if (ImGui::Checkbox("##active", &active))
        {
            for (auto& pipeline : ipEngine.pipelines)
                if (pipeline.plugins[i]) pipeline.plugins[i]->active=active;
            pipelineDirty=true;
        }
        TrackerUI::Hint("Enable / bypass this stage");
        ImGui::SameLine();
        ImGui::SetCursorPosY(row.y);
        bool selected=selectedPipelineItem==i;
        if (ImGui::Selectable("##stage", selected, ImGuiSelectableFlags_AllowDoubleClick, ImVec2(0,43*dpiScale)))
        {
            selectedPipelineItem=i;
            pipelineDirty=true; ipEngine.takeSnapshot=true;
            if (ImGui::IsMouseDoubleClicked(0)) pipelineDialogOpen[i]=true;
        }
        TrackerUI::Hint("Select to preview this stage. Double-click to edit. Drag to reorder.");
        ImVec2 a=ImGui::GetItemRectMin(), b=ImGui::GetItemRectMax();
        auto* draw=ImGui::GetWindowDrawList();
        draw->PushClipRect(a,b,true);
        if (selected) draw->AddRectFilled(a,ImVec2(a.x+2*dpiScale,b.y),ImGui::GetColorU32(TrackerUI::Colors.Accent));
        char number[16]; snprintf(number,sizeof(number),"%02d",i+1);
        draw->AddText(TrackerUI::Mono,TrackerUI::Mono->FontSize,ImVec2(a.x+8*dpiScale,a.y+6*dpiScale),
                      ImGui::GetColorU32(selected ? TrackerUI::Colors.Accent : TrackerUI::Colors.Muted),number);
        std::string title=PluginTitle(pp->registryName);
        draw->AddText(ImVec2(a.x+36*dpiScale,a.y+3*dpiScale),ImGui::GetColorU32(active ? TrackerUI::Colors.Ink : TrackerUI::Colors.Muted),title.c_str());
        draw->AddText(ImVec2(a.x+36*dpiScale,a.y+23*dpiScale),ImGui::GetColorU32(TrackerUI::Colors.Muted),PluginSummary(pp->registryName));
        draw->PopClipRect();
        if (ImGui::BeginDragDropSource())
        {
            ImGui::SetDragDropPayload("PIPELINE_STAGE",&i,sizeof(i));
            ImGui::TextUnformatted(title.c_str());
            ImGui::EndDragDropSource();
        }
        if (ImGui::BeginDragDropTarget())
        {
            if (const auto* payload=ImGui::AcceptDragDropPayload("PIPELINE_STAGE"))
            { moveFrom=*(const int*)payload->Data; moveTo=i; }
            ImGui::EndDragDropTarget();
        }
        if (selected && scrollToPipelineSelection) ImGui::SetScrollHereY(.5f);
        ImGui::PopID();
    }
    scrollToPipelineSelection=false;
    ImGui::EndChild();
    ImGui::PopStyleColor();
    if (moveFrom>=0) MovePipelinePlugin(moveFrom,moveTo);

    ImGui::InvisibleButton("##pipeSplit", ImVec2(-1,5*dpiScale));
    if (ImGui::IsItemHovered() || ImGui::IsItemActive()) ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNS);
    if (ImGui::IsItemActive()) pipelineListHeight=std::clamp(ph/dpiScale+ImGui::GetIO().MouseDelta.y/dpiScale,90.f,maxHeight/dpiScale);
    auto a=ImGui::GetItemRectMin(), b=ImGui::GetItemRectMax();
    float center=(a.x+b.x)*.5f;
    ImGui::GetWindowDrawList()->AddLine(ImVec2(center-16*dpiScale,a.y+2*dpiScale),
        ImVec2(center+16*dpiScale,a.y+2*dpiScale),ImGui::GetColorU32(ImGuiCol_Separator),2*dpiScale);

    ImGui::BeginDisabled(!PluginAt(selectedPipelineItem));
    if (ImGui::Button("Edit parameters...", ImVec2(std::max(110*dpiScale,ImGui::GetContentRegionAvail().x-117*dpiScale),0)))
        pipelineDialogOpen[selectedPipelineItem]=true;
    ImGui::SameLine();
    ImGui::BeginDisabled(selectedPipelineItem<=0);
    if (TrackerUI::IconButton("##moveUp",TrackerUI::Icon::Up,"Move stage up",dpiScale))
        MovePipelinePlugin(selectedPipelineItem,selectedPipelineItem-1);
    ImGui::EndDisabled();
    ImGui::SameLine();
    ImGui::BeginDisabled(selectedPipelineItem>=count-1);
    if (TrackerUI::IconButton("##moveDown",TrackerUI::Icon::Down,"Move stage down",dpiScale))
        MovePipelinePlugin(selectedPipelineItem,selectedPipelineItem+1);
    ImGui::EndDisabled();
    ImGui::SameLine();
    if (TrackerUI::IconButton("##remove",TrackerUI::Icon::Remove,"Remove selected stage",dpiScale))
        RemovePipelinePlugin(selectedPipelineItem);
    ImGui::EndDisabled();
    ImGui::Separator();
    TrackerUI::Label("PLUGIN LIBRARY");
    ImGui::SetNextItemWidth(-1);
    ImGui::InputTextWithHint("##PluginSearch","Search plugins, e.g. background...",pluginSearch,sizeof(pluginSearch));
    std::string needle=pluginSearch;
    std::transform(needle.begin(),needle.end(),needle.begin(),[](unsigned char c){return (char)std::tolower(c);});
    auto matches=[&](const std::string& key, const char* category) {
        std::string haystack=key+" "+PluginTitle(key)+" "+PluginSummary(key)+" "+category;
        std::transform(haystack.begin(),haystack.end(),haystack.begin(),[](unsigned char c){return (char)std::tolower(c);});
        return haystack.find(needle)!=std::string::npos;
    };
    struct Category { const char* name; std::vector<std::string> keys; };
    static const std::vector<Category> categories={
        {"Enhance",{"Clahe","Curves","WhiteBalance","Denoise","TemporalDenoise","Sharpen","Dehaze"}},
        {"Background",{"BackgroundDifference","FrameDifference","MovingAverage","BackgroundDiffMog","BackgroundDiffMog2","BackgroundDiffGmg","BackgroundDiffGsoc","BackgroundDiffKnn"}},
        {"Segment & clean",{"AdaptiveThreshold","ColorSegmentation","Erosion","Dilation","SafeErosion"}},
        {"Detect & track",{"ExtractBlobs","GetBlobsAngles","Aruco","ArucoColor","SimpleTags","PatternTracker","YoloDetector","TrackBlobs"}},
        {"Regions",{"ZonesOfInterest"}},
        {"Record & export",{"RecordVideo","RecordPixels","TakeSnapshots","Stopwatch","RemoteControl"}}};
    std::vector<bool> categorized(availablePluginNames.size(),false);
    bool anyMatch=false, selectedVisible=false;
    auto entry=[&](int i) {
        anyMatch=true;
        if (selectedAvailablePlugin==i) selectedVisible=true;
        const auto& key=availablePluginNames[i];
        std::string title=PluginTitle(key);
        ImGui::PushID(i);
        if (ImGui::Selectable(title.c_str(),selectedAvailablePlugin==i,ImGuiSelectableFlags_AllowDoubleClick,ImVec2(0,22*dpiScale)))
        {
            selectedAvailablePlugin=i; selectedVisible=true;
            if (ImGui::IsMouseDoubleClicked(0)) { cv::FileNode node; AddPipelinePlugin(key,node); }
        }
        TrackerUI::Hint(PluginSummary(key));
        ImGui::PopID();
    };
    ImGui::BeginChild("AvailablePlugins",ImVec2(0,std::max(45.f,ImGui::GetContentRegionAvail().y-ImGui::GetFrameHeightWithSpacing())),ImGuiChildFlags_Borders);
    for (const auto& category : categories)
    {
        std::vector<int> members;
        for (const auto& key : category.keys)
            for (int i=0; i<(int)availablePluginNames.size(); ++i)
                if (availablePluginNames[i]==key)
                {
                    categorized[i]=true;
                    if (matches(key,category.name)) members.push_back(i);
                    break;
                }
        if (members.empty()) continue;
        anyMatch=true;
        if (!needle.empty()) ImGui::SetNextItemOpen(true,ImGuiCond_Always);
        if (ImGui::CollapsingHeader(category.name,ImGuiTreeNodeFlags_DefaultOpen))
        { for (int i : members) entry(i); }
    }
    for (int i=0; i<(int)availablePluginNames.size(); ++i)
        if (!categorized[i] && matches(availablePluginNames[i],"Other")) entry(i);
    if (!anyMatch) { ImGui::TextUnformatted("No matching plugins."); ImGui::TextWrapped("Try a name or a task, such as contrast, blobs or video."); }
    ImGui::EndChild();
    ImGui::BeginDisabled(!selectedVisible);
    if (TrackerUI::AccentButton("+ Add to pipeline",ImVec2(-1,0)))
    { cv::FileNode node; AddPipelinePlugin(availablePluginNames[selectedAvailablePlugin],node); }
    ImGui::EndDisabled();
}

// ============================================================================
// DrawBackgroundTab
// ============================================================================

void AppGui::DrawBackgroundTab()
{
    TrackerUI::Label("REFERENCE BACKGROUND");
    ImGui::TextWrapped("Estimate a reference image from your recording, or load one from disk.");
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
    TrackerUI::Label("ANALYSIS INTERVAL");
    ImGui::TextWrapped("Limit processing to part of the recording. Times are in seconds; zero duration uses the remainder.");
    ImGui::Separator();

    if (ImGui::InputFloat("Start (s)", &ipEngine.startTime, 0.1f, 1.0f, "%.2f")) pipelineDirty=true;
    if (ImGui::InputFloat("Duration (s)", &ipEngine.durationTime, 0.1f, 1.0f, "%.2f")) pipelineDirty=true;
    if (ImGui::InputFloat("Timestep (s)", &ipEngine.timestep, 0.001f, 0.01f, "%.3f")) pipelineDirty=true;
    if (ImGui::Checkbox("Use time boundaries", &ipEngine.useTimeBoundaries)) pipelineDirty=true;
    ipEngine.startTime=std::max(0.f,ipEngine.startTime);
    ipEngine.durationTime=std::max(0.f,ipEngine.durationTime);
    ipEngine.timestep=std::max(0.f,ipEngine.timestep);
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
    dl->AddRectFilled(p0, p1, ImGui::GetColorU32(TrackerUI::Colors.Canvas));
    for (int i = 1; i < 4; i++)
    {
        float t = i / 4.0f;
        dl->AddLine(ImVec2(p0.x + t * sz, p0.y), ImVec2(p0.x + t * sz, p1.y), ImGui::GetColorU32(TrackerUI::Colors.Line));
        dl->AddLine(ImVec2(p0.x, p0.y + t * sz), ImVec2(p1.x, p0.y + t * sz), ImGui::GetColorU32(TrackerUI::Colors.Line));
    }
    dl->AddLine(ImVec2(p0.x, p1.y), ImVec2(p1.x, p0.y), ImGui::GetColorU32(TrackerUI::Colors.Muted));
    dl->AddRect(p0, p1, ImGui::GetColorU32(TrackerUI::Colors.Line));

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
        dl->AddLine(prev, cur, ImGui::GetColorU32(TrackerUI::Colors.Accent), 2.0f);
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
        dl->AddCircleFilled(sp, r, ImGui::GetColorU32(TrackerUI::Colors.Ink));
        dl->AddCircle(sp, r, ImGui::GetColorU32(TrackerUI::Colors.Surface));
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

// per-plugin help: default values and the impact of each setting
static const char* PluginHelpText(const std::string& name)
{
    if (name == "BackgroundDifference")
        return "Background Difference\n\n"
               "Subtracts a fixed background image (set in the Background tab) "
               "from the current frame and keeps pixels that differ.\n\n"
               "Threshold (def 42): minimum brightness difference to count as "
               "foreground. Lower = more sensitive (more noise); higher = only "
               "strong changes.\n"
               "Additive: OR the result into the mask (combine detectors) "
               "instead of AND.\n"
               "Restrict to Zone: keep foreground only inside the given ROI region.";

    if (name == "BackgroundDiffMog")
        return "Background Diff MOG (Mixture of Gaussians)\n\n"
               "Learns a per-pixel statistical model of the scene and marks "
               "pixels that no longer fit it as foreground.\n\n"
               "History (def 200): number of past frames the model remembers. "
               "Higher = more stable, slower to adapt.\n"
               "Num Mixtures (def 5): Gaussian components per pixel (1-8). More = "
               "handles multi-modal backgrounds (waves, foliage) but slower.\n"
               "Background Ratio (def 0.7): fraction of weight that defines the "
               "background. Higher = more of the scene treated as background.\n"
               "Noise Sigma (def 15): expected image noise; higher tolerates more "
               "noise (fewer false detections, may miss faint targets).\n"
               "Learning Rate (def 0.05): how fast the model adapts (0-1). Higher "
               "adapts quickly but absorbs slow/stopped targets. Frozen while paused.\n"
               "Additive / Restrict to Zone: as in the other detectors.";

    if (name == "BackgroundDiffMog2")
        return "Background Diff MOG2\n\n"
               "Improved Mixture-of-Gaussians subtractor with automatic per-pixel "
               "component count.\n\n"
               "History (def 200): frames remembered; higher = more stable, slower "
               "to adapt.\n"
               "Threshold (def 16): squared Mahalanobis distance to flag a pixel as "
               "foreground. Lower = more sensitive/noisy; higher = stricter.\n"
               "Shadow Detection: mark shadows separately (grey) instead of "
               "foreground. Useful outdoors.\n"
               "Learning Rate (def 0.05): adaptation speed (0-1); -1 lets it auto-"
               "tune. Frozen while paused.\n"
               "Additive / Restrict to Zone: as in the other detectors.";

    if (name == "BackgroundDiffGmg")
        return "Background Diff GMG\n\n"
               "Bayesian per-pixel subtractor; needs a warm-up period before it "
               "produces stable output.\n\n"
               "Learning Rate (def 0.05): how fast the background model adapts "
               "(0-1). Higher adapts faster but absorbs slow targets. Frozen "
               "while paused.\n"
               "Additive / Restrict to Zone: as in the other detectors.";

    if (name == "BackgroundDiffGsoc")
        return "Background Diff GSOC\n\n"
               "Sample-based background subtractor (OpenCV bgsegm). Each pixel "
               "keeps a set of recent background samples; pixels that match too "
               "few are marked foreground. Robust on noisy / low-contrast footage.\n\n"
               "Num Samples (def 20, range 2-1023): background samples kept per "
               "pixel. More = more stable model, slower to adapt, more memory.\n"
               "Replace Rate (def 0.003): chance of replacing a stored sample with "
               "the current pixel each frame. Higher = adapts faster to lasting "
               "changes, but a slow/stopped target can be absorbed into the "
               "background.\n"
               "Propagation Rate (def 0.01): chance of spreading a good background "
               "sample to neighbouring pixels. Higher = fills noise/holes faster "
               "and smooths the background, but can eat small foreground.\n"
               "Hits Threshold (def 32): how many times a stored sample must be "
               "matched, over successive frames, before it is trusted as "
               "background. This is a count over TIME, independent of Num Samples "
               "(so 32 with 20 samples is fine). Higher = the model demands more "
               "evidence before accepting/replacing a background sample (more "
               "stable; slow scene changes stay flagged longer); lower = adapts "
               "more readily.\n"
               "The model is frozen automatically while paused (learning only from "
               "real movie frames).\n"
               "Additive / Restrict to Zone: as in the other detectors.";

    if (name == "BackgroundDiffKnn")
        return "Background Diff KNN\n\n"
               "K-nearest-neighbours background subtractor. Fast and robust; a "
               "good default on difficult footage.\n\n"
               "History (def 500): frames remembered; higher = more stable, slower "
               "to adapt.\n"
               "Dist2 Threshold (def 400): squared colour distance for a pixel to "
               "match the background. Lower = more sensitive (more foreground and "
               "noise); higher = stricter. Try ~100 for faint targets.\n"
               "Shadow Detection: mark shadows separately instead of foreground.\n"
               "Learning Rate (def -1 = automatic): adaptation speed. Frozen while "
               "paused.\n"
               "Additive / Restrict to Zone: as in the other detectors.";

    if (name == "ColorSegmentation")
        return "Color Segmentation\n\n"
               "Keeps pixels whose colour falls inside a range.\n\n"
               "Color Space: BGR (blue/green/red) or HSV (hue/saturation/value). "
               "HSV is usually better for picking a colour regardless of "
               "brightness.\n"
               "Min/Max per channel: the accepted range. Narrow it around the "
               "target colour; widen if the target is missed.\n"
               "Additive / Restrict to Zone: as in the other plugins.";

    if (name == "Erosion")
        return "Erosion\n\n"
               "Shrinks white regions of the mask, removing small speckle noise "
               "and thin bridges.\n\n"
               "Size (def small): radius of the erosion. Larger removes more but "
               "also eats into real objects. Pair with Dilation to clean noise "
               "while keeping object size.";

    if (name == "Dilation")
        return "Dilation\n\n"
               "Grows white regions of the mask, filling small holes and joining "
               "nearby blobs.\n\n"
               "Size (def small): radius of the dilation. Larger fills more but "
               "merges separate objects.";

    if (name == "SafeErosion")
        return "Safe Erosion\n\n"
               "Erosion that never fully removes a connected component (keeps at "
               "least a seed pixel), so small objects are not lost.\n\n"
               "Size: erosion radius.";

    if (name == "ExtractBlobs")
        return "Extract Blobs\n\n"
               "Finds connected white regions (blobs) in the mask and measures "
               "their position and size. Feeds Track Blobs and the CSV output.\n\n"
               "Min Size / Max Size (px area): only keep blobs within this size "
               "range. Raise Min Size to reject noise.\n"
               "Output File: CSV of blob position/size per frame (enable Output).";

    if (name == "GetBlobsAngles")
        return "Get Blobs Angles\n\n"
               "Computes the orientation (major-axis angle) of each blob found by "
               "Extract Blobs. No parameters.";

    if (name == "TrackBlobs")
        return "Track Blobs (Tracker)\n\n"
               "Links blobs across frames into persistent tracked entities.\n\n"
               "Max Entities: how many targets to track at once.\n"
               "Min Interdistance: minimum spacing between entities (avoids two "
               "IDs on one object).\n"
               "Max Motion/sec: largest allowed speed; limits how far an entity "
               "may jump between frames.\n"
               "Extrapolation Decay / Motion Estimator: how the tracker predicts "
               "position when a blob is briefly missing.\n"
               "Virtual Entities: keep a predicted entity alive when detection "
               "drops out (lifetime/delay control how long).\n"
               "Trail Length: number of past positions drawn on the HUD.\n"
               "Output File: CSV of tracked positions (enable Output).";

    if (name == "FrameDifference")
        return "Frame Difference\n\n"
               "Marks pixels that changed between consecutive frames (motion), "
               "either on the source image or on the current mask.\n\n"
               "Threshold (def small): minimum change to count as motion.\n"
               "Use Pipeline: difference the running mask instead of the raw "
               "frame.\n"
               "Invert / Additive / Restrict to Zone: as in the other plugins.";

    if (name == "MovingAverage")
        return "Moving Average\n\n"
               "Averages the mask over the last N frames and keeps pixels active "
               "often enough. Smooths flickering detections.\n\n"
               "Length (def frames): number of frames averaged. Longer = smoother "
               "but laggier.\n"
               "Threshold: how many of the N frames a pixel must be active to be "
               "kept.\n"
               "Centered (no time lag): a plain (trailing) average lags moving "
               "objects by about half the window. When Centered is on, the engine "
               "shows the frame Length/2 behind the pipeline, so the averaged "
               "mask lines up with the object with no lag (using future frames "
               "from the buffer). Adds Length/2 frames of latency to playback.\n"
               "Clear History: reset the accumulated frames.";

    if (name == "AdaptiveThreshold")
        return "Adaptive Threshold\n\n"
               "Thresholds each pixel against the local average, coping with "
               "uneven lighting.\n\n"
               "Block Size (odd): neighbourhood used for the local average. Larger "
               "= smoother, less local.\n"
               "Constant (C): offset subtracted from the local mean; raise to "
               "reject more (cleaner), lower to keep more.\n"
               "Threshold Method: Mean or Gaussian weighting of the neighbourhood.\n"
               "Additive / Invert / Restrict to Zone: as in the other plugins.";

    if (name == "Clahe")
        return "CLAHE (Contrast Limited Adaptive Histogram Equalisation)\n\n"
               "Enhances local contrast in place (on the lightness channel). Makes "
               "faint detail pop for viewing and for threshold-based detectors.\n\n"
               "Clip Limit (def 3.0): contrast strength; higher = punchier but "
               "amplifies noise.\n"
               "Tile Size (def 8): grid of local regions; smaller tiles = more "
               "local contrast.";

    if (name == "WhiteBalance")
        return "White Balance\n\n"
               "Removes colour casts in place.\n\n"
               "Mode: Gray World / Simple (automatic), Manual (per-channel gains, "
               "set with the Pick White tool by clicking a neutral pixel), or "
               "Underwater (restores the red channel lost in water).\n"
               "Saturation Threshold (Gray World): ignore over-saturated pixels "
               "when estimating the cast.\n"
               "Gains (Manual): per-channel multipliers (1 = unchanged).\n"
               "Strength (Underwater): amount of red compensation.";

    if (name == "Denoise")
        return "Denoise\n\n"
               "Reduces image noise in place (single frame).\n\n"
               "Method: Bilateral (fast, edge-preserving smoothing) or NL-means "
               "(slower, cleaner on textured noise).\n"
               "Bilateral - Diameter (def 5): neighbourhood size. Sigma Color "
               "(def 50): how similar colours must be to blur together (higher = "
               "stronger). Sigma Space (def 50): spatial reach.\n"
               "NL-means - Strength / Color Strength (def 3): filter power; higher "
               "removes more noise but softens detail. CPU-heavy.";

    if (name == "TemporalDenoise")
        return "Temporal Denoise\n\n"
               "Multi-frame NL-means denoising: reduces noise using several "
               "consecutive frames, which cleans much harder than a single-frame "
               "filter without blurring moving objects (patches are matched, not "
               "averaged). Applied in place.\n\n"
               "Window (frames, def 5, odd): how many frames are used. It denoises "
               "the CENTRE frame of the window, so the output is delayed by half "
               "the window (e.g. 2 frames for a window of 5) - but with no "
               "positional lag on moving targets.\n"
               "Strength / Color Strength (def 3): filter power; higher removes "
               "more noise but softens detail.\n"
               "Note: NL-means is CPU-heavy, so playback will be slow - this is "
               "best used for offline analysis.";

    if (name == "Sharpen")
        return "Sharpen\n\n"
               "Unsharp mask: adds back the difference between the image and a "
               "blurred copy, boosting edges. Applied in place; best after a "
               "denoise stage (otherwise it amplifies noise).\n\n"
               "Amount (def 1.0): sharpening strength. 0 = none; large = harsh, "
               "may create halos.\n"
               "Radius (def 2.0): blur size; small enhances fine detail, large "
               "enhances coarse structure.";

    if (name == "Dehaze")
        return "Dehaze (Dark Channel Prior)\n\n"
               "Estimates and removes the veiling haze / backscatter that washes "
               "out contrast in turbid water or fog. Applied in place. Combine "
               "with Underwater White Balance for the cast.\n\n"
               "Strength (omega, def 0.95): how much haze to remove. Higher = "
               "punchier but can look unnatural or over-dark.\n"
               "Patch Size (def 15): scale of the local estimate. Larger = smoother "
               "transmission, less local detail.\n"
               "Min Transmission (def 0.1): floor on the recovered transmission; "
               "raise it if very hazy regions turn noisy/over-corrected.";

    if (name == "Curves")
        return "Curves\n\n"
               "Per-channel tone/colour curves applied in place. One tool for "
               "levels, gamma, contrast and colour balance.\n\n"
               "Channel: Master affects all channels; Red/Green/Blue are applied "
               "on top of Master.\n"
               "Editor: click to add a point, drag to move it, right-click to "
               "remove. A straight diagonal leaves the image unchanged; an "
               "S-shape adds contrast; lifting the dark end raises shadows.";

    if (name == "ZonesOfInterest")
        return "Zones of Interest\n\n"
               "Defines which parts of the frame are processed, as polygons drawn "
               "on the video and/or a zone-mask image.\n\n"
               "Select this plugin, then on the video: click to add points, drag "
               "to move, click the first point (or right-click) to close a "
               "polygon, right-click a point to delete it.\n"
               "Region number: 0 and un-painted areas are ignored by downstream "
               "plugins; a plugin's Restrict to Zone keeps only one region.\n"
               "New polygons default to region 1; change a selected polygon's "
               "region below. Save/Load stores the polygons in a text file.";

    if (name == "YoloDetector")
        return "Yolo Detector\n\n"
               "Runs an ONNX YOLO model (v5/v8/v11) on the CPU or GPU.\n\n"
               "Model / Class Names: the .onnx file and an optional class-name "
               "list. No model ships with useTracker.\n"
               "Model Type: output layout; Auto suits standard exports.\n"
               "Compute Target: CPU, OpenCL or Vulkan (falls back to CPU if the "
               "GPU cannot run the model).\n"
               "Input Size: network resolution; larger = more accurate, slower.\n"
               "Confidence: minimum score to keep a detection.\n"
               "NMS Threshold: overlap above which duplicate boxes are merged.\n"
               "Class Filter: comma-separated names/ids to keep (empty = all).\n"
               "Detections are drawn on the HUD, written to CSV and stamped into "
               "the mask for Extract Blobs / trackers.";

    if (name == "PatternTracker")
        return "Pattern Tracker\n\n"
               "Follows a target by matching its appearance in a local window "
               "each frame. Seed it by clicking on the video or from detected "
               "blobs (place it after Extract Blobs).\n\n"
               "Backend: Template match (local cross-correlation) or CSRT "
               "(correlation-filter tracker, handles scale better).\n"
               "Search Distance: how far the target may move between frames.\n"
               "Template Size: box size for click-seeded targets.\n"
               "Match Threshold: below this correlation the target is lost.\n"
               "Update Threshold / Rate: adapt the template only when confident, "
               "to follow appearance changes without drifting.\n"
               "Predict position / Max Lost Frames / Max Targets / Trail Length: "
               "search prediction, drop-out tolerance, count and HUD trail.\n"
               "Seed from detected blobs: auto-create targets from blobs above a "
               "minimum size.";

    if (name == "RecordVideo")
        return "Record Video\n\n"
               "Encodes the video to an H.264 file (records the original frame, "
               "not the enhanced/annotated view). Enable Output and the REC "
               "button.\n\n"
               "Output File: destination (.mp4/.avi/.mkv).\n"
               "Preset: x264 speed/quality trade-off (e.g. fast, medium, slow).\n"
               "Bitrate (kbps): higher = better quality, larger file.";

    if (name == "RecordPixels")
        return "Record Pixels\n\n"
               "Writes per-pixel data of the mask to a CSV file over time. Enable "
               "Output.\n\nOutput File: destination CSV.";

    if (name == "SimpleTags")
        return "Simple Tags\n\n"
               "Detects simple rectangular coded tags.\n\n"
               "Tag Width / Height: the tag's cell grid size.\n"
               "Output File: CSV of detected tag ids/positions.";

    if (name == "TakeSnapshots")
        return "Take Snapshots\n\n"
               "Saves image snapshots of the mask while Output is enabled.\n\n"
               "Output Dir/Pattern: folder (a frame number is appended).";

    if (name == "Stopwatch")
        return "Stopwatch\n\n"
               "Logs timed events triggered by keyboard shortcuts to a CSV file.\n\n"
               "Output File: destination CSV of event times.";

    if (name == "RemoteControl")
        return "Remote Control\n\n"
               "Bluetooth remote control input (Linux, if built with Bluetooth).\n\n"
               "Corner: screen corner used for the on-screen control overlay.";

    if (name == "ArucoColor")
        return "Aruco Color\n\n"
               "Detects custom colour ArUco-style markers.\n\n"
               "Marker Cols/Rows: marker grid dimensions.\n"
               "Saturation/Value Threshold: colour segmentation of the marker.\n"
               "AT Block Size / Constant: adaptive threshold for the black grid.\n"
               "Min/Max Marker Area: accepted marker size range.\n"
               "Max Hue Deviation / Marker Range: colour and search tolerances.\n"
               "Dictionary / Reference Hues: marker code set and colour codes.\n"
               "Output File: CSV of detected markers.";

    if (name == "Aruco")
        return "Aruco\n\n"
               "Detects standard ArUco markers.\n\n"
               "Min/Max Size: accepted marker size range.\n"
               "Threshold 1/2: adaptive threshold parameters.\n"
               "Mask Shape/Radius/Perspective Shift/Value: optional masking of the "
               "detected marker.\n"
               "Output File: CSV of detected markers.";

    return "No help available for this plugin yet.";
}

void AppGui::DrawPluginDialog(int index)
{
    if (index < 0 || ipEngine.pipelines.empty()) return;
    if (index >= (int)ipEngine.pipelines[0].plugins.size()) return;

    // Get the plugin pointer from the first available pipeline
    PipelinePlugin* pp = ipEngine.pipelines[0].plugins[index];
    if (!pp) pp = ipEngine.pipelines[ipEngine.threadsCount].plugins[index];
    if (!pp) return;

    std::string title = PluginTitle(pp->registryName) +
                        " / stage " + std::to_string(index+1) + "###PluginDlg" + std::to_string(index);
    bool open = pipelineDialogOpen[index];

    // per-dialog help-panel state
    if ((int)pipelineHelpOpen.size() < (int)pipelineDialogOpen.size())
        pipelineHelpOpen.resize(pipelineDialogOpen.size(), false);
    bool helpOpen = (index < (int)pipelineHelpOpen.size()) ? (bool)pipelineHelpOpen[index] : false;

    // Place new dialogs inside the work area, staggered so several don't stack
    // exactly. Without this the saved/default position can land off-screen when
    // the main window is small (not maximized), leaving the dialog invisible.
    ImGuiViewport* dvp = ImGui::GetMainViewport();
    ImVec2 workMin = dvp->WorkPos;
    ImVec2 workMax = ImVec2(dvp->WorkPos.x + dvp->WorkSize.x,
                            dvp->WorkPos.y + dvp->WorkSize.y);
    float stagger = 32.0f * dpiScale * (index % 6);
    ImGui::SetNextWindowPos(ImVec2(workMin.x + 80*dpiScale + stagger,
                                   workMin.y + 80*dpiScale + stagger),
                            ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(520*dpiScale, 480*dpiScale), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSizeConstraints(ImVec2(300*dpiScale,200*dpiScale),dvp->WorkSize);
    if (!ImGui::Begin(title.c_str(), &open))
    {
        pipelineDialogOpen[index] = open;
        ImGui::End();
        return;
    }
    pipelineDialogOpen[index] = open;

    // Clamp the dialog back into the work area if it (or the main window) has
    // moved/shrunk so part of it would be off-screen — keeps it reachable.
    {
        ImVec2 pos = ImGui::GetWindowPos();
        ImVec2 sz  = ImGui::GetWindowSize();
        ImVec2 cl  = pos;
        if (cl.x + sz.x > workMax.x) cl.x = workMax.x - sz.x;
        if (cl.y + sz.y > workMax.y) cl.y = workMax.y - sz.y;
        if (cl.x < workMin.x) cl.x = workMin.x;
        if (cl.y < workMin.y) cl.y = workMin.y;
        if (cl.x != pos.x || cl.y != pos.y)
            ImGui::SetWindowPos(cl);
    }

    // help toggle button, right-aligned at the top of the dialog. Shows "?"
    // when closed, and a fold-away arrow when the help panel is open.
    {
        float bw = ImGui::GetFrameHeight();
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() +
                             ImGui::GetContentRegionAvail().x - bw);
        bool toggled = helpOpen
            ? ImGui::ArrowButton("##help", ImGuiDir_Right)
            : ImGui::Button("?##help", ImVec2(bw, 0));
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip(helpOpen ? "Hide parameter help" : "Show parameter help");
        if (toggled)
        {
            helpOpen = !helpOpen;
            // widen the dialog for the help panel, or shrink it back when folding
            ImGui::SetWindowSize(ImVec2(std::min((helpOpen ? 900.0f : 520.0f) * dpiScale, dvp->WorkSize.x),
                                        ImGui::GetWindowHeight()));
        }
        if (index < (int)pipelineHelpOpen.size())
            pipelineHelpOpen[index] = helpOpen;
    }

    // left column holds all the controls; help panel (if open) sits on the right
    if (helpOpen)
        ImGui::BeginChild("##ctrls", ImVec2(std::min(460*dpiScale,ImGui::GetContentRegionAvail().x*.57f), 0), false);

    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x*.52f);

    // Common controls
    bool isActive = pp->active;
    if (ImGui::Checkbox("Active", &isActive))
    {
        for (unsigned int p = 0; p <= ipEngine.threadsCount; p++)
            if (ipEngine.pipelines[p].plugins[index])
                ipEngine.pipelines[p].plugins[index]->active = isActive;
        pipelineDirty = true;
    }

    ImGui::SameLine();
    bool isOutput = pp->output;
    if (ImGui::Checkbox("Output", &isOutput))
    {
        for (unsigned int p = 0; p <= ipEngine.threadsCount; p++)
            if (ipEngine.pipelines[p].plugins[index])
                ipEngine.pipelines[p].plugins[index]->output = isOutput;
        pipelineDirty = true;
    }

    TrackerUI::Hint("Include this stage when Write outputs is enabled in the bottom bar.");

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
                ImGui::TextColored(TrackerUI::Colors.Accent,
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

    // --- Denoise (single threaded, no sync needed) ---
    else if (Denoise* p = dynamic_cast<Denoise*>(pp))
    {
        ImGui::TextWrapped("Spatial noise reduction, applied in place.");
        ImGui::Spacing();

        int method = p->method;
        const char* methods[] = { "Bilateral (fast)", "NL-means (slow, cleaner)" };
        if (ImGui::Combo("Method", &method, methods, 2))
        {
            p->method = method;
            changed = true;
        }

        if (p->method == Denoise::BILATERAL)
        {
            changed |= ImGui::SliderInt("Diameter", &p->diameter, 1, 25);
            changed |= ImGui::SliderFloat("Sigma Color", &p->sigmaColor, 1.0f, 150.0f, "%.0f");
            changed |= ImGui::SliderFloat("Sigma Space", &p->sigmaSpace, 1.0f, 150.0f, "%.0f");
        }
        else
        {
            changed |= ImGui::SliderFloat("Strength", &p->nlmStrength, 1.0f, 30.0f, "%.1f");
            changed |= ImGui::SliderFloat("Color Strength", &p->nlmColorStrength, 1.0f, 30.0f, "%.1f");
            ImGui::TextDisabled("NL-means is CPU-heavy; expect slow playback.");
        }
    }

    // --- TemporalDenoise (single threaded, no sync needed) ---
    else if (TemporalDenoise* p = dynamic_cast<TemporalDenoise*>(pp))
    {
        ImGui::TextWrapped("Multi-frame (temporal) noise reduction, in place. "
                           "Denoises the centre frame of a rolling window using "
                           "the frames around it. Result is delayed by half the "
                           "window but has no motion lag.");
        ImGui::Spacing();
        changed |= ImGui::SliderInt("Window (frames)", &p->windowSize, 3, 11);
        if (p->windowSize % 2 == 0) p->windowSize += 1;
        changed |= ImGui::SliderFloat("Strength", &p->strength, 1.0f, 30.0f, "%.1f");
        changed |= ImGui::SliderFloat("Color Strength", &p->colorStrength, 1.0f, 30.0f, "%.1f");
        ImGui::TextDisabled("CPU-heavy; expect slow playback. Best offline.");
    }

    // --- Sharpen (single threaded, no sync needed) ---
    else if (Sharpen* p = dynamic_cast<Sharpen*>(pp))
    {
        ImGui::TextWrapped("Unsharp-mask sharpening, applied in place. Best "
                           "after a denoise stage.");
        ImGui::Spacing();
        changed |= ImGui::SliderFloat("Amount", &p->amount, 0.0f, 4.0f, "%.2f");
        changed |= ImGui::SliderFloat("Radius", &p->radius, 0.5f, 10.0f, "%.1f");
    }

    // --- Dehaze (single threaded, no sync needed) ---
    else if (Dehaze* p = dynamic_cast<Dehaze*>(pp))
    {
        ImGui::TextWrapped("Dark Channel Prior dehazing, applied in place. "
                           "Removes veiling haze / backscatter. Pairs well with "
                           "Underwater White Balance.");
        ImGui::Spacing();
        changed |= ImGui::SliderFloat("Strength (omega)", &p->omega, 0.0f, 1.0f, "%.2f");
        changed |= ImGui::SliderInt("Patch Size", &p->patchSize, 3, 41);
        changed |= ImGui::SliderFloat("Min Transmission", &p->t0, 0.01f, 0.5f, "%.2f");
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
            ImGui::TextColored(TrackerUI::Colors.Accent,
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
                           "local window each frame. The bounding box is taken "
                           "from the foreground mask blob (needs a background "
                           "subtractor upstream) and adapts to its size over time.");
        ImGui::Spacing();

        // seed by clicking on the video
        bool seeding = (patternSeedPluginIndex == index);
        if (ImGui::Checkbox("Click on video to add targets", &seeding))
            patternSeedPluginIndex = seeding ? index : -1;
        if (seeding)
            ImGui::TextColored(TrackerUI::Colors.Accent,
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

        changed |= ImGui::Checkbox("Fit box to mask blob", &p->fitToMask);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Take the box from the foreground blob instead of a\n"
                              "fixed square. Seeds (click or auto) and the per-frame\n"
                              "box are derived from the mask pixels.");
        if (p->fitToMask)
        {
            changed |= ImGui::SliderFloat("Size Adapt Rate", &p->sizeAdaptRate, 0.0f, 1.0f, "%.2f");
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("How fast the box grows/shrinks toward the blob.\n"
                                  "0 = frozen size, 1 = snap instantly. Low values\n"
                                  "keep the box steady when the blob flickers.");
        }

        if (ImGui::InputInt(p->fitToMask ? "Fallback Size (px)" : "Template Size (px)",
                            &p->templateSize))
        {
            if (p->templateSize < 8) p->templateSize = 8;
            changed = true;
        }
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip(p->fitToMask
                ? "Square box size used only when a click lands off every blob"
                : "Box side for seeded targets");

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
        changed |= ImGui::InputInt("Min Blob Size", &p->minBlobSeedSize);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Minimum mask-blob area (px). Applies all the time:\n"
                              "used to seed new targets AND, each frame, a target\n"
                              "whose blob falls below this is treated as undetected\n"
                              "(so noise cannot keep a dead target alive).\n"
                              "Requires 'Fit box to mask'.");

        changed |= ImGui::Checkbox("Seed from YOLO", &p->seedFromYolo);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Auto-create targets from a YOLO Detector placed\n"
                              "earlier in the pipeline, one per detection above the\n"
                              "confidence threshold (not already tracked).");
        if (p->seedFromYolo)
            changed |= ImGui::SliderFloat("Min Confidence", &p->yoloConfidence, 0.0f, 1.0f, "%.2f");

        ImGui::Separator();
        changed |= ImGui::InputInt("Confirm after N detections", &p->confirmDetections);
        if (p->confirmDetections < 1) p->confirmDetections = 1;
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("A new target is a candidate (orange) until it has been\n"
                              "detected on this many consecutive frames, then it goes\n"
                              "live (green). Candidates are not stamped into the mask\n"
                              "or written to output, and a candidate that misses a\n"
                              "frame is discarded. 1 = live immediately.");
        changed |= ImGui::Checkbox("Show candidates", &p->showCandidates);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Draw unconfirmed candidate targets (orange) on the HUD.");

        changed |= ImGui::Checkbox("Merge overlapping", &p->mergeOverlapping);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("When two targets overlap (a blob that split then\n"
                              "rejoined), drop the weaker one so a single target\n"
                              "survives. Keeps the confirmed / older / better-tracked one.");
        if (p->mergeOverlapping)
        {
            changed |= ImGui::SliderFloat("Merge Overlap (IoU)", &p->mergeOverlap, 0.05f, 1.0f, "%.2f");
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("Box overlap (intersection-over-union) above which\n"
                                  "two targets are treated as the same and merged.");
            changed |= ImGui::InputInt("Merge Delay (frames)", &p->mergeDelay);
            if (p->mergeDelay < 0) p->mergeDelay = 0;
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("The overlap must persist for this many frames before\n"
                                  "the weaker target is merged away. Prevents merging two\n"
                                  "distinct targets that only briefly cross. 0 = immediate.");
        }

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
            p->SetSize(std::max(1, size));   // kernel radius must be >= 1
            changed = true;
        }
    }

    // --- Erosion ---
    else if (Erosion* p = dynamic_cast<Erosion*>(pp))
    {
        int size = p->size;
        if (ImGui::InputInt("Size", &size))
        {
            p->SetSize(std::max(1, size));   // kernel radius must be >= 1
            changed = true;
        }
    }

    // --- SafeErosion ---
    else if (SafeErosion* p = dynamic_cast<SafeErosion*>(pp))
    {
        if (ImGui::InputInt("Size", &p->size))
            p->size = std::max(1, p->size);
        changed = true;
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

        if (ImGui::Checkbox("Centered (no time lag)", &p->centered))
            changed = true;
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Delay the displayed frame by half the window so "
                              "the averaged mask is centered in time (no lag). "
                              "Needs the frame buffer / adds Length/2 latency.");

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
            ImGui::TextColored(TrackerUI::Colors.Green,
                               "Editing active on the video:");
        else
            ImGui::TextColored(TrackerUI::Colors.Accent,
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

    // close the controls column and draw the help panel on the right
    ImGui::PopItemWidth();
    if (helpOpen)
    {
        ImGui::EndChild();
        ImGui::SameLine();
        ImGui::BeginChild("##help", ImVec2(0, 0), true);
        ImGui::TextWrapped("%s", PluginHelpText(pp->registryName));
        ImGui::EndChild();
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
    ipEngine.CloseOutput();
    output = false;
    pendingRewind = false;
    bookmarks.clear();
    loopStart = loopEnd = -1;
    loopEnabled = false;
    rulerMeasurements.clear();
    rulerAnchored = false;
    patternSeedPluginIndex = whitePickPluginIndex = -1;
    roiEditing = false;
    roiActivePolygon = roiSelectedPolygon = roiDragPoly = roiDragPoint = -1;
    zoomStartX = zoomStartY = 0;
    zoomEndX = zoomEndY = 1;
    videoSliderPos = 0;
    sliderMoving = false;
    playSpeed = 0;
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

    ipEngine.RefreshCurrentFrame();
    SyncHudSize();
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
    if (!testMode && pfd::settings::available())
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
    fileBrowser.focusName = mode == FileBrowser::SAVE;

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
        ImGui::TextColored(TrackerUI::Colors.Red, "%s", fileBrowser.error.c_str());
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
    quitConfirmFocus = 1;  // default focus on Cancel
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

        // Keyboard is handled manually here (global ImGui nav is off so it does
        // not swallow playback shortcuts). Tab / Left / Right move between the
        // two buttons, Enter activates the focused one, Escape cancels.
        bool doQuit = false, doCancel = false;
        if (!quitConfirmJustOpened)
        {
            if (ImGui::IsKeyPressed(ImGuiKey_LeftArrow) ||
                ImGui::IsKeyPressed(ImGuiKey_RightArrow) ||
                ImGui::IsKeyPressed(ImGuiKey_Tab))
                quitConfirmFocus ^= 1;
            if (ImGui::IsKeyPressed(ImGuiKey_Enter) ||
                ImGui::IsKeyPressed(ImGuiKey_KeypadEnter))
            {
                if (quitConfirmFocus == 0) doQuit = true; else doCancel = true;
            }
            if (ImGui::IsKeyPressed(ImGuiKey_Escape))
                doCancel = true;
        }

        // draw a button, highlighting it when it holds the keyboard focus
        auto focusButton = [&](const char* label, int which) -> bool
        {
            bool focused = (quitConfirmFocus == which);
            if (focused)
            {
                ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_ButtonHovered));
                ImGui::PushStyleColor(ImGuiCol_Border, ImGui::GetStyleColorVec4(ImGuiCol_NavHighlight));
                ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.0f * dpiScale);
            }
            bool clicked = ImGui::Button(label, ImVec2(120 * dpiScale, 0));
            if (ImGui::IsItemHovered()) quitConfirmFocus = which;
            if (focused)
            {
                ImGui::PopStyleVar();
                ImGui::PopStyleColor(2);
            }
            return clicked;
        };

        if (focusButton("Quit", 0))   doQuit = true;
        ImGui::SameLine();
        if (focusButton("Cancel", 1)) doCancel = true;

        if (doQuit)
        {
            running = false;
            showQuitConfirm = false;
            ImGui::CloseCurrentPopup();
        }
        else if (doCancel)
        {
            showQuitConfirm = false;
            ImGui::CloseCurrentPopup();
        }
        quitConfirmJustOpened = false;

        ImGui::EndPopup();
    }
}

// ============================================================================
// Test harness — drive the GUI from a script and take screenshots
// ============================================================================

// map a key name to (ImGuiKey, SDL keycode, isShortcut)
static bool MapTestKey(const std::string& name, ImGuiKey& ik, SDL_Keycode& sk, bool& shortcut)
{
    shortcut = false;
    if (name.size()==1 && name[0]>='a' && name[0]<='z')
    {
        ik=(ImGuiKey)(ImGuiKey_A+name[0]-'a'); sk=SDLK_a+name[0]-'a';
        shortcut=true; return true;
    }
    if (name == "space")  { ik = ImGuiKey_Space;      sk = SDLK_SPACE;  shortcut = true; return true; }
    if (name == "left")   { ik = ImGuiKey_LeftArrow;  sk = SDLK_LEFT;   shortcut = true; return true; }
    if (name == "right")  { ik = ImGuiKey_RightArrow; sk = SDLK_RIGHT;  shortcut = true; return true; }
    if (name == "up")     { ik = ImGuiKey_UpArrow;    sk = SDLK_UP;     return true; }
    if (name == "down")   { ik = ImGuiKey_DownArrow;  sk = SDLK_DOWN;   return true; }
    if (name == "enter")  { ik = ImGuiKey_Enter;      sk = SDLK_RETURN; return true; }
    if (name == "tab")    { ik = ImGuiKey_Tab;        sk = SDLK_TAB;    return true; }
    if (name == "escape") { ik = ImGuiKey_Escape;     sk = SDLK_ESCAPE; return true; }
    if (name == "backspace") { ik = ImGuiKey_Backspace; sk = SDLK_BACKSPACE; shortcut = true; return true; }
    return false;
}

void AppGui::LoadTestScript(const std::string& file)
{
    std::ifstream f(file);
    if (!f.is_open())
    {
        std::cerr << "Could not open test script " << file << std::endl;
        return;
    }
    std::string line;
    while (std::getline(f, line))
    {
        size_t h = line.find('#');
        if (h != std::string::npos) line = line.substr(0, h);
        // trim
        size_t a = line.find_first_not_of(" \t\r\n");
        if (a == std::string::npos) continue;
        testScript.push_back(line);
    }
    testMode = true;
    std::cerr << "Test mode: " << testScript.size() << " commands loaded" << std::endl;
}

void AppGui::TestAdvance()
{
    if (!testMode) return;

    // release an injected click after it has been held a couple of frames
    if (testReleaseIn > 0)
    {
        if (--testReleaseIn == 0) { testLeftDown = false; testRightDown = false; }
        return;
    }
    if (testWaitFrames > 0) { testWaitFrames--; return; }

    while (testPc < testScript.size())
    {
        std::istringstream ss(testScript[testPc++]);
        std::string op; ss >> op;

        if (op == "save-ui" || op == "load-ui")
        {
            std::string path; std::getline(ss >> std::ws,path);
            if (op == "save-ui") ImGui::SaveIniSettingsToDisk(path.c_str());
            else
            {
                ImGui::LoadIniSettingsFromDisk(path.c_str());
                pendingScaleChange=pendingThemeChange=true;
            }
            return;
        }

        if (op == "resize")
        {
            int w=1280, h=800; ss >> w >> h;
            SDL_SetWindowSize(window,w,h);
            return;
        }
        if (op == "ui-scale")
        {
            float scale=1; ss >> scale;
            dpiScale=std::clamp(scale,.75f,2.f); pendingScaleChange=true;
            return;
        }
        if (op == "text")
        {
            std::string text; std::getline(ss >> std::ws,text);
            ImGui::GetIO().AddInputCharactersUTF8(text.c_str());
            return;
        }
        if (op == "expect")
        {
            std::string property, expected, actual; ss >> property;
            std::getline(ss >> std::ws,expected);
            if (property == "pipeline")
            {
                for (int i=0; auto* p=PluginAt(i); ++i)
                { if (i) actual+=' '; actual+=p->registryName; }
            }
            else if (property == "selected")
                actual=PluginAt(selectedPipelineItem) ? PluginAt(selectedPipelineItem)->registryName : "none";
            else if (property == "playing") actual=play ? "true" : "false";
            else if (property == "output") actual=output ? "true" : "false";
            else if (property == "frame") actual=std::to_string(ipEngine.GetPresentFrameNumber());
            else if (property == "source") actual=HasSource() ? "loaded" : "empty";
            else if (property == "theme")
            {
                bool appliedDark=ImGui::GetStyleColorVec4(ImGuiCol_WindowBg).x<.5f;
                actual=appliedDark!=darkMode ? "pending" : (darkMode ? "dark" : "light");
            }
            else if (property == "error") actual=errorMessage.empty() ? "none" : "present";
            else if (property == "view")
            {
                const char* names[]={"pipeline","background","calibration","timing"};
                actual=names[activeTab];
            }
            else if (property == "preview")
            {
                cv::Mat frame=ipEngine.GetPresentImage();
                cv::Scalar mean=frame.empty() ? cv::Scalar() : cv::mean(frame);
                actual=mean[0]+mean[1]+mean[2]>3 ? "visible" : "black";
            }
            else if (property == "zoom") actual=zoomEndX-zoomStartX<.999f ? "zoomed" : "fit";
            else { actual="unknown property"; }
            if (actual!=expected)
            {
                std::cerr << "GUI CHECK FAILED: " << property << ": expected [" << expected
                          << "], got [" << actual << "]" << std::endl;
                testFailed=true; running=false; return;
            }
            std::cerr << "GUI check passed: " << property << " = " << actual << std::endl;
            continue;
        }

        if (op == "move")
        {
            ss >> testMouseX >> testMouseY;
            return; // allow hover to settle before clicking overlap-aware tabs
        }
        if (op == "scroll")  // test aid: inject a mouse-wheel delta (zoom)
        {
            float w = 0.0f; ss >> w; testWheel = w;
            return;
        }
        if (op == "press")   // test aid: hold a mouse button (for drags)
        {
            std::string btn; ss >> btn;
            if (btn == "right") testRightDown = true; else testLeftDown = true;
            return;
        }
        if (op == "release") // test aid: release held mouse buttons
        {
            testLeftDown = false; testRightDown = false;
            return;
        }
        if (op == "scale")   // test aid: set the input downscale directly
        {
            float s = 1.0f; ss >> s;
            ipEngine.SetInputScale(s);
            SyncHudSize();
            pipelineDirty = true;
            return;
        }
        if (op == "click")
        {
            std::string btn; ss >> btn;
            if (btn == "right") testRightDown = true; else testLeftDown = true;
            testReleaseIn = 3;
            return;
        }
        if (op == "wait")
        {
            int n = 1; ss >> n; testWaitFrames = std::max(1, n);
            return;
        }
        if (op == "shot")
        {
            std::string path; ss >> path; testShotPath = path;
            return; // captured after this frame renders
        }
        if (op == "key")
        {
            std::string k; ss >> k;
            bool ctrl = false;
            if (k == "ctrl") { ctrl = true; ss >> k; }
            ImGuiKey ik; SDL_Keycode sk; bool sc;
            if (MapTestKey(k, ik, sk, sc))
            {
                testKey = ik; testKeyReleaseIn = 2; testKeyCtrl=ctrl;
                ImGuiIO& io = ImGui::GetIO();
                if (sc && !io.WantTextInput && !fileBrowser.visible && !showQuitConfirm &&
                    !ImGui::IsAnyItemActive() && !ImGui::IsPopupOpen(nullptr, ImGuiPopupFlags_AnyPopupId | ImGuiPopupFlags_AnyPopupLevel))
                    HandleShortcut(sk, ctrl);
            }
            testWaitFrames = 1;
            return;
        }
        if (op == "quit")
        {
            running = false; testMode = false;
            return;
        }
    }

    // script exhausted: quit cleanly
    if (testPc >= testScript.size() && testShotPath.empty())
        running = false;
}

void AppGui::TestInjectInput()
{
    ImGuiIO& io = ImGui::GetIO();
    io.AddMousePosEvent(testMouseX, testMouseY);
    io.AddMouseButtonEvent(0, testLeftDown);
    io.AddMouseButtonEvent(1, testRightDown);
    if (testWheel != 0.0f) { io.AddMouseWheelEvent(0.0f, testWheel); testWheel = 0.0f; }

    if (testKey >= 0)
    {
        io.AddKeyEvent(ImGuiMod_Ctrl,testKeyCtrl);
        io.AddKeyEvent((ImGuiKey)testKey, true);
        if (testKeyReleaseIn > 0 && --testKeyReleaseIn == 0)
        {
            io.AddKeyEvent((ImGuiKey)testKey, false);
            io.AddKeyEvent(ImGuiMod_Ctrl,false);
            testKey = -1;
        }
    }
}

void AppGui::CaptureScreenshot(const std::string& path)
{
    int w = 0, h = 0;
    SDL_GL_GetDrawableSize(window, &w, &h);
    if (w <= 0 || h <= 0) return;

    std::vector<unsigned char> buf((size_t)w * h * 4);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadBuffer(GL_BACK);
    glReadPixels(0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, buf.data());

    cv::Mat img(h, w, CV_8UC4, buf.data());
    cv::Mat flipped, bgr;
    cv::flip(img, flipped, 0);              // GL origin is bottom-left
    cv::cvtColor(flipped, bgr, cv::COLOR_RGBA2BGR);
    cv::imwrite(path, bgr);
    std::cerr << "Test screenshot: " << path << " (" << w << "x" << h << ")" << std::endl;
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

// Create the HUD at the SOURCE resolution, independent of the input downscale,
// so plugin-drawn overlays (boxes, labels) keep a fixed on-screen size and font
// regardless of the processing scale. Plugins draw at source coordinates (they
// scale their processing-space positions by GetOutputScale()).
void AppGui::SyncHudSize()
{
    if (!ipEngine.capture) return;
    int w = ipEngine.capture->width;
    int h = ipEngine.capture->height;
    if (w <= 0 || h <= 0) return;
    hud.create(h, w, CV_8UC4);
    hudApp.create(h, w, CV_8UC4);
    ipEngine.hud = hud;
}

void AppGui::ResetEngine()
{
    // Regenerate HUD
    if (ipEngine.capture)
    {
        ipEngine.Reset();
        SyncHudSize();
    }
}

void AppGui::ResetEngine(Parameters& params)
{
    ipEngine.CloseOutput();
    output = false;
    play = false;
    if (ipEngine.capture) ipEngine.capture->Pause();
    pendingRewind = false;
    ipEngine.Reset(params);

    if (ipEngine.capture)
    {
        SyncHudSize();
        ipEngine.takeSnapshot = true;
    }

    // Clean up pipeline dialogs
    pipelineDialogOpen.clear();
    pipelineHelpOpen.clear();
    selectedPipelineItem = -1;
    patternSeedPluginIndex = whitePickPluginIndex = -1;
    roiEditing = false;

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
    ipEngine.RefreshCurrentFrame();
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
        pipelineHelpOpen.push_back(false);
        selectedPipelineItem = (int)pipelineDialogOpen.size()-1;
    }
    else
    {
        ipEngine.Insert(pos, pfv, true);
        pipelineDialogOpen.insert(pipelineDialogOpen.begin() + pos, false);
        pipelineHelpOpen.insert(pipelineHelpOpen.begin() + pos, false);
        selectedPipelineItem = pos;
    }

    pipelineDirty = true;
    scrollToPipelineSelection = true;
    ipEngine.takeSnapshot = true;

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
