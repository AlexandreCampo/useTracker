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

#ifndef DIALOGIMAGEBACKGROUNDDIFFGMG_H
#define DIALOGIMAGEBACKGROUNDDIFFGMG_H

//(*Headers(DialogBackgroundDiffGMG)
#include <wx/spinctrl.h>
#include <wx/checkbox.h>
#include <wx/dialog.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/stattext.h>
#include <wx/radiobox.h>
//*)

#include "PipelinePlugin.h"
#include "plugins/BackgroundDiffGMG.h"

class DialogBackgroundDiffGMG: public wxDialog
{
	public:

    std::vector<BackgroundDiffGMG*> plugin;
    void SetPlugin (std::vector<PipelinePlugin*> pfv);

		DialogBackgroundDiffGMG(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~DialogBackgroundDiffGMG();

		//(*Declarations(DialogBackgroundDiffGMG)
		wxCheckBox* CheckBoxRestrictToZone;
		wxSpinCtrl* SpinCtrlBackgroundDiffGMGLearningRate;
		wxSpinCtrl* SpinCtrlZone;
		wxButton* ButtonOk;
		wxRadioBox* RadioBoxOperator;
		wxStaticText* StaticText5;
		wxStaticText* StaticText6;
		//*)

		wxSpinCtrlDouble* SpinCtrlDoubleBackgroundDiffGMGBackgroundRatio;
		wxSpinCtrlDouble* SpinCtrlDoubleBackgroundDiffGMGNoiseSigma;
		wxSpinCtrlDouble* SpinCtrlDoubleBackgroundDiffGMGLearningRate;

		
	protected:

		//(*Identifiers(DialogBackgroundDiffGMG)
		static const long ID_STATICTEXT5;
		static const long ID_SPINCTRL_LEARNINGRATE;
		static const long ID_RADIOBOX2;
		static const long ID_CHECKBOX1;
		static const long ID_STATICTEXT6;
		static const long ID_SPINCTRL_ZONE;
		static const long ID_BUTTON1;
		//*)

		static const long ID_SPINCTRLD_BGRATIO;
		static const long ID_SPINCTRLD_NOISESIGMA;
		static const long ID_SPINCTRLD_LEARNINGRATE;

	private:

		//(*Handlers(DialogBackgroundDiffGMG)
		void OnButtonCancelClick(wxCommandEvent& event);
		void OnButtonApplyClick(wxCommandEvent& event);
		void OnButtonOkClick(wxCommandEvent& event);
		void OnSpinCtrlBackgroundDiffGMGSizeChange(wxSpinEvent& event);
		void OnKeyDown(wxKeyEvent& event);
		void OnSpinCtrlBackgroundDiffGMGLearningRateChange(wxSpinDoubleEvent& event);
		void OnCheckBoxRestrictToZoneClick(wxCommandEvent& event);
		void OnSpinCtrlZoneChange(wxSpinEvent& event);
		void OnRadioBoxOperatorSelect(wxCommandEvent& event);
		//*)

		DECLARE_EVENT_TABLE()
};

#endif
