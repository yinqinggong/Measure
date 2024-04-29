#include "pch.h"
#include "RtspDevice.h"
#include <assert.h>
#include "LogFile.h"
#include "common.h"

const int g_reconectTimes = 1;//rtsp重连次数，外部有定时重连，所以1次

static int interrupt_cb(void *context)
{
	CRtspDevice *device = (CRtspDevice *)context;
	if (!device)
	{
		return 0;
	}
	if (device->m_tStart == 0)
	{
		return 0;
	}
	int curTime = time(NULL);
	int timeout = curTime - device->m_tStart;
	if (timeout > 3)  //timeout arrived?
	{
		return 1;
	}
	else
	{
		//return 0 for no timeout
		return 0;
	}
}

static double r2d(AVRational r)
{
	return r.num == 0 || r.den == 0 ? 0. : (double)r.num / (double)r.den;
}

void av_packet_rescale_ts(AVPacket *pkt, AVRational src_tb, AVRational dst_tb)
{
	if (pkt->pts != AV_NOPTS_VALUE)
		pkt->pts = av_rescale_q(pkt->pts, src_tb, dst_tb);
	if (pkt->dts != AV_NOPTS_VALUE)
		pkt->dts = av_rescale_q(pkt->dts, src_tb, dst_tb);
	if (pkt->duration > 0)
		pkt->duration = av_rescale_q(pkt->duration, src_tb, dst_tb);
}
int CRtspDevice::WritePacket(AVPacket &packet)
{
	if (ofmt_ctx_)
	{
		av_packet_rescale_ts(&packet, src_tb_, dst_tb_);
		return av_interleaved_write_frame(ofmt_ctx_, &packet);
	}
	return -1;
}

void CRtspDevice::Decode()
{
	int ret = -1;
	AVPacket pkt, packet;
	while (!bStopPkt_)
	{
		if (m_pktQueue.empty())
		{
			Sleep(20);
			continue;
		}
		{
			CTempLock tempLock(&m_queueLock);
#ifdef _WIN64	
#else
			av_init_packet(&pkt);
#endif 
			pkt = m_pktQueue.front();
			m_pktQueue.pop_front();
		}

		WriteLog(_T("pop - size():%d"), m_pktQueue.size());
		if (pkt.stream_index == video_st_index_)
		{
			//存视频文件
			//if (CheckStorageFile())
			//{
			//	//ffmpeg7.0-64编译不错先注释
			//	//av_copy_packet(&packet, &pkt);
			//	WritePacket(packet);
			//}
			ret = avcodec_send_packet(avctx_, &pkt);
			if (ret < 0)
			{
				WriteLog(_T("avcodec_send_packet:ret<0"));
				break;
			}
			while (ret >= 0 && !bStopPkt_)
			{
				ret = avcodec_receive_frame(avctx_, src_frame_);
				if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
					break;
				}
				else if (ret < 0) {
					//av_log(NULL, AV_LOG_ERROR, "Error while receiving a frame from the decoder\n");
					break;
				}
				if (!sws_context_) {
					sws_context_ = sws_getContext(avctx_->width, avctx_->height,
						avctx_->pix_fmt, avctx_->width, avctx_->height, AV_PIX_FMT_YUV420P,
						SWS_BICUBIC, NULL, NULL, NULL);
				}
				if (!buffer_) {
					int buf_size = av_image_get_buffer_size(AV_PIX_FMT_YUV420P,
						avctx_->width, avctx_->height, 1);
					buffer_ = new unsigned char[buf_size];
					memset(buffer_, 0, buf_size);
					av_image_fill_arrays(dst_frame_->data, dst_frame_->linesize, buffer_,
						AV_PIX_FMT_YUV420P, avctx_->width, avctx_->height, 1);
				}
				sws_scale(sws_context_, (const uint8_t* const*)src_frame_->data,
					src_frame_->linesize, 0, avctx_->height,
					dst_frame_->data, dst_frame_->linesize);
				//窗口绘制视频回调
				if (m_pD3DHwnd_)
				{
					m_pD3DHwnd_->PlayVideo(dst_frame_->data[0], dst_frame_->data[2], dst_frame_->data[1], avctx_->height, avctx_->width);
				}
			}
			av_packet_unref(&pkt);
		}
	}
	bPlaying_ = false;
}
UINT CRtspDevice::DecodeFrame(LPVOID lpParam)
{
	CRtspDevice *pDecode = (CRtspDevice*)lpParam;
	pDecode->Decode();
	return 1;
}

