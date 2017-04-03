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

#include "DialogAnthomology.h"

//(*InternalHeaders(DialogAnthomology)
#include <wx/string.h>
#include <wx/intl.h>
//*)

//(*IdInit(DialogAnthomology)
const long DialogAnthomology::ID_STATICTEXT1 = wxNewId();
const long DialogAnthomology::ID_SPINCTRL1 = wxNewId();
const long DialogAnthomology::ID_BUTTON1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(DialogAnthomology,wxDialog)
	//(*EventTable(DialogAnthomology)
	//*)
END_EVENT_TABLE()

DialogAnthomology::DialogAnthomology(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
	//(*Initialize(DialogAnthomology)
	wxFlexGridSizer* FlexGridSizer1;
	wxFlexGridSizer* FlexGridSizer2;
	wxFlexGridSizer* FlexGridSizer3;
	wxStaticBoxSizer* StaticBoxSizer1;

	Create(parent, id, _("Anthomology"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, _T("id"));
	SetClientSize(wxDefaultSize);
	Move(wxDefaultPosition);
	SetMinSize(wxSize(-1,-1));
	SetMaxSize(wxSize(-1,-1));
	SetFocus();
	FlexGridSizer3 = new wxFlexGridSizer(0, 1, 0, 0);
	StaticBoxSizer1 = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Anthomology"));
	FlexGridSizer1 = new wxFlexGridSizer(0, 2, 0, 0);
	StaticText1 = new wxStaticText(this, ID_STATICTEXT1, _("Shrink borders by (pixels)"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
	FlexGridSizer1->Add(StaticText1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	SpinCtrlAnthomologySize = new wxSpinCtrl(this, ID_SPINCTRL1, _T("1"), wxDefaultPosition, wxDefaultSize, 0, 1, 1000, 1, _T("ID_SPINCTRL1"));
	SpinCtrlAnthomologySize->SetValue(_T("1"));
	FlexGridSizer1->Add(SpinCtrlAnthomologySize, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
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

	Connect(ID_SPINCTRL1,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&DialogAnthomology::OnSpinCtrlAnthomologySizeChange);
	Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&DialogAnthomology::OnButtonOkClick);
	//*)

	Fit();
	Centre();
}

DialogAnthomology::~DialogAnthomology()
{
	//(*Destroy(DialogAnthomology)
	//*)
}

void DialogAnthomology::SetPlugin (std::vector<PipelinePlugin*> pfv)
{
    for (auto func : pfv)
	plugin.push_back(dynamic_cast<Anthomology*> (func));
    SpinCtrlAnthomologySize->SetValue(plugin[0]->size);
}

void DialogAnthomology::OnButtonCancelClick(wxCommandEvent& event)
{
    this->Hide();
}


void DialogAnthomology::OnButtonOkClick(wxCommandEvent& event)
{
    OnButtonApplyClick(event);
    this->Hide();
}

void DialogAnthomology::OnSpinCtrlAnthomologySizeChange(wxSpinEvent& event)
{
    OnButtonApplyClick (event);
}

void DialogAnthomology::OnButtonApplyClick(wxCommandEvent& event)
{
    for (auto f : plugin)
    	f->SetSize(SpinCtrlAnthomologySize->GetValue());
}

