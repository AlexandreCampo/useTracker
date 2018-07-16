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

#include "DialogAdaptiveThreshold.h"

#include <opencv2/imgproc/imgproc.hpp>

//(*InternalHeaders(DialogAdaptiveThreshold)
#include <wx/string.h>
#include <wx/intl.h>
//*)

//(*IdInit(DialogAdaptiveThreshold)
const long DialogAdaptiveThreshold::ID_RADIOBOX1 = wxNewId();
const long DialogAdaptiveThreshold::ID_STATICTEXT1 = wxNewId();
const long DialogAdaptiveThreshold::ID_SPINCTRL1 = wxNewId();
const long DialogAdaptiveThreshold::ID_STATICTEXT2 = wxNewId();
const long DialogAdaptiveThreshold::ID_SPINCTRL2 = wxNewId();
const long DialogAdaptiveThreshold::ID_CHECKBOX2 = wxNewId();
const long DialogAdaptiveThreshold::ID_RADIOBOX2 = wxNewId();
const long DialogAdaptiveThreshold::ID_CHECKBOX1 = wxNewId();
const long DialogAdaptiveThreshold::ID_STATICTEXT3 = wxNewId();
const long DialogAdaptiveThreshold::ID_SPINCTRL_ZONE = wxNewId();
const long DialogAdaptiveThreshold::ID_BUTTON1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(DialogAdaptiveThreshold,wxDialog)
	//(*EventTable(DialogAdaptiveThreshold)
	//*)
END_EVENT_TABLE()

