/***************************************************************
 * Name:      MainFrame.h
 * Purpose:   Defines Application Frame
 * Author:     ()
 * Created:   2014-08-16
 * Copyright:  ()
 * License:
 **************************************************************/

#ifndef MAINFRAME_H
#define MAINFRAME_H

//(*Headers(MainFrame)
#include <wx/bmpbuttn.h>
#include <wx/glcanvas.h>
#include <wx/spinctrl.h>
#include <wx/checkbox.h>
#include <wx/sizer.h>
#include <wx/notebook.h>
#include <wx/button.h>
#include <wx/menu.h>
#include <wx/filedlg.h>
#include <wx/scrolwin.h>
#include <wx/slider.h>
#include <wx/statbox.h>
#include <wx/filepicker.h>
#include <wx/statline.h>
#include <wx/frame.h>
#include <wx/tglbtn.h>
#include <wx/gbsizer.h>
#include <wx/stattext.h>
#include <wx/radiobox.h>
#include <wx/statbmp.h>
//*)

#include <wx/checklst.h>
#include <wx/listbox.h>
#include <wx/dnd.h>
#include <wx/timer.h>
#include "ImageProcessingEngine.h"
#include "Parameters.h"

#include <opencv2/opencv.hpp>


struct Timer;
class MyTextDropTargetAdd;
class MyTextDropTargetMove;
class MyTextDropTargetRemove;


class MainFrame: public wxFrame
{
public:

    MainFrame(wxWindow* parent,wxWindowID id = -1);
    virtual ~MainFrame();

    void OnTimerNotify ();
    bool OnPipelineAdd(wxCoord x, wxCoord y, const wxString& str);
    bool OnPipelineMove(wxCoord x, wxCoord y, const wxString& str);
    bool OnPipelineRemove(wxCoord x, wxCoord y, const wxString& str);

private:

    //(*Handlers(MainFrame)
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnGLCanvas1Paint(wxPaintEvent& event);
    void OnListBox1Select(wxCommandEvent& event);
    void OnbuttonOutputClick(wxCommandEvent& event);
    void OnbuttonPlayClick(wxCommandEvent& event);
    void OnbuttonStopClick(wxCommandEvent& event);
    void OnvideoSliderCmdScrollChanged(wxScrollEvent& event);
    void OnvideoSliderCmdScrollThumbTrack(wxScrollEvent& event);
    void OnbuttonPipelineAddClick(wxCommandEvent& event);
    void OnbuttonPipelineRemoveClick(wxCommandEvent& event);
    void OnUpdateFrameParameters(wxCommandEvent& event);
    void OnbuttonBgRecalculateClick(wxCommandEvent& event);
    void OnbuttonSelectOutputVideoFileClick(wxCommandEvent& event);
    void OnbuttonSelectOutputTextFileClick(wxCommandEvent& event);
    void OnbuttonSelectZonesFileClick(wxCommandEvent& event);
    void OnNotebook1PageChanged(wxNotebookEvent& event);
    void OnUpdateSpin(wxSpinEvent& event);
    void OnListBoxPipelineDClick(wxCommandEvent& event);
    void OnButtonBgLoadClick(wxCommandEvent& event);
    void OnButtonBgSaveClick(wxCommandEvent& event);
    void OnbuttonForwardClick(wxCommandEvent& event);
    void OnbuttonBackwardsClick(wxCommandEvent& event);
    void OnbuttonStepForwardClick(wxCommandEvent& event);
    void OnbuttonStepBackwardsClick(wxCommandEvent& event);
    void OnbuttonHudClick(wxCommandEvent& event);
    void OnMenuLoadSettingsSelected(wxCommandEvent& event);
    void OnMenuSaveSettingsSelected(wxCommandEvent& event);
    void OnMenuOpenAVTCamSelected(wxCommandEvent& event);
    void OnMenuOpenUSBCamSelected(wxCommandEvent& event);
    void OnMenuOpenImageSelected(wxCommandEvent& event);
    void OnMenuOpenVideoFileSelected(wxCommandEvent& event);
    void OnGLCanvas1LeftDown(wxMouseEvent& event);
    void OnGLCanvas1LeftUp(wxMouseEvent& event);
    void OnGLCanvas1MouseMove(wxMouseEvent& event);
    void OnGLCanvas1RightDown(wxMouseEvent& event);
    void OnGLCanvas1LeftDClick(wxMouseEvent& event);
    void OnGLCanvas1RightUp(wxMouseEvent& event);
    void OnGLCanvas1Resize(wxSizeEvent& event);
    void OnToggleButtonProcessingToggle(wxCommandEvent& event);
    void OnSpinCtrlStartTimeChange(wxSpinEvent& event);
    void OnSpinCtrlDurationChange(wxSpinEvent& event);
    void OnSpinCtrlTimestepIntChange(wxSpinEvent& event);
    void OnSpinCtrlTimestepChange(wxSpinEvent& event);
    void OnFilePickerCtrlZonesFileChanged(wxFileDirPickerEvent& event);
    void OnSpinCtrlBgFramesChange(wxSpinEvent& event);
    void OnSpinCtrlBgEndChange(wxSpinEvent& event);
    void OnSpinCtrlBgStartChange(wxSpinEvent& event);
    void OnCheckBoxRecalculateClick(wxCommandEvent& event);
    void OnRadioBoxMethodSelect(wxCommandEvent& event);
    //*)

