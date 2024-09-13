#include "stdafx.h"
#include "rtsp_callback.h"

#include "rtsp_StreamClientState.h"
#include "rtsp_client.h"
#include "rtsp_H264Sink.h"
#include "rtsp_callback.h"

#include <groupsock/include/GroupsockHelper.hh>

namespace plog
{
    // A function that outputs a string that identifies each stream (for debugging output).  Modify this if you wish:
    Record& operator<<(Record& record, const RTSPClient& rtspClient)
    {
        return record << "[URL:" << rtspClient.url() << "]: ";
    }

    Record& operator<<(Record& record, const CustomRTSPClient& rtspClient)
    {
        return record << "[URL:" << rtspClient.url() << "]: ";
    }
}

// A function that outputs a string that identifies each subsession (for debugging output).  Modify this if you wish:
UsageEnvironment& operator<<(UsageEnvironment& env, const MediaSubsession& subsession)
{
    return env << subsession.mediumName() << "/" << subsession.codecName();
}
std::ostream& operator<<(std::ostream& stream, const MediaSubsession& subsession)
{
    return stream << subsession.mediumName() << "/" << subsession.codecName();
}

void continueAfterDESCRIBE(RTSPClient* rtspClient, int resultCode, char* resultString)
{
    CustomRTSPClient* customClient = reinterpret_cast<CustomRTSPClient*>(rtspClient);

    do {
        //LOGI << *customClient << "continueAfterDESCRIBE";
        UsageEnvironment& env = rtspClient->envir(); // alias
        StreamClientState& scs = customClient->getStreamClientState(); // alias

        if (resultCode != 0) {
            //LOGE << *rtspClient << "Failed to get a SDP description: " << resultString << "\n";
            delete[] resultString;
            break;
        }

        char* const sdpDescription = resultString;
        //LOGD << *rtspClient << "=== Got a SDP description ===\n"
        //    << sdpDescription << "\n=== end of a SDP description ===";

        // Create a media session object from this SDP description:
        scs.session = MediaSession::createNew(env, sdpDescription);
        delete[] sdpDescription; // because we don't need it anymore
        if (scs.session == nullptr) {
            //LOGE << *rtspClient << "Failed to create a MediaSession object from the SDP description: " << env.getResultMsg() << "\n";
            break;
        }
        else if (!scs.session->hasSubsessions()) {
            //LOGE << *rtspClient << "This session has no media subsessions (i.e., no \"m=\" lines)\n";
            break;
        }

        // Then, create and set up our data source objects for the session.  We do this by iterating over the session's 'subsessions',
        // calling "MediaSubsession::initiate()", and then sending a RTSP "SETUP" command, on each one.
        // (Each 'subsession' will have its own data source.)
        scs.iter = new MediaSubsessionIterator(*scs.session);
        setupNextSubsession(rtspClient);
        return;
    } while (0);

    // An unrecoverable error occurred with this stream.
    // shutdownStream( rtspClient );

    // reconnect
    customClient->getSession()->Reconnect();
}

