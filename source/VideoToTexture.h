// sudo apt install ffmpeg libavfilter-dev libavdevice-dev
// https://ffmpeg.org/doxygen/trunk/decode_video_8c-example.html
#ifndef VIDEO_TO_TEXTURE_H__
#define VIDEO_TO_TEXTURE_H__

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <inttypes.h>
}

#include "TinyTools.h"

#include <string>

class VideoToTexture
{
public:
    VideoToTexture();
    ~VideoToTexture();

    bool Open(const std::string& pFilename);

private:
    const static int  H264_INBUF_SIZE  = 16384; // number of bytes we read per chunk
    uint8_t inbuf[H264_INBUF_SIZE + FF_INPUT_BUFFER_PADDING_SIZE];   

    struct
    {
        uint8_t *pixels = nullptr;
        int width = 0;
        int height = 0;
        std::mutex mFrameLock;
    }mCurrentFrame;

    int width, height;
    AVPacket *pkt = nullptr;
    const AVCodec *codec = nullptr;
    AVCodecParserContext *parser = nullptr;
    AVCodecContext *codec_context = nullptr;
    FILE *mFile = nullptr;
    AVFrame *frame = nullptr;

    tinytools::threading::SleepableThread  mFrameRender;

    void decode(AVCodecContext *dec_ctx, AVFrame *frame, AVPacket *pkt);
    void UpdateFrame(AVFrame *frame);
};

#endif // VIDEO_TO_TEXTURE_H__