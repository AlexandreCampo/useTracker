#ifndef DIALOGIMAGEEROSION_H
#define DIALOGIMAGEEROSION_H

//(*Headers(DialogErosion)
#include <wx/spinctrl.h>
#include <wx/dialog.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/stattext.h>
//*)

#include "PipelinePlugin.h"
#include "plugins/Erosion.h"

class DialogErosion: public wxDialog
{
	public:

    std::vector<Erosion*> plugin;
    void SetPlugin (std::vector<PipelinePlugin*> pfv);

		DialogErosion(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~DialogErosion();

		//(*Declarations(DialogErosion)
		wxSpinCtrl* SpinCtrlErosionSize;
		wxStaticText* StaticText1;
		wxButton* ButtonOk;
		//*)

	protected:

		//(*Identifiers(DialogErosion)
		static const long ID_STATICTEXT1;
		static const long ID_SPINCTRL1;
		static const long ID_BUTTON1;
		//*)

	private:

		//(*Handlers(DialogErosion)
		void OnButtonCancelClick(wxCommandEvent& event);
		void OnButtonApplyClick(wxCommandEvent& event);
		void OnButtonOkClick(wxCommandEvent& event);
		void OnSpinCtrlErosionSizeChange(wxSpinEvent& event);
		//*)

		DECLARE_EVENT_TABLE()
};

#endif
