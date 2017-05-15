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

#ifndef DIALOGARUCOCOLORN_H
#define DIALOGARUCOCOLOR_H

//(*Headers(DialogArucoColor)
#include <wx/spinctrl.h>
#include <wx/checkbox.h>
#include <wx/dialog.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/filepicker.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
//*)

#include "PipelinePlugin.h"
#include "plugins/ArucoColor.h"

class DialogArucoColor: public wxDialog
{
	public:

    std::vector<ArucoColor*> plugin;
    void SetPlugin (std::vector<PipelinePlugin*> pfv);

		DialogArucoColor(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~DialogArucoColor();

		//(*Declarations(DialogArucoColor)
		wxSpinCtrl* SpinCtrlMaxMarkerRange;
		wxSpinCtrl* SpinCtrlAtBlockSize;
		wxCheckBox* CheckBoxOutput;
		wxButton* ButtonUpdateDictionary;
		wxStaticText* StaticText1;
		wxStaticText* StaticText10;
		wxSpinCtrl* SpinCtrlMinArea;
		wxSpinCtrl* SpinCtrlMarkerCols;
		wxStaticText* StaticText3;
		wxSpinCtrl* SpinCtrlSaturationThreshold;
		wxButton* ButtonOk;
		wxSpinCtrl* SpinCtrlMaxArea;
		wxStaticText* StaticText8;
		wxStaticText* StaticText12;
		wxStaticText* StaticText7;
		wxSpinCtrl* SpinCtrlValueThreshold;
		wxStaticText* StaticText4;
		wxStaticText* StaticText5;
		wxStaticText* StaticText2;
		wxStaticText* StaticText6;
		wxTextCtrl* TextCtrlDictionary;
		wxTextCtrl* TextCtrlRefHues;
		wxFilePickerCtrl* FilePickerCtrl1;
		wxButton* ButtonUpdateRefHues;
		wxStaticText* StaticText9;
		wxSpinCtrl* SpinCtrlAtConstant;
		wxStaticText* StaticText11;
		wxSpinCtrl* SpinCtrlMaxHueDeviation;
		wxSpinCtrl* SpinCtrlMarkerRows;
		//*)

	protected:

		//(*Identifiers(DialogArucoColor)
		static const long ID_STATICTEXT1;
		static const long ID_SPINCTRL1;
		static const long ID_STATICTEXT2;
		static const long ID_SPINCTRL2;
		static const long ID_STATICTEXT3;
		static const long ID_TEXTCTRL1;
		static const long ID_BUTTON2;
		static const long ID_STATICTEXT4;
		static const long ID_TEXTCTRL2;
		static const long ID_BUTTON3;
		static const long ID_STATICTEXT5;
		static const long ID_SPINCTRL4;
		static const long ID_STATICTEXT6;
		static const long ID_SPINCTRL3;
		static const long ID_STATICTEXT7;
		static const long ID_SPINCTRL5;
		static const long ID_STATICTEXT8;
		static const long ID_SPINCTRL6;
		static const long ID_STATICTEXT9;
		static const long ID_SPINCTRL7;
		static const long ID_STATICTEXT10;
		static const long ID_SPINCTRL8;
		static const long ID_STATICTEXT11;
		static const long ID_SPINCTRL9;
		static const long ID_STATICTEXT12;
		static const long ID_SPINCTRL10;
		static const long ID_CHECKBOX1;
		static const long ID_FILEPICKERCTRL1;
		static const long ID_BUTTON1;
		//*)

	private:

		//(*Handlers(DialogArucoColor)
		void OnButtonCancelClick(wxCommandEvent& event);
		void OnButtonApplyClick(wxCommandEvent& event);
		void OnButtonOkClick(wxCommandEvent& event);
		void OnSpinCtrlArucoColorSizeChange(wxSpinEvent& event);
		void OnKeyDown(wxKeyEvent& event);
		void OnSpinCtrlMarkerColsChange(wxSpinEvent& event);
		void OnSpinCtrlMarkerRowsChange(wxSpinEvent& event);
		void OnSpinCtrlAtBlockSizeChange(wxSpinEvent& event);
		void OnSpinCtrlAtConstantChange(wxSpinEvent& event);
		void OnSpinCtrlSaturationThresholdChange(wxSpinEvent& event);
		void OnSpinCtrlValueThresholdChange(wxSpinEvent& event);
		void OnSpinCtrlMinAreaChange(wxSpinEvent& event);
		void OnSpinCtrlMaxAreaChange(wxSpinEvent& event);
		void OnSpinCtrlMaxHueDeviationChange(wxSpinEvent& event);
		void OnSpinCtrlMaxMarkerRangeChange(wxSpinEvent& event);
		void OnButtonUpdateRefHuesClick(wxCommandEvent& event);
		void OnButtonUpdateDictionaryClick(wxCommandEvent& event);
		void OnCheckBoxOutputClick(wxCommandEvent& event);
		void OnFilePickerCtrl1FileChanged(wxFileDirPickerEvent& event);
		//*)

		DECLARE_EVENT_TABLE()
};

#endif
