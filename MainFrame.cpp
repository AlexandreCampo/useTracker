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
#include "DialogAruco.h"
#include "DialogStopwatch.h"
#include "DialogSimpleTags.h"
#include "DialogAdaptiveThreshold.h"

#include "Utils.h"
#include "Capture.h"
#include "CaptureVideo.h"
#include "CaptureUSBCamera.h"
#include "CaptureImage.h"
#include "CaptureAVTCamera.h"
#include "CaptureMultiUSBCamera.h"
#include "CaptureMultiVideo.h"
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
const long MainFrame::ID_CHECKBOX6 = wxNewId();
const long MainFrame::ID_STATICBITMAP4 = wxNewId();
const long MainFrame::ID_BUTTON9 = wxNewId();
const long MainFrame::ID_STATICBITMAP5 = wxNewId();
const long MainFrame::ID_BUTTON10 = wxNewId();
const long MainFrame::ID_BITMAPBUTTON13 = wxNewId();
const long MainFrame::ID_BUTTON8 = wxNewId();
const long MainFrame::ID_BITMAPBUTTON14 = wxNewId();
const long MainFrame::ID_BUTTON11 = wxNewId();
const long MainFrame::ID_SCROLLEDWINDOW4 = wxNewId();
const long MainFrame::ID_CHOICE1 = wxNewId();
const long MainFrame::ID_RADIOBOX2 = wxNewId();
const long MainFrame::ID_STATICTEXT10 = wxNewId();
const long MainFrame::ID_SPINCTRL4 = wxNewId();
const long MainFrame::ID_STATICTEXT11 = wxNewId();
const long MainFrame::ID_SPINCTRL5 = wxNewId();
const long MainFrame::ID_STATICTEXT12 = wxNewId();
const long MainFrame::ID_SPINCTRL9 = wxNewId();
const long MainFrame::ID_STATICTEXT13 = wxNewId();
const long MainFrame::ID_SPINCTRL10 = wxNewId();
const long MainFrame::ID_STATICTEXT16 = wxNewId();
const long MainFrame::ID_SPINCTRL13 = wxNewId();
const long MainFrame::ID_STATICTEXT14 = wxNewId();
const long MainFrame::ID_SPINCTRL11 = wxNewId();
const long MainFrame::ID_STATICTEXT15 = wxNewId();
const long MainFrame::ID_SPINCTRL12 = wxNewId();
const long MainFrame::ID_CHECKBOX3 = wxNewId();
const long MainFrame::ID_CHECKBOX4 = wxNewId();
const long MainFrame::ID_CHECKBOX5 = wxNewId();
const long MainFrame::ID_BITMAPBUTTON5 = wxNewId();
const long MainFrame::ID_BUTTON2 = wxNewId();
const long MainFrame::ID_BITMAPBUTTON12 = wxNewId();
const long MainFrame::ID_BUTTON7 = wxNewId();
const long MainFrame::ID_BITMAPBUTTON6 = wxNewId();
const long MainFrame::ID_BUTTON5 = wxNewId();
const long MainFrame::ID_BITMAPBUTTON7 = wxNewId();
const long MainFrame::ID_BUTTON6 = wxNewId();
const long MainFrame::ID_SCROLLEDWINDOW3 = wxNewId();
const long MainFrame::ID_NOTEBOOK1 = wxNewId();
const long MainFrame::idMenuOpenSource = wxNewId();
const long MainFrame::idMenuSaveSource = wxNewId();
const long MainFrame::idMenuLoadSettings = wxNewId();
const long MainFrame::idMenuSaveSettings = wxNewId();
const long MainFrame::idMenuQuit = wxNewId();
const long MainFrame::idMenuAbout = wxNewId();
//*)

