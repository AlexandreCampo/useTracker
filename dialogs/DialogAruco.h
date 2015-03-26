#ifdef ARUCO

#ifndef DIALOGARUCO_H
#define DIALOGARUCO_H

//(*Headers(DialogAruco)
#include <wx/spinctrl.h>
#include <wx/checkbox.h>
#include <wx/dialog.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/filepicker.h>
#include <wx/stattext.h>
#include <wx/radiobox.h>
//*)

#include "PipelinePlugin.h"
#include "plugins/Aruco.h"

class DialogAruco: public wxDialog
{
	public:

    std::vector<Aruco*> plugin;
    void SetPlugin (std::vector<PipelinePlugin*> pfv);

		DialogAruco(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~DialogAruco();

		//(*Declarations(DialogAruco)
		wxCheckBox* CheckBoxOutput;
		wxStaticText* StaticText1;
		wxStaticText* StaticText3;
		wxSpinCtrl* SpinCtrlMaxSize;
		wxRadioBox* RadioBoxMethod;
		wxButton* ButtonOk;
		wxSpinCtrl* SpinCtrlMinSize;
		wxStaticText* StaticText4;
		wxStaticText* StaticText2;
		wxSpinCtrl* SpinCtrlThreshold1;
		wxFilePickerCtrl* FilePickerCtrl1;
		wxSpinCtrl* SpinCtrlThreshold2;
		//*)

		wxSpinCtrlDouble* SpinCtrlDoubleMaxSize;
		wxSpinCtrlDouble* SpinCtrlDoubleMinSize;
		/* wxSpinCtrlDouble* SpinCtrlDoubleThreshold1; */
		/* wxSpinCtrlDouble* SpinCtrlDoubleThreshold2; */


	protected:

		//(*Identifiers(DialogAruco)
		static const long ID_RADIOBOX1;
		static const long ID_STATICTEXT1;
		static const long ID_SPINCTRL1;
		static const long ID_STATICTEXT2;
		static const long ID_SPINCTRL2;
		static const long ID_STATICTEXT3;
		static const long ID_SPINCTRL3;
		static const long ID_STATICTEXT4;
		static const long ID_SPINCTRL4;
		static const long ID_CHECKBOX1;
		static const long ID_FILEPICKERCTRL1;
		static const long ID_BUTTON1;
		//*)

		/* static const long ID_SPINCTRL1x; */
		/* static const long ID_SPINCTRL2x; */
		static const long ID_SPINCTRL3x;
		static const long ID_SPINCTRL4x;

	private:

		//(*Handlers(DialogAruco)
		void OnSpinCtrlThreshold1Change(wxSpinEvent& event);
		void OnSpinCtrlMinSizeChange(wxSpinDoubleEvent& event);
		void OnSpinCtrlMaxSizeChange(wxSpinDoubleEvent& event);
		void OnButtonOkClick(wxCommandEvent& event);
		void OnSpinCtrlThreshold2Change(wxSpinEvent& event);
		void OnRadioBoxMethodSelect(wxCommandEvent& event);
		void OnCheckBoxOutputClick(wxCommandEvent& event);
		void OnFilePickerCtrl1FileChanged(wxFileDirPickerEvent& event);
		//*)

		DECLARE_EVENT_TABLE()
};

#endif
#endif
