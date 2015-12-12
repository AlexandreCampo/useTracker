/*=============================================================================
  Copyright (C) 2012 Allied Vision Technologies.  All Rights Reserved.

  Redistribution of this file, in original or modified form, without
  prior written consent of Allied Vision Technologies is prohibited.

-------------------------------------------------------------------------------

  File:        ApiController.cpp

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

#include "ApiController.h"
#include <sstream>
#include <iostream>
#include "StreamSystemInfo.h"
#include "ErrorCodeToMessage.h"

#include <AVTImageTransform/Include/VmbTransform.h>


namespace AVT {
namespace VmbAPI {

enum    { NUM_FRAMES=3, };

ApiController::ApiController()
    // Get a reference to the Vimba singleton
    : m_system( VimbaSystem::GetInstance() )
{
}

ApiController::~ApiController()
{
}

// Translates Vimba error codes to readable error messages
std::string ApiController::ErrorCodeToMessage( VmbErrorType eErr ) const
{
    return AVT::VmbAPI::ErrorCodeToMessage( eErr );
}

VmbErrorType ApiController::StartUp()
{
    VmbErrorType res;

    // Start Vimba
    res = m_system.Startup();
    if( VmbErrorSuccess == res )
    {
        // This will be wrapped in a shared_ptr so we don't delete it
        SP_SET( m_pCameraObserver , new CameraObserver() );
        // Register an observer whose callback routine gets triggered whenever a camera is plugged in or out
        res = m_system.RegisterCameraListObserver( m_pCameraObserver );
    }

    return res;
}

void ApiController::ShutDown()
{
    // Release Vimba
    m_system.Shutdown();
}
/*** helper function to set image size to a value that is dividable by modulo 2.
\note this is needed because AVTImageTransform does not support odd values for some input formats
*/
inline VmbErrorType SetValueIntMod2( const CameraPtr &camera, const std::string &featureName, VmbInt64_t &storage )
{
    VmbErrorType    res;
    FeaturePtr      pFeature;
    res = SP_ACCESS( camera )->GetFeatureByName( featureName.c_str(), pFeature );
    if( VmbErrorSuccess == res )
    {
        VmbInt64_t minValue,maxValue;
        res = SP_ACCESS( pFeature )->GetRange( minValue,maxValue );
        if( VmbErrorSuccess == res )
        {
            maxValue = ( maxValue>>1 )<<1; // mod 2 dividable
            res = SP_ACCESS( pFeature )->SetValue( maxValue );
            if( VmbErrorSuccess == res )
            {
                storage = maxValue;
            }
        }
    }
    return res;
}

VmbErrorType ApiController::StartContinuousImageAcquisition( const std::string &rStrCameraID )
{
    // Open the desired camera by its ID
    VmbErrorType res = m_system.OpenCameraByID( rStrCameraID.c_str(), VmbAccessModeFull, m_pCamera );
    if( VmbErrorSuccess == res )
    {
        // Set the GeV packet size to the highest possible value
        // (In this example we do not test whether this cam actually is a GigE cam)
        FeaturePtr pCommandFeature;
        if( VmbErrorSuccess == SP_ACCESS( m_pCamera )->GetFeatureByName( "GVSPAdjustPacketSize", pCommandFeature ) )
        {
            if( VmbErrorSuccess == SP_ACCESS( pCommandFeature )->RunCommand() )
            {
                bool bIsCommandDone = false;
                do
                {
                    if( VmbErrorSuccess != SP_ACCESS( pCommandFeature )->IsCommandDone( bIsCommandDone ) )
                    {
                        break;
                    }
                } while( false == bIsCommandDone );
            }
        }
        res = SetValueIntMod2( m_pCamera,"Width", m_nWidth );
        if( VmbErrorSuccess == res )
        {
            res = SetValueIntMod2( m_pCamera, "Height", m_nHeight );
            if( VmbErrorSuccess == res )
            {
                FeaturePtr pFormatFeature;
                // Set pixel format. For the sake of simplicity we only support Mono and RGB in this example.
                res = SP_ACCESS( m_pCamera )->GetFeatureByName( "PixelFormat", pFormatFeature );
                if( VmbErrorSuccess == res )
                {
                    // Read back the currently selected pixel format
                    SP_ACCESS( pFormatFeature )->GetValue( m_nPixelFormat );

                    if( VmbErrorSuccess == res )
                    {
                        // Create a frame observer for this camera (This will be wrapped in a shared_ptr so we don't delete it)
                        SP_SET( m_pFrameObserver , new FrameObserver( m_pCamera ) );
                        // Start streaming
                        res = SP_ACCESS( m_pCamera )->StartContinuousImageAcquisition( NUM_FRAMES,  m_pFrameObserver );
                    }
                }
            }
        }
    }

    return res;
}

