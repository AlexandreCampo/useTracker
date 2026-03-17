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

#include <opencv2/opencv.hpp>

#include "ImageProcessingEngine.h"
#include "Parameters.h"

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
    float processingBlending = 0.0f;
    float videoSliderPos = 0.0f;
    bool sliderMoving = false;

    // Zoom/pan
    float zoomStartX = 0.0f;
    float zoomStartY = 0.0f;
    float zoomEndX = 1.0f;
    float zoomEndY = 1.0f;

    // Pipeline dialog state
    int selectedPipelineItem = -1;
    int selectedAvailablePlugin = -1;
    std::vector<bool> pipelineDialogOpen;

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
    void Cleanup();

    void UpdateEngine();
    void RenderFrame();

    void UploadVideoTexture(const cv::Mat& frame);
    GLuint LoadIconTexture(const std::string& path);

    // UI drawing
    void DrawMenuBar();
    void DrawToolbar();
    void DrawVideoDisplay();
    void DrawTabs();
    void DrawProcessingTab();
    void DrawBackgroundTab();
    void DrawCalibrationTab();
    void DrawProcessingFrameTab();

    // Dialog drawing (one per plugin type)
    void DrawPluginDialog(int index);

    // Helpers
    void OpenSource();
    void SaveSource();
    void LoadSettings();
    void SaveSettings();
    void ResetEngine();
    void ResetEngine(Parameters& params);

    bool AddPipelinePlugin(const std::string& name, cv::FileNode& fn, int pos = -1);
    std::string CamelCaseToText(const std::string& txt);
    std::string TextToCamelCase(const std::string& txt);
};


#endif // APPGUI_H
