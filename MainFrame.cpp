/***************************************************************
 * Name:      MainFrame.cpp
 * Purpose:   Code for Application Frame
 * Author:     ()
 * Created:   2014-08-16
 * Copyright:  ()
 * License:
 **************************************************************/

#include "MainFrame.h"
#include "App.h"
#include <wx/msgdlg.h>

//(*InternalHeaders(MainFrame)
#include <wx/string.h>
#include <wx/intl.h>
#include <wx/bitmap.h>
#include <wx/image.h>
//*)

#include <GL/glut.h>
#include <iostream>
#include <wx/wx.h>
#include <wx/glcanvas.h>
#include <wx/time.h>
#include <wx/filename.h>
#include <wx/filedlg.h>
#include <wx/graphics.h>
#include <wx/tooltip.h>

#include "Background.h"

// Include standard OpenCV headers
#include<opencv2/highgui/highgui.hpp>

#include "DialogSafeErosion.h"
//#include "DialogFrameDifference.h"
#include "DialogErosion.h"
#include "DialogMovingAverage.h"
#include "DialogDilation.h"
#include "DialogExtractMotion.h"
#include "DialogColorSegmentation.h"
#include "DialogExtractBlobs.h"
#include "DialogTracker.h"
#include "DialogRecordVideo.h"
#include "DialogRecordPixels.h"
#include "DialogOpenCapture.h"

#include "Utils.h"
#include "CaptureVideo.h"
#include "CaptureUSBCamera.h"
#include "CaptureImage.h"
#include "CaptureAVTCamera.h"
#include "CaptureDefault.h"
#include "Tracker.h"

#include "Pipeline.h"


extern Parameters parameters;

//helper functions
enum wxbuildinfoformat {
    short_f, long_f };

wxString wxbuildinfo(wxbuildinfoformat format)
{
    wxString wxbuild(wxVERSION_STRING);

    if (format == long_f )
    {
#if defined(__WXMSW__)
        wxbuild << _T("-Windows");
#elif defined(__UNIX__)
        wxbuild << _T("-Linux");
#endif

#if wxUSE_UNICODE
        wxbuild << _T("-Unicode build");
#else
        wxbuild << _T("-ANSI build");
#endif // wxUSE_UNICODE
    }

    return wxbuild;
}

//(*IdInit(MainFrame)
const long MainFrame::ID_GLCANVAS1 = wxNewId();
const long MainFrame::ID_BITMAPBUTTON1 = wxNewId();
const long MainFrame::ID_BITMAPBUTTON3 = wxNewId();
const long MainFrame::ID_BITMAPBUTTON4 = wxNewId();
const long MainFrame::ID_BITMAPBUTTON9 = wxNewId();
const long MainFrame::ID_BITMAPBUTTON2 = wxNewId();
const long MainFrame::ID_BITMAPBUTTON10 = wxNewId();
const long MainFrame::ID_BITMAPBUTTON8 = wxNewId();
const long MainFrame::ID_SLIDER1 = wxNewId();
const long MainFrame::ID_BITMAPBUTTON11 = wxNewId();
const long MainFrame::ID_TOGGLEBUTTON1 = wxNewId();
const long MainFrame::ID_STATICBOX1 = wxNewId();
const long MainFrame::ID_STATICLINE1 = wxNewId();
const long MainFrame::ID_STATICBOX2 = wxNewId();
const long MainFrame::ID_SCROLLEDWINDOW1 = wxNewId();
const long MainFrame::ID_STATICTEXT1 = wxNewId();
const long MainFrame::ID_SPINCTRL1 = wxNewId();
const long MainFrame::ID_STATICTEXT3 = wxNewId();
const long MainFrame::ID_SPINCTRL3 = wxNewId();
const long MainFrame::ID_STATICTEXT2 = wxNewId();
const long MainFrame::ID_SPINCTRL2 = wxNewId();
const long MainFrame::ID_STATICTEXT5 = wxNewId();
const long MainFrame::ID_RADIOBOX1 = wxNewId();
const long MainFrame::ID_STATICTEXT4 = wxNewId();
const long MainFrame::ID_CHECKBOX1 = wxNewId();
const long MainFrame::ID_STATICBITMAP1 = wxNewId();
const long MainFrame::ID_BUTTON1 = wxNewId();
const long MainFrame::ID_STATICBITMAP2 = wxNewId();
const long MainFrame::ID_BUTTON3 = wxNewId();
const long MainFrame::ID_STATICBITMAP3 = wxNewId();
const long MainFrame::ID_BUTTON4 = wxNewId();
const long MainFrame::ID_SCROLLEDWINDOW2 = wxNewId();
const long MainFrame::ID_STATICTEXT6 = wxNewId();
const long MainFrame::ID_SPINCTRL6 = wxNewId();
const long MainFrame::ID_STATICTEXT7 = wxNewId();
const long MainFrame::ID_SPINCTRL7 = wxNewId();
const long MainFrame::ID_STATICTEXT8 = wxNewId();
const long MainFrame::ID_SPINCTRL8 = wxNewId();
const long MainFrame::ID_STATICTEXT9 = wxNewId();
const long MainFrame::ID_CHECKBOX2 = wxNewId();
const long MainFrame::ID_FILEPICKERCTRL1 = wxNewId();
const long MainFrame::ID_SCROLLEDWINDOW4 = wxNewId();
const long MainFrame::ID_NOTEBOOK1 = wxNewId();
const long MainFrame::ID_MENUITEM1 = wxNewId();
const long MainFrame::idMeuLoadSettings = wxNewId();
const long MainFrame::idMenuSaveSettings = wxNewId();
const long MainFrame::idMenuQuit = wxNewId();
const long MainFrame::idMenuAbout = wxNewId();
//*)

// const long MainFrame::ID_SPINCTRL6x = wxNewId();
// const long MainFrame::ID_SPINCTRL7x = wxNewId();
const long MainFrame::ID_SPINCTRL8x = wxNewId();


BEGIN_EVENT_TABLE(MainFrame,wxFrame)
    //(*EventTable(MainFrame)
    //*)
END_EVENT_TABLE()