void continueAfterSETUP(RTSPClient* rtspClient, int resultCode, char* resultString)
{
    CustomRTSPClient* customClient = reinterpret_cast<CustomRTSPClient*>(rtspClient);

    do {
        //LOGI << *customClient << "continueAfterSETUP";
        UsageEnvironment& env = rtspClient->envir(); // alias
        StreamClientState& scs = customClient->getStreamClientState(); // alias

        if (resultCode != 0) {
            //LOGE << *rtspClient << "Failed to set up the \"" << *scs.subsession << "\" subsession: " << resultString << "\n";
            break;
        }

        //LOGI << *rtspClient << "Set up the SubSession[" << *scs.subsession << " port: " << scs.subsession->clientPortNum();
        if (scs.subsession->rtcpIsMuxed()) {
            //LOGI << *rtspClient << "and " << scs.subsession->clientPortNum() + 1;
        }

        // Having successfully setup the subsession, create a data sink for it, and call "startPlaying()" on it.
        // (This will prepare the data sink to receive data; the actual flow of data from the client won't start happening until later,
        // after we've sent a RTSP "PLAY" command.)

        //LOGD << *rtspClient << "Created a data sink for the \"" << *scs.subsession << "\" subsession";
        if (strcmp(scs.subsession->mediumName(), "video") == 0) {

            if (strcmp(scs.subsession->codecName(), "H264") == 0) {

                // For H.264 video stream, we use a special sink that insert start_codes:
                scs.subsession->sink = H264Sink::createNew(
                    env, customClient->getSession(), *scs.subsession, rtspClient->url());

                if (scs.subsession->sink == nullptr) {
                    //LOGE << *rtspClient << "Failed to create a data sink for the \"" << *scs.subsession << "\" subsession: " << env.getResultMsg() << "\n";
                    continue;
                }
                //LOGI << *rtspClient << "Created a data sink for the \"" << *scs.subsession << "\" subsession\n";
                //LOGD << fmt::format("Frame Rate:{0}  VideoResolution:{1}x{2}", scs.subsession->videoFPS(), scs.subsession->videoWidth(), scs.subsession->videoHeight());

                // a hack to let subsession handler functions get the "RTSPClient" from the subsession
                scs.subsession->miscPtr = rtspClient;
                scs.subsession->sink->startPlaying(*(scs.subsession->readSource()), subsessionAfterPlaying, scs.subsession);

                // Also set a handler to be called if a RTCP "BYE" arrives for this subsession:
                if (scs.subsession->rtcpInstance() != nullptr) {
                    scs.subsession->rtcpInstance()->setByeHandler(subsessionByeHandler, scs.subsession);
                }

            }
            else if (strcmp(scs.subsession->codecName(), "MP4V-ES") == 0) {
                // create MPEG4ES sink.
            }
            else if (strcmp(scs.subsession->codecName(), "JPEG") == 0) {
                // create jpeg sink.
            }

        }
        else if (strcmp(scs.subsession->mediumName(), "audio") == 0) {
            // create audio sink.
        }
        else if (strcmp(scs.subsession->mediumName(), "application") == 0) {
            // create meta sink.
        }

    } while (0);

    delete[] resultString;

    // Set up the next subsession, if any:
    setupNextSubsession(rtspClient);
}

void continueAfterPLAY(RTSPClient* rtspClient, int resultCode, char* resultString)
{
    Boolean success = false;
    CustomRTSPClient* customClient = reinterpret_cast<CustomRTSPClient*>(rtspClient);

    do {
        //LOGI << *customClient << "continueAfterPLAY";
        UsageEnvironment& env  = rtspClient->envir(); // alias
        StreamClientState& scs = customClient->getStreamClientState(); // alias

        if (resultCode != 0) {
            //LOGE << *rtspClient << "Failed to start playing session: " << resultString;
            break;
        }

        // Set a timer to be handled at the end of the stream's expected duration
        // (if the stream does not already signal its endusing a RTCP "BYE"). This is optional.
        // If, instead, you want to keep the stream active
        // - e.g., so you can later'seek' back within it and do another RTSP "PLAY"
        // - then you can omit this code.
        // (Alternatively, if you don't want to receive the entire stream, you could set this timer for some shorter value.)
        if (scs.duration > 0) {
            unsigned const delaySlop = 2; // number of seconds extra to delay, after the stream's expected duration.  (This is optional.)
            scs.duration += delaySlop;
            unsigned uSecsToDelay = unsigned(scs.duration * 1000000);
            scs.streamTimerTask = env.taskScheduler().scheduleDelayedTask(uSecsToDelay, reinterpret_cast<TaskFunc*>(streamTimerHandler), rtspClient);
        }

        std::ostringstream s;
        //LOGI << *rtspClient << "Started playing session";
        if (scs.duration > 0) {
            //LOGI << *rtspClient << " (for up to " << scs.duration << " seconds)...";
        }
        else {
            //LOGI << *rtspClient << " ( infite seconds )...";
        }

        success = true;

    } while (0);
    delete[] resultString;

    if (!success) {
        //LOGE << *rtspClient << "An unrecoverable error occurred with this stream.";
        shutdownStream(rtspClient);
    }
}

