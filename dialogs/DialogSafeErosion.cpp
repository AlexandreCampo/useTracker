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

#include "DialogSafeErosion.h"

//(*InternalHeaders(DialogSafeErosion)
#include <wx/string.h>
#include <wx/intl.h>
//*)

//(*IdInit(DialogSafeErosion)
const long DialogSafeErosion::ID_BUTTON2 = wxNewId();
//*)

BEGIN_EVENT_TABLE(DialogSafeErosion,wxDialog)
	//(*EventTable(DialogSafeErosion)
	//*)
END_EVENT_TABLE()

DialogSafeErosion::DialogSafeErosion(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
	//(*Initialize(DialogSafeErosion)
	wxFlexGridSizer* FlexGridSizer1;
	wxFlexGridSizer* FlexGridSizer2;
	wxStaticBoxSizer* StaticBoxSizer1;

	Create(parent, id, _("Safe erosion"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, _T("id"));
	SetClientSize(wxDefaultSize);
	Move(wxDefaultPosition);
	FlexGridSizer1 = new wxFlexGridSizer(0, 1, 0, 0);
	StaticBoxSizer1 = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Extract blobs"));
	FlexGridSizer1->Add(StaticBoxSizer1, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer2 = new wxFlexGridSizer(0, 3, 0, 0);
	buttonOk = new wxButton(this, ID_BUTTON2, _("Ok"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON2"));
	buttonOk->SetFocus();
	FlexGridSizer2->Add(buttonOk, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer1->Add(FlexGridSizer2, 1, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5);
	SetSizer(FlexGridSizer1);
	FlexGridSizer1->Fit(this);
	FlexGridSizer1->SetSizeHints(this);
	//*)

	Fit();
	Centre();
}

DialogSafeErosion::~DialogSafeErosion()
{
	//(*Destroy(DialogSafeErosion)
	//*)
}

void DialogSafeErosion::SetPlugin (std::vector<PipelinePlugin*> pfv)
{
    for (auto func : pfv)
	plugin.push_back(dynamic_cast<SafeErosion*> (func));
//    SpinCtrlSafeErosionSize->SetValue(f->size);
}

void DialogSafeErosion::OnButtonCancelClick(wxCommandEvent& event)
{
    this->Hide();
}


void DialogSafeErosion::OnButtonOkClick(wxCommandEvent& event)
{
    OnButtonApplyClick(event);
    this->Hide();
}

void DialogSafeErosion::OnSpinCtrlSafeErosionSizeChange(wxSpinEvent& event)
{
//    OnButtonApplyClick (event);
}

void DialogSafeErosion::OnButtonApplyClick(wxCommandEvent& event)
{
 //   plugin->size = SpinCtrlSafeErosionSize->GetValue();
}

