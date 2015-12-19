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
* @brief �������Ŷ���
*/
#define GDALALG_EXPORTS
#ifndef GDALALG_EXPORTS
#define GDALALG_API __declspec(dllimport)
#else
#define GDALALG_API __declspec(dllexport)
#endif

/*!�ɹ�ִ��*/
const int RE_SUCCESS = 0;

/*!�ļ�������*/
const int RE_FILENOTEXIST = 1;

/*!�ļ���ʽ����֧��*/
const int RE_FILENOTSUPPORT = 2;

/*!ͼ���������Ͳ���ȷ*/
const int RE_FILETYPEERROR = 3;

/*!����ͼ��ʧ��*/
const int RE_CREATEFAILED = 4;

/*!�����������*/
const int RE_PARAMERROR = 5;

/*!��������*/
const int RE_FAILED = 6;

// ���õ��������Ͷ��壺
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

//�������õĺ궨�壺
/**
*@brief�ͷ�����
*/
#define RELEASE(x)	if(x!=NULL){delete[]x;x=NULL;}

/**
*@brief����������
*
*�ṩ����������ӿڣ�����ӳ��ǰ�㷨�Ľ���ֵ
*/
class GDALALG_API CProcessBase
{
public:
	/**
	*@brief���캯��
	*/
	CProcessBase()
	{
		m_dPosition = 0.0;
		m_iStepCount = 100;
		m_iCurStep = 0;
		m_bIsContinue = true;
	}

	/**
	*@brief��������
	*/
	virtual~CProcessBase(){}

	/**
	*@brief���ý�����Ϣ
	*@parampszMsg			������Ϣ
	*/
	virtual void SetMessage(const char *pszMsg) = 0;

	/**
	*@brief���ý���ֵ
	*@paramdPosition		����ֵ
	*@return �����Ƿ�ȡ����״̬��trueΪ��ȡ����falseΪȡ��
	*/
	virtual bool SetPosition(double dPosition) = 0;

	/**
	*@brief������ǰ��һ��������true��ʾ������false��ʾȡ��
	*@return �����Ƿ�ȡ����״̬��trueΪ��ȡ����falseΪȡ��
	*/
	virtual bool StepIt() = 0;

	/**
	*@brief���ý��ȸ���
	*@paramiStepCount		���ȸ���
	*/
	virtual void SetStepCount(int iStepCount)
	{
		ReSetProcess();
		m_iStepCount = iStepCount;
	}

	/**
	*@brief��ȡ������Ϣ
	*@return ���ص�ǰ������Ϣ
	*/
	string GetMessage()
	{
		return m_strMessage;
	}

	/**
	*@brief��ȡ����ֵ
	*@return ���ص�ǰ����ֵ
	*/
	double GetPosition()
	{
		return m_dPosition;
	}

	/**
	*@brief���ý�����
	*/
	void ReSetProcess()
	{
		m_dPosition = 0.0;
		m_iStepCount = 100;
		m_iCurStep = 0;
		m_bIsContinue = true;
	}

	/*!������Ϣ*/
	string m_strMessage;
	/*!����ֵ*/
	double m_dPosition;
	/*!���ȸ���*/
	int m_iStepCount;
	/*!���ȵ�ǰ����*/
	int m_iCurStep;
	/*!�Ƿ�ȡ����ֵΪfalseʱ��ʾ����ȡ��*/
	bool m_bIsContinue;
};

/**
*@brief����̨��������
*
*�ṩ����̨����Ľ�������ӿڣ�����ӳ��ǰ�㷨�Ľ���ֵ
*/
class CConsoleProcess : public CProcessBase
{
public:
	/**
	*@brief���캯��
	*/
	CConsoleProcess()
	{
		m_dPosition = 0;
		m_iStepCount = 100;
		m_iCurStep = 0;
	};

	/**
	*@brief��������
	*/
	~CConsoleProcess()
	{
		//remove(m_pszFile);
	};

	/**
	*@brief���ý�����Ϣ
	*@parampszMsg			������Ϣ
	*/
	void SetMessage(const char* pszMsg)
	{
		m_strMessage = pszMsg;
		printf("%s\n", pszMsg);
	}

	/**
	*@brief���ý���ֵ
	*@paramdPosition		����ֵ
	*@return�����Ƿ�ȡ����״̬��trueΪ��ȡ����falseΪȡ��
	*/
	bool SetPosition(double dPosition)
	{
		m_dPosition = dPosition;
		TermProgress(m_dPosition);
		m_bIsContinue = true;
		return true;
	}

	/**
	*@brief������ǰ��һ��
	*@return�����Ƿ�ȡ����״̬��trueΪ��ȡ����falseΪȡ��
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
*@brief�������Ŷ���
*/
#ifndef STD_API
#define STD_API __stdcall
#endif


#endif //GDALALGCORE_H