MainFrame::MainFrame(wxWindow* parent,wxWindowID id)
{
    //(*Initialize(MainFrame)
    wxMenuItem* MenuItem2;
    wxStaticBoxSizer* StaticBoxSizer2;
    wxMenuItem* MenuItem1;
    wxFlexGridSizer* FlexGridSizer1;
    wxFlexGridSizer* FlexGridSizer2;
    wxMenu* Menu1;
    wxFlexGridSizer* FlexGridSizer11;
    wxFlexGridSizer* FlexGridSizer4;
    wxFlexGridSizer* FlexGridSizer6;
    wxFlexGridSizer* FlexGridSizer3;
    wxStaticBoxSizer* StaticBoxSizer4;
    wxFlexGridSizer* FlexGridSizer10;
    wxMenuBar* MenuBar1;
    wxGridBagSizer* GridBagSizer1;
    wxMenu* Menu2;
    wxFlexGridSizer* FlexGridSizer5;
    wxStaticBoxSizer* StaticBoxSizer1;

    Create(parent, wxID_ANY, _("USE Tracker"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE, _T("wxID_ANY"));
    SetMinSize(wxSize(-1,-1));
    SetMaxSize(wxSize(-1,-1));
    FlexGridSizer1 = new wxFlexGridSizer(1, 2, 0, 0);
    FlexGridSizer1->AddGrowableCol(0);
    FlexGridSizer1->AddGrowableRow(0);
    FlexGridSizer6 = new wxFlexGridSizer(2, 1, 0, 0);
    FlexGridSizer6->AddGrowableCol(0);
    FlexGridSizer6->AddGrowableRow(0);
    int GLCanvasAttributes_1[] = {
    	WX_GL_RGBA,
    	WX_GL_DOUBLEBUFFER,
    	WX_GL_DEPTH_SIZE,      16,
    	WX_GL_STENCIL_SIZE,    0,
    	0, 0 };
    GLCanvas1 = new wxGLCanvas(this, ID_GLCANVAS1, wxDefaultPosition, wxSize(-1,-1), 0, _T("ID_GLCANVAS1"), GLCanvasAttributes_1);
    GLCanvas1->SetMinSize(wxSize(800,600));
    GLCanvas1->SetMaxSize(wxSize(-1,-1));
    GLCanvas1->SetFocus();
    FlexGridSizer6->Add(GLCanvas1, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer2 = new wxFlexGridSizer(1, 9, 0, 0);
    FlexGridSizer2->AddGrowableCol(7);
    buttonOutput = new wxBitmapButton(this, ID_BITMAPBUTTON1, wxBitmap(wxImage(_T("/usr/local/share/useTracker/images/Actions-media-record-icon inactive.png"))), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW, wxDefaultValidator, _T("ID_BITMAPBUTTON1"));
    FlexGridSizer2->Add(buttonOutput, 1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    buttonStop = new wxBitmapButton(this, ID_BITMAPBUTTON3, wxBitmap(wxImage(_T("/usr/local/share/useTracker/images/Actions-media-playback-stop-icon (1).png"))), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW|wxNO_BORDER, wxDefaultValidator, _T("ID_BITMAPBUTTON3"));
    FlexGridSizer2->Add(buttonStop, 1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    buttonBackwards = new wxBitmapButton(this, ID_BITMAPBUTTON4, wxBitmap(wxImage(_T("/usr/local/share/useTracker/images/Actions-media-seek-backward-icon (1).png"))), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW|wxNO_BORDER, wxDefaultValidator, _T("ID_BITMAPBUTTON4"));
    FlexGridSizer2->Add(buttonBackwards, 1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    buttonStepBackwards = new wxBitmapButton(this, ID_BITMAPBUTTON9, wxBitmap(wxImage(_T("/usr/local/share/useTracker/images/Actions-media-skip-backward-icon (1).png"))), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW|wxNO_BORDER, wxDefaultValidator, _T("ID_BITMAPBUTTON9"));
    FlexGridSizer2->Add(buttonStepBackwards, 1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    buttonPlay = new wxBitmapButton(this, ID_BITMAPBUTTON2, wxBitmap(wxImage(_T("/usr/local/share/useTracker/images/Actions-media-playback-start-icon (1).png"))), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW|wxNO_BORDER, wxDefaultValidator, _T("ID_BITMAPBUTTON2"));
    buttonPlay->SetBitmapSelected(wxBitmap(wxImage(_T("/usr/local/share/useTracker/images/Actions-media-playback-pause-icon (1).png"))));
    FlexGridSizer2->Add(buttonPlay, 1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    buttonStepForward = new wxBitmapButton(this, ID_BITMAPBUTTON10, wxBitmap(wxImage(_T("/usr/local/share/useTracker/images/Actions-media-skip-forward-icon (1).png"))), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW|wxNO_BORDER, wxDefaultValidator, _T("ID_BITMAPBUTTON10"));
    FlexGridSizer2->Add(buttonStepForward, 1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    buttonForward = new wxBitmapButton(this, ID_BITMAPBUTTON8, wxBitmap(wxImage(_T("/usr/local/share/useTracker/images/Actions-media-seek-forward-icon (1).png"))), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW|wxNO_BORDER, wxDefaultValidator, _T("ID_BITMAPBUTTON8"));
    FlexGridSizer2->Add(buttonForward, 1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    videoSlider = new wxSlider(this, ID_SLIDER1, 0, 0, 10000, wxDefaultPosition, wxSize(-1,-1), 0, wxDefaultValidator, _T("ID_SLIDER1"));
    videoSlider->SetMinSize(wxSize(-1,-1));
    videoSlider->SetMaxSize(wxSize(-1,-1));
    FlexGridSizer2->Add(videoSlider, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 12);
    buttonHud = new wxBitmapButton(this, ID_BITMAPBUTTON11, wxBitmap(wxImage(_T("/usr/local/share/useTracker/images/Apps-utilities-system-monitor-icon active (1).png"))), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW|wxNO_BORDER, wxDefaultValidator, _T("ID_BITMAPBUTTON11"));
    FlexGridSizer2->Add(buttonHud, 1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    FlexGridSizer6->Add(FlexGridSizer2, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer1->Add(FlexGridSizer6, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Notebook1 = new wxNotebook(this, ID_NOTEBOOK1, wxDefaultPosition, wxDefaultSize, 0, _T("ID_NOTEBOOK1"));
    Notebook1->SetMaxSize(wxSize(-1,650));
    ProcessingTab = new wxScrolledWindow(Notebook1, ID_SCROLLEDWINDOW1, wxDefaultPosition, wxDefaultSize, wxVSCROLL|wxHSCROLL, _T("ID_SCROLLEDWINDOW1"));
    FlexGridSizer11 = new wxFlexGridSizer(0, 1, 0, 0);
    ToggleButtonProcessing = new wxToggleButton(ProcessingTab, ID_TOGGLEBUTTON1, _("Show Processing"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TOGGLEBUTTON1"));
    FlexGridSizer11->Add(ToggleButtonProcessing, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticBox1 = new wxStaticBox(ProcessingTab, ID_STATICBOX1, _("Processing pipeline"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICBOX1"));
    FlexGridSizer11->Add(StaticBox1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticLine1 = new wxStaticLine(ProcessingTab, ID_STATICLINE1, wxDefaultPosition, wxSize(10,-1), wxLI_HORIZONTAL, _T("ID_STATICLINE1"));
    FlexGridSizer11->Add(StaticLine1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticBox2 = new wxStaticBox(ProcessingTab, ID_STATICBOX2, _("Available functions"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICBOX2"));
    FlexGridSizer11->Add(StaticBox2, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    ProcessingTab->SetSizer(FlexGridSizer11);
    FlexGridSizer11->Fit(ProcessingTab);
    FlexGridSizer11->SetSizeHints(ProcessingTab);
    BackgroundTab = new wxScrolledWindow(Notebook1, ID_SCROLLEDWINDOW2, wxDefaultPosition, wxDefaultSize, wxVSCROLL|wxHSCROLL, _T("ID_SCROLLEDWINDOW2"));
    FlexGridSizer10 = new wxFlexGridSizer(0, 1, 0, 0);
    StaticBoxSizer2 = new wxStaticBoxSizer(wxVERTICAL, BackgroundTab, _("Background calculation"));
    FlexGridSizer5 = new wxFlexGridSizer(0, 2, 0, 0);
    StaticText1 = new wxStaticText(BackgroundTab, ID_STATICTEXT1, _("Start time"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT, _T("ID_STATICTEXT1"));
    FlexGridSizer5->Add(StaticText1, 1, wxTOP|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5);
    SpinCtrlBgStart = new wxSpinCtrl(BackgroundTab, ID_SPINCTRL1, _T("0"), wxDefaultPosition, wxDefaultSize, 0, 0, 1000000000, 0, _T("ID_SPINCTRL1"));
    SpinCtrlBgStart->SetValue(_T("0"));
    FlexGridSizer5->Add(SpinCtrlBgStart, 1, wxTOP|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText3 = new wxStaticText(BackgroundTab, ID_STATICTEXT3, _("End time"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT3"));
    FlexGridSizer5->Add(StaticText3, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    SpinCtrlBgEnd = new wxSpinCtrl(BackgroundTab, ID_SPINCTRL3, _T("0"), wxDefaultPosition, wxDefaultSize, 0, 0, 1000000000, 0, _T("ID_SPINCTRL3"));
    SpinCtrlBgEnd->SetValue(_T("0"));
    FlexGridSizer5->Add(SpinCtrlBgEnd, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    StaticText2 = new wxStaticText(BackgroundTab, ID_STATICTEXT2, _("# of frames"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT, _T("ID_STATICTEXT2"));
    FlexGridSizer5->Add(StaticText2, 1, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 0);
    SpinCtrlBgFrames = new wxSpinCtrl(BackgroundTab, ID_SPINCTRL2, _T("17"), wxDefaultPosition, wxDefaultSize, 0, 1, 1000, 17, _T("ID_SPINCTRL2"));
    SpinCtrlBgFrames->SetValue(_T("17"));
    FlexGridSizer5->Add(SpinCtrlBgFrames, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    StaticText5 = new wxStaticText(BackgroundTab, ID_STATICTEXT5, _("Method"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT5"));
    FlexGridSizer5->Add(StaticText5, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    wxString __wxRadioBoxChoices_1[2] =
    {
    	_("mean"),
    	_("median")
    };
    RadioBoxMethod = new wxRadioBox(BackgroundTab, ID_RADIOBOX1, wxEmptyString, wxDefaultPosition, wxDefaultSize, 2, __wxRadioBoxChoices_1, 2, 0, wxDefaultValidator, _T("ID_RADIOBOX1"));
    RadioBoxMethod->SetSelection(0);
    FlexGridSizer5->Add(RadioBoxMethod, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText4 = new wxStaticText(BackgroundTab, ID_STATICTEXT4, _("When loading"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT4"));
    FlexGridSizer5->Add(StaticText4, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    CheckBoxRecalculate = new wxCheckBox(BackgroundTab, ID_CHECKBOX1, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX1"));
    CheckBoxRecalculate->SetValue(false);
    FlexGridSizer5->Add(CheckBoxRecalculate, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    StaticBoxSizer2->Add(FlexGridSizer5, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer4 = new wxFlexGridSizer(0, 2, 0, 0);
    StaticBitmap1 = new wxStaticBitmap(BackgroundTab, ID_STATICBITMAP1, wxBitmap(wxImage(_T("/usr/local/share/useTracker/images/Categories-applications-system-icon (1).png"))), wxDefaultPosition, wxDefaultSize, wxSIMPLE_BORDER, _T("ID_STATICBITMAP1"));
    FlexGridSizer4->Add(StaticBitmap1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    ButtonBgRecalculate = new wxButton(BackgroundTab, ID_BUTTON1, _("Recalculate"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
    FlexGridSizer4->Add(ButtonBgRecalculate, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticBitmap2 = new wxStaticBitmap(BackgroundTab, ID_STATICBITMAP2, wxBitmap(wxImage(_T("/usr/local/share/useTracker/images/Actions-document-open-icon (1).png"))), wxDefaultPosition, wxDefaultSize, wxSIMPLE_BORDER, _T("ID_STATICBITMAP2"));
    FlexGridSizer4->Add(StaticBitmap2, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    ButtonBgLoad = new wxButton(BackgroundTab, ID_BUTTON3, _("Load background"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON3"));
    FlexGridSizer4->Add(ButtonBgLoad, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticBitmap3 = new wxStaticBitmap(BackgroundTab, ID_STATICBITMAP3, wxBitmap(wxImage(_T("/usr/local/share/useTracker/images/Actions-document-save-icon (1).png"))), wxDefaultPosition, wxDefaultSize, wxSIMPLE_BORDER, _T("ID_STATICBITMAP3"));
    FlexGridSizer4->Add(StaticBitmap3, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    ButtonBgSave = new wxButton(BackgroundTab, ID_BUTTON4, _("Save background"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON4"));
    FlexGridSizer4->Add(ButtonBgSave, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticBoxSizer2->Add(FlexGridSizer4, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer10->Add(StaticBoxSizer2, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BackgroundTab->SetSizer(FlexGridSizer10);
    FlexGridSizer10->Fit(BackgroundTab);
    FlexGridSizer10->SetSizeHints(BackgroundTab);
    ConfigTab = new wxScrolledWindow(Notebook1, ID_SCROLLEDWINDOW4, wxDefaultPosition, wxDefaultSize, wxVSCROLL, _T("ID_SCROLLEDWINDOW4"));
    FlexGridSizer3 = new wxFlexGridSizer(4, 1, 0, 0);
    FlexGridSizer3->AddGrowableCol(0);
    StaticBoxSizer1 = new wxStaticBoxSizer(wxHORIZONTAL, ConfigTab, _("General"));
    GridBagSizer1 = new wxGridBagSizer(0, 0);
    StaticText6 = new wxStaticText(ConfigTab, ID_STATICTEXT6, _("Start time (s)"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT6"));
    GridBagSizer1->Add(StaticText6, wxGBPosition(1, 0), wxDefaultSpan, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 1);
    SpinCtrlStartTime = new wxSpinCtrl(ConfigTab, ID_SPINCTRL6, _T("0"), wxDefaultPosition, wxDefaultSize, 0, 0, 1000000000, 0, _T("ID_SPINCTRL6"));
    SpinCtrlStartTime->SetValue(_T("0"));
    GridBagSizer1->Add(SpinCtrlStartTime, wxGBPosition(1, 1), wxDefaultSpan, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 1);
    StaticText7 = new wxStaticText(ConfigTab, ID_STATICTEXT7, _("Duration (s)"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT7"));
    GridBagSizer1->Add(StaticText7, wxGBPosition(2, 0), wxDefaultSpan, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 1);
    SpinCtrlDuration = new wxSpinCtrl(ConfigTab, ID_SPINCTRL7, _T("0"), wxDefaultPosition, wxDefaultSize, 0, 0, 1000000000, 0, _T("ID_SPINCTRL7"));
    SpinCtrlDuration->SetValue(_T("0"));
    GridBagSizer1->Add(SpinCtrlDuration, wxGBPosition(2, 1), wxDefaultSpan, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 1);
    StaticText8 = new wxStaticText(ConfigTab, ID_STATICTEXT8, _("Timestep (s)"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT8"));
    GridBagSizer1->Add(StaticText8, wxGBPosition(3, 0), wxDefaultSpan, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 1);
    SpinCtrlTimestep = new wxSpinCtrl(ConfigTab, ID_SPINCTRL8, _T("0"), wxDefaultPosition, wxDefaultSize, 0, 0, 100, 0, _T("ID_SPINCTRL8"));
    SpinCtrlTimestep->SetValue(_T("0"));
    GridBagSizer1->Add(SpinCtrlTimestep, wxGBPosition(3, 1), wxDefaultSpan, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 1);
    StaticText9 = new wxStaticText(ConfigTab, ID_STATICTEXT9, _("Use time bounds"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT9"));
    GridBagSizer1->Add(StaticText9, wxGBPosition(4, 0), wxDefaultSpan, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    CheckBoxUseTimeBounds = new wxCheckBox(ConfigTab, ID_CHECKBOX2, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX2"));
    CheckBoxUseTimeBounds->SetValue(false);
    GridBagSizer1->Add(CheckBoxUseTimeBounds, wxGBPosition(4, 1), wxDefaultSpan, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticBoxSizer1->Add(GridBagSizer1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 1);
    FlexGridSizer3->Add(StaticBoxSizer1, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticBoxSizer4 = new wxStaticBoxSizer(wxHORIZONTAL, ConfigTab, _("Zones of interest"));
    FilePickerCtrlZones = new wxFilePickerCtrl(ConfigTab, ID_FILEPICKERCTRL1, wxEmptyString, wxEmptyString, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxFLP_CHANGE_DIR|wxFLP_FILE_MUST_EXIST|wxFLP_OPEN, wxDefaultValidator, _T("ID_FILEPICKERCTRL1"));
    StaticBoxSizer4->Add(FilePickerCtrlZones, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer3->Add(StaticBoxSizer4, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    ConfigTab->SetSizer(FlexGridSizer3);
    FlexGridSizer3->Fit(ConfigTab);
    FlexGridSizer3->SetSizeHints(ConfigTab);
    Notebook1->AddPage(ProcessingTab, _("Main"), false);
    Notebook1->AddPage(BackgroundTab, _("Bg"), false);
    Notebook1->AddPage(ConfigTab, _("Config"), false);
    FlexGridSizer1->Add(Notebook1, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    SetSizer(FlexGridSizer1);
    MenuBar1 = new wxMenuBar();
    Menu1 = new wxMenu();
    MenuOpenCapture = new wxMenuItem(Menu1, ID_MENUITEM1, _("Open source\tCtrl+O"), wxEmptyString, wxITEM_NORMAL);
    Menu1->Append(MenuOpenCapture);
    Menu1->Break();
    MenuLoadSettings = new wxMenuItem(Menu1, idMeuLoadSettings, _("Load settings\tCtrl+L"), wxEmptyString, wxITEM_NORMAL);
    Menu1->Append(MenuLoadSettings);
    MenuSaveSettings = new wxMenuItem(Menu1, idMenuSaveSettings, _("Save settings\tCtrl+S"), wxEmptyString, wxITEM_NORMAL);
    Menu1->Append(MenuSaveSettings);
    Menu1->Break();
    MenuItem1 = new wxMenuItem(Menu1, idMenuQuit, _("Quit\tAlt-F4"), _("Quit the application"), wxITEM_NORMAL);
    Menu1->Append(MenuItem1);
    MenuBar1->Append(Menu1, _("&File"));
    Menu2 = new wxMenu();
    MenuItem2 = new wxMenuItem(Menu2, idMenuAbout, _("About\tF1"), _("Show info about this application"), wxITEM_NORMAL);
    Menu2->Append(MenuItem2);
    MenuBar1->Append(Menu2, _("Help"));
    SetMenuBar(MenuBar1);
    FileDialog1 = new wxFileDialog(this, _("Select file"), wxEmptyString, wxEmptyString, wxFileSelectorDefaultWildcardStr, wxFD_DEFAULT_STYLE, wxDefaultPosition, wxDefaultSize, _T("wxFileDialog"));
    FlexGridSizer1->Fit(this);
    FlexGridSizer1->SetSizeHints(this);

    GLCanvas1->Connect(wxEVT_PAINT,(wxObjectEventFunction)&MainFrame::OnGLCanvas1Paint,0,this);
    GLCanvas1->Connect(wxEVT_CHAR,(wxObjectEventFunction)&MainFrame::OnGLCanvas1Char,0,this);
    GLCanvas1->Connect(wxEVT_LEFT_DOWN,(wxObjectEventFunction)&MainFrame::OnGLCanvas1LeftDown,0,this);
    GLCanvas1->Connect(wxEVT_LEFT_UP,(wxObjectEventFunction)&MainFrame::OnGLCanvas1LeftUp,0,this);
    GLCanvas1->Connect(wxEVT_LEFT_DCLICK,(wxObjectEventFunction)&MainFrame::OnGLCanvas1LeftDClick,0,this);
    GLCanvas1->Connect(wxEVT_RIGHT_DOWN,(wxObjectEventFunction)&MainFrame::OnGLCanvas1RightDown,0,this);
    GLCanvas1->Connect(wxEVT_RIGHT_UP,(wxObjectEventFunction)&MainFrame::OnGLCanvas1RightUp,0,this);
    GLCanvas1->Connect(wxEVT_MOTION,(wxObjectEventFunction)&MainFrame::OnGLCanvas1MouseMove,0,this);
    GLCanvas1->Connect(wxEVT_ENTER_WINDOW,(wxObjectEventFunction)&MainFrame::OnGLCanvas1MouseEnter,0,this);
    GLCanvas1->Connect(wxEVT_SIZE,(wxObjectEventFunction)&MainFrame::OnGLCanvas1Resize,0,this);
    Connect(ID_BITMAPBUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&MainFrame::OnbuttonOutputClick);
    Connect(ID_BITMAPBUTTON3,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&MainFrame::OnbuttonStopClick);
    Connect(ID_BITMAPBUTTON4,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&MainFrame::OnbuttonBackwardsClick);
    Connect(ID_BITMAPBUTTON9,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&MainFrame::OnbuttonStepBackwardsClick);
    Connect(ID_BITMAPBUTTON2,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&MainFrame::OnbuttonPlayClick);
    Connect(ID_BITMAPBUTTON10,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&MainFrame::OnbuttonStepForwardClick);
    Connect(ID_BITMAPBUTTON8,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&MainFrame::OnbuttonForwardClick);
    Connect(ID_SLIDER1,wxEVT_SCROLL_THUMBTRACK,(wxObjectEventFunction)&MainFrame::OnvideoSliderCmdScrollThumbTrack);
    Connect(ID_SLIDER1,wxEVT_SCROLL_CHANGED,(wxObjectEventFunction)&MainFrame::OnvideoSliderCmdScrollChanged);
    Connect(ID_BITMAPBUTTON11,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&MainFrame::OnbuttonHudClick);
    Connect(ID_TOGGLEBUTTON1,wxEVT_COMMAND_TOGGLEBUTTON_CLICKED,(wxObjectEventFunction)&MainFrame::OnToggleButtonProcessingToggle);
    Connect(ID_SPINCTRL1,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&MainFrame::OnSpinCtrlBgStartChange);
    Connect(ID_SPINCTRL3,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&MainFrame::OnSpinCtrlBgEndChange);
    Connect(ID_SPINCTRL2,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&MainFrame::OnSpinCtrlBgFramesChange);
    Connect(ID_RADIOBOX1,wxEVT_COMMAND_RADIOBOX_SELECTED,(wxObjectEventFunction)&MainFrame::OnRadioBoxMethodSelect);
    Connect(ID_CHECKBOX1,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&MainFrame::OnCheckBoxRecalculateClick);
    Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&MainFrame::OnbuttonBgRecalculateClick);
    Connect(ID_BUTTON3,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&MainFrame::OnButtonBgLoadClick);
    Connect(ID_BUTTON4,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&MainFrame::OnButtonBgSaveClick);
    Connect(ID_SPINCTRL6,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&MainFrame::OnSpinCtrlStartTimeChange);
    Connect(ID_SPINCTRL7,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&MainFrame::OnSpinCtrlDurationChange);
    Connect(ID_CHECKBOX2,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&MainFrame::OnCheckBoxUseTimeBoundsClick);
    Connect(ID_FILEPICKERCTRL1,wxEVT_COMMAND_FILEPICKER_CHANGED,(wxObjectEventFunction)&MainFrame::OnFilePickerCtrlZonesFileChanged);
    Connect(ID_NOTEBOOK1,wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED,(wxObjectEventFunction)&MainFrame::OnNotebook1PageChanged);
    Connect(ID_MENUITEM1,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&MainFrame::OnMenuOpenCaptureSelected);
    Connect(idMeuLoadSettings,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&MainFrame::OnMenuLoadSettingsSelected);
    Connect(idMenuSaveSettings,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&MainFrame::OnMenuSaveSettingsSelected);
    Connect(idMenuQuit,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&MainFrame::OnQuit);
    Connect(idMenuAbout,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&MainFrame::OnAbout);
    //*)

    // end of codeblocks generated code... now comes some customization.

    // disable AVT menu if not compiled with VIMBA
    //#ifndef VIMBA
    //Menu1->Enable (ID_MENUITEM2, false);
    //#endif // VIMBA

    // codeblocks 13.12 / wxsmith does not support spinctrldouble yet... adding manually
    SpinCtrlTimestepDouble = new wxSpinCtrlDouble(ConfigTab, ID_SPINCTRL8x, _T("0"), wxDefaultPosition, wxDefaultSize, 0, 0, 100, 0, 0.01, _T("ID_SPINCTRL8"));
    SpinCtrlTimestepDouble->SetValue(_T("0"));
    GridBagSizer1->Replace(SpinCtrlTimestep, SpinCtrlTimestepDouble);
    SpinCtrlTimestepDouble->Bind(wxEVT_SPINCTRLDOUBLE, &MainFrame::OnSpinCtrlDoubleTimestepChange, this);

    // adjust scrollbars in tabs
    int sz = FlexGridSizer3->CalcMin().GetHeight() * 115 / 100;
    ConfigTab->SetScrollbars (0, 5, 0, sz/5);
    sz = FlexGridSizer11->CalcMin().GetHeight() * 115 / 100;
    ProcessingTab->SetScrollbars (0, 5, 0, sz/5);
    sz = FlexGridSizer10->CalcMin().GetHeight() * 115 / 100;
    BackgroundTab->SetScrollbars (0, 5, 0, sz/5);

    // setup drag and drop for image processing pipeline
    ListBoxPipeline = new wxCheckListBox(StaticBox1, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(ProcessingTab,wxSize(115,120)), 0, 0, wxLB_SINGLE|wxLB_NEEDED_SB);
    ListBoxPipelinePlugins = new wxListBox(StaticBox2, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(ProcessingTab,wxSize(115,150)), 0, 0, wxLB_SINGLE);

    textDropTargetAdd = new MyTextDropTargetAdd (this);
    textDropTargetMove = new MyTextDropTargetMove (this);
    textDropTargetRemove = new MyTextDropTargetRemove (this);

    ListBoxPipelinePlugins->Bind(wxEVT_MOTION, &MainFrame::OnListBoxPipelinePluginsBeginDrag, this);
    ListBoxPipeline->Bind(wxEVT_MOTION, &MainFrame::OnListBoxPipelineBeginDrag, this);
    ListBoxPipeline->Bind(wxEVT_LISTBOX_DCLICK, &MainFrame::OnListBoxPipelineDClick, this);
    ListBoxPipeline->Bind(wxEVT_CHECKLISTBOX, &MainFrame::OnListBoxPipelineCheck, this);

    ListBoxPipelinePlugins->Append("background difference");
    ListBoxPipelinePlugins->Append("color segmentation");
    ListBoxPipelinePlugins->Append("erosion");
    ListBoxPipelinePlugins->Append("safe erosion");
    ListBoxPipelinePlugins->Append("dilation");
    ListBoxPipelinePlugins->Append("moving average");
    ListBoxPipelinePlugins->Append("frame difference");
    ListBoxPipelinePlugins->Append("extract blobs");
    ListBoxPipelinePlugins->Append("get blobs angles");
    ListBoxPipelinePlugins->Append("track blobs");
    ListBoxPipelinePlugins->Append("simple tags");
    ListBoxPipelinePlugins->Append("record video");
    ListBoxPipelinePlugins->Append("record pixels");

    // connect opengl view with current displayed tab
    activeTab = ProcessingTab;

    // load, configure, and run the tracker
    output = false;
    play = false;
    manualPlay = false;
    sliderMoving = false;
    hudVisible = true;

    int argc = wxTheApp->argc;
    char** argv = wxTheApp->argv;
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_ALPHA | GLUT_DEPTH);

    ipEngine.LoadXML (parameters.rootNode);
    if (!ipEngine.capture) ipEngine.capture = new CaptureDefault();
    ResetImageProcessingEngine(parameters);

    // last step...
    Connect( wxID_ANY, wxEVT_IDLE, wxIdleEventHandler(MainFrame::OnIdle) );
}

MainFrame::~MainFrame()
{
    //(*Destroy(MainFrame)
    //*)
}

void MainFrame::OnQuit(wxCommandEvent& event)
{
    Close();
}

void MainFrame::OnAbout(wxCommandEvent& event)
{
    wxString msg = wxbuildinfo(long_f);
    wxMessageBox(msg, _("Welcome to..."));
}

void MainFrame::OnGLCanvas1Paint(wxPaintEvent& event)
{
    GLCanvas1->SetCurrent();
    wxPaintDC(this);

    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glColor4f (1.0f, 1.0f, 1.0f, 1.0f);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable     (GL_BLEND);
    glEnable(GL_TEXTURE_2D);

    // These are necessary if using glTexImage2D instead of gluBuild2DMipmaps
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    // Set Projection Matrix
    glViewport(0, 0, glCanvasWidth, glCanvasHeight);
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity();

    // Switch to Model View Matrix
    gluOrtho2D(orthoX, orthoX + orthoWidth, orthoY + orthoHeight, orthoY);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Draw the main view
    // ------------------
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // BW
    if (activeTab == ProcessingTab && showProcessing)
    {
    	glTexImage2D(
    	    GL_TEXTURE_2D,
    	    0,
    	    GL_RGB,
    	    oglScreen.cols,
    	    oglScreen.rows,
    	    0,
    	    GL_LUMINANCE,
    	    GL_UNSIGNED_BYTE,
    	    oglScreen.data);
    }
    // BGR image
    else
    {
	glTexImage2D(
	    GL_TEXTURE_2D,
	    0,
	    GL_RGB,
    	    oglScreen.cols,
    	    oglScreen.rows,
	    0,
	    GL_BGR,
	    GL_UNSIGNED_BYTE,
	    oglScreen.data);
    }

    // Draw a textured quad
    glBegin(GL_QUADS);
    glTexCoord2f(zoomStartX, zoomStartY); glVertex2f(0.0f, 0.0f);
    glTexCoord2f(zoomEndX, zoomStartY); glVertex2f(oglScreen.cols, 0.0f);
    glTexCoord2f(zoomEndX, zoomEndY); glVertex2f(oglScreen.cols, oglScreen.rows);
    glTexCoord2f(zoomStartX, zoomEndY); glVertex2f(0.0f, oglScreen.rows);
    glEnd();

    // Draw HUD stuff
    // --------------
    if (hudVisible && activeTab != BackgroundTab)
    {

	// Create Texture
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(
	    GL_TEXTURE_2D,
	    0,
	    GL_RGBA,
	    hud.cols,
	    hud.rows,
	    0,
	    GL_BGRA,
	    GL_UNSIGNED_BYTE,
	    hud.data);

	// Draw a textured quad
	glColor4f (1.0f, 1.0f, 1.0f, 0.5f);
	glBegin(GL_QUADS);
	glTexCoord2f(zoomStartX, zoomStartY); glVertex2f(0.0f, 0.0f);
	glTexCoord2f(zoomEndX, zoomStartY); glVertex2f(hud.cols, 0.0f);
	glTexCoord2f(zoomEndX, zoomEndY); glVertex2f(hud.cols, hud.rows);
	glTexCoord2f(zoomStartX, zoomEndY); glVertex2f(0.0f, hud.rows);
	glEnd();

	// Create Texture
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(
	    GL_TEXTURE_2D,
	    0,
	    GL_RGBA,
	    hudApp.cols,
	    hudApp.rows,
	    0,
	    GL_BGRA,
	    GL_UNSIGNED_BYTE,
	    hudApp.data);

	// Draw a textured quad
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glColor4f (1.0f, 1.0f, 1.0f, 1.0f);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f); glVertex2f(0.0f, 0.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex2f(hudApp.cols, 0.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex2f(hudApp.cols, hudApp.rows);
	glTexCoord2f(0.0f, 1.0f); glVertex2f(0.0f, hudApp.rows);
	glEnd();
    }

    glFlush();
    GLCanvas1->SwapBuffers();
}

void MainFrame::OnIdle(wxIdleEvent& evt)
{
    wxClientDC dc(this);

    // pipeline snapshot position checked
    int psel = ListBoxPipeline->GetSelection();
    if (psel != wxNOT_FOUND)
	ipEngine.snapshotPos = psel;
    else
	ipEngine.snapshotPos = ipEngine.pipelines[0].plugins.size() - 1;

    // estimate time to wait before getting next frame
    bool getNextFrame = true;
    wxLongLong currentTime = wxGetUTCTimeUSec();

    // calculate delay to grab next frame (reserve some usec, better be slightly in advance)
    wxLongLong td = ipEngine.capture->GetNextFrameSystemTime() - currentTime - 500;

    // wait for next frame / refresh ?
    if (td > 0)
    {
	// min UI refresh rate
	if (td < 33000) wxMicroSleep(td.ToLong());
	else getNextFrame = false;
    }

    // process frames only out of bg tab
    if (activeTab != BackgroundTab)
    {
	if (play)
	{
	    if (getNextFrame)
	    {
		bool gotFrame = ipEngine.GetNextFrame();

		// end of stream ?
		if (!gotFrame)
		{
		    buttonPlay->SetBitmap(wxBitmap(wxImage(_T("/usr/local/share/useTracker/images/Actions-media-playback-start-icon (1).png"))));
		    buttonPlay->Refresh();
		    play = false;
		}
	    }
	}

	// print hud
	hudApp.setTo (0);
	if (hudVisible) PrintInfoToHud();

	ipEngine.Step(hudVisible);
    }

    // draw the main graphics screen
    if (activeTab == ProcessingTab && showProcessing)
	oglScreen = ipEngine.pipelineSnapshot;
    else if (activeTab == ProcessingTab || activeTab == ConfigTab)
	oglScreen = ipEngine.capture->frame;
    else if (activeTab == BackgroundTab)
    oglScreen = ipEngine.background;

    // render ogl + hud with blending
    GLCanvas1->Refresh();

    // Draw a selection range if needed
    DrawTrackerSelection(dc);

    // refresh slider pos and drawing
    if (!sliderMoving)
    {
	videoSlider->SetValue(ipEngine.capture->GetFrameNumber() * 10000 / (ipEngine.capture->GetFrameCount()+1));
    }

    evt.RequestMore(); // render continuously, not only once on idle
}

void MainFrame::DrawTrackerSelection(wxClientDC& dc)
{
    if (ipEngine.capture->type == Capture::VIDEO)
    {
	if (ListBoxPipeline->GetCount() > 0)
	{
	    PipelinePlugin* pp = ipEngine.pipelines[ipEngine.threadsCount].plugins[ipEngine.snapshotPos];
	    Tracker* tracker = dynamic_cast<Tracker*>(pp);

	    if (tracker && tracker->history.size() > 0)
	    {
		float hstart = tracker->historyStartFrame;
		float hlength = tracker->history.size() / tracker->entitiesCount;
		float totalFrames = ipEngine.capture->GetFrameCount()+1;

		wxPoint pos = videoSlider->GetPosition();
		wxSize sz = videoSlider->GetSize();

		pos.x += 5;
		sz.x -= 10;

		// clear area
		wxPoint clearpos = pos;
		wxSize clearsz = sz;
		clearpos.y += sz.y + 1;
		clearpos.x -= 9;
		clearsz.y = 8;
		clearsz.x += 14;
		dc.SetPen(wxNullPen);
		dc.SetBrush(dc.GetBackground());
		dc.DrawRectangle(clearpos, clearsz);

		int px1 = pos.x + sz.x * hstart / totalFrames;
		int px2 = pos.x + sz.x * (hstart+hlength) / totalFrames;

		dc.SetPen(wxPen(wxColour(255, 100, 100), 2, wxSOLID));
		dc.SetBrush(wxNullBrush);
		int py = pos.y + sz.y + 1 + 4;
		dc.DrawLine(px1, py, px2, py);
		dc.SetPen(wxPen(wxColour(255, 100, 100), 4, wxSOLID));
		dc.SetBrush(wxBrush(wxColour(255, 100, 100)));
		dc.DrawCircle (px1, py, 2);
		dc.DrawCircle (px2, py, 2);
	    }
	}
    }
}

void MainFrame::PrintInfoToHud()
{
    int stotal = ipEngine.capture->GetTime();
    int h = stotal / 3600;
    int m = (stotal - h*3600) / 60;
    int s = stotal - h*3600 - m*60;
    char str[256];
    sprintf (str, "%02d:%02d:%02d", h, m, s);

    int baseline = 0;
    Size textSize = getTextSize(str, FONT_HERSHEY_SIMPLEX, 2.0, 2, &baseline);
    Point pos (hudApp.cols - textSize.width - hudApp.cols / 20, textSize.height + hudApp.rows / 20);
    putText(hudApp, str, pos+Point(2,2), FONT_HERSHEY_SIMPLEX, 2.0, cvScalar(0,0,0,255), 2, CV_AA);
    putText(hudApp, str, pos, FONT_HERSHEY_SIMPLEX, 2.0, cvScalar(0,255,200,255), 2, CV_AA);

    // selection
    if (marqueeRegisteredDown && marqueeRegisteredMotion)
    {
	rectangle(hudApp, Point(marqueeStart.x, marqueeStart.y), Point(marqueeEnd.x, marqueeEnd.y), Scalar (255, 255, 255, 255), 2);
    }

    // play speed
    if (playSpeed > 0)
    {
	sprintf (str, "%dx faster", playSpeed+1);
	textSize = getTextSize(str, FONT_HERSHEY_SIMPLEX, 1, 2, &baseline);
	pos.x =  hudApp.cols - textSize.width - hudApp.cols / 20;
	pos.y += textSize.height + hudApp.rows / 20;

	putText(hudApp, str, pos+Point(2,2), FONT_HERSHEY_SIMPLEX, 1, cvScalar(0,0,0,255), 2, CV_AA);
	putText(hudApp, str, pos, FONT_HERSHEY_SIMPLEX, 1, cvScalar(0,255,200,255), 2, CV_AA);
    }
    if (playSpeed < 0)
    {
	sprintf (str, "%dx slower", -playSpeed+1);
	textSize = getTextSize(str, FONT_HERSHEY_SIMPLEX, 1, 2, &baseline);
	pos.x =  hudApp.cols - textSize.width - hudApp.cols / 20;
	pos.y += textSize.height + hudApp.rows / 20;
	putText(hudApp, str, pos+Point(2,2), FONT_HERSHEY_SIMPLEX, 1, cvScalar(0,0,0,255), 2, CV_AA);
	putText(hudApp, str, pos, FONT_HERSHEY_SIMPLEX, 1, cvScalar(0,255,200,255), 2, CV_AA);
    }

    // frame number
    if (manualPlay)
    {
	sprintf (str, "frame number %ld", ipEngine.capture->GetFrameNumber());
	textSize = getTextSize(str, FONT_HERSHEY_SIMPLEX, 1, 2, &baseline);
	pos.x =  hudApp.cols - textSize.width - hudApp.cols / 20;
	pos.y += textSize.height + hudApp.rows / 20;
	putText(hudApp, str, pos+Point(2,2), FONT_HERSHEY_SIMPLEX, 1, cvScalar(0,0,0,255), 2, CV_AA);
	putText(hudApp, str, pos, FONT_HERSHEY_SIMPLEX, 1, cvScalar(0,255,200,255), 2, CV_AA);
    }
}

void MainFrame::OnbuttonOutputClick(wxCommandEvent& event)
{
    if (!output)
    {
	buttonOutput->SetBitmap(wxBitmap(wxImage(_T("/usr/local/share/useTracker/images/Actions-media-record-icon active.png"))));
	buttonOutput->Refresh();
	ipEngine.OpenOutput();
	output = true;
    }
    else
    {
	buttonOutput->SetBitmap(wxBitmap(wxImage(_T("/usr/local/share/useTracker/images/Actions-media-record-icon inactive.png"))));
	buttonOutput->Refresh();
	ipEngine.CloseOutput();
	output = false;
    }
}

void MainFrame::OnbuttonPlayClick(wxCommandEvent& event)
{
    if (!play)
    {
	buttonPlay->SetBitmap(wxBitmap(wxImage(_T("/usr/local/share/useTracker/images/Actions-media-playback-pause-icon (1).png"))));
	buttonPlay->Refresh();
	play = true;
	manualPlay = false;
	ipEngine.capture->Play();
    }
    else
    {
	buttonPlay->SetBitmap(wxBitmap(wxImage(_T("/usr/local/share/useTracker/images/Actions-media-playback-start-icon (1).png"))));
	buttonPlay->Refresh();
	play = false;
	ipEngine.capture->Pause();
    }
}

void MainFrame::OnbuttonStopClick(wxCommandEvent& event)
{
    buttonPlay->SetBitmap(wxBitmap(wxImage(_T("/usr/local/share/useTracker/images/Actions-media-playback-start-icon (1).png"))));
    buttonPlay->Refresh();
    videoSlider->SetValue(0);
    ipEngine.capture->Stop();
    play = false;

    // clear any tracking history (if tracker plugin is present)
    for (unsigned int i= 0; i < ipEngine.pipelines[0].plugins.size(); i++)
    {
	PipelinePlugin* pp = ipEngine.pipelines[ipEngine.threadsCount].plugins[i];
	Tracker* tracker = dynamic_cast<Tracker*>(pp);
	if (tracker && !tracker->replay) tracker->ClearHistory();
    }
}

void MainFrame::OnvideoSliderCmdScrollChanged(wxScrollEvent& event)
{
    if(ipEngine.capture->type == Capture::VIDEO)
    {
	CaptureVideo* capv = dynamic_cast<CaptureVideo*>(ipEngine.capture);
	int targetFrame = videoSlider->GetValue() * ipEngine.capture->GetFrameCount() / 10000;
	capv->SetFrameNumber(targetFrame);
	sliderMoving = false;
	videoSlider->UnsetToolTip();

	// clear any tracking history (if tracker plugin is present)
	for (unsigned int i= 0; i < ipEngine.pipelines[0].plugins.size(); i++)
	{
	    PipelinePlugin* pp = ipEngine.pipelines[ipEngine.threadsCount].plugins[i];
	    Tracker* tracker = dynamic_cast<Tracker*>(pp);
	    if (tracker && !tracker->replay) tracker->ClearHistory();
	}
    }
}

void MainFrame::OnvideoSliderCmdScrollThumbTrack(wxScrollEvent& event)
{
    if (ipEngine.capture->type == Capture::VIDEO)
    {
	wxLongLong s;
	int targetFrame = videoSlider->GetValue() * ipEngine.capture->GetFrameCount() / 10;
	s.Assign(targetFrame / ipEngine.capture->fps);
	wxTimeSpan ts (0,0,0,s);
	wxString f ("%H:%M:%S");
	videoSlider->SetToolTip(ts.Format(f));
    }

    sliderMoving = true;
}

void MainFrame::OnUpdateFrameParameters (wxCommandEvent& event)
{
    ipEngine.startTime = SpinCtrlStartTime->GetValue();
    ipEngine.durationTime = SpinCtrlDuration->GetValue();
    ipEngine.timestep = SpinCtrlTimestep->GetValue();

    // background
    ipEngine.bgFrames = SpinCtrlBgFrames->GetValue();
    //ipEngine.bgInterval = SpinCtrlBgInterval->GetValue();
    ipEngine.bgStartTime = SpinCtrlBgStart->GetValue();
    //parameters.bgFilename = string(TextCtrlBgFilename->GetValue());
    //parameters.bgUseFile = RadioBoxBgUseFile->IsItemEnabled(0);
}

void MainFrame::OnUpdateInternalParameters (wxCommandEvent& event)
{
    // read all
}


void MainFrame::OnbuttonBgRecalculateClick(wxCommandEvent& event)
{
    Mat newBg;
    if (ipEngine.bgCalcType == ImageProcessingEngine::BG_MEDIAN)
	newBg = CalculateBackgroundMedian (ipEngine.capture, ipEngine.bgStartTime, ipEngine.bgEndTime, ipEngine.bgFrames);
    else if (ipEngine.bgCalcType == ImageProcessingEngine::BG_MEAN)
	newBg = CalculateBackgroundMean (ipEngine.capture, ipEngine.bgStartTime, ipEngine.bgEndTime, ipEngine.bgFrames);
    newBg.copyTo(ipEngine.background);

    // wxProgressDialog dialog(wxT(“Progress dialog example”),
    // 			    wxT(“An informative message”),
    // 			    max,
    // 			    // range
    // 			    this,
    // 			    // parent
    // 			    wxPD_CAN_ABORT |
    // 			    wxPD_APP_MODAL |
    // 			    wxPD_ELAPSED_TIME |
    // 			    wxPD_ESTIMATED_TIME |
    // 			    wxPD_REMAINING_TIME);
    // bool cont = true;
    // for ( int i = 0; i <= max; i++ )
    // {
    // 	wxSleep(1);
    // 	if ( i == max )
    // 	    cont = dialog.Update(i, wxT(“That’s all, folks!”));
    // 	else if ( i == max / 2 )
    // 	    cont = dialog.Update(i, wxT(“Only a half left (very
    // 							   ➥ long message)!”));
    // 	else
    // 	    cont = dialog.Update(i);
    // 	if ( !cont )
    // 	{
    // 	    if ( wxMessageBox(wxT(“Do you really want to cancel?”),
    // 			      wxT(“Progress dialog question”),
    // 			      wxYES_NO | wxICON_QUESTION) == wxYES )
    // 		break;
    // 	    dialog.Resume();
    // 	}
    // }
    // if ( !cont )
    // 	wxLogStatus(wxT(“Progress dialog aborted!”));
    // else
    // 	wxLogStatus(wxT(“Countdown from %d finished”), max);



}

void MainFrame::OnbuttonSelectZonesFileClick(wxCommandEvent& event)
{
    wxString caption = wxT("Choose an image to define zones of interest");
    wxString wildcard = wxT("Image file (*.png)|*.png");
    wxString defaultDir = wxEmptyString;
    wxString defaultFilename = wxEmptyString;
    wxFileDialog dialog(this, caption, defaultDir, defaultFilename, wildcard, wxFD_OPEN | wxFD_FILE_MUST_EXIST);

    if (dialog.ShowModal() == wxID_OK)
    {
	wxString path = dialog.GetPath();
//	int filterIndex = dialog.GetFilterIndex();
    }

    // process data

}

void MainFrame::OnNotebook1PageChanged(wxNotebookEvent& event)
{
    if (Notebook1->GetSelection() == 0)
    {
	activeTab = ProcessingTab;
	ipEngine.takeSnapshot = true;
    }
    else if (Notebook1->GetSelection() == 1)
    {
	activeTab = BackgroundTab;
	ipEngine.takeSnapshot = false;
    }
    else if (Notebook1->GetSelection() == 2)
    {
	activeTab = ConfigTab;
	ipEngine.takeSnapshot = false;
    }
}


void MainFrame::OnButtonBgLoadClick(wxCommandEvent& event)
{
    wxString caption = wxT("Choose a background image to use");
    wxString wildcard = wxT("Image file (*.png)|*.png");

    wxFileName fn (ipEngine.bgFilename);

    wxString defaultDir = fn.GetFullName();
    wxString defaultFilename = fn.GetPath();

    wxFileDialog dialog(this, caption, "", "", wildcard, wxFD_OPEN | wxFD_FILE_MUST_EXIST);

    if (dialog.ShowModal() == wxID_OK)
    {
//	dialog.Hide();
	wxString path = dialog.GetPath();
//	int filterIndex = dialog.GetFilterIndex();

	// process data
	Mat bg = imread (path.ToStdString());

	if (bg.cols != ipEngine.capture->width || bg.rows != ipEngine.capture->height)
	{
//	    parameters.bgFilename = path;
	    cerr << "Background and video dimensions mismatch... please update your background" << std::endl;

	    wxMessageBox( wxT("Background picture and video dimensions mismatch."), wxT("An error was encountered..."), wxOK | wxICON_ERROR);
	}
	else
	{
	    ipEngine.background = bg;
	    ipEngine.bgFilename = path.ToStdString();
	}
    }
}

void MainFrame::OnButtonBgSaveClick(wxCommandEvent& event)
{
    wxString caption = wxT("Save current background image");
    wxString wildcard = wxT("Image file (*.png)|*.png");
    wxString defaultDir = wxEmptyString;
    wxString defaultFilename = wxEmptyString;
    wxFileDialog dialog(this, caption, "", "", wildcard, wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

    if (dialog.ShowModal() == wxID_OK)
    {
	wxString path = dialog.GetPath();

	SaveMatToPNG (ipEngine.background, static_cast<const char*>(path));
	parameters.bgFilename = path.ToStdString();
    }
}

void MainFrame::OnListBoxPipelinePluginsBeginDrag(wxMouseEvent& event)
{
    if (event.Dragging())
    {
	int n = ListBoxPipelinePlugins->HitTest(event.GetPosition());
	if (n != wxNOT_FOUND)
	{
	    wxString text = ListBoxPipelinePlugins->GetString(n);
	    wxTextDataObject tdo(text);
	    wxDropSource tds(tdo, ListBoxPipelinePlugins);

	    ListBoxPipeline->SetDropTarget (new MyTextDropTargetAdd(this));
	    ListBoxPipelinePlugins->SetDropTarget (NULL);
	    tds.DoDragDrop(wxDrag_AllowMove);
	}
    }
}

void MainFrame::OnListBoxPipelineBeginDrag(wxMouseEvent& event)
{
    if (event.Dragging())
    {
	int n = ListBoxPipeline->HitTest(event.GetPosition());
	if (n != wxNOT_FOUND)
	{
	    wxString text = ListBoxPipeline->GetString(n);
	    draggedPipelineItem = n;
	    bool checked = ListBoxPipeline->IsChecked(n);

	    // remove first
	    ListBoxPipeline->Delete(n);
	    ListBoxPipeline->Refresh();

	    wxTextDataObject tdo(text);
	    wxDropSource tds(tdo, ListBoxPipeline);

	    // try to drop
	    draggedPipelineItemProcessed = false;
	    ListBoxPipeline->SetDropTarget (new MyTextDropTargetMove(this));
	    ListBoxPipelinePlugins->SetDropTarget (new MyTextDropTargetRemove(this));
        tds.DoDragDrop(wxDrag_AllowMove);

	    // did not process ? restore text
	    if (!draggedPipelineItemProcessed)
	    {
		ListBoxPipeline->Insert(text, n);
		ListBoxPipeline->SetSelection(n);
		ListBoxPipeline->Check(n, checked);
	    }
	}
    }
}

bool MainFrame::OnPipelineAdd(wxCoord x, wxCoord y, const wxString& str)
{
    // drop to the appropriate place
    long sel = ListBoxPipeline->HitTest(wxPoint(x, y));

    FileNode fn;
    AddPipelinePlugin (str.ToStdString(), fn, sel, true);

    return true;
}

bool MainFrame::OnPipelineMove(wxCoord x, wxCoord y, const wxString& str)
{
    // remove data from vectors, but keep ptrs alive
    vector<PipelinePlugin*> pfv = ipEngine.Erase (draggedPipelineItem);

    vector<wxDialog*>::iterator it2 = pipelineDialogs.begin()+draggedPipelineItem;
    wxDialog* dlg = *it2;
    pipelineDialogs.erase(it2);

    // drop to the appropriate place
    long sel = ListBoxPipeline->HitTest(wxPoint(x, y));

    if (sel < 0)
    {
	int n =	ListBoxPipeline->Append(str);
	ListBoxPipeline->SetSelection(n);
	int idx = 0; if (!pfv[idx]) idx = ipEngine.threadsCount;
	ListBoxPipeline->Check(n, pfv[idx]->active);
	ipEngine.PushBack(pfv);
	pipelineDialogs.push_back(dlg);
    }
    else
    {
	ListBoxPipeline->Insert (str, sel);
	ListBoxPipeline->SetSelection (sel);
	int idx = 0; if (!pfv[idx]) idx = ipEngine.threadsCount;
	ListBoxPipeline->Check(sel, pfv[idx]->active);
	ipEngine.Insert(sel, pfv);
	pipelineDialogs.insert(pipelineDialogs.begin()+sel, dlg);
    }

    // advertise handling
    draggedPipelineItemProcessed = true;

    return true;
}


bool MainFrame::OnPipelineRemove(wxCoord x, wxCoord y, const wxString& str)
{
    // delete from image processing pipeline
    vector<PipelinePlugin*> pfv = ipEngine.Erase(draggedPipelineItem);
    for (auto f : pfv) delete f;

    // destroy the associated dialog
    vector<wxDialog*>::iterator it2 = pipelineDialogs.begin()+draggedPipelineItem;
    if (*it2) (*it2)->Destroy();
    pipelineDialogs.erase(it2);

    // advertise handling
    draggedPipelineItemProcessed = true;

    return true;
}

void MainFrame::OnListBoxPipelineDClick(wxCommandEvent& event)
{
    int sel = event.GetSelection();
    wxString str = ListBoxPipeline->GetString(sel);

    if (pipelineDialogs[sel])
	pipelineDialogs[sel]->Show();
}

void MainFrame::OnListBoxPipelineCheck(wxCommandEvent& event)
{
    int sel = event.GetSelection();

    if (sel == wxNOT_FOUND) return;

    for (unsigned int i = 0; i < ipEngine.pipelines.size(); i++)
    {
        auto it = ipEngine.pipelines[i].plugins.begin()+sel;
        if (*it) (*it)->active = ListBoxPipeline->IsChecked(sel);
    }
    // for (auto p : ipEngine.pipelines)
    // {
    //     auto it = p.plugins.begin()+sel;
    //     if (*it) (*it)->active = ListBoxPipeline->IsChecked(sel);
    // }
}

void MainFrame::OnbuttonForwardClick(wxCommandEvent& event)
{
    CaptureVideo* cap = dynamic_cast<CaptureVideo*> (ipEngine.capture);
    if (cap)
    {
	playSpeed++;

	if (playSpeed >= 0)
	    cap->SetSpeedFaster(playSpeed + 1);
	else
	    cap->SetSpeedSlower(-playSpeed + 1);
    }
}

void MainFrame::OnbuttonBackwardsClick(wxCommandEvent& event)
{
    CaptureVideo* cap = dynamic_cast<CaptureVideo*> (ipEngine.capture);
    if (cap)
    {
	playSpeed--;

	if (playSpeed >= 0)
	    cap->SetSpeedFaster(playSpeed + 1);
	else
	    cap->SetSpeedSlower(-playSpeed + 1);
    }
}

void MainFrame::OnbuttonStepForwardClick(wxCommandEvent& event)
{
    // disable play as we enter frame by frame
    buttonPlay->SetBitmap(wxBitmap(wxImage(_T("/usr/local/share/useTracker/images/Actions-media-playback-start-icon (1).png"))));
    buttonPlay->Refresh();
    play = false;
    manualPlay = true;
    ipEngine.capture->Pause();

    // request next frame manually
    if (!ipEngine.GetNextFrame())
    {
	// did not get a frame, pause
	buttonPlay->SetBitmap(wxBitmap(wxImage(_T("/usr/local/share/useTracker/images/Actions-media-playback-start-icon (1).png"))));
	buttonPlay->Refresh();
	play = false;
    }
}

void MainFrame::OnbuttonStepBackwardsClick(wxCommandEvent& event)
{
    if(ipEngine.capture->type == Capture::VIDEO)
    {
	CaptureVideo* capv = dynamic_cast<CaptureVideo*>(ipEngine.capture);

	// disable play as we enter frame by frame
	buttonPlay->SetBitmap(wxBitmap(wxImage(_T("/usr/local/share/useTracker/images/Actions-media-playback-start-icon (1).png"))));
	buttonPlay->Refresh();
	play = false;
	manualPlay = true;
	capv->Pause();

	// jump video to previous frame...
	capv->GetPreviousFrame();
    }
}

void MainFrame::OnbuttonHudClick(wxCommandEvent& event)
{
    if (hudVisible == true)
    {
	buttonHud->SetBitmap(wxBitmap(wxImage(_T("/usr/local/share/useTracker/images/Apps-utilities-system-monitor-icon inactive (1).png"))));
	buttonPlay->Refresh();
	hudVisible = false;
    }
    else
    {
	buttonHud->SetBitmap(wxBitmap(wxImage(_T("/usr/local/share/useTracker/images/Apps-utilities-system-monitor-icon active (1).png"))));
	buttonPlay->Refresh();
	hudVisible = true;
    }
}

string MainFrame::TextToCamelCase (string text)
{
    string res;

    transform(text.begin(), text.end(), text.begin(), (int (*)(int))tolower);

    bool upNext = true;
    for (unsigned int i = 0; i < text.size(); i++)
    {
        if (isalpha(text[i]))
	{
            if (upNext) res += toupper(text[i]);
            else res += text[i];
            upNext = false;
        }
        else upNext = true;
    }
    return res;
}

string MainFrame::CamelCaseToText (string text)
{
    string res;

    for (unsigned int i = 0; i < text.size(); i++)
    {
	// convert char
	char c = tolower(text[i]);

	// if it was a capital and not first char, add space
	if (c != text[i] && i != 0) res += " ";

	// append char
	res += c;
    }
    return res;
}

bool MainFrame::AddPipelinePlugin (string str, cv::FileNode& fn, int pos, bool showDialog)
{
    wxDialog* dlg;

    auto pfv = NewPipelinePluginVector[TextToCamelCase(str)] (fn, ipEngine.threadsCount);
//    auto pfv = CreatePipelinePlugin (TextToCamelCase(str), fn, ipEngine.threadsCount);

    if (str == "background difference")
    {
	DialogExtractMotion* dialog = new DialogExtractMotion(this);
	dialog->SetPlugin(pfv);
	dlg = dialog;
    }
    else if (str == "moving average")
    {
	DialogMovingAverage* dialog = new DialogMovingAverage(this);
	dialog->SetPlugin(pfv);
	dlg = dialog;
    }
    else if (str == "color segmentation")
    {
	DialogColorSegmentation* dialog = new DialogColorSegmentation(this);
	dialog->SetPlugin(pfv);
	dlg = dialog;
    }
    else if (str == "erosion")
    {
	DialogErosion* dialog = new DialogErosion(this);
	dialog->SetPlugin(pfv);
	dlg = dialog;
    }
    else if (str == "dilation")
    {
	DialogDilation* dialog = new DialogDilation(this);
	dialog->SetPlugin(pfv);
	dlg = dialog;
    }
    else if (str == "extract blobs")
    {
	DialogExtractBlobs* dialog = new DialogExtractBlobs(this);
	dialog->SetPlugin(pfv);
	dlg = dialog;
    }
    else if (str == "get blobs angles")
    {
	dlg = nullptr;
    }
    else if (str == "track blobs")
    {
	DialogTracker* dialog = new DialogTracker(this);
	dialog->SetPlugin(pfv);
	dlg = dialog;
    }
    else if (str == "safe erosion")
    {
	DialogSafeErosion* dialog = new DialogSafeErosion(this);
	dialog->SetPlugin(pfv);
	dlg = dialog;
    }
    else if (str == "frame difference")
    {
	dlg = nullptr;
    }
    else if (str == "record video")
    {
	DialogRecordVideo* dialog = new DialogRecordVideo(this);
	dialog->SetPlugin(pfv);
	dlg = dialog;
    }
    else if (str == "record pixels")
    {
	DialogRecordPixels* dialog = new DialogRecordPixels(this);
	dialog->SetPlugin(pfv);
	dlg = dialog;
    }
    else return false;

    if (pos < 0)
    {
	// push right before last text line
	int n = ListBoxPipeline->Append(str);
	ListBoxPipeline->SetSelection(n);
	int idx = 0; if (!pfv[idx]) idx = ipEngine.threadsCount;
	ListBoxPipeline->Check(n, pfv[idx]->active);
	ipEngine.PushBack(pfv, true);
	pipelineDialogs.push_back(dlg);
    }
    else
    {
	ListBoxPipeline->Insert (str, pos);
	ListBoxPipeline->SetSelection (pos);
	int idx = 0; if (!pfv[idx]) idx = ipEngine.threadsCount;
	ListBoxPipeline->Check(pos, pfv[idx]->active);
	ipEngine.Insert(pos, pfv, true);
	pipelineDialogs.insert(pipelineDialogs.begin()+pos, dlg);
    }

    if (showDialog && dlg)
    {
	dlg->Show();
    }

    return true;
}

void MainFrame::OnMenuSaveSettingsSelected(wxCommandEvent& event)
{
    wxString caption = wxT("Save current settings to an XML file");
    wxString wildcard = wxT("Settings file (*.xml)|*.xml");
    wxString defaultDir = wxEmptyString;
    wxString defaultFilename = wxEmptyString;
    wxFileDialog dialog(this, caption, defaultDir, defaultFilename, wildcard, wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

    if (dialog.ShowModal() == wxID_OK)
    {
	wxString path = dialog.GetPath();

	// open file
	FileStorage fs(path.ToStdString(), FileStorage::WRITE);

	if (fs.isOpened())
	{
	    fs << "Configuration" << "{";

	    ipEngine.SaveXML(fs);

	    // add pipeline
	    fs << "Pipeline" << "{";

	    // loop through all plugins of the first pipeline (in the gui)
	    for (unsigned int i = 0; i < ipEngine.pipelines[0].plugins.size(); i++)
	    {
		wxString text = ListBoxPipeline->GetString(i);
		string cc = TextToCamelCase (text.ToStdString());

		fs << string("Plugin_") + std::to_string(i) << "{" << cc << "{"; // ugly hack to go around duplicate key bug

		// plugins are nullptr in some pipelines, depending on multithreading...
		if (ipEngine.pipelines[0].plugins[i])
		    ipEngine.pipelines[0].plugins[i]->SaveXML(fs);
		else
		    ipEngine.pipelines[ipEngine.threadsCount].plugins[i]->SaveXML(fs);

		fs << "}" << "}";
	    }

	    fs << "}"; // Pipeline
	    fs << "}"; // Configuration
	    fs.release();
	}
	else
	{
	    wxMessageBox( wxT("Could not open file for saving settings."), wxT("An error was encountered..."), wxOK | wxICON_ERROR);
	}
    }
}

void MainFrame::OnMenuLoadSettingsSelected(wxCommandEvent& event)
{
    // select file
    wxString caption = wxT("Load settings from an XML file");
    wxString wildcard = wxT("Settings file (*.xml)|*.xml");
    wxString defaultDir = wxEmptyString;
    wxString defaultFilename = wxEmptyString;
    wxFileDialog dialog(this, caption, defaultDir, defaultFilename, wildcard, wxFD_OPEN | wxFD_FILE_MUST_EXIST);

    if (dialog.ShowModal() == wxID_OK)
    {
	wxString path = dialog.GetPath();

	parameters.file.release();
	parameters.file.open(path.ToStdString(), FileStorage::READ);
	if (parameters.file.isOpened())
	{
	    // press stop...
	    buttonPlay->SetBitmap(wxBitmap(wxImage(_T("/usr/local/share/useTracker/images/Actions-media-playback-start-icon (1).png"))));
	    buttonPlay->Refresh();
	    videoSlider->SetValue(0);
	    ipEngine.capture->Stop();
	    play = false;

	    parameters.rootNode = parameters.file["Configuration"];
	    ipEngine.LoadXML (parameters.rootNode);
	    ResetImageProcessingEngine(parameters);
	}
	else
	{
	    wxMessageBox( wxT("Could not load settings from this file."), wxT("An error was encountered..."), wxOK | wxICON_ERROR);
	}
    }
}

void MainFrame::OnGLCanvas1LeftDown(wxMouseEvent& event)
{
    scrollRegisteredDown = true;
    scrollStart = event.GetPosition();
}

void MainFrame::OnGLCanvas1LeftUp(wxMouseEvent& event)
{
    // end of any scroll
    scrollRegisteredDown = false;
    scrollRegisteredMotion = false;
}

void MainFrame::OnGLCanvas1RightDown(wxMouseEvent& event)
{
    marqueeStart = event.GetPosition();
    ConvertWxCoordinatesToImage(marqueeStart);
    marqueeRegisteredDown = true;
}

void MainFrame::OnGLCanvas1LeftDClick(wxMouseEvent& event)
{
    zoomStartX = 0;
    zoomStartY = 0;
    zoomEndX = 1;
    zoomEndY = 1;
}

void MainFrame::OnGLCanvas1MouseMove(wxMouseEvent& event)
{
    if (marqueeRegisteredDown)
    {
	marqueeEnd = event.GetPosition();
	ConvertWxCoordinatesToImage(marqueeEnd);
	marqueeRegisteredMotion = true;
	AdjustMarqueeAspectRatio();
    }
    else if (scrollRegisteredDown)
    {
	// start scrolling after significant motion
	wxPoint p = event.GetPosition();
	scrollRegisteredMotion = true;
	// if (abs(p.x - scrollStart.x) > 10) scrollRegisteredMotion = true;
	// if (abs(p.y - scrollStart.y) > 10) scrollRegisteredMotion = true;

	if (scrollRegisteredMotion)
	{
	    // get texture scale motion on screen
	    float sx = zoomEndX - zoomStartX;
	    float sy = zoomEndY - zoomStartY;
	    float dx = scrollStart.x - p.x;
	    float dy = scrollStart.y - p.y;

	    // scale motion to fit over texture
	    dx *= sx / glCanvasWidth;
	    dy *= sy / glCanvasHeight;

	    // add motion to zoom
	    zoomStartX += dx;
	    zoomStartY += dy;
	    zoomEndX += dx;
	    zoomEndY += dy;

	    // check that zoom stays within image boundaries
	    float d = 0 - zoomStartX;
	    if (d > 0) {zoomStartX += d; zoomEndX += d;}
	    d = 0 - zoomStartY;
	    if (d > 0) {zoomStartY += d; zoomEndY += d;}
	    d = zoomEndX - 1;
	    if (d > 0) {zoomStartX -= d; zoomEndX -= d;}
	    d = zoomEndY - 1;
	    if (d > 0) {zoomStartY -= d; zoomEndY -= d;}

	    scrollStart = p;
	}
    }
}

void MainFrame::OnGLCanvas1RightUp(wxMouseEvent& event)
{
    // zoom if a marquee selection was done
    if (marqueeRegisteredDown)
    {
	AdjustMarqueeAspectRatio();

	float zex = zoomEndX;
	float zey = zoomEndY;
	float zsx = zoomStartX;
	float zsy = zoomStartY;

	// calculate zoom in texture coordinates
	zoomEndX = zsx + (zex - zsx) * (float) marqueeEnd.x / (float)oglScreen.cols;
	zoomEndY = zsy + (zey - zsy) * (float) marqueeEnd.y / (float)oglScreen.rows;
	zoomStartX = zsx + (zex - zsx) * (float) marqueeStart.x / (float)oglScreen.cols;
	zoomStartY = zsy + (zey - zsy) * (float) marqueeStart.y / (float)oglScreen.rows;

	// rearrange so that rectangle is well oriented
	if (zoomStartX > zoomEndX) {float tmp = zoomEndX; zoomEndX = zoomStartX; zoomStartX = tmp;}
	if (zoomStartY > zoomEndY) {float tmp = zoomEndY; zoomEndY = zoomStartY; zoomStartY = tmp;}
    }

    // cancel any marquee selection
    marqueeRegisteredDown = false;
    marqueeRegisteredMotion = false;
}


void MainFrame::AdjustMarqueeAspectRatio ()
{
    int mdx = marqueeEnd.x - marqueeStart.x;
    int mdy = marqueeEnd.y - marqueeStart.y;

    float ratio = (float) oglScreen.cols / (float) oglScreen.rows;
    float mRatio = (float)abs(mdx) / (float)abs(mdy);

    if (mRatio > ratio)
    {
    	// restrict x axis
    	if ((mdx > 0 && mdy > 0) || (mdx <= 0 && mdy <= 0))
    	    marqueeEnd.x = marqueeStart.x + (float) (mdy) * ratio;
    	else
    	    marqueeEnd.x = marqueeStart.x - (float) (mdy) * ratio;
    }
    else
    {
    	// restrict y axis
    	if ((mdx > 0 && mdy > 0) || (mdx <= 0 && mdy <= 0))
    	    marqueeEnd.y = marqueeStart.y + (float) (mdx) / ratio;
    	else
    	    marqueeEnd.y = marqueeStart.y - (float) (mdx) / ratio;
    }
}

void MainFrame::ConvertWxCoordinatesToImage(wxPoint& p)
{
    int wx = p.x;
    int wy = p.y;

    // first limit coords
    if (wx < glPadX) wx = glPadX;
    if (wx >= glCanvasWidth - glPadX) wx = glCanvasWidth - glPadX - 1;
    if (wy < glPadY) wy = glPadY;
    if (wy >= glCanvasHeight - glPadY) wy = glCanvasHeight - glPadY - 1;

    // then transform
    p.x = ((float)wx - glPadX) / (glCanvasWidth - glPadX * 2) * (float) oglScreen.cols;
    p.y = ((float)wy - glPadY) / (glCanvasHeight - glPadY * 2) * (float) oglScreen.rows;
}

void MainFrame::AdjustOrthoAspectRatio (int w, int h)
{
    int glsx, glsy;
    GLCanvas1->GetClientSize(&glsx, &glsy);
    glCanvasWidth = glsx;
    glCanvasHeight = glsy;
    float glRatio = glCanvasWidth / glCanvasHeight;
    float iRatio = (float)w/(float)h;

    float orthoPadX = 0;
    float orthoPadY = 0;

    if (iRatio > glRatio)
    {
	// pad top/bottom
	orthoPadY = ((float)w / glRatio - (float)h) / 2;
    }
    else
    {
	// pad left/right
	orthoPadX = ((float)h * glRatio - (float)w) / 2;
    }
    orthoX = -orthoPadX;
    orthoWidth = w + orthoPadX * 2;
    orthoY = -orthoPadY;
    orthoHeight = h + orthoPadY * 2;

    glPadX = orthoPadX * glCanvasWidth / orthoWidth;
    glPadY = orthoPadY * glCanvasHeight / orthoHeight;
}

void MainFrame::OnGLCanvas1Resize(wxSizeEvent& event)
{
    AdjustOrthoAspectRatio (ipEngine.capture->width, ipEngine.capture->height);
    GLCanvas1->Refresh();
}

void MainFrame::OnToggleButtonProcessingToggle(wxCommandEvent& event)
{
    showProcessing = ToggleButtonProcessing->GetValue();
//    ipEngine.takeSnapshot = true; // showProcessing;
}

void MainFrame::OnSpinCtrlStartTimeChange(wxSpinEvent& event)
{
    ipEngine.startTime = event.GetPosition();
}

void MainFrame::OnSpinCtrlDurationChange(wxSpinEvent& event)
{
    ipEngine.durationTime = event.GetPosition();
}

void MainFrame::OnSpinCtrlDoubleTimestepChange(wxSpinDoubleEvent& event)
{
    ipEngine.timestep = event.GetValue();
}

void MainFrame::OnFilePickerCtrlZonesFileChanged(wxFileDirPickerEvent& event)
{
    wxString caption = wxT("Choose an image to use for zones");
    wxString wildcard = wxT("Image file (*.png)|*.png");

    wxFileName fn (ipEngine.zonesFilename);

    wxString defaultDir = fn.GetFullName();
    wxString defaultFilename = fn.GetPath();

    wxFileDialog dialog(this, caption, defaultDir, defaultFilename, wildcard, wxFD_OPEN | wxFD_FILE_MUST_EXIST);

    if (dialog.ShowModal() == wxID_OK)
    {
	wxString path = dialog.GetPath();

	// process data
	Mat zones = imread (path.ToStdString(), CV_LOAD_IMAGE_GRAYSCALE);

	if (zones.cols != ipEngine.capture->width || zones.rows != ipEngine.capture->height)
	{
	    cerr << "Zones image and video dimensions mismatch... please update your image" << std::endl;

	    wxMessageBox( wxT("Zones image and video dimensions mismatch."), wxT("An error was encountered..."), wxOK | wxICON_ERROR);
	}
	else
	{
	    ipEngine.zoneMap = zones;
	    ipEngine.zonesFilename = path.ToStdString();
	}
    }
}

void MainFrame::OnSpinCtrlBgFramesChange(wxSpinEvent& event)
{
    ipEngine.bgFrames = event.GetPosition();
}

void MainFrame::OnSpinCtrlBgEndChange(wxSpinEvent& event)
{
    ipEngine.bgEndTime = event.GetPosition();
}

void MainFrame::OnSpinCtrlBgStartChange(wxSpinEvent& event)
{
    ipEngine.bgStartTime = event.GetPosition();
}

void MainFrame::OnCheckBoxRecalculateClick(wxCommandEvent& event)
{
    ipEngine.bgRecalculate = CheckBoxRecalculate->GetValue();
}

void MainFrame::OnRadioBoxMethodSelect(wxCommandEvent& event)
{
    string sel = RadioBoxMethod->GetStringSelection().ToStdString();

    if (!sel.empty())
    {
	if (sel == "median") ipEngine.bgCalcType = ImageProcessingEngine::BG_MEDIAN;
	if (sel == "mean") ipEngine.bgCalcType = ImageProcessingEngine::BG_MEAN;
    }
}

void MainFrame::UpdateUI ()
{
    SpinCtrlBgStart->SetValue(ipEngine.bgStartTime);
    SpinCtrlBgEnd->SetValue(ipEngine.bgEndTime);
    SpinCtrlBgFrames->SetValue(ipEngine.bgFrames);
    CheckBoxRecalculate->SetValue(ipEngine.bgRecalculate);
    if (ipEngine.bgCalcType == ImageProcessingEngine::BG_MEDIAN) RadioBoxMethod->SetStringSelection("median");
    if (ipEngine.bgCalcType == ImageProcessingEngine::BG_MEAN) RadioBoxMethod->SetStringSelection("mean");

    SpinCtrlStartTime->SetValue(ipEngine.startTime);
    SpinCtrlDuration->SetValue(ipEngine.durationTime);
    SpinCtrlTimestepDouble->SetValue(ipEngine.timestep);

    if (!parameters.zonesFilename.empty())
	FilePickerCtrlZones->SetPath(parameters.zonesFilename);
    else
	FilePickerCtrlZones->SetPath(ipEngine.zonesFilename);
}

void MainFrame::ResetImageProcessingEngine(Parameters& parameters)
{
    ipEngine.Reset(parameters);
    UpdateUI();
    AdjustOrthoAspectRatio (ipEngine.capture->width, ipEngine.capture->height);
    GLCanvas1->Refresh();

    // regenerate and assign hud / empty frame
    hud.create(ipEngine.capture->height, ipEngine.capture->width, CV_8UC4);
    hudApp.create(ipEngine.capture->height, ipEngine.capture->width, CV_8UC4);
    ipEngine.hud = hud;
    ipEngine.takeSnapshot = true;

    // clean UI pipeline
    for (unsigned int i = 0; i < pipelineDialogs.size(); i++)
    {
	delete pipelineDialogs[i];
    }
    pipelineDialogs.clear();
    ListBoxPipeline->Clear();
    ListBoxPipeline->Refresh();

    // reset and reload plugins
    FileNode fn = parameters.rootNode["Pipeline"];
    if (!fn.empty())
    {
	FileNodeIterator it = fn.begin(), it_end = fn.end();
        for (; it != it_end; ++it)
	{
	    FileNode fn = *((*it).begin()); // ugly hack to go around duplicate key bug
    	    string txt = CamelCaseToText(fn.name());
    	    AddPipelinePlugin (txt, fn);
	}
    }
}

void MainFrame::ResetImageProcessingEngine()
{
    // regenerate and assign hud / empty frame
    hud.create(ipEngine.capture->height, ipEngine.capture->width, CV_8UC4);
    hudApp.create(ipEngine.capture->height, ipEngine.capture->width, CV_8UC4);
    ipEngine.hud = hud;

    ipEngine.Reset();
    UpdateUI();
    AdjustOrthoAspectRatio (ipEngine.capture->width, ipEngine.capture->height);
    GLCanvas1->Refresh();
}

void MainFrame::OnMenuOpenCaptureSelected(wxCommandEvent& event)
{
    // press stop...
    buttonPlay->SetBitmap(wxBitmap(wxImage(_T("/usr/local/share/useTracker/images/Actions-media-playback-start-icon (1).png"))));
    buttonPlay->Refresh();
    videoSlider->SetValue(0);
    ipEngine.capture->Stop();
    play = false;

    while (1)
    {
	// show dialog
	DialogOpenCapture dialog(this);

	// use capture if possible
	if (dialog.ShowModal() == wxID_OK)
	{
	    if (dialog.capture->type != Capture::NONE)
	    {
		delete ipEngine.capture;
		ipEngine.capture = dialog.capture;
		ResetImageProcessingEngine();
		AdjustOrthoAspectRatio (ipEngine.capture->width, ipEngine.capture->height);
		GLCanvas1->Refresh();
		break;
	    }
	    else
	    {
		wxMessageBox( wxT("Could not open this source, please select another one."), wxT("An error was encountered..."), wxOK | wxICON_ERROR);

		// the capture object was created by dialog and won't be deleted there
		delete dialog.capture;
	    }
	}
	else break;
    }
}

void MainFrame::OnGLCanvas1Char(wxKeyEvent& event)
{
    wxCommandEvent e;
    switch (event.GetKeyCode())
    {
    case WXK_SPACE:
	OnbuttonPlayClick(e);
	break;

    case WXK_RIGHT:
	OnbuttonStepForwardClick(e);
	break;

    case WXK_LEFT:
	OnbuttonStepBackwardsClick(e);
	break;

    case WXK_CONTROL_R:
	OnbuttonOutputClick(e);
	break;

    case WXK_CONTROL_O:
	OnMenuOpenCaptureSelected(e);
	break;

    case WXK_CONTROL_L:
	OnMenuLoadSettingsSelected(e);
	break;

    case WXK_CONTROL_S:
	OnMenuSaveSettingsSelected(e);
	break;

    case '+':
    case '=':
	OnbuttonForwardClick(e);
	break;

    case '-':
	OnbuttonBackwardsClick(e);
	break;

    case WXK_BACK:
	if (ipEngine.capture->type == Capture::VIDEO)
	{
	    CaptureVideo* cap = dynamic_cast<CaptureVideo*> (ipEngine.capture);
	    playSpeed = 0;
	    cap->SetSpeedFaster(1);
	}
	break;
    }
}

void MainFrame::OnGLCanvas1MouseEnter(wxMouseEvent& event)
{
    GLCanvas1->SetFocus();
}

void MainFrame::OnCheckBoxUseTimeBoundsClick(wxCommandEvent& event)
{
    ipEngine.useTimeBoundaries = event.IsChecked();
}
