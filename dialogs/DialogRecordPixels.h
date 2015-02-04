#ifndef DIALOGRECORDPIXELS_H
#define DIALOGRECORDPIXELS_H

//(*Headers(DialogRecordPixels)
#include <wx/checkbox.h>
#include <wx/dialog.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/filepicker.h>
//*)

#include "PipelinePlugin.h"
#include "plugins/RecordPixels.h"

class DialogRecordPixels: public wxDialog
{
	public:

    std::vector<RecordPixels*> plugin;
    void SetPlugin (std::vector<PipelinePlugin*> pfv);


		DialogRecordPixels(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~DialogRecordPixels();

		//(*Declarations(DialogRecordPixels)
		wxCheckBox* CheckBoxOutput;
		wxButton* ButtonOk;
		wxFilePickerCtrl* FilePickerCtrl1;
		//*)

	protected:

		//(*Identifiers(DialogRecordPixels)
		static const long ID_CHECKBOX1;
		static const long ID_FILEPICKERCTRL1;
		static const long ID_BUTTON1;
		//*)

	private:

		//(*Handlers(DialogRecordPixels)
		void OnButtonOkClick(wxCommandEvent& event);
		void OnFilePickerCtrl1FileChanged(wxFileDirPickerEvent& event);
		void OnCheckBoxOutputClick(wxCommandEvent& event);
		//*)

		DECLARE_EVENT_TABLE()
};

#endif
