#ifdef ARUCO

#include "DialogAruco.h"

//(*InternalHeaders(DialogAruco)
#include <wx/string.h>
#include <wx/intl.h>
//*)

//(*IdInit(DialogAruco)
const long DialogAruco::ID_RADIOBOX1 = wxNewId();
const long DialogAruco::ID_STATICTEXT1 = wxNewId();
const long DialogAruco::ID_SPINCTRL1 = wxNewId();
const long DialogAruco::ID_STATICTEXT2 = wxNewId();
const long DialogAruco::ID_SPINCTRL2 = wxNewId();
const long DialogAruco::ID_STATICTEXT3 = wxNewId();
const long DialogAruco::ID_SPINCTRL3 = wxNewId();
const long DialogAruco::ID_STATICTEXT4 = wxNewId();
const long DialogAruco::ID_SPINCTRL4 = wxNewId();
const long DialogAruco::ID_CHECKBOX1 = wxNewId();
const long DialogAruco::ID_FILEPICKERCTRL1 = wxNewId();
const long DialogAruco::ID_BUTTON1 = wxNewId();
//*)

//const long DialogAruco::ID_SPINCTRL1x = wxNewId();
//const long DialogAruco::ID_SPINCTRL2x = wxNewId();
const long DialogAruco::ID_SPINCTRL3x = wxNewId();
const long DialogAruco::ID_SPINCTRL4x = wxNewId();


BEGIN_EVENT_TABLE(DialogAruco,wxDialog)
	//(*EventTable(DialogAruco)
	//*)
END_EVENT_TABLE()

