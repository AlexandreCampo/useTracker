/*----------------------------------------------------------------------------*/
/*    Copyright (C) 2015 Alexandre Campo                                      */
/*                                                                            */
/*    This file is part of USE Tracker.                                       */
/*                                                                            */
/*    USE Tracker is free software: you can redistribute it and/or modify     */
/*    it under the terms of the GNU General Public License as published by    */
/*    the Free Software Foundation, either version 3 of the License, or       */
/*    (at your option) any later version.                                     */
/*                                                                            */
/*    USE Tracker is distributed in the hope that it will be useful,          */
/*    but WITHOUT ANY WARRANTY; without even the implied warranty of          */
/*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           */
/*    GNU General Public License for more details.                            */
/*                                                                            */
/*    You should have received a copy of the GNU General Public License       */
/*    along with USE Tracker.  If not, see <http://www.gnu.org/licenses/>.    */
/*----------------------------------------------------------------------------*/

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
#include "CaptureDefault.h"
#include "CaptureVideo.h"
#include "CaptureImage.h"
#include "CaptureUSBCamera.h"
#include "CaptureMultiUSBCamera.h"
#ifdef VIMBA
#include "CaptureAVTCamera.h"
#endif

class DialogOpenCapture: public wxDialog
{
	public:

		DialogOpenCapture(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~DialogOpenCapture();

		Capture* capture = nullptr;
		Capture* previousCapture = nullptr;
				

		//(*Declarations(DialogOpenCapture)
		wxButton* ButtonUSBOk;
		wxButton* ButtonImageOk;
		wxPanel* PanelVideo;
		wxPanel* PanelImage;
		wxPanel* PanelConfigFile;
		wxButton* ButtonVideoCancel;
		wxButton* ButtonVideoOk;
		wxStaticText* StaticText1;
		wxFilePickerCtrl* FilePickerCtrlSourceConfigFile;
		wxStaticText* StaticText3;
		wxButton* ButtonUSBCancel;
		wxButton* ButtonAVTOk;
		wxFilePickerCtrl* FilePickerCtrlVideo;
		wxButton* ButtonAVTCancel;
		wxPanel* PanelUSB;
		wxFilePickerCtrl* FilePickerCtrlImage;
		wxSpinCtrl* SpinCtrlUSBDevice;
		wxStaticText* StaticText4;
		wxPanel* PanelAVT;
		wxStaticText* StaticText2;
		wxNotebook* Notebook1;
		wxSpinCtrl* SpinCtrlAVTDevice;
		wxButton* ButtonConfigFileCancel;
		wxButton* ButtonImageCancel;
		wxButton* ButtonConfigFileOk;
		wxPanel* PanelMultiUSB;
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
		static const long ID_STATICTEXT4;
		static const long ID_STATICTEXT3;
		static const long ID_BUTTON5;
		static const long ID_BUTTON6;
		static const long ID_PANEL3;
		static const long ID_FILEPICKERCTRL2;
		static const long ID_BUTTON7;
		static const long ID_BUTTON8;
		static const long ID_PANEL4;
		static const long ID_PANEL6;
		static const long ID_FILEPICKERCTRL3;
		static const long ID_BUTTON9;
		static const long ID_BUTTON10;
		static const long ID_PANEL5;
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
		void OnButtonConfigFileCancelClick(wxCommandEvent& event);
		void OnButtonConfigFileOkClick(wxCommandEvent& event);
		//*)

		void ConnectCharEvent(wxWindow* pclComponent);

		DECLARE_EVENT_TABLE()
};

#endif
