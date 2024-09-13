#ifndef STREAM_CLIENT_STATE_H
#define STREAM_CLIENT_STATE_H

#include <liveMedia/include/liveMedia.hh>
#include <BasicUsageEnvironment/include/BasicUsageEnvironment.hh>

// Define a class to hold per-stream state that we maintain throughout each stream's lifetime:

class StreamClientState {
public:
   StreamClientState();
   virtual ~StreamClientState();

public:
   MediaSubsessionIterator* iter;
   MediaSession*            session;
   MediaSubsession*         subsession;
   TaskToken                streamTimerTask;

   /// \brief duration
   /// live rtsp camera : 0 play infinite sec
   /// wowza rtsp server : remain end of playing-time. sec + 2
   double duration;
};

#endif // STREAM_CLIENT_STATE_H