DialogAruco::DialogAruco(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
	//(*Initialize(DialogAruco)
	wxStaticBoxSizer* StaticBoxSizer2;
	wxFlexGridSizer* FlexGridSizer1;
	wxFlexGridSizer* FlexGridSizer2;
	wxFlexGridSizer* FlexGridSizer4;
	wxStaticBoxSizer* StaticBoxSizer3;
	wxFlexGridSizer* FlexGridSizer3;
	wxFlexGridSizer* FlexGridSizer5;
	wxStaticBoxSizer* StaticBoxSizer1;

	Create(parent, wxID_ANY, _("Aruco markers detector"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, _T("wxID_ANY"));
	FlexGridSizer1 = new wxFlexGridSizer(0, 1, 0, 0);
	FlexGridSizer1->AddGrowableCol(0);
	wxString __wxRadioBoxChoices_1[3] =
	{
		_("Fixed threshold"),
		_("Adaptive threshold"),
		_("Canny edge detector")
	};
	RadioBoxMethod = new wxRadioBox(this, ID_RADIOBOX1, _("Thresholding method"), wxDefaultPosition, wxDefaultSize, 3, __wxRadioBoxChoices_1, 3, 0, wxDefaultValidator, _T("ID_RADIOBOX1"));
	RadioBoxMethod->SetSelection(1);
	FlexGridSizer1->Add(RadioBoxMethod, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticBoxSizer1 = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Thresholds"));
	FlexGridSizer2 = new wxFlexGridSizer(0, 2, 0, 0);
	StaticText1 = new wxStaticText(this, ID_STATICTEXT1, _("Threshold 1"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
	FlexGridSizer2->Add(StaticText1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	SpinCtrlThreshold1 = new wxSpinCtrl(this, ID_SPINCTRL1, _T("19"), wxDefaultPosition, wxDefaultSize, 0, 0, 255, 19, _T("ID_SPINCTRL1"));
	SpinCtrlThreshold1->SetValue(_T("19"));
	FlexGridSizer2->Add(SpinCtrlThreshold1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticText2 = new wxStaticText(this, ID_STATICTEXT2, _("Threshold2"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT2"));
	FlexGridSizer2->Add(StaticText2, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	SpinCtrlThreshold2 = new wxSpinCtrl(this, ID_SPINCTRL2, _T("7"), wxDefaultPosition, wxDefaultSize, 0, 0, 255, 7, _T("ID_SPINCTRL2"));
	SpinCtrlThreshold2->SetValue(_T("7"));
	FlexGridSizer2->Add(SpinCtrlThreshold2, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticBoxSizer1->Add(FlexGridSizer2, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer1->Add(StaticBoxSizer1, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticBoxSizer2 = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Marker size"));
	FlexGridSizer3 = new wxFlexGridSizer(0, 2, 0, 0);
	StaticText3 = new wxStaticText(this, ID_STATICTEXT3, _("Min size (pixels)"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT3"));
	FlexGridSizer3->Add(StaticText3, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	SpinCtrlMinSize = new wxSpinCtrl(this, ID_SPINCTRL3, _T("20"), wxDefaultPosition, wxDefaultSize, 0, 5, 10000, 20, _T("ID_SPINCTRL3"));
	SpinCtrlMinSize->SetValue(_T("20"));
	FlexGridSizer3->Add(SpinCtrlMinSize, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticText4 = new wxStaticText(this, ID_STATICTEXT4, _("Max size (pixels)"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT4"));
	FlexGridSizer3->Add(StaticText4, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	SpinCtrlMaxSize = new wxSpinCtrl(this, ID_SPINCTRL4, _T("1000"), wxDefaultPosition, wxDefaultSize, 0, 5, 10000, 1000, _T("ID_SPINCTRL4"));
	SpinCtrlMaxSize->SetValue(_T("1000"));
	FlexGridSizer3->Add(SpinCtrlMaxSize, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticBoxSizer2->Add(FlexGridSizer3, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer1->Add(StaticBoxSizer2, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticBoxSizer3 = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Output to file"));
	FlexGridSizer5 = new wxFlexGridSizer(0, 2, 0, 0);
	FlexGridSizer5->AddGrowableCol(1);
	CheckBoxOutput = new wxCheckBox(this, ID_CHECKBOX1, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX1"));
	CheckBoxOutput->SetValue(false);
	FlexGridSizer5->Add(CheckBoxOutput, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FilePickerCtrl1 = new wxFilePickerCtrl(this, ID_FILEPICKERCTRL1, wxEmptyString, wxEmptyString, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxFLP_OVERWRITE_PROMPT|wxFLP_SAVE|wxFLP_USE_TEXTCTRL, wxDefaultValidator, _T("ID_FILEPICKERCTRL1"));
	FlexGridSizer5->Add(FilePickerCtrl1, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticBoxSizer3->Add(FlexGridSizer5, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer1->Add(StaticBoxSizer3, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer4 = new wxFlexGridSizer(0, 3, 0, 0);
	ButtonOk = new wxButton(this, ID_BUTTON1, _("Ok"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
	ButtonOk->SetFocus();
	FlexGridSizer4->Add(ButtonOk, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer1->Add(FlexGridSizer4, 1, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5);
	SetSizer(FlexGridSizer1);
	FlexGridSizer1->Fit(this);
	FlexGridSizer1->SetSizeHints(this);

	Connect(ID_RADIOBOX1,wxEVT_COMMAND_RADIOBOX_SELECTED,(wxObjectEventFunction)&DialogAruco::OnRadioBoxMethodSelect);
	Connect(ID_SPINCTRL1,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&DialogAruco::OnSpinCtrlThreshold1Change);
	Connect(ID_SPINCTRL2,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&DialogAruco::OnSpinCtrlThreshold2Change);
	Connect(ID_SPINCTRL3,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&DialogAruco::OnSpinCtrlMinSizeChange);
	Connect(ID_SPINCTRL4,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&DialogAruco::OnSpinCtrlMaxSizeChange);
	Connect(ID_CHECKBOX1,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&DialogAruco::OnCheckBoxOutputClick);
	Connect(ID_FILEPICKERCTRL1,wxEVT_COMMAND_FILEPICKER_CHANGED,(wxObjectEventFunction)&DialogAruco::OnFilePickerCtrl1FileChanged);
	Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&DialogAruco::OnButtonOkClick);
	//*)

	// codeblocks 13.12 / wxsmith does not support spinctrldouble yet... adding manually
	// SpinCtrlDoubleThreshold1 = new wxSpinCtrlDouble(this, ID_SPINCTRL1x, _T("0"), wxDefaultPosition, wxDefaultSize, 0, 0, 1, 0, 0.01, _T("ID_SPINCTRL1x"));
	// FlexGridSizer2->Replace(SpinCtrlThreshold1, SpinCtrlDoubleThreshold1);
	// SpinCtrlThreshold1->Bind(wxEVT_SPINCTRLDOUBLE, &DialogAruco::OnSpinCtrlThreshold1Change, this);

	// SpinCtrlDoubleThreshold2 = new wxSpinCtrlDouble(this, ID_SPINCTRL2x, _T("0"), wxDefaultPosition, wxDefaultSize, 0, 0, 1, 0, 0.01, _T("ID_SPINCTRL2x"));
	// FlexGridSizer2->Replace(SpinCtrlThreshold2, SpinCtrlDoubleThreshold2);
	// SpinCtrlThreshold2->Bind(wxEVT_SPINCTRLDOUBLE, &DialogAruco::OnSpinCtrlThreshold2Change, this);

	SpinCtrlDoubleMinSize = new wxSpinCtrlDouble(this, ID_SPINCTRL3x, _T("0"), wxDefaultPosition, wxDefaultSize, 0, 0, 1, 0, 0.0001, _T("ID_SPINCTRL3x"));
	FlexGridSizer3->Replace(SpinCtrlMinSize, SpinCtrlDoubleMinSize);
	SpinCtrlMinSize->Bind(wxEVT_SPINCTRLDOUBLE, &DialogAruco::OnSpinCtrlMinSizeChange, this);

	SpinCtrlDoubleMaxSize = new wxSpinCtrlDouble(this, ID_SPINCTRL4x, _T("0"), wxDefaultPosition, wxDefaultSize, 0, 0, 1, 0, 0.0001, _T("ID_SPINCTRL4x"));
	FlexGridSizer3->Replace(SpinCtrlMaxSize, SpinCtrlDoubleMaxSize);
	SpinCtrlMaxSize->Bind(wxEVT_SPINCTRLDOUBLE, &DialogAruco::OnSpinCtrlMaxSizeChange, this);

}

DialogAruco::~DialogAruco()
{
	//(*Destroy(DialogAruco)
	//*)
}

void DialogAruco::SetPlugin (vector<PipelinePlugin*> pfv)
{
    for (auto func : pfv)
    	plugin.push_back(dynamic_cast<Aruco*> (func));

    SpinCtrlDoubleMinSize->SetValue(plugin[0]->minSize);
    SpinCtrlDoubleMaxSize->SetValue(plugin[0]->maxSize);

    SpinCtrlThreshold1->SetValue(plugin[0]->thresh1);
    SpinCtrlThreshold2->SetValue(plugin[0]->thresh2);

    RadioBoxMethod->SetSelection(plugin[0]->thresholdMethod);

    FilePickerCtrl1->SetPath(plugin[0]->outputFilename);
    CheckBoxOutput->SetValue(plugin[0]->output);

}


void DialogAruco::OnSpinCtrlThreshold1Change(wxSpinEvent& event)
{
    for (auto f : plugin)
	f->SetThreshold1(event.GetValue());
}

void DialogAruco::OnSpinCtrlMinSizeChange(wxSpinDoubleEvent& event)
{
    for (auto f : plugin)
	f->SetMinSize(event.GetValue());
}

void DialogAruco::OnSpinCtrlMaxSizeChange(wxSpinDoubleEvent& event)
{
    for (auto f : plugin)
	f->SetMaxSize(event.GetValue());
}

void DialogAruco::OnButtonOkClick(wxCommandEvent& event)
{
    this->Hide();
}

void DialogAruco::OnSpinCtrlThreshold2Change(wxSpinEvent& event)
{
    for (auto f : plugin)
	f->SetThreshold2(event.GetValue());
}

void DialogAruco::OnRadioBoxMethodSelect(wxCommandEvent& event)
{
    for (auto f : plugin)
	f->SetThresholdMethod(RadioBoxMethod->GetSelection());
}

#endif

void DialogAruco::OnCheckBoxOutputClick(wxCommandEvent& event)
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

void DialogAruco::OnFilePickerCtrl1FileChanged(wxFileDirPickerEvent& event)
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
