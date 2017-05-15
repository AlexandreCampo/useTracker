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

#include "DialogArucoColor.h"

//(*InternalHeaders(DialogArucoColor)
#include <wx/string.h>
#include <wx/intl.h>
//*)

//(*IdInit(DialogArucoColor)
const long DialogArucoColor::ID_STATICTEXT1 = wxNewId();
const long DialogArucoColor::ID_SPINCTRL1 = wxNewId();
const long DialogArucoColor::ID_STATICTEXT2 = wxNewId();
const long DialogArucoColor::ID_SPINCTRL2 = wxNewId();
const long DialogArucoColor::ID_STATICTEXT3 = wxNewId();
const long DialogArucoColor::ID_TEXTCTRL1 = wxNewId();
const long DialogArucoColor::ID_BUTTON2 = wxNewId();
const long DialogArucoColor::ID_STATICTEXT4 = wxNewId();
const long DialogArucoColor::ID_TEXTCTRL2 = wxNewId();
const long DialogArucoColor::ID_BUTTON3 = wxNewId();
const long DialogArucoColor::ID_STATICTEXT5 = wxNewId();
const long DialogArucoColor::ID_SPINCTRL4 = wxNewId();
const long DialogArucoColor::ID_STATICTEXT6 = wxNewId();
const long DialogArucoColor::ID_SPINCTRL3 = wxNewId();
const long DialogArucoColor::ID_STATICTEXT7 = wxNewId();
const long DialogArucoColor::ID_SPINCTRL5 = wxNewId();
const long DialogArucoColor::ID_STATICTEXT8 = wxNewId();
const long DialogArucoColor::ID_SPINCTRL6 = wxNewId();
const long DialogArucoColor::ID_STATICTEXT9 = wxNewId();
const long DialogArucoColor::ID_SPINCTRL7 = wxNewId();
const long DialogArucoColor::ID_STATICTEXT10 = wxNewId();
const long DialogArucoColor::ID_SPINCTRL8 = wxNewId();
const long DialogArucoColor::ID_STATICTEXT11 = wxNewId();
const long DialogArucoColor::ID_SPINCTRL9 = wxNewId();
const long DialogArucoColor::ID_STATICTEXT12 = wxNewId();
const long DialogArucoColor::ID_SPINCTRL10 = wxNewId();
const long DialogArucoColor::ID_CHECKBOX1 = wxNewId();
const long DialogArucoColor::ID_FILEPICKERCTRL1 = wxNewId();
const long DialogArucoColor::ID_BUTTON1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(DialogArucoColor,wxDialog)
	//(*EventTable(DialogArucoColor)
	//*)
END_EVENT_TABLE()

