#ifndef DIALOGOPENCAPTURE_H
#define DIALOGOPENCAPTURE_H

//(*Headers(DialogOpenCapture)
#include <wx/spinctrl.h>
#include <wx/dialog.h>
#include <wx/sizer.h>
#include <wx/notebook.h>
#include <wx/button.h>
#include <wx/panel.h>
#include <wx/filepicker.h>
#include <wx/stattext.h>
//*)

#include "Capture.h"
#include "CaptureVideo.h"
#include "CaptureImage.h"
#include "CaptureUSBCamera.h"
#ifdef VIMBA
#include "CaptureAVTCamera.h"
#endif

class DialogOpenCapture: public wxDialog
{
	public:

		DialogOpenCapture(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~DialogOpenCapture();

		Capture* capture = nullptr;
		
		

		//(*Declarations(DialogOpenCapture)
		wxButton* ButtonUSBOk;
		wxButton* ButtonImageOk;
		wxPanel* PanelVideo;
		wxPanel* PanelImage;
		wxButton* ButtonVideoCancel;
		wxButton* ButtonVideoOk;
		wxStaticText* StaticText1;
		wxButton* ButtonUSBCancel;
		wxButton* ButtonAVTOk;
		wxFilePickerCtrl* FilePickerCtrlVideo;
		wxButton* ButtonAVTCancel;
		wxPanel* PanelUSB;
		wxFilePickerCtrl* FilePickerCtrlImage;
		wxSpinCtrl* SpinCtrlUSBDevice;
		wxPanel* PanelAVT;
		wxStaticText* StaticText2;
		wxNotebook* Notebook1;
		wxSpinCtrl* SpinCtrlAVTDevice;
		wxButton* ButtonImageCancel;
		//*)

	protected:

		//(*Identifiers(DialogOpenCapture)
		static const long ID_FILEPICKERCTRL1;
		static const long ID_BUTTON1;
		static const long ID_BUTTON2;
		static const long ID_PANEL1;
		static const long ID_STATICTEXT1;
		static const long ID_SPINCTRL1;
		static const long ID_BUTTON3;
		static const long ID_BUTTON4;
		static const long ID_PANEL2;
		static const long ID_STATICTEXT2;
		static const long ID_SPINCTRL2;
		static const long ID_BUTTON5;
		static const long ID_BUTTON6;
		static const long ID_PANEL3;
		static const long ID_FILEPICKERCTRL2;
		static const long ID_BUTTON7;
		static const long ID_BUTTON8;
		static const long ID_PANEL4;
		static const long ID_NOTEBOOK1;
		//*)

	private:

		//(*Handlers(DialogOpenCapture)
		void OnFilePickerCtrlVideoFileChanged(wxFileDirPickerEvent& event);
		void OnButtonVideoCancelClick(wxCommandEvent& event);
		void OnButtonVideoOkClick(wxCommandEvent& event);
		void OnButtonUSBOkClick(wxCommandEvent& event);
		void OnButtonUSBCancelClick(wxCommandEvent& event);
		void OnButtonAVTOkClick(wxCommandEvent& event);
		void OnButtonImageCancelClick(wxCommandEvent& event);
		void OnButtonImageOkClick(wxCommandEvent& event);
		void OnButtonAVTCancelClick(wxCommandEvent& event);
		void OnKeyDown(wxKeyEvent& event);
		void OnChar(wxKeyEvent& event);
		void OnNotebook1PageChanged(wxNotebookEvent& event);
		void OnFilePickerCtrlImageFileChanged(wxFileDirPickerEvent& event);
		void OnSpinCtrlAVTDeviceChange(wxSpinEvent& event);
		void OnSpinCtrlUSBDeviceChange(wxSpinEvent& event);
		//*)

		DECLARE_EVENT_TABLE()
};

#endif
