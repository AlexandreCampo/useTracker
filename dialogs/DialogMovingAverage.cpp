#include "DialogMovingAverage.h"

//(*InternalHeaders(DialogMovingAverage)
#include <wx/string.h>
#include <wx/intl.h>
//*)

//(*IdInit(DialogMovingAverage)
const long DialogMovingAverage::ID_STATICTEXT1 = wxNewId();
const long DialogMovingAverage::ID_SPINCTRL1 = wxNewId();
const long DialogMovingAverage::ID_STATICTEXT2 = wxNewId();
const long DialogMovingAverage::ID_SPINCTRL2 = wxNewId();
const long DialogMovingAverage::ID_BUTTON1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(DialogMovingAverage,wxDialog)
	//(*EventTable(DialogMovingAverage)
	//*)
END_EVENT_TABLE()

DialogMovingAverage::DialogMovingAverage(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
	//(*Initialize(DialogMovingAverage)
	wxStaticBoxSizer* StaticBoxSizer2;
	wxFlexGridSizer* FlexGridSizer1;
	wxFlexGridSizer* FlexGridSizer2;
	wxFlexGridSizer* FlexGridSizer4;
	wxFlexGridSizer* FlexGridSizer3;
	wxStaticBoxSizer* StaticBoxSizer1;

	Create(parent, wxID_ANY, _("Moving average"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, _T("wxID_ANY"));
	SetMinSize(wxSize(-1,-1));
	SetMaxSize(wxSize(-1,-1));
	SetFocus();
	FlexGridSizer3 = new wxFlexGridSizer(0, 1, 0, 0);
	StaticBoxSizer1 = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Moving Average"));
	FlexGridSizer1 = new wxFlexGridSizer(0, 2, 0, 0);
	StaticText1 = new wxStaticText(this, ID_STATICTEXT1, _("Average over (frames)"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
	FlexGridSizer1->Add(StaticText1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	SpinCtrlMovingAverageLength = new wxSpinCtrl(this, ID_SPINCTRL1, _T("1"), wxDefaultPosition, wxDefaultSize, 0, 1, 1000, 1, _T("ID_SPINCTRL1"));
	SpinCtrlMovingAverageLength->SetValue(_T("1"));
	FlexGridSizer1->Add(SpinCtrlMovingAverageLength, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticBoxSizer1->Add(FlexGridSizer1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer3->Add(StaticBoxSizer1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticBoxSizer2 = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Segmentation"));
	FlexGridSizer4 = new wxFlexGridSizer(0, 2, 0, 0);
	StaticText2 = new wxStaticText(this, ID_STATICTEXT2, _("Keep pixels present in at least (# of frames)"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT2"));
	FlexGridSizer4->Add(StaticText2, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	SpinCtrlSegmentationThreshold = new wxSpinCtrl(this, ID_SPINCTRL2, _T("1"), wxDefaultPosition, wxDefaultSize, 0, 1, 1000, 1, _T("ID_SPINCTRL2"));
	SpinCtrlSegmentationThreshold->SetValue(_T("1"));
	FlexGridSizer4->Add(SpinCtrlSegmentationThreshold, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticBoxSizer2->Add(FlexGridSizer4, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer3->Add(StaticBoxSizer2, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer2 = new wxFlexGridSizer(0, 3, 0, 0);
	ButtonOk = new wxButton(this, ID_BUTTON1, _("OK"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
	ButtonOk->SetDefault();
	ButtonOk->SetFocus();
	FlexGridSizer2->Add(ButtonOk, 1, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer3->Add(FlexGridSizer2, 1, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5);
	SetSizer(FlexGridSizer3);
	FlexGridSizer3->Fit(this);
	FlexGridSizer3->SetSizeHints(this);

	Connect(ID_SPINCTRL1,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&DialogMovingAverage::OnSpinCtrlMovingAverageLengthChange);
	Connect(ID_SPINCTRL2,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&DialogMovingAverage::OnSpinCtrlSegmentationThresholdChange);
	Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&DialogMovingAverage::OnButtonOkClick);
	Connect(wxEVT_KEY_DOWN,(wxObjectEventFunction)&DialogMovingAverage::OnKeyDown);
	//*)

	Fit();
	Centre();
}

DialogMovingAverage::~DialogMovingAverage()
{
	//(*Destroy(DialogMovingAverage)
	//*)
}

void DialogMovingAverage::SetPlugin (std::vector<PipelinePlugin*> pfv)
{
    for (auto func : pfv)
	plugin.push_back(dynamic_cast<MovingAverage*> (func));
    SpinCtrlMovingAverageLength->SetValue(plugin[0]->length);
    SpinCtrlSegmentationThreshold->SetValue(plugin[0]->threshold);
    SpinCtrlSegmentationThreshold->SetRange(1, plugin[0]->length);
}

void DialogMovingAverage::OnButtonCancelClick(wxCommandEvent& event)
{
    this->Hide();
}


void DialogMovingAverage::OnButtonOkClick(wxCommandEvent& event)
{
    OnButtonApplyClick(event);
    this->Hide();
}

void DialogMovingAverage::OnSpinCtrlMovingAverageLengthChange(wxSpinEvent& event)
{
    OnButtonApplyClick (event);
}

void DialogMovingAverage::OnButtonApplyClick(wxCommandEvent& event)
{
    for (auto f : plugin)
    {
    	f->SetLength(SpinCtrlMovingAverageLength->GetValue());
    	f->SetThreshold(SpinCtrlSegmentationThreshold->GetValue());
    }
   SpinCtrlSegmentationThreshold->SetRange(1, plugin[0]->length);
}

void DialogMovingAverage::OnSpinCtrlSegmentationThresholdChange(wxSpinEvent& event)
{
    OnButtonApplyClick (event);
}

