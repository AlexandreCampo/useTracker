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

#include "DialogBackgroundDiffMOG.h"

#include <iostream>

//(*InternalHeaders(DialogBackgroundDiffMOG)
#include <wx/string.h>
#include <wx/intl.h>
//*)

//(*IdInit(DialogBackgroundDiffMOG)
const long DialogBackgroundDiffMOG::ID_STATICTEXT1 = wxNewId();
const long DialogBackgroundDiffMOG::ID_SPINCTRL_HISTORY = wxNewId();
const long DialogBackgroundDiffMOG::ID_STATICTEXT2 = wxNewId();
const long DialogBackgroundDiffMOG::ID_SPINCTRL_NMIXTURES = wxNewId();
const long DialogBackgroundDiffMOG::ID_STATICTEXT3 = wxNewId();
const long DialogBackgroundDiffMOG::ID_SPINCTRL_BGRATIO = wxNewId();
const long DialogBackgroundDiffMOG::ID_STATICTEXT4 = wxNewId();
const long DialogBackgroundDiffMOG::ID_SPINCTRL_NOISESIGMA = wxNewId();
const long DialogBackgroundDiffMOG::ID_STATICTEXT5 = wxNewId();
const long DialogBackgroundDiffMOG::ID_SPINCTRL_LEARNINGRATE = wxNewId();
const long DialogBackgroundDiffMOG::ID_RADIOBOX2 = wxNewId();
const long DialogBackgroundDiffMOG::ID_CHECKBOX1 = wxNewId();
const long DialogBackgroundDiffMOG::ID_STATICTEXT6 = wxNewId();
const long DialogBackgroundDiffMOG::ID_SPINCTRL_ZONE = wxNewId();
const long DialogBackgroundDiffMOG::ID_BUTTON1 = wxNewId();
//*)

const long DialogBackgroundDiffMOG::ID_SPINCTRLD_BGRATIO = wxNewId();
const long DialogBackgroundDiffMOG::ID_SPINCTRLD_NOISESIGMA = wxNewId();
const long DialogBackgroundDiffMOG::ID_SPINCTRLD_LEARNINGRATE = wxNewId();


BEGIN_EVENT_TABLE(DialogBackgroundDiffMOG,wxDialog)
	//(*EventTable(DialogBackgroundDiffMOG)
	//*)
END_EVENT_TABLE()

