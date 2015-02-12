/*=============================================================================
  Copyright (C) 2012 Allied Vision Technologies.  All Rights Reserved.

  Redistribution of this file, in original or modified form, without
  prior written consent of Allied Vision Technologies is prohibited.

-------------------------------------------------------------------------------

  File:        ApiController.h

  Description: Implementation file for the ApiController helper class that
               demonstrates how to implement an asynchronous, continuous image
               acquisition with VimbaCPP.

-------------------------------------------------------------------------------

  THIS SOFTWARE IS PROVIDED BY THE AUTHOR "AS IS" AND ANY EXPRESS OR IMPLIED
  WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF TITLE,
  NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A PARTICULAR  PURPOSE ARE
  DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
  AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
  TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

=============================================================================*/

#ifndef AVT_VMBAPI_EXAMPLES_APICONTROLLER
#define AVT_VMBAPI_EXAMPLES_APICONTROLLER

#include <string>

#include <VimbaCPP/Include/VimbaCPP.h>

#include "CameraObserver.h"
#include "FrameObserver.h"

#include<opencv2/highgui/highgui.hpp>

namespace AVT {
namespace VmbAPI {

class ApiController
{
  public:
    ApiController();
    ~ApiController();

    VmbErrorType        StartUp();
    void                ShutDown();

    VmbErrorType        StartContinuousImageAcquisition( const std::string &rStrCameraID );
    VmbErrorType        StopContinuousImageAcquisition();

    int                 GetWidth();
    int                 GetHeight();
    VmbPixelFormatType  GetPixelFormat();
    CameraPtrVector     GetCameraList();
    FramePtr            GetFrame();
    bool GetFrame(cv::Mat& m);
    bool FrameAvailable();
    unsigned int GetQueueFrameSize();
    VmbErrorType        QueueFrame( FramePtr pFrame );
    void                ClearFrameQueue();

    CameraObserver*            GetCameraObserver();
    FrameObserver*            GetFrameObserver();

    std::string         ErrorCodeToMessage( VmbErrorType eErr ) const;
    std::string         GetVersion() const;

  private:
    // A reference to our Vimba singleton
    VimbaSystem&                m_system;
    // The currently streaming camera
    CameraPtr                   m_pCamera;
    // Every camera has its own frame observer
    IFrameObserverPtr           m_pFrameObserver;
    // Our camera observer
    ICameraListObserverPtr      m_pCameraObserver;
    // The current pixel format
    VmbInt64_t                  m_nPixelFormat;
    // The current width
    VmbInt64_t                  m_nWidth;
    // The current height
    VmbInt64_t                  m_nHeight;
};

}} // namespace AVT::VmbAPI::Examples

#endif
