#ifndef GDALALGORITHM_H 
#define GDALALGORITHM_H
#include "GDALAlgCore.h"

#ifdef __cplusplus
extern "C" {
#endif

	/**
	* @brief 图像矩形裁切（使用RasterIO方法）
	* @param pszSrcFile			输入文件路径
	* @param pszDstFile			输出文件路径
	* @param iStartX			起始坐标X
	* @param iStartY			起始坐标Y
	* @param iSizeX				裁切宽度
	* @param iSizeY				裁切高度
	* @param pszFormat			输出文件格式，详细参考GDAL支持数据类型
	* @param pProcess			进度条指针
	* @return  返回值，表示计算过程中出现的各种错误信息
	*/
	int  GDALALG_API ImageCut(const char * pszSrcFile, const char * pszDstFile, int  iStartX, int  iStartY, int  iSizeX, int  iSizeY,
		const char * pszFormat = "GTiff", CProcessBase* pProcess = NULL);

	int  GDALALG_API ImageCutAuto(const char * pszSrcFile, const char * pszDstFile, int  iStartX, int  iStartY, int  iSizeX, int  iSizeY,
		const char * pszFormat = "GTiff", CProcessBase* pProcess = NULL);
	/**
	* @brief 图像AOI裁切（使用GDALWarp方法）
	* @param pszSrcFile			输入文件路径
	* @param pszDstFile			输出文件路径
	* @param pszAOIWKT			裁切AOI，使用WKT字符串格式，坐标是图像行列号
	* @param pszFormat			输出文件格式，详细参考GDAL支持数据类型
	* @param pProcess			进度条指针
	* @return  返回值，表示计算过程中出现的各种错误信息
	*/
	int  GDALALG_API ImageCutByAOI(const char * pszSrcFile, const char * pszDstFile, const char * pszAOIWKT,
		const char * pszFormat = "GTiff", CProcessBase* pProcess = NULL);

	/**
	* @brief 图像重采样（使用RasterIO方法）
	* @param pszSrcFile			输入文件路径
	* @param pszDstFile			输出文件路径
	* @param dResX				X方向转换采样比，大于1图像变大，小于1图像变小
	* @param dResY				Y方向转换采样比，大于1图像变大，小于1图像变小
	* @param pszFormat			输出文件格式，详细参考GDAL支持数据类型
	* @param pProcess			进度条指针
	* @return  返回值，表示计算过程中出现的各种错误信息
	*/

	int  GDALALG_API ImageResample1(const char * pszSrcFile, const char * pszDstFile, double  dResX, double  dResY,
		const char * pszFormat = "GTiff", CProcessBase* pProcess = NULL);

	/**
	* @brief 图像重采样（使用GDALWarp方法）
	* @param pszSrcFile			输入文件路径
	* @param pszDstFile			输出文件路径
	* @param dResX				X方向转换采样比，大于1图像变大，小于1图像变小
	* @param dResY				Y方向转换采样比，大于1图像变大，小于1图像变小
	* @param eResampleMethod	重采样方式，具体参考GDALResampleAlg的定义
	* @param pszFormat			输出文件格式，详细参考GDAL支持数据类型
	* @param pProcess			进度条指针
	* @return  返回值，表示计算过程中出现的各种错误信息
	*/
	int  GDALALG_API ImageResample2(const char * pszSrcFile, const char * pszDstFile, double  dResX, double  dResY,
		GDALResampleAlg eResampleMethod = GRA_NearestNeighbour, const char * pszFormat = "GTiff", CProcessBase* pProcess = NULL);

#ifdef __cplusplus
}
#endif

#endif /*GDALALGORITHM_H*/
