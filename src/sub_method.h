extern int pos_x, pos_y;
extern double radian;
extern int mode;
extern int THRE;
extern cv::Mat circleFrame;
extern bool mbed_flag;
extern bool calib_m;
extern bool rec;
extern DWORD start_time;
extern char mouse_filename[];
extern char main_filename[];
extern char video_filename[];
extern std::mutex mtx;
extern double dx, dy;
extern int itos;
extern int angle_target;

//mbedのシリアルポーと情報
extern HANDLE mbed_mouse;
extern HANDLE mbed_led;
//モータ関数
void init_Device();
void move_rx28(int speed_x, int speed_y, int st);
void calc_speed(int *x, int *y, int *st);
void motor_task();
void end_device_rx28();
void serial_led_task();


//画像処理関数
void img_processing_main(cv::Mat *src, cv::Mat *back, cv::Mat *dst);
void moment_task(cv::Mat *dst, cv::Moments mom);

//シリアル通信関数
void serial_task_read();
//void serial_setup_mouse();
//void serial_exit_mouse();

//フラグ処理関数
bool check_flag();
void change_flag();
void mode_releace();
int check_mode();
bool calib_motor(int ch);

//ファイル名
void set_fname();