#ifndef GDALALGCORE_H 
#define GDALALGCORE_H

#include <limits>
#include <string>
#include <vector>
#include <complex>
using namespace std;

#include "gdal_priv.h"
#include "gdal_alg_priv.h"
#include "gdalwarper.h"
#include "ogrsf_frmts.h"
#include "gdal.h"

/**
* @brief 导出符号定义
*/
#define GDALALG_EXPORTS
#ifndef GDALALG_EXPORTS
#define GDALALG_API __declspec(dllimport)
#else
#define GDALALG_API __declspec(dllexport)
#endif

/*!成功执行*/
const int RE_SUCCESS = 0;

/*!文件不存在*/
const int RE_FILENOTEXIST = 1;

/*!文件格式不被支持*/
const int RE_FILENOTSUPPORT = 2;

/*!图像数据类型不正确*/
const int RE_FILETYPEERROR = 3;

/*!创建图像失败*/
const int RE_CREATEFAILED = 4;

/*!输入参数错误*/
const int RE_PARAMERROR = 5;

/*!其他错误*/
const int RE_FAILED = 6;

// 常用的数据类型定义：
/*!byte*/
typedef unsigned char	byte;

/*!8U*/
typedef unsigned char	DT_8U;

/*!16U*/
typedef unsigned short	DT_16U;

/*!16S*/
typedef short			DT_16S;

/*!32U*/
typedef unsigned int	DT_32U;

/*!32S*/
typedef int				DT_32S;

/*!32F*/
typedef float			DT_32F;

/*!64F*/
typedef double			DT_64F;

/*!32CF*/
typedef complex<float>	DT_32CF;

/*!64CF*/
typedef complex<double>	DT_64CF;

//其他常用的宏定义：
/**
*@brief释放数组
*/
#define RELEASE(x)	if(x!=NULL){delete[]x;x=NULL;}

/**
*@brief进度条基类
*
*提供进度条基类接口，来反映当前算法的进度值
*/
class GDALALG_API CProcessBase
{
public:
	/**
	*@brief构造函数
	*/
	CProcessBase()
	{
		m_dPosition = 0.0;
		m_iStepCount = 100;
		m_iCurStep = 0;
		m_bIsContinue = true;
	}

	/**
	*@brief析构函数
	*/
	virtual~CProcessBase(){}

	/**
	*@brief设置进度信息
	*@parampszMsg			进度信息
	*/
	virtual void SetMessage(const char *pszMsg) = 0;

	/**
	*@brief设置进度值
	*@paramdPosition		进度值
	*@return 返回是否取消的状态，true为不取消，false为取消
	*/
	virtual bool SetPosition(double dPosition) = 0;

	/**
	*@brief进度条前进一步，返回true表示继续，false表示取消
	*@return 返回是否取消的状态，true为不取消，false为取消
	*/
	virtual bool StepIt() = 0;

	/**
	*@brief设置进度个数
	*@paramiStepCount		进度个数
	*/
	virtual void SetStepCount(int iStepCount)
	{
		ReSetProcess();
		m_iStepCount = iStepCount;
	}

	/**
	*@brief获取进度信息
	*@return 返回当前进度信息
	*/
	string GetMessage()
	{
		return m_strMessage;
	}

	/**
	*@brief获取进度值
	*@return 返回当前进度值
	*/
	double GetPosition()
	{
		return m_dPosition;
	}

	/**
	*@brief重置进度条
	*/
	void ReSetProcess()
	{
		m_dPosition = 0.0;
		m_iStepCount = 100;
		m_iCurStep = 0;
		m_bIsContinue = true;
	}

	/*!进度信息*/
	string m_strMessage;
	/*!进度值*/
	double m_dPosition;
	/*!进度个数*/
	int m_iStepCount;
	/*!进度当前个数*/
	int m_iCurStep;
	/*!是否取消，值为false时表示计算取消*/
	bool m_bIsContinue;
};

/**
*@brief控制台进度条类
*
*提供控制台程序的进度条类接口，来反映当前算法的进度值
*/
class CConsoleProcess : public CProcessBase
{
public:
	/**
	*@brief构造函数
	*/
	CConsoleProcess()
	{
		m_dPosition = 0;
		m_iStepCount = 100;
		m_iCurStep = 0;
	};

	/**
	*@brief析构函数
	*/
	~CConsoleProcess()
	{
		//remove(m_pszFile);
	};

	/**
	*@brief设置进度信息
	*@parampszMsg			进度信息
	*/
	void SetMessage(const char* pszMsg)
	{
		m_strMessage = pszMsg;
		printf("%s\n", pszMsg);
	}

	/**
	*@brief设置进度值
	*@paramdPosition		进度值
	*@return返回是否取消的状态，true为不取消，false为取消
	*/
	bool SetPosition(double dPosition)
	{
		m_dPosition = dPosition;
		TermProgress(m_dPosition);
		m_bIsContinue = true;
		return true;
	}

	/**
	*@brief进度条前进一步
	*@return返回是否取消的状态，true为不取消，false为取消
	*/
	bool StepIt()
	{
		m_iCurStep++;
		m_dPosition = m_iCurStep*1.0 / m_iStepCount;

		TermProgress(m_dPosition);
		m_bIsContinue = true;
		return true;
	}

private:
	void TermProgress(double dfComplete)
	{
		static int nLastTick = -1;
		int nThisTick = (int)(dfComplete*40.0);

		nThisTick = MIN(40, MAX(0, nThisTick));

		if (nThisTick < nLastTick && nLastTick >= 39)
			nLastTick = -1;

		if (nThisTick <= nLastTick)
			return;

		while (nThisTick > nLastTick)
		{
			nLastTick++;
			if (nLastTick % 4 == 0)
				fprintf(stdout, "%d", (nLastTick / 4) * 10);
			else
				fprintf(stdout, ".");
		}

		if (nThisTick == 40)
			fprintf(stdout, "-done.\n");
		else
			fflush(stdout);
	}
};

/**
*@brief导出符号定义
*/
#ifndef STD_API
#define STD_API __stdcall
#endif


#endif //GDALALGCORE_H