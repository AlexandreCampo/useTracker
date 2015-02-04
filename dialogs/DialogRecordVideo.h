#ifndef DIALOGRECORDVIDEO_H
#define DIALOGRECORDVIDEO_H

//(*Headers(DialogRecordVideo)
#include <wx/spinctrl.h>
#include <wx/checkbox.h>
#include <wx/dialog.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/filepicker.h>
#include <wx/stattext.h>
#include <wx/choice.h>
//*)

#include "PipelinePlugin.h"
#include "plugins/RecordVideo.h"

class DialogRecordVideo: public wxDialog
{
	public:

    std::vector<RecordVideo*> plugin;
    void SetPlugin (std::vector<PipelinePlugin*> pfv);

		DialogRecordVideo(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~DialogRecordVideo();

		//(*Declarations(DialogRecordVideo)
		wxCheckBox* CheckBoxOutput;
		wxStaticText* StaticText1;
		wxSpinCtrl* SpinCtrlBitrate;
		wxChoice* ChoicePreset;
		wxButton* ButtonOk;
		wxStaticText* StaticText2;
		wxFilePickerCtrl* FilePickerCtrl1;
		//*)

	protected:

		//(*Identifiers(DialogRecordVideo)
		static const long ID_STATICTEXT1;
		static const long ID_SPINCTRL1;
		static const long ID_STATICTEXT2;
		static const long ID_CHOICE1;
		static const long ID_CHECKBOX1;
		static const long ID_FILEPICKERCTRL1;
		static const long ID_BUTTON1;
		//*)

	private:

		//(*Handlers(DialogRecordVideo)
		void OnSpinCtrlBitrateChange(wxSpinEvent& event);
		void OnChoicePresetSelect(wxCommandEvent& event);
		void OnCheckBoxOutputClick(wxCommandEvent& event);
		void OnButtonOkClick(wxCommandEvent& event);
		void OnFilePickerCtrl1FileChanged(wxFileDirPickerEvent& event);
		//*)

		DECLARE_EVENT_TABLE()
};

#endif