UINT CRtspDevice::ReceivePacket(LPVOID lpParam)
{
	CRtspDevice *pDecode = (CRtspDevice*)lpParam;

	if (pDecode->OpenInput() < 0)
	{
		WriteLog(_T("OpenInput failed"));
		//播放结束，显示按钮
		if (pDecode->m_noPlayCB)
		{
			pDecode->m_noPlayCB(pDecode->main_user_, pDecode->m_devInfo.index, true);
		}
		pDecode->bPlaying_ = false;
		pDecode->bStopPkt_ = true;
		return -1;
	}

	AVPacket pkt;
	int ret;
	while (!pDecode->bStopPkt_)
	{
		do {
#ifdef _WIN64	
#else
			av_init_packet(&pkt);
#endif 
			pDecode->m_tStart = time(NULL);
			ret = av_read_frame(pDecode->ifmt_ctx_, &pkt); 
			pDecode->m_tStart = 0;// read frames
		} while (ret == AVERROR(EAGAIN));
		if (ret < 0)
		{
			WriteLog(_T("开始重连，ret=%d"), ret);
			while (pDecode->ReOpenInput() < 0)
			{
				if (++pDecode->m_reconectTimes >= g_reconectTimes) break;
			}
			if (pDecode->m_reconectTimes >= g_reconectTimes)
			{
				WriteLog(_T("重连了1次失败，退出"));
				//播放结束，显示按钮
				if (pDecode->m_noPlayCB && !pDecode->bStopPkt_)
				{
					pDecode->m_noPlayCB(pDecode->main_user_, pDecode->m_devInfo.index, true);
				}
				pDecode->bStopPkt_ = true;
				pDecode->bPlaying_ = false;
				break;
			}
			else
			{
				WriteLog(_T("重连成功"));
				pDecode->m_reconectTimes = 0;
				continue;
			}
		}
		{
			CTempLock  tempLock(&pDecode->m_queueLock);
			//如果收数据太快则丢帧
			if (pDecode->m_pktQueue.size() > 3000)
			{
				WriteLog(_T("队列数据超过300，开始丢帧"));
				AVPacket tempPkt = pDecode->m_pktQueue.front();
				pDecode->m_pktQueue.pop_front();
				av_packet_unref(&tempPkt);
			}
			pDecode->m_pktQueue.push_back(pkt);
		}
		WriteLog(_T("receive - size():%d"), pDecode->m_pktQueue.size());
	}
	pDecode->bPlaying_ = false;
	return 1;
}

CRtspDevice::CRtspDevice(DeviceInfo devInfo, NoPlayCB noPlayCB)
: codec_(NULL),
avctx_(NULL),
src_frame_(NULL),
dst_frame_(NULL),
sws_context_(NULL),
buffer_(NULL),
ifmt_ctx_(NULL),
ofmt_ctx_(NULL)
{
	video_st_index_ = -1;
	bStopPkt_ = false;
	bPlaying_ = false;
	m_pktThread = NULL;
	m_hPktThreadHandle = NULL;
	m_decodeThread = NULL;
	m_hDecodeThreadHandle = NULL;
	m_pD3DHwnd_ = NULL;
	m_curWeek = -1;
	m_curHour = -1;
	m_devInfo = devInfo;
	memset(m_videoFile, 0, 256);
	//m_reconecting = 0;
	m_reconectTimes = 0;
	memset(&src_tb_, 0, sizeof(AVRational));
	memset(&dst_tb_, 0, sizeof(AVRational));
	m_tStart = 0;
	m_noPlayCB = noPlayCB;
	Create();
}

CRtspDevice::~CRtspDevice() {

	Destroy();
	if (m_pD3DHwnd_)
	{
		delete m_pD3DHwnd_;
		m_pD3DHwnd_ = NULL;
	}
}