// const long MainFrame::ID_SPINCTRL6x = wxNewId();
// const long MainFrame::ID_SPINCTRL7x = wxNewId();
const long MainFrame::ID_SPINCTRL8x = wxNewId();
const long MainFrame::ID_SPINCTRL13x = wxNewId();


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
    wxFlexGridSizer* FlexGridSizer8;
    wxFlexGridSizer* FlexGridSizer1;
    wxFlexGridSizer* FlexGridSizer2;
    wxFlexGridSizer* FlexGridSizer15;
    wxMenu* Menu1;
    wxFlexGridSizer* FlexGridSizer11;
    wxFlexGridSizer* FlexGridSizer7;
    wxStaticBoxSizer* StaticBoxSizerConfigMultiSource;
    wxFlexGridSizer* FlexGridSizer4;
    wxFlexGridSizer* FlexGridSizer9;
    wxStaticBoxSizer* StaticBoxSizer3;
    wxFlexGridSizer* FlexGridSizer14;
    wxFlexGridSizer* FlexGridSizer6;
    wxFlexGridSizer* FlexGridSizer3;
    wxFlexGridSizer* FlexGridSizerCalibMain;
    wxStaticBoxSizer* StaticBoxSizer4;
    wxStaticBoxSizer* StaticBoxSizerCalibSubdevices;
    wxFlexGridSizer* FlexGridSizer10;
    wxFlexGridSizer* FlexGridSizer13;
    wxMenuBar* MenuBar1;
    wxGridBagSizer* GridBagSizer1;
    wxFlexGridSizer* FlexGridSizer12;
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
    GLCanvas1 = new wxGLCanvas(this, ID_GLCANVAS1, wxDefaultPosition, wxSize(900,675), 0, _T("ID_GLCANVAS1"), GLCanvasAttributes_1);
    GLCanvas1->SetMinSize(wxSize(-1,-1));
    GLCanvas1->SetMaxSize(wxSize(-1,-1));
    GLCanvas1->SetFocus();
    FlexGridSizer6->Add(GLCanvas1, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer2 = new wxFlexGridSizer(1, 9, 0, 0);
    FlexGridSizer2->AddGrowableCol(7);
    buttonOutput = new wxBitmapButton(this, ID_BITMAPBUTTON1, wxBitmap(wxImage(_T("/usr/share/useTracker/images/Actions-media-record-icon inactive.png"))), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW, wxDefaultValidator, _T("ID_BITMAPBUTTON1"));
    FlexGridSizer2->Add(buttonOutput, 1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    buttonStop = new wxBitmapButton(this, ID_BITMAPBUTTON3, wxBitmap(wxImage(_T("/usr/share/useTracker/images/Actions-media-playback-stop-icon (1).png"))), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW|wxNO_BORDER, wxDefaultValidator, _T("ID_BITMAPBUTTON3"));
    FlexGridSizer2->Add(buttonStop, 1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    buttonBackwards = new wxBitmapButton(this, ID_BITMAPBUTTON4, wxBitmap(wxImage(_T("/usr/share/useTracker/images/Actions-media-seek-backward-icon (1).png"))), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW|wxNO_BORDER, wxDefaultValidator, _T("ID_BITMAPBUTTON4"));
    FlexGridSizer2->Add(buttonBackwards, 1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    buttonStepBackwards = new wxBitmapButton(this, ID_BITMAPBUTTON9, wxBitmap(wxImage(_T("/usr/share/useTracker/images/Actions-media-skip-backward-icon (1).png"))), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW|wxNO_BORDER, wxDefaultValidator, _T("ID_BITMAPBUTTON9"));
    FlexGridSizer2->Add(buttonStepBackwards, 1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    buttonPlay = new wxBitmapButton(this, ID_BITMAPBUTTON2, wxBitmap(wxImage(_T("/usr/share/useTracker/images/Actions-media-playback-start-icon (1).png"))), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW|wxNO_BORDER, wxDefaultValidator, _T("ID_BITMAPBUTTON2"));
    buttonPlay->SetBitmapSelected(wxBitmap(wxImage(_T("/usr/share/useTracker/images/Actions-media-playback-pause-icon (1).png"))));
    FlexGridSizer2->Add(buttonPlay, 1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    buttonStepForward = new wxBitmapButton(this, ID_BITMAPBUTTON10, wxBitmap(wxImage(_T("/usr/share/useTracker/images/Actions-media-skip-forward-icon (1).png"))), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW|wxNO_BORDER, wxDefaultValidator, _T("ID_BITMAPBUTTON10"));
    FlexGridSizer2->Add(buttonStepForward, 1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    buttonForward = new wxBitmapButton(this, ID_BITMAPBUTTON8, wxBitmap(wxImage(_T("/usr/share/useTracker/images/Actions-media-seek-forward-icon (1).png"))), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW|wxNO_BORDER, wxDefaultValidator, _T("ID_BITMAPBUTTON8"));
    FlexGridSizer2->Add(buttonForward, 1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    videoSlider = new wxSlider(this, ID_SLIDER1, 0, 0, 10000, wxDefaultPosition, wxSize(-1,-1), 0, wxDefaultValidator, _T("ID_SLIDER1"));
    videoSlider->SetMinSize(wxSize(-1,-1));
    videoSlider->SetMaxSize(wxSize(-1,-1));
    FlexGridSizer2->Add(videoSlider, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 12);
    buttonHud = new wxBitmapButton(this, ID_BITMAPBUTTON11, wxBitmap(wxImage(_T("/usr/share/useTracker/images/Apps-utilities-system-monitor-icon active (1).png"))), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW|wxNO_BORDER, wxDefaultValidator, _T("ID_BITMAPBUTTON11"));
    FlexGridSizer2->Add(buttonHud, 1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    FlexGridSizer6->Add(FlexGridSizer2, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer1->Add(FlexGridSizer6, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Notebook1 = new wxNotebook(this, ID_NOTEBOOK1, wxDefaultPosition, wxDefaultSize, 0, _T("ID_NOTEBOOK1"));
    Notebook1->SetMaxSize(wxSize(-1,650));
    ProcessingTab = new wxScrolledWindow(Notebook1, ID_SCROLLEDWINDOW1, wxDefaultPosition, wxDefaultSize, wxVSCROLL|wxHSCROLL, _T("ID_SCROLLEDWINDOW1"));
    FlexGridSizer11 = new wxFlexGridSizer(0, 1, 0, 0);
    FlexGridSizer11->AddGrowableCol(0);
    ToggleButtonProcessing = new wxToggleButton(ProcessingTab, ID_TOGGLEBUTTON1, _("Show Processing"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TOGGLEBUTTON1"));
    FlexGridSizer11->Add(ToggleButtonProcessing, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticBox1 = new wxStaticBox(ProcessingTab, ID_STATICBOX1, _("Processing pipeline"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICBOX1"));
    FlexGridSizer11->Add(StaticBox1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticBox2 = new wxStaticBox(ProcessingTab, ID_STATICBOX2, _("Available functions"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICBOX2"));
    FlexGridSizer11->Add(StaticBox2, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    ProcessingTab->SetSizer(FlexGridSizer11);
    FlexGridSizer11->Fit(ProcessingTab);
    FlexGridSizer11->SetSizeHints(ProcessingTab);
    BackgroundTab = new wxScrolledWindow(Notebook1, ID_SCROLLEDWINDOW2, wxDefaultPosition, wxDefaultSize, wxVSCROLL|wxHSCROLL, _T("ID_SCROLLEDWINDOW2"));
    FlexGridSizer10 = new wxFlexGridSizer(0, 1, 0, 0);
    FlexGridSizer10->AddGrowableCol(0);
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
    wxString __wxRadioBoxChoices_1[3] =
    {
    	_("mean"),
    	_("median"),
	_("max")
    };
    RadioBoxMethod = new wxRadioBox(BackgroundTab, ID_RADIOBOX1, wxEmptyString, wxDefaultPosition, wxDefaultSize, 3, __wxRadioBoxChoices_1, 2, 0, wxDefaultValidator, _T("ID_RADIOBOX1"));
    RadioBoxMethod->SetSelection(0);
    FlexGridSizer5->Add(RadioBoxMethod, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText4 = new wxStaticText(BackgroundTab, ID_STATICTEXT4, _("When loading"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT4"));
    FlexGridSizer5->Add(StaticText4, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    CheckBoxRecalculate = new wxCheckBox(BackgroundTab, ID_CHECKBOX1, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX1"));
    CheckBoxRecalculate->SetValue(false);
    FlexGridSizer5->Add(CheckBoxRecalculate, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    StaticBoxSizer2->Add(FlexGridSizer5, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer4 = new wxFlexGridSizer(0, 2, 0, 0);
    StaticBitmap1 = new wxStaticBitmap(BackgroundTab, ID_STATICBITMAP1, wxBitmap(wxImage(_T("/usr/share/useTracker/images/Categories-applications-system-icon (1).png"))), wxDefaultPosition, wxDefaultSize, wxSIMPLE_BORDER, _T("ID_STATICBITMAP1"));
    FlexGridSizer4->Add(StaticBitmap1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    ButtonBgRecalculate = new wxButton(BackgroundTab, ID_BUTTON1, _("Recalculate"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
    FlexGridSizer4->Add(ButtonBgRecalculate, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticBitmap2 = new wxStaticBitmap(BackgroundTab, ID_STATICBITMAP2, wxBitmap(wxImage(_T("/usr/share/useTracker/images/Actions-document-open-icon (1).png"))), wxDefaultPosition, wxDefaultSize, wxSIMPLE_BORDER, _T("ID_STATICBITMAP2"));
    FlexGridSizer4->Add(StaticBitmap2, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    ButtonBgLoad = new wxButton(BackgroundTab, ID_BUTTON3, _("Load background"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON3"));
    FlexGridSizer4->Add(ButtonBgLoad, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticBitmap3 = new wxStaticBitmap(BackgroundTab, ID_STATICBITMAP3, wxBitmap(wxImage(_T("/usr/share/useTracker/images/Actions-document-save-icon (1).png"))), wxDefaultPosition, wxDefaultSize, wxSIMPLE_BORDER, _T("ID_STATICBITMAP3"));
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
    StaticText9 = new wxStaticText(ConfigTab, ID_STATICTEXT9, _("Apply limits"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT9"));
    GridBagSizer1->Add(StaticText9, wxGBPosition(4, 0), wxDefaultSpan, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    CheckBoxUseTimeBounds = new wxCheckBox(ConfigTab, ID_CHECKBOX2, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX2"));
    CheckBoxUseTimeBounds->SetValue(false);
    GridBagSizer1->Add(CheckBoxUseTimeBounds, wxGBPosition(4, 1), wxDefaultSpan, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticBoxSizer1->Add(GridBagSizer1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 1);
    FlexGridSizer3->Add(StaticBoxSizer1, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticBoxSizer4 = new wxStaticBoxSizer(wxHORIZONTAL, ConfigTab, _("Zones of interest"));
    FilePickerCtrlZones = new wxFilePickerCtrl(ConfigTab, ID_FILEPICKERCTRL1, wxEmptyString, _("Choose an image to use for zones"), _T("*.png"), wxDefaultPosition, wxDefaultSize, wxFLP_CHANGE_DIR|wxFLP_FILE_MUST_EXIST|wxFLP_OPEN, wxDefaultValidator, _T("ID_FILEPICKERCTRL1"));
    StaticBoxSizer4->Add(FilePickerCtrlZones, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer3->Add(StaticBoxSizer4, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticBoxSizerConfigMultiSource = new wxStaticBoxSizer(wxVERTICAL, ConfigTab, _("Multi source"));
    FlexGridSizer15 = new wxFlexGridSizer(0, 1, 0, 0);
    FlexGridSizer15->AddGrowableCol(0);
    CheckBoxConfigStitchAdjustLuminosity = new wxCheckBox(ConfigTab, ID_CHECKBOX6, _("Adjust luminosity"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX6"));
    CheckBoxConfigStitchAdjustLuminosity->SetValue(false);
    FlexGridSizer15->Add(CheckBoxConfigStitchAdjustLuminosity, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer13 = new wxFlexGridSizer(0, 2, 0, 0);
    StaticBitmap4 = new wxStaticBitmap(ConfigTab, ID_STATICBITMAP4, wxBitmap(wxImage(_T("/usr/share/useTracker/images/Apps-plasmagik-icon (1).png"))), wxDefaultPosition, wxDefaultSize, wxSIMPLE_BORDER, _T("ID_STATICBITMAP4"));
    FlexGridSizer13->Add(StaticBitmap4, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    ButtonConfigStitch = new wxButton(ConfigTab, ID_BUTTON9, _("Stitch images"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON9"));
    FlexGridSizer13->Add(ButtonConfigStitch, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticBitmap5 = new wxStaticBitmap(ConfigTab, ID_STATICBITMAP5, wxBitmap(wxImage(_T("/usr/share/useTracker/images/Actions-edit-delete-icon (1).png"))), wxDefaultPosition, wxDefaultSize, wxSIMPLE_BORDER, _T("ID_STATICBITMAP5"));
    FlexGridSizer13->Add(StaticBitmap5, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    ButtonConfigResetStitching = new wxButton(ConfigTab, ID_BUTTON10, _("Reset stitching"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON10"));
    FlexGridSizer13->Add(ButtonConfigResetStitching, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BitmapButton5 = new wxBitmapButton(ConfigTab, ID_BITMAPBUTTON13, wxBitmap(wxImage(_T("/usr/share/useTracker/images/Actions-document-open-icon (1).png"))), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW, wxDefaultValidator, _T("ID_BITMAPBUTTON13"));
    FlexGridSizer13->Add(BitmapButton5, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    ButtonConfigLoadStitching = new wxButton(ConfigTab, ID_BUTTON8, _("Load stitching"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON8"));
    FlexGridSizer13->Add(ButtonConfigLoadStitching, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BitmapButton6 = new wxBitmapButton(ConfigTab, ID_BITMAPBUTTON14, wxBitmap(wxImage(_T("/usr/share/useTracker/images/Actions-document-save-icon (1).png"))), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW, wxDefaultValidator, _T("ID_BITMAPBUTTON14"));
    FlexGridSizer13->Add(BitmapButton6, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    ButtonConfigSaveStitching = new wxButton(ConfigTab, ID_BUTTON11, _("Save stitching"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON11"));
    FlexGridSizer13->Add(ButtonConfigSaveStitching, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer15->Add(FlexGridSizer13, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticBoxSizerConfigMultiSource->Add(FlexGridSizer15, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    FlexGridSizer3->Add(StaticBoxSizerConfigMultiSource, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    ConfigTab->SetSizer(FlexGridSizer3);
    FlexGridSizer3->Fit(ConfigTab);
    FlexGridSizer3->SetSizeHints(ConfigTab);
    CalibrationTab = new wxScrolledWindow(Notebook1, ID_SCROLLEDWINDOW3, wxDefaultPosition, wxDefaultSize, wxVSCROLL|wxHSCROLL, _T("ID_SCROLLEDWINDOW3"));
    FlexGridSizer7 = new wxFlexGridSizer(0, 1, 0, 0);
    FlexGridSizer7->AddGrowableCol(0);
    StaticBoxSizer3 = new wxStaticBoxSizer(wxVERTICAL, CalibrationTab, _("Calibration of the source"));
    FlexGridSizerCalibMain = new wxFlexGridSizer(0, 1, 0, 0);
    FlexGridSizerCalibMain->AddGrowableCol(0);
    StaticBoxSizerCalibSubdevices = new wxStaticBoxSizer(wxHORIZONTAL, CalibrationTab, _("Select subdevice"));
    ChoiceCalibSubdevices = new wxChoice(CalibrationTab, ID_CHOICE1, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE1"));
    StaticBoxSizerCalibSubdevices->Add(ChoiceCalibSubdevices, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizerCalibMain->Add(StaticBoxSizerCalibSubdevices, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 1);
    wxString __wxRadioBoxChoices_2[3] =
    {
    	_("Chessboard"),
    	_("Circles grid"),
    	_("Asymmetric circles grid")
    };
    RadioBoxCalibBoardType = new wxRadioBox(CalibrationTab, ID_RADIOBOX2, _("Board type"), wxDefaultPosition, wxDefaultSize, 3, __wxRadioBoxChoices_2, 3, 0, wxDefaultValidator, _T("ID_RADIOBOX2"));
    RadioBoxCalibBoardType->SetSelection(0);
    FlexGridSizerCalibMain->Add(RadioBoxCalibBoardType, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 1);
    FlexGridSizer8 = new wxFlexGridSizer(0, 2, 0, 0);
    StaticText10 = new wxStaticText(CalibrationTab, ID_STATICTEXT10, _("Columns"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT10"));
    FlexGridSizer8->Add(StaticText10, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    SpinCtrlCalibWidth = new wxSpinCtrl(CalibrationTab, ID_SPINCTRL4, _T("0"), wxDefaultPosition, wxDefaultSize, 0, 3, 100, 0, _T("ID_SPINCTRL4"));
    SpinCtrlCalibWidth->SetValue(_T("0"));
    FlexGridSizer8->Add(SpinCtrlCalibWidth, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    StaticText11 = new wxStaticText(CalibrationTab, ID_STATICTEXT11, _("Rows"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT11"));
    FlexGridSizer8->Add(StaticText11, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    SpinCtrlCalibHeight = new wxSpinCtrl(CalibrationTab, ID_SPINCTRL5, _T("0"), wxDefaultPosition, wxDefaultSize, 0, 3, 100, 0, _T("ID_SPINCTRL5"));
    SpinCtrlCalibHeight->SetValue(_T("0"));
    FlexGridSizer8->Add(SpinCtrlCalibHeight, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    StaticText12 = new wxStaticText(CalibrationTab, ID_STATICTEXT12, _("Sidelen (mm)"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT12"));
    FlexGridSizer8->Add(StaticText12, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    SpinCtrlCalibSquareSize = new wxSpinCtrl(CalibrationTab, ID_SPINCTRL9, _T("10"), wxDefaultPosition, wxDefaultSize, 0, 1, 1000, 10, _T("ID_SPINCTRL9"));
    SpinCtrlCalibSquareSize->SetValue(_T("10"));
    FlexGridSizer8->Add(SpinCtrlCalibSquareSize, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    StaticText13 = new wxStaticText(CalibrationTab, ID_STATICTEXT13, _("# of frames"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT13"));
    FlexGridSizer8->Add(StaticText13, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    SpinCtrlCalibFramesCount = new wxSpinCtrl(CalibrationTab, ID_SPINCTRL10, _T("0"), wxDefaultPosition, wxDefaultSize, 0, 0, 100, 0, _T("ID_SPINCTRL10"));
    SpinCtrlCalibFramesCount->SetValue(_T("0"));
    FlexGridSizer8->Add(SpinCtrlCalibFramesCount, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    StaticText16 = new wxStaticText(CalibrationTab, ID_STATICTEXT16, _("Frame delay (s)"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT16"));
    FlexGridSizer8->Add(StaticText16, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    SpinCtrlCalibFrameDelay = new wxSpinCtrl(CalibrationTab, ID_SPINCTRL13, _T("0"), wxDefaultPosition, wxDefaultSize, 0, 0, 100, 0, _T("ID_SPINCTRL13"));
    SpinCtrlCalibFrameDelay->SetValue(_T("0"));
    FlexGridSizer8->Add(SpinCtrlCalibFrameDelay, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    StaticText14 = new wxStaticText(CalibrationTab, ID_STATICTEXT14, _("Aspect"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT14"));
    FlexGridSizer8->Add(StaticText14, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer9 = new wxFlexGridSizer(0, 3, 0, 0);
    SpinCtrlCalibAspectNum = new wxSpinCtrl(CalibrationTab, ID_SPINCTRL11, _T("0"), wxDefaultPosition, wxDefaultSize, 0, 0, 100, 0, _T("ID_SPINCTRL11"));
    SpinCtrlCalibAspectNum->SetValue(_T("0"));
    SpinCtrlCalibAspectNum->SetMinSize(wxDLG_UNIT(CalibrationTab,wxSize(25,-1)));
    FlexGridSizer9->Add(SpinCtrlCalibAspectNum, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    StaticText15 = new wxStaticText(CalibrationTab, ID_STATICTEXT15, _(":"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT15"));
    FlexGridSizer9->Add(StaticText15, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    SpinCtrlCalibAspectDen = new wxSpinCtrl(CalibrationTab, ID_SPINCTRL12, _T("0"), wxDefaultPosition, wxDefaultSize, 0, 0, 100, 0, _T("ID_SPINCTRL12"));
    SpinCtrlCalibAspectDen->SetValue(_T("0"));
    SpinCtrlCalibAspectDen->SetMinSize(wxDLG_UNIT(CalibrationTab,wxSize(25,-1)));
    FlexGridSizer9->Add(SpinCtrlCalibAspectDen, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    FlexGridSizer8->Add(FlexGridSizer9, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    FlexGridSizerCalibMain->Add(FlexGridSizer8, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 1);
    FlexGridSizer14 = new wxFlexGridSizer(0, 1, 0, 0);
    FlexGridSizer14->AddGrowableCol(0);
    CheckBoxCalibZeroTangentDist = new wxCheckBox(CalibrationTab, ID_CHECKBOX3, _("Assume no tangential disto"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX3"));
    CheckBoxCalibZeroTangentDist->SetValue(false);
    FlexGridSizer14->Add(CheckBoxCalibZeroTangentDist, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 0);
    CheckBoxCalibFixPrincipalPoint = new wxCheckBox(CalibrationTab, ID_CHECKBOX4, _("Fix principal pt at center"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX4"));
    CheckBoxCalibFixPrincipalPoint->SetValue(false);
    FlexGridSizer14->Add(CheckBoxCalibFixPrincipalPoint, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 0);
    CheckBoxCalibFlipVertical = new wxCheckBox(CalibrationTab, ID_CHECKBOX5, _("Vertical flip"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX5"));
    CheckBoxCalibFlipVertical->SetValue(false);
    FlexGridSizer14->Add(CheckBoxCalibFlipVertical, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 0);
    FlexGridSizerCalibMain->Add(FlexGridSizer14, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 1);
    FlexGridSizer12 = new wxFlexGridSizer(0, 2, 0, 0);
    BitmapButton1 = new wxBitmapButton(CalibrationTab, ID_BITMAPBUTTON5, wxBitmap(wxImage(_T("/usr/share/useTracker/images/Categories-applications-system-icon (1).png"))), wxDefaultPosition, wxDefaultSize, wxNO_BORDER, wxDefaultValidator, _T("ID_BITMAPBUTTON5"));
    FlexGridSizer12->Add(BitmapButton1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    ButtonCalibCalculate = new wxButton(CalibrationTab, ID_BUTTON2, _("Calibrate"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON2"));
    FlexGridSizer12->Add(ButtonCalibCalculate, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    BitmapButton4 = new wxBitmapButton(CalibrationTab, ID_BITMAPBUTTON12, wxBitmap(wxImage(_T("/usr/share/useTracker/images/Actions-edit-delete-icon (1).png"))), wxDefaultPosition, wxDefaultSize, wxNO_BORDER, wxDefaultValidator, _T("ID_BITMAPBUTTON12"));
    FlexGridSizer12->Add(BitmapButton4, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    ButtonCalibReset = new wxButton(CalibrationTab, ID_BUTTON7, _("Reset"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON7"));
    FlexGridSizer12->Add(ButtonCalibReset, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    BitmapButton2 = new wxBitmapButton(CalibrationTab, ID_BITMAPBUTTON6, wxBitmap(wxImage(_T("/usr/share/useTracker/images/Actions-document-open-icon (1).png"))), wxDefaultPosition, wxDefaultSize, wxNO_BORDER, wxDefaultValidator, _T("ID_BITMAPBUTTON6"));
    FlexGridSizer12->Add(BitmapButton2, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    ButtonCalibLoad = new wxButton(CalibrationTab, ID_BUTTON5, _("Load calibration"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON5"));
    FlexGridSizer12->Add(ButtonCalibLoad, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    BitmapButton3 = new wxBitmapButton(CalibrationTab, ID_BITMAPBUTTON7, wxBitmap(wxImage(_T("/usr/share/useTracker/images/Actions-document-save-icon (1).png"))), wxDefaultPosition, wxDefaultSize, wxNO_BORDER, wxDefaultValidator, _T("ID_BITMAPBUTTON7"));
    FlexGridSizer12->Add(BitmapButton3, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    ButtonCalibSave = new wxButton(CalibrationTab, ID_BUTTON6, _("Save calibration"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON6"));
    FlexGridSizer12->Add(ButtonCalibSave, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    FlexGridSizerCalibMain->Add(FlexGridSizer12, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticBoxSizer3->Add(FlexGridSizerCalibMain, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer7->Add(StaticBoxSizer3, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    CalibrationTab->SetSizer(FlexGridSizer7);
    FlexGridSizer7->Fit(CalibrationTab);
    FlexGridSizer7->SetSizeHints(CalibrationTab);
    Notebook1->AddPage(ProcessingTab, _("Main"), false);
    Notebook1->AddPage(BackgroundTab, _("Bg"), false);
    Notebook1->AddPage(ConfigTab, _("Config"), false);
    Notebook1->AddPage(CalibrationTab, _("Calib"), false);
    FlexGridSizer1->Add(Notebook1, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    SetSizer(FlexGridSizer1);
    MenuBar1 = new wxMenuBar();
    Menu1 = new wxMenu();
    MenuOpenCapture = new wxMenuItem(Menu1, idMenuOpenSource, _("Open source\tCtrl+O"), wxEmptyString, wxITEM_NORMAL);
    Menu1->Append(MenuOpenCapture);
    MenuSaveCapture = new wxMenuItem(Menu1, idMenuSaveSource, _("Save source\tCtrl+X"), wxEmptyString, wxITEM_NORMAL);
    Menu1->Append(MenuSaveCapture);
    Menu1->Break();
    MenuLoadSettings = new wxMenuItem(Menu1, idMenuLoadSettings, _("Load settings\tCtrl+L"), wxEmptyString, wxITEM_NORMAL);
    Menu1->Append(MenuLoadSettings);
    MenuSaveSettings = new wxMenuItem(Menu1, idMenuSaveSettings, _("Save settings\tCtrl+S"), wxEmptyString, wxITEM_NORMAL);
    Menu1->Append(MenuSaveSettings);
    Menu1->Break();
    MenuItem1 = new wxMenuItem(Menu1, idMenuQuit, _("Quit\tAlt-F4"), _("Quit the application"), wxITEM_NORMAL);
    Menu1->Append(MenuItem1);
    MenuBar1->Append(Menu1, _("&File"));
    Menu2 = new wxMenu();
    MenuItem2 = new wxMenuItem(Menu2, idMenuAbout, _("About\tF1"), _("Show info about USE Tracker"), wxITEM_NORMAL);
    Menu2->Append(MenuItem2);
    MenuBar1->Append(Menu2, _("Help"));
    SetMenuBar(MenuBar1);
    FileDialog1 = new wxFileDialog(this, _("Select file"), wxEmptyString, wxEmptyString, wxFileSelectorDefaultWildcardStr, wxFD_DEFAULT_STYLE, wxDefaultPosition, wxDefaultSize, _T("wxFileDialog"));
    FlexGridSizer1->Fit(this);
    FlexGridSizer1->SetSizeHints(this);

    GLCanvas1->Connect(wxEVT_PAINT,(wxObjectEventFunction)&MainFrame::OnGLCanvas1Paint,0,this);
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
    Connect(ID_CHECKBOX6,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&MainFrame::OnCheckBoxConfigStitchAdjustLuminosityClick);
    Connect(ID_BUTTON9,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&MainFrame::OnButtonConfigStitchClick);
    Connect(ID_BUTTON10,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&MainFrame::OnButtonConfigResetStitchingClick);
    Connect(ID_BUTTON8,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&MainFrame::OnButtonConfigLoadStitchingClick);
    Connect(ID_BUTTON11,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&MainFrame::OnButtonConfigSaveStitchingClick);
    Connect(ID_CHOICE1,wxEVT_COMMAND_CHOICE_SELECTED,(wxObjectEventFunction)&MainFrame::OnChoiceCalibSubdevicesSelect);
    Connect(ID_RADIOBOX2,wxEVT_COMMAND_RADIOBOX_SELECTED,(wxObjectEventFunction)&MainFrame::OnRadioBoxCalibBoardTypeSelect);
    Connect(ID_SPINCTRL4,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&MainFrame::OnSpinCtrlCalibWidthChange);
    Connect(ID_SPINCTRL5,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&MainFrame::OnSpinCtrlCalibHeightChange);
    Connect(ID_SPINCTRL9,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&MainFrame::OnSpinCtrlCalibSquareSizeChange);
    Connect(ID_SPINCTRL10,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&MainFrame::OnSpinCtrlCalibFramesCountChange);
    Connect(ID_SPINCTRL13,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&MainFrame::OnSpinCtrlCalibFrameDelayChange);
    Connect(ID_SPINCTRL11,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&MainFrame::OnSpinCtrlCalibAspectNumChange);
    Connect(ID_SPINCTRL12,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&MainFrame::OnSpinCtrlCalibAspectDenChange);
    Connect(ID_CHECKBOX3,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&MainFrame::OnCheckBoxCalibZeroTangentDistClick);
    Connect(ID_CHECKBOX4,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&MainFrame::OnCheckBoxCalibFixPrincipalPointClick);
    Connect(ID_CHECKBOX5,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&MainFrame::OnCheckBoxCalibFlipVerticalClick);
    Connect(ID_BUTTON2,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&MainFrame::OnButtonCalibCalculateClick);
    Connect(ID_BUTTON7,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&MainFrame::OnButtonCalibResetClick);
    Connect(ID_BUTTON5,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&MainFrame::OnButtonCalibLoadClick);
    Connect(ID_BUTTON6,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&MainFrame::OnButtonCalibSaveClick);
    Connect(ID_NOTEBOOK1,wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED,(wxObjectEventFunction)&MainFrame::OnNotebook1PageChanged);
    Connect(idMenuOpenSource,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&MainFrame::OnMenuOpenCaptureSelected);
    Connect(idMenuSaveSource,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&MainFrame::OnMenuSaveCaptureSelected);
    Connect(idMenuLoadSettings,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&MainFrame::OnMenuLoadSettingsSelected);
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
    SpinCtrlTimestep->Hide();

    SpinCtrlCalibFrameDelayDouble = new wxSpinCtrlDouble(CalibrationTab, ID_SPINCTRL13x, _T("0"), wxDefaultPosition, wxDefaultSize, 0, -1, 10000, 0, 0.01, _T("ID_SPINCTRL13"));
    SpinCtrlCalibFrameDelayDouble->SetValue(_T("0"));
    FlexGridSizer8->Replace(SpinCtrlCalibFrameDelay, SpinCtrlCalibFrameDelayDouble);
    SpinCtrlCalibFrameDelayDouble->Bind(wxEVT_SPINCTRLDOUBLE, &MainFrame::OnSpinCtrlDoubleCalibFrameDelayChange, this);
    SpinCtrlCalibFrameDelay->Hide();
//    FlexGridSizer8->Layout();
//    FlexGridSizer7->Fit(CalibrationTab);

    // save a sizer to show/hide
    StaticBoxSizerCalibSubdevices2 = StaticBoxSizerCalibSubdevices;
    StaticBoxSizerConfigMultiSource2 = StaticBoxSizerConfigMultiSource;
//    FlexGridSizerCalibStitch2 = FlexGridSizerCalibStitch;
//    FlexGridSizerCalibMain2 = FlexGridSizerCalibMain;

    // adjust scrollbars in tabs
    int sz = FlexGridSizer3->CalcMin().GetHeight() * 115 / 100;
    ConfigTab->SetScrollbars (0, 5, 0, sz/5);
    sz = FlexGridSizer11->CalcMin().GetHeight() * 115 / 100;
    ProcessingTab->SetScrollbars (0, 5, 0, sz/5);
    sz = FlexGridSizer10->CalcMin().GetHeight() * 115 / 100;
    BackgroundTab->SetScrollbars (0, 5, 0, sz/5);

    // setup drag and drop for image processing pipeline
    ListBoxPipeline = new wxCheckListBox(StaticBox1, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(ProcessingTab,wxSize(115,100)), 0, 0, wxLB_SINGLE|wxLB_NEEDED_SB);
    ListBoxPipelinePlugins = new wxListBox(StaticBox2, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(ProcessingTab,wxSize(115,180)), 0, 0, wxLB_SINGLE);

    textDropTargetAdd = new MyTextDropTargetAdd (this);
    textDropTargetMove = new MyTextDropTargetMove (this);
    textDropTargetRemove = new MyTextDropTargetRemove (this);

    ListBoxPipelinePlugins->Bind(wxEVT_MOTION, &MainFrame::OnListBoxPipelinePluginsBeginDrag, this);
    ListBoxPipeline->Bind(wxEVT_MOTION, &MainFrame::OnListBoxPipelineBeginDrag, this);
    ListBoxPipeline->Bind(wxEVT_LISTBOX_DCLICK, &MainFrame::OnListBoxPipelineDClick, this);
    ListBoxPipeline->Bind(wxEVT_CHECKLISTBOX, &MainFrame::OnListBoxPipelineCheck, this);

    ListBoxPipelinePlugins->Append("zones of interest");
    ListBoxPipelinePlugins->Append("background difference");
    ListBoxPipelinePlugins->Append("adaptive threshold");
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
    ListBoxPipelinePlugins->Append("stopwatch");
    ListBoxPipelinePlugins->Append("remote control");

    #ifdef ARUCO
    ListBoxPipelinePlugins->Append("aruco");
    #endif

    // connect char events
    ConnectCharEvent();

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

    // load capture source from command line params if possible
    if (!parameters.inputFilename.empty())
    {
	// check extension
	wxFileName f (parameters.inputFilename);
	if (f.GetExt() == "xml")
	{
	    // TODO duplicated from dialog open capture. Needs a cleaner solution
	    std::string filename = parameters.inputFilename;
	    cv::FileStorage file;
	    cv::FileNode rootNode;

	    file.open(filename, cv::FileStorage::READ);
	    if (file.isOpened())
	    {
		rootNode = file["Source"];

		if (!rootNode.empty())
		{
		    string type = (string)rootNode["Type"];

		    if (type == "multiVideo")
		    {
			ipEngine.capture->LoadXML(rootNode);
		    }
		    else if (type == "video")
		    {
			ipEngine.capture = new CaptureVideo(rootNode);
		    }
		    else if (type == "USBcamera")
		    {
			ipEngine.capture = new CaptureUSBCamera(rootNode);
		    }
		    else if (type == "image")
		    {
			ipEngine.capture = new CaptureImage(rootNode);
		    }
#ifdef VIMBA
		    else if (type == "AVTcamera")
		    {
			ipEngine.capture = new CaptureAVTCamera(rootNode);
		    }
#endif // VIMBA
		    else if (type == "multiUSBcamera")
		    {
			ipEngine.capture = new CaptureMultiUSBCamera(rootNode);
		    }
		}
	    }
	}
	else
	{
	    // try to load input as video file
	    ipEngine.capture = new CaptureVideo (parameters.inputFilename);

	    // if video not loaded, try image
	    if (ipEngine.capture->type == Capture::NONE)
	    {
		delete ipEngine.capture;
		ipEngine.capture = new CaptureImage (parameters.inputFilename);
	    }
	}
    }
    else if (parameters.usbDevice >= 0)
    {
	ipEngine.capture = new CaptureUSBCamera (parameters.usbDevice);
    }
    else if (parameters.avtDevice >= 0)
    {
#ifdef VIMBA
	ipEngine.capture = new CaptureAVTCamera (parameters.avtDevice);
#endif //VIMBA
    }
    else if (parameters.multiUSBCapture == true)
    {
	CaptureMultiUSBCamera* mu = new CaptureMultiUSBCamera (parameters.usbDevices);
	ipEngine.capture = mu;
	if (mu && !parameters.stitchingFilename.empty())
	{
	    cv::FileStorage file (parameters.stitchingFilename, FileStorage::READ);
	    if (file.isOpened())
	    {
		cv::FileNode rootNode = file["Source"];
		mu->LoadXML(rootNode);
	    }
	}
    }
    else if (parameters.multiVideoCapture == true)
    {
	CaptureMultiVideo* mv = new CaptureMultiVideo (parameters.inputFilenames);
	ipEngine.capture = mv;
	if (mv && !parameters.stitchingFilename.empty())
	{
	    cv::FileStorage file (parameters.stitchingFilename, FileStorage::READ);
	    if (file.isOpened())
	    {
		cv::FileNode rootNode = file["Source"];
		mv->LoadXML(rootNode, true);
	    }
	}
    }

    if (ipEngine.capture && !parameters.calibrationFilename.empty())
    {
	cv::FileStorage file (parameters.calibrationFilename, FileStorage::READ);
	if (file.isOpened())
	{
	    cv::FileNode rootNode = file["Calibration"];
	    ipEngine.capture->calibration.LoadXML(rootNode);
	}
    }

    if (!ipEngine.capture) ipEngine.capture = new CaptureDefault();
    wxString str = "USE Tracker: ";
    this->SetTitle(str + ipEngine.capture->GetName());

    ResetImageProcessingEngine(parameters);

    // last step...
    Connect( wxID_ANY, wxEVT_IDLE, wxIdleEventHandler(MainFrame::OnIdle) );
}

MainFrame::~MainFrame()
{
    // invalidate this ptr to avoid crash at event disconnection
    Notebook1 = nullptr;

    //(*Destroy(MainFrame)
    //*)
}

void MainFrame::OnQuit(wxCommandEvent& event)
{
    Close();
}

void MainFrame::OnAbout(wxCommandEvent& event)
{
    wxString msg = _("Find updates, documentation, and contact info at :\n http://usetracker.org\n\n USE Tracker is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version. \n\n USE Tracker is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details. \n\n You should have received a copy of the GNU General Public License along with USE Tracker.  If not, see <http://www.gnu.org/licenses/>. ");
	wxMessageBox(msg, _("About USE Tracker"));
}

void MainFrame::OnGLCanvas1Paint(wxPaintEvent& event)
{
    Mat oglScreenScaled;
    Mat hudScaled;

    // resize oglscreen in case it is too big....
    if (oglScreen.cols >= 4096  || oglScreen.rows >= 4096)
    {
	Mat tmpScreen;
	Mat tmpHud;

	float coeff = 1;
	if (oglScreen.cols > oglScreen.rows)
	    coeff = oglScreen.cols / 4096.0;
	else
	    coeff = oglScreen.rows / 4096.0;

	Size sz (oglScreen.cols / coeff, oglScreen.rows / coeff);
	resize (oglScreen, oglScreenScaled, sz);
	resize (hud, hudScaled, sz);
    }
    else
    {
	oglScreenScaled = oglScreen; // .clone();
	hudScaled = hud;
    }

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
    if ((activeTab == ProcessingTab) && showProcessing)
    {
    	glTexImage2D(
    	    GL_TEXTURE_2D,
    	    0,
    	    GL_RGB,
    	    oglScreenScaled.cols,
    	    oglScreenScaled.rows,
    	    0,
    	    GL_LUMINANCE,
    	    GL_UNSIGNED_BYTE,
    	    oglScreenScaled.data);
    }
    // BGR image
    else
    {
	glTexImage2D(
	    GL_TEXTURE_2D,
	    0,
	    GL_RGB,
    	    oglScreenScaled.cols,
    	    oglScreenScaled.rows,
	    0,
	    GL_BGR,
	    GL_UNSIGNED_BYTE,
	    oglScreenScaled.data);
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
    if (hudVisible && (activeTab != BackgroundTab))
    {
	// Create Texture
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(
	    GL_TEXTURE_2D,
	    0,
	    GL_RGBA,
	    hudScaled.cols,
	    hudScaled.rows,
	    0,
	    GL_BGRA,
	    GL_UNSIGNED_BYTE,
	    hudScaled.data);

	// Draw a textured quad
	glColor4f (1.0f, 1.0f, 1.0f, 0.5f);
	glBegin(GL_QUADS);
	glTexCoord2f(zoomStartX, zoomStartY); glVertex2f(0.0f, 0.0f);
	glTexCoord2f(zoomEndX, zoomStartY); glVertex2f(oglScreen.cols, 0.0f);
	glTexCoord2f(zoomEndX, zoomEndY); glVertex2f(oglScreen.cols, oglScreen.rows);
	glTexCoord2f(zoomStartX, zoomEndY); glVertex2f(0.0f, oglScreen.rows);
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
	glTexCoord2f(1.0f, 0.0f); glVertex2f(oglScreen.cols, 0.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex2f(oglScreen.cols, oglScreen.rows);
	glTexCoord2f(0.0f, 1.0f); glVertex2f(0.0f, oglScreen.rows);
	glEnd();
    }

//    SaveMatToPNG (hud, "debugStitchedHud.png");

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
	else
	{
	    wxMicroSleep(33000);
	    getNextFrame = false;
	}
    }

    // process frames only out of bg tab & calib tab
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
		    buttonPlay->SetBitmap(wxBitmap(wxImage(_T("/usr/share/useTracker/images/Actions-media-playback-start-icon (1).png"))));
		    buttonPlay->Refresh();
		    play = false;
		}
	    }
	}

	// print hud
	hudApp.setTo (0);
	if (hudVisible) PrintInfoToHud();

	if (activeTab != CalibrationTab)
	{
	    ipEngine.Step(hudVisible);
	}
	else if (activeTab == CalibrationTab)
	{
	    ipEngine.capture->Calibrate();
	    ipEngine.capture->CalibrationOutputHud(hud);
	}
    }

    // draw the main graphics screen
    if (activeTab == ProcessingTab && showProcessing)
	oglScreen = ipEngine.pipelineSnapshot;
    else if (activeTab == ProcessingTab || activeTab == ConfigTab)
	oglScreen = ipEngine.capture->frame;
    else if (activeTab == CalibrationTab)
	oglScreen = ipEngine.capture->CalibrationGetFrame();
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
    if (ipEngine.capture->type == Capture::VIDEO || ipEngine.capture->type == Capture::MULTI_VIDEO)
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
	float coeffX = (float) hudApp.cols / (float) hud.cols;
	float coeffY = (float) hudApp.rows / (float) hud.rows;
	rectangle(hudApp, Point(marqueeStart.x * coeffX, marqueeStart.y * coeffY), Point(marqueeEnd.x * coeffX, marqueeEnd.y * coeffY), Scalar (255, 255, 255, 255), 2);
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
	buttonOutput->SetBitmap(wxBitmap(wxImage(_T("/usr/share/useTracker/images/Actions-media-record-icon active.png"))));
	buttonOutput->Refresh();
	ipEngine.OpenOutput();
	output = true;
    }
    else
    {
	buttonOutput->SetBitmap(wxBitmap(wxImage(_T("/usr/share/useTracker/images/Actions-media-record-icon inactive.png"))));
	buttonOutput->Refresh();
	ipEngine.CloseOutput();
	output = false;
    }
}

void MainFrame::OnbuttonPlayClick(wxCommandEvent& event)
{
    if (!play)
    {
	buttonPlay->SetBitmap(wxBitmap(wxImage(_T("/usr/share/useTracker/images/Actions-media-playback-pause-icon (1).png"))));
	buttonPlay->Refresh();
	play = true;
	manualPlay = false;
	ipEngine.capture->Play();
    }
    else
    {
	buttonPlay->SetBitmap(wxBitmap(wxImage(_T("/usr/share/useTracker/images/Actions-media-playback-start-icon (1).png"))));
	buttonPlay->Refresh();
	play = false;
	ipEngine.capture->Pause();
    }
}

void MainFrame::OnbuttonStopClick(wxCommandEvent& event)
{
    buttonPlay->SetBitmap(wxBitmap(wxImage(_T("/usr/share/useTracker/images/Actions-media-playback-start-icon (1).png"))));
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

	MovingAverage* movingAverage = dynamic_cast<MovingAverage*>(pp);
	if (movingAverage) movingAverage->ClearHistory();
    }
}

void MainFrame::OnvideoSliderCmdScrollChanged(wxScrollEvent& event)
{
    if(ipEngine.capture->type == Capture::VIDEO || ipEngine.capture->type == Capture::MULTI_VIDEO)
    {
	int targetFrame = videoSlider->GetValue() * ipEngine.capture->GetFrameCount() / 10000;

	CaptureVideo* capv = dynamic_cast<CaptureVideo*>(ipEngine.capture);
	if (capv) capv->SetFrameNumber(targetFrame);

	CaptureMultiVideo* mcapv = dynamic_cast<CaptureMultiVideo*>(ipEngine.capture);
	if (mcapv) mcapv->SetFrameNumber(targetFrame);

	sliderMoving = false;
	videoSlider->UnsetToolTip();

	// clear any tracking history (if tracker plugin is present)
	for (unsigned int i= 0; i < ipEngine.pipelines[0].plugins.size(); i++)
	{
	    PipelinePlugin* pp = ipEngine.pipelines[ipEngine.threadsCount].plugins[i];

	    Tracker* tracker = dynamic_cast<Tracker*>(pp);
	    if (tracker && !tracker->replay) tracker->ClearHistory();

	    MovingAverage* movingAverage = dynamic_cast<MovingAverage*>(pp);
	    if (movingAverage) movingAverage->ClearHistory();
	}
    }
}

void MainFrame::OnvideoSliderCmdScrollThumbTrack(wxScrollEvent& event)
{
    if (ipEngine.capture->type == Capture::VIDEO || ipEngine.capture->type == Capture::MULTI_VIDEO)
    {
	wxLongLong s;
	int targetFrame = videoSlider->GetValue() * ipEngine.capture->GetFrameCount() / 10;
	s.Assign(targetFrame / ipEngine.capture->fps);
	wxTimeSpan ts (0,0,0,s);
	wxString f ("%H:%M:%S");
	videoSlider->SetToolTip(ts.Format(f));
	videoSlider->GetToolTip()->SetDelay(50);
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
    else if (ipEngine.bgCalcType == ImageProcessingEngine::BG_MAX)
	newBg = CalculateBackgroundMax (ipEngine.capture, ipEngine.bgStartTime, ipEngine.bgEndTime, ipEngine.bgFrames);
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
    else if (Notebook1->GetSelection() == 3)
    {
	activeTab = CalibrationTab;
	ipEngine.takeSnapshot = false;
    }

    // update viewport as a function of new view
    if (activeTab == CalibrationTab)
    {
	Size sz = ipEngine.capture->CalibrationGetFrame().size();
	AdjustOrthoAspectRatio (sz.width, sz.height);
    }
    else
    {
	AdjustOrthoAspectRatio (ipEngine.capture->width, ipEngine.capture->height);
    }
    UpdateUI();
    GLCanvas1->Refresh();
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
	    bg.copyTo(ipEngine.background);
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

    CaptureMultiVideo* mcap = dynamic_cast<CaptureMultiVideo*> (ipEngine.capture);
    if (mcap)
    {
	playSpeed++;

	if (playSpeed >= 0)
	    mcap->SetSpeedFaster(playSpeed + 1);
	else
	    mcap->SetSpeedSlower(-playSpeed + 1);
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
    CaptureMultiVideo* mcap = dynamic_cast<CaptureMultiVideo*> (ipEngine.capture);
    if (mcap)
    {
	playSpeed--;

	if (playSpeed >= 0)
	    mcap->SetSpeedFaster(playSpeed + 1);
	else
	    mcap->SetSpeedSlower(-playSpeed + 1);
    }
}

void MainFrame::OnbuttonStepForwardClick(wxCommandEvent& event)
{
    // disable play as we enter frame by frame
    buttonPlay->SetBitmap(wxBitmap(wxImage(_T("/usr/share/useTracker/images/Actions-media-playback-start-icon (1).png"))));
    buttonPlay->Refresh();
    play = false;
    manualPlay = true;
    ipEngine.capture->Pause();

    // request next frame manually
    if (!ipEngine.GetNextFrame())
    {
	// did not get a frame, pause
	buttonPlay->SetBitmap(wxBitmap(wxImage(_T("/usr/share/useTracker/images/Actions-media-playback-start-icon (1).png"))));
	buttonPlay->Refresh();
	play = false;
    }
}

void MainFrame::OnbuttonStepBackwardsClick(wxCommandEvent& event)
{
    if(ipEngine.capture->type == Capture::VIDEO || ipEngine.capture->type == Capture::MULTI_VIDEO)
    {
	CaptureVideo* capv = dynamic_cast<CaptureVideo*>(ipEngine.capture);
	CaptureMultiVideo* mcapv = dynamic_cast<CaptureMultiVideo*>(ipEngine.capture);

	// disable play as we enter frame by frame
	buttonPlay->SetBitmap(wxBitmap(wxImage(_T("/usr/share/useTracker/images/Actions-media-playback-start-icon (1).png"))));
	buttonPlay->Refresh();
	play = false;
	manualPlay = true;
	if (capv) capv->Pause();
	if (mcapv) mcapv->Pause();

	// jump video to previous frame...
	if (capv) capv->GetPreviousFrame();
	if (mcapv) mcapv->GetPreviousFrame();
    }
}

void MainFrame::OnbuttonHudClick(wxCommandEvent& event)
{
    if (hudVisible == true)
    {
	buttonHud->SetBitmap(wxBitmap(wxImage(_T("/usr/share/useTracker/images/Apps-utilities-system-monitor-icon inactive (1).png"))));
	buttonPlay->Refresh();
	hudVisible = false;
    }
    else
    {
	buttonHud->SetBitmap(wxBitmap(wxImage(_T("/usr/share/useTracker/images/Apps-utilities-system-monitor-icon active (1).png"))));
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
    else if (str == "adaptive threshold")
    {
	DialogAdaptiveThreshold* dialog = new DialogAdaptiveThreshold(this);
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
    else if (str == "zones of interest")
    {
	dlg = nullptr;
    }
    #ifdef ARUCO
    else if (str == "aruco")
    {
	DialogAruco* dialog = new DialogAruco(this);
	dialog->SetPlugin(pfv);
	dlg = dialog;
    }
    #endif
    else if (str == "simple tags")
    {
	DialogSimpleTags* dialog = new DialogSimpleTags(this);
	dialog->SetPlugin(pfv);
	dlg = dialog;
    }
    else if (str == "stopwatch")
    {
	DialogStopwatch* dialog = new DialogStopwatch(this);
	dialog->SetPlugin(pfv);
	dlg = dialog;
    }
    else if (str == "remote control")
    {
	dlg = nullptr;
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
	    buttonPlay->SetBitmap(wxBitmap(wxImage(_T("/usr/share/useTracker/images/Actions-media-playback-start-icon (1).png"))));
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
	wxString path = event.GetPath();

	// process data
	Mat zones = imread (path.ToStdString(), CV_LOAD_IMAGE_GRAYSCALE);

	if (zones.cols != ipEngine.capture->width || zones.rows != ipEngine.capture->height)
	{
	    wxMessageBox( wxT("Zones image and video dimensions mismatch."), wxT("An error was encountered..."), wxOK | wxICON_ERROR);
	}
	else
	{
	    zones.copyTo(ipEngine.zoneMap);
	    ipEngine.zonesFilename = path.ToStdString();
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
	if (sel == "max") ipEngine.bgCalcType = ImageProcessingEngine::BG_MAX;
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
    if (ipEngine.bgCalcType == ImageProcessingEngine::BG_MAX) RadioBoxMethod->SetStringSelection("max");

    SpinCtrlStartTime->SetValue(ipEngine.startTime);
    SpinCtrlDuration->SetValue(ipEngine.durationTime);
    SpinCtrlTimestepDouble->SetValue(ipEngine.timestep);

    CheckBoxUseTimeBounds->SetValue(ipEngine.useTimeBoundaries);

    if (!parameters.zonesFilename.empty())
	FilePickerCtrlZones->SetPath(parameters.zonesFilename);
    else
	FilePickerCtrlZones->SetPath(ipEngine.zonesFilename);


    // calibration tab
    RadioBoxCalibBoardType->SetSelection(ipEngine.capture->CalibrationGetBoardType());
    SpinCtrlCalibWidth->SetValue(ipEngine.capture->CalibrationGetBoardWidth());
    SpinCtrlCalibHeight->SetValue(ipEngine.capture->CalibrationGetBoardHeight());
    SpinCtrlCalibSquareSize->SetValue(ipEngine.capture->CalibrationGetSquareSize());

    SpinCtrlCalibFramesCount->SetValue(ipEngine.capture->CalibrationGetFramesCount());
    SpinCtrlCalibAspectNum->SetValue(ipEngine.capture->CalibrationGetAspectNum());
    SpinCtrlCalibAspectDen->SetValue(ipEngine.capture->CalibrationGetAspectDen());
    SpinCtrlCalibFrameDelayDouble->SetValue(ipEngine.capture->CalibrationGetFrameDelay());

    CheckBoxCalibZeroTangentDist->SetValue(ipEngine.capture->CalibrationGetZeroTangentDist());
    CheckBoxCalibFixPrincipalPoint->SetValue(ipEngine.capture->CalibrationGetFixPrincipalPoint());
    CheckBoxCalibFlipVertical->SetValue(ipEngine.capture->CalibrationGetFlipVertical());

    // check if some options should be hidden or displayed
    bool multi = ipEngine.capture->type == Capture::MULTI_USB_CAMERA || ipEngine.capture->type == Capture::MULTI_VIDEO;

    StaticBoxSizerConfigMultiSource2->Show(multi);
    StaticBoxSizerCalibSubdevices2->Show(multi);
//    BitmapButtonConfigStitch->Show(multi);
//    ButtonCalibStitch->Show(multi);

//    FlexGridSizerCalibStitch2->Show(multi);
//    FlexGridSizerCalibMain2->Layout();
    //this->Fit();

    // fill subdevices names
    if (multi)
    {
	if (ipEngine.capture->type == Capture::MULTI_USB_CAMERA)
	{
	    CaptureMultiUSBCamera* c = dynamic_cast<CaptureMultiUSBCamera*> (ipEngine.capture);
	    ChoiceCalibSubdevices->Clear();
	    unsigned int n = c->GetDeviceCount();
	    for (unsigned int i = 0; i < n; i++)
	    {
		ChoiceCalibSubdevices->Append(c->GetDeviceName(i));
	    }
	    ChoiceCalibSubdevices->SetSelection(c->GetDeviceToCalibrate());
	}
	else if (ipEngine.capture->type == Capture::MULTI_VIDEO)
	{
	    CaptureMultiVideo* c = dynamic_cast<CaptureMultiVideo*> (ipEngine.capture);
	    ChoiceCalibSubdevices->Clear();
	    unsigned int n = c->GetDeviceCount();
	    for (unsigned int i = 0; i < n; i++)
	    {
		wxFileName f (c->GetDeviceName(i));
		wxString n = f.GetName();
		wxString n2 = n;
		if (n2.Length() > 23)
		{
		    n2 = n.SubString(0,20);
		    n2 << "...";
		}
		ChoiceCalibSubdevices->Append(n2);
	    }
	    ChoiceCalibSubdevices->SetSelection(c->GetDeviceToCalibrate());
	}
    }
}

void MainFrame::ResetImageProcessingEngine(Parameters& parameters)
{
    ipEngine.Reset(parameters);
    UpdateUI();
    AdjustOrthoAspectRatio (ipEngine.capture->width, ipEngine.capture->height);
    GLCanvas1->Refresh();

    // regenerate and assign hud / empty frame
    hud.create(ipEngine.capture->height, ipEngine.capture->width, CV_8UC4);
    hudApp.create(1280 * ((float) ipEngine.capture->height) / ((float) ipEngine.capture->width), 1280, CV_8UC4);
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
    hudApp.create(1280 * ((float) ipEngine.capture->height) / ((float) ipEngine.capture->width), 1280, CV_8UC4);
    // hudApp.create(ipEngine.capture->height, ipEngine.capture->width, CV_8UC4);
    ipEngine.hud = hud;

    ipEngine.Reset();
    UpdateUI();
    AdjustOrthoAspectRatio (ipEngine.capture->width, ipEngine.capture->height);
    GLCanvas1->Refresh();
}

void MainFrame::OnMenuOpenCaptureSelected(wxCommandEvent& event)
{
    // press stop...
    buttonPlay->SetBitmap(wxBitmap(wxImage(_T("/usr/share/useTracker/images/Actions-media-playback-start-icon (1).png"))));
    buttonPlay->Refresh();
    videoSlider->SetValue(0);
    ipEngine.capture->Stop();
    play = false;

    while (1)
    {
	// show dialog
	DialogOpenCapture dialog(this);
	dialog.previousCapture = ipEngine.capture;

	// use capture if possible
	if (dialog.ShowModal() == wxID_OK)
	{
	    if (dialog.capture->type != Capture::NONE)
	    {
		ipEngine.capture = dialog.capture;
		ResetImageProcessingEngine();
		AdjustOrthoAspectRatio (ipEngine.capture->width, ipEngine.capture->height);
		GLCanvas1->Refresh();

		wxString str = "USE Tracker: ";
		this->SetTitle(str + ipEngine.capture->GetName());
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

void MainFrame::OnChar(wxKeyEvent& event)
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
	CaptureVideo* cap = dynamic_cast<CaptureVideo*> (ipEngine.capture);
	if (cap)
	{
	    playSpeed = 0;
	    cap->SetSpeedFaster(1);
	}
	CaptureMultiVideo* mcap = dynamic_cast<CaptureMultiVideo*> (ipEngine.capture);
	if (mcap)
	{
	    playSpeed = 0;
	    mcap->SetSpeedFaster(1);
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

void MainFrame::ConnectCharEvent()
{
    RecursiveConnectCharEvent(this);
    RecursiveDisconnectCharEvent(Notebook1);
}

void MainFrame::DisconnectCharEvent()
{
    RecursiveDisconnectCharEvent(this);
}


void MainFrame::RecursiveConnectCharEvent(wxWindow* pclComponent)
{
    if(pclComponent)
    {
	wxDialog* dlg = dynamic_cast<wxDialog*> (pclComponent);

	if (dlg == nullptr)
	{
	    pclComponent->Connect(wxID_ANY,
				  wxEVT_CHAR,
				  wxKeyEventHandler(MainFrame::OnChar),
				  (wxObject*) NULL,
				  this);

	    wxWindowListNode* pclNode = pclComponent->GetChildren().GetFirst();
	    while(pclNode)
	    {
		wxWindow* pclChild = pclNode->GetData();
		this->RecursiveConnectCharEvent(pclChild);

		pclNode = pclNode->GetNext();
	    }
	}
    }
}

void MainFrame::RecursiveDisconnectCharEvent(wxWindow* pclComponent)
{
    if(pclComponent)
    {
	wxDialog* dlg = dynamic_cast<wxDialog*> (pclComponent);

	if (dlg == nullptr)
	{
	    pclComponent->Disconnect(wxID_ANY,
				     wxEVT_CHAR,
				     wxKeyEventHandler(MainFrame::OnChar),
				     (wxObject*) NULL,
				     this);

	    wxWindowListNode* pclNode = pclComponent->GetChildren().GetFirst();
	    while(pclNode)
	    {
		wxWindow* pclChild = pclNode->GetData();
		this->RecursiveDisconnectCharEvent(pclChild);

		pclNode = pclNode->GetNext();
	    }
	}
    }
}

bool MainFrame::IsRecording()
{
    return output;
}

void MainFrame::OnMenuCalibrateSourceSelected(wxCommandEvent& event)
{
}

void MainFrame::OnMenuLoadCalibrationDataSelected(wxCommandEvent& event)
{
}

void MainFrame::OnMenuSaveCalibrationDataSelected(wxCommandEvent& event)
{
}

// ======================================================
// Events related to calibration
// ======================================================

void MainFrame::OnButtonCalibCalculateClick(wxCommandEvent& event)
{
    ipEngine.capture->CalibrationStart();
}

void MainFrame::OnButtonCalibResetClick(wxCommandEvent& event)
{
    ipEngine.capture->CalibrationReset();
}

void MainFrame::OnButtonCalibSaveClick(wxCommandEvent& event)
{
    wxString caption = wxT("Save current calibration settings");
    wxString wildcard = wxT("XML file (*.xml)|*.xml");
    wxFileDialog dialog(this, caption, "", "", wildcard, wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

    if (dialog.ShowModal() == wxID_OK)
    {
	wxString path = dialog.GetPath();

	// open file
	FileStorage fs(path.ToStdString(), FileStorage::WRITE);

	if (fs.isOpened())
	{
	    fs << "Calibration" << "{";

	    ipEngine.capture->CalibrationSaveXML(fs);

	    fs << "}";
	    fs.release();
	}
	else
	{
	    wxMessageBox( wxT("Could not open file for saving calibration settings."), wxT("An error was encountered..."), wxOK | wxICON_ERROR);
	}
    }
}

void MainFrame::OnButtonCalibLoadClick(wxCommandEvent& event)
{
    wxString caption = wxT("Choose a calibration file to use");
    wxString wildcard = wxT("XML data (*.xml)|*.xml");

    wxFileDialog dialog(this, caption, "", "", wildcard, wxFD_OPEN | wxFD_FILE_MUST_EXIST);

    if (dialog.ShowModal() == wxID_OK)
    {
	wxString path = dialog.GetPath();

	cv::FileStorage file;
	cv::FileNode rootNode;
	file.open(path.ToStdString(), FileStorage::READ);
	if (file.isOpened())
	{
	    rootNode = file["Calibration"];

	    // process data
	    ipEngine.capture->CalibrationLoadXML(rootNode);
	}
    }
}

void MainFrame::OnRadioBoxCalibBoardTypeSelect(wxCommandEvent& event)
{
    ipEngine.capture->CalibrationSetBoardType(event.GetSelection());
}

void MainFrame::OnSpinCtrlCalibWidthChange(wxSpinEvent& event)
{
    ipEngine.capture->CalibrationSetBoardWidth(event.GetValue());
}

void MainFrame::OnSpinCtrlCalibHeightChange(wxSpinEvent& event)
{
    ipEngine.capture->CalibrationSetBoardHeight(event.GetValue());
}

void MainFrame::OnSpinCtrlCalibSquareSizeChange(wxSpinEvent& event)
{
    ipEngine.capture->CalibrationSetSquareSize(event.GetValue());
}

void MainFrame::OnSpinCtrlCalibFramesCountChange(wxSpinEvent& event)
{
    ipEngine.capture->CalibrationSetFramesCount(event.GetValue());
}

void MainFrame::OnSpinCtrlCalibAspectNumChange(wxSpinEvent& event)
{
    ipEngine.capture->CalibrationSetAspect(event.GetValue() / SpinCtrlCalibAspectDen->GetValue());
}

void MainFrame::OnSpinCtrlCalibAspectDenChange(wxSpinEvent& event)
{
    ipEngine.capture->CalibrationSetAspect(SpinCtrlCalibAspectNum->GetValue() / event.GetValue());
}

void MainFrame::OnCheckBoxCalibZeroTangentDistClick(wxCommandEvent& event)
{
    ipEngine.capture->CalibrationSetZeroTangentDist(event.IsChecked());
}

void MainFrame::OnCheckBoxCalibFixPrincipalPointClick(wxCommandEvent& event)
{
    ipEngine.capture->CalibrationSetFixPrincipalPoint(event.IsChecked());
}

void MainFrame::OnCheckBoxCalibFlipVerticalClick(wxCommandEvent& event)
{
    ipEngine.capture->CalibrationSetFlipVertical(event.IsChecked());
}

void MainFrame::OnButtonCalibStitchClick(wxCommandEvent& event)
{
}

void MainFrame::OnSpinCtrlCalibFrameDelayChange(wxSpinEvent& event)
{
    ipEngine.capture->CalibrationSetFrameDelay(event.GetValue());
}

void MainFrame::OnSpinCtrlDoubleCalibFrameDelayChange(wxSpinDoubleEvent& event)
{
    ipEngine.capture->CalibrationSetFrameDelay(event.GetValue());
}

void MainFrame::OnChoiceCalibSubdevicesSelect(wxCommandEvent& event)
{
    if (ipEngine.capture->type == Capture::MULTI_USB_CAMERA)
    {
	CaptureMultiUSBCamera* c = dynamic_cast<CaptureMultiUSBCamera*> (ipEngine.capture);
	c->SetDeviceToCalibrate(event.GetSelection());

	// adjust display to new subcapture
	Size sz = ipEngine.capture->CalibrationGetFrame().size();
	AdjustOrthoAspectRatio (sz.width, sz.height);

	// make sure calib tab shows correct settings
	UpdateUI();
    }
    if (ipEngine.capture->type == Capture::MULTI_VIDEO)
    {
	CaptureMultiVideo* c = dynamic_cast<CaptureMultiVideo*> (ipEngine.capture);
	c->SetDeviceToCalibrate(event.GetSelection());

	// adjust display to new subcapture
	Size sz = ipEngine.capture->CalibrationGetFrame().size();
	AdjustOrthoAspectRatio (sz.width, sz.height);

	// make sure calib tab shows correct settings
	UpdateUI();
    }
}

void MainFrame::OnButtonConfigStitchClick(wxCommandEvent& event)
{
    if (ipEngine.capture->type == Capture::MULTI_USB_CAMERA)
    {
	CaptureMultiUSBCamera* c = dynamic_cast<CaptureMultiUSBCamera*>(ipEngine.capture);
	c->Stitch();
	ResetImageProcessingEngine();
	AdjustOrthoAspectRatio (ipEngine.capture->width, ipEngine.capture->height);
	GLCanvas1->Refresh();
    }
    if (ipEngine.capture->type == Capture::MULTI_VIDEO)
    {
	CaptureMultiVideo* c = dynamic_cast<CaptureMultiVideo*>(ipEngine.capture);
	c->Stitch();
	ResetImageProcessingEngine();
	AdjustOrthoAspectRatio (ipEngine.capture->width, ipEngine.capture->height);
	GLCanvas1->Refresh();
    }
}

void MainFrame::OnButtonConfigResetStitchingClick(wxCommandEvent& event)
{
    if (ipEngine.capture->type == Capture::MULTI_USB_CAMERA)
    {
	CaptureMultiUSBCamera* c = dynamic_cast<CaptureMultiUSBCamera*>(ipEngine.capture);
	c->ResetStitching();
	ResetImageProcessingEngine();
	AdjustOrthoAspectRatio (ipEngine.capture->width, ipEngine.capture->height);
	GLCanvas1->Refresh();
    }
    if (ipEngine.capture->type == Capture::MULTI_VIDEO)
    {
	CaptureMultiVideo* c = dynamic_cast<CaptureMultiVideo*>(ipEngine.capture);
	c->ResetStitching();
	ResetImageProcessingEngine();
	AdjustOrthoAspectRatio (ipEngine.capture->width, ipEngine.capture->height);
	GLCanvas1->Refresh();
    }
}

void MainFrame::OnMenuSaveCaptureSelected(wxCommandEvent& event)
{
    wxString caption = wxT("Save current source configuration to an XML file");
    wxString wildcard = wxT("Source configuration file (*.xml)|*.xml");
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
	    fs << "Source" << "{";

	    ipEngine.capture->SaveXML (fs);

	    fs << "}"; // SourceConfiguration
	    fs.release();
	}
	else
	{
	    wxMessageBox( wxT("Could not open file for saving settings."), wxT("An error was encountered..."), wxOK | wxICON_ERROR);
	}
    }
}

void MainFrame::OnButtonConfigLoadStitchingClick(wxCommandEvent& event)
{
    wxString caption = wxT("Choose a stitching file to use");
    wxString wildcard = wxT("XML data (*.xml)|*.xml");

    wxFileDialog dialog(this, caption, "", "", wildcard, wxFD_OPEN | wxFD_FILE_MUST_EXIST);

    if (dialog.ShowModal() == wxID_OK)
    {
	wxString path = dialog.GetPath();

	cv::FileStorage file;
	cv::FileNode rootNode;
	file.open(path.ToStdString(), FileStorage::READ);
	if (file.isOpened())
	{
	    rootNode = file["Source"];

	    // try to cast as multi device
	    CaptureMultiVideo* cv = dynamic_cast<CaptureMultiVideo*> (ipEngine.capture);
	    if (cv)
	    {
		cv->LoadXML(rootNode, true);
		ResetImageProcessingEngine();
		AdjustOrthoAspectRatio (ipEngine.capture->width, ipEngine.capture->height);
		GLCanvas1->Refresh();
	    }

	    // TODO ADD MULTI USB
	    // CaptureMultiVideo* cv = dynamic_cast<CaptureMultiVideo*> (ipEngine.capture);
	    // if (cv)
	    // {
	    // 	cv->LoadXML(rootNode, true);
	    // }


	}
    }
}

void MainFrame::OnButtonConfigSaveStitchingClick(wxCommandEvent& event)
{
    wxString caption = wxT("Save current stitching settings");
    wxString wildcard = wxT("XML file (*.xml)|*.xml");
    wxFileDialog dialog(this, caption, "", "", wildcard, wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

    if (dialog.ShowModal() == wxID_OK)
    {
	wxString path = dialog.GetPath();

	// open file
	FileStorage fs(path.ToStdString(), FileStorage::WRITE);

	if (fs.isOpened())
	{
	    fs << "Source" << "{";

	    ipEngine.capture->SaveXML (fs);

	    fs << "}"; // SourceConfiguration
	    fs.release();
	}
	else
	{
	    wxMessageBox( wxT("Could not open file for saving stitching settings."), wxT("An error was encountered..."), wxOK | wxICON_ERROR);
	}
    }
}

void MainFrame::OnCheckBoxConfigStitchAdjustLuminosityClick(wxCommandEvent& event)
{
    if (ipEngine.capture->type == Capture::MULTI_VIDEO)
    {
	CaptureMultiVideo* c = dynamic_cast<CaptureMultiVideo*>(ipEngine.capture);
//	c->ResetStitching();
    }
}
