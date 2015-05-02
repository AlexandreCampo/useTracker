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

#include "DialogStopwatch.h"

#include "MainFrame.h"

#include <algorithm>
#include <vector>

using namespace std;

//(*InternalHeaders(DialogStopwatch)
#include <wx/string.h>
#include <wx/intl.h>
//*)

//(*IdInit(DialogStopwatch)
const long DialogStopwatch::ID_CHECKBOX1 = wxNewId();
const long DialogStopwatch::ID_FILEPICKERCTRL1 = wxNewId();
const long DialogStopwatch::ID_BUTTON5 = wxNewId();
const long DialogStopwatch::ID_STATICTEXT3 = wxNewId();
const long DialogStopwatch::ID_TEXTCTRL3 = wxNewId();
const long DialogStopwatch::ID_STATICTEXT4 = wxNewId();
const long DialogStopwatch::ID_TEXTCTRL4 = wxNewId();
const long DialogStopwatch::ID_RADIOBUTTON1 = wxNewId();
const long DialogStopwatch::ID_RADIOBUTTON2 = wxNewId();
const long DialogStopwatch::ID_BUTTON2 = wxNewId();
const long DialogStopwatch::ID_LISTVIEW1 = wxNewId();
const long DialogStopwatch::ID_BUTTON3 = wxNewId();
const long DialogStopwatch::ID_LISTVIEW2 = wxNewId();
const long DialogStopwatch::ID_BUTTON4 = wxNewId();
const long DialogStopwatch::ID_BUTTON1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(DialogStopwatch,wxDialog)
	//(*EventTable(DialogStopwatch)
	//*)
END_EVENT_TABLE()

