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
#include <wx/frame.h>
#include <wx/tglbtn.h>
#include <wx/gbsizer.h>
#include <wx/stattext.h>
#include <wx/choice.h>
#include <wx/radiobox.h>
#include <wx/statbmp.h>
//*)

#include <wx/dcclient.h>
#include <wx/checklst.h>
#include <wx/listbox.h>
#include <wx/dnd.h>
#include "ImageProcessingEngine.h"
#include "Parameters.h"

#include <opencv2/opencv.hpp>


class MyTextDropTargetAdd;
class MyTextDropTargetMove;
class MyTextDropTargetRemove;

// define function pointer type for char handlers

class MainFrame: public wxFrame
{
public:

    MainFrame(wxWindow* parent,wxWindowID id = -1);
    virtual ~MainFrame();

    bool OnPipelineAdd(wxCoord x, wxCoord y, const wxString& str);
    bool OnPipelineMove(wxCoord x, wxCoord y, const wxString& str);
    bool OnPipelineRemove(wxCoord x, wxCoord y, const wxString& str);
    void ConnectCharEvent();
    void DisconnectCharEvent();
    void RecursiveConnectCharEvent(wxWindow* pclComponent);
    void RecursiveDisconnectCharEvent(wxWindow* pclComponent);
    void OnChar(wxKeyEvent& event);
    bool IsRecording();

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
    void OnFilePickerCtrlZonesFileChanged(wxFileDirPickerEvent& event);
    void OnSpinCtrlBgFramesChange(wxSpinEvent& event);
    void OnSpinCtrlBgEndChange(wxSpinEvent& event);
    void OnSpinCtrlBgStartChange(wxSpinEvent& event);
    void OnCheckBoxRecalculateClick(wxCommandEvent& event);
    void OnRadioBoxMethodSelect(wxCommandEvent& event);
    void OnMenuOpenCaptureSelected(wxCommandEvent& event);
    void OnGLCanvas1MouseEnter(wxMouseEvent& event);
    void OnCheckBoxUseTimeBoundsClick(wxCommandEvent& event);
    void OnMenuCalibrateSourceSelected(wxCommandEvent& event);
    void OnMenuLoadCalibrationDataSelected(wxCommandEvent& event);
    void OnMenuSaveCalibrationDataSelected(wxCommandEvent& event);
    void OnButtonCalibCalculateClick(wxCommandEvent& event);
    void OnButtonCalibResetClick(wxCommandEvent& event);
    void OnButtonCalibSaveClick(wxCommandEvent& event);
    void OnButtonCalibLoadClick(wxCommandEvent& event);
    void OnRadioBoxCalibBoardTypeSelect(wxCommandEvent& event);
    void OnSpinCtrlCalibWidthChange(wxSpinEvent& event);
    void OnSpinCtrlCalibHeightChange(wxSpinEvent& event);
    void OnSpinCtrlCalibSquareSizeChange(wxSpinEvent& event);
    void OnSpinCtrlCalibFramesCountChange(wxSpinEvent& event);
    void OnSpinCtrlCalibAspectNumChange(wxSpinEvent& event);
    void OnSpinCtrlCalibAspectDenChange(wxSpinEvent& event);
    void OnCheckBoxCalibZeroTangentDistClick(wxCommandEvent& event);
    void OnCheckBoxCalibFixPrincipalPointClick(wxCommandEvent& event);
    void OnCheckBoxCalibFlipVerticalClick(wxCommandEvent& event);
    void OnButtonCalibStitchClick(wxCommandEvent& event);
    void OnSpinCtrlCalibFrameDelayChange(wxSpinEvent& event);
    void OnChoiceCalibSubdevicesSelect(wxCommandEvent& event);
    void OnButtonConfigStitchClick(wxCommandEvent& event);
    void OnButtonConfigResetStitchingClick(wxCommandEvent& event);
    void OnMenuSaveCaptureSelected(wxCommandEvent& event);
    void OnButtonConfigLoadStitchingClick(wxCommandEvent& event);
    void OnButtonConfigSaveStitchingClick(wxCommandEvent& event);
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
    static const long ID_STATICTEXT9;
    static const long ID_CHECKBOX2;
    static const long ID_FILEPICKERCTRL1;
    static const long ID_STATICBITMAP4;
    static const long ID_BUTTON9;
    static const long ID_STATICBITMAP5;
    static const long ID_BUTTON10;
    static const long ID_BITMAPBUTTON13;
    static const long ID_BUTTON8;
    static const long ID_BITMAPBUTTON14;
    static const long ID_BUTTON11;
    static const long ID_SCROLLEDWINDOW4;
    static const long ID_CHOICE1;
    static const long ID_RADIOBOX2;
    static const long ID_STATICTEXT10;
    static const long ID_SPINCTRL4;
    static const long ID_STATICTEXT11;
    static const long ID_SPINCTRL5;
    static const long ID_STATICTEXT12;
    static const long ID_SPINCTRL9;
    static const long ID_STATICTEXT13;
    static const long ID_SPINCTRL10;
    static const long ID_STATICTEXT16;
    static const long ID_SPINCTRL13;
    static const long ID_STATICTEXT14;
    static const long ID_SPINCTRL11;
    static const long ID_STATICTEXT15;
    static const long ID_SPINCTRL12;
    static const long ID_CHECKBOX3;
    static const long ID_CHECKBOX4;
    static const long ID_CHECKBOX5;
    static const long ID_BITMAPBUTTON5;
    static const long ID_BUTTON2;
    static const long ID_BITMAPBUTTON12;
    static const long ID_BUTTON7;
    static const long ID_BITMAPBUTTON6;
    static const long ID_BUTTON5;
    static const long ID_BITMAPBUTTON7;
    static const long ID_BUTTON6;
    static const long ID_SCROLLEDWINDOW3;
    static const long ID_NOTEBOOK1;
    static const long idMenuOpenSource;
    static const long idMenuSaveSource;
    static const long idMenuLoadSettings;
    static const long idMenuSaveSettings;
    static const long idMenuQuit;
    static const long idMenuAbout;
    //*)
    /* static const long ID_SPINCTRL6x; */
    /* static const long ID_SPINCTRL7x; */
    static const long ID_SPINCTRL8x;
    static const long ID_SPINCTRL13x;