void setupNextSubsession(RTSPClient* rtspClient)
{
    //LOGI << *rtspClient << "setupNextSubsession";

    CustomRTSPClient* customClient = reinterpret_cast<CustomRTSPClient*>(rtspClient);
    UsageEnvironment& env  = rtspClient->envir(); // alias    
    StreamClientState& scs = customClient->getStreamClientState(); // alias

    scs.subsession = scs.iter->next();
    if (scs.subsession != nullptr) {

        // NOTE(yhpark): only subscribe vidoe/H264.
        if ((strcmp(scs.subsession->mediumName(), "video") == 0 || strcmp(scs.subsession->codecName(), "H264") == 0) == false) {
            //LOGW << "only folr vidoe/h264, so give up on the subsession name[" << scs.subsession->mediumName() << "] codec[" << scs.subsession->codecName() << "]";
            setupNextSubsession(rtspClient); // give up on this subsession; go to the next one
            return;
        }

        if (scs.subsession->initiate() == false) {

            //LOGE << *rtspClient << "Failed to initiate the \"" << *scs.subsession << "\" subsession: " << env.getResultMsg() << "\n";
            setupNextSubsession(rtspClient); // give up on this subsession; go to the next one

        }
        else {

            //LOGD << *rtspClient << "Initiated the \"" << *scs.subsession << "\" subsession...";
            if (scs.subsession->rtcpIsMuxed()) {
                //LOGD << *rtspClient << "client port " << scs.subsession->clientPortNum();
            }
            else {
                //LOGD << *rtspClient << "client ports " << scs.subsession->clientPortNum() << "-" << scs.subsession->clientPortNum() + 1;
            }

            //LOGD << *rtspClient << "rtpPayloadFormat:" << scs.subsession->rtpPayloadFormat();
#ifdef _DEBUG_RTSP_PACKET
            scs.subsession->rtpSource()->setAuxilliaryReadHandler(rtp_packet_handler, nullptr);
#endif // _DEBUG_RTSP_PACKET

            // yhpark rtst. add. socket.
            if (scs.subsession->rtpSource() != nullptr) {

                // Because we're saving the incoming data, rather than playing it in real time,
                // allow an especially large time threshold(1 second) for reordering misordered incoming packets:
                //unsigned const packetReorderingThresholdTime = 1000000; // 1 second

                // NOTE(yhpark): we are palying it real time. 1 -> 3 sec.
                unsigned const packetReorderingThresholdTime = 3000000; // 3 second
                scs.subsession->rtpSource()->setPacketReorderingThresholdTime(packetReorderingThresholdTime);

                // Set the RTP source's OS socket buffer size as appropriate - either if we were explicitly asked (using -B),
                // or if the desired FileSink buffer size happens to be larger than the current OS socket buffer size.
                // (The latter case is a heuristic, on the assumption that if the user asked for a large FileSink buffer size,
                // then the input data rate may be large enough to justify increasing the OS socket buffer size also.)
                int socketNum = scs.subsession->rtpSource()->RTPgs()->socketNum();
                u_int curBufferSize = getReceiveBufferSize(env, socketNum);
                //LOGD << *rtspClient << "curBufferSize:" << curBufferSize;

                // NOTE(yhpark): I don't think we need to increase tcp/udp socket buffer size.
                //if (curBufferSize < g_buffer_size) {
                //    uint newBufferSize = setReceiveBufferTo(env, socketNum, g_buffer_size);
                //    LOGW << "Changed socket receive buffer size for the \""
                //        << scs.subsession->mediumName() << "/" << scs.subsession->codecName()
                //        << "\" subsession from " << curBufferSize << " to " << g_buffer_size << " result:" << newBufferSize;
                //}
            }

            // Continue setting up this subsession, by sending a RTSP "SETUP" command:
            rtspClient->sendSetupCommand(*scs.subsession, continueAfterSETUP, false, customClient->getSession()->GetSessionInfo().use_tcp);
            }
        return;
        }

    // We've finished setting up all of the subsessions.  Now, send a RTSP "PLAY" command to start the streaming:
    if (scs.session->absStartTime() != nullptr) {

        // Special case: The stream is indexed by 'absolute' time, so send an appropriate "PLAY" command:
        rtspClient->sendPlayCommand(*scs.session, continueAfterPLAY, scs.session->absStartTime(), scs.session->absEndTime());

    }
    else {

        scs.duration = scs.session->playEndTime() - scs.session->playStartTime();
        rtspClient->sendPlayCommand(*scs.session, continueAfterPLAY);
    }
    }

