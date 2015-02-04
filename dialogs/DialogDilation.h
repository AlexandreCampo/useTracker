#ifndef DIALOGIMAGEDILATION_H
#define DIALOGIMAGEDILATION_H

//(*Headers(DialogDilation)
#include <wx/spinctrl.h>
#include <wx/dialog.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/stattext.h>
//*)

#include "PipelinePlugin.h"
#include "plugins/Dilation.h"

class DialogDilation: public wxDialog
{
	public:

    std::vector<Dilation*> plugin;
    void SetPlugin (std::vector<PipelinePlugin*> pfv);

		DialogDilation(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~DialogDilation();

		//(*Declarations(DialogDilation)
		wxStaticText* StaticText1;
		wxSpinCtrl* SpinCtrlDilationSize;
		wxButton* ButtonOk;
		//*)

	protected:

		//(*Identifiers(DialogDilation)
		static const long ID_STATICTEXT1;
		static const long ID_SPINCTRL1;
		static const long ID_BUTTON1;
		//*)

	private:

		//(*Handlers(DialogDilation)
		void OnButtonCancelClick(wxCommandEvent& event);
		void OnButtonApplyClick(wxCommandEvent& event);
		void OnButtonOkClick(wxCommandEvent& event);
		void OnSpinCtrlDilationSizeChange(wxSpinEvent& event);
		//*)

		DECLARE_EVENT_TABLE()
};

#endif
