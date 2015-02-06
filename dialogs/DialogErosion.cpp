#include "DialogErosion.h"

//(*InternalHeaders(DialogErosion)
#include <wx/string.h>
#include <wx/intl.h>
//*)

//(*IdInit(DialogErosion)
const long DialogErosion::ID_STATICTEXT1 = wxNewId();
const long DialogErosion::ID_SPINCTRL1 = wxNewId();
const long DialogErosion::ID_BUTTON1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(DialogErosion,wxDialog)
	//(*EventTable(DialogErosion)
	//*)
END_EVENT_TABLE()

DialogErosion::DialogErosion(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
	//(*Initialize(DialogErosion)
	wxFlexGridSizer* FlexGridSizer1;
	wxFlexGridSizer* FlexGridSizer2;
	wxFlexGridSizer* FlexGridSizer3;
	wxStaticBoxSizer* StaticBoxSizer1;

	Create(parent, id, _("Erosion"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, _T("id"));
	SetClientSize(wxDefaultSize);
	Move(wxDefaultPosition);
	SetMinSize(wxSize(-1,-1));
	SetMaxSize(wxSize(-1,-1));
	SetFocus();
	FlexGridSizer3 = new wxFlexGridSizer(0, 1, 0, 0);
	StaticBoxSizer1 = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Erosion"));
	FlexGridSizer1 = new wxFlexGridSizer(0, 2, 0, 0);
	StaticText1 = new wxStaticText(this, ID_STATICTEXT1, _("Shrink borders by (pixels)"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
	FlexGridSizer1->Add(StaticText1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	SpinCtrlErosionSize = new wxSpinCtrl(this, ID_SPINCTRL1, _T("1"), wxDefaultPosition, wxDefaultSize, 0, 1, 1000, 1, _T("ID_SPINCTRL1"));
	SpinCtrlErosionSize->SetValue(_T("1"));
	FlexGridSizer1->Add(SpinCtrlErosionSize, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
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

	Connect(ID_SPINCTRL1,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&DialogErosion::OnSpinCtrlErosionSizeChange);
	Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&DialogErosion::OnButtonOkClick);
	//*)

	Fit();
	Centre();
}

DialogErosion::~DialogErosion()
{
	//(*Destroy(DialogErosion)
	//*)
}

void DialogErosion::SetPlugin (std::vector<PipelinePlugin*> pfv)
{
    for (auto func : pfv)
	plugin.push_back(dynamic_cast<Erosion*> (func));
    SpinCtrlErosionSize->SetValue(plugin[0]->size);
}

void DialogErosion::OnButtonCancelClick(wxCommandEvent& event)
{
    this->Hide();
}


void DialogErosion::OnButtonOkClick(wxCommandEvent& event)
{
    OnButtonApplyClick(event);
    this->Hide();
}

void DialogErosion::OnSpinCtrlErosionSizeChange(wxSpinEvent& event)
{
    OnButtonApplyClick (event);
}

void DialogErosion::OnButtonApplyClick(wxCommandEvent& event)
{
    for (auto f : plugin)
    	f->SetSize(SpinCtrlErosionSize->GetValue());
}

