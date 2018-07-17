#ifndef ZEGOMediaPlayer_h
#define ZEGOMediaPlayer_h
#include "video_format.h"

namespace AVE
{
    class IMediaPlayer
    {
    public:
        enum PlayerType
        {
            TYPE_PLAYER = 0,    //only play
            TYPE_AUX            //play and mix into publish stream.
        };
        
        class EventCallback
        {
        public:
            virtual void OnPlayStart() = 0;
            virtual void OnPlayError(int code) = 0;
            virtual void OnVideoBegin() = 0;
            virtual void OnAudioBegin() = 0;
            virtual void OnPlayEnd() = 0;
            virtual void OnSeekComplete(int code, long timestamp_ms) = 0;    //Units in millisecond
        };
        
        class VideoPlayCallback
        {
        public:
            virtual void OnPlayVideoData(const char* data, int len, const VideoDataFormat& format) = 0;
        };

        virtual void SetEventCallback(EventCallback* callback) = 0;
		virtual void SetVolume(int volume) = 0;
        virtual void Start(const char* path, bool repeat_play = false) = 0;// repeat_play: play repeat or not
        virtual void Stop() = 0;
        virtual void Pause() = 0;
        virtual void Resume() = 0;
        virtual void SeekTo(long timestamp_ms) = 0;                          //Units in millisecond
        virtual long GetDuration() = 0;
        virtual long GetCurrentDuration() = 0;
        
        virtual void SetView(void *view) = 0;
        //format support:ARGB32/ABGR32/RGBA32/BGRA32
        virtual void SetVideoPlayCallback(VideoPlayCallback *callback, VideoPixelFormat format) = 0;
    };
}

#endif /* ZEGOMediaPlayer_h */