    //(*Identifiers(MainFrame)
    static const long ID_GLCANVAS1;
    static const long ID_BITMAPBUTTON1;
    static const long ID_BITMAPBUTTON3;
    static const long ID_BITMAPBUTTON4;
    static const long ID_BITMAPBUTTON9;
    static const long ID_BITMAPBUTTON2;
    static const long ID_BITMAPBUTTON10;
    static const long ID_BITMAPBUTTON8;
    static const long ID_SLIDER1;
    static const long ID_BITMAPBUTTON11;
    static const long ID_TOGGLEBUTTON1;
    static const long ID_STATICBOX1;
    static const long ID_STATICLINE1;
    static const long ID_STATICBOX2;
    static const long ID_SCROLLEDWINDOW1;
    static const long ID_STATICTEXT1;
    static const long ID_SPINCTRL1;
    static const long ID_STATICTEXT3;
    static const long ID_SPINCTRL3;
    static const long ID_STATICTEXT2;
    static const long ID_SPINCTRL2;
    static const long ID_STATICTEXT5;
    static const long ID_RADIOBOX1;
    static const long ID_STATICTEXT4;
    static const long ID_CHECKBOX1;
    static const long ID_STATICBITMAP1;
    static const long ID_BUTTON1;
    static const long ID_STATICBITMAP2;
    static const long ID_BUTTON3;
    static const long ID_STATICBITMAP3;
    static const long ID_BUTTON4;
    static const long ID_SCROLLEDWINDOW2;
    static const long ID_STATICTEXT6;
    static const long ID_SPINCTRL6;
    static const long ID_STATICTEXT7;
    static const long ID_SPINCTRL7;
    static const long ID_STATICTEXT8;
    static const long ID_SPINCTRL8;
    static const long ID_FILEPICKERCTRL1;
    static const long ID_SCROLLEDWINDOW4;
    static const long ID_NOTEBOOK1;
    static const long idMenuOpenVideo;
    static const long ID_MENUITEM1;
    static const long idMenuOpenCaptureDevice;
    static const long ID_MENUITEM2;
    static const long idMeuLoadSettings;
    static const long idMenuSaveSettings;
    static const long idMenuQuit;
    static const long idMenuAbout;
    //*)
    /* static const long ID_SPINCTRL6x; */
    /* static const long ID_SPINCTRL7x; */
    static const long ID_SPINCTRL8x;

    //(*Declarations(MainFrame)
    wxButton* ButtonBgSave;
    wxBitmapButton* buttonStop;
    wxSpinCtrl* SpinCtrlStartTime;
    wxToggleButton* ToggleButtonProcessing;
    wxScrolledWindow* ConfigTab;
    wxSpinCtrl* SpinCtrlDuration;
    wxBitmapButton* buttonPlay;
    wxMenuItem* MenuOpenImage;
    wxBitmapButton* buttonForward;
    wxButton* ButtonBgRecalculate;
    wxMenuItem* MenuOpenVideoFile;
    wxMenuItem* MenuOpenUSBCam;
    wxMenuItem* MenuSaveSettings;
    wxBitmapButton* buttonStepBackwards;
    wxScrolledWindow* BackgroundTab;
    wxStaticText* StaticText1;
    wxSlider* videoSlider;
    wxStaticBox* StaticBox2;
    wxSpinCtrl* SpinCtrlTimestep;
    wxStaticText* StaticText3;
    wxButton* ButtonBgLoad;
    wxMenuItem* MenuOpenAVTCam;
    wxRadioBox* RadioBoxMethod;
    wxStaticLine* StaticLine1;
    wxGLCanvas* GLCanvas1;
    wxBitmapButton* buttonStepForward;
    wxStaticText* StaticText8;
    wxFileDialog* FileDialog1;
    wxStaticBox* StaticBox1;
    wxBitmapButton* buttonBackwards;
    wxStaticText* StaticText7;
    wxSpinCtrl* SpinCtrlBgEnd;
    wxScrolledWindow* ProcessingTab;
    wxStaticText* StaticText4;
    wxSpinCtrl* SpinCtrlBgStart;
    wxStaticText* StaticText5;
    wxSpinCtrl* SpinCtrlBgFrames;
    wxStaticText* StaticText2;
    wxNotebook* Notebook1;
    wxStaticBitmap* StaticBitmap3;
    wxStaticBitmap* StaticBitmap1;
    wxStaticText* StaticText6;
    wxMenuItem* MenuLoadSettings;
    wxFilePickerCtrl* FilePickerCtrlZones;
    wxBitmapButton* buttonOutput;
    wxCheckBox* CheckBoxRecalculate;
    wxBitmapButton* buttonHud;
    wxStaticBitmap* StaticBitmap2;
    //*)

