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

#ifndef DIALOG_FRAME_DIFFERENCE_H
#define DIALOG_FRAME_DIFFERENCE_H

//(*Headers(DialogFrameDifference)
#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/dialog.h>
#include <wx/radiobox.h>
#include <wx/sizer.h>
#include <wx/spinctrl.h>
#include <wx/stattext.h>
//*)

#include "PipelinePlugin.h"
#include "plugins/FrameDifference.h"

class DialogFrameDifference: public wxDialog
{
	public:

    std::vector<FrameDifference*> plugin;
    void SetPlugin (std::vector<PipelinePlugin*> pfv);

		DialogFrameDifference(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~DialogFrameDifference();

		//(*Declarations(DialogFrameDifference)
		wxButton* ButtonOk;
		wxCheckBox* CheckBoxRestrictToZone;
		wxRadioBox* RadioBoxMethod;
		wxRadioBox* RadioBoxOperator;
		wxSpinCtrl* SpinCtrlThreshold;
		wxSpinCtrl* SpinCtrlZone;
		wxStaticText* StaticText1;
		wxStaticText* StaticText3;
		//*)

	protected:

		//(*Identifiers(DialogFrameDifference)
		static const long ID_RADIOBOX1;
		static const long ID_STATICTEXT1;
		static const long ID_SPINCTRL1;
		static const long ID_RADIOBOX2;
		static const long ID_CHECKBOX1;
		static const long ID_STATICTEXT3;
		static const long ID_SPINCTRL_ZONE;
		static const long ID_BUTTON1;
		//*)

	private:

		//(*Handlers(DialogFrameDifference)
		void OnButtonOkClick(wxCommandEvent& event);
		void OnKeyDown(wxKeyEvent& event);
		void OnRadioBoxMethodSelect(wxCommandEvent& event);
		void OnSpinCtrlBlockSizeChange(wxSpinEvent& event);
		void OnSpinCtrlConstantChange(wxSpinEvent& event);
		void OnSpinCtrlZoneChange(wxSpinEvent& event);
		void OnCheckBoxRestrictToZoneClick(wxCommandEvent& event);
		void OnRadioBoxOperatorSelect(wxCommandEvent& event);
		void OnCheckBox1Click(wxCommandEvent& event);
		void OnCheckBoxInvertClick(wxCommandEvent& event);
		//*)

		DECLARE_EVENT_TABLE()
};

#endif
