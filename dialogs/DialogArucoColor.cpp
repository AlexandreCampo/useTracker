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

#include "DialogArucoColor.h"

//(*InternalHeaders(DialogArucoColor)
#include <wx/string.h>
#include <wx/intl.h>
//*)

//(*IdInit(DialogArucoColor)
const long DialogArucoColor::ID_STATICTEXT1 = wxNewId();
const long DialogArucoColor::ID_SPINCTRL1 = wxNewId();
const long DialogArucoColor::ID_STATICTEXT2 = wxNewId();
const long DialogArucoColor::ID_SPINCTRL2 = wxNewId();
const long DialogArucoColor::ID_STATICTEXT3 = wxNewId();
const long DialogArucoColor::ID_TEXTCTRL1 = wxNewId();
const long DialogArucoColor::ID_STATICTEXT4 = wxNewId();
const long DialogArucoColor::ID_TEXTCTRL2 = wxNewId();
const long DialogArucoColor::ID_STATICTEXT5 = wxNewId();
const long DialogArucoColor::ID_SPINCTRL4 = wxNewId();
const long DialogArucoColor::ID_STATICTEXT6 = wxNewId();
const long DialogArucoColor::ID_SPINCTRL3 = wxNewId();
const long DialogArucoColor::ID_STATICTEXT7 = wxNewId();
const long DialogArucoColor::ID_SPINCTRL5 = wxNewId();
const long DialogArucoColor::ID_STATICTEXT8 = wxNewId();
const long DialogArucoColor::ID_SPINCTRL6 = wxNewId();
const long DialogArucoColor::ID_STATICTEXT9 = wxNewId();
const long DialogArucoColor::ID_SPINCTRL7 = wxNewId();
const long DialogArucoColor::ID_STATICTEXT10 = wxNewId();
const long DialogArucoColor::ID_SPINCTRL8 = wxNewId();
const long DialogArucoColor::ID_STATICTEXT11 = wxNewId();
const long DialogArucoColor::ID_SPINCTRL9 = wxNewId();
const long DialogArucoColor::ID_STATICTEXT12 = wxNewId();
const long DialogArucoColor::ID_SPINCTRL10 = wxNewId();
const long DialogArucoColor::ID_BUTTON1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(DialogArucoColor,wxDialog)
	//(*EventTable(DialogArucoColor)
	//*)
END_EVENT_TABLE()

