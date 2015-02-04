#include "DialogTracker.h"

//(*InternalHeaders(DialogTracker)
#include <wx/string.h>
#include <wx/intl.h>
//*)

//(*IdInit(DialogTracker)
const long DialogTracker::ID_CHECKBOX2 = wxNewId();
const long DialogTracker::ID_STATICTEXT1A = wxNewId();
const long DialogTracker::ID_CHECKBOX3 = wxNewId();
const long DialogTracker::ID_CHECKBOX1 = wxNewId();
const long DialogTracker::ID_FILEPICKERCTRL1 = wxNewId();
const long DialogTracker::ID_CHECKBOX4 = wxNewId();
const long DialogTracker::ID_FILEPICKERCTRL2 = wxNewId();
const long DialogTracker::ID_BUTTON2 = wxNewId();
//*)

const long DialogTracker::ID_STATICTEXT1 = wxNewId();
const long DialogTracker::ID_SPINCTRL1 = wxNewId();
const long DialogTracker::ID_STATICTEXT2 = wxNewId();
const long DialogTracker::ID_SPINCTRL2 = wxNewId();
const long DialogTracker::ID_STATICTEXT3 = wxNewId();
const long DialogTracker::ID_SPINCTRL3 = wxNewId();
const long DialogTracker::ID_STATICTEXT4 = wxNewId();
const long DialogTracker::ID_SPINCTRL4 = wxNewId();
const long DialogTracker::ID_STATICTEXT5 = wxNewId();
const long DialogTracker::ID_SPINCTRL5 = wxNewId();
const long DialogTracker::ID_STATICTEXT6 = wxNewId();
const long DialogTracker::ID_SPINCTRL6 = wxNewId();
const long DialogTracker::ID_STATICTEXT10 = wxNewId();
const long DialogTracker::ID_SPINCTRL8 = wxNewId();
const long DialogTracker::ID_STATICTEXT11 = wxNewId();
const long DialogTracker::ID_SPINCTRL9 = wxNewId();
const long DialogTracker::ID_STATICTEXT12 = wxNewId();
const long DialogTracker::ID_SPINCTRL10 = wxNewId();


BEGIN_EVENT_TABLE(DialogTracker,wxDialog)
	//(*EventTable(DialogTracker)
	//*)
END_EVENT_TABLE()

