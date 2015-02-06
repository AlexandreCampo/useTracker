#ifndef DIALOGSAFEEROSION_H
#define DIALOGSAFEEROSION_H

//(*Headers(DialogSafeErosion)
#include <wx/dialog.h>
#include <wx/sizer.h>
#include <wx/button.h>
//*)

#include "PipelinePlugin.h"
#include "plugins/SafeErosion.h"

class DialogSafeErosion: public wxDialog
{
	public:

    std::vector<SafeErosion*> plugin;
    void SetPlugin (std::vector<PipelinePlugin*> pfv);

		DialogSafeErosion(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~DialogSafeErosion();

		//(*Declarations(DialogSafeErosion)
		wxButton* buttonOk;
		//*)

	protected:

		//(*Identifiers(DialogSafeErosion)
		static const long ID_BUTTON2;
		//*)

	private:

		//(*Handlers(DialogSafeErosion)
		void OnButtonCancelClick(wxCommandEvent& event);
		void OnButtonApplyClick(wxCommandEvent& event);
		void OnButtonOkClick(wxCommandEvent& event);
		void OnSpinCtrlSafeErosionSizeChange(wxSpinEvent& event);
		void OnKeyDown(wxKeyEvent& event);
		//*)

		DECLARE_EVENT_TABLE()
};

#endif
