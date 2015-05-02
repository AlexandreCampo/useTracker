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
const long DialogExtractMotion::ID_SPINCTRL1 = wxNewId();
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
	wxFlexGridSizer* FlexGridSizer3;
	wxStaticBoxSizer* StaticBoxSizer1;

	Create(parent, id, _("Extract motion"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, _T("id"));
	SetClientSize(wxDefaultSize);
	Move(wxDefaultPosition);
	SetMinSize(wxSize(-1,-1));
	SetMaxSize(wxSize(-1,-1));
	SetFocus();
	FlexGridSizer3 = new wxFlexGridSizer(0, 1, 0, 0);
	StaticBoxSizer1 = new wxStaticBoxSizer(wxHORIZONTAL, this, _("ExtractMotion"));
	FlexGridSizer1 = new wxFlexGridSizer(0, 2, 0, 0);
	StaticText1 = new wxStaticText(this, ID_STATICTEXT1, _("Sensitivity to difference with background"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
	FlexGridSizer1->Add(StaticText1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	SpinCtrlExtractMotionSize = new wxSpinCtrl(this, ID_SPINCTRL1, _T("20"), wxDefaultPosition, wxDefaultSize, 0, 1, 1000, 20, _T("ID_SPINCTRL1"));
	SpinCtrlExtractMotionSize->SetValue(_T("20"));
	FlexGridSizer1->Add(SpinCtrlExtractMotionSize, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticBoxSizer1->Add(FlexGridSizer1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
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

	Connect(ID_SPINCTRL1,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&DialogExtractMotion::OnSpinCtrlExtractMotionSizeChange);
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
    SpinCtrlExtractMotionSize->SetValue(plugin[0]->threshold);
}

void DialogExtractMotion::OnButtonCancelClick(wxCommandEvent& event)
{
    this->Hide();
}

void DialogExtractMotion::OnButtonOkClick(wxCommandEvent& event)
{
    OnButtonApplyClick(event);
    this->Hide();
}

void DialogExtractMotion::OnSpinCtrlExtractMotionSizeChange(wxSpinEvent& event)
{
    OnButtonApplyClick(event);
}

void DialogExtractMotion::OnButtonApplyClick(wxCommandEvent& event)
{
    for (auto f : plugin)
    {
    	f->threshold = SpinCtrlExtractMotionSize->GetValue();
    }
}

