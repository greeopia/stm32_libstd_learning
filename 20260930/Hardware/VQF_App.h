#ifndef __VQF_APP_H
#define __VQF_APP_H

#include <stdint.h>

/* VQF姿态解算的集成层
 * 这个模块只依赖 vqf.c/vqf.h 和标准C库，不依赖STM32的头文件，
 * 所以同一份源码既能在Keil工程里编译，也能在PC上编译做仿真验证。
 * 输入是MPU6050的原始int16数据，输出是四元数和欧拉角（单位：度）。
 */

void VQF_App_Init(float SampleHz);										//初始化，SampleHz为真实采样率（Hz）
void VQF_App_Update(const int16_t AccRaw[3], const int16_t GyrRaw[3]);	//喂入一帧原始数据，内部完成单位换算
void VQF_App_GetQuat(float Quat[4]);									//读取四元数，顺序为w,x,y,z
void VQF_App_GetEuler(float *Roll, float *Pitch, float *Yaw);			//读取欧拉角，单位：度
float VQF_App_GetBiasDps(float BiasDps[3]);								//读取陀螺零偏估计（度/秒），返回估计不确定度（度/秒）
uint8_t VQF_App_IsRestDetected(void);									//返回1表示VQF判定当前处于静止状态
void VQF_App_GetRestDebug(float GyrLpDps[3], float *GyrDev, float *AccDev);	//读取静止检测的中间量，供调试用
float VQF_App_GetBiasClip(void);										//读取当前使用的biasClip阈值（度/秒）

#endif
