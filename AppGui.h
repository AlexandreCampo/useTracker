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

#ifndef APPGUI_H
#define APPGUI_H

#include <SDL.h>
#include <GL/gl.h>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <filesystem>

#include <opencv2/opencv.hpp>

#include "ImageProcessingEngine.h"
#include "Parameters.h"

// Fallback in-app file browser, used when no native dialog backend
// (zenity/kdialog/...) is available on the system
struct FileBrowser
{
    enum Mode { OPEN, SAVE, FOLDER };

    bool visible = false;
    Mode mode = OPEN;
    std::string title;
    std::vector<std::string> extensions; // lowercase, with dot; empty = show all
    std::function<void(const std::string&)> onSelect;
    std::filesystem::path dir;
    std::vector<std::pair<std::string, bool>> entries; // name, isDirectory
    char nameBuf[512] = {0};
    std::string error;
    int selected = -1;            // highlighted row (keyboard)
    bool scrollToSelected = false;
    bool focusName = false;       // request keyboard focus on the filename field
};

class AppGui
{
public:
    AppGui();
    ~AppGui();

    int Run();

private:
    // SDL / GL
    SDL_Window* window = nullptr;
    SDL_GLContext gl_context = nullptr;
    int windowWidth = 1280;
    int windowHeight = 800;
    float dpiScale = 1.0f;

    // width of the right-hand control panel (in unscaled px; *dpiScale applied
    // at use). Draggable via the splitter between the video area and the panel.
    float controlPanelWidth = 380.0f;

    // input-downscale slider state: the widget edits downscaleUI live while
    // dragging and only applies the (heavy) rebuild on release
    float downscaleUI = 1.0f;
    bool  draggingScale = false;

    // ruler / measure tool: draw lines or rectangles over the video and read
    // their size in pixels, to help tune pixel-valued parameters
    bool rulerActive = false;
    int  rulerShape = 0;            // 0 = line, 1 = rectangle
    bool rulerAnchored = false;     // first point placed, waiting for the second
    cv::Point2f rulerP1;
    struct Measurement { cv::Point2f p1, p2; int shape; };
    std::vector<Measurement> rulerMeasurements;

    // timeline markers: bookmarks (reference points) and a loop region
    std::vector<double> bookmarks;     // times in seconds
    double loopStart = -1.0;           // loop region start (s), -1 = unset
    double loopEnd   = -1.0;           // loop region end (s), -1 = unset
    bool   loopEnabled = false;        // loop playback between loopStart/loopEnd
    void AddLoopPoint(double t);       // sets start, then end, then restarts

    // Video texture
    GLuint videoTexture = 0;
    int texWidth = 0;
    int texHeight = 0;

    // Engine
    ImageProcessingEngine ipEngine;

    // UI state
    bool running = true;
    bool play = false;
    bool hudVisible = true;
    bool output = false;
    int playSpeed = 0;
    float processingBlending = 0.5f;
    // set whenever the pipeline must be (re)processed: new frame, seek,
    // plugin parameter change, pipeline edit, ... — cleared after Step()
    bool pipelineDirty = true;
    float videoSliderPos = 0.0f;
    bool sliderMoving = false;
    bool pendingScaleChange = false;

    // Zoom/pan
    float zoomStartX = 0.0f;
    float zoomStartY = 0.0f;
    float zoomEndX = 1.0f;
    float zoomEndY = 1.0f;

    // Pipeline dialog state
    int selectedPipelineItem = -1;
    int selectedAvailablePlugin = -1;
    std::vector<bool> pipelineDialogOpen;
    std::vector<bool> pipelineHelpOpen;   // per-dialog: help panel expanded

    // File dialogs / popups
    FileBrowser fileBrowser;
    std::string errorMessage;
    bool showAbout = false;

    // Quit confirmation (avoid losing unsaved pipeline / ROI work)
    bool showQuitConfirm = false;
    bool quitConfirmJustOpened = false;
    int  quitConfirmFocus = 1;  // 0 = Quit, 1 = Cancel (default); Tab/arrows move

