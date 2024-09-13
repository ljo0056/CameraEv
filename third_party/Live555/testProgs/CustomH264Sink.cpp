void CustomH264Sink::afterGettingFrame(void* clientData, unsigned frameSize, unsigned numTruncatedBytes, struct timeval presentationTime, unsigned durationInMicroseconds)
{
    CustomH264Sink* sink = (CustomH264Sink*)clientData;
    /* HERE I DISCOVERED THAT sink byte buffer does not contain an IFRAME */
    sink->afterGettingFrame(frameSize, numTruncatedBytes, presentationTime, durationInMicroseconds);
}

void CustomH264Sink::afterGettingFrame(unsigned frameSize, unsigned numTruncatedBytes, struct timeval presentationTime, unsigned /*durationInMicroseconds*/)
{
    // We've just received a frame of data. (Optionally) print out information about it:

    if (fSubsession.rtpSource()->curPacketMarkerBit())
    {
        m_nFrameSize += frameSize;
        int nNALType = fReceiveBuffer[H264_NAL_HEADER_STARTCODE_LENGTH] & 0x1F;

        bool bNalHeaderPresent = false;
        if ((nNALType == 0) && (m_nFrameSize >= H264_NAL_HEADER_STARTCODE_LENGTH + H264_NAL_HEADER_STARTCODE_LENGTH))
        { // Most of the cameras sending the frames without nal header start code, however some cameras send the frame with start code
        // Since we initialized the buffer with start code, we need to skip that.
        // We are reinitilzing without nal header start code, we will execute this code only for the first frame
            nNALType = fReceiveBuffer[H264_NAL_HEADER_STARTCODE_LENGTH + H264_NAL_HEADER_STARTCODE_LENGTH] & 0x1F;

            // First make sure we have valid NAL Type
            if (nNALType == NALTYPE::NALTYPE_IDRPicture || nNALType == NALTYPE::NALTYPE_SEI || nNALType == NALTYPE::NALTYPE_SequenceParameterSet
                || nNALType == NALTYPE::NALTYPE_PictureParameterSet || nNALType == NALTYPE::NALTYPE_AccessUnitDelimiter || nNALType == NALTYPE::NALTYPE_SliceLayerWithoutPartitioning)
            {
                memmove(fReceiveBuffer, fReceiveBuffer + H264_NAL_HEADER_STARTCODE_LENGTH, m_nFrameSize - H264_NAL_HEADER_STARTCODE_LENGTH);
                bNalHeaderPresent = true;
            }
        }

        if (nNALType == NALTYPE::NALTYPE_IDRPicture)
        {
            envir() << "I Frame (" << m_nFrameWidth << "x" << m_nFrameHeight << ") " << m_nFrameSize + frameSize << "\n";
            m_nFrameCounter = 0;

            if (m_nConfigLength)
                m_pCamera->Add2FrameQueue(CC_SAMPLETYPE_MPEG4AVC_CONFIG, m_pConfig, m_nConfigLength, m_nFrameWidth, m_nFrameHeight, m_nFrameCounter);
            m_pCamera->Add2FrameQueue(CC_SAMPLETYPE_MPEG4AVC_IFRAME, fReceiveBuffer, m_nFrameSize, m_nFrameWidth, m_nFrameHeight, m_nFrameCounter);
            DisplayDiagnosticsInfo(m_nFrameSize);
            CalculateFrameRate();
            CalculateIFrameInterval(true);
        }
        else if (nNALType == NALTYPE::NALTYPE_SEI)
        {
            //envir() << "SEI " << m_nFrameSize << "\n";
        }
        else if (nNALType == NALTYPE::NALTYPE_SequenceParameterSet)
        {
            // Some Cameras send I frame with Config Data , process those cases
            unsigned long nEndOfConfigData = m_nFrameSize + 1;
            unsigned long nBytesToCheck = m_nFrameSize - 5;
            BYTE nNALTypeSPS;
            const unsigned char* pData = (const unsigned char*)fReceiveBuffer;

            // Special Case 1
            for (unsigned long n = 0; n < nBytesToCheck; ++n)
            {
                if (pData[0] == 0x00 && pData[1] == 0x00 && pData[2] == 0x00 && pData[3] == 0x01)
                {
                    nNALTypeSPS = pData[4] & 0x1F;

                    if ((nNALTypeSPS == NALTYPE_IDRPicture) || (nNALTypeSPS == NALTYPE_SliceLayerWithoutPartitioning))
                    { // Frame Data is started
                        nEndOfConfigData = n;
                        break;
                    }
                }

                ++pData;
            }




            if (nEndOfConfigData < m_nFrameSize)
            {
                m_nFrameCounter = 0;

                if (m_nConfigLength)
                    m_pCamera->Add2FrameQueue(CC_SAMPLETYPE_MPEG4AVC_CONFIG, fReceiveBuffer, nEndOfConfigData, m_nFrameWidth, m_nFrameHeight, m_nFrameCounter);
                m_pCamera->Add2FrameQueue(CC_SAMPLETYPE_MPEG4AVC_IFRAME, fReceiveBuffer + nEndOfConfigData, m_nFrameSize - nEndOfConfigData, m_nFrameWidth, m_nFrameHeight, m_nFrameCounter);
                DisplayDiagnosticsInfo(m_nFrameSize);
                CalculateFrameRate();
                CalculateIFrameInterval(true);
            }
            else
            { // This is normal case, most of the cameras
                if (m_pConfig == NULL && m_nFrameSize)
                {
                    m_nConfigLength = m_nFrameSize;
                    m_pConfig = new uint8_t[m_nConfigLength];
                    memcpy(m_pConfig, fReceiveBuffer, m_nConfigLength);
                }
            }
            // Diag
            //envir() << "SPS " << m_nFrameSize << "\n";
        }
        else if (nNALType == NALTYPE::NALTYPE_PictureParameterSet)
        {

        }
        else if (nNALType == NALTYPE::NALTYPE_AccessUnitDelimiter)
        {
            //envir() << "AUD " << m_nFrameSize << "\n";
        }
        else if (nNALType == NALTYPE::NALTYPE_SliceLayerWithoutPartitioning)
        {
            //envir() << "P Frame " << m_nFrameSize << "\n";
            m_nFrameCounter++;
            m_pCamera->Add2FrameQueue(CC_SAMPLETYPE_MPEG4AVC_PFRAME, fReceiveBuffer, m_nFrameSize, m_nFrameWidth, m_nFrameHeight, m_nFrameCounter);
            CalculateFrameRate();
            CalculateIFrameInterval(false);
        }
        else
        {
            envir() << "Unrecognizable DATA ............................................................. \n";
            envir() << nNALType;
            bNalHeaderPresent = false;
        }

        //envir() << m_nFrameSize << "\n";

        if (bNalHeaderPresent)
        { // Reinitilize the offset, since this camera is sending nal header start code
            m_nNalHeaderStartCodeOffset = 0;
        }

        m_nFrameSize = m_nNalHeaderStartCodeOffset;
    }
    else
    {
        m_nFrameSize += frameSize;

        // 08/19/2015 ODD Case, Major IP Camera Vendor's firmware sends SEI data without rtp market set, Needs to investigate further
        int nNALType = fReceiveBuffer[H264_NAL_HEADER_STARTCODE_LENGTH] & 0x1F;

        if (nNALType == NALTYPE::NALTYPE_SEI)
        {
            // Ignore the packet, we don't need SEI motion detection data
            m_nFrameSize = H264_NAL_HEADER_STARTCODE_LENGTH;
            envir() << "SEI without Marker " << m_nFrameSize << "
        }

        //envir() << "Incomplete Data....................... \n";
    }



    // Then continue, to request the next frame of data:
    continuePlaying();
}

Boolean CustomH264Sink::continuePlaying()
{
    if (fSource == NULL) return False; // sanity check (should not happen)

    // Request the next frame of data from our input source. "afterGettingFrame()" will get called later, when it arrives:

#if 1
    fSource->getNextFrame(fReceiveBuffer + m_nFrameSize, fReceiveBufferLength - m_nFrameSize,
        afterGettingFrame, this,
        onSourceClosure, this);
#else
    fSource->getNextFrame(fReceiveBuffer, fReceiveBufferLength,
        afterGettingFrame, this,
        onSourceClosure, this);

#endif


    return True;
}