bool CRtspDevice::Create() {
	bool ret = false;

	do {
#ifdef _WIN64	
#else
		avcodec_register_all();
		av_register_all(); 
#endif                                                          // Register all codecs and formats so that they can be used.
		avformat_network_init();

		src_frame_ = av_frame_alloc();
		if (!src_frame_)
			break;
		dst_frame_ = av_frame_alloc();
		if (!dst_frame_)
			break;

		ret = true;
	} while (0);

	if (!ret) {
		Destroy();
	}

	return ret;
}

void CRtspDevice::Destroy()
{
	{
		CTempLock tempLock(&m_queueLock);
		AVPacket pkt;
		while (!(m_pktQueue.empty())) {
			if (m_pktQueue.empty())  break;
			pkt = m_pktQueue.front();
			m_pktQueue.pop_front();
			av_packet_unref(&pkt);
		}
	}

	if (src_frame_) {
		av_frame_free(&src_frame_); // ffmpeg 2.1+
	}

	if (dst_frame_) {
		av_frame_free(&dst_frame_); // ffmpeg 2.1+
	}

	if (buffer_) {
		delete[] buffer_;
		buffer_ = NULL;
	}

	if (sws_context_) {
		sws_freeContext(sws_context_);
		sws_context_ = NULL;
	}

	if (avctx_ != NULL) {
		/*if (avctx_->extradata != NULL) {
		free(avctx_->extradata);
		avctx_->extradata = NULL;
		}*/

#ifdef _WIN64	
		avcodec_free_context(&avctx_);
#else
		avcodec_close(avctx_);
#endif  
		av_free(avctx_);
		avctx_ = NULL;
	}

	if (NULL != ifmt_ctx_) {
		avformat_close_input(&ifmt_ctx_);
		ifmt_ctx_ = NULL;
	}
	if (NULL != ofmt_ctx_) {
		avformat_close_input(&ofmt_ctx_);
		ofmt_ctx_ = NULL;
	}
	avformat_network_deinit();
}

bool CRtspDevice::StartThread()
{
	bStopPkt_ = false;
	bPlaying_ = true;
	//开启解码线程
	m_decodeThread = AfxBeginThread(DecodeFrame, (LPVOID)this);
	if (m_decodeThread == NULL)
	{
		return false;
	}
	m_hDecodeThreadHandle = m_decodeThread->m_hThread;

	//开启收流线程
	m_pktThread = AfxBeginThread(ReceivePacket, (LPVOID)this);
	if (m_pktThread == NULL)
	{
		return false;
	}
	m_hPktThreadHandle = m_pktThread->m_hThread;

	return true;
}

