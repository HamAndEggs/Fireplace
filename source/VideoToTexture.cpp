
#include "VideoToTexture.h"

#include <iostream>



VideoToTexture::VideoToTexture()
{

}

VideoToTexture::~VideoToTexture()
{
    fclose(mFile);

    av_parser_close(parser);
    avcodec_free_context(&c);
    av_frame_free(&frame);
    av_packet_free(&pkt);
}

bool VideoToTexture::Open(const std::string& pFilename)
{
    codec = avcodec_find_decoder(AV_CODEC_ID_H264);
    if(!codec) {
        std::cerr << "Error: cannot find the h264 codec: " << filepath.c_str() <<< "\n";
        return false;
    }

    codec_context = avcodec_alloc_context3(codec);

    if(codec->capabilities & CODEC_CAP_TRUNCATED)
    {
        codec_context->flags |= CODEC_FLAG_TRUNCATED;
    }

    if(avcodec_open2(codec_context, codec, NULL) < 0)
    {
        std::cerr << "Error: could not open codec.\n";
        return false;
    }

    mFile = fopen(filepath.c_str(), "rb");

    if(!mFile)
    {
        std::cerr << "Error: cannot open: " << filepath.c_str() << "\n";
        return false;
    }

    picture = av_frame_alloc();
    parser = av_parser_init(AV_CODEC_ID_H264);

    if(!parser)
    {
        std::cerr << "Erorr: cannot create H264 parser.\n";
        return false;
    }

    mFrameRender.TickMS(1000 / 25,[this](){
        bool keepGoing = true;
                /* read raw data from the input file */
        size_t data_size = fread(inbuf, 1, INBUF_SIZE, f);
        if (ferror(f))
            keepGoing = false;

        int eof = !data_size;
 
        /* use the parser to split the data into frames */
        uint8_t *data = inbuf;
        while ( (data_size > 0 || eof) && keepGoing ) {

            const int bytes_read = (int)fread(inbuf, 1, H264_INBUF_SIZE, mFile);
            if(bytes_read) {
                std::copy(inbuf, inbuf + bytes_read, std::back_inserter(buffer));
            }

            int ret = av_parser_parse2(parser, c, &pkt->data, &pkt->size,
                                   data, data_size, AV_NOPTS_VALUE, AV_NOPTS_VALUE, 0);
            if (ret < 0)
            {
                std::cerr << "Error while parsing\n";
                keepGoing = false;
            }
            else
            {
                data      += ret;
                data_size -= ret;
    
                if (pkt->size)
                {
                    decode(c, frame, pkt);
                }
                else if (eof)
                {
                    // loop
    //                fseek(f,0,SEEK_SET);
                }
            }
        }
    });

    return true;
}

void VideoToTexture::decode(AVCodecContext *dec_ctx, AVFrame *frame, AVPacket *pkt)
{
    int ret;
 
    ret = avcodec_send_packet(dec_ctx, pkt);
    if (ret < 0) {
        std::cerr << "Error sending a packet for decoding\n";
        return;
    }
 
    while (ret >= 0) {
        ret = avcodec_receive_frame(dec_ctx, frame);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
            return;
        else if (ret < 0) {
            std::cerr << "Error during decoding\n";
            return;
        }
        UpdateFrame(frame);
    }
}

void VideoToTexture::UpdateFrame(AVFrame *frame)
{
    // the picture is allocated by the decoder. no need to free it
    const uint8_t* srcPixels = frame->data[0];
    const int lineSize = std::abs(frame->linesize);
    const int width = frame->width;
    const int height = frame->height;
    std::unique_lock<std::mutex> lk(mSleeperMutex);
    if( mCurrentFrame.pixels == nullptr || mCurrentFrame.width != width || mCurrentFrame.height != height )
    {
        delete []mCurrentFrame.pixels;
        mCurrentFrame.pixels = new uint8_t[4 * width * height];
    }
    mCurrentFrame.width = width;
    mCurrentFrame.height = height;

    for(int y = 0 ; y < height ; y++ )
    {
        memcpy(mCurrentFrame.pixels,srcPixels,width * 4);
    }

}

