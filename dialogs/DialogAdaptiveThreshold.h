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

#ifndef DIALOG_ADAPTIVE_THRESHOLD_H
#define DIALOG_ADAPTIVE_THRESHOLD_H

//(*Headers(DialogAdaptiveThreshold)
#include <wx/spinctrl.h>
#include <wx/dialog.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/stattext.h>
#include <wx/radiobox.h>
//*)

#include "PipelinePlugin.h"
#include "plugins/AdaptiveThreshold.h"

class DialogAdaptiveThreshold: public wxDialog
{
	public:

    std::vector<AdaptiveThreshold*> plugin;
    void SetPlugin (std::vector<PipelinePlugin*> pfv);

		DialogAdaptiveThreshold(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~DialogAdaptiveThreshold();

		//(*Declarations(DialogAdaptiveThreshold)
		wxStaticText* StaticText1;
		wxRadioBox* RadioBoxMethod;
		wxButton* ButtonOk;
		wxSpinCtrl* SpinCtrlConstant;
		wxStaticText* StaticText2;
		wxSpinCtrl* SpinCtrlBlockSize;
		//*)

	protected:

		//(*Identifiers(DialogAdaptiveThreshold)
		static const long ID_RADIOBOX1;
		static const long ID_STATICTEXT1;
		static const long ID_SPINCTRL1;
		static const long ID_STATICTEXT2;
		static const long ID_SPINCTRL2;
		static const long ID_BUTTON1;
		//*)

	private:

		//(*Handlers(DialogAdaptiveThreshold)
		void OnButtonOkClick(wxCommandEvent& event);
		void OnKeyDown(wxKeyEvent& event);
		void OnRadioBoxMethodSelect(wxCommandEvent& event);
		void OnSpinCtrlBlockSizeChange(wxSpinEvent& event);
		void OnSpinCtrlConstantChange(wxSpinEvent& event);
		//*)

		DECLARE_EVENT_TABLE()
};

#endif