// Implementation of the other event handlers:

void subsessionAfterPlaying(void* clientData)
{
    MediaSubsession* subsession = reinterpret_cast<MediaSubsession*>(clientData);
    RTSPClient* rtspClient = reinterpret_cast<RTSPClient*>(subsession->miscPtr);
    //LOGI << *rtspClient << "subsessionAfterPlaying";

    // Begin by closing this subsession's stream:
    Medium::close(subsession->sink);
    subsession->sink = nullptr;

    // Next, check whether *all* subsessions' streams have now been closed:
    MediaSession& session = subsession->parentSession();
    MediaSubsessionIterator iter(session);
    while ((subsession = iter.next()) != nullptr) {
        if (subsession->sink != nullptr) return; // this subsession is still active
    }

    // All subsessions' streams have now been closed, so shutdown the client:
    shutdownStream(rtspClient);
}

void subsessionByeHandler(void* clientData)
{
    MediaSubsession* subsession = reinterpret_cast<MediaSubsession*>(clientData);
    RTSPClient* rtspClient = reinterpret_cast<RTSPClient*>(subsession->miscPtr);
    //LOGI << *rtspClient << "subsessionByeHandler";

    //LOGD << "Received RTCP \"BYE\" on \"" << *subsession->mediumName() << "\" subsession. url:" << rtspClient->url();

    // Now act as if the subsession had closed:
    subsessionAfterPlaying(subsession);
}

void streamTimerHandler(void* clientData)
{
    CustomRTSPClient* customClient = reinterpret_cast<CustomRTSPClient*>(clientData);
    StreamClientState& scs = customClient->getStreamClientState(); // alias
    //LOGW << *customClient << "streamTimerHandler called. It could be Wowza player's file finished.";

    scs.streamTimerTask = nullptr;

    customClient->getSession()->Reconnect();
}

void shutdownStream(RTSPClient* rtspClient)
{
    //LOGD << *rtspClient << "shutdownStream";

    CustomRTSPClient* customClient = reinterpret_cast<CustomRTSPClient*>(rtspClient);
    StreamClientState& stream_client_state = customClient->getStreamClientState(); // alias

    // First, check whether any subsessions have still to be closed:
    if (stream_client_state.session != nullptr) {
        Boolean someSubsessionsWereActive = false;
        MediaSubsessionIterator iter(*stream_client_state.session);
        MediaSubsession* subsession;

        while ((subsession = iter.next()) != nullptr) {
            if (subsession->sink != nullptr) {
                Medium::close(subsession->sink);
                subsession->sink = nullptr;

                if (subsession->rtcpInstance() != nullptr) {
                    // in case the server sends a RTCP "BYE" while handling "TEARDOWN"
                    subsession->rtcpInstance()->setByeHandler(nullptr, nullptr);
                }

                someSubsessionsWereActive = true;
            }
        }

        if (someSubsessionsWereActive) {
            // Send a RTSP "TEARDOWN" command, to tell the server to shutdown the stream.
            // Don't bother handling the response to the "TEARDOWN".
            rtspClient->sendTeardownCommand(*stream_client_state.session, nullptr);
        }
    }

    //LOGD << *rtspClient << "Closing the stream.";
    Medium::close(rtspClient);
}

#ifdef _DEBUG_RTSP_PACKET
void rtp_packet_handler(void* clientData, uint8_t* packet, unsigned& packetSize)
{
    (void)clientData;

    if (packet == nullptr || packetSize < 12)
        return;

    rtp_pkt_minimum_t* rtp = reinterpret_cast<rtp_pkt_minimum_t*>(packet);

    struct timeval tv;
    gettimeofday(&tv, NULL);

    // endian converting.
    uint ssrc = ntohl(rtp->ssrc);
    ushort seqnum = ntohs(rtp->seqnum);
    ushort flags = ntohs(rtp->flags);
    uint timestamp = ntohl(rtp->timestamp);

    LOGV << "ssrc:" << ssrc << " seqnum:" << seqnum << " flags:" << flags << " timestamp:" << timestamp << " packetSize:" << packetSize;
}
#endif // _DEBUG_RTSP_PACKET
