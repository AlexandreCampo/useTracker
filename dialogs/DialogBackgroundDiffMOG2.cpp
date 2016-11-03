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

#include "DialogBackgroundDiffMOG2.h"

#include <iostream>

//(*InternalHeaders(DialogBackgroundDiffMOG2)
#include <wx/string.h>
#include <wx/intl.h>
//*)

//(*IdInit(DialogBackgroundDiffMOG2)
const long DialogBackgroundDiffMOG2::ID_STATICTEXT1 = wxNewId();
const long DialogBackgroundDiffMOG2::ID_SPINCTRL_HISTORY = wxNewId();
const long DialogBackgroundDiffMOG2::ID_STATICTEXT2 = wxNewId();
const long DialogBackgroundDiffMOG2::ID_SPINCTRL_NMIXTURES = wxNewId();
const long DialogBackgroundDiffMOG2::ID_STATICTEXT3 = wxNewId();
const long DialogBackgroundDiffMOG2::ID_SPINCTRL_LEARNINGRATE = wxNewId();
const long DialogBackgroundDiffMOG2::ID_CHECKBOX3 = wxNewId();
const long DialogBackgroundDiffMOG2::ID_RADIOBOX2 = wxNewId();
const long DialogBackgroundDiffMOG2::ID_CHECKBOX1 = wxNewId();
const long DialogBackgroundDiffMOG2::ID_STATICTEXT6 = wxNewId();
const long DialogBackgroundDiffMOG2::ID_SPINCTRL_ZONE = wxNewId();
const long DialogBackgroundDiffMOG2::ID_BUTTON1 = wxNewId();
//*)

const long DialogBackgroundDiffMOG2::ID_SPINCTRLD_THRESHOLD = wxNewId();
const long DialogBackgroundDiffMOG2::ID_SPINCTRLD_LEARNINGRATE = wxNewId();


BEGIN_EVENT_TABLE(DialogBackgroundDiffMOG2,wxDialog)
	//(*EventTable(DialogBackgroundDiffMOG2)
	//*)
END_EVENT_TABLE()

