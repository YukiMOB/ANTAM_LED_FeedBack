#include "sys.h"
#include "sub_method.h"

/*
2017_1215 ビジュアルフィードバック状態と固定の切り替え処理を両方実装
ON/OFF領域の反転の実装
*/
HANDLE mbed_led;
#define BAUDRATE_LED_MBED 57600
#define LED_A 960  + 1//[ms] lx,lyを更新する周期 960が安定
#define LED_CHANGE_CYCLE 40000 //[ms] min = LED_F
#define SETUP_TIME 30000 //[ms]
#define EXPERIMENT_TIME 1800000 + SETUP_TIME //[ms]
//#define EXPERIMENT_TIME 180 //[ms]
#define LIGHT_ARC 180 //無視劇状態にしたいなら0
#define LED_ARRAY_X 32
#define LED_ARRAY_Y 8
#define MOUSE_F 1 //フィードバックの周期[ms]
#define ADVANTAGE 0
#define REVERSE_FLAG 0 //1にすると点灯範囲をあべこべにする
#define STATIC_FLAG 1 //1にするとビジュアルフィードバックではない固定の座標切り替えにする
#define STATIC_ANGLE_TARGET 10
#define SENSOR_HZ 8
DWORD t;
DWORD dt;
DWORD dt2;
void serial_setup_led();
void serial_exit_led();
void serial_led_task();
int exchangeLedData(double);
int roundOff(double);
int roundUp(double);
int roundDown(double);
int check_led_value(int, int);
double get_angle(double, double, double, double, int);
double get_x(double, double);
double get_y(double, double);
float median(std::vector<float>);
float sum(std::vector<float>);
float average(std::vector<float>);
float mode_led(std::vector<float>);
double pre_lx = 0;
double pre_ly = 0;
int angle_target;
int itos;
using namespace ::std;


mutex mtx;