DialogAdaptiveThreshold::DialogAdaptiveThreshold(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
	//(*Initialize(DialogAdaptiveThreshold)
	wxFlexGridSizer* FlexGridSizer1;
	wxFlexGridSizer* FlexGridSizer2;
	wxFlexGridSizer* FlexGridSizer4;
	wxFlexGridSizer* FlexGridSizer6;
	wxFlexGridSizer* FlexGridSizer3;
	wxFlexGridSizer* FlexGridSizer5;
	wxStaticBoxSizer* StaticBoxSizer1;

	Create(parent, wxID_ANY, _("Adaptive background difference"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, _T("wxID_ANY"));
	SetMinSize(wxSize(-1,-1));
	SetMaxSize(wxSize(-1,-1));
	SetFocus();
	FlexGridSizer3 = new wxFlexGridSizer(0, 1, 0, 0);
	wxString __wxRadioBoxChoices_1[2] =
	{
		_("mean"),
		_("gaussian")
	};
	RadioBoxMethod = new wxRadioBox(this, ID_RADIOBOX1, _("Thresholding method"), wxDefaultPosition, wxDefaultSize, 2, __wxRadioBoxChoices_1, 2, 0, wxDefaultValidator, _T("ID_RADIOBOX1"));
	RadioBoxMethod->SetSelection(0);
	FlexGridSizer3->Add(RadioBoxMethod, 1, wxALL|wxEXPAND, 5);
	StaticBoxSizer1 = new wxStaticBoxSizer(wxVERTICAL, this, _("Adaptive background difference"));
	FlexGridSizer5 = new wxFlexGridSizer(2, 1, 0, 0);
	FlexGridSizer1 = new wxFlexGridSizer(0, 2, 0, 0);
	StaticText1 = new wxStaticText(this, ID_STATICTEXT1, _("Block size"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
	FlexGridSizer1->Add(StaticText1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	SpinCtrlBlockSize = new wxSpinCtrl(this, ID_SPINCTRL1, _T("1"), wxDefaultPosition, wxDefaultSize, 0, 1, 1000, 1, _T("ID_SPINCTRL1"));
	SpinCtrlBlockSize->SetValue(_T("1"));
	FlexGridSizer1->Add(SpinCtrlBlockSize, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticText2 = new wxStaticText(this, ID_STATICTEXT2, _("Constant"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT2"));
	FlexGridSizer1->Add(StaticText2, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	SpinCtrlConstant = new wxSpinCtrl(this, ID_SPINCTRL2, _T("0"), wxDefaultPosition, wxDefaultSize, 0, -255, 255, 0, _T("ID_SPINCTRL2"));
	SpinCtrlConstant->SetValue(_T("0"));
	FlexGridSizer1->Add(SpinCtrlConstant, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer5->Add(FlexGridSizer1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	FlexGridSizer6 = new wxFlexGridSizer(0, 3, 0, 0);
	CheckBoxInvert = new wxCheckBox(this, ID_CHECKBOX2, _("Invert"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX2"));
	CheckBoxInvert->SetValue(false);
	FlexGridSizer6->Add(CheckBoxInvert, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer5->Add(FlexGridSizer6, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	StaticBoxSizer1->Add(FlexGridSizer5, 1, wxALL|wxEXPAND, 5);
	FlexGridSizer3->Add(StaticBoxSizer1, 1, wxALL|wxEXPAND, 5);
	wxString __wxRadioBoxChoices_2[2] =
	{
		_("Confirm previously detected pixels (AND operator)"),
		_("Add to previously detected pixels (OR operator)")
	};
	RadioBoxOperator = new wxRadioBox(this, ID_RADIOBOX2, _("Action type"), wxDefaultPosition, wxDefaultSize, 2, __wxRadioBoxChoices_2, 2, 0, wxDefaultValidator, _T("ID_RADIOBOX2"));
	RadioBoxOperator->SetSelection(0);
	FlexGridSizer3->Add(RadioBoxOperator, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer4 = new wxFlexGridSizer(0, 3, 0, 0);
	FlexGridSizer4->AddGrowableCol(0);
	CheckBoxRestrictToZone = new wxCheckBox(this, ID_CHECKBOX1, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX1"));
	CheckBoxRestrictToZone->SetValue(false);
	FlexGridSizer4->Add(CheckBoxRestrictToZone, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticText3 = new wxStaticText(this, ID_STATICTEXT3, _("Restrict to zone (grey level)"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT3"));
	FlexGridSizer4->Add(StaticText3, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	SpinCtrlZone = new wxSpinCtrl(this, ID_SPINCTRL_ZONE, _T("0"), wxDefaultPosition, wxDefaultSize, 0, 0, 255, 0, _T("ID_SPINCTRL_ZONE"));
	SpinCtrlZone->SetValue(_T("0"));
	FlexGridSizer4->Add(SpinCtrlZone, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer3->Add(FlexGridSizer4, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer2 = new wxFlexGridSizer(0, 3, 0, 0);
	ButtonOk = new wxButton(this, ID_BUTTON1, _("OK"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
	ButtonOk->SetDefault();
	ButtonOk->SetFocus();
	FlexGridSizer2->Add(ButtonOk, 1, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer3->Add(FlexGridSizer2, 1, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5);
	SetSizer(FlexGridSizer3);
	FlexGridSizer3->Fit(this);
	FlexGridSizer3->SetSizeHints(this);

	Connect(ID_RADIOBOX1,wxEVT_COMMAND_RADIOBOX_SELECTED,(wxObjectEventFunction)&DialogAdaptiveThreshold::OnRadioBoxMethodSelect);
	Connect(ID_SPINCTRL1,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&DialogAdaptiveThreshold::OnSpinCtrlBlockSizeChange);
	Connect(ID_SPINCTRL2,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&DialogAdaptiveThreshold::OnSpinCtrlConstantChange);
	Connect(ID_CHECKBOX2,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&DialogAdaptiveThreshold::OnCheckBoxInvertClick);
	Connect(ID_RADIOBOX2,wxEVT_COMMAND_RADIOBOX_SELECTED,(wxObjectEventFunction)&DialogAdaptiveThreshold::OnRadioBoxOperatorSelect);
	Connect(ID_CHECKBOX1,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&DialogAdaptiveThreshold::OnCheckBoxRestrictToZoneClick);
	Connect(ID_SPINCTRL_ZONE,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&DialogAdaptiveThreshold::OnSpinCtrlZoneChange);
	Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&DialogAdaptiveThreshold::OnButtonOkClick);
	//*)

	Fit();
	Centre();
}

DialogAdaptiveThreshold::~DialogAdaptiveThreshold()
{
	//(*Destroy(DialogAdaptiveThreshold)
	//*)
}

void DialogAdaptiveThreshold::SetPlugin (std::vector<PipelinePlugin*> pfv)
{
    for (auto func : pfv)
	plugin.push_back(dynamic_cast<AdaptiveThreshold*> (func));

    if (plugin[0]->thresholdMethod == cv::ADAPTIVE_THRESH_MEAN_C)
	RadioBoxMethod->SetSelection(0);
    else if (plugin[0]->thresholdMethod == cv::ADAPTIVE_THRESH_GAUSSIAN_C)
	RadioBoxMethod->SetSelection(0);

    SpinCtrlBlockSize->SetValue(plugin[0]->blockSize);
    SpinCtrlConstant->SetValue(plugin[0]->constant);

    SpinCtrlZone->SetValue(plugin[0]->zone);
    RadioBoxOperator->SetSelection(plugin[0]->additive);
    CheckBoxRestrictToZone->SetValue(plugin[0]->restrictToZone);

    CheckBoxInvert->SetValue(plugin[0]->invert);
}

void DialogAdaptiveThreshold::OnButtonOkClick(wxCommandEvent& event)
{
    this->Hide();
}

void DialogAdaptiveThreshold::OnRadioBoxMethodSelect(wxCommandEvent& event)
{
    for (auto f : plugin)
	f->SetThresholdMethod(RadioBoxMethod->GetSelection());
}

void DialogAdaptiveThreshold::OnSpinCtrlBlockSizeChange(wxSpinEvent& event)
{
    for (auto f : plugin)
	f->SetBlockSize(event.GetValue());
}

void DialogAdaptiveThreshold::OnSpinCtrlConstantChange(wxSpinEvent& event)
{
    for (auto f : plugin)
	f->SetConstant(event.GetValue());
}

void DialogAdaptiveThreshold::OnSpinCtrlZoneChange(wxSpinEvent& event)
{
    for (auto f : plugin)
    	f->zone = SpinCtrlZone->GetValue();
}

void DialogAdaptiveThreshold::OnCheckBoxRestrictToZoneClick(wxCommandEvent& event)
{
    for (auto f : plugin)
    	f->restrictToZone = event.IsChecked();
}

void DialogAdaptiveThreshold::OnRadioBoxOperatorSelect(wxCommandEvent& event)
{
    for (auto f : plugin)
    	f->additive = RadioBoxOperator->GetSelection();
}

void DialogAdaptiveThreshold::OnCheckBox1Click(wxCommandEvent& event)
{
}

void DialogAdaptiveThreshold::OnCheckBoxInvertClick(wxCommandEvent& event)
{
    for (auto f : plugin)
    	f->invert = event.IsChecked();
}
