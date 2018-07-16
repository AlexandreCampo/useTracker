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

#ifdef ARUCO

#ifndef DIALOGARUCO_H
#define DIALOGARUCO_H

//(*Headers(DialogAruco)
#include <wx/spinctrl.h>
#include <wx/checkbox.h>
#include <wx/dialog.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/filepicker.h>
#include <wx/stattext.h>
#include <wx/radiobox.h>
//*)

#include "PipelinePlugin.h"
#include "plugins/Aruco.h"

class DialogAruco: public wxDialog
{
	public:

    std::vector<Aruco*> plugin;
    void SetPlugin (std::vector<PipelinePlugin*> pfv);

		DialogAruco(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~DialogAruco();

		//(*Declarations(DialogAruco)
		wxCheckBox* CheckBoxOutput;
		wxStaticText* StaticText1;
		wxSpinCtrl* SpinCtrlMaskPerspectiveShift;
		wxStaticText* StaticText3;
		wxSpinCtrl* SpinCtrlMaxSize;
		wxRadioBox* RadioBoxMethod;
		wxButton* ButtonOk;
		wxSpinCtrl* SpinCtrlMaskValue;
		wxSpinCtrl* SpinCtrlMinSize;
		wxStaticText* StaticText7;
		wxStaticText* StaticText4;
		wxStaticText* StaticText5;
		wxStaticText* StaticText2;
		wxStaticText* StaticText6;
		wxSpinCtrl* SpinCtrlMaskRadius;
		wxSpinCtrl* SpinCtrlThreshold1;
		wxFilePickerCtrl* FilePickerCtrl1;
		wxRadioBox* RadioBoxMaskShape;
		wxSpinCtrl* SpinCtrlThreshold2;
		//*)

		wxSpinCtrlDouble* SpinCtrlDoubleMaxSize;
		wxSpinCtrlDouble* SpinCtrlDoubleMinSize;
		/* wxSpinCtrlDouble* SpinCtrlDoubleThreshold1; */
		/* wxSpinCtrlDouble* SpinCtrlDoubleThreshold2; */


	protected:

		//(*Identifiers(DialogAruco)
		static const long ID_RADIOBOX1;
		static const long ID_STATICTEXT1;
		static const long ID_SPINCTRL1;
		static const long ID_STATICTEXT2;
		static const long ID_SPINCTRL2;
		static const long ID_STATICTEXT3;
		static const long ID_SPINCTRL3;
		static const long ID_STATICTEXT4;
		static const long ID_SPINCTRL4;
		static const long ID_RADIOBOX2;
		static const long ID_STATICTEXT5;
		static const long ID_SPINCTRL5;
		static const long ID_STATICTEXT6;
		static const long ID_SPINCTRL6;
		static const long ID_STATICTEXT7;
		static const long ID_SPINCTRL7;
		static const long ID_CHECKBOX1;
		static const long ID_FILEPICKERCTRL1;
		static const long ID_BUTTON1;
		//*)

		/* static const long ID_SPINCTRL1x; */
		/* static const long ID_SPINCTRL2x; */
		static const long ID_SPINCTRL3x;
		static const long ID_SPINCTRL4x;

	private:

		//(*Handlers(DialogAruco)
		void OnSpinCtrlThreshold1Change(wxSpinEvent& event);
		void OnSpinCtrlMinSizeChange(wxSpinDoubleEvent& event);
		void OnSpinCtrlMaxSizeChange(wxSpinDoubleEvent& event);
		void OnButtonOkClick(wxCommandEvent& event);
		void OnSpinCtrlThreshold2Change(wxSpinEvent& event);
		void OnRadioBoxMethodSelect(wxCommandEvent& event);
		void OnCheckBoxOutputClick(wxCommandEvent& event);
		void OnFilePickerCtrl1FileChanged(wxFileDirPickerEvent& event);
		void OnRadioBoxMaskShapeSelect(wxCommandEvent& event);
		void OnSpinCtrlMaskRadiusChange(wxSpinEvent& event);
		void OnSpinCtrlMaskPerspectiveShiftChange(wxSpinEvent& event);
		void OnSpinCtrlMaskValueChange(wxSpinEvent& event);
		//*)

		DECLARE_EVENT_TABLE()
};

#endif
#endif
