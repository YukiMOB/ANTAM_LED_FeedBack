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
///////////////////////��������e���ݒ�///////////////////////////////////
/////////////////////////////////////////////////////////////////////////

//�J�����Ɋւ����`
#define CAP_NUM 0	//�J����ID�̐ݒ�
#define CAM_W 320*2	//�擾�摜���T�C�Y //�W�� 640
#define CAM_H 240*2	//�擾�摜�c�T�C�Y //     480
#define FPS 30		//�t���[�����[�g�̐ݒ�

//Dinamixel2USB�̃|�[�g�̐ݒ�
#define PORT_NUM 4	//Dinamixel2USB�̃|�[�gNo.
#define BORD_NUM 34	//16		//115200bps	//34	//57600(bps)
#define KP_GAIN 8		//���Q�C���̐ݒ�
#define KI_GAIN 0*FPS
#define KD_GAIN 8/FPS


//mbed�|�[�gNUM
//�p�\�R���ɂ���ēK�X�ύX
#define COM_NUM_MBED_MOUSE "\\\\.\\COM3"//��\\\\.\\�͏����Ȃ�
#define COM_NUM_MBED_LED "\\\\.\\COM5"//��\\\\.\\�͏����Ȃ�

///////////////////////////////////////////////////////////////////////////
///////////////////////�����܂Ŋe���ݒ�///////////////////////////////////
/////////////////////////////////////////////////////////////////////////

//���ԂŃv���O�����𐧌䂷��t���O
//���ԂŃv���O�������I��������ꍇ��TRUE,�I�������Ȃ��ꍇ��FALSE
//�g�p���Ȃ������R�����g�A�E�g�i����//������j���ĉ�����(���ǂ��炩��K���I��)
//#define timer_flag TRUE//���Ԑ���g�p
#define timer_flag FALSE//���Ԑ���g�p���Ȃ�

//�v������
//��L��teimer_flag��TRUE�ɂ����ꍇ���Ԃ�ݒ�
#define REC_HOUR 0 //����
#define REC_MINUTE 5//�� 1�`59
