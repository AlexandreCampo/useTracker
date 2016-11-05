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

#include "DialogTakeSnapshots.h"

//(*InternalHeaders(DialogTakeSnapshots)
#include <wx/string.h>
#include <wx/intl.h>
//*)

#include <wx/filename.h>

//(*IdInit(DialogTakeSnapshots)
const long DialogTakeSnapshots::ID_CHECKBOX1 = wxNewId();
const long DialogTakeSnapshots::ID_FILEPICKERCTRL1 = wxNewId();
const long DialogTakeSnapshots::ID_STATICTEXT1 = wxNewId();
const long DialogTakeSnapshots::ID_BUTTON2 = wxNewId();
//*)

BEGIN_EVENT_TABLE(DialogTakeSnapshots,wxDialog)
	//(*EventTable(DialogTakeSnapshots)
	//*)
END_EVENT_TABLE()

DialogTakeSnapshots::DialogTakeSnapshots(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
	//(*Initialize(DialogTakeSnapshots)
	wxStaticBoxSizer* StaticBoxSizer2;
	wxFlexGridSizer* FlexGridSizer1;
	wxFlexGridSizer* FlexGridSizer2;
	wxFlexGridSizer* FlexGridSizer5;

	Create(parent, wxID_ANY, _("Extract blobs"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, _T("wxID_ANY"));
	FlexGridSizer1 = new wxFlexGridSizer(0, 1, 0, 0);
	StaticBoxSizer2 = new wxStaticBoxSizer(wxVERTICAL, this, _("Save detected pixels to images"));
	FlexGridSizer5 = new wxFlexGridSizer(0, 2, 0, 0);
	FlexGridSizer5->AddGrowableCol(1);
	CheckBoxOutput = new wxCheckBox(this, ID_CHECKBOX1, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX1"));
	CheckBoxOutput->SetValue(false);
	FlexGridSizer5->Add(CheckBoxOutput, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FilePickerCtrl1 = new wxFilePickerCtrl(this, ID_FILEPICKERCTRL1, wxEmptyString, _("Choose an output file"), _T("*.csv"), wxDefaultPosition, wxDefaultSize, wxFLP_OVERWRITE_PROMPT|wxFLP_SAVE|wxFLP_USE_TEXTCTRL, wxDefaultValidator, _T("ID_FILEPICKERCTRL1"));
	FlexGridSizer5->Add(FilePickerCtrl1, 1, wxALL|wxEXPAND, 5);
	StaticBoxSizer2->Add(FlexGridSizer5, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticText1 = new wxStaticText(this, ID_STATICTEXT1, _("(frame numbers will be appended)"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
	StaticBoxSizer2->Add(StaticText1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer1->Add(StaticBoxSizer2, 1, wxALL|wxEXPAND, 5);
	FlexGridSizer2 = new wxFlexGridSizer(0, 3, 0, 0);
	buttonOk = new wxButton(this, ID_BUTTON2, _("Ok"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON2"));
	buttonOk->SetFocus();
	FlexGridSizer2->Add(buttonOk, 1, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer1->Add(FlexGridSizer2, 1, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5);
	SetSizer(FlexGridSizer1);
	FlexGridSizer1->Fit(this);
	FlexGridSizer1->SetSizeHints(this);

	Connect(ID_CHECKBOX1,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&DialogTakeSnapshots::OnCheckBoxOutputClick);
	Connect(ID_FILEPICKERCTRL1,wxEVT_COMMAND_FILEPICKER_CHANGED,(wxObjectEventFunction)&DialogTakeSnapshots::OnFilePickerCtrl1FileChanged);
	Connect(ID_BUTTON2,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&DialogTakeSnapshots::OnButtonOkClick);
	//*)

	Fit();
	Centre();
}

DialogTakeSnapshots::~DialogTakeSnapshots()
{
	//(*Destroy(DialogTakeSnapshots)
	//*)
}

void DialogTakeSnapshots::SetPlugin (std::vector<PipelinePlugin*> pfv)
{
    for (auto func : pfv)
	plugin.push_back(dynamic_cast<TakeSnapshots*> (func));

    FilePickerCtrl1->SetPath(plugin[0]->outputFilename);
    CheckBoxOutput->SetValue(plugin[0]->output);
}

void DialogTakeSnapshots::OnButtonCancelClick(wxCommandEvent& event)
{
    this->Hide();
}


void DialogTakeSnapshots::OnButtonOkClick(wxCommandEvent& event)
{
    this->Hide();
}


void DialogTakeSnapshots::OnButtonApplyClick(wxCommandEvent& event)
{
}

void DialogTakeSnapshots::OnFilePickerCtrl1FileChanged(wxFileDirPickerEvent& event)
{
    wxFileName filename (FilePickerCtrl1->GetFileName());
    wxString path = filename.GetPath();

    if (filename.DirExists() || path.IsEmpty())
    {
	if (filename.GetFullPath() != plugin[0]->outputFilename)
	{
	    for (auto f : plugin)
	    {
		f->CloseOutput();
		f->outputFilename = filename.GetFullPath();
		f->output = false;
	    }
	    CheckBoxOutput->SetValue(false);

	    if (!path.IsEmpty())
		wxSetWorkingDirectory(path);
	}
    }
}

void DialogTakeSnapshots::OnCheckBoxOutputClick(wxCommandEvent& event)
{
    for (auto f : plugin)
    {
	f->output = CheckBoxOutput->IsChecked();

	if (f->output)
	    f->OpenOutput();
	else
	    f->CloseOutput();
    }
}

