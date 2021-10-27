#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <stdlib.h>
// #include "opencv/cv.h"
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgproc/types_c.h>
#include <QThread>
#include <QTimer>
#include <QMutex>
#include <QImage>
#include <QtSql/QSqlDatabase>
#include <QMessageBox>
#include <QtSql>
#include <QVector>
#include <iostream>

// #include "iostream"
#include "vector"
#include <string>
#include <utility>
#include <iomanip>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <time.h>
// 自己的头文件
#include "ncnn_recognition/recognition.h"
#include "measure_dis.h"
// #include "MVCapture.h"
#include "gabor.h"
#include "MCP/PointFeature.h"
#include "MCP/ImgPreprocess.h"
#include "utils.h"

#include "Camera.h"
// using namespace cv;
/////////////////////////running status//////////////////////////////////////
enum flags {reg, rec};
// enum camID {camid0, camid1}
/////////////////////////////////////////////////////////////////////////////



/////////////////////////宏定义///////////////////////////////////////////////
#define NCNN

#define feature_dim 256

#ifdef NCNN
#define register_num 10
#else
#define register_num 6
#endif
/////////////////////////////////////////////////////////////////////////////
namespace Ui {
class Dialog;
}
/////////////////////////////////////////////////////////////////////////////
//class MyMessageBox:public QMessageBox{
//protected:
//    void showEvent(QShowEvent*event){
//        QMessageBox::showEvent(event);
//        setFixedSize(480,320);
//    }
//};

class Dialog : public QDialog
{
    Q_OBJECT

public:
    // struct result{
    //     float score;
    //     int id;
    // } ;
    #ifdef NCNN
    struct result{
        float score;
        string id;
    } ;
    #else
    struct result{
        double score;
        string id;
    } ;
    #endif

    flags state = rec;  //init status
    int reg_num = 0;    //init reg number
    ncnn_net net;
    explicit Dialog(QWidget *parent = 0);
    void registered_num();
    cv::Mat run_roi(cv::Mat matOriginal);
    void Image_normalize(cv::Mat, cv::Mat &);
    void Enhancement(cv::Mat input, cv::Mat &output);
    // cv::Mat img_normalize(cv::Mat roi_img);
    // void normalize2(cv::Mat& roi_img);
//    void recognition();
    std::vector<float> extract_feature(cv::Mat roi);
    float cal_sim(float feature1[],float feature2[]);

    #ifdef NCNN
    result recognition(float[], float[]);
    #else
	result recognition(vector<cv::Point> PointVector_rec, vector<vector<float>> FeatureDescriptor_rec);
    #endif

    void adjust_cam();
    void detectAndDisplay(cv::Mat frame);
    void RestoreVectors(std::vector<std::vector<cv::Rect>>& vecs_bank, std::vector<cv::Rect>& vecAll);
    //*************截取手掌roi，参考王浩师兄代码*************
    cv::Mat getRoiImg(cv::Mat PalmveinImg, bool flag_showImg, std::string imgName, int & e_rror, cv::Mat &showImg2);
    cv::Mat finger_segmentation(cv::Mat img);
    cv::Mat prewitt_segmentation(cv::Mat img, std::string PPorPV);
    void imageblur(cv::Mat& src, cv::Mat& dst, cv::Size size, int threshold);
    void debug_imshow(const char * name, cv::Mat img);
    void bwareaopen(cv::Mat &src, std::vector<cv::Mat> &area, int n);
    void bwareaopen2(cv::Mat &src);
    void getMaxRegion(cv::Mat srcImg, cv::Mat& dstImg, cv::Point* maxarea_point, int& totalnum, int& e_rror);
    int get_square_twopoints(cv::Point* areapoint, int& totalnum, int& beginpoint, cv::Point& centroidpoint, cv::Point* square_point, std::vector<cv::Point>& fingerpoint, cv::Mat& backImg, int& e_rror);
    cv::Mat get_palm_ROI(cv::Mat srcImg, const cv::Point* square_points, int &e_rror);
    double Dis_of_twoPoint(cv::Point point1, cv::Point point2);
    int find_extremum(double* Dis, int totalnum, int position, int length_limit, bool isMax, double* disValley, int num, cv::Mat& backImg, cv::Point* areapoint, int startpoint);
    void rotateImage(cv::Mat srcImg, cv::Mat& rotateImg, cv::Point2f rotatecenter, int rotateangle);
    ~Dialog();
    //*************类外函数***********
    friend string getCurrentTime();
    //*************类外类***********
    friend class Camera;

private slots:
//    void on_starting_clicked();9op0

public slots:
    void processFrameAndUpdateGUI();
    void get_palmImg(); // 读取子线程的图片
    void del_oneuser();
    void register_id();
    void show_register_ok();
    void show_running();
    void database();
    void show_reco();
    void change_state_reg();   //change status rec to reg

//    void cal_sim_value();
private:
    Ui::Dialog *ui;

    // cv::VideoCapture capWebcam;
    // cv::Mat matOriginal_main;
    cv::Mat img_pp;
    cv::Mat img_pv;
    int width =640;// 640
    int height =480;//480
    int camid0 = 0;
    int camid1 = 1;
    //CAMERA cam0;
    //CAMERA cam1;

    QSqlDatabase db;
    QSqlQuery query;
    QImage qimgOriginal;
    QImage qimgProcessed;

    // std::string palm_cascade_name = "/home/pi/Downloads/qt/palm.xml";
    // cv::CascadeClassifier palm_cascade;

    QTimer* tmrTimer;
    QMutex mutex_main;
    int roi_count=0;
    int refreshTime;
    // int register_num = 0;
    bool rec_reg_finished;

    QThread *thread;
    measure_dis *meas1;
};

int OTSU(cv::Mat image);

#endif // DIALOG_H
