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

#include "DialogExtractMotion.h"

//(*InternalHeaders(DialogExtractMotion)
#include <wx/string.h>
#include <wx/intl.h>
//*)


//(*IdInit(DialogExtractMotion)
const long DialogExtractMotion::ID_STATICTEXT1 = wxNewId();
const long DialogExtractMotion::ID_SPINCTRL_THRESHOLD = wxNewId();
const long DialogExtractMotion::ID_RADIOBOX1 = wxNewId();
const long DialogExtractMotion::ID_CHECKBOX1 = wxNewId();
const long DialogExtractMotion::ID_STATICTEXT2 = wxNewId();
const long DialogExtractMotion::ID_SPINCTRL_ZONE = wxNewId();
const long DialogExtractMotion::ID_BUTTON1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(DialogExtractMotion,wxDialog)
	//(*EventTable(DialogExtractMotion)
	//*)
END_EVENT_TABLE()

DialogExtractMotion::DialogExtractMotion(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
	//(*Initialize(DialogExtractMotion)
	wxFlexGridSizer* FlexGridSizer1;
	wxFlexGridSizer* FlexGridSizer2;
	wxFlexGridSizer* FlexGridSizer4;
	wxFlexGridSizer* FlexGridSizer6;
	wxFlexGridSizer* FlexGridSizer3;
	wxFlexGridSizer* FlexGridSizer5;
	wxStaticBoxSizer* StaticBoxSizer1;

	Create(parent, wxID_ANY, _("Extract motion"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, _T("wxID_ANY"));
	SetMinSize(wxSize(-1,-1));
	SetMaxSize(wxSize(-1,-1));
	SetFocus();
	FlexGridSizer3 = new wxFlexGridSizer(0, 1, 0, 0);
	StaticBoxSizer1 = new wxStaticBoxSizer(wxVERTICAL, this, _("Background difference"));
	FlexGridSizer6 = new wxFlexGridSizer(3, 1, 0, 0);
	FlexGridSizer6->AddGrowableCol(0);
	FlexGridSizer1 = new wxFlexGridSizer(0, 2, 0, 0);
	StaticText1 = new wxStaticText(this, ID_STATICTEXT1, _("Sensitivity to difference with background"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
	FlexGridSizer1->Add(StaticText1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	SpinCtrlThreshold = new wxSpinCtrl(this, ID_SPINCTRL_THRESHOLD, _T("20"), wxDefaultPosition, wxDefaultSize, 0, 1, 1000, 20, _T("ID_SPINCTRL_THRESHOLD"));
	SpinCtrlThreshold->SetValue(_T("20"));
	FlexGridSizer1->Add(SpinCtrlThreshold, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer6->Add(FlexGridSizer1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer5 = new wxFlexGridSizer(0, 1, 0, 0);
	FlexGridSizer5->AddGrowableCol(0);
	wxString __wxRadioBoxChoices_1[2] =
	{
		_("Confirm previously detected pixels (AND operator)"),
		_("Add to previously detected pixels (OR operator)")
	};
	RadioBox1 = new wxRadioBox(this, ID_RADIOBOX1, _("Action type"), wxDefaultPosition, wxDefaultSize, 2, __wxRadioBoxChoices_1, 2, 0, wxDefaultValidator, _T("ID_RADIOBOX1"));
	RadioBox1->SetSelection(0);
	FlexGridSizer5->Add(RadioBox1, 1, wxALL|wxEXPAND, 5);
	FlexGridSizer6->Add(FlexGridSizer5, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer4 = new wxFlexGridSizer(0, 3, 0, 0);
	FlexGridSizer4->AddGrowableCol(0);
	CheckBoxRestrictToZone = new wxCheckBox(this, ID_CHECKBOX1, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX1"));
	CheckBoxRestrictToZone->SetValue(false);
	FlexGridSizer4->Add(CheckBoxRestrictToZone, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticText2 = new wxStaticText(this, ID_STATICTEXT2, _("Restrict to zone (grey level)"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT2"));
	FlexGridSizer4->Add(StaticText2, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	SpinCtrlZone = new wxSpinCtrl(this, ID_SPINCTRL_ZONE, _T("0"), wxDefaultPosition, wxDefaultSize, 0, 0, 255, 0, _T("ID_SPINCTRL_ZONE"));
	SpinCtrlZone->SetValue(_T("0"));
	FlexGridSizer4->Add(SpinCtrlZone, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer6->Add(FlexGridSizer4, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
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

	Connect(ID_SPINCTRL_THRESHOLD,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&DialogExtractMotion::OnSpinCtrlThresholdChange);
	Connect(ID_RADIOBOX1,wxEVT_COMMAND_RADIOBOX_SELECTED,(wxObjectEventFunction)&DialogExtractMotion::OnRadioBox1Select);
	Connect(ID_CHECKBOX1,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&DialogExtractMotion::OnCheckBoxRestrictToZoneClick);
	Connect(ID_SPINCTRL_ZONE,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&DialogExtractMotion::OnSpinCtrlZoneChange);
	Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&DialogExtractMotion::OnButtonOkClick);
	//*)

	Fit();
	Centre();
}

DialogExtractMotion::~DialogExtractMotion()
{
	//(*Destroy(DialogExtractMotion)
	//*)
}

void DialogExtractMotion::SetPlugin (std::vector<PipelinePlugin*> pfv)
{
    for (auto func : pfv)
	plugin.push_back(dynamic_cast<ExtractMotion*> (func));

    SpinCtrlThreshold->SetValue(plugin[0]->threshold);

    SpinCtrlZone->SetValue(plugin[0]->zone);
    RadioBox1->SetSelection(plugin[0]->additive);
    CheckBoxRestrictToZone->SetValue(plugin[0]->restrictToZone);
}

void DialogExtractMotion::OnButtonCancelClick(wxCommandEvent& event)
{
    this->Hide();
}

void DialogExtractMotion::OnButtonOkClick(wxCommandEvent& event)
{
//    OnButtonApplyClick(event);
    this->Hide();
}

void DialogExtractMotion::OnButtonApplyClick(wxCommandEvent& event)
{
}


void DialogExtractMotion::OnCheckBoxRestrictToZoneClick(wxCommandEvent& event)
{
    for (auto f : plugin)
    	f->restrictToZone = event.IsChecked();
}

void DialogExtractMotion::OnRadioBox1Select(wxCommandEvent& event)
{
    for (auto f : plugin)
    	f->additive = RadioBox1->GetSelection();
}

void DialogExtractMotion::OnSpinCtrlZoneChange(wxSpinEvent& event)
{
    for (auto f : plugin)
    	f->zone = SpinCtrlZone->GetValue();
}

void DialogExtractMotion::OnSpinCtrlThresholdChange(wxSpinEvent& event)
{
    for (auto f : plugin)
    	f->threshold = SpinCtrlThreshold->GetValue();
}
