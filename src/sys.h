#pragma once
#define _USE_MATH_DEFINES
#include <opencv2/opencv.hpp>
//#include <opencv2/opencv_lib.hpp>
#include <dynamixel.h>
#include <math.h>
#include <iostream>
#include <windows.h>
#include <thread>
#include <mutex>
#include <fstream>
#include <stdio.h>
#include <tchar.h>
#include <SDKDDKVer.h>
#include <vector>
#include <iterator>
#include <algorithm>
#pragma comment(lib, "winmm.lib")
#pragma warning(disable:4996)

#define WIN_NAME "2color_img"
#define TEST_MODE 0
#define RELEASE_MODE 1
#define MOTOR_SWICH 0
#define MOTOR_STATE 1
///////////////////////////////////////////////////////////////////////////
///////////////////////ここから各自設定///////////////////////////////////
/////////////////////////////////////////////////////////////////////////

//カメラに関する定義
#define CAP_NUM 0	//カメラIDの設定
#define CAM_W 320*2	//取得画像横サイズ //展示 640
#define CAM_H 240*2	//取得画像縦サイズ //     480
#define FPS 30		//フレームレートの設定

//Dinamixel2USBのポートの設定
#define PORT_NUM 4	//Dinamixel2USBのポートNo.
#define BORD_NUM 34	//16		//115200bps	//34	//57600(bps)
#define KP_GAIN 8		//比例ゲインの設定
#define KI_GAIN 0*FPS
#define KD_GAIN 8/FPS


//mbedポートNUM
//パソコンによって適宜変更
#define COM_NUM_MBED_MOUSE "\\\\.\\COM3"//※\\\\.\\は消さない
#define COM_NUM_MBED_LED "\\\\.\\COM5"//※\\\\.\\は消さない

///////////////////////////////////////////////////////////////////////////
///////////////////////ここまで各自設定///////////////////////////////////
/////////////////////////////////////////////////////////////////////////

//時間でプログラムを制御するフラグ
//時間でプログラムを終了させる場合はTRUE,終了させない場合はFALSE
//使用しない方をコメントアウト（頭に//をつける）して下さい(※どちらかを必ず選択)
//#define timer_flag TRUE//時間制御使用
#define timer_flag FALSE//時間制御使用しない

//計測時間
//上記でteimer_flagをTRUEにした場合時間を設定
#define REC_HOUR 0 //時間
#define REC_MINUTE 5//分 1〜59
