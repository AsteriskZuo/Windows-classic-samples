
#pragma once;

#include <string>

#define SPDLOG_DEBUG_ON
#include "spdlog/spdlog.h"
namespace spd = spdlog;


#define _trans_buffsize 4096
struct _trans_buff_
{
	char *globe_buffer;
	wchar_t *globe_bufferw;
	_trans_buff_()
		:globe_buffer(NULL),
		globe_bufferw(NULL)
	{
		globe_buffer = new char[_trans_buffsize];
		if (globe_buffer) memset(globe_buffer, 0, _trans_buffsize*sizeof(char));
		globe_bufferw = new wchar_t[_trans_buffsize];
		if (globe_bufferw) memset(globe_bufferw, 0, _trans_buffsize*sizeof(wchar_t));
	}
	~_trans_buff_()
	{
		if (globe_buffer) delete[] globe_buffer;
		if (globe_bufferw) delete[] globe_bufferw;
	}
};
static std::auto_ptr<_trans_buff_> f_trans_buff_(new _trans_buff_);

std::string WChar2Ansi(LPCWSTR pwszSrc, int codePage = CP_ACP)
{
	int nLen = WideCharToMultiByte(codePage, 0, pwszSrc, -1, NULL, 0, NULL, NULL);
	if (nLen <= 0) return std::string("");

	char* pszDst = nullptr;
	if (nLen > _trans_buffsize)
	{
		pszDst = new char[nLen];

		if (NULL == pszDst) return std::string("");
		WideCharToMultiByte(codePage, 0, pwszSrc, -1, pszDst, nLen, NULL, NULL);
		pszDst[nLen - 1] = 0;
		std::string strTemp(pszDst);
		delete[] pszDst;
		return strTemp;
	}
	else
	{
		//boost::mutex::scoped_lock s_lock(m_mutex_tcp_Identity);
		pszDst = f_trans_buff_->globe_buffer;

		WideCharToMultiByte(codePage, 0, pwszSrc, -1, pszDst, nLen, NULL, NULL);
		pszDst[nLen - 1] = 0;
		std::string strTemp(pszDst);
		return strTemp;
	}

}




static bool isInit = false;
static std::shared_ptr<spdlog::logger> g_log;
void init_log()
{
	//建议加局域锁
	if (!isInit)
	{
		spdlog::set_level(spdlog::level::debug);
		spdlog::flush_on(spdlog::level::debug);
		g_log = spd::basic_logger_mt("basic_logger", "logs/basic-log.txt");
		isInit = true;
	}
}
void uninit_log()
{
	//建议加局域锁
	if (isInit)
	{
		spdlog::drop(g_log->name());
	}
}

void ouput_log(const std::string funname, const std::string& content)
{
	SPDLOG_DEBUG(g_log, "{} [=] {}", funname.data(), content.data());
}

void output_log_w(const std::string funname, const std::wstring& content)
{
	std::string c = WChar2Ansi(content.data());
	ouput_log(funname, c);
}