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
const long DialogTracker::ID_BUTTON1 = wxNewId();
const long DialogTracker::ID_STATICTEXTTRAILENGTH = wxNewId();
const long DialogTracker::ID_SPINCTRLTRAILLENGTH = wxNewId();
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
	wxFlexGridSizer* FlexGridSizer7;
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
	FilePickerCtrl1 = new wxFilePickerCtrl(this, ID_FILEPICKERCTRL1, wxEmptyString, _("Choose an output file"), _T("*.csv"), wxDefaultPosition, wxDefaultSize, wxFLP_OVERWRITE_PROMPT|wxFLP_SAVE|wxFLP_USE_TEXTCTRL, wxDefaultValidator, _T("ID_FILEPICKERCTRL1"));
	FlexGridSizer3->Add(FilePickerCtrl1, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticBoxSizer2->Add(FlexGridSizer3, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer1->Add(StaticBoxSizer2, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticBoxSizer4 = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Replay data"));
	FlexGridSizer6 = new wxFlexGridSizer(0, 1, 0, 0);
	FlexGridSizer6->AddGrowableCol(0);
	CheckBoxReplay = new wxCheckBox(this, ID_CHECKBOX4, _("Replay data from history"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX4"));
	CheckBoxReplay->SetValue(false);
	FlexGridSizer6->Add(CheckBoxReplay, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	ButtonLoadHistory = new wxButton(this, ID_BUTTON1, _("Load history from file"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
	FlexGridSizer6->Add(ButtonLoadHistory, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer7 = new wxFlexGridSizer(0, 2, 0, 0);
	FlexGridSizer7->AddGrowableCol(1);
	StaticTextTrailLength = new wxStaticText(this, ID_STATICTEXTTRAILENGTH, _("History trail length (frames)"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXTTRAILENGTH"));
	FlexGridSizer7->Add(StaticTextTrailLength, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	SpinCtrlTrailLength = new wxSpinCtrl(this, ID_SPINCTRLTRAILLENGTH, _T("10"), wxDefaultPosition, wxDefaultSize, 0, 0, 500, 10, _T("ID_SPINCTRLTRAILLENGTH"));
	SpinCtrlTrailLength->SetValue(_T("10"));
	FlexGridSizer7->Add(SpinCtrlTrailLength, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer6->Add(FlexGridSizer7, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
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
	Connect(ID_FILEPICKERCTRL1,wxEVT_COMMAND_FILEPICKER_CHANGED,(wxObjectEventFunction)&DialogTracker::OnFilePickerCtrl1FileChanged);
	Connect(ID_CHECKBOX4,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&DialogTracker::OnCheckBoxReplayClick);
	Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&DialogTracker::OnButtonLoadHistoryClick);
	Connect(ID_SPINCTRLTRAILLENGTH,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&DialogTracker::OnSpinCtrlTrailLengthChange);
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

	SpinCtrlEntitiesCount->Bind(wxEVT_COMMAND_SPINCTRL_UPDATED, &DialogTracker::OnSpinCtrlEntitiesCountChange, this);
	SpinCtrlMinInterdistance->Bind(wxEVT_SPINCTRLDOUBLE, &DialogTracker::OnSpinCtrlMinInterdistanceChange, this);
	SpinCtrlMaxMotion->Bind(wxEVT_SPINCTRLDOUBLE, &DialogTracker::OnSpinCtrlMaxMotionChange, this);
	SpinCtrlExtrapolationDecay->Bind(wxEVT_SPINCTRLDOUBLE, &DialogTracker::OnSpinCtrlExtrapolationDecayChange, this);
	SpinCtrlExtrapolationHistorySize->Bind(wxEVT_COMMAND_SPINCTRL_UPDATED, &DialogTracker::OnSpinCtrlExtrapolationHistorySizeChange, this);
	SpinCtrlExtrapolationTimeout->Bind(wxEVT_SPINCTRLDOUBLE, &DialogTracker::OnSpinCtrlExtrapolationTimeoutChange, this);
	SpinCtrlVEDistance->Bind(wxEVT_SPINCTRLDOUBLE, &DialogTracker::OnSpinCtrlVEDistanceChange, this);
	SpinCtrlVEDelay->Bind(wxEVT_SPINCTRLDOUBLE, &DialogTracker::OnSpinCtrlVEDelayChange, this);
	SpinCtrlVELifetime->Bind(wxEVT_SPINCTRLDOUBLE, &DialogTracker::OnSpinCtrlVELifetimeChange, this);
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

void DialogTracker::OnSpinCtrlExtrapolationTimeoutChange(wxSpinDoubleEvent& event)
{
    for (auto f : plugin)
	f->motionEstimatorTimeout = event.GetValue();
}

void DialogTracker::OnSpinCtrlExtrapolationHistorySizeChange(wxSpinEvent& event)
{
    for (auto f : plugin)
	f->motionEstimatorLength = event.GetValue();
}

void DialogTracker::OnSpinCtrlExtrapolationDecayChange(wxSpinDoubleEvent& event)
{
    for (auto f : plugin)
	f->extrapolationDecay = event.GetValue();
}

void DialogTracker::OnSpinCtrlMaxMotionChange(wxSpinDoubleEvent& event)
{
    for (auto f : plugin)
	f->maxMotionPerSecond = event.GetValue();
}

void DialogTracker::OnSpinCtrlMinInterdistanceChange(wxSpinDoubleEvent& event)
{
    for (auto f : plugin)
	f->minInterdistance = event.GetValue();
}

void DialogTracker::OnSpinCtrlEntitiesCountChange(wxSpinEvent& event)
{
    for (auto f : plugin)
        f->SetMaxEntities(event.GetValue());
}

void DialogTracker::OnSpinCtrlVELifetimeChange(wxSpinDoubleEvent& event)
{
    for (auto f : plugin)
	f->virtualEntitiesLifetime = event.GetValue();
}

void DialogTracker::OnSpinCtrlVEDistanceChange(wxSpinDoubleEvent& event)
{
    for (auto f : plugin)
	f->virtualEntitiesDistsq = event.GetValue() * event.GetValue();
}

void DialogTracker::OnSpinCtrlVEDelayChange(wxSpinDoubleEvent& event)
{
    for (auto f : plugin)
	f->virtualEntitiesDelay = event.GetValue();
}

void DialogTracker::OnCheckBoxReplayClick(wxCommandEvent& event)
{
    for (auto f : plugin)
    {
	f->SetReplay(event.IsChecked());
    }
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


void DialogTracker::OnButtonLoadHistoryClick(wxCommandEvent& event)
{
    wxString caption = wxT("Choose a data file to replay");
    wxString wildcard = wxT("CSV file (*.csv)|*.csv");
    wxFileDialog dialog(this, caption, "", "", wildcard, wxFD_CHANGE_DIR | wxFD_OPEN | wxFD_FILE_MUST_EXIST);

    if (dialog.ShowModal() == wxID_OK)
    {
	wxString path = dialog.GetPath();

	// process data
	for (auto f : plugin)
	{
	    f->LoadHistory(path.ToStdString());
	    f->SetReplay(true);
	}
	SpinCtrlEntitiesCount->SetValue(plugin[0]->entitiesCount);
	CheckBoxReplay->SetValue(true);
    }
}

void DialogTracker::OnSpinCtrlTrailLengthChange(wxSpinEvent& event)
{
    for (auto f : plugin)
	f->trailLength = event.GetValue();
}