DialogArucoColor::DialogArucoColor(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
	//(*Initialize(DialogArucoColor)
	wxStaticBoxSizer* StaticBoxSizer2;
	wxFlexGridSizer* FlexGridSizer1;
	wxFlexGridSizer* FlexGridSizer2;
	wxFlexGridSizer* FlexGridSizer4;
	wxStaticBoxSizer* StaticBoxSizer3;
	wxFlexGridSizer* FlexGridSizer6;
	wxFlexGridSizer* FlexGridSizer3;
	wxStaticBoxSizer* StaticBoxSizer4;
	wxFlexGridSizer* FlexGridSizer5;
	wxStaticBoxSizer* StaticBoxSizer1;

	Create(parent, wxID_ANY, _("ArucoColor"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, _T("wxID_ANY"));
	SetMinSize(wxSize(-1,-1));
	SetMaxSize(wxSize(-1,-1));
	SetFocus();
	FlexGridSizer3 = new wxFlexGridSizer(0, 1, 0, 0);
	StaticBoxSizer1 = new wxStaticBoxSizer(wxVERTICAL, this, _("ArucoColor"));
	FlexGridSizer6 = new wxFlexGridSizer(0, 1, 0, 0);
	FlexGridSizer6->AddGrowableCol(0);
	StaticBoxSizer2 = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Markers"));
	FlexGridSizer1 = new wxFlexGridSizer(0, 2, 0, 0);
	FlexGridSizer1->AddGrowableCol(0);
	StaticText1 = new wxStaticText(this, ID_STATICTEXT1, _("Markers number of columns (pixels)"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
	FlexGridSizer1->Add(StaticText1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	SpinCtrlMarkerCols = new wxSpinCtrl(this, ID_SPINCTRL1, _T("2"), wxDefaultPosition, wxDefaultSize, 0, 1, 1000, 2, _T("ID_SPINCTRL1"));
	SpinCtrlMarkerCols->SetValue(_T("2"));
	FlexGridSizer1->Add(SpinCtrlMarkerCols, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticText2 = new wxStaticText(this, ID_STATICTEXT2, _("Markers number of rows (pixels)"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT2"));
	FlexGridSizer1->Add(StaticText2, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	SpinCtrlMarkerRows = new wxSpinCtrl(this, ID_SPINCTRL2, _T("2"), wxDefaultPosition, wxDefaultSize, 0, 1, 1000, 2, _T("ID_SPINCTRL2"));
	SpinCtrlMarkerRows->SetValue(_T("2"));
	FlexGridSizer1->Add(SpinCtrlMarkerRows, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticText3 = new wxStaticText(this, ID_STATICTEXT3, _("Reference hues (list of hues from 0 to 180)"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT3"));
	FlexGridSizer1->Add(StaticText3, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	TextCtrlRefHues = new wxTextCtrl(this, ID_TEXTCTRL1, _("Text"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL1"));
	FlexGridSizer1->Add(TextCtrlRefHues, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticText4 = new wxStaticText(this, ID_STATICTEXT4, _("Dictionary (list of codes)"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT4"));
	FlexGridSizer1->Add(StaticText4, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	TextCtrlDictionary = new wxTextCtrl(this, ID_TEXTCTRL2, _("Text"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL2"));
	FlexGridSizer1->Add(TextCtrlDictionary, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticBoxSizer2->Add(FlexGridSizer1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer6->Add(StaticBoxSizer2, 1, wxALL|wxEXPAND, 5);
	StaticBoxSizer3 = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Detection"));
	FlexGridSizer4 = new wxFlexGridSizer(0, 2, 0, 0);
	FlexGridSizer4->AddGrowableCol(0);
	StaticText5 = new wxStaticText(this, ID_STATICTEXT5, _("Adaptive threshold block size"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT5"));
	FlexGridSizer4->Add(StaticText5, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	SpinCtrlAtBlockSize = new wxSpinCtrl(this, ID_SPINCTRL4, _T("13"), wxDefaultPosition, wxDefaultSize, 0, 1, 1000, 13, _T("ID_SPINCTRL4"));
	SpinCtrlAtBlockSize->SetValue(_T("13"));
	FlexGridSizer4->Add(SpinCtrlAtBlockSize, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticText6 = new wxStaticText(this, ID_STATICTEXT6, _("Adaptive threshold constant"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT6"));
	FlexGridSizer4->Add(StaticText6, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	SpinCtrlAtConstant = new wxSpinCtrl(this, ID_SPINCTRL3, _T("-5"), wxDefaultPosition, wxDefaultSize, 0, -255, 255, -5, _T("ID_SPINCTRL3"));
	SpinCtrlAtConstant->SetValue(_T("-5"));
	FlexGridSizer4->Add(SpinCtrlAtConstant, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticText7 = new wxStaticText(this, ID_STATICTEXT7, _("Saturation threshold (from HSV, retain above)"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT7"));
	FlexGridSizer4->Add(StaticText7, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	SpinCtrlSaturationThreshold = new wxSpinCtrl(this, ID_SPINCTRL5, _T("60"), wxDefaultPosition, wxDefaultSize, 0, 0, 255, 60, _T("ID_SPINCTRL5"));
	SpinCtrlSaturationThreshold->SetValue(_T("60"));
	FlexGridSizer4->Add(SpinCtrlSaturationThreshold, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticText8 = new wxStaticText(this, ID_STATICTEXT8, _("Value threshold (from HSV, retain above)"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT8"));
	FlexGridSizer4->Add(StaticText8, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	SpinCtrlValueThreshold = new wxSpinCtrl(this, ID_SPINCTRL6, _T("100"), wxDefaultPosition, wxDefaultSize, 0, 0, 255, 100, _T("ID_SPINCTRL6"));
	SpinCtrlValueThreshold->SetValue(_T("100"));
	FlexGridSizer4->Add(SpinCtrlValueThreshold, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticText9 = new wxStaticText(this, ID_STATICTEXT9, _("Minimum marker area"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT9"));
	FlexGridSizer4->Add(StaticText9, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	SpinCtrlMinArea = new wxSpinCtrl(this, ID_SPINCTRL7, _T("150"), wxDefaultPosition, wxDefaultSize, 0, 0, 1000000000, 150, _T("ID_SPINCTRL7"));
	SpinCtrlMinArea->SetValue(_T("150"));
	FlexGridSizer4->Add(SpinCtrlMinArea, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticText10 = new wxStaticText(this, ID_STATICTEXT10, _("Maximum marker area"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT10"));
	FlexGridSizer4->Add(StaticText10, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	SpinCtrlMaxArea = new wxSpinCtrl(this, ID_SPINCTRL8, _T("1600"), wxDefaultPosition, wxDefaultSize, 0, 0, 1000000000, 1600, _T("ID_SPINCTRL8"));
	SpinCtrlMaxArea->SetValue(_T("1600"));
	FlexGridSizer4->Add(SpinCtrlMaxArea, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticBoxSizer3->Add(FlexGridSizer4, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer6->Add(StaticBoxSizer3, 1, wxALL|wxEXPAND, 5);
	StaticBoxSizer4 = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Recognition"));
	FlexGridSizer5 = new wxFlexGridSizer(0, 2, 0, 0);
	FlexGridSizer5->AddGrowableCol(0);
	StaticText11 = new wxStaticText(this, ID_STATICTEXT11, _("Maximum hue deviation"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT11"));
	FlexGridSizer5->Add(StaticText11, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	SpinCtrlMaxHueDeviation = new wxSpinCtrl(this, ID_SPINCTRL9, _T("25"), wxDefaultPosition, wxDefaultSize, 0, 0, 180, 25, _T("ID_SPINCTRL9"));
	SpinCtrlMaxHueDeviation->SetValue(_T("25"));
	FlexGridSizer5->Add(SpinCtrlMaxHueDeviation, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticText12 = new wxStaticText(this, ID_STATICTEXT12, _("Maximum marker range"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT12"));
	FlexGridSizer5->Add(StaticText12, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	SpinCtrlMaxMarkerRange = new wxSpinCtrl(this, ID_SPINCTRL10, _T("4"), wxDefaultPosition, wxDefaultSize, 0, 0, 10000, 4, _T("ID_SPINCTRL10"));
	SpinCtrlMaxMarkerRange->SetValue(_T("4"));
	FlexGridSizer5->Add(SpinCtrlMaxMarkerRange, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticBoxSizer4->Add(FlexGridSizer5, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer6->Add(StaticBoxSizer4, 1, wxALL|wxEXPAND, 5);
	StaticBoxSizer1->Add(FlexGridSizer6, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer3->Add(StaticBoxSizer1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer2 = new wxFlexGridSizer(0, 3, 0, 0);
	ButtonOk = new wxButton(this, ID_BUTTON1, _("OK"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
	ButtonOk->SetDefault();
	ButtonOk->SetFocus();
	FlexGridSizer2->Add(ButtonOk, 1, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer3->Add(FlexGridSizer2, 1, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5);
	SetSizer(FlexGridSizer3);
	FlexGridSizer3->Fit(this);
	FlexGridSizer3->SetSizeHints(this);

	Connect(ID_SPINCTRL1,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&DialogArucoColor::OnSpinCtrlArucoColorSizeChange);
	Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&DialogArucoColor::OnButtonOkClick);
	//*)

	Fit();
	Centre();
}

DialogArucoColor::~DialogArucoColor()
{
	//(*Destroy(DialogArucoColor)
	//*)
}

void DialogArucoColor::SetPlugin (std::vector<PipelinePlugin*> pfv)
{
    for (auto func : pfv)
	plugin.push_back(dynamic_cast<ArucoColor*> (func));
    SpinCtrlArucoColorSize->SetValue(plugin[0]->size);
}

void DialogArucoColor::OnButtonCancelClick(wxCommandEvent& event)
{
    this->Hide();
}


void DialogArucoColor::OnButtonOkClick(wxCommandEvent& event)
{
    OnButtonApplyClick(event);
    this->Hide();
}

void DialogArucoColor::OnSpinCtrlArucoColorSizeChange(wxSpinEvent& event)
{
    OnButtonApplyClick (event);
}

void DialogArucoColor::OnButtonApplyClick(wxCommandEvent& event)
{
    for (auto f : plugin)
    	f->SetSize(SpinCtrlArucoColorSize->GetValue());
}

