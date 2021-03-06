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

#include "DialogExtractBlobs.h"

//(*InternalHeaders(DialogExtractBlobs)
#include <wx/string.h>
#include <wx/intl.h>
//*)

#include <wx/filename.h>

//(*IdInit(DialogExtractBlobs)
const long DialogExtractBlobs::ID_STATICTEXT1 = wxNewId();
const long DialogExtractBlobs::ID_SPINCTRL1 = wxNewId();
const long DialogExtractBlobs::ID_STATICTEXT3 = wxNewId();
const long DialogExtractBlobs::ID_SPINCTRL2 = wxNewId();
const long DialogExtractBlobs::ID_STATICTEXT2 = wxNewId();
const long DialogExtractBlobs::ID_CHECKBOX2 = wxNewId();
const long DialogExtractBlobs::ID_CHECKBOX1 = wxNewId();
const long DialogExtractBlobs::ID_FILEPICKERCTRL1 = wxNewId();
const long DialogExtractBlobs::ID_BUTTON2 = wxNewId();
//*)

BEGIN_EVENT_TABLE(DialogExtractBlobs,wxDialog)
	//(*EventTable(DialogExtractBlobs)
	//*)
END_EVENT_TABLE()

DialogExtractBlobs::DialogExtractBlobs(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
	//(*Initialize(DialogExtractBlobs)
	wxStaticBoxSizer* StaticBoxSizer2;
	wxFlexGridSizer* FlexGridSizer1;
	wxFlexGridSizer* FlexGridSizer2;
	wxFlexGridSizer* FlexGridSizer3;
	wxFlexGridSizer* FlexGridSizer5;
	wxStaticBoxSizer* StaticBoxSizer1;

	Create(parent, wxID_ANY, _("Extract blobs"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, _T("wxID_ANY"));
	FlexGridSizer1 = new wxFlexGridSizer(0, 1, 0, 0);
	StaticBoxSizer1 = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Extract blobs"));
	FlexGridSizer3 = new wxFlexGridSizer(0, 2, 0, 0);
	StaticText1 = new wxStaticText(this, ID_STATICTEXT1, _("Minimum blob size (pixels)"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
	FlexGridSizer3->Add(StaticText1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	SpinCtrlExtractBlobsSize = new wxSpinCtrl(this, ID_SPINCTRL1, _T("1"), wxDefaultPosition, wxDefaultSize, 0, 1, 10000000, 1, _T("ID_SPINCTRL1"));
	SpinCtrlExtractBlobsSize->SetValue(_T("1"));
	FlexGridSizer3->Add(SpinCtrlExtractBlobsSize, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticText3 = new wxStaticText(this, ID_STATICTEXT3, _("Maximum blob size (pixels)"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT3"));
	FlexGridSizer3->Add(StaticText3, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	SpinCtrlMaxSize = new wxSpinCtrl(this, ID_SPINCTRL2, _T("0"), wxDefaultPosition, wxDefaultSize, 0, 0, 10000000, 0, _T("ID_SPINCTRL2"));
	SpinCtrlMaxSize->SetValue(_T("0"));
	FlexGridSizer3->Add(SpinCtrlMaxSize, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticText2 = new wxStaticText(this, ID_STATICTEXT2, _("Record labels (required to get angles)"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT2"));
	FlexGridSizer3->Add(StaticText2, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	CheckBoxRecordLabels = new wxCheckBox(this, ID_CHECKBOX2, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX2"));
	CheckBoxRecordLabels->SetValue(false);
	FlexGridSizer3->Add(CheckBoxRecordLabels, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticBoxSizer1->Add(FlexGridSizer3, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer1->Add(StaticBoxSizer1, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticBoxSizer2 = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Output to file"));
	FlexGridSizer5 = new wxFlexGridSizer(0, 2, 0, 0);
	FlexGridSizer5->AddGrowableCol(1);
	CheckBoxOutput = new wxCheckBox(this, ID_CHECKBOX1, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX1"));
	CheckBoxOutput->SetValue(false);
	FlexGridSizer5->Add(CheckBoxOutput, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FilePickerCtrl1 = new wxFilePickerCtrl(this, ID_FILEPICKERCTRL1, wxEmptyString, _("Choose an output file"), _T("*.csv"), wxDefaultPosition, wxDefaultSize, wxFLP_OVERWRITE_PROMPT|wxFLP_SAVE|wxFLP_USE_TEXTCTRL, wxDefaultValidator, _T("ID_FILEPICKERCTRL1"));
	FlexGridSizer5->Add(FilePickerCtrl1, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticBoxSizer2->Add(FlexGridSizer5, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer1->Add(StaticBoxSizer2, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer2 = new wxFlexGridSizer(0, 3, 0, 0);
	buttonOk = new wxButton(this, ID_BUTTON2, _("Ok"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON2"));
	buttonOk->SetFocus();
	FlexGridSizer2->Add(buttonOk, 1, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer1->Add(FlexGridSizer2, 1, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5);
	SetSizer(FlexGridSizer1);
	FlexGridSizer1->Fit(this);
	FlexGridSizer1->SetSizeHints(this);

	Connect(ID_SPINCTRL1,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&DialogExtractBlobs::OnSpinCtrlExtractBlobsSizeChange);
	Connect(ID_SPINCTRL2,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&DialogExtractBlobs::OnSpinCtrlMaxSizeChange);
	Connect(ID_CHECKBOX2,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&DialogExtractBlobs::OnCheckBoxRecordLabelsClick);
	Connect(ID_CHECKBOX1,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&DialogExtractBlobs::OnCheckBoxOutputClick);
	Connect(ID_FILEPICKERCTRL1,wxEVT_COMMAND_FILEPICKER_CHANGED,(wxObjectEventFunction)&DialogExtractBlobs::OnFilePickerCtrl1FileChanged);
	Connect(ID_BUTTON2,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&DialogExtractBlobs::OnButtonOkClick);
	//*)

	Fit();
	Centre();
}

DialogExtractBlobs::~DialogExtractBlobs()
{
	//(*Destroy(DialogExtractBlobs)
	//*)
}

void DialogExtractBlobs::SetPlugin (std::vector<PipelinePlugin*> pfv)
{
    for (auto func : pfv)
	plugin.push_back(dynamic_cast<ExtractBlobs*> (func));
    SpinCtrlExtractBlobsSize->SetValue(plugin[0]->minSize);
    SpinCtrlMaxSize->SetValue(plugin[0]->maxSize);
    FilePickerCtrl1->SetPath(plugin[0]->outputFilename);
    CheckBoxOutput->SetValue(plugin[0]->output);
    CheckBoxRecordLabels->SetValue(plugin[0]->recordLabels);
}

void DialogExtractBlobs::OnButtonCancelClick(wxCommandEvent& event)
{
    this->Hide();
}


void DialogExtractBlobs::OnButtonOkClick(wxCommandEvent& event)
{
    this->Hide();
}

void DialogExtractBlobs::OnSpinCtrlExtractBlobsSizeChange(wxSpinEvent& event)
{
    for (auto f : plugin)
    	f->minSize = SpinCtrlExtractBlobsSize->GetValue();
}

void DialogExtractBlobs::OnButtonApplyClick(wxCommandEvent& event)
{
}

void DialogExtractBlobs::OnFilePickerCtrl1FileChanged(wxFileDirPickerEvent& event)
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

void DialogExtractBlobs::OnCheckBoxOutputClick(wxCommandEvent& event)
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

void DialogExtractBlobs::OnCheckBoxRecordLabelsClick(wxCommandEvent& event)
{
    for (auto f : plugin)
	f->recordLabels = CheckBoxRecordLabels->IsChecked();
}

void DialogExtractBlobs::OnSpinCtrlMaxSizeChange(wxSpinEvent& event)
{
    for (auto f : plugin)
    	f->maxSize = SpinCtrlMaxSize->GetValue();
}