void serial_led_task() {
	DWORD dwWritten;
	DWORD numberOfPut;
	t = timeGetTime() - start_time;
	serial_setup_led();
	char key;
	char c;
	char buf[] = "";
	static bool start_write = true;
	static bool not_rerealse = true;
	static int pre_center = 0;
	static int pre_angle = 0;
	static double lx = 0;
	static double ly = 0;
	static std::vector <float> x(LED_A, 0);
	static std::vector <float> y(LED_A, 0);
	static std::vector <float> angle_window(LED_A, 0);//窓の大きさがLED_A
	static int angle = 0;
	static int array_count = 0;
	static bool led_static_tf = TRUE;

	string start = "s";
	string set = ":";
	string end = "e";
	//string str;
	ostringstream center_s;
	ostringstream range_s;
	string str;
	while (1) {
		//std::cout << "led:" << check_mode() << std::endl;
		if (check_mode() == RELEASE_MODE) {
			//特定の周波数で加算平均を掛ける処理 結構処理重たい
			t = timeGetTime() - start_time;
			if ((t > EXPERIMENT_TIME) || (t < SETUP_TIME && start_write == true)) {
				angle_target = 0;
				itos = 0;
				str = "s0:0e";
				if (t < SETUP_TIME && start_write == true) {
					start_write = false;
					WriteFile(mbed_led, &str, str.size(), &dwWritten, NULL);
					ReadFile(mbed_led, &c, 1, &numberOfPut, NULL);
				}
				if (t > EXPERIMENT_TIME) {
					WriteFile(mbed_led, &str, str.size(), &dwWritten, NULL);
					ReadFile(mbed_led, &c, 1, &numberOfPut, NULL);
					memset(&str, 0x0, sizeof(str));
					range_s.str("");
					range_s.clear(stringstream::goodbit);
					center_s.str("");
					center_s.clear(stringstream::goodbit);
					break;
				}
			}
			else if (t >= SETUP_TIME - LED_A && t < EXPERIMENT_TIME && start_write == false && (t - SETUP_TIME <= MOUSE_F || (t - SETUP_TIME) % SENSOR_HZ <= MOUSE_F)) {
				if (array_count > LED_A - 1) {
					array_count = 0;
				}
				mtx.lock();
				if (dx != 0 && dy != 0) {
					//angle_window[array_count] = roundOff((get_angle(dx, dy, 0, 0, REVERSE_FLAG)) * 0.1) * 10;
					angle_window[array_count] = (get_angle(dx, dy, 0, 0, REVERSE_FLAG) + 30);
					x[array_count] = dx;
					y[array_count] = dy;
				}
				mtx.unlock();
				array_count++;
			}
			if (t >= SETUP_TIME && ((t - SETUP_TIME) % LED_CHANGE_CYCLE < MOUSE_F || t - SETUP_TIME < MOUSE_F) && t >= SETUP_TIME && ((sum(x) != 0 && sum(y) != 0 && STATIC_FLAG == 0))) {
				//angle_target = get_angle(average(x), average(y), 0, 0, REVERSE_FLAG);//角度取得、加算平均
				//angle_target = roundOff(get_angle(median(x), median(y), 0, 0, REVERSE_FLAG) * 0.1) * 10;//角度取得、加算平均
				//angle_target = exchangeLedData(get_angle(median(x), median(y), 0, 0, REVERSE_FLAG) + 30);//角度取得、加算平均
				angle_target = median(angle_window);
				//angle_target = exchangeLedData(average(angle_window));
				//angle_target = roundOff(average(angle_window)* 0.1) * 10;
				//angle_target = mode_led(angle_window);
				itos = exchangeLedData(angle_target);//これに30度足したもの。書き込み用
				//itos = angle_target;
				center_s << itos;
				//itos = angle_target;
				//center_s << angle_target;
				if (REVERSE_FLAG == 0) {
					range_s << exchangeLedData(LIGHT_ARC);
				}
				else {
					range_s << abs(exchangeLedData(LIGHT_ARC) - 32);
				}
				str = start + center_s.str() + set + range_s.str() + end;
				if (pre_center != itos) {
					WriteFile(mbed_led, &str, str.size(), &dwWritten, NULL);
					ReadFile(mbed_led, &c, 1, &numberOfPut, NULL);
					//cout << "str :" << str << endl;
					pre_center = itos;
					pre_angle = angle_target;
					memset(&str, 0x0, sizeof(str));
					range_s.str("");
					range_s.clear(stringstream::goodbit);
					center_s.str("");
					center_s.clear(stringstream::goodbit);
				}
			}
			else if (t >= SETUP_TIME && ((t - SETUP_TIME) % LED_CHANGE_CYCLE < MOUSE_F || t - SETUP_TIME < MOUSE_F) && t >= SETUP_TIME && STATIC_FLAG == 1) {
				if (led_static_tf == TRUE) {
					//angle_target = angle_target - 180;
					angle_target = STATIC_ANGLE_TARGET + 30;
					led_static_tf = FALSE;
				}
				else {
					//angle_target = angle_target + 180;
					angle_target = (180 + STATIC_ANGLE_TARGET) + 30; //If it doesn`t need to stimulus change, it comment out.
					led_static_tf = TRUE;
				}

				itos = exchangeLedData(angle_target);
				center_s << itos;
				if (REVERSE_FLAG == 0) {
					range_s << exchangeLedData(LIGHT_ARC);
				}
				else {
					range_s << abs(exchangeLedData(LIGHT_ARC) - 32);
				}

				str = start + center_s.str() + set + range_s.str() + end;
				if (pre_center != itos) {
					WriteFile(mbed_led, &str, str.size(), &dwWritten, NULL);
					ReadFile(mbed_led, &c, 1, &numberOfPut, NULL);
					cout << "str :" << str << endl;
					Sleep(10);
					pre_center = itos;
					pre_angle = angle_target;
					//memset(&str, 0x0, sizeof(str));
					range_s.str("");
					range_s.clear(stringstream::goodbit);
					center_s.str("");
					center_s.clear(stringstream::goodbit);
				}
			}
			//memset(&str, 0x0, sizeof(str));
			range_s.str("");
			range_s.clear(stringstream::goodbit);
			center_s.str("");
			center_s.clear(stringstream::goodbit);
		}
		else if (not_rerealse == true) {
			angle_target = 0;
			itos = 0;
			str = "s0:0e";
			WriteFile(mbed_led, &str, str.size(), &dwWritten, NULL);
			ReadFile(mbed_led, &c, 1, &numberOfPut, NULL);
			//memset(&str, 0x0, sizeof(str));
			range_s.str("");
			range_s.clear(stringstream::goodbit);
			center_s.str("");
			center_s.clear(stringstream::goodbit);
			not_rerealse = false;
		}

		if (check_flag()) {
			break;
		}

	}
	serial_exit_led();

}

