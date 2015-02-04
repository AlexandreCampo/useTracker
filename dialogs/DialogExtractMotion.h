#ifndef DIALOGIMAGEEXTRACTMOTION_H
#define DIALOGIMAGEEXTRACTMOTION_H

//(*Headers(DialogExtractMotion)
#include <wx/spinctrl.h>
#include <wx/dialog.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/stattext.h>
//*)

#include "PipelinePlugin.h"
#include "plugins/ExtractMotion.h"

class DialogExtractMotion: public wxDialog
{
	public:

    std::vector<ExtractMotion*> plugin;
    void SetPlugin (std::vector<PipelinePlugin*> pfv);

		DialogExtractMotion(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~DialogExtractMotion();

		//(*Declarations(DialogExtractMotion)
		wxSpinCtrl* SpinCtrlExtractMotionSize;
		wxStaticText* StaticText1;
		wxButton* ButtonOk;
		//*)

	protected:

		//(*Identifiers(DialogExtractMotion)
		static const long ID_STATICTEXT1;
		static const long ID_SPINCTRL1;
		static const long ID_BUTTON1;
		//*)

	private:

		//(*Handlers(DialogExtractMotion)
		void OnButtonCancelClick(wxCommandEvent& event);
		void OnButtonApplyClick(wxCommandEvent& event);
		void OnButtonOkClick(wxCommandEvent& event);
		void OnSpinCtrlExtractMotionSizeChange(wxSpinEvent& event);
		//*)

		DECLARE_EVENT_TABLE()
};

#endif