DialogStopwatch::DialogStopwatch(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
	//(*Initialize(DialogStopwatch)
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
	wxStaticBoxSizer* StaticBoxSizer1;

	Create(parent, wxID_ANY, _("Stopwatch"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, _T("wxID_ANY"));
	FlexGridSizer1 = new wxFlexGridSizer(0, 1, 0, 0);
	FlexGridSizer1->AddGrowableCol(0);
	StaticBoxSizer4 = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Output"));
	FlexGridSizer3 = new wxFlexGridSizer(0, 2, 0, 0);
	FlexGridSizer3->AddGrowableCol(1);
	CheckBoxOutput = new wxCheckBox(this, ID_CHECKBOX1, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX1"));
	CheckBoxOutput->SetValue(false);
	FlexGridSizer3->Add(CheckBoxOutput, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 1);
	FilePickerCtrl1 = new wxFilePickerCtrl(this, ID_FILEPICKERCTRL1, wxEmptyString, _("Select output file"), _T("*.csv"), wxDefaultPosition, wxDefaultSize, wxFLP_OVERWRITE_PROMPT|wxFLP_SAVE|wxFLP_USE_TEXTCTRL, wxDefaultValidator, _T("ID_FILEPICKERCTRL1"));
	FlexGridSizer3->Add(FilePickerCtrl1, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 1);
	StaticBoxSizer4->Add(FlexGridSizer3, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	FlexGridSizer1->Add(StaticBoxSizer4, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 1);
	StaticBoxSizer5 = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Load data"));
	FlexGridSizer9 = new wxFlexGridSizer(0, 1, 0, 0);
	FlexGridSizer9->AddGrowableCol(0);
	ButtonLoadData = new wxButton(this, ID_BUTTON5, _("Load events from file"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON5"));
	FlexGridSizer9->Add(ButtonLoadData, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 3);
	StaticBoxSizer5->Add(FlexGridSizer9, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	FlexGridSizer1->Add(StaticBoxSizer5, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 1);
	StaticBoxSizer3 = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Add shortcut"));
	FlexGridSizer4 = new wxFlexGridSizer(0, 1, 0, 0);
	FlexGridSizer4->AddGrowableCol(0);
	FlexGridSizer5 = new wxFlexGridSizer(0, 2, 0, 0);
	FlexGridSizer5->AddGrowableCol(1);
	StaticText3 = new wxStaticText(this, ID_STATICTEXT3, _("Keystroke"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT3"));
	FlexGridSizer5->Add(StaticText3, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	TextCtrlKey = new wxTextCtrl(this, ID_TEXTCTRL3, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL3"));
	TextCtrlKey->SetMaxLength(1);
	FlexGridSizer5->Add(TextCtrlKey, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	StaticText4 = new wxStaticText(this, ID_STATICTEXT4, _("Event name"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT4"));
	FlexGridSizer5->Add(StaticText4, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	TextCtrlName = new wxTextCtrl(this, ID_TEXTCTRL4, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL4"));
	FlexGridSizer5->Add(TextCtrlName, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	FlexGridSizer4->Add(FlexGridSizer5, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	FlexGridSizer8 = new wxFlexGridSizer(0, 2, 0, 0);
	FlexGridSizer8->AddGrowableCol(0);
	FlexGridSizer8->AddGrowableCol(1);
	RadioButton1 = new wxRadioButton(this, ID_RADIOBUTTON1, _("Single"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP, wxDefaultValidator, _T("ID_RADIOBUTTON1"));
	FlexGridSizer8->Add(RadioButton1, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	RadioButtonBeginEnd = new wxRadioButton(this, ID_RADIOBUTTON2, _("Start/End"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_RADIOBUTTON2"));
	FlexGridSizer8->Add(RadioButtonBeginEnd, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	FlexGridSizer4->Add(FlexGridSizer8, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	ButtonAddEvent = new wxButton(this, ID_BUTTON2, _("Add shortcut"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON2"));
	FlexGridSizer4->Add(ButtonAddEvent, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 3);
	StaticBoxSizer3->Add(FlexGridSizer4, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	FlexGridSizer1->Add(StaticBoxSizer3, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 1);
	StaticBoxSizer1 = new wxStaticBoxSizer(wxHORIZONTAL, this, _("User defined events"));
	FlexGridSizer6 = new wxFlexGridSizer(0, 1, 0, 0);
	FlexGridSizer6->AddGrowableCol(0);
	ListView1 = new wxListView(this, ID_LISTVIEW1, wxDefaultPosition, wxSize(300,140), wxLC_REPORT, wxDefaultValidator, _T("ID_LISTVIEW1"));
	FlexGridSizer6->Add(ListView1, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	ButtonDeleteEvents = new wxButton(this, ID_BUTTON3, _("Delete selected shortcuts"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON3"));
	FlexGridSizer6->Add(ButtonDeleteEvents, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 1);
	StaticBoxSizer1->Add(FlexGridSizer6, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	FlexGridSizer1->Add(StaticBoxSizer1, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 1);
	StaticBoxSizer2 = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Recorded events"));
	FlexGridSizer7 = new wxFlexGridSizer(0, 1, 0, 0);
	FlexGridSizer7->AddGrowableCol(0);
	ListView2 = new wxListView(this, ID_LISTVIEW2, wxDefaultPosition, wxSize(300,140), wxLC_REPORT, wxDefaultValidator, _T("ID_LISTVIEW2"));
	FlexGridSizer7->Add(ListView2, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	ButtonDeleteRecordedEvents = new wxButton(this, ID_BUTTON4, _("Delete selected events"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON4"));
	FlexGridSizer7->Add(ButtonDeleteRecordedEvents, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 1);
	StaticBoxSizer2->Add(FlexGridSizer7, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	FlexGridSizer1->Add(StaticBoxSizer2, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 1);
	FlexGridSizer2 = new wxFlexGridSizer(0, 3, 0, 0);
	ButtonOk = new wxButton(this, ID_BUTTON1, _("Ok"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
	FlexGridSizer2->Add(ButtonOk, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer1->Add(FlexGridSizer2, 1, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 1);
	SetSizer(FlexGridSizer1);
	FlexGridSizer1->Fit(this);
	FlexGridSizer1->SetSizeHints(this);

	Connect(ID_CHECKBOX1,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&DialogStopwatch::OnCheckBoxOutputClick);
	Connect(ID_FILEPICKERCTRL1,wxEVT_COMMAND_FILEPICKER_CHANGED,(wxObjectEventFunction)&DialogStopwatch::OnFilePickerCtrl1FileChanged);
	Connect(ID_BUTTON5,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&DialogStopwatch::OnButtonLoadDataClick);
	Connect(ID_BUTTON2,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&DialogStopwatch::OnButtonAddEventClick);
	Connect(ID_BUTTON3,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&DialogStopwatch::OnButtonDeleteEventsClick);
	Connect(ID_BUTTON4,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&DialogStopwatch::OnButtonDeleteRecordedEventsClick);
	Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&DialogStopwatch::OnButtonOkClick);
	//*)

	ListView1->AppendColumn(_("Key"), wxLIST_FORMAT_LEFT, 40);
	ListView1->AppendColumn(_("Name"), wxLIST_FORMAT_LEFT, 180);
	ListView1->AppendColumn(_("Type"), wxLIST_FORMAT_LEFT, 79);

	ListView2->AppendColumn(_("Time"), wxLIST_FORMAT_LEFT, 70);
	ListView2->AppendColumn(_("Name"), wxLIST_FORMAT_LEFT, 159);
	ListView2->AppendColumn(_("Type"), wxLIST_FORMAT_LEFT, 70);

	// override char events
	mainFrame = dynamic_cast<MainFrame*> (parent);
	notebook = dynamic_cast<wxNotebook*> (mainFrame->FindWindowByName(_T("ID_NOTEBOOK1"), mainFrame));
	mainFrame->DisconnectCharEvent();
	RecursiveConnectCharEvent(mainFrame);
	RecursiveDisconnectCharEvent(notebook);

	ListView1->Layout();
	ListView2->Layout();
	Fit();
}

DialogStopwatch::~DialogStopwatch()
{
    // disconnect event handler, reconnect mainframe
    RecursiveDisconnectCharEvent(mainFrame);
    mainFrame->ConnectCharEvent();

	//(*Destroy(DialogStopwatch)
	//*)
}

void DialogStopwatch::SetPlugin (std::vector<PipelinePlugin*> pfv)
{
    for (auto func : pfv)
	plugin.push_back(dynamic_cast<Stopwatch*> (func));

    FilePickerCtrl1->SetPath(plugin[0]->outputFilename);
    CheckBoxOutput->SetValue(plugin[0]->output);
    RefreshShortcuts();
}

void DialogStopwatch::OnButtonOkClick(wxCommandEvent& event)
{
    this->Hide();
}

void DialogStopwatch::OnButtonAddEventClick(wxCommandEvent& event)
{
    // grab key + description from text fields
    unsigned char key = TextCtrlKey->GetValue().ToStdString()[0];
    std::string name = TextCtrlName->GetValue().ToStdString();

    int type = 0;
    if (RadioButtonBeginEnd->GetValue()) type = 1;

    // replace spaces with underscores in description
    std::replace(name.begin(), name.end(), ' ', '_');

    TextCtrlKey->Clear();
    TextCtrlName->Clear();

    // add the event to plugin
    for (auto f : plugin)
    	f->AddShortcut(key, name, type);

    // refresh event list
    RefreshShortcuts();
}

void DialogStopwatch::RefreshShortcuts()
{
    ListView1->DeleteAllItems();

    vector<Stopwatch::Shortcut>& shortcuts = plugin[0]->shortcuts;

    for (auto e : shortcuts)
    {
	int index = ListView1->GetItemCount();
	ListView1->InsertItem(index, string(1, e.key));
	ListView1->SetItem(index, 1, e.name);

	if (e.type == 0)
	    ListView1->SetItem(index, 2, _("single"));
	else
	    ListView1->SetItem(index, 2, _("start/end"));
    }

    int i = ListView1->GetItemCount();
    if (i > 0) ListView1->EnsureVisible(i-1);
}

void DialogStopwatch::RefreshEvents()
{
    ListView2->DeleteAllItems();

    vector<Stopwatch::Event>& events = plugin[0]->events;

    for (auto e : events)
    {
	int index = ListView2->GetItemCount();
	ListView2->InsertItem(index, std::to_string(e.time));
	ListView2->SetItem(index, 1, e.name);

	if (e.type == 0)
	    ListView2->SetItem(index, 2, _("start"));
	else
	    ListView2->SetItem(index, 2, _("end"));

	ListView2->SetItemData(index, e.id);
    }
}


void DialogStopwatch::OnChar(wxKeyEvent& event)
{
    if (mainFrame->IsRecording() && plugin[0]->active)
    {
	int recordedId =  plugin[0]->RecordEvent (event.GetKeyCode());

	// if record successful, refresh list of recorded events
	if (recordedId >= 0)
	{
	    Stopwatch::Event event = plugin[0]->GetLastEvent();

	    int index = ListView2->GetItemCount();
	    ListView2->InsertItem (index, std::to_string(event.time));
	    ListView2->SetItem (index, 1, event.name);

	    if (event.type == 0)
		ListView2->SetItem (index, 2, _("start"));
	    else
		ListView2->SetItem (index, 2, _("end"));

	    ListView2->SetItemData(index, recordedId);

	    ListView2->SortItems(DialogStopwatchCompareItems, (long int) &(plugin[0]->events));
	    index = ListView2->FindItem(-1, recordedId);

	    ListView2->EnsureVisible(index);
	    ListView2->Focus(index);
	    ListView2->Select(index, true);
	}
   }
    mainFrame->OnChar(event);
}


void DialogStopwatch::RecursiveConnectCharEvent(wxWindow* pclComponent)
{
    if(pclComponent)
    {
	wxDialog* dlg = dynamic_cast<wxDialog*> (pclComponent);

	if (dlg == nullptr)
	{
	    pclComponent->Connect(wxID_ANY,
				  wxEVT_CHAR,
				  wxKeyEventHandler(DialogStopwatch::OnChar),
				  (wxObject*) NULL,
				  this);

	    wxWindowListNode* pclNode = pclComponent->GetChildren().GetFirst();
	    while(pclNode)
	    {
		wxWindow* pclChild = pclNode->GetData();
		this->RecursiveConnectCharEvent(pclChild);

		pclNode = pclNode->GetNext();
	    }
	}
    }
}

void DialogStopwatch::RecursiveDisconnectCharEvent(wxWindow* pclComponent)
{
    if(pclComponent)
    {
	wxDialog* dlg = dynamic_cast<wxDialog*> (pclComponent);

	if (dlg == nullptr)
	{
	    pclComponent->Disconnect(wxID_ANY,
				     wxEVT_CHAR,
				     wxKeyEventHandler(DialogStopwatch::OnChar),
				     (wxObject*) NULL,
				     this);

	    wxWindowListNode* pclNode = pclComponent->GetChildren().GetFirst();
	    while(pclNode)
	    {
		wxWindow* pclChild = pclNode->GetData();
		this->RecursiveDisconnectCharEvent(pclChild);

		pclNode = pclNode->GetNext();
	    }
	}
    }
}


void DialogStopwatch::OnCheckBoxOutputClick(wxCommandEvent& event)
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

void DialogStopwatch::OnFilePickerCtrl1FileChanged(wxFileDirPickerEvent& event)
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

void DialogStopwatch::OnButtonDeleteRecordedEventsClick(wxCommandEvent& event)
{
    int selected = ListView2->GetFirstSelected();

    while (selected >= 0)
    {
	int id = ListView2->GetItemData(selected);
	ListView2->DeleteItem(selected);

	for (auto f : plugin)
	    f->DeleteEvent(id);

	selected = ListView2->GetFirstSelected();
    }
}

void DialogStopwatch::OnButtonDeleteEventsClick(wxCommandEvent& event)
{
    int selected = ListView1->GetFirstSelected();

    while (selected >= 0)
    {
	ListView1->DeleteItem(selected);

	for (auto f : plugin)
	    f->DeleteShortcut(selected);

	selected = ListView1->GetFirstSelected();
    }
}


int wxCALLBACK DialogStopwatchCompareItems(wxIntPtr item1, wxIntPtr item2, wxIntPtr data)
{
    // get list of events
    std::vector<Stopwatch::Event> *events = static_cast<std::vector<Stopwatch::Event>*> ((void*)data);

    // compare
    if ((*events)[item1].time < (*events)[item2].time)
        return -1;
    if ((*events)[item1].time > (*events)[item2].time)
        return 1;

    return 0;
}

void DialogStopwatch::OnButtonLoadDataClick(wxCommandEvent& event)
{
    wxString caption = wxT("Choose a file to load events data");
    wxString wildcard = wxT("CSV file (*.csv)|*.csv");
    wxFileDialog dialog(this, caption, "", "", wildcard, wxFD_CHANGE_DIR | wxFD_OPEN | wxFD_FILE_MUST_EXIST);

    if (dialog.ShowModal() == wxID_OK)
    {
	wxString path = dialog.GetPath();

	// process data
	for (auto f : plugin)
	{
	    f->LoadData(path.ToStdString());
	}

	// refresh list view
	RefreshEvents();
    }
}