DialogArucoColor::DialogArucoColor(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
	//(*Initialize(DialogArucoColor)
	wxStaticBoxSizer* StaticBoxSizer2;
	wxFlexGridSizer* FlexGridSizer8;
	wxFlexGridSizer* FlexGridSizer1;
	wxFlexGridSizer* FlexGridSizer2;
	wxStaticBoxSizer* StaticBoxSizer5;
	wxFlexGridSizer* FlexGridSizer7;
	wxFlexGridSizer* FlexGridSizer4;
	wxFlexGridSizer* FlexGridSizer9;
	wxStaticBoxSizer* StaticBoxSizer3;
	wxFlexGridSizer* FlexGridSizer6;
	wxFlexGridSizer* FlexGridSizer3;
	wxStaticBoxSizer* StaticBoxSizer4;
	wxFlexGridSizer* FlexGridSizer5;

	Create(parent, wxID_ANY, _("ArucoColor"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, _T("wxID_ANY"));
	SetMinSize(wxSize(-1,-1));
	SetMaxSize(wxSize(-1,-1));
	SetFocus();
	FlexGridSizer3 = new wxFlexGridSizer(0, 1, 0, 0);
	FlexGridSizer6 = new wxFlexGridSizer(0, 1, 0, 0);
	FlexGridSizer6->AddGrowableCol(0);
	StaticBoxSizer2 = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Markers"));
	FlexGridSizer8 = new wxFlexGridSizer(0, 1, 0, 0);
	FlexGridSizer1 = new wxFlexGridSizer(0, 2, 0, 0);
	FlexGridSizer1->AddGrowableCol(0);
	StaticText1 = new wxStaticText(this, ID_STATICTEXT1, _("Markers number of columns (pixels)"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
	FlexGridSizer1->Add(StaticText1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 1);
	SpinCtrlMarkerCols = new wxSpinCtrl(this, ID_SPINCTRL1, _T("2"), wxDefaultPosition, wxDefaultSize, 0, 1, 1000, 2, _T("ID_SPINCTRL1"));
	SpinCtrlMarkerCols->SetValue(_T("2"));
	FlexGridSizer1->Add(SpinCtrlMarkerCols, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 1);
	StaticText2 = new wxStaticText(this, ID_STATICTEXT2, _("Markers number of rows (pixels)"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT2"));
	FlexGridSizer1->Add(StaticText2, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 1);
	SpinCtrlMarkerRows = new wxSpinCtrl(this, ID_SPINCTRL2, _T("2"), wxDefaultPosition, wxDefaultSize, 0, 1, 1000, 2, _T("ID_SPINCTRL2"));
	SpinCtrlMarkerRows->SetValue(_T("2"));
	FlexGridSizer1->Add(SpinCtrlMarkerRows, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 1);
	FlexGridSizer8->Add(FlexGridSizer1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer7 = new wxFlexGridSizer(0, 3, 0, 0);
	FlexGridSizer7->AddGrowableCol(1);
	StaticText3 = new wxStaticText(this, ID_STATICTEXT3, _("Reference hues\n (list of hues, 0 to 180)"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE, _T("ID_STATICTEXT3"));
	FlexGridSizer7->Add(StaticText3, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	TextCtrlRefHues = new wxTextCtrl(this, ID_TEXTCTRL1, _("0 45 90 135"), wxDefaultPosition, wxSize(200,40), wxTE_AUTO_SCROLL|wxTE_PROCESS_ENTER|wxTE_MULTILINE|wxTE_WORDWRAP, wxDefaultValidator, _T("ID_TEXTCTRL1"));
	TextCtrlRefHues->SetMinSize(wxSize(-1,-1));
	TextCtrlRefHues->SetMaxSize(wxSize(-1,-1));
	FlexGridSizer7->Add(TextCtrlRefHues, 1, wxALL|wxEXPAND, 5);
	ButtonUpdateRefHues = new wxButton(this, ID_BUTTON2, _("Update"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON2"));
	FlexGridSizer7->Add(ButtonUpdateRefHues, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticText4 = new wxStaticText(this, ID_STATICTEXT4, _("Dictionary \n(list of codes)"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE, _T("ID_STATICTEXT4"));
	FlexGridSizer7->Add(StaticText4, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	TextCtrlDictionary = new wxTextCtrl(this, ID_TEXTCTRL2, _("1000 2000"), wxDefaultPosition, wxSize(200,100), wxTE_AUTO_SCROLL|wxTE_PROCESS_ENTER|wxTE_MULTILINE|wxTE_WORDWRAP, wxDefaultValidator, _T("ID_TEXTCTRL2"));
	FlexGridSizer7->Add(TextCtrlDictionary, 1, wxALL|wxEXPAND, 5);
	ButtonUpdateDictionary = new wxButton(this, ID_BUTTON3, _("Update"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON3"));
	FlexGridSizer7->Add(ButtonUpdateDictionary, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer8->Add(FlexGridSizer7, 1, wxALL|wxEXPAND, 5);
	StaticBoxSizer2->Add(FlexGridSizer8, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer6->Add(StaticBoxSizer2, 1, wxALL|wxEXPAND, 5);
	StaticBoxSizer3 = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Detection"));
	FlexGridSizer4 = new wxFlexGridSizer(0, 2, 0, 0);
	FlexGridSizer4->AddGrowableCol(0);
	StaticText5 = new wxStaticText(this, ID_STATICTEXT5, _("Adaptive threshold block size"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT5"));
	FlexGridSizer4->Add(StaticText5, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 1);
	SpinCtrlAtBlockSize = new wxSpinCtrl(this, ID_SPINCTRL4, _T("13"), wxDefaultPosition, wxDefaultSize, 0, 1, 1000, 13, _T("ID_SPINCTRL4"));
	SpinCtrlAtBlockSize->SetValue(_T("13"));
	FlexGridSizer4->Add(SpinCtrlAtBlockSize, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 1);
	StaticText6 = new wxStaticText(this, ID_STATICTEXT6, _("Adaptive threshold constant"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT6"));
	FlexGridSizer4->Add(StaticText6, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 1);
	SpinCtrlAtConstant = new wxSpinCtrl(this, ID_SPINCTRL3, _T("-5"), wxDefaultPosition, wxDefaultSize, 0, -255, 255, -5, _T("ID_SPINCTRL3"));
	SpinCtrlAtConstant->SetValue(_T("-5"));
	FlexGridSizer4->Add(SpinCtrlAtConstant, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 1);
	StaticText7 = new wxStaticText(this, ID_STATICTEXT7, _("Saturation threshold (from HSV, retain above)"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT7"));
	FlexGridSizer4->Add(StaticText7, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 1);
	SpinCtrlSaturationThreshold = new wxSpinCtrl(this, ID_SPINCTRL5, _T("60"), wxDefaultPosition, wxDefaultSize, 0, 0, 255, 60, _T("ID_SPINCTRL5"));
	SpinCtrlSaturationThreshold->SetValue(_T("60"));
	FlexGridSizer4->Add(SpinCtrlSaturationThreshold, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 1);
	StaticText8 = new wxStaticText(this, ID_STATICTEXT8, _("Value threshold (from HSV, retain above)"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT8"));
	FlexGridSizer4->Add(StaticText8, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 1);
	SpinCtrlValueThreshold = new wxSpinCtrl(this, ID_SPINCTRL6, _T("100"), wxDefaultPosition, wxDefaultSize, 0, 0, 255, 100, _T("ID_SPINCTRL6"));
	SpinCtrlValueThreshold->SetValue(_T("100"));
	FlexGridSizer4->Add(SpinCtrlValueThreshold, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 1);
	StaticText9 = new wxStaticText(this, ID_STATICTEXT9, _("Minimum marker area"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT9"));
	FlexGridSizer4->Add(StaticText9, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 1);
	SpinCtrlMinArea = new wxSpinCtrl(this, ID_SPINCTRL7, _T("150"), wxDefaultPosition, wxDefaultSize, 0, 0, 1000000000, 150, _T("ID_SPINCTRL7"));
	SpinCtrlMinArea->SetValue(_T("150"));
	FlexGridSizer4->Add(SpinCtrlMinArea, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 1);
	StaticText10 = new wxStaticText(this, ID_STATICTEXT10, _("Maximum marker area"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT10"));
	FlexGridSizer4->Add(StaticText10, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 1);
	SpinCtrlMaxArea = new wxSpinCtrl(this, ID_SPINCTRL8, _T("1600"), wxDefaultPosition, wxDefaultSize, 0, 0, 1000000000, 1600, _T("ID_SPINCTRL8"));
	SpinCtrlMaxArea->SetValue(_T("1600"));
	FlexGridSizer4->Add(SpinCtrlMaxArea, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 1);
	StaticBoxSizer3->Add(FlexGridSizer4, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer6->Add(StaticBoxSizer3, 1, wxALL|wxEXPAND, 5);
	StaticBoxSizer4 = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Recognition"));
	FlexGridSizer5 = new wxFlexGridSizer(0, 2, 0, 0);
	FlexGridSizer5->AddGrowableCol(0);
	StaticText11 = new wxStaticText(this, ID_STATICTEXT11, _("Maximum hue deviation"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT11"));
	FlexGridSizer5->Add(StaticText11, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 1);
	SpinCtrlMaxHueDeviation = new wxSpinCtrl(this, ID_SPINCTRL9, _T("25"), wxDefaultPosition, wxDefaultSize, 0, 0, 180, 25, _T("ID_SPINCTRL9"));
	SpinCtrlMaxHueDeviation->SetValue(_T("25"));
	FlexGridSizer5->Add(SpinCtrlMaxHueDeviation, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 1);
	StaticText12 = new wxStaticText(this, ID_STATICTEXT12, _("Maximum marker range"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT12"));
	FlexGridSizer5->Add(StaticText12, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 1);
	SpinCtrlMaxMarkerRange = new wxSpinCtrl(this, ID_SPINCTRL10, _T("4"), wxDefaultPosition, wxDefaultSize, 0, 0, 10000, 4, _T("ID_SPINCTRL10"));
	SpinCtrlMaxMarkerRange->SetValue(_T("4"));
	FlexGridSizer5->Add(SpinCtrlMaxMarkerRange, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 1);
	StaticBoxSizer4->Add(FlexGridSizer5, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer6->Add(StaticBoxSizer4, 1, wxALL|wxEXPAND, 5);
	StaticBoxSizer5 = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Label"));
	FlexGridSizer9 = new wxFlexGridSizer(0, 2, 0, 0);
	FlexGridSizer9->AddGrowableCol(1);
	CheckBoxOutput = new wxCheckBox(this, ID_CHECKBOX1, _("Output to file"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX1"));
	CheckBoxOutput->SetValue(false);
	FlexGridSizer9->Add(CheckBoxOutput, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FilePickerCtrl1 = new wxFilePickerCtrl(this, ID_FILEPICKERCTRL1, wxEmptyString, wxEmptyString, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxFLP_OVERWRITE_PROMPT|wxFLP_SAVE|wxFLP_USE_TEXTCTRL, wxDefaultValidator, _T("ID_FILEPICKERCTRL1"));
	FlexGridSizer9->Add(FilePickerCtrl1, 1, wxALL|wxEXPAND, 5);
	StaticBoxSizer5->Add(FlexGridSizer9, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer6->Add(StaticBoxSizer5, 1, wxALL|wxEXPAND, 5);
	FlexGridSizer3->Add(FlexGridSizer6, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer2 = new wxFlexGridSizer(0, 3, 0, 0);
	ButtonOk = new wxButton(this, ID_BUTTON1, _("OK"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
	ButtonOk->SetDefault();
	ButtonOk->SetFocus();
	FlexGridSizer2->Add(ButtonOk, 1, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer3->Add(FlexGridSizer2, 1, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5);
	SetSizer(FlexGridSizer3);
	FlexGridSizer3->Fit(this);
	FlexGridSizer3->SetSizeHints(this);

	Connect(ID_SPINCTRL1,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&DialogArucoColor::OnSpinCtrlMarkerColsChange);
	Connect(ID_SPINCTRL2,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&DialogArucoColor::OnSpinCtrlMarkerRowsChange);
	Connect(ID_BUTTON2,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&DialogArucoColor::OnButtonUpdateRefHuesClick);
	Connect(ID_BUTTON3,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&DialogArucoColor::OnButtonUpdateDictionaryClick);
	Connect(ID_SPINCTRL4,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&DialogArucoColor::OnSpinCtrlAtBlockSizeChange);
	Connect(ID_SPINCTRL3,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&DialogArucoColor::OnSpinCtrlAtConstantChange);
	Connect(ID_SPINCTRL5,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&DialogArucoColor::OnSpinCtrlSaturationThresholdChange);
	Connect(ID_SPINCTRL6,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&DialogArucoColor::OnSpinCtrlValueThresholdChange);
	Connect(ID_SPINCTRL7,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&DialogArucoColor::OnSpinCtrlMinAreaChange);
	Connect(ID_SPINCTRL8,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&DialogArucoColor::OnSpinCtrlMaxAreaChange);
	Connect(ID_SPINCTRL9,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&DialogArucoColor::OnSpinCtrlMaxHueDeviationChange);
	Connect(ID_SPINCTRL10,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&DialogArucoColor::OnSpinCtrlMaxMarkerRangeChange);
	Connect(ID_CHECKBOX1,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&DialogArucoColor::OnCheckBoxOutputClick);
	Connect(ID_FILEPICKERCTRL1,wxEVT_COMMAND_FILEPICKER_CHANGED,(wxObjectEventFunction)&DialogArucoColor::OnFilePickerCtrl1FileChanged);
	Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&DialogArucoColor::OnButtonOkClick);
	//*)

	Fit();
	Centre();
}

DialogArucoColor::~DialogArucoColor()
{
	//(*Destroy(DialogArucoColor)
	//*)
}

void DialogArucoColor::SetPlugin (std::vector<PipelinePlugin*> pfv)
{
    for (auto func : pfv)
	plugin.push_back(dynamic_cast<ArucoColor*> (func));

    SpinCtrlMarkerCols->SetValue(plugin[0]->GetMarkerCols());
    SpinCtrlMarkerRows->SetValue(plugin[0]->GetMarkerRows());
    SpinCtrlSaturationThreshold->SetValue(plugin[0]->GetSaturationThreshold());
    SpinCtrlValueThreshold->SetValue(plugin[0]->GetValueThreshold());
    SpinCtrlAtBlockSize->SetValue(plugin[0]->GetAdaptiveThresholdBlockSize());
    SpinCtrlAtConstant->SetValue(plugin[0]->GetAdaptiveThresholdConstant());
    SpinCtrlMinArea->SetValue(plugin[0]->GetMinMarkerArea());
    SpinCtrlMaxArea->SetValue(plugin[0]->GetMaxMarkerArea());
    SpinCtrlMaxHueDeviation->SetValue(plugin[0]->GetMaxHueDeviation());
    SpinCtrlMaxMarkerRange->SetValue(plugin[0]->GetMaxMarkerRange());
    TextCtrlDictionary->SetValue(plugin[0]->GetDictionaryString());
    TextCtrlRefHues->SetValue(plugin[0]->GetReferenceHuesString());

    FilePickerCtrl1->SetPath(plugin[0]->outputFilename);
    CheckBoxOutput->SetValue(plugin[0]->output);
}

void DialogArucoColor::OnButtonCancelClick(wxCommandEvent& event)
{
    this->Hide();
}


void DialogArucoColor::OnButtonOkClick(wxCommandEvent& event)
{
    this->Hide();
}

void DialogArucoColor::OnButtonApplyClick(wxCommandEvent& event)
{
}


void DialogArucoColor::OnSpinCtrlMarkerColsChange(wxSpinEvent& event)
{
    for (auto f : plugin)
	f->SetMarkerCols(event.GetValue());
}

void DialogArucoColor::OnSpinCtrlMarkerRowsChange(wxSpinEvent& event)
{
    for (auto f : plugin)
	f->SetMarkerRows(event.GetValue());
}

void DialogArucoColor::OnSpinCtrlAtBlockSizeChange(wxSpinEvent& event)
{

    for (auto f : plugin)
	f->SetAdaptiveThresholdBlockSize(event.GetValue());
}

void DialogArucoColor::OnSpinCtrlAtConstantChange(wxSpinEvent& event)
{
    for (auto f : plugin)
	f->SetAdaptiveThresholdConstant(event.GetValue());
}

void DialogArucoColor::OnSpinCtrlSaturationThresholdChange(wxSpinEvent& event)
{
    for (auto f : plugin)
	f->SetSaturationThreshold(event.GetValue());
}

void DialogArucoColor::OnSpinCtrlValueThresholdChange(wxSpinEvent& event)
{
    for (auto f : plugin)
	f->SetValueThreshold(event.GetValue());
}

void DialogArucoColor::OnSpinCtrlMinAreaChange(wxSpinEvent& event)
{
    for (auto f : plugin)
	f->SetMinMarkerArea(event.GetValue());
}

void DialogArucoColor::OnSpinCtrlMaxAreaChange(wxSpinEvent& event)
{
    for (auto f : plugin)
	f->SetMaxMarkerArea(event.GetValue());
}

void DialogArucoColor::OnSpinCtrlMaxHueDeviationChange(wxSpinEvent& event)
{
    for (auto f : plugin)
	f->SetMaxHueDeviation(event.GetValue());
}

void DialogArucoColor::OnSpinCtrlMaxMarkerRangeChange(wxSpinEvent& event)
{
    for (auto f : plugin)
	f->SetMaxMarkerRange(event.GetValue());
}

void DialogArucoColor::OnButtonUpdateRefHuesClick(wxCommandEvent& event)
{
    // build string
    wxString str;
    for (int i = 0; i < TextCtrlRefHues->GetNumberOfLines(); i++)
    {
	str += TextCtrlRefHues->GetLineText(i);
    }

    std::cout << "setting refhues " << str << std::endl;

    for (auto f : plugin)
	f->SetReferenceHuesString(str.ToStdString());
}

void DialogArucoColor::OnButtonUpdateDictionaryClick(wxCommandEvent& event)
{
    // build string
    wxString str;
    for (int i = 0; i < TextCtrlDictionary->GetNumberOfLines(); i++)
    {
	str += TextCtrlDictionary->GetLineText(i);
    }

    for (auto f : plugin)

	f->SetDictionaryString(str.ToStdString());
}

void DialogArucoColor::OnCheckBoxOutputClick(wxCommandEvent& event)
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

void DialogArucoColor::OnFilePickerCtrl1FileChanged(wxFileDirPickerEvent& event)
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