    // GUI test harness (--test <script>): drives the interface with scripted
    // mouse moves / clicks / keys and captures screenshots, for automated tests
    bool testMode = false;
    std::vector<std::string> testScript;
    size_t testPc = 0;
    int testWaitFrames = 0;
    float testMouseX = 0.0f, testMouseY = 0.0f;
    bool testLeftDown = false, testRightDown = false;
    int testReleaseIn = 0;          // frames until an injected click releases
    std::string testShotPath;       // pending screenshot path
    int testKey = -1;               // ImGuiKey to inject this/next frame (-1 none)
    int testKeyReleaseIn = 0;       // frames until the injected key releases

    // PatternTracker: index of the plugin currently in click-to-seed mode
    // (-1 = none). A left click on the video seeds a target in that plugin.
    int patternSeedPluginIndex = -1;

    // WhiteBalance: index of the plugin currently in "pick white" mode
    // (-1 = none). A left click on the video picks a neutral pixel.
    int whitePickPluginIndex = -1;

    // Polygon ROI editor state
    bool roiEditing = false;
    int roiActivePolygon = -1;     // polygon receiving new vertices (-1 = none)
    int roiSelectedPolygon = -1;   // polygon selected in the list / for delete
    int roiDragPoly = -1;          // polygon whose vertex is being dragged
    int roiDragPoint = -1;         // vertex index being dragged

    // Available plugin names for UI
    std::vector<std::string> availablePluginNames;

    // HUD mats
    cv::Mat oglScreen;
    cv::Mat hudApp;
    cv::Mat hud;
    cv::Scalar textColor;

    // Icon textures
    struct IconTex { GLuint id = 0; int w = 0; int h = 0; };
    std::map<std::string, IconTex> icons;

    // Methods
    bool InitSDL();
    bool InitImGui();
    void ApplyUIScale();
    void Cleanup();

    void UpdateEngine();
    void RenderFrame();

    void UploadVideoTexture(const cv::Mat& frame);
    GLuint LoadIconTexture(const std::string& path);

    // UI drawing
    void DrawMenuBar();
    void DrawToolbar();
    void DrawDownscaleControl();
    void DottedScaleSlider();
    void DrawRulerOverlay(float imgMinX, float imgMinY, float imgSizeX, float imgSizeY);
    void DrawSeekBar(float width);
    void DrawVideoDisplay();
    void DrawTabs();
    void DrawProcessingTab();
    void DrawBackgroundTab();
    void DrawCalibrationTab();
    void DrawProcessingFrameTab();

    // Dialog drawing (one per plugin type)
    void DrawPluginDialog(int index);

    // Interactive tone-curve editor (used by the Curves plugin dialog).
    // Returns true when the control points were edited.
    bool DrawCurveEditor(const char* id, std::vector<cv::Point2f>& pts);

    // Polygon ROI overlay editing on the video (called from DrawVideoDisplay
    // when a ZonesOfInterest plugin is selected). (minX,minY)/(sizeX,sizeY)
    // describe the drawn image rectangle; the zoom UVs map screen <-> frame.
    void HandleRoiEditing(class ZonesOfInterest* zoi,
                          float minX, float minY, float sizeX, float sizeY);

    // File dialogs (native if available, in-app fallback otherwise)
    void OpenFileDialog(const std::string& title, FileBrowser::Mode mode,
                        const std::vector<std::string>& filters,
                        const std::string& defaultName,
                        std::function<void(const std::string&)> onSelect);
    void RefreshFileBrowser();
    void DrawFileBrowser();
    void DrawErrorPopup();
    void RequestQuit();     // ask to quit (shows the confirmation modal)
    void DrawQuitConfirm();

    // test harness
    void LoadTestScript(const std::string& file);
    void TestAdvance();        // per-frame: run the next script command(s)
    void TestInjectInput();    // per-frame: feed scripted mouse/keys into ImGui
    void CaptureScreenshot(const std::string& path);

    // Helpers
    void OpenSource();
    void OpenSourceFile(const std::string& filename);
    void ChangeCapture(Capture* newCapture);
    void SaveSource();
    void LoadSettings();
    void SaveSettings();
    void DoSaveSettings(const std::string& result);
    void ResetEngine();
    void ResetEngine(Parameters& params);
    // (re)create the HUD overlay at the current processing resolution
    void SyncHudSize();
    void HandleShortcut(SDL_Keycode key, bool ctrl);

    bool AddPipelinePlugin(const std::string& name, cv::FileNode& fn, int pos = -1);
    std::string CamelCaseToText(const std::string& txt);
    std::string TextToCamelCase(const std::string& txt);
};


#endif // APPGUI_H
