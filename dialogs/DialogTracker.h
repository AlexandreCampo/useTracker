#ifndef DIALOGTRACKER_H
#define DIALOGTRACKER_H

//(*Headers(DialogTracker)
#include <wx/checkbox.h>
#include <wx/dialog.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/filepicker.h>
#include <wx/stattext.h>
//*)

#include <wx/stattext.h>
#include <wx/spinctrl.h>

#include "PipelinePlugin.h"
#include "plugins/Tracker.h"

class DialogTracker: public wxDialog
{
	public:

    vector<Tracker*> plugin;
    void SetPlugin (vector<PipelinePlugin*> pfv);
    wxStaticBoxSizer* StaticBoxSizerVE;
    wxFlexGridSizer* FlexGridSizerMain;

		DialogTracker(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~DialogTracker();

		//(*Declarations(DialogTracker)
		wxButton* buttonOk;
		wxCheckBox* CheckBoxReplay;
		wxCheckBox* CheckBoxUseVE;
		wxCheckBox* CheckBoxOutput;
		wxStaticText* StaticText1a;
		wxCheckBox* CheckBoxVEZone;
		wxFilePickerCtrl* FilePickerCtrl1;
		wxFilePickerCtrl* FilePickerCtrl2;
		//*)

		wxStaticText* StaticText1;
		wxSpinCtrl* SpinCtrlEntitiesCount;
		wxStaticText* StaticText2;
		wxSpinCtrlDouble* SpinCtrlMinInterdistance;
		wxStaticText* StaticText3;
		wxSpinCtrlDouble* SpinCtrlMaxMotion;
		wxStaticText* StaticText4;
		wxSpinCtrlDouble* SpinCtrlExtrapolationDecay;
		wxStaticText* StaticText5;
		wxSpinCtrl* SpinCtrlExtrapolationHistorySize;
		wxStaticText* StaticText6;
		wxSpinCtrlDouble* SpinCtrlExtrapolationTimeout;

		wxStaticText* StaticText10;
		wxSpinCtrlDouble* SpinCtrlVEDistance;
		wxStaticText* StaticText11;
		wxSpinCtrlDouble* SpinCtrlVEDelay;
		wxStaticText* StaticText12;
		wxSpinCtrlDouble* SpinCtrlVELifetime;

		/* SpinCtrlEntitiesCount = new wxSpinCtrl(Panel1, ID_SPINCTRL1, _T("0"), wxDefaultPosition, wxDefaultSize, 0, 0, 100000, 0, _T("ID_SPINCTRL1")); */
		/* StaticText2 = new wxStaticText(Panel1, ID_STATICTEXT2, _("Min interdistance"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT2")); */
		/* SpinCtrlMinInterdistance = new wxSpinCtrlDouble(Panel1, ID_SPINCTRL2, _T("0"), wxDefaultPosition, wxDefaultSize, 0, 0, 100000000, 0, _T("ID_SPINCTRL2")); */
		/* StaticText3 = new wxStaticText(Panel1, ID_STATICTEXT3, _("Max motion (pixel/sec)"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT3")); */
		/* SpinCtrlMaxMotion = new wxSpinCtrlDouble(Panel1, ID_SPINCTRL3, _T("0"), wxDefaultPosition, wxDefaultSize, 0, 0, 1000000000, 0, _T("ID_SPINCTRL3")); */
		/* StaticText4 = new wxStaticText(Panel1, ID_STATICTEXT4, _("Extrapolation decay"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT4")); */
		/* SpinCtrlExtrapolationDecay = new wxSpinCtrlDouble(Panel1, ID_SPINCTRL4, _T("0"), wxDefaultPosition, wxDefaultSize, 0, 0, 1, 0, _T("ID_SPINCTRL4")); */
		/* StaticText5 = new wxStaticText(Panel1, ID_STATICTEXT5, _("Extrapolation history size"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT5")); */
		/* SpinCtrlExtrapolationHistorySize = new wxSpinCtrl(Panel1, ID_SPINCTRL5, _T("0"), wxDefaultPosition, wxDefaultSize, 0, 0, 1000, 0, _T("ID_SPINCTRL5")); */
		/* StaticText6 = new wxStaticText(Panel1, ID_STATICTEXT6, _("Extrapolation timeout"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT6")); */
		/* SpinCtrlExtrapolationTimeout = new wxSpinCtrlDouble(Panel1, ID_SPINCTRL6, _T("0"), wxDefaultPosition, wxDefaultSize, 0, 0, 1000, 0, _T("ID_SPINCTRL6")); */

		/* StaticText10 = new wxStaticText(Panel1, ID_STATICTEXT10, _("Create when last position dist >"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT10")); */
		/* SpinCtrlVEDistance = new wxSpinCtrlDouble(Panel1, ID_SPINCTRL8, _T("0"), wxDefaultPosition, wxDefaultSize, 0, 0, 100000, 0, _T("ID_SPINCTRL8")); */
		/* StaticText11 = new wxStaticText(Panel1, ID_STATICTEXT11, _("Time to promotion"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT11")); */
		/* SpinCtrlVEDelay = new wxSpinCtrlDouble(Panel1, ID_SPINCTRL9, _T("0"), wxDefaultPosition, wxDefaultSize, 0, 0, 1000, 0, _T("ID_SPINCTRL9")); */
		/* StaticText12 = new wxStaticText(Panel1, ID_STATICTEXT12, _("Max lifetime"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT12")); */
		/* SpinCtrlVELifetime = new wxSpinCtrlDouble(Panel1, ID_SPINCTRL10, _T("0"), wxDefaultPosition, wxDefaultSize, 0, 0, 1000, 0, _T("ID_SPINCTRL10")); */


	protected:

		//(*Identifiers(DialogTracker)
		static const long ID_CHECKBOX2;
		static const long ID_STATICTEXT1A;
		static const long ID_CHECKBOX3;
		static const long ID_CHECKBOX1;
		static const long ID_FILEPICKERCTRL1;
		static const long ID_CHECKBOX4;
		static const long ID_FILEPICKERCTRL2;
		static const long ID_BUTTON2;
		//*)

		static const long ID_STATICTEXT1;
		static const long ID_SPINCTRL1;
		static const long ID_STATICTEXT2;
		static const long ID_SPINCTRL2;
		static const long ID_STATICTEXT3;
		static const long ID_SPINCTRL3;
		static const long ID_STATICTEXT4;
		static const long ID_SPINCTRL4;
		static const long ID_STATICTEXT5;
		static const long ID_SPINCTRL5;
		static const long ID_STATICTEXT6;
		static const long ID_SPINCTRL6;
		static const long ID_STATICTEXT10;
		static const long ID_SPINCTRL8;
		static const long ID_STATICTEXT11;
		static const long ID_SPINCTRL9;
		static const long ID_STATICTEXT12;
		static const long ID_SPINCTRL10;


	private:

		//(*Handlers(DialogTracker)
		void OnButtonOkClick(wxCommandEvent& event);
		void OnButtonCancelClick(wxCommandEvent& event);
		void OnFilePickerCtrl1FileChanged(wxFileDirPickerEvent& event);
		void OnCheckBoxOutputClick(wxCommandEvent& event);
		void OnCheckBoxOutputClick1(wxCommandEvent& event);
		void OnSpinCtrlExtrapolationTimeoutChange(wxSpinEvent& event);
		void OnSpinCtrlExtrapolationHistorySizeChange(wxSpinEvent& event);
		void OnSpinCtrlExtrapolationDecayChange(wxSpinEvent& event);
		void OnSpinCtrlMaxMotionChange(wxSpinEvent& event);
		void OnSpinCtrlMinInterdistanceChange(wxSpinEvent& event);
		void OnSpinCtrlEntitiesCountChange(wxSpinEvent& event);
		void OnSpinCtrlVELifetimeChange(wxSpinEvent& event);
		void OnSpinCtrlVEDistanceChange(wxSpinEvent& event);
		void OnSpinCtrlVEDelayChange(wxSpinEvent& event);
		void OnCheckBoxReplayClick(wxCommandEvent& event);
		void OnFilePickerCtrl2FileChanged(wxFileDirPickerEvent& event);
		void OnCheckBoxUseVEClick(wxCommandEvent& event);
		//*)

		DECLARE_EVENT_TABLE()
};

#endif
