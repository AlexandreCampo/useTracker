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

#ifndef DIALOGSAFEEROSION_H
#define DIALOGSAFEEROSION_H

//(*Headers(DialogSafeErosion)
#include <wx/dialog.h>
#include <wx/sizer.h>
#include <wx/button.h>
//*)

#include "PipelinePlugin.h"
#include "plugins/SafeErosion.h"

class DialogSafeErosion: public wxDialog
{
	public:

    std::vector<SafeErosion*> plugin;
    void SetPlugin (std::vector<PipelinePlugin*> pfv);

		DialogSafeErosion(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~DialogSafeErosion();

		//(*Declarations(DialogSafeErosion)
		wxButton* buttonOk;
		//*)

	protected:

		//(*Identifiers(DialogSafeErosion)
		static const long ID_BUTTON2;
		//*)

	private:

		//(*Handlers(DialogSafeErosion)
		void OnButtonCancelClick(wxCommandEvent& event);
		void OnButtonApplyClick(wxCommandEvent& event);
		void OnButtonOkClick(wxCommandEvent& event);
		void OnSpinCtrlSafeErosionSizeChange(wxSpinEvent& event);
		void OnKeyDown(wxKeyEvent& event);
		//*)

		DECLARE_EVENT_TABLE()
};

#endif
