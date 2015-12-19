#ifndef GDALALGORITHM_H 
#define GDALALGORITHM_H
#include "GDALAlgCore.h"

#ifdef __cplusplus
extern "C" {
#endif

	/**
	* @brief ͼ����β��У�ʹ��RasterIO������
	* @param pszSrcFile			�����ļ�·��
	* @param pszDstFile			����ļ�·��
	* @param iStartX			��ʼ����X
	* @param iStartY			��ʼ����Y
	* @param iSizeX				���п��
	* @param iSizeY				���и߶�
	* @param pszFormat			����ļ���ʽ����ϸ�ο�GDAL֧����������
	* @param pProcess			������ָ��
	* @return  ����ֵ����ʾ��������г��ֵĸ��ִ�����Ϣ
	*/
	int  GDALALG_API ImageCut(const char * pszSrcFile, const char * pszDstFile, int  iStartX, int  iStartY, int  iSizeX, int  iSizeY,
		const char * pszFormat = "GTiff", CProcessBase* pProcess = NULL);

	int  GDALALG_API ImageCutAuto(const char * pszSrcFile, const char * pszDstFile, int  iStartX, int  iStartY, int  iSizeX, int  iSizeY,
		const char * pszFormat = "GTiff", CProcessBase* pProcess = NULL);
	/**
	* @brief ͼ��AOI���У�ʹ��GDALWarp������
	* @param pszSrcFile			�����ļ�·��
	* @param pszDstFile			����ļ�·��
	* @param pszAOIWKT			����AOI��ʹ��WKT�ַ�����ʽ��������ͼ�����к�
	* @param pszFormat			����ļ���ʽ����ϸ�ο�GDAL֧����������
	* @param pProcess			������ָ��
	* @return  ����ֵ����ʾ��������г��ֵĸ��ִ�����Ϣ
	*/
	int  GDALALG_API ImageCutByAOI(const char * pszSrcFile, const char * pszDstFile, const char * pszAOIWKT,
		const char * pszFormat = "GTiff", CProcessBase* pProcess = NULL);

	/**
	* @brief ͼ���ز�����ʹ��RasterIO������
	* @param pszSrcFile			�����ļ�·��
	* @param pszDstFile			����ļ�·��
	* @param dResX				X����ת�������ȣ�����1ͼ����С��1ͼ���С
	* @param dResY				Y����ת�������ȣ�����1ͼ����С��1ͼ���С
	* @param pszFormat			����ļ���ʽ����ϸ�ο�GDAL֧����������
	* @param pProcess			������ָ��
	* @return  ����ֵ����ʾ��������г��ֵĸ��ִ�����Ϣ
	*/

	int  GDALALG_API ImageResample1(const char * pszSrcFile, const char * pszDstFile, double  dResX, double  dResY,
		const char * pszFormat = "GTiff", CProcessBase* pProcess = NULL);

	/**
	* @brief ͼ���ز�����ʹ��GDALWarp������
	* @param pszSrcFile			�����ļ�·��
	* @param pszDstFile			����ļ�·��
	* @param dResX				X����ת�������ȣ�����1ͼ����С��1ͼ���С
	* @param dResY				Y����ת�������ȣ�����1ͼ����С��1ͼ���С
	* @param eResampleMethod	�ز�����ʽ������ο�GDALResampleAlg�Ķ���
	* @param pszFormat			����ļ���ʽ����ϸ�ο�GDAL֧����������
	* @param pProcess			������ָ��
	* @return  ����ֵ����ʾ��������г��ֵĸ��ִ�����Ϣ
	*/
	int  GDALALG_API ImageResample2(const char * pszSrcFile, const char * pszDstFile, double  dResX, double  dResY,
		GDALResampleAlg eResampleMethod = GRA_NearestNeighbour, const char * pszFormat = "GTiff", CProcessBase* pProcess = NULL);

#ifdef __cplusplus
}
#endif

#endif /*GDALALGORITHM_H*/
