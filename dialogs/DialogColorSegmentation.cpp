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

#include "DialogColorSegmentation.h"

#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>

#include <vector>

using namespace cv;
using namespace std;

//(*InternalHeaders(DialogColorSegmentation)
#include <wx/string.h>
#include <wx/intl.h>
//*)

//(*IdInit(DialogColorSegmentation)
const long DialogColorSegmentation::ID_RADIOBOX1 = wxNewId();
const long DialogColorSegmentation::ID_STATICTEXT1 = wxNewId();
const long DialogColorSegmentation::ID_COLOURPICKERCTRL1 = wxNewId();
const long DialogColorSegmentation::ID_STATICTEXT3 = wxNewId();
const long DialogColorSegmentation::ID_SPINCTRL1 = wxNewId();
const long DialogColorSegmentation::ID_STATICTEXT4 = wxNewId();
const long DialogColorSegmentation::ID_SPINCTRL2 = wxNewId();
const long DialogColorSegmentation::ID_STATICTEXT5 = wxNewId();
const long DialogColorSegmentation::ID_SPINCTRL3 = wxNewId();
const long DialogColorSegmentation::ID_STATICTEXT2 = wxNewId();
const long DialogColorSegmentation::ID_COLOURPICKERCTRL2 = wxNewId();
const long DialogColorSegmentation::ID_STATICTEXT7 = wxNewId();
const long DialogColorSegmentation::ID_SPINCTRL4 = wxNewId();
const long DialogColorSegmentation::ID_STATICTEXT6 = wxNewId();
const long DialogColorSegmentation::ID_SPINCTRL5 = wxNewId();
const long DialogColorSegmentation::ID_STATICTEXT8 = wxNewId();
const long DialogColorSegmentation::ID_SPINCTRL6 = wxNewId();
const long DialogColorSegmentation::ID_RADIOBOX2 = wxNewId();
const long DialogColorSegmentation::ID_CHECKBOX1 = wxNewId();
const long DialogColorSegmentation::ID_STATICTEXT9 = wxNewId();
const long DialogColorSegmentation::ID_SPINCTRL_ZONE = wxNewId();
const long DialogColorSegmentation::ID_BUTTON1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(DialogColorSegmentation,wxDialog)
	//(*EventTable(DialogColorSegmentation)
	//*)
END_EVENT_TABLE()

