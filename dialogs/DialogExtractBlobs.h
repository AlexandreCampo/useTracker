#ifndef DIALOGEXTRACTBLOBS_H
#define DIALOGEXTRACTBLOBS_H

//(*Headers(DialogExtractBlobs)
#include <wx/spinctrl.h>
#include <wx/checkbox.h>
#include <wx/dialog.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/filepicker.h>
#include <wx/stattext.h>
//*)

#include "PipelinePlugin.h"
#include "plugins/ExtractBlobs.h"

class DialogExtractBlobs: public wxDialog
{
	public:

    std::vector<ExtractBlobs*> plugin;
    void SetPlugin (std::vector<PipelinePlugin*> pfv);

		DialogExtractBlobs(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~DialogExtractBlobs();

		//(*Declarations(DialogExtractBlobs)
		wxButton* buttonOk;
		wxCheckBox* CheckBoxOutput;
		wxStaticText* StaticText1;
		wxSpinCtrl* SpinCtrlExtractBlobsSize;
		wxCheckBox* CheckBoxRecordLabels;
		wxStaticText* StaticText2;
		wxFilePickerCtrl* FilePickerCtrl1;
		//*)

	protected:

		//(*Identifiers(DialogExtractBlobs)
		static const long ID_STATICTEXT1;
		static const long ID_SPINCTRL1;
		static const long ID_STATICTEXT2;
		static const long ID_CHECKBOX2;
		static const long ID_CHECKBOX1;
		static const long ID_FILEPICKERCTRL1;
		static const long ID_BUTTON2;
		//*)

	private:

		//(*Handlers(DialogExtractBlobs)
		void OnButtonCancelClick(wxCommandEvent& event);
		void OnButtonApplyClick(wxCommandEvent& event);
		void OnButtonOkClick(wxCommandEvent& event);
		void OnSpinCtrlExtractBlobsSizeChange(wxSpinEvent& event);
		void OnFilePickerCtrl1FileChanged(wxFileDirPickerEvent& event);
		void OnCheckBoxOutputClick(wxCommandEvent& event);
		void OnCheckBoxRecordLabelsClick(wxCommandEvent& event);
		void OnKeyDown(wxKeyEvent& event);
		void OnTextCtrl1Text(wxCommandEvent& event);
		//*)

		DECLARE_EVENT_TABLE()
};

#endif
