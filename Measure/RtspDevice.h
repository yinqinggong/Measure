#ifndef THIRD_PARTY_FFMPEG_H264_DECODER_H_
#define THIRD_PARTY_FFMPEG_H264_DECODER_H_

#ifndef __STDC_CONSTANT_MACROS
#define __STDC_CONSTANT_MACROS
#endif

typedef void(CALLBACK *NoPlayCB)(LONG nUser, int index, bool bShow);

#include "pch.h"
#include "Device.h"
#include <deque>
#include "hwnd.h"

extern "C"{
#include "libavcodec/avcodec.h"
#include "libavdevice/avdevice.h"
#include "libavformat/avformat.h"
#include "libavfilter/avfilter.h"
#include "libavutil/avutil.h"
#include "libswscale/swscale.h"
#include "libavutil/pixdesc.h"
#include "libavutil/imgutils.h"
}

class CRtspDevice : public CDevice
{
public:
	explicit CRtspDevice(DeviceInfo devInfo, NoPlayCB noPlayCB);
	virtual ~CRtspDevice();

	bool Create();
	void Destroy();

private:
#ifdef _WIN64
	const AVCodec* codec_;
#else
	AVCodec* codec_;
#endif 

	AVCodecContext* avctx_;
	SwsContext* sws_context_;
	AVFrame* src_frame_;
	AVFrame* dst_frame_;
	AVFormatContext *ifmt_ctx_;
	AVFormatContext *ofmt_ctx_;
	AVRational src_tb_;
	AVRational dst_tb_;

	unsigned char* buffer_;
	CHwnd *m_pD3DHwnd_;
public:
	bool StartNewFile(tm &curtime, bool newDay);
	bool CheckStorageFile();
	void Decode();
	int WritePacket(AVPacket &packet);
	bool StartThread();
	int OpenInput();
	int ReOpenInput();
	int OpenOutput(std::string outUrl);
	int CloseOutput();
private:
	//解码队列
	std::deque<AVPacket>  m_pktQueue;
	CCriticalSection	m_queueLock;

	int video_st_index_;
	bool bStopPkt_;
	bool bPlaying_;

	//收流线程
	CWinThread		*m_pktThread;
	HANDLE m_hPktThreadHandle;
	static UINT ReceivePacket(LPVOID lpParam);

	//解码线程
	CWinThread		*m_decodeThread;
	HANDLE m_hDecodeThreadHandle;
	static UINT DecodeFrame(LPVOID lpParam);

public:
	virtual int StartRealPlay(HWND hWnd);
	virtual int StopRealPlay();

	bool GetbPlaying();
	int GetIndex();
	int GetDeviceID();
public:
	DeviceInfo m_devInfo; 
	int m_tStart;
private:
	char   m_videoFile[256];
	int    m_curWeek;       // 记录当前周几 隔天换文件夹
	int    m_curHour;       // 记录当前小时 隔小时换文件
	//	int    m_reconecting;   //正在重连
	int    m_reconectTimes; //重连次数
	NoPlayCB m_noPlayCB;   //算法结果回调
};

#endif // THIRD_PARTY_FFMPEG_H264_DECODER_H_

