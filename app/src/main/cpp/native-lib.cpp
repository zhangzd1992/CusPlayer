#include <jni.h>
#include <string>
#include <android/native_window_jni.h>
#include <zconf.h>

extern "C"{
    #include "libavcodec/avcodec.h"
    #include <libswscale/swscale.h>
    #include <libavutil/imgutils.h>
    #include <libavformat/avformat.h>
}



extern "C" JNIEXPORT jstring JNICALL
Java_com_example_zhangzd_cusplayer_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(av_version_info());
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_zhangzd_cusplayer_CusPlayer_start__Ljava_lang_String_2Landroid_view_Surface_2(
        JNIEnv *env, jobject instance, jstring path_, jobject surface) {
    const char *path = env->GetStringUTFChars(path_, 0);
//    // 初始化网络
    avformat_network_init();
    AVFormatContext * formatContext = avformat_alloc_context();
    AVDictionary *opts = NULL;
    av_dict_set(&opts,"timeout","3000000",0);
//    //打开视频文件
    int ret = avformat_open_input(&formatContext,path,NULL,&opts);
    avformat_find_stream_info(formatContext,NULL);

    //同时方ffmpeg 解析流
    int index = -1; //视频流的索引
    for (int i = 0; i < formatContext->nb_streams; ++i) {
        if(formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            index = i;
            break;
        }
    }
    //解析器参数
    AVCodecParameters  *codecParameters = formatContext->streams[index]->codecpar;
//    formatContext.
    AVCodec * avCodec = avcodec_find_decoder(codecParameters->codec_id);
    //获取解析器上下文
    AVCodecContext *codecContext = avcodec_alloc_context3(avCodec);
    //将解码器参数设置给解码器上下文
    avcodec_parameters_to_context(codecContext,codecParameters);

    //打开解码器
    avcodec_open2(codecContext,avCodec,NULL);

    //申请空的avPacket空间
    AVPacket * avPacket =  av_packet_alloc();


    //获取转换上下文
    SwsContext *swsContext = sws_getContext(codecContext->width,codecContext->height,codecContext->pix_fmt,
                   codecContext->width,codecContext->height,AV_PIX_FMT_RGBA,SWS_BILINEAR,0,0,0);

    ANativeWindow* aNativeWindow = ANativeWindow_fromSurface(env,surface);
    ANativeWindow_Buffer buffer;

    ANativeWindow_setBuffersGeometry(aNativeWindow,codecContext->width,codecContext->height,WINDOW_FORMAT_RGBA_8888);



    //读取一个packet复制到自定义的packet中去
    while(av_read_frame(formatContext,avPacket) >= 0) {
        avcodec_send_packet(codecContext,avPacket);
        AVFrame *frame =  av_frame_alloc();
        ret = avcodec_receive_frame(codecContext,frame);
        if(ret == AVERROR(EAGAIN)) {
            continue;
        }else if(ret < 0){
            break;
        }

        uint8_t *dst_data[0];  //每一行的数据
        int line_size[0];      //每一行的首地址

        //初始化数据数组和首地址数组
        av_image_alloc(dst_data,line_size,codecContext->width,codecContext->height,AV_PIX_FMT_RGBA,1);

        if(avPacket->stream_index == index) {
            if(ret == 0) {
                ANativeWindow_lock(aNativeWindow,&buffer,NULL);

                sws_scale(swsContext,frame->data,frame->linesize,0,frame->height,dst_data,line_size);

                //buffer 首行地址
               uint8_t *firstWindow  = static_cast<uint8_t *>(buffer.bits);
               //元数据首地址
               uint8_t * src_data = dst_data[0];
                int destStride = buffer.stride * 4; //每行的字节数
                int lineSize = line_size[0];   //数据源每行的首地址
                for (int i = 0; i < buffer.height; ++i) {
                    memcpy(firstWindow + i* destStride ,src_data + i * lineSize,destStride);
                }

                ANativeWindow_unlockAndPost(aNativeWindow);
                usleep(1000 * 16);
                av_frame_free(&frame);
            }
        }
    }

    ANativeWindow_release(aNativeWindow);
    avcodec_close(codecContext);
    avformat_free_context(formatContext);
    env->ReleaseStringUTFChars(path_, path);
}