DialogTracker::DialogTracker(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
	//(*Initialize(DialogTracker)
	wxStaticBoxSizer* StaticBoxSizer2;
	wxFlexGridSizer* FlexGridSizer1;
	wxFlexGridSizer* FlexGridSizer2;
	wxFlexGridSizer* FlexGridSizer4;
	wxStaticBoxSizer* StaticBoxSizer3;
	wxFlexGridSizer* FlexGridSizer6;
	wxFlexGridSizer* FlexGridSizer3;
	wxStaticBoxSizer* StaticBoxSizer4;
	wxFlexGridSizer* FlexGridSizer5;
	wxStaticBoxSizer* StaticBoxSizer1;

	Create(parent, wxID_ANY, _("Track blobs"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, _T("wxID_ANY"));
	SetMinSize(wxSize(-1,-1));
	SetMaxSize(wxSize(-1,-1));
	FlexGridSizer1 = new wxFlexGridSizer(0, 1, 0, 0);
	StaticBoxSizer1 = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Parameters"));
	FlexGridSizer4 = new wxFlexGridSizer(0, 2, 0, 0);
	FlexGridSizer4->AddGrowableCol(1);
	StaticBoxSizer1->Add(FlexGridSizer4, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer1->Add(StaticBoxSizer1, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	CheckBoxUseVE = new wxCheckBox(this, ID_CHECKBOX2, _("Use virtual entities"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX2"));
	CheckBoxUseVE->SetValue(false);
	FlexGridSizer1->Add(CheckBoxUseVE, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticBoxSizer3 = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Virtual entities"));
	FlexGridSizer5 = new wxFlexGridSizer(0, 2, 0, 0);
	FlexGridSizer5->AddGrowableCol(1);
	StaticText1a = new wxStaticText(this, ID_STATICTEXT1A, _("Create when out of visible zone"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1A"));
	FlexGridSizer5->Add(StaticText1a, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	CheckBoxVEZone = new wxCheckBox(this, ID_CHECKBOX3, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX3"));
	CheckBoxVEZone->SetValue(false);
	FlexGridSizer5->Add(CheckBoxVEZone, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	StaticBoxSizer3->Add(FlexGridSizer5, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer1->Add(StaticBoxSizer3, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticBoxSizer2 = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Output to file"));
	FlexGridSizer3 = new wxFlexGridSizer(0, 2, 0, 0);
	FlexGridSizer3->AddGrowableCol(1);
	CheckBoxOutput = new wxCheckBox(this, ID_CHECKBOX1, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX1"));
	CheckBoxOutput->SetValue(false);
	FlexGridSizer3->Add(CheckBoxOutput, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FilePickerCtrl1 = new wxFilePickerCtrl(this, ID_FILEPICKERCTRL1, wxEmptyString, wxEmptyString, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxFLP_CHANGE_DIR|wxFLP_OVERWRITE_PROMPT|wxFLP_SAVE|wxFLP_USE_TEXTCTRL, wxDefaultValidator, _T("ID_FILEPICKERCTRL1"));
	FlexGridSizer3->Add(FilePickerCtrl1, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticBoxSizer2->Add(FlexGridSizer3, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer1->Add(StaticBoxSizer2, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticBoxSizer4 = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Replay data"));
	FlexGridSizer6 = new wxFlexGridSizer(0, 2, 0, 0);
	FlexGridSizer6->AddGrowableCol(1);
	CheckBoxReplay = new wxCheckBox(this, ID_CHECKBOX4, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX4"));
	CheckBoxReplay->SetValue(false);
	FlexGridSizer6->Add(CheckBoxReplay, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FilePickerCtrl2 = new wxFilePickerCtrl(this, ID_FILEPICKERCTRL2, wxEmptyString, wxEmptyString, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxFLP_FILE_MUST_EXIST|wxFLP_OPEN|wxFLP_USE_TEXTCTRL, wxDefaultValidator, _T("ID_FILEPICKERCTRL2"));
	FlexGridSizer6->Add(FilePickerCtrl2, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticBoxSizer4->Add(FlexGridSizer6, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer1->Add(StaticBoxSizer4, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer2 = new wxFlexGridSizer(1, 3, 0, 0);
	buttonOk = new wxButton(this, ID_BUTTON2, _("Ok"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON2"));
	buttonOk->SetFocus();
	FlexGridSizer2->Add(buttonOk, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer1->Add(FlexGridSizer2, 1, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5);
	SetSizer(FlexGridSizer1);
	FlexGridSizer1->Fit(this);
	FlexGridSizer1->SetSizeHints(this);

	Connect(ID_CHECKBOX2,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&DialogTracker::OnCheckBoxUseVEClick);
	Connect(ID_CHECKBOX1,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&DialogTracker::OnCheckBoxOutputClick1);
	Connect(ID_CHECKBOX4,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&DialogTracker::OnCheckBoxReplayClick);
	Connect(ID_FILEPICKERCTRL2,wxEVT_COMMAND_FILEPICKER_CHANGED,(wxObjectEventFunction)&DialogTracker::OnFilePickerCtrl2FileChanged);
	Connect(ID_BUTTON2,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&DialogTracker::OnButtonOkClick);
	//*)

	// contents of subpanels manually tuned because of spinctrldouble...

	StaticText1 = new wxStaticText(this, ID_STATICTEXT1, _("Number of entities"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
	FlexGridSizer4->Add(StaticText1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	SpinCtrlEntitiesCount = new wxSpinCtrl(this, ID_SPINCTRL1, _T("0"), wxDefaultPosition, wxDefaultSize, 0, 1, 100000, 1, _T("ID_SPINCTRL1"));
	SpinCtrlEntitiesCount->SetValue(_T("1"));
	FlexGridSizer4->Add(SpinCtrlEntitiesCount, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	StaticText2 = new wxStaticText(this, ID_STATICTEXT2, _("Min interdistance"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT2"));
	FlexGridSizer4->Add(StaticText2, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	SpinCtrlMinInterdistance = new wxSpinCtrlDouble(this, ID_SPINCTRL2, _T("0"), wxDefaultPosition, wxDefaultSize, 0, 0, 100000000, 0, 0.1, _T("ID_SPINCTRL2"));
	SpinCtrlMinInterdistance->SetValue(_T("0"));
	FlexGridSizer4->Add(SpinCtrlMinInterdistance, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	StaticText3 = new wxStaticText(this, ID_STATICTEXT3, _("Max motion (pixel/sec)"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT3"));
	FlexGridSizer4->Add(StaticText3, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	SpinCtrlMaxMotion = new wxSpinCtrlDouble(this, ID_SPINCTRL3, _T("0"), wxDefaultPosition, wxDefaultSize, 0, 0, 1000000000, 0, 0.1, _T("ID_SPINCTRL3"));
	SpinCtrlMaxMotion->SetValue(_T("0"));
	FlexGridSizer4->Add(SpinCtrlMaxMotion, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	StaticText4 = new wxStaticText(this, ID_STATICTEXT4, _("Extrapolation decay"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT4"));
	FlexGridSizer4->Add(StaticText4, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	SpinCtrlExtrapolationDecay = new wxSpinCtrlDouble(this, ID_SPINCTRL4, _T("0"), wxDefaultPosition, wxDefaultSize, 0, 0, 1, 0, 0.01, _T("ID_SPINCTRL4"));
	SpinCtrlExtrapolationDecay->SetValue(_T("0"));
	FlexGridSizer4->Add(SpinCtrlExtrapolationDecay, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	StaticText5 = new wxStaticText(this, ID_STATICTEXT5, _("Extrapolation history size"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT5"));
	FlexGridSizer4->Add(StaticText5, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	SpinCtrlExtrapolationHistorySize = new wxSpinCtrl(this, ID_SPINCTRL5, _T("0"), wxDefaultPosition, wxDefaultSize, 0, 0, 1000, 0, _T("ID_SPINCTRL5"));
	SpinCtrlExtrapolationHistorySize->SetValue(_T("0"));
	FlexGridSizer4->Add(SpinCtrlExtrapolationHistorySize, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	StaticText6 = new wxStaticText(this, ID_STATICTEXT6, _("Extrapolation timeout"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT6"));
	FlexGridSizer4->Add(StaticText6, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	SpinCtrlExtrapolationTimeout = new wxSpinCtrlDouble(this, ID_SPINCTRL6, _T("0"), wxDefaultPosition, wxDefaultSize, 0, 0, 1000, 0, 0.01, _T("ID_SPINCTRL6"));
	SpinCtrlExtrapolationTimeout->SetValue(_T("0"));
	FlexGridSizer4->Add(SpinCtrlExtrapolationTimeout, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);

	StaticText10 = new wxStaticText(this, ID_STATICTEXT10, _("Create when last position dist >"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT10"));
	FlexGridSizer5->Add(StaticText10, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	SpinCtrlVEDistance = new wxSpinCtrlDouble(this, ID_SPINCTRL8, _T("0"), wxDefaultPosition, wxDefaultSize, 0, 0, 100000, 0, 0.1, _T("ID_SPINCTRL8"));
	SpinCtrlVEDistance->SetValue(_T("0"));
	FlexGridSizer5->Add(SpinCtrlVEDistance, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	StaticText11 = new wxStaticText(this, ID_STATICTEXT11, _("Time to promotion"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT11"));
	FlexGridSizer5->Add(StaticText11, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	SpinCtrlVEDelay = new wxSpinCtrlDouble(this, ID_SPINCTRL9, _T("0"), wxDefaultPosition, wxDefaultSize, 0, 0, 1000, 0, 0.01, _T("ID_SPINCTRL9"));
	SpinCtrlVEDelay->SetValue(_T("0"));
	FlexGridSizer5->Add(SpinCtrlVEDelay, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	StaticText12 = new wxStaticText(this, ID_STATICTEXT12, _("Max lifetime"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT12"));
	FlexGridSizer5->Add(StaticText12, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	SpinCtrlVELifetime = new wxSpinCtrlDouble(this, ID_SPINCTRL10, _T("0"), wxDefaultPosition, wxDefaultSize, 0, 0, 1000, 0, 0.01, _T("ID_SPINCTRL10"));
	SpinCtrlVELifetime->SetValue(_T("0"));
	FlexGridSizer5->Add(SpinCtrlVELifetime, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);

	FlexGridSizer1->Layout();
	this->Fit();
	// FlexGridSizer1->SetSizeHints(this);

	StaticBoxSizerVE = StaticBoxSizer3;
	FlexGridSizerMain = FlexGridSizer1;

	Connect(ID_SPINCTRL1,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&DialogTracker::OnSpinCtrlEntitiesCountChange);
	Connect(ID_SPINCTRL2,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&DialogTracker::OnSpinCtrlMinInterdistanceChange);
	Connect(ID_SPINCTRL3,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&DialogTracker::OnSpinCtrlMaxMotionChange);
	Connect(ID_SPINCTRL4,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&DialogTracker::OnSpinCtrlExtrapolationDecayChange);
	Connect(ID_SPINCTRL5,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&DialogTracker::OnSpinCtrlExtrapolationHistorySizeChange);
	Connect(ID_SPINCTRL6,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&DialogTracker::OnSpinCtrlExtrapolationTimeoutChange);
	Connect(ID_SPINCTRL8,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&DialogTracker::OnSpinCtrlVEDistanceChange);
	Connect(ID_SPINCTRL9,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&DialogTracker::OnSpinCtrlVEDelayChange);
	Connect(ID_SPINCTRL10,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&DialogTracker::OnSpinCtrlVELifetimeChange);

}

DialogTracker::~DialogTracker()
{
	//(*Destroy(DialogTracker)
	//*)
}


void DialogTracker::SetPlugin (vector<PipelinePlugin*> pfv)
{
    for (auto func : pfv)
    	plugin.push_back(dynamic_cast<Tracker*> (func));
    SpinCtrlMinInterdistance->SetValue(plugin[0]->minInterdistance);
    SpinCtrlMaxMotion->SetValue(plugin[0]->maxMotionPerSecond);
    SpinCtrlExtrapolationDecay->SetValue(plugin[0]->extrapolationDecay);
    SpinCtrlExtrapolationHistorySize->SetValue(plugin[0]->motionEstimatorLength);
    SpinCtrlExtrapolationTimeout->SetValue(plugin[0]->motionEstimatorTimeout);
    SpinCtrlEntitiesCount->SetValue(plugin[0]->entitiesCount);

    CheckBoxUseVE->SetValue(plugin[0]->useVirtualEntities);
    CheckBoxVEZone->SetValue(plugin[0]->virtualEntitiesZone);
    SpinCtrlVEDelay->SetValue(plugin[0]->virtualEntitiesDelay);
    SpinCtrlVEDistance->SetValue(sqrt(plugin[0]->virtualEntitiesDistsq));
    SpinCtrlVELifetime->SetValue(plugin[0]->virtualEntitiesLifetime);

    // show/hide VE sizer
    StaticBoxSizerVE->Show(plugin[0]->useVirtualEntities);
    FlexGridSizerMain->Layout();
    this->Fit();

    CheckBoxOutput->SetValue(plugin[0]->output);
    FilePickerCtrl1->SetPath(plugin[0]->outputFilename);
}

void DialogTracker::OnButtonCancelClick(wxCommandEvent& event)
{
    this->Hide();
}


void DialogTracker::OnButtonOkClick(wxCommandEvent& event)
{
//    OnButtonApplyClick(event);
    this->Hide();
}

void DialogTracker::OnFilePickerCtrl1FileChanged(wxFileDirPickerEvent& event)
{
    string path (FilePickerCtrl1->GetPath().ToStdString());
    if (path != plugin[0]->outputFilename)
    {
	for (auto f : plugin)
	{
	    f->CloseOutput();
	    f->outputFilename = path;
	    f->output = false;
	}
	CheckBoxOutput->SetValue(false);
    }
}

void DialogTracker::OnCheckBoxOutputClick1(wxCommandEvent& event)
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

void DialogTracker::OnSpinCtrlExtrapolationTimeoutChange(wxSpinEvent& event)
{
    for (auto f : plugin)
	f->motionEstimatorTimeout = event.GetPosition();
}

void DialogTracker::OnSpinCtrlExtrapolationHistorySizeChange(wxSpinEvent& event)
{
    for (auto f : plugin)
	f->motionEstimatorLength = event.GetPosition();
}

void DialogTracker::OnSpinCtrlExtrapolationDecayChange(wxSpinEvent& event)
{
    for (auto f : plugin)
	f->extrapolationDecay = event.GetPosition();
}

void DialogTracker::OnSpinCtrlMaxMotionChange(wxSpinEvent& event)
{
    for (auto f : plugin)
	f->maxMotionPerSecond = event.GetPosition();
}

void DialogTracker::OnSpinCtrlMinInterdistanceChange(wxSpinEvent& event)
{
    for (auto f : plugin)
	f->minInterdistance = event.GetPosition();
}

void DialogTracker::OnSpinCtrlEntitiesCountChange(wxSpinEvent& event)
{
    for (auto f : plugin)
        f->SetMaxEntities(event.GetPosition());
}

void DialogTracker::OnSpinCtrlVELifetimeChange(wxSpinEvent& event)
{
    for (auto f : plugin)
	f->virtualEntitiesLifetime = event.GetPosition();
}

void DialogTracker::OnSpinCtrlVEDistanceChange(wxSpinEvent& event)
{
    for (auto f : plugin)
	f->virtualEntitiesDistsq = event.GetPosition() * event.GetPosition();
}

void DialogTracker::OnSpinCtrlVEDelayChange(wxSpinEvent& event)
{
    for (auto f : plugin)
	f->virtualEntitiesDelay = event.GetPosition();
}

void DialogTracker::OnCheckBoxReplayClick(wxCommandEvent& event)
{
    for (auto f : plugin)
    {
	f->SetReplay(event.IsChecked());
    }
}

void DialogTracker::OnFilePickerCtrl2FileChanged(wxFileDirPickerEvent& event)
{
    string path (FilePickerCtrl1->GetPath().ToStdString());

    for (auto f : plugin)
    {
//	f->LoadHistory(path);
//	f->SetReplay(true);
    }
    CheckBoxReplay->SetValue(true);
}

void DialogTracker::OnCheckBoxUseVEClick(wxCommandEvent& event)
{
    // show/hide VE sizer
    StaticBoxSizerVE->Show(event.IsChecked());
    FlexGridSizerMain->Layout();
    this->Fit();

    for (auto f : plugin)
	f->useVirtualEntities = event.IsChecked();
}
