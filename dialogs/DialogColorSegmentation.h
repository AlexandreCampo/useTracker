#ifndef DIALOGIMAGECOLORSEGMENTATION_H
#define DIALOGIMAGECOLORSEGMENTATION_H

//(*Headers(DialogColorSegmentation)
#include <wx/spinctrl.h>
#include <wx/dialog.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/stattext.h>
#include <wx/clrpicker.h>
#include <wx/radiobox.h>
//*)

#include "PipelinePlugin.h"
#include "plugins/ColorSegmentation.h"

class DialogColorSegmentation: public wxDialog
{
	public:

    cv::Mat pixelSrc;
    cv::Mat pixelDst;
    cv::Mat pixelRes;
    std::vector<ColorSegmentation*> plugin;
    void SetPlugin (std::vector<PipelinePlugin*> pfv);

		DialogColorSegmentation(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~DialogColorSegmentation();

		//(*Declarations(DialogColorSegmentation)
		wxStaticText* HighC2;
		wxSpinCtrl* SpinCtrlHighC2;
		wxStaticText* HighC1;
		wxSpinCtrl* SpinCtrlLowC2;
		wxColourPickerCtrl* ColourPickerCtrlMin;
		wxSpinCtrl* SpinCtrlHighC3;
		wxSpinCtrl* SpinCtrlLowC3;
		wxColourPickerCtrl* ColourPickerCtrlMax;
		wxStaticText* StaticText1;
		wxStaticText* LowC1;
		wxStaticText* LowC3;
		wxButton* ButtonOk;
		wxStaticText* LowC2;
		wxSpinCtrl* SpinCtrlLowC1;
		wxSpinCtrl* SpinCtrlHighC1;
		wxStaticText* StaticText2;
		wxRadioBox* RadioBox1;
		wxStaticText* HighC3;
		//*)

	protected:

		//(*Identifiers(DialogColorSegmentation)
		static const long ID_RADIOBOX1;
		static const long ID_STATICTEXT1;
		static const long ID_COLOURPICKERCTRL1;
		static const long ID_STATICTEXT3;
		static const long ID_SPINCTRL1;
		static const long ID_STATICTEXT4;
		static const long ID_SPINCTRL2;
		static const long ID_STATICTEXT5;
		static const long ID_SPINCTRL3;
		static const long ID_STATICTEXT2;
		static const long ID_COLOURPICKERCTRL2;
		static const long ID_STATICTEXT7;
		static const long ID_SPINCTRL4;
		static const long ID_STATICTEXT6;
		static const long ID_SPINCTRL5;
		static const long ID_STATICTEXT8;
		static const long ID_SPINCTRL6;
		static const long ID_BUTTON1;
		//*)

	private:

		//(*Handlers(DialogColorSegmentation)
		void OnButtonCancelClick(wxCommandEvent& event);
		void OnButtonApplyClick(wxCommandEvent& event);
		void OnButtonOkClick(wxCommandEvent& event);
		void OnRadioBoxSelect(wxCommandEvent& event);
		void OnSpinCtrlChange(wxSpinEvent& event);
		void OnColourPickerCtrlMaxColourChanged(wxColourPickerEvent& event);
		void OnColourPickerCtrlMinColourChanged(wxColourPickerEvent& event);
		//*)

		DECLARE_EVENT_TABLE()
};

#endif