DialogBackgroundDiffMOG2::DialogBackgroundDiffMOG2(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
	//(*Initialize(DialogBackgroundDiffMOG2)
	wxFlexGridSizer* FlexGridSizer1;
	wxFlexGridSizer* FlexGridSizer2;
	wxFlexGridSizer* FlexGridSizer4;
	wxFlexGridSizer* FlexGridSizer3;
	wxFlexGridSizer* FlexGridSizer5;
	wxStaticBoxSizer* StaticBoxSizer1;

	Create(parent, wxID_ANY, _("Background Difference MOG 2"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, _T("wxID_ANY"));
	SetMinSize(wxSize(-1,-1));
	SetMaxSize(wxSize(-1,-1));
	SetFocus();
	FlexGridSizer3 = new wxFlexGridSizer(0, 1, 0, 0);
	StaticBoxSizer1 = new wxStaticBoxSizer(wxVERTICAL, this, _("Background difference MOG 2"));
	FlexGridSizer5 = new wxFlexGridSizer(0, 1, 0, 0);
	FlexGridSizer1 = new wxFlexGridSizer(0, 2, 0, 0);
	StaticText1 = new wxStaticText(this, ID_STATICTEXT1, _("# of frames for history"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
	FlexGridSizer1->Add(StaticText1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	SpinCtrlHistory = new wxSpinCtrl(this, ID_SPINCTRL_HISTORY, _T("200"), wxDefaultPosition, wxDefaultSize, 0, 1, 5000, 200, _T("ID_SPINCTRL_HISTORY"));
	SpinCtrlHistory->SetValue(_T("200"));
	FlexGridSizer1->Add(SpinCtrlHistory, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticText2 = new wxStaticText(this, ID_STATICTEXT2, _("threshold"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT2"));
	FlexGridSizer1->Add(StaticText2, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	SpinCtrlThreshold = new wxSpinCtrl(this, ID_SPINCTRL_NMIXTURES, _T("5"), wxDefaultPosition, wxDefaultSize, 0, 1, 1000, 5, _T("ID_SPINCTRL_NMIXTURES"));
	SpinCtrlThreshold->SetValue(_T("5"));
	FlexGridSizer1->Add(SpinCtrlThreshold, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticText3 = new wxStaticText(this, ID_STATICTEXT3, _("learning rate"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT3"));
	FlexGridSizer1->Add(StaticText3, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	SpinCtrlLearningRate = new wxSpinCtrl(this, ID_SPINCTRL_LEARNINGRATE, _T("0"), wxDefaultPosition, wxDefaultSize, 0, 0, 100, 0, _T("ID_SPINCTRL_LEARNINGRATE"));
	SpinCtrlLearningRate->SetValue(_T("0"));
	FlexGridSizer1->Add(SpinCtrlLearningRate, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer5->Add(FlexGridSizer1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	CheckBoxShadowDetect = new wxCheckBox(this, ID_CHECKBOX3, _("detect shadows"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX3"));
	CheckBoxShadowDetect->SetValue(false);
	FlexGridSizer5->Add(CheckBoxShadowDetect, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticBoxSizer1->Add(FlexGridSizer5, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer3->Add(StaticBoxSizer1, 1, wxALL|wxEXPAND, 5);
	wxString __wxRadioBoxChoices_1[2] =
	{
		_("Confirm previously detected pixels (AND operator)"),
		_("Add to previously detected pixels (OR operator)")
	};
	RadioBoxOperator = new wxRadioBox(this, ID_RADIOBOX2, _("Action type"), wxDefaultPosition, wxDefaultSize, 2, __wxRadioBoxChoices_1, 2, 0, wxDefaultValidator, _T("ID_RADIOBOX2"));
	RadioBoxOperator->SetSelection(0);
	FlexGridSizer3->Add(RadioBoxOperator, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer4 = new wxFlexGridSizer(0, 3, 0, 0);
	FlexGridSizer4->AddGrowableCol(0);
	CheckBoxRestrictToZone = new wxCheckBox(this, ID_CHECKBOX1, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX1"));
	CheckBoxRestrictToZone->SetValue(false);
	FlexGridSizer4->Add(CheckBoxRestrictToZone, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticText6 = new wxStaticText(this, ID_STATICTEXT6, _("Restrict to zone (grey level)"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT6"));
	FlexGridSizer4->Add(StaticText6, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
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

	Connect(ID_SPINCTRL_HISTORY,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&DialogBackgroundDiffMOG2::OnSpinCtrlHistoryChange);
	Connect(ID_SPINCTRL_NMIXTURES,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&DialogBackgroundDiffMOG2::OnSpinCtrlThresholdChange);
	Connect(ID_SPINCTRL_LEARNINGRATE,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&DialogBackgroundDiffMOG2::OnSpinCtrlLearningRateChange);
	Connect(ID_CHECKBOX3,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&DialogBackgroundDiffMOG2::OnCheckBoxShadowDetectClick);
	Connect(ID_RADIOBOX2,wxEVT_COMMAND_RADIOBOX_SELECTED,(wxObjectEventFunction)&DialogBackgroundDiffMOG2::OnRadioBoxOperatorSelect);
	Connect(ID_CHECKBOX1,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&DialogBackgroundDiffMOG2::OnCheckBoxRestrictToZoneClick);
	Connect(ID_SPINCTRL_ZONE,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&DialogBackgroundDiffMOG2::OnSpinCtrlZoneChange);
	Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&DialogBackgroundDiffMOG2::OnButtonOkClick);
	//*)

	// codeblocks 16 / wxsmith does not support spinctrldouble yet... adding manually
	SpinCtrlDoubleThreshold = new wxSpinCtrlDouble(this, ID_SPINCTRLD_THRESHOLD, _T("0"), wxDefaultPosition, wxDefaultSize, 0, 0, 255, 0, 0.1, _T("ID_SPINCTRLD_THRESHOLD"));
	FlexGridSizer1->Replace(SpinCtrlThreshold, SpinCtrlDoubleThreshold);
	SpinCtrlDoubleThreshold->Bind(wxEVT_SPINCTRLDOUBLE, &DialogBackgroundDiffMOG2::OnSpinCtrlThresholdChange, this);

	SpinCtrlDoubleLearningRate = new wxSpinCtrlDouble(this, ID_SPINCTRLD_LEARNINGRATE, _T("0"), wxDefaultPosition, wxDefaultSize, 0, 0, 1, 0.01, 0.001, _T("ID_SPINCTRLD_LEARNINGRATE"));
	FlexGridSizer1->Replace(SpinCtrlLearningRate, SpinCtrlDoubleLearningRate);
	SpinCtrlDoubleLearningRate->Bind(wxEVT_SPINCTRLDOUBLE, &DialogBackgroundDiffMOG2::OnSpinCtrlLearningRateChange, this);

	Fit();
	Centre();
}

DialogBackgroundDiffMOG2::~DialogBackgroundDiffMOG2()
{
	//(*Destroy(DialogBackgroundDiffMOG2)
	//*)
}

void DialogBackgroundDiffMOG2::SetPlugin (std::vector<PipelinePlugin*> pfv)
{
    for (auto func : pfv)
	plugin.push_back(dynamic_cast<BackgroundDiffMOG2*> (func));

    SpinCtrlHistory->SetValue(plugin[0]->history);
    SpinCtrlDoubleThreshold->SetValue(plugin[0]->threshold);
    SpinCtrlDoubleLearningRate->SetValue(plugin[0]->learningRate);
    CheckBoxShadowDetect->SetValue(plugin[0]->shadowDetection);
    
    SpinCtrlZone->SetValue(plugin[0]->zone);
    RadioBoxOperator->SetSelection(plugin[0]->additive);
    CheckBoxRestrictToZone->SetValue(plugin[0]->restrictToZone);
}

void DialogBackgroundDiffMOG2::OnButtonCancelClick(wxCommandEvent& event)
{
    this->Hide();
}

void DialogBackgroundDiffMOG2::OnButtonOkClick(wxCommandEvent& event)
{
    this->Hide();
}

void DialogBackgroundDiffMOG2::OnButtonApplyClick(wxCommandEvent& event)
{
}

void DialogBackgroundDiffMOG2::OnCheckBoxRestrictToZoneClick(wxCommandEvent& event)
{
    for (auto f : plugin)
    	f->restrictToZone = event.IsChecked();
}

void DialogBackgroundDiffMOG2::OnSpinCtrlZoneChange(wxSpinEvent& event)
{
    for (auto f : plugin)
    	f->zone = SpinCtrlZone->GetValue();
}

void DialogBackgroundDiffMOG2::OnRadioBoxOperatorSelect(wxCommandEvent& event)
{
    for (auto f : plugin)
    	f->additive = RadioBoxOperator->GetSelection();
}

void DialogBackgroundDiffMOG2::OnSpinCtrlHistoryChange(wxSpinEvent& event)
{
    for (auto f : plugin)
    	f->SetHistory(event.GetValue());
}

void DialogBackgroundDiffMOG2::OnSpinCtrlThresholdChange(wxSpinDoubleEvent& event)
{
    for (auto f : plugin)
    	f->SetThreshold(event.GetValue());
}

void DialogBackgroundDiffMOG2::OnCheckBoxShadowDetectClick(wxCommandEvent& event)
{
    for (auto f : plugin)
    	f->SetShadowDetection(event.IsChecked());
}

void DialogBackgroundDiffMOG2::OnSpinCtrlLearningRateChange(wxSpinDoubleEvent& event)
{
    for (auto f : plugin)
    	f->learningRate = event.GetValue();
}