VmbErrorType ApiController::StopContinuousImageAcquisition()
{
    // Stop streaming
    m_pCamera->StopContinuousImageAcquisition();

    // Close camera
    return  m_pCamera->Close();
}

CameraPtrVector ApiController::GetCameraList()
{
    CameraPtrVector cameras;
    // Get all known cameras
    if( VmbErrorSuccess == m_system.GetCameras( cameras ) )
    {
        // And return them
        return cameras;
    }
    return CameraPtrVector();
}

int ApiController::GetWidth()
{
    return (int)m_nWidth;
}

int ApiController::GetHeight()
{
    return (int)m_nHeight;
}

VmbPixelFormatType ApiController::GetPixelFormat()
{
    return (VmbPixelFormatType)m_nPixelFormat;
}

// Returns the oldest frame that has not been picked up yet
FramePtr ApiController::GetFrame()
{
    return SP_DYN_CAST( m_pFrameObserver, FrameObserver )->GetFrame();
}

// Get the oldest frame and encode data in the given Mat
bool ApiController::GetFrame(cv::Mat& m)
{
    bool success = false;

    FramePtr frame = SP_DYN_CAST( m_pFrameObserver, FrameObserver )->GetFrame();

    VmbFrameStatusType status;
    frame->GetReceiveStatus( status );

    // See if it is not corrupt
    if( status == VmbFrameStatusComplete )
    {
	unsigned char* buffer;
	VmbErrorType err = SP_ACCESS( frame )->GetImage( buffer );
	
	if( err == VmbErrorSuccess )
	{
	    VmbUint32_t size;
	    err = SP_ACCESS( frame )->GetImageSize( size );
	    
	    // Copy it
	    VmbImage            SourceImage,DestImage;
//	VmbError_t          Result;
	    SourceImage.Size    = sizeof( SourceImage );
	    DestImage.Size      = sizeof( DestImage );
	    
	    VmbSetImageInfoFromPixelFormat( m_nPixelFormat, m_nWidth, m_nHeight, &SourceImage );
	    VmbSetImageInfoFromPixelFormat( VmbPixelFormatBgr8, m_nWidth, m_nHeight, &DestImage );
	    
	    SourceImage.Data    = buffer;
	    DestImage.Data      = m.data;
	    
	    VmbImageTransform(&SourceImage, &DestImage, NULL, 0);
	    //memcpy (m.data, buffer, size);
	    
	    success = true;
	}
    }

    // done copying the frame, give it back to the camera
    SP_ACCESS( m_pCamera )->QueueFrame( frame );

    return success;
}


// Clears all remaining frames that have not been picked up
void ApiController::ClearFrameQueue()
{
    SP_DYN_CAST( m_pFrameObserver,FrameObserver )->ClearFrameQueue();
}

    bool ApiController::FrameAvailable()
    {
	return SP_DYN_CAST( m_pFrameObserver,FrameObserver )->FrameAvailable();
    }

    unsigned int ApiController::GetQueueFrameSize()
    {
	return SP_DYN_CAST( m_pFrameObserver,FrameObserver )->GetQueueFrameSize();
    }

// Queues a frame to continue streaming
VmbErrorType ApiController::QueueFrame( FramePtr pFrame )
{
    return SP_ACCESS( m_pCamera )->QueueFrame( pFrame );
}

// Returns the camera observer as QObjects to connect their signals to the view's sots
CameraObserver* ApiController::GetCameraObserver()
{
    return SP_DYN_CAST( m_pCameraObserver, CameraObserver ).get();
}

// Returns the frame observer as QObjects to connect their signals to the view's sots
FrameObserver* ApiController::GetFrameObserver()
{
    return SP_DYN_CAST( m_pFrameObserver, FrameObserver ).get();
}

std::string ApiController::GetVersion() const
{
    std::ostringstream os;
    os << m_system;
    return os.str();
}

}} // namespace AVT::VmbAPI::Examples
