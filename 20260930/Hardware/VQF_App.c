#include <math.h>
#include "vqf.h"
#include "VQF_App.h"

/* 量程换算常数，必须和MPU6050_Init()里的配置保持一致
 * GYRO_CONFIG  = 0x18  ->  ±2000°/s  ->  16.4   LSB/(°/s)
 * ACCEL_CONFIG = 0x18  ->  ±16g      ->  2048   LSB/g
 * 如果以后改了量程，这两个常数要跟着改
 */
#define GYRO_LSB_PER_DPS		16.4f
#define ACCEL_LSB_PER_G			2048.0f

#define DEG_TO_RAD				0.017453292519943295f
#define RAD_TO_DEG				57.29577951308232f
#define STD_GRAVITY				9.80665f

/* VQF判定静止的条件之一：低通后的陀螺读数每个分量都要小于biasClip，
 * 默认2.0°/s。MPU6050在±2000dps档的零偏手册允许到±20°/s，实测零偏超过这个值
 * 就会导致Rest永远为0、静止零偏估计永远不启动，此时需要把这里调大。
 * 调大之前先看串口输出的GLP（低通后的三轴陀螺读数，单位°/s），
 * 取比实测零偏略大的值即可，比如实测最大分量是3.4°/s，这里写5.0f。
 */
#define VQF_BIAS_CLIP_DPS		2.0f

/* VQF的三块状态，放在全局区，避免占用MCU的栈 */
static vqf_params_t VQF_Params;
static vqf_state_t  VQF_State;
static vqf_coeffs_t VQF_Coeffs;

/**
  * 函    数：VQF初始化
  * 参    数：SampleHz 真实采样率，单位Hz
  * 返 回 值：无
  * 说    明：init_params必须先调用，否则initVqf会按照未初始化的参数计算滤波器系数
  */
void VQF_App_Init(float SampleHz)
{
	init_params(&VQF_Params);						//先装载默认参数（tauAcc=3s、tauMag=9s等）
	
	VQF_Params.biasClip = VQF_BIAS_CLIP_DPS;		//覆盖默认的2.0°/s
	
	/*陀螺和加速度计同速率采样，磁力计不使用（MPU6050没有磁力计，传-1表示同陀螺速率）*/
	initVqf(&VQF_Params, &VQF_State, &VQF_Coeffs, 1.0f / SampleHz, 1.0f / SampleHz, -1.0f);
}

/**
  * 函    数：VQF喂入一帧数据
  * 参    数：AccRaw 加速度计原始值（int16，单位LSB）
  * 参    数：GyrRaw 陀螺仪原始值（int16，单位LSB）
  * 返 回 值：无
  * 说    明：VQF要求的单位是角速度rad/s、加速度m/s²（含重力）
  */
void VQF_App_Update(const int16_t AccRaw[3], const int16_t GyrRaw[3])
{
	vqf_real_t Gyr[3];
	vqf_real_t Acc[3];
	uint8_t i;
	
	for (i = 0; i < 3; i ++)
	{
		Gyr[i] = (vqf_real_t)GyrRaw[i] / GYRO_LSB_PER_DPS * DEG_TO_RAD;
		Acc[i] = (vqf_real_t)AccRaw[i] / ACCEL_LSB_PER_G * STD_GRAVITY;
	}
	
	updateGyr(&VQF_Params, &VQF_State, &VQF_Coeffs, Gyr);	//先陀螺
	updateAcc(&VQF_Params, &VQF_State, &VQF_Coeffs, Acc);	//再加速度计
}

/**
  * 函    数：读取姿态四元数
  * 参    数：Quat 输出数组，顺序为w,x,y,z
  * 返 回 值：无
  */
void VQF_App_GetQuat(float Quat[4])
{
	vqf_real_t Q[4];
	uint8_t i;
	
	getQuat6D(&VQF_State, Q);		//6D为陀螺+加速度计融合，无磁力计参与
	
	for (i = 0; i < 4; i ++)
	{
		Quat[i] = (float)Q[i];
	}
}