int CRtspDevice::ReOpenInput()
{
	if (NULL != ifmt_ctx_) {
		avformat_close_input(&ifmt_ctx_);
		ifmt_ctx_ = NULL;
	}
	return OpenInput();
}
int CRtspDevice::OpenInput()
{
	ifmt_ctx_ = avformat_alloc_context();
	AVDictionary* options = nullptr;
	//av_dict_set(&options, "buffer_size", "102400", 0); //设置缓存大小，1080p可将值调大
	av_dict_set(&options, "rtsp_transport", "tcp", 0);
	av_dict_set(&options, "stimeout", "3000000", 0); //设置超时断开连接时间，单位微秒
	av_dict_set(&options, "max_delay", "500000", 0); //设置最大时延
	ifmt_ctx_->interrupt_callback.opaque = this;
	ifmt_ctx_->interrupt_callback.callback = interrupt_cb;//
	m_tStart = time(NULL);
	int ret = avformat_open_input(&ifmt_ctx_, m_devInfo.cameras[m_devInfo.channel].rtsp, nullptr, &options);
	m_tStart = 0;
	if (ret < 0)
	{
		//av_strerror();
		WriteLog(_T("avformat_open_input failed"));
		av_log(NULL, AV_LOG_ERROR, "Input file open input failed\n");
		return  ret;
	}

	//设置探寻信息的时间和字节长度
	ifmt_ctx_->probesize = 500 * 1024;
	ifmt_ctx_->max_analyze_duration = 5 * AV_TIME_BASE;
	ret = avformat_find_stream_info(ifmt_ctx_, nullptr);
	if (ret < 0)
	{
		WriteLog(_T("Find input file stream inform failed"));
		av_log(NULL, AV_LOG_ERROR, "Find input file stream inform failed\n");
	}
	else
	{
		WriteLog(_T("Find input file stream inform success"));
		av_log(NULL, AV_LOG_FATAL, "Open input file  %s success\n", m_devInfo.cameras[m_devInfo.channel].rtsp);
	}
	//codec_ = avcodec_find_decoder(AV_CODEC_ID_H264);
	//if (!codec_) return -1;
	ret = av_find_best_stream(ifmt_ctx_, AVMEDIA_TYPE_VIDEO, -1, -1, &codec_, 0);
	if (ret < 0) return ret;
	video_st_index_ = ret;

	/* create decoding context */
	avctx_ = avcodec_alloc_context3(codec_);
	if (!avctx_) return -1;
	//avctx_->codec_id = AV_CODEC_ID_H264;
	
	avctx_->codec_type = AVMEDIA_TYPE_VIDEO;
	avctx_->pix_fmt = AV_PIX_FMT_YUV420P;
	//if (codec_->capabilities & CODEC_CAP_TRUNCATED)
	//	avctx_->flags |= CODEC_FLAG_TRUNCATED;

	avcodec_parameters_to_context(avctx_, ifmt_ctx_->streams[video_st_index_]->codecpar);
	av_opt_set_int(avctx_, "refcounted_frames", 1, 0);
	//codec_ = avcodec_find_decoder(avctx_->codec_id);
	/* init the video decoder */
	if ((ret = avcodec_open2(avctx_, codec_, NULL)) < 0) {
		//av_log(NULL, AV_LOG_ERROR, "Cannot open video decoder\n");
		//DWORD err = GetLastError();
		WriteLog(_T("avcodec_open2 failed"));
		return ret;
	}

	auto inputStream = ifmt_ctx_->streams[video_st_index_];
	if (inputStream)
	{
		src_tb_ = inputStream->time_base;
	}
	return ret;
}
int CRtspDevice::OpenOutput(std::string outUrl)
{
	//int ret = avformat_alloc_output_context2(&ofmt_ctx_, nullptr, "mpegts", outUrl.c_str());
	int ret = avformat_alloc_output_context2(&ofmt_ctx_, nullptr, "flv", outUrl.c_str());

	if (ret < 0)
	{
		WriteLog(_T("open output context failed"));
		av_log(NULL, AV_LOG_ERROR, "open output context failed\n");
		goto Error;
	}

	ret = avio_open2(&ofmt_ctx_->pb, outUrl.c_str(), AVIO_FLAG_WRITE, nullptr, nullptr);
	if (ret < 0)
	{
		WriteLog(_T("avio_open2 failed"));
		av_log(NULL, AV_LOG_ERROR, "open avio failed");
		goto Error;
	}

	//for (int i = 0; i < ifmt_ctx_->nb_streams; i++)
	//{

	//AVCodecContext *codec_ctx = avcodec_alloc_context3(in_codec);
	/*ret = avcodec_parameters_to_context(avctx_, ifmt_ctx_->streams[video_st_index_]->codecpar);
	if (ret < 0){
	printf("Failed to copy in_stream codecpar to codec context\n");
	}*/
	AVStream * stream = avformat_new_stream(ofmt_ctx_, codec_);
	ret = avcodec_parameters_from_context(stream->codecpar, avctx_);
	if (ret < 0){
		printf("Failed to copy codec context to out_stream codecpar context\n");
	}

	/*AVStream * stream = avformat_new_stream(ofmt_ctx_, ifmt_ctx_->streams[i]->codec->codec);
	ret = avcodec_copy_context(stream->codec, ifmt_ctx_->streams[i]->codec);
	if (ret < 0)
	{
	av_log(NULL, AV_LOG_ERROR, "copy coddec context failed");
	goto Error;
	}*/
	//}
	//ofmt_ctx_->streams[video_st_index_]->codecpar->codec_type = AVMEDIA_TYPE_VIDEO;
	ret = avformat_write_header(ofmt_ctx_, nullptr);
	if (ret < 0)
	{
		av_log(NULL, AV_LOG_ERROR, "format write header failed");
		goto Error;
	}

	av_log(NULL, AV_LOG_FATAL, " Open output file success %s\n", outUrl.c_str());

	auto outputStream = ofmt_ctx_->streams[0];
	if (outputStream)
	{
		dst_tb_ = outputStream->time_base;
	}

	return ret;
Error:
	if (ofmt_ctx_)
	{
		//avformat_free_context(ofmt_ctx_);
		avformat_close_input(&ofmt_ctx_);
		ofmt_ctx_ = NULL;
	}
	return ret;
}
int CRtspDevice::CloseOutput()
{
	if (ofmt_ctx_)
	{
		//avformat_free_context(ofmt_ctx_);
		avformat_close_input(&ofmt_ctx_);
		ofmt_ctx_ = NULL;
	}
	return 1;
}

