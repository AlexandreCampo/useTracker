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
#include <wx/dialog.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/stattext.h>
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
		wxSpinCtrl* SpinCtrlArucoColorSize;
		wxStaticText* StaticText1;
		wxButton* ButtonOk;
		//*)

	protected:

		//(*Identifiers(DialogArucoColor)
		static const long ID_STATICTEXT1;
		static const long ID_SPINCTRL1;
		static const long ID_BUTTON1;
		//*)

	private:

		//(*Handlers(DialogArucoColor)
		void OnButtonCancelClick(wxCommandEvent& event);
		void OnButtonApplyClick(wxCommandEvent& event);
		void OnButtonOkClick(wxCommandEvent& event);
		void OnSpinCtrlArucoColorSizeChange(wxSpinEvent& event);
		void OnKeyDown(wxKeyEvent& event);
		//*)

		DECLARE_EVENT_TABLE()
};

#endif