DialogBackgroundDiffMOG::DialogBackgroundDiffMOG(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
	//(*Initialize(DialogBackgroundDiffMOG)
	wxFlexGridSizer* FlexGridSizer1;
	wxFlexGridSizer* FlexGridSizer2;
	wxFlexGridSizer* FlexGridSizer4;
	wxFlexGridSizer* FlexGridSizer3;
	wxStaticBoxSizer* StaticBoxSizer1;

	Create(parent, id, _("Background Difference MOG"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, _T("id"));
	SetClientSize(wxDefaultSize);
	Move(wxDefaultPosition);
	SetMinSize(wxSize(-1,-1));
	SetMaxSize(wxSize(-1,-1));
	SetFocus();
	FlexGridSizer3 = new wxFlexGridSizer(0, 1, 0, 0);
	StaticBoxSizer1 = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Background difference MOG"));
	FlexGridSizer1 = new wxFlexGridSizer(0, 2, 0, 0);
	StaticText1 = new wxStaticText(this, ID_STATICTEXT1, _("# of frames for history"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
	FlexGridSizer1->Add(StaticText1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	SpinCtrlBackgroundDiffMOGHistory = new wxSpinCtrl(this, ID_SPINCTRL_HISTORY, _T("200"), wxDefaultPosition, wxDefaultSize, 0, 1, 5000, 200, _T("ID_SPINCTRL_HISTORY"));
	SpinCtrlBackgroundDiffMOGHistory->SetValue(_T("200"));
	FlexGridSizer1->Add(SpinCtrlBackgroundDiffMOGHistory, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticText2 = new wxStaticText(this, ID_STATICTEXT2, _("# of gaussian mixtures"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT2"));
	FlexGridSizer1->Add(StaticText2, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	SpinCtrlBackgroundDiffMOGNMixtures = new wxSpinCtrl(this, ID_SPINCTRL_NMIXTURES, _T("5"), wxDefaultPosition, wxDefaultSize, 0, 1, 1000, 5, _T("ID_SPINCTRL_NMIXTURES"));
	SpinCtrlBackgroundDiffMOGNMixtures->SetValue(_T("5"));
	FlexGridSizer1->Add(SpinCtrlBackgroundDiffMOGNMixtures, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticText3 = new wxStaticText(this, ID_STATICTEXT3, _("background ratio"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT3"));
	FlexGridSizer1->Add(StaticText3, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	SpinCtrlBackgroundDiffMOGBackgroundRatio = new wxSpinCtrl(this, ID_SPINCTRL_BGRATIO, _T("0"), wxDefaultPosition, wxDefaultSize, 0, 0, 100, 0, _T("ID_SPINCTRL_BGRATIO"));
	SpinCtrlBackgroundDiffMOGBackgroundRatio->SetValue(_T("0"));
	FlexGridSizer1->Add(SpinCtrlBackgroundDiffMOGBackgroundRatio, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticText4 = new wxStaticText(this, ID_STATICTEXT4, _("noise sigma"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT4"));
	FlexGridSizer1->Add(StaticText4, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	SpinCtrlBackgroundDiffMOGNoiseSigma = new wxSpinCtrl(this, ID_SPINCTRL_NOISESIGMA, _T("0"), wxDefaultPosition, wxDefaultSize, 0, 0, 100, 0, _T("ID_SPINCTRL_NOISESIGMA"));
	SpinCtrlBackgroundDiffMOGNoiseSigma->SetValue(_T("0"));
	FlexGridSizer1->Add(SpinCtrlBackgroundDiffMOGNoiseSigma, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticText5 = new wxStaticText(this, ID_STATICTEXT5, _("learning rate"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT5"));
	FlexGridSizer1->Add(StaticText5, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	SpinCtrlBackgroundDiffMOGLearningRate = new wxSpinCtrl(this, ID_SPINCTRL_LEARNINGRATE, _T("0"), wxDefaultPosition, wxDefaultSize, 0, 0, 100, 0, _T("ID_SPINCTRL_LEARNINGRATE"));
	SpinCtrlBackgroundDiffMOGLearningRate->SetValue(_T("0"));
	FlexGridSizer1->Add(SpinCtrlBackgroundDiffMOGLearningRate, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticBoxSizer1->Add(FlexGridSizer1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer3->Add(StaticBoxSizer1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
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

	Connect(ID_SPINCTRL_HISTORY,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&DialogBackgroundDiffMOG::OnSpinCtrlBackgroundDiffMOGHistoryChange);
	Connect(ID_SPINCTRL_NMIXTURES,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&DialogBackgroundDiffMOG::OnSpinCtrlBackgroundDiffMOGNMixturesChange);
	Connect(ID_SPINCTRL_BGRATIO,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&DialogBackgroundDiffMOG::OnSpinCtrlBackgroundDiffMOGBackgroundRatioChange);
	Connect(ID_SPINCTRL_LEARNINGRATE,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&DialogBackgroundDiffMOG::OnSpinCtrlBackgroundDiffMOGLearningRateChange);
	Connect(ID_RADIOBOX2,wxEVT_COMMAND_RADIOBOX_SELECTED,(wxObjectEventFunction)&DialogBackgroundDiffMOG::OnRadioBoxOperatorSelect);
	Connect(ID_CHECKBOX1,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&DialogBackgroundDiffMOG::OnCheckBoxRestrictToZoneClick);
	Connect(ID_SPINCTRL_ZONE,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&DialogBackgroundDiffMOG::OnSpinCtrlZoneChange);
	Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&DialogBackgroundDiffMOG::OnButtonOkClick);
	//*)

	// codeblocks 16 / wxsmith does not support spinctrldouble yet... adding manually
	SpinCtrlDoubleBackgroundDiffMOGBackgroundRatio = new wxSpinCtrlDouble(this, ID_SPINCTRLD_BGRATIO, _T("0"), wxDefaultPosition, wxDefaultSize, 0, 0, 1, 0.7, 0.01, _T("ID_SPINCTRLD_BGRATIO"));
	FlexGridSizer1->Replace(SpinCtrlBackgroundDiffMOGBackgroundRatio, SpinCtrlDoubleBackgroundDiffMOGBackgroundRatio);
	SpinCtrlDoubleBackgroundDiffMOGBackgroundRatio->Bind(wxEVT_SPINCTRLDOUBLE, &DialogBackgroundDiffMOG::OnSpinCtrlBackgroundDiffMOGBackgroundRatioChange, this);

	SpinCtrlDoubleBackgroundDiffMOGNoiseSigma = new wxSpinCtrlDouble(this, ID_SPINCTRLD_NOISESIGMA, _T("0"), wxDefaultPosition, wxDefaultSize, 0, 0, 255, 15, 0.1, _T("ID_SPINCTRLD_NOISESIGMA"));
	FlexGridSizer1->Replace(SpinCtrlBackgroundDiffMOGNoiseSigma, SpinCtrlDoubleBackgroundDiffMOGNoiseSigma);
	SpinCtrlDoubleBackgroundDiffMOGNoiseSigma->Bind(wxEVT_SPINCTRLDOUBLE, &DialogBackgroundDiffMOG::OnSpinCtrlBackgroundDiffMOGNoiseSigmaChange, this);

	SpinCtrlDoubleBackgroundDiffMOGLearningRate = new wxSpinCtrlDouble(this, ID_SPINCTRLD_LEARNINGRATE, _T("0"), wxDefaultPosition, wxDefaultSize, 0, 0, 1, 0.01, 0.001, _T("ID_SPINCTRLD_LEARNINGRATE"));
	FlexGridSizer1->Replace(SpinCtrlBackgroundDiffMOGLearningRate, SpinCtrlDoubleBackgroundDiffMOGLearningRate);
	SpinCtrlDoubleBackgroundDiffMOGLearningRate->Bind(wxEVT_SPINCTRLDOUBLE, &DialogBackgroundDiffMOG::OnSpinCtrlBackgroundDiffMOGLearningRateChange, this);

	Fit();
	Centre();
}

DialogBackgroundDiffMOG::~DialogBackgroundDiffMOG()
{
	//(*Destroy(DialogBackgroundDiffMOG)
	//*)
}

void DialogBackgroundDiffMOG::SetPlugin (std::vector<PipelinePlugin*> pfv)
{
    for (auto func : pfv)
	plugin.push_back(dynamic_cast<BackgroundDiffMOG*> (func));

    SpinCtrlBackgroundDiffMOGHistory->SetValue(plugin[0]->history);
    SpinCtrlBackgroundDiffMOGNMixtures->SetValue(plugin[0]->nMixtures);
    SpinCtrlDoubleBackgroundDiffMOGBackgroundRatio->SetValue(plugin[0]->backgroundRatio);
    SpinCtrlDoubleBackgroundDiffMOGNoiseSigma->SetValue(plugin[0]->noiseSigma);
    SpinCtrlDoubleBackgroundDiffMOGLearningRate->SetValue(plugin[0]->learningRate);

    SpinCtrlZone->SetValue(plugin[0]->zone);
    RadioBoxOperator->SetSelection(plugin[0]->additive);
    CheckBoxRestrictToZone->SetValue(plugin[0]->restrictToZone);
}

void DialogBackgroundDiffMOG::OnButtonCancelClick(wxCommandEvent& event)
{
    this->Hide();
}

void DialogBackgroundDiffMOG::OnButtonOkClick(wxCommandEvent& event)
{
//    OnButtonApplyClick(event);
    this->Hide();
}

void DialogBackgroundDiffMOG::OnSpinCtrlBackgroundDiffMOGSizeChange(wxSpinEvent& event)
{
    OnButtonApplyClick(event);
}

void DialogBackgroundDiffMOG::OnButtonApplyClick(wxCommandEvent& event)
{
}

void DialogBackgroundDiffMOG::OnSpinCtrlBackgroundDiffMOGNMixturesChange(wxSpinEvent& event)
{
    for (auto f : plugin)
    	f->SetNMixtures(event.GetValue());
}

void DialogBackgroundDiffMOG::OnSpinCtrlBackgroundDiffMOGHistoryChange(wxSpinEvent& event)
{
    for (auto f : plugin)
    	f->SetHistory(event.GetValue());
}

void DialogBackgroundDiffMOG::OnSpinCtrlBackgroundDiffMOGBackgroundRatioChange(wxSpinDoubleEvent& event)
{
    for (auto f : plugin)
    	f->SetBackgroundRatio(event.GetValue());
}

void DialogBackgroundDiffMOG::OnSpinCtrlBackgroundDiffMOGNoiseSigmaChange(wxSpinDoubleEvent& event)
{
    for (auto f : plugin)
    	f->SetNoiseSigma(event.GetValue());
}

void DialogBackgroundDiffMOG::OnSpinCtrlBackgroundDiffMOGLearningRateChange(wxSpinDoubleEvent& event)
{
    for (auto f : plugin)
    	f->learningRate = event.GetValue();
}

void DialogBackgroundDiffMOG::OnCheckBoxRestrictToZoneClick(wxCommandEvent& event)
{
    for (auto f : plugin)
    	f->restrictToZone = event.IsChecked();
}

void DialogBackgroundDiffMOG::OnSpinCtrlZoneChange(wxSpinEvent& event)
{
    for (auto f : plugin)
    	f->zone = SpinCtrlZone->GetValue();
}

void DialogBackgroundDiffMOG::OnRadioBoxOperatorSelect(wxCommandEvent& event)
{
    for (auto f : plugin)
    	f->additive = RadioBoxOperator->GetSelection();
}