int CRtspDevice::StartRealPlay(HWND hWnd)
{
	bPlaying_ = true;
	if (!m_pD3DHwnd_ && hWnd)
	{
		m_pD3DHwnd_ = new CHwnd();
		m_pD3DHwnd_->InitHwnd(hWnd);
	}

	//if (OpenInput() < 0)
	//{
	//	WriteLog(_T("OpenInput failed"));
	//	//播放结束，显示按钮
	//	if (m_noPlayCB)
	//	{
	//		m_noPlayCB(main_user_, m_devInfo.index, true);
	//	}
	//	bPlaying_ = false;
	//	return -1;
	//}
	StartThread();
	if (m_noPlayCB)
	{
		m_noPlayCB(main_user_, m_devInfo.index, false);
	}
	return 1;
}
int CRtspDevice::StopRealPlay()
{
	bStopPkt_ = true;
	bPlaying_ = false;

	//关闭解码线程
	if (m_hDecodeThreadHandle != INVALID_HANDLE_VALUE)
	{
		WaitAndTermThread(m_hDecodeThreadHandle, 500);
		m_hDecodeThreadHandle = INVALID_HANDLE_VALUE;
		m_decodeThread = NULL;
	}

	if (m_hPktThreadHandle != INVALID_HANDLE_VALUE)
	{
		WaitAndTermThread(m_hPktThreadHandle, 500);
		m_hPktThreadHandle = INVALID_HANDLE_VALUE;
		m_pktThread = NULL;
	}
	return 0;
}
bool CRtspDevice::CheckStorageFile()
{
	int hour, min, sec;
	time_t nowtime = time(0);
	tm curtime;
	localtime_s(&curtime, &nowtime);
	hour = curtime.tm_hour;
	min = curtime.tm_min;
	sec = curtime.tm_sec;
	if (curtime.tm_wday != m_curWeek)
	{
		m_curWeek = curtime.tm_wday;
		m_curHour = hour;	
		WriteLog(_T("隔天-StartNewFile"));

		return StartNewFile(curtime, true);
	}
	else if (hour != m_curHour)
	{
		m_curHour = hour;
		WriteLog(_T("隔小时-StartNewFile"));
		return StartNewFile(curtime, false);
	}
	/*else if (m_reconecting == 1)
	{
	m_reconecting = 0;
	StartNewFile(curtime, false);
	WriteLog(_T("重连-StartNewFile"));
	}*/
	else
	{
		return CheckDirectory(m_videoFile);
	}

	return 0;
}

bool CRtspDevice::StartNewFile(tm &curtime, bool newDay)
{
	int year = curtime.tm_year + 1900;
	int mon = curtime.tm_mon + 1;
	int day = curtime.tm_mday;
	int hour = curtime.tm_hour;
	int min = curtime.tm_min;
	int sec = curtime.tm_sec;

	sprintf_s(m_videoFile, "%s\\%04d%02d%02d\\%02d%02d%02d_%02d5959.ts", m_devInfo.cameras[m_devInfo.channel].recordPath, year, mon, day, hour, min, sec, hour);

	if (CheckDirectory(m_videoFile)) {
		CloseOutput();
		OpenOutput(m_videoFile);
		return true;
	}
	else
	{
		CString strVideoFile;
		AnsiToUnicode(m_videoFile, strVideoFile);
		WriteLog(_T("录像目录不存在，m_videoFile=%s"), strVideoFile);
	}
	return false;
}

bool CRtspDevice::GetbPlaying()
{
	return 	bPlaying_ ;
}

int CRtspDevice::GetIndex()
{
	return m_devInfo.index;
}
int CRtspDevice::GetDeviceID()
{
	return m_devInfo.device_id;
}