    wxSpinCtrlDouble* SpinCtrlTimestepDouble;
    /* wxSpinCtrlDouble* SpinCtrlStartTimeDouble; */
    /* wxSpinCtrlDouble* SpinCtrlDurationDouble; */

// My methods

//void OnUpdateFrameParameters ();
    void OnUpdateInternalParameters (wxCommandEvent& event);

//void OnUpdateFrameParameters (wxCommandEvent& event);
    void OnListBoxPipelinePluginsBeginDrag(wxMouseEvent& event);
    void OnListBoxPipelineBeginDrag(wxMouseEvent& event);
    void OnListBoxPipelineCheck(wxCommandEvent& event);

//void Draw (Mat& oglScreen, Mat& hud);
    void Step();
    void OnIdle(wxIdleEvent& evt);
    bool AddPipelinePlugin (std::string str, cv::FileNode& fn, int pos = -1, bool showDialog = false);

    void PrintInfoToHud();
    std::string TextToCamelCase (std::string txt);
    std::string CamelCaseToText (std::string txt);

    void ConvertWxCoordinatesToImage(wxPoint& p);
    void AdjustMarqueeAspectRatio();
    void AdjustOrthoAspectRatio(int w, int h);
    void UpdateUI();

    void ResetImageProcessingEngine();

// My members

    wxCheckListBox* ListBoxPipeline;
    wxListBox* ListBoxPipelinePlugins;

    MyTextDropTargetAdd* textDropTargetAdd;
    MyTextDropTargetMove* textDropTargetMove;
    MyTextDropTargetRemove* textDropTargetRemove;

    int draggedPipelineItem;
    int draggedPipelineItemProcessed;

    wxScrolledWindow* activeTab;
    std::vector<wxDialog*> pipelineDialogs;

    bool showProcessing = false;

    Timer* timer;
    wxLongLong playTimestep;
    wxLongLong lastPlayTime;
//volatile bool timerNotified = false;

// main screen
    float orthoX;
    float orthoY;
    float orthoWidth;
    float orthoHeight;
    float glCanvasWidth;
    float glCanvasHeight;
    float glPadX;
    float glPadY;

// zoom
    wxPoint marqueeStart;
    wxPoint marqueeEnd;
    bool marqueeRegisteredDown = false;
    bool marqueeRegisteredMotion = false;
    float zoomStartX = 0;
    float zoomStartY = 0;
    float zoomEndX = 1;
    float zoomEndY = 1;

// scroll
    wxPoint scrollStart;
    bool scrollRegisteredMotion = false;
    bool scrollRegisteredDown = false;

    ImageProcessingEngine ipEngine;

    bool output;
    bool play;
    bool manualPlay;
    bool sliderMoving;
    bool hudVisible;

    int playSpeed;

//	char* textureBuffer;
//	char* textureBufferHud;
//	int textureWidth = 1;
//	int textureHeight = 1;
    cv::Mat emptyFrame;

    cv::Mat oglScreen;
    cv::Mat hudApp;
    cv::Mat hud;
//	Mat previousFrame;

    DECLARE_EVENT_TABLE()
};

struct Timer : public wxTimer
{
    MainFrame* frame;

    Timer (MainFrame* frame) {this->frame = frame;}
    void Notify() {frame->OnTimerNotify();}
};


class MyTextDropTargetAdd : public wxTextDropTarget
{
public:
    MainFrame* frame;

    MyTextDropTargetAdd(MainFrame* f)
	{
	    frame = f;
	}

    virtual bool OnDropText(wxCoord x, wxCoord y, const wxString& data)
	{
	    return frame->OnPipelineAdd(x, y, data);
	}
};

class MyTextDropTargetMove : public wxTextDropTarget
{
public:
    MainFrame* frame;

    MyTextDropTargetMove(MainFrame* f)
	{
	    frame = f;
	}

    virtual bool OnDropText(wxCoord x, wxCoord y, const wxString& data)
	{
	    return frame->OnPipelineMove(x, y, data);
	}
};

class MyTextDropTargetRemove : public wxTextDropTarget
{
public:
    MainFrame* frame;

    MyTextDropTargetRemove(MainFrame* f)
	{
	    frame = f;
	}

    virtual bool OnDropText(wxCoord x, wxCoord y, const wxString& data)
	{
	    return frame->OnPipelineRemove(x, y, data);
	}
};

#endif // MAINFRAME_H