    //(*Declarations(MainFrame)
    wxBitmapButton* BitmapButton2;
    wxButton* ButtonBgSave;
    wxSpinCtrl* SpinCtrlCalibSquareSize;
    wxBitmapButton* buttonStop;
    wxSpinCtrl* SpinCtrlStartTime;
    wxBitmapButton* BitmapButton3;
    wxStaticText* StaticText13;
    wxToggleButton* ToggleButtonProcessing;
    wxStaticText* StaticText14;
    wxStaticText* StaticText15;
    wxScrolledWindow* ConfigTab;
    wxSpinCtrl* SpinCtrlDuration;
    wxBitmapButton* buttonPlay;
    wxSpinCtrl* SpinCtrlCalibAspectDen;
    wxButton* ButtonCalibCalculate;
    wxBitmapButton* buttonForward;
    wxButton* ButtonBgRecalculate;
    wxStaticBitmap* StaticBitmap5;
    wxSpinCtrl* SpinCtrlCalibFramesCount;
    wxMenuItem* MenuSaveSettings;
    wxBitmapButton* buttonStepBackwards;
    wxButton* ButtonConfigLoadStitching;
    wxCheckBox* CheckBoxCalibZeroTangentDist;
    wxScrolledWindow* BackgroundTab;
    wxStaticText* StaticText1;
    wxScrolledWindow* CalibrationTab;
    wxStaticText* StaticText10;
    wxStaticText* StaticText16;
    wxSlider* videoSlider;
    wxStaticBox* StaticBox2;
    wxCheckBox* CheckBoxCalibFixPrincipalPoint;
    wxSpinCtrl* SpinCtrlTimestep;
    wxStaticText* StaticText3;
    wxButton* ButtonBgLoad;
    wxChoice* ChoiceCalibSubdevices;
    wxButton* ButtonConfigSaveStitching;
    wxButton* ButtonConfigStitch;
    wxButton* ButtonCalibSave;
    wxRadioBox* RadioBoxMethod;
    wxCheckBox* CheckBoxUseTimeBounds;
    wxGLCanvas* GLCanvas1;
    wxButton* ButtonConfigResetStitching;
    wxBitmapButton* buttonStepForward;
    wxStaticText* StaticText8;
    wxStaticText* StaticText12;
    wxBitmapButton* BitmapButton5;
    wxBitmapButton* BitmapButton4;
    wxFileDialog* FileDialog1;
    wxStaticBox* StaticBox1;
    wxBitmapButton* buttonBackwards;
    wxSpinCtrl* SpinCtrlCalibAspectNum;
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
    wxSpinCtrl* SpinCtrlCalibFrameDelay;
    wxStaticBitmap* StaticBitmap4;
    wxStaticText* StaticText6;
    wxMenuItem* MenuOpenCapture;
    wxMenuItem* MenuLoadSettings;
    wxCheckBox* CheckBoxCalibFlipVertical;
    wxSpinCtrl* SpinCtrlCalibWidth;
    wxFilePickerCtrl* FilePickerCtrlZones;
    wxButton* ButtonCalibReset;
    wxBitmapButton* BitmapButton1;
    wxSpinCtrl* SpinCtrlCalibHeight;
    wxRadioBox* RadioBoxCalibBoardType;
    wxBitmapButton* buttonOutput;
    wxStaticText* StaticText9;
    wxCheckBox* CheckBoxRecalculate;
    wxBitmapButton* buttonHud;
    wxStaticText* StaticText11;
    wxMenuItem* MenuSaveCapture;
    wxButton* ButtonCalibLoad;
    wxBitmapButton* BitmapButton6;
    wxStaticBitmap* StaticBitmap2;
    //*)