/**
  * 函    数：读取欧拉角
  * 参    数：Roll Pitch Yaw 输出角度，单位度，范围分别约为±180、±90、±180
  * 返 回 值：无
  * 说    明：6D解算下Roll和Pitch由重力方向修正，Yaw只能靠陀螺积分，长时间必然缓慢漂移
  */
void VQF_App_GetEuler(float *Roll, float *Pitch, float *Yaw)
{
	vqf_real_t Q[4];
	vqf_real_t w, x, y, z, SinPitch;
	
	getQuat6D(&VQF_State, Q);
	w = Q[0]; x = Q[1]; y = Q[2]; z = Q[3];
	
	*Roll = atan2f(2.0f * (w * x + y * z), 1.0f - 2.0f * (x * x + y * y)) * RAD_TO_DEG;
	
	SinPitch = 2.0f * (w * y - z * x);			//asin的输入必须限制在±1以内，否则浮点误差会让结果变成NaN
	if (SinPitch >  1.0f) { SinPitch =  1.0f; }
	if (SinPitch < -1.0f) { SinPitch = -1.0f; }
	*Pitch = asinf(SinPitch) * RAD_TO_DEG;
	
	*Yaw = atan2f(2.0f * (w * z + x * y), 1.0f - 2.0f * (y * y + z * z)) * RAD_TO_DEG;
}

/**
  * 函    数：读取陀螺零偏估计
  * 参    数：BiasDps 输出数组，零偏估计值，单位度/秒
  * 返 回 值：零偏估计的不确定度，单位度/秒，越小表示估计越可信
  */
float VQF_App_GetBiasDps(float BiasDps[3])
{
	vqf_real_t Bias[3];
	vqf_real_t Sigma;
	uint8_t i;
	
	Sigma = getBiasEstimate(&VQF_State, &VQF_Coeffs, Bias);
	
	for (i = 0; i < 3; i ++)
	{
		BiasDps[i] = (float)(Bias[i] * RAD_TO_DEG);
	}
	
	return (float)(Sigma * RAD_TO_DEG);
}

/**
  * 函    数：读取静止检测结果
  * 参    数：无
  * 返 回 值：1表示当前处于静止状态，0表示运动中
  * 说    明：VQF只有在检测到静止时才会用陀螺的低通滤波值去修正零偏估计
  */
uint8_t VQF_App_IsRestDetected(void)
{
	return getRestDetected(&VQF_State) ? 1 : 0;
}

/**
  * 函    数：读取静止检测的中间量（调试用）
  * 参    数：GyrLpDps 低通后的陀螺读数，单位度/秒，三个分量都要小于biasClip
  * 参    数：GyrDev 陀螺读数相对其低通参考值的偏差，除以了阈值，必须小于1
  * 参    数：AccDev 加速度计偏差除以了阈值，必须小于1
  * 返 回 值：无
  * 说    明：VQF判定静止需要同时满足：
  *           1. GyrDev < 1 且 AccDev < 1，并连续保持restMinT(默认1.5秒)
  *           2. GyrLpDps的三个分量绝对值都小于biasClip
  *           只要有一条不满足，Rest就一直是0
  */
void VQF_App_GetRestDebug(float GyrLpDps[3], float *GyrDev, float *AccDev)
{
	vqf_real_t Dev[2];
	uint8_t i;
	
	getRelativeRestDeviations(&VQF_Params, &VQF_State, Dev);
	*GyrDev = (float)Dev[0];
	*AccDev = (float)Dev[1];
	
	for (i = 0; i < 3; i ++)
	{
		GyrLpDps[i] = (float)(VQF_State.restLastGyrLp[i] * RAD_TO_DEG);
	}
}

/**
  * 函    数：读取当前使用的biasClip阈值
  * 参    数：无
  * 返 回 值：biasClip，单位度/秒
  */
float VQF_App_GetBiasClip(void)
{
	return (float)VQF_Params.biasClip;
}
