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

#include "DialogBackgroundDiffGMG.h"

#include <iostream>

//(*InternalHeaders(DialogBackgroundDiffGMG)
#include <wx/string.h>
#include <wx/intl.h>
//*)

//(*IdInit(DialogBackgroundDiffGMG)
const long DialogBackgroundDiffGMG::ID_STATICTEXT5 = wxNewId();
const long DialogBackgroundDiffGMG::ID_SPINCTRL_LEARNINGRATE = wxNewId();
const long DialogBackgroundDiffGMG::ID_RADIOBOX2 = wxNewId();
const long DialogBackgroundDiffGMG::ID_CHECKBOX1 = wxNewId();
const long DialogBackgroundDiffGMG::ID_STATICTEXT6 = wxNewId();
const long DialogBackgroundDiffGMG::ID_SPINCTRL_ZONE = wxNewId();
const long DialogBackgroundDiffGMG::ID_BUTTON1 = wxNewId();
//*)

const long DialogBackgroundDiffGMG::ID_SPINCTRLD_BGRATIO = wxNewId();
const long DialogBackgroundDiffGMG::ID_SPINCTRLD_NOISESIGMA = wxNewId();
const long DialogBackgroundDiffGMG::ID_SPINCTRLD_LEARNINGRATE = wxNewId();


BEGIN_EVENT_TABLE(DialogBackgroundDiffGMG,wxDialog)
	//(*EventTable(DialogBackgroundDiffGMG)
	//*)
END_EVENT_TABLE()

DialogBackgroundDiffGMG::DialogBackgroundDiffGMG(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
	//(*Initialize(DialogBackgroundDiffGMG)
	wxFlexGridSizer* FlexGridSizer1;
	wxFlexGridSizer* FlexGridSizer2;
	wxFlexGridSizer* FlexGridSizer4;
	wxFlexGridSizer* FlexGridSizer3;
	wxStaticBoxSizer* StaticBoxSizer1;

	Create(parent, id, _("Background Difference GMG"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, _T("id"));
	SetClientSize(wxDefaultSize);
	Move(wxDefaultPosition);
	SetMinSize(wxSize(-1,-1));
	SetMaxSize(wxSize(-1,-1));
	SetFocus();
	FlexGridSizer3 = new wxFlexGridSizer(0, 1, 0, 0);
	StaticBoxSizer1 = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Background difference GMG"));
	FlexGridSizer1 = new wxFlexGridSizer(0, 2, 0, 0);
	StaticText5 = new wxStaticText(this, ID_STATICTEXT5, _("learning rate"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT5"));
	FlexGridSizer1->Add(StaticText5, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	SpinCtrlBackgroundDiffGMGLearningRate = new wxSpinCtrl(this, ID_SPINCTRL_LEARNINGRATE, _T("0"), wxDefaultPosition, wxDefaultSize, 0, 0, 100, 0, _T("ID_SPINCTRL_LEARNINGRATE"));
	SpinCtrlBackgroundDiffGMGLearningRate->SetValue(_T("0"));
	FlexGridSizer1->Add(SpinCtrlBackgroundDiffGMGLearningRate, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticBoxSizer1->Add(FlexGridSizer1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
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

	Connect(ID_SPINCTRL_LEARNINGRATE,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&DialogBackgroundDiffGMG::OnSpinCtrlBackgroundDiffGMGLearningRateChange);
	Connect(ID_RADIOBOX2,wxEVT_COMMAND_RADIOBOX_SELECTED,(wxObjectEventFunction)&DialogBackgroundDiffGMG::OnRadioBoxOperatorSelect);
	Connect(ID_CHECKBOX1,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&DialogBackgroundDiffGMG::OnCheckBoxRestrictToZoneClick);
	Connect(ID_SPINCTRL_ZONE,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&DialogBackgroundDiffGMG::OnSpinCtrlZoneChange);
	Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&DialogBackgroundDiffGMG::OnButtonOkClick);
	//*)

	// codeblocks 16 / wxsmith does not support spinctrldouble yet... adding manually
	SpinCtrlDoubleBackgroundDiffGMGLearningRate = new wxSpinCtrlDouble(this, ID_SPINCTRLD_LEARNINGRATE, _T("0"), wxDefaultPosition, wxDefaultSize, 0, 0, 1, 0.01, 0.001, _T("ID_SPINCTRLD_LEARNINGRATE"));
	FlexGridSizer1->Replace(SpinCtrlBackgroundDiffGMGLearningRate, SpinCtrlDoubleBackgroundDiffGMGLearningRate);
	SpinCtrlDoubleBackgroundDiffGMGLearningRate->Bind(wxEVT_SPINCTRLDOUBLE, &DialogBackgroundDiffGMG::OnSpinCtrlBackgroundDiffGMGLearningRateChange, this);

	Fit();
	Centre();
}

DialogBackgroundDiffGMG::~DialogBackgroundDiffGMG()
{
	//(*Destroy(DialogBackgroundDiffGMG)
	//*)
}

void DialogBackgroundDiffGMG::SetPlugin (std::vector<PipelinePlugin*> pfv)
{
    for (auto func : pfv)
	plugin.push_back(dynamic_cast<BackgroundDiffGMG*> (func));

    SpinCtrlDoubleBackgroundDiffGMGLearningRate->SetValue(plugin[0]->learningRate);

    SpinCtrlZone->SetValue(plugin[0]->zone);
    RadioBoxOperator->SetSelection(plugin[0]->additive);
    CheckBoxRestrictToZone->SetValue(plugin[0]->restrictToZone);
}

void DialogBackgroundDiffGMG::OnButtonCancelClick(wxCommandEvent& event)
{
    this->Hide();
}

void DialogBackgroundDiffGMG::OnButtonOkClick(wxCommandEvent& event)
{
//    OnButtonApplyClick(event);
    this->Hide();
}

void DialogBackgroundDiffGMG::OnSpinCtrlBackgroundDiffGMGSizeChange(wxSpinEvent& event)
{
    OnButtonApplyClick(event);
}

void DialogBackgroundDiffGMG::OnButtonApplyClick(wxCommandEvent& event)
{
}

void DialogBackgroundDiffGMG::OnSpinCtrlBackgroundDiffGMGLearningRateChange(wxSpinDoubleEvent& event)
{
    for (auto f : plugin)
    	f->learningRate = event.GetValue();
}

void DialogBackgroundDiffGMG::OnCheckBoxRestrictToZoneClick(wxCommandEvent& event)
{
    for (auto f : plugin)
    	f->restrictToZone = event.IsChecked();
}

void DialogBackgroundDiffGMG::OnSpinCtrlZoneChange(wxSpinEvent& event)
{
    for (auto f : plugin)
    	f->zone = SpinCtrlZone->GetValue();
}

void DialogBackgroundDiffGMG::OnRadioBoxOperatorSelect(wxCommandEvent& event)
{
    for (auto f : plugin)
    	f->additive = RadioBoxOperator->GetSelection();
}