    wxSpinCtrlDouble* SpinCtrlTimestepDouble;
    wxSpinCtrlDouble* SpinCtrlCalibFrameDelayDouble;
    wxStaticBoxSizer* StaticBoxSizerCalibSubdevices2;
    wxStaticBoxSizer* StaticBoxSizerConfigMultiSource2;
    wxFlexGridSizer* FlexGridSizerCalibStitch2;
    wxFlexGridSizer* FlexGridSizerCalibMain2;


// My methods

    void OnSpinCtrlDoubleTimestepChange(wxSpinDoubleEvent& event);
    void OnSpinCtrlDoubleCalibFrameDelayChange(wxSpinDoubleEvent& event);

    void OnUpdateInternalParameters (wxCommandEvent& event);

    void OnListBoxPipelinePluginsBeginDrag(wxMouseEvent& event);
    void OnListBoxPipelineBeginDrag(wxMouseEvent& event);
    void OnListBoxPipelineCheck(wxCommandEvent& event);

    void OnIdle(wxIdleEvent& evt);
    bool AddPipelinePlugin (std::string str, cv::FileNode& fn, int pos = -1, bool showDialog = false);

    void PrintInfoToHud();
    void DrawTrackerSelection(wxClientDC& dc);
    std::string TextToCamelCase (std::string txt);
    std::string CamelCaseToText (std::string txt);

    void ConvertWxCoordinatesToImage(wxPoint& p);
    void AdjustMarqueeAspectRatio();
    void AdjustOrthoAspectRatio(int w, int h);
    void UpdateUI();
    void UpdateCalibrationTab();

    void ResetImageProcessingEngine(Parameters& parameters);
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

    int playSpeed = 0;

    cv::Mat oglScreen;
    cv::Mat hudApp;
    cv::Mat hud;

    DECLARE_EVENT_TABLE()
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
