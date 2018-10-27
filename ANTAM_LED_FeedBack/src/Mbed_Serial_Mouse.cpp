#include "sys.h"
#include "sub_method.h"

HANDLE mbed_mouse;

void serial_setup_mouse();
void recive_value(int *, int *, int *, int *);
void serial_task_read();
void serial_exit_mouse();


char *buf;//��M���������𕶎���Ƃ��Ď�M����ϐ�
int hip = 0;//buf�ɏ������񂾕�����
bool rec = false;
double dx, dy;
double proofreding_x = 0.024264188;
double proofreding_y = 0.027281252;
std::ofstream mouse;//�}�E�X�ړ��ʂ��t�@�C���ɏ����o���ϐ�



DWORD start_time = 0;//�f�[�^�擾�J�n�����i�X���b�h�J�n�����j
					 //DWORD t1;
DWORD t1, t2;



void serial_setup_mouse() {
	bool check = false;
	//�V���A���|�[�g��ڑ�(mbed_mouse)
	mbed_mouse = CreateFile(_T(COM_NUM_MBED_MOUSE), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (mbed_mouse == INVALID_HANDLE_VALUE) {
		std::cout << "mbed(mouse) PORT COULD NOT OPEN" << std::endl;
	}
	else {
		std::cout << "mbed(mouse) PORT OPEN" << std::endl;
	}
	//�u�b�t�@�[�̏���
	check = SetupComm(mbed_mouse, 1024, 1024);
	if (!check) {
		std::cout << "mbed(mouse) COULD NOT SET UP BUFFER" << std::endl;
		CloseHandle(mbed_mouse);
	}
	else {
		std::cout << "mbed SET UP OK" << std::endl;
	}
	//�u�b�t�@�̏�����
	check = PurgeComm(mbed_mouse, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);
	if (!check) {
		std::cout << "mbed(mouse) COULD NOT BUFFER CLER" << std::endl;
		CloseHandle(mbed_mouse);
	}
	else {
		std::cout << "mbed(mouse) BUFFER OK" << std::endl;
	}
	//�V���A���ʐM�̃X�e�[�^�X��ݒ�
	DCB dcb_mouse;
	GetCommState(mbed_mouse, &dcb_mouse);
	dcb_mouse.DCBlength = sizeof(DCB);
	dcb_mouse.BaudRate = 57600;
	dcb_mouse.fBinary = TRUE;
	dcb_mouse.ByteSize = 8;
	dcb_mouse.fParity = NOPARITY;
	dcb_mouse.StopBits = ONESTOPBIT;
	//�ݒ�̓K�p
	check = SetCommState(mbed_mouse, &dcb_mouse);
	if (!check) {
		std::cout << "mbed(mouse) SetCommState FAILED" << std::endl;
		CloseHandle(mbed_mouse);
	}
	else {
		std::cout << "mbed(mouse) SetCommOK" << std::endl;
	}

	
}

void serial_task_read() {
	buf = new char[255];
	int x, y;
	int move_x = 0, move_y = 0;
	serial_setup_mouse();
	while (1) {
		if (check_mode() == RELEASE_MODE && !rec) {
			rec = true;
			mouse = std::ofstream(mouse_filename);
			mouse << "time[ms]" << "," << "x" << "," << "y" << "," << "arc" << "," << "pos"<< std::endl;
			//mouse << std::endl;
		}

		recive_value(&x, &y, &move_x, &move_y);

		if (check_flag())
			break;
	}
	serial_exit_mouse();
}

void recive_value(int *x, int *y, int *move_x, int *move_y) {
	//�f�[�^�̎�M
	int length = 0;
	char c;
	DWORD errors;//�G���[�����i�[����ϐ�
	COMSTAT comStat;//��M�o�b�t�@�̃o�C�g�����i�[����ϐ�
	ClearCommError(mbed_mouse, &errors, &comStat);//erros�ɃG���[���AcomStat�Ɏ�M�o�b�t�@�����i�[
	length = comStat.cbInQue; // ��M�������b�Z�[�W�����擾����
	mtx.lock();
	int angle = angle_target;
	int position = itos;
	mtx.unlock();

	if (length > 0) {
		for (int i = 0; i < length; i++) {
			DWORD numberOfPut;//��M�������b�Z�[�W��
			ReadFile(mbed_mouse, &c, 1, &numberOfPut, NULL);
			if (c == ',') {
				//x�������ւ̈ړ��ʂ�int�֕ϊ�
				*x = atoi(buf);
				memset(buf, 0, sizeof(buf));
				hip = 0;
			}//���s�R�[�h�Ȃ�t�@�C���֒l�̏����o��
			else if (c == '\n') {
				//���������ւ̈ړ��ʂ�int�֕ϊ�
				*y = atoi(buf);
				//�t�@�C���̏����o������
				t1 = timeGetTime() - start_time;
				if (check_mode() == RELEASE_MODE) {
					//csv�t�@�C���ւ̏����o��
					*move_x += *x;
					*move_y += *y;
					dx = *x * proofreding_x;
					dy = *y * proofreding_y;
					
					//std::cout << "REC:" << (int64)t1 << ":" << *move_x << "," << *move_y << "," << get_angle() << std::endl;
					std::cout << "REC:" << (int64)t1 << ":" << *move_x << "," << *move_y << "," << angle << "," << position <<  std::endl;
					mouse << (int64)t1 << "," << *move_x << "," << *move_y << "," << angle << "," << position << std::endl;
				}
				else {
					//debug
					//std::cout << (int64)t1 << ":" << *x << "," << *y << std::endl;
					std::cout << (int64)t1 << ":" << *x << "," << *y << "," << angle << "," << position <<  std::endl;
				}
				memset(buf, 0, sizeof(buf));
				hip = 0;
			}//�f�t�H���g�ł�buf�ɕ������i�[
			else {
				buf[hip] = c;
				hip++;
			}
		}
	}
	else {
		t2 = timeGetTime() - start_time;
		if ((t2 - t1) >= 8) {
			if (check_mode() == RELEASE_MODE) {
				//csv�t�@�C���ւ̏����o��
				//std::cout << "REC:" << (int64)t2 << ":" << *move_x << "," << *move_y << "," << get_angle() << std::endl;
				std::cout << "REC:" << (int64)t2 << ":" << *move_x << "," << *move_y << "," << angle << "," << position << std::endl;
				dx = 0;
				dy = 0;
				mouse << (int64)t2 << "," << *move_x << "," << *move_y << "," << angle << "," << position << std::endl;
			}
			else {
				std::cout << (int64)t2 << ":" << "0" << "," << "0" << "," << angle << "," << position <<  std::endl;
			}
			t1 = timeGetTime() - start_time;
		}
		else {
			Sleep(1);
		}
	}
}



void serial_exit_mouse() {
	bool check = false;
	check = PurgeComm(mbed_mouse, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);
	if (!check) {
		std::cout << "COULD NOT CLER" << std::endl;
	}
	CloseHandle(mbed_mouse);
	std::cout << "close serial port Mouse" << std::endl;
}