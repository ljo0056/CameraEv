#ifndef RTSP_CALLBACK_H
#define RTSP_CALLBACK_H

#include <liveMedia/include/liveMedia.hh>
#include <BasicUsageEnvironment/include/BasicUsageEnvironment.hh>

// RTSP 'response handlers':
void continueAfterDESCRIBE( RTSPClient* rtspClient, int resultCode, char* resultString );
void continueAfterSETUP( RTSPClient* rtspClient, int resultCode, char* resultString );
void continueAfterPLAY( RTSPClient* rtspClient, int resultCode, char* resultString );

void subsessionAfterPlaying( void* clientData ); // called when a stream's subsession (e.g., audio or video substream) ends
void subsessionByeHandler( void* clientData ); // called when a RTCP "BYE" is received for a subsession
void streamTimerHandler( void* clientData );

// Used to iterate through each stream's 'subsessions', setting up each one:
void setupNextSubsession( RTSPClient* rtspClient );
void shutdownStream( RTSPClient* rtspClient );

#ifdef _DEBUG_RTSP_PACKET
struct rtp_pkt_minimum_s {
   unsigned short flags;
   unsigned short seqnum;
   unsigned int timestamp;
   unsigned int ssrc;
};
using rtp_pkt_minimum_t = rtp_pkt_minimum_s;
void rtp_packet_handler( void* clientData, unsigned char* packet, unsigned& packetSize );
#endif // _DEBUG_RTSP_PACKET

#endif // RTSP_CALLBACK_H
