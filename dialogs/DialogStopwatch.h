#ifndef DIALOGSTOPWATCH_H
#define DIALOGSTOPWATCH_H

//(*Headers(DialogStopwatch)
#include <wx/checkbox.h>
#include <wx/dialog.h>
#include <wx/sizer.h>
#include <wx/listctrl.h>
#include <wx/button.h>
#include <wx/radiobut.h>
#include <wx/filepicker.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
//*)

#include <wx/notebook.h>

#include "PipelinePlugin.h"
#include "plugins/Stopwatch.h"

class MainFrame;

int wxCALLBACK DialogStopwatchCompareItems(wxIntPtr item1, wxIntPtr item2, wxIntPtr WXUNUSED(sortData));

class DialogStopwatch: public wxDialog
{
public:

    MainFrame* mainFrame;
wxNotebook* notebook;
    std::vector<Stopwatch*> plugin;
    void SetPlugin (std::vector<PipelinePlugin*> pfv);

    void OnChar(wxKeyEvent& event);
    void RecursiveConnectCharEvent(wxWindow* pclComponent);
    void RecursiveDisconnectCharEvent(wxWindow* pclComponent);
    void RefreshShortcuts();
    void RefreshEvents();

		DialogStopwatch(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~DialogStopwatch();

		//(*Declarations(DialogStopwatch)
		wxRadioButton* RadioButton1;
		wxTextCtrl* TextCtrlKey;
		wxTextCtrl* TextCtrlName;
		wxCheckBox* CheckBoxOutput;
		wxButton* ButtonLoadData;
		wxStaticText* StaticText3;
		wxButton* ButtonOk;
		wxButton* ButtonAddEvent;
		wxListView* ListView2;
		wxStaticText* StaticText4;
		wxListView* ListView1;
		wxRadioButton* RadioButtonBeginEnd;
		wxButton* ButtonDeleteRecordedEvents;
		wxFilePickerCtrl* FilePickerCtrl1;
		wxButton* ButtonDeleteEvents;
		//*)

	protected:

		//(*Identifiers(DialogStopwatch)
		static const long ID_CHECKBOX1;
		static const long ID_FILEPICKERCTRL1;
		static const long ID_BUTTON5;
		static const long ID_STATICTEXT3;
		static const long ID_TEXTCTRL3;
		static const long ID_STATICTEXT4;
		static const long ID_TEXTCTRL4;
		static const long ID_RADIOBUTTON1;
		static const long ID_RADIOBUTTON2;
		static const long ID_BUTTON2;
		static const long ID_LISTVIEW1;
		static const long ID_BUTTON3;
		static const long ID_LISTVIEW2;
		static const long ID_BUTTON4;
		static const long ID_BUTTON1;
		//*)

	private:

		//(*Handlers(DialogStopwatch)
		void OnButtonOkClick(wxCommandEvent& event);
		void OnButtonAddEventClick(wxCommandEvent& event);
		void OnCheckBoxOutputClick(wxCommandEvent& event);
		void OnFilePickerCtrl1FileChanged(wxFileDirPickerEvent& event);
		void OnButtonDeleteRecordedEventsClick(wxCommandEvent& event);
		void OnButtonDeleteEventsClick(wxCommandEvent& event);
		void OnButtonLoadDataClick(wxCommandEvent& event);
		//*)

		DECLARE_EVENT_TABLE()
};

#endif