double get_x(double x, double y) {
	if (x == 0 && y == 0) {
		return pre_lx;
	}
	else return x;
}
double get_y(double y, double x) {
	if (x == 0 && y == 0) {
		return pre_ly;
	}
	else return y;
}
void serial_setup_led() {
	bool check = false;
	//シリアルポートを接続(mbed_led)
	mbed_led = CreateFile(_T(COM_NUM_MBED_LED), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (mbed_led == INVALID_HANDLE_VALUE) {
		std::cout << "mbed(led) PORT COULD NOT OPEN" << std::endl;
	}
	else {
		std::cout << "mbed(led) PORT OPEN" << std::endl;
	}
	//ブッファーの準備
	check = SetupComm(mbed_led, 1024, 1024);
	if (!check) {
		std::cout << "mbed(led) COULD NOT SET UP BUFFER" << std::endl;
		CloseHandle(mbed_led);
	}
	else {
		std::cout << "mbed SET UP OK" << std::endl;
	}
	//ブッファの初期化
	check = PurgeComm(mbed_led, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);
	if (!check) {
		std::cout << "mbed(led) COULD NOT BUFFER CLER" << std::endl;
		CloseHandle(mbed_led);
	}
	else {
		std::cout << "mbed(led) BUFFER OK" << std::endl;
	}
	//シリアル通信のステータスを設定
	DCB dcb_led;
	GetCommState(mbed_led, &dcb_led);
	dcb_led.DCBlength = sizeof(DCB);
	dcb_led.BaudRate = BAUDRATE_LED_MBED;
	dcb_led.fBinary = TRUE;
	dcb_led.ByteSize = 8;
	dcb_led.fParity = NOPARITY;
	dcb_led.StopBits = ONESTOPBIT;
	//設定の適用
	check = SetCommState(mbed_led, &dcb_led);
	if (!check) {
		std::cout << "mbed(led) SetCommState FAILED" << std::endl;
		CloseHandle(mbed_led);
	}
	else {
		std::cout << "mbed(led) SetCommOK" << std::endl;
	}
}

void serial_exit_led() {
	bool check = false;
	check = PurgeComm(mbed_led, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);
	if (!check) {
		std::cout << "COULD NOT CLER" << std::endl;
	}
	CloseHandle(mbed_led);
	std::cout << "close serial port led" << std::endl;
}


//特定範囲の切捨て、切り上げの関数
int check_led_value(int d, int c) {
	if (d > c) {
		return d - c;
	}
	else if (d < 0) {
		return d + c;
	}
	else {
		return d;
	}
}


//ラジアンデータをLEDアレイの座標データに変換
int exchangeLedData(double d) {
	return roundOff(d / (360 / LED_ARRAY_X));
}

//小数点の切捨て、切り上げの関数

int roundOff(double n) {
	double decimal = 0;
	decimal = n - (int)n;
	if (decimal >= 0.5 || decimal <= -0.5) {
		return roundUp(n);
	}
	else {
		return roundDown(n);
	}
}

int roundDown(double n) {
	return (int)n;
}

int roundUp(double n) {
	if (n >= 0) {
		return (int)n + 1;
	}
	else {
		return (int)n - 1;
	}
}


double get_angle(double x, double y, double px, double py, int flag) {
	double radian = atan2(y - py, x - px);
	double angle = radian * 180 / M_PI;

	if (flag == 1) {
		if (angle < 180) {
			angle = angle + 180;
		}
		else {
			angle = angle - 180;
		}
	}
	if (angle < 0) {
		//cout << "rad" << 360 + angle << endl;
		return angle = 360 + angle;
	}
	if (angle > 360)
		return angle = angle - 360;
	//cout << "rad" <<  angle  << endl;
	else
	{
		return angle;
	}
}
float median(std::vector<float> v) {
	size_t size = v.size();
	float *t = new float[size];
	std::copy(v.begin(), v.end(), t);
	std::sort(t, &t[size]);
	float result = size % 2 ? t[size / 2] : (t[(size / 2) - 1] + t[size / 2] / 2);
	delete[] t;
	return result;
}

float sum(std::vector<float> v) {
	int size = v.size();
	float sum = 0;
	for (int i = 0; i < size; i++) {
		sum += v[i];
	}
	return sum;
}

float average(std::vector<float> v) {
	int size = v.size();
	float sum = 0;
	for (int i = 0; i < size; i++) {
		sum += v[i];
	}
	return sum / size;
}

float mode_led(std::vector<float> v) {
	int size = v.size();
	std::vector<int>led_window(32, 0);
	int mode = 0, modev = 0;
	for (int i = 0; i < size; i++) {
		led_window[v[i]] += 1;
		if (modev < led_window[v[i]]) {
			mode = v[i];
			modev = led_window[v[i]];
		}
	}
	return mode;
}