DialogColorSegmentation::DialogColorSegmentation(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
	//(*Initialize(DialogColorSegmentation)
	wxFlexGridSizer* FlexGridSizer1;
	wxFlexGridSizer* FlexGridSizer2;
	wxFlexGridSizer* FlexGridSizer4;
	wxFlexGridSizer* FlexGridSizer6;
	wxFlexGridSizer* FlexGridSizer3;
	wxFlexGridSizer* FlexGridSizer5;
	wxStaticBoxSizer* StaticBoxSizer1;

	Create(parent, wxID_ANY, _("Color segmentation"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, _T("wxID_ANY"));
	SetMinSize(wxSize(-1,-1));
	SetMaxSize(wxSize(-1,-1));
	SetFocus();
	FlexGridSizer3 = new wxFlexGridSizer(0, 1, 0, 0);
	FlexGridSizer4 = new wxFlexGridSizer(0, 2, 0, 0);
	wxString __wxRadioBoxChoices_1[2] =
	{
		_("HSV"),
		_("RGB")
	};
	RadioBox1 = new wxRadioBox(this, ID_RADIOBOX1, _("Type"), wxDefaultPosition, wxDefaultSize, 2, __wxRadioBoxChoices_1, 2, 0, wxDefaultValidator, _T("ID_RADIOBOX1"));
	FlexGridSizer4->Add(RadioBox1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticBoxSizer1 = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Bounding colors"));
	FlexGridSizer1 = new wxFlexGridSizer(0, 2, 0, 0);
	StaticText1 = new wxStaticText(this, ID_STATICTEXT1, _("Lower bound color (min)"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
	FlexGridSizer1->Add(StaticText1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	ColourPickerCtrlMin = new wxColourPickerCtrl(this, ID_COLOURPICKERCTRL1, wxColour(0,0,0), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_COLOURPICKERCTRL1"));
	FlexGridSizer1->Add(ColourPickerCtrlMin, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	LowC1 = new wxStaticText(this, ID_STATICTEXT3, _("H"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT3"));
	FlexGridSizer1->Add(LowC1, 1, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 0);
	SpinCtrlLowC1 = new wxSpinCtrl(this, ID_SPINCTRL1, _T("0"), wxDefaultPosition, wxDefaultSize, 0, 0, 360, 0, _T("ID_SPINCTRL1"));
	SpinCtrlLowC1->SetValue(_T("0"));
	FlexGridSizer1->Add(SpinCtrlLowC1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	LowC2 = new wxStaticText(this, ID_STATICTEXT4, _("S"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT4"));
	FlexGridSizer1->Add(LowC2, 1, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 0);
	SpinCtrlLowC2 = new wxSpinCtrl(this, ID_SPINCTRL2, _T("0"), wxDefaultPosition, wxDefaultSize, 0, 0, 255, 0, _T("ID_SPINCTRL2"));
	SpinCtrlLowC2->SetValue(_T("0"));
	FlexGridSizer1->Add(SpinCtrlLowC2, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	LowC3 = new wxStaticText(this, ID_STATICTEXT5, _("V"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT5"));
	FlexGridSizer1->Add(LowC3, 1, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 0);
	SpinCtrlLowC3 = new wxSpinCtrl(this, ID_SPINCTRL3, _T("0"), wxDefaultPosition, wxDefaultSize, 0, 0, 255, 0, _T("ID_SPINCTRL3"));
	SpinCtrlLowC3->SetValue(_T("0"));
	FlexGridSizer1->Add(SpinCtrlLowC3, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	StaticText2 = new wxStaticText(this, ID_STATICTEXT2, _("Upper bound color (max)"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT2"));
	FlexGridSizer1->Add(StaticText2, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	ColourPickerCtrlMax = new wxColourPickerCtrl(this, ID_COLOURPICKERCTRL2, wxColour(0,0,0), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_COLOURPICKERCTRL2"));
	FlexGridSizer1->Add(ColourPickerCtrlMax, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	HighC1 = new wxStaticText(this, ID_STATICTEXT7, _("H"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT7"));
	FlexGridSizer1->Add(HighC1, 1, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 0);
	SpinCtrlHighC1 = new wxSpinCtrl(this, ID_SPINCTRL4, _T("360"), wxDefaultPosition, wxDefaultSize, 0, 0, 360, 360, _T("ID_SPINCTRL4"));
	SpinCtrlHighC1->SetValue(_T("360"));
	FlexGridSizer1->Add(SpinCtrlHighC1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	HighC2 = new wxStaticText(this, ID_STATICTEXT6, _("S"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT6"));
	FlexGridSizer1->Add(HighC2, 1, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 0);
	SpinCtrlHighC2 = new wxSpinCtrl(this, ID_SPINCTRL5, _T("255"), wxDefaultPosition, wxDefaultSize, 0, 0, 255, 255, _T("ID_SPINCTRL5"));
	SpinCtrlHighC2->SetValue(_T("255"));
	FlexGridSizer1->Add(SpinCtrlHighC2, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	HighC3 = new wxStaticText(this, ID_STATICTEXT8, _("V"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT8"));
	FlexGridSizer1->Add(HighC3, 1, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 0);
	SpinCtrlHighC3 = new wxSpinCtrl(this, ID_SPINCTRL6, _T("255"), wxDefaultPosition, wxDefaultSize, 0, 0, 255, 255, _T("ID_SPINCTRL6"));
	SpinCtrlHighC3->SetValue(_T("255"));
	FlexGridSizer1->Add(SpinCtrlHighC3, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	StaticBoxSizer1->Add(FlexGridSizer1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer4->Add(StaticBoxSizer1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer3->Add(FlexGridSizer4, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer6 = new wxFlexGridSizer(0, 1, 0, 0);
	FlexGridSizer6->AddGrowableCol(0);
	wxString __wxRadioBoxChoices_2[2] =
	{
		_("Confirm previously detected pixels (AND operator)"),
		_("Add to previously detected pixels (OR operator)")
	};
	RadioBoxOperator = new wxRadioBox(this, ID_RADIOBOX2, _("Action type"), wxDefaultPosition, wxDefaultSize, 2, __wxRadioBoxChoices_2, 2, 0, wxDefaultValidator, _T("ID_RADIOBOX2"));
	RadioBoxOperator->SetSelection(0);
	FlexGridSizer6->Add(RadioBoxOperator, 1, wxALL|wxEXPAND, 5);
	FlexGridSizer3->Add(FlexGridSizer6, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer5 = new wxFlexGridSizer(0, 3, 0, 0);
	FlexGridSizer5->AddGrowableCol(1);
	CheckBoxRestrictToZone = new wxCheckBox(this, ID_CHECKBOX1, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX1"));
	CheckBoxRestrictToZone->SetValue(false);
	FlexGridSizer5->Add(CheckBoxRestrictToZone, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticText3 = new wxStaticText(this, ID_STATICTEXT9, _("Restrict to zone (grey level)"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT9"));
	FlexGridSizer5->Add(StaticText3, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	SpinCtrlZone = new wxSpinCtrl(this, ID_SPINCTRL_ZONE, _T("0"), wxDefaultPosition, wxDefaultSize, 0, 0, 255, 0, _T("ID_SPINCTRL_ZONE"));
	SpinCtrlZone->SetValue(_T("0"));
	FlexGridSizer5->Add(SpinCtrlZone, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer3->Add(FlexGridSizer5, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer2 = new wxFlexGridSizer(0, 3, 0, 0);
	ButtonOk = new wxButton(this, ID_BUTTON1, _("OK"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
	ButtonOk->SetDefault();
	ButtonOk->SetFocus();
	FlexGridSizer2->Add(ButtonOk, 1, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer3->Add(FlexGridSizer2, 1, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5);
	SetSizer(FlexGridSizer3);
	FlexGridSizer3->Fit(this);
	FlexGridSizer3->SetSizeHints(this);

	Connect(ID_RADIOBOX1,wxEVT_COMMAND_RADIOBOX_SELECTED,(wxObjectEventFunction)&DialogColorSegmentation::OnRadioBoxSelect);
	Connect(ID_COLOURPICKERCTRL1,wxEVT_COMMAND_COLOURPICKER_CHANGED,(wxObjectEventFunction)&DialogColorSegmentation::OnColourPickerCtrlMinColourChanged);
	Connect(ID_SPINCTRL1,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&DialogColorSegmentation::OnSpinCtrlChange);
	Connect(ID_SPINCTRL2,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&DialogColorSegmentation::OnSpinCtrlChange);
	Connect(ID_SPINCTRL3,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&DialogColorSegmentation::OnSpinCtrlChange);
	Connect(ID_COLOURPICKERCTRL2,wxEVT_COMMAND_COLOURPICKER_CHANGED,(wxObjectEventFunction)&DialogColorSegmentation::OnColourPickerCtrlMaxColourChanged);
	Connect(ID_SPINCTRL4,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&DialogColorSegmentation::OnSpinCtrlChange);
	Connect(ID_SPINCTRL5,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&DialogColorSegmentation::OnSpinCtrlChange);
	Connect(ID_SPINCTRL6,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&DialogColorSegmentation::OnSpinCtrlChange);
	Connect(ID_RADIOBOX2,wxEVT_COMMAND_RADIOBOX_SELECTED,(wxObjectEventFunction)&DialogColorSegmentation::OnRadioBoxOperatorSelect);
	Connect(ID_CHECKBOX1,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&DialogColorSegmentation::OnCheckBoxRestrictToZoneClick);
	Connect(ID_SPINCTRL_ZONE,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&DialogColorSegmentation::OnSpinCtrlZoneChange);
	Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&DialogColorSegmentation::OnButtonOkClick);
	//*)

	Fit();
	Centre();

	pixelSrc = Mat (1, 1, CV_8UC3);
	pixelDst = Mat (1, 1, CV_8UC3);
	pixelRes = Mat (1, 1, CV_8UC1);

}

DialogColorSegmentation::~DialogColorSegmentation()
{
	//(*Destroy(DialogColorSegmentation)
	//*)
}

void DialogColorSegmentation::SetPlugin (vector<PipelinePlugin*> pfv)
{
    for (auto func : pfv)
	plugin.push_back(dynamic_cast<ColorSegmentation*> (func));

    SpinCtrlZone->SetValue(plugin[0]->zone);
    RadioBoxOperator->SetSelection(plugin[0]->additive);
    CheckBoxRestrictToZone->SetValue(plugin[0]->restrictToZone);

    if (plugin[0]->type == ColorSegmentation::BGR)
    {
	RadioBox1->SetSelection(1);

	SpinCtrlLowC1->SetValue(plugin[0]->minBGR[2]);
	SpinCtrlLowC2->SetValue(plugin[0]->minBGR[1]);
	SpinCtrlLowC3->SetValue(plugin[0]->minBGR[0]);
	SpinCtrlHighC1->SetValue(plugin[0]->maxBGR[2]);
	SpinCtrlHighC2->SetValue(plugin[0]->maxBGR[1]);
	SpinCtrlHighC3->SetValue(plugin[0]->maxBGR[0]);

	LowC1->SetLabel("R");
	LowC2->SetLabel("G");
	LowC3->SetLabel("B");
	HighC1->SetLabel("R");
	HighC2->SetLabel("G");
	HighC3->SetLabel("B");

	SpinCtrlLowC1->SetRange (0, 255);
	SpinCtrlLowC2->SetRange (0, 255);
	SpinCtrlLowC2->SetRange (0, 255);
	SpinCtrlHighC1->SetRange (0, 255);
	SpinCtrlHighC2->SetRange (0, 255);
	SpinCtrlHighC2->SetRange (0, 255);

	Vec3b min = plugin[0]->minBGR;
	Vec3b max = plugin[0]->maxBGR;

	ColourPickerCtrlMin->SetColour (wxColour(min[2], min[1], min[0]));
	ColourPickerCtrlMax->SetColour (wxColour(max[2], max[1], max[0]));

    }
    else
    {
	RadioBox1->SetSelection(0);

	SpinCtrlLowC1->SetValue(plugin[0]->minHSV[0] * 2);
	SpinCtrlLowC2->SetValue(plugin[0]->minHSV[1]);
	SpinCtrlLowC3->SetValue(plugin[0]->minHSV[2]);
	SpinCtrlHighC1->SetValue(plugin[0]->maxHSV[0] * 2);
	SpinCtrlHighC2->SetValue(plugin[0]->maxHSV[1]);
	SpinCtrlHighC3->SetValue(plugin[0]->maxHSV[2]);

	LowC1->SetLabel("H");
	LowC2->SetLabel("S");
	LowC3->SetLabel("V");
	HighC1->SetLabel("H");
	HighC2->SetLabel("S");
	HighC3->SetLabel("V");

	SpinCtrlLowC1->SetRange (0, 360);
	SpinCtrlLowC2->SetRange (0, 255);
	SpinCtrlLowC2->SetRange (0, 255);
	SpinCtrlHighC1->SetRange (0, 360);
	SpinCtrlHighC2->SetRange (0, 255);
	SpinCtrlHighC2->SetRange (0, 255);

	pixelSrc.at<Vec3b>(0,0) = plugin[0]->minHSV;
	cvtColor(pixelSrc, pixelDst, CV_HSV2RGB);
	Vec3b& rgbMin = pixelDst.at<Vec3b>(0,0);
	ColourPickerCtrlMin->SetColour (wxColour(rgbMin[0], rgbMin[1], rgbMin[2]));

	pixelSrc.at<Vec3b>(0,0) = plugin[0]->maxHSV;
	cvtColor(pixelSrc, pixelDst, CV_HSV2RGB);
	Vec3b& rgbMax = pixelDst.at<Vec3b>(0,0);
	ColourPickerCtrlMax->SetColour (wxColour(rgbMax[0], rgbMax[1], rgbMax[2]));
    }
}

void DialogColorSegmentation::OnButtonCancelClick(wxCommandEvent& event)
{
    this->Hide();
}

void DialogColorSegmentation::OnButtonApplyClick(wxCommandEvent& event)
{
//    plugin->size = SpinCtrlColorSegmentationSize->GetValue();
}

void DialogColorSegmentation::OnButtonOkClick(wxCommandEvent& event)
{
    OnButtonApplyClick(event);
    this->Hide();
}

void DialogColorSegmentation::OnRadioBoxSelect(wxCommandEvent& event)
{
    int sel = RadioBox1->GetSelection();

    // HSV
    if (sel == 0)
    {
	LowC1->SetLabel("H");
	LowC2->SetLabel("S");
	LowC3->SetLabel("V");
	HighC1->SetLabel("H");
	HighC2->SetLabel("S");
	HighC3->SetLabel("V");

	SpinCtrlLowC1->SetRange (0, 360);
	SpinCtrlLowC2->SetRange (0, 255);
	SpinCtrlLowC2->SetRange (0, 255);
	SpinCtrlHighC1->SetRange (0, 360);
	SpinCtrlHighC2->SetRange (0, 255);
	SpinCtrlHighC2->SetRange (0, 255);

	for (auto f : plugin)
	    f->type = ColorSegmentation::HSV;
    }
    // BGR
    else if (sel == 1)
    {
	LowC1->SetLabel("R");
	LowC2->SetLabel("G");
	LowC3->SetLabel("B");
	HighC1->SetLabel("R");
	HighC2->SetLabel("G");
	HighC3->SetLabel("B");

	SpinCtrlLowC1->SetRange (0, 255);
	SpinCtrlLowC2->SetRange (0, 255);
	SpinCtrlLowC2->SetRange (0, 255);
	SpinCtrlHighC1->SetRange (0, 255);
	SpinCtrlHighC2->SetRange (0, 255);
	SpinCtrlHighC2->SetRange (0, 255);

	for (auto f : plugin)
	    f->type = ColorSegmentation::BGR;
    }
}

void DialogColorSegmentation::OnSpinCtrlChange(wxSpinEvent& event)
{
    int sel = RadioBox1->GetSelection();

    //HSV
    if (sel == 0)
    {
	for (auto f : plugin)
	{
	    f->minHSV[0] = SpinCtrlLowC1->GetValue() / 2;
	    f->minHSV[1] = SpinCtrlLowC2->GetValue();
	    f->minHSV[2] = SpinCtrlLowC3->GetValue();
	    f->maxHSV[0] = SpinCtrlHighC1->GetValue() / 2;
	    f->maxHSV[1] = SpinCtrlHighC2->GetValue();
	    f->maxHSV[2] = SpinCtrlHighC3->GetValue();
	}

	// convert to get RGB values
	pixelSrc.at<Vec3b>(0,0) = plugin[0]->minHSV;
	cvtColor(pixelSrc, pixelDst, CV_HSV2RGB);
	Vec3b& rgbMin = pixelDst.at<Vec3b>(0,0);
	ColourPickerCtrlMin->SetColour (wxColour(rgbMin[0], rgbMin[1], rgbMin[2]));

	pixelSrc.at<Vec3b>(0,0) = plugin[0]->maxHSV;
	cvtColor(pixelSrc, pixelDst, CV_HSV2RGB);
	Vec3b& rgbMax = pixelDst.at<Vec3b>(0,0);
	ColourPickerCtrlMax->SetColour (wxColour(rgbMax[0], rgbMax[1], rgbMax[2]));
    }
    // RGB
    else if (sel == 1)
    {
	for (auto f : plugin)
	{
	    f->minBGR[2] = SpinCtrlLowC1->GetValue();
	    f->minBGR[1] = SpinCtrlLowC2->GetValue();
	    f->minBGR[0] = SpinCtrlLowC3->GetValue();
	    f->maxBGR[2] = SpinCtrlHighC1->GetValue();
	    f->maxBGR[1] = SpinCtrlHighC2->GetValue();
	    f->maxBGR[0] = SpinCtrlHighC3->GetValue();
	}

	Vec3b min = plugin[0]->minBGR;
	Vec3b max = plugin[0]->maxBGR;

	ColourPickerCtrlMin->SetColour (wxColour(min[2], min[1], min[0]));
	ColourPickerCtrlMax->SetColour (wxColour(max[2], max[1], max[0]));
    }

    // update color pickers
}

void DialogColorSegmentation::OnColourPickerCtrlMinColourChanged(wxColourPickerEvent& event)
{
    wxColour col = ColourPickerCtrlMin->GetColour ();

    int sel = RadioBox1->GetSelection();

    if (sel == 0)
    {
	pixelSrc.at<Vec3b>(0,0) = Vec3b (col.Blue(), col.Green(), col.Red());
	cvtColor(pixelSrc, pixelDst, CV_BGR2HSV);
	for (auto f : plugin)
	    f->minHSV = pixelDst.at<Vec3b>(0,0);

	SpinCtrlLowC1->SetValue(plugin[0]->minHSV[0] * 2);
	SpinCtrlLowC2->SetValue(plugin[0]->minHSV[1]);
	SpinCtrlLowC3->SetValue(plugin[0]->minHSV[2]);
    }
    else if (sel == 1)
    {
	Vec3b bgr (col.Blue(), col.Green(), col.Red());
	for (auto f : plugin)
	    f->minBGR = bgr;

	SpinCtrlLowC1->SetValue(col.Red());
	SpinCtrlLowC2->SetValue(col.Green());
	SpinCtrlLowC3->SetValue(col.Blue());
    }
}

void DialogColorSegmentation::OnColourPickerCtrlMaxColourChanged(wxColourPickerEvent& event)
{
    wxColour col = ColourPickerCtrlMax->GetColour ();

    int sel = RadioBox1->GetSelection();

    if (sel == 0)
    {
	pixelSrc.at<Vec3b>(0,0) = Vec3b (col.Blue(), col.Green(), col.Red());
	cvtColor(pixelSrc, pixelDst, CV_BGR2HSV);
	for (auto f : plugin)
	    f->maxHSV = pixelDst.at<Vec3b>(0,0);

	SpinCtrlHighC1->SetValue(plugin[0]->maxHSV[0] * 2);
	SpinCtrlHighC2->SetValue(plugin[0]->maxHSV[1]);
	SpinCtrlHighC3->SetValue(plugin[0]->maxHSV[2]);
    }
    else if (sel == 1)
    {
	Vec3b bgr (col.Blue(), col.Green(), col.Red());
	for (auto f : plugin)
	    f->maxBGR = bgr;

	SpinCtrlHighC1->SetValue(col.Red());
	SpinCtrlHighC2->SetValue(col.Green());
	SpinCtrlHighC3->SetValue(col.Blue());
    }
}


void DialogColorSegmentation::OnCheckBoxRestrictToZoneClick(wxCommandEvent& event)
{
    for (auto f : plugin)
    	f->restrictToZone = event.IsChecked();
}

void DialogColorSegmentation::OnSpinCtrlZoneChange(wxSpinEvent& event)
{
    for (auto f : plugin)
    	f->zone = SpinCtrlZone->GetValue();
}

void DialogColorSegmentation::OnRadioBoxOperatorSelect(wxCommandEvent& event)
{
    for (auto f : plugin)
    	f->additive = RadioBoxOperator->GetSelection();
}
