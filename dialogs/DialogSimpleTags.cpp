#include "DialogSimpleTags.h"

//(*InternalHeaders(DialogSimpleTags)
#include <wx/string.h>
#include <wx/intl.h>
//*)

//(*IdInit(DialogSimpleTags)
const long DialogSimpleTags::ID_STATICTEXT1 = wxNewId();
const long DialogSimpleTags::ID_SPINCTRL1 = wxNewId();
const long DialogSimpleTags::ID_STATICTEXT2 = wxNewId();
const long DialogSimpleTags::ID_SPINCTRL2 = wxNewId();
const long DialogSimpleTags::ID_CHECKBOX1 = wxNewId();
const long DialogSimpleTags::ID_FILEPICKERCTRL1 = wxNewId();
const long DialogSimpleTags::ID_BUTTON1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(DialogSimpleTags,wxDialog)
	//(*EventTable(DialogSimpleTags)
	//*)
END_EVENT_TABLE()

DialogSimpleTags::DialogSimpleTags(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
	//(*Initialize(DialogSimpleTags)
	wxStaticBoxSizer* StaticBoxSizer2;
	wxFlexGridSizer* FlexGridSizer1;
	wxFlexGridSizer* FlexGridSizer2;
	wxFlexGridSizer* FlexGridSizer4;
	wxFlexGridSizer* FlexGridSizer3;
	wxStaticBoxSizer* StaticBoxSizer1;

	Create(parent, wxID_ANY, _("Simple tags"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, _T("wxID_ANY"));
	FlexGridSizer1 = new wxFlexGridSizer(0, 1, 0, 0);
	StaticBoxSizer1 = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Simple tags"));
	FlexGridSizer4 = new wxFlexGridSizer(0, 2, 0, 0);
	FlexGridSizer4->AddGrowableCol(1);
	StaticText1 = new wxStaticText(this, ID_STATICTEXT1, _("Tag width (pixels)"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
	FlexGridSizer4->Add(StaticText1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	SpinCtrlTagWidth = new wxSpinCtrl(this, ID_SPINCTRL1, _T("100"), wxDefaultPosition, wxDefaultSize, 0, 1, 10000, 100, _T("ID_SPINCTRL1"));
	SpinCtrlTagWidth->SetValue(_T("100"));
	FlexGridSizer4->Add(SpinCtrlTagWidth, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticText2 = new wxStaticText(this, ID_STATICTEXT2, _("Tag height (pixels)"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT2"));
	FlexGridSizer4->Add(StaticText2, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	SpinCtrlTagHeight = new wxSpinCtrl(this, ID_SPINCTRL2, _T("20"), wxDefaultPosition, wxDefaultSize, 0, 1, 10000, 20, _T("ID_SPINCTRL2"));
	SpinCtrlTagHeight->SetValue(_T("20"));
	FlexGridSizer4->Add(SpinCtrlTagHeight, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticBoxSizer1->Add(FlexGridSizer4, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer1->Add(StaticBoxSizer1, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticBoxSizer2 = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Output to file"));
	FlexGridSizer3 = new wxFlexGridSizer(0, 2, 0, 0);
	FlexGridSizer3->AddGrowableCol(1);
	CheckBoxOutput = new wxCheckBox(this, ID_CHECKBOX1, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX1"));
	CheckBoxOutput->SetValue(false);
	FlexGridSizer3->Add(CheckBoxOutput, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FilePickerCtrl1 = new wxFilePickerCtrl(this, ID_FILEPICKERCTRL1, wxEmptyString, wxEmptyString, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxFLP_OVERWRITE_PROMPT|wxFLP_SAVE|wxFLP_USE_TEXTCTRL, wxDefaultValidator, _T("ID_FILEPICKERCTRL1"));
	FlexGridSizer3->Add(FilePickerCtrl1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticBoxSizer2->Add(FlexGridSizer3, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer1->Add(StaticBoxSizer2, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer2 = new wxFlexGridSizer(0, 3, 0, 0);
	ButtonOk = new wxButton(this, ID_BUTTON1, _("Ok"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
	FlexGridSizer2->Add(ButtonOk, 1, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer1->Add(FlexGridSizer2, 1, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5);
	SetSizer(FlexGridSizer1);
	FlexGridSizer1->Fit(this);
	FlexGridSizer1->SetSizeHints(this);

	Connect(ID_SPINCTRL1,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&DialogSimpleTags::OnSpinCtrlTagWidthChange);
	Connect(ID_SPINCTRL2,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&DialogSimpleTags::OnSpinCtrlTagHeightChange);
	Connect(ID_CHECKBOX1,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&DialogSimpleTags::OnCheckBoxOutputClick);
	Connect(ID_FILEPICKERCTRL1,wxEVT_COMMAND_FILEPICKER_CHANGED,(wxObjectEventFunction)&DialogSimpleTags::OnFilePickerCtrl1FileChanged);
	Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&DialogSimpleTags::OnButtonOkClick);
	//*)
}

DialogSimpleTags::~DialogSimpleTags()
{
	//(*Destroy(DialogSimpleTags)
	//*)
}

void DialogSimpleTags::SetPlugin (std::vector<PipelinePlugin*> pfv)
{
    for (auto func : pfv)
	plugin.push_back(dynamic_cast<SimpleTags*> (func));
    // SpinCtrlSimpleTagsSize->SetValue(plugin[0]->minSize);
    // SpinCtrlMaxSize->SetValue(plugin[0]->maxSize);
    FilePickerCtrl1->SetPath(plugin[0]->outputFilename);
    CheckBoxOutput->SetValue(plugin[0]->output);
}


void DialogSimpleTags::OnButtonOkClick(wxCommandEvent& event)
{
    this->Hide();
}

void DialogSimpleTags::OnCheckBoxOutputClick(wxCommandEvent& event)
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

void DialogSimpleTags::OnFilePickerCtrl1FileChanged(wxFileDirPickerEvent& event)
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

void DialogSimpleTags::OnSpinCtrlTagWidthChange(wxSpinEvent& event)
{
    for (auto f : plugin)
	f->SetTagDimensions(SpinCtrlTagWidth->GetValue(), SpinCtrlTagHeight->GetValue());
}

void DialogSimpleTags::OnSpinCtrlTagHeightChange(wxSpinEvent& event)
{
    for (auto f : plugin)
	f->SetTagDimensions(SpinCtrlTagWidth->GetValue(), SpinCtrlTagHeight->GetValue());
}
