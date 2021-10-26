#include "dialog.h"
#include "ui_dialog.h"
#include "dirent.h"
#include "dirent.h"
#include "sqlite3.h"     //sqlite数据库对应的函数库的头文件
#include "string"
#include "vector"
#include <sys/stat.h>
#include <iostream>
// #include "iostream"
#include <QMessageBox>
#include <QDebug>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <cmath>
#include <QString>
#include <QVector>
#include <unistd.h>
#include <wiringPi.h>
#include <softPwm.h>
#include <time.h>
#include "ncnn_recognition/recognition.h"
// #include "utils.h"


using namespace cv;
using namespace std;
CAMERA cam0(0, 640, 480);
CAMERA cam1(2, 640, 480);

auto t_cam0 = async(&CAMERA::update_img , &cam0);
auto t_cam1 = async(&CAMERA::update_img , &cam1);

// 排序
template <typename T>
vector< size_t> sort_indexes(const vector< T>  & v);

/////////////////////////////////////////////////////////////////////////////
Dialog::Dialog(QWidget *parent) : QDialog(parent),ui(new Ui::Dialog)
{

    ui->setupUi(this);
    mutex_main.lock();
    rec_reg_finished = true;
    mutex_main.unlock();

	ui->textBrowser->setFontPointSize(20);
	ui->textBrowser->setFontWeight(QFont::Normal);

    refreshTime = 30;
	
	// 显示掌静脉logo----------------------------
    Mat logo = imread("../1.jpg");
	
	QImage qlogo((uchar*)logo.data, logo.cols, logo.rows, logo.step, QImage::Format_RGB888);
	ui->processed->setPixmap(QPixmap::fromImage(qlogo)); // show the Image
	// ----------------------------------

	//相机线程
	

    thread = new QThread(); // 子线程测量距离
    meas1 = new measure_dis();

    meas1->moveToThread(thread);
    connect(thread, SIGNAL(started()), meas1, SLOT(startwork())); // 线程启动 开始测距+存图
    connect(meas1, SIGNAL(detected_palm()), this, SLOT(get_palmImg()));
    connect(meas1, SIGNAL(finished()), thread, SLOT(quit()), Qt::DirectConnection);

    // tmrTimer = new QTimer(this);
    // connect(tmrTimer, SIGNAL(timeout()), this, SLOT(processFrameAndUpdateGUI()));
//    connect(tmrTimer, SIGNAL(timeout()), this, SLOT(show_running()));
    connect(ui->register_btn,SIGNAL(clicked(bool)),this,SLOT(change_state_reg()));
    connect(ui->del_btn,SIGNAL(clicked(bool)),this,SLOT(del_oneuser()));

    thread->start();
//    connect(ui->rec, SIGNAL(clicked(bool)), this, SLOT(show_reco()));

//    connect(ui->quit, SIGNAL(clicked()), qApp, SLOT(quit()));

    // tmrTimer->start(refreshTime);
    // 输出pwm
    // softPwmWrite(pwm_pin, 32);

    // if(!palm_cascade.load(palm_cascade_name))
    // {
    //     QMessageBox::information(this, "Error with palm_cascade", "There was an error when trying to load palm_cascade xml");
    //     return;
    // }
}
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
Dialog::~Dialog()
{
    meas1->stopwork();
    thread->wait();
    qDebug()<<"Deleting thread and measurement in Thread "<<this->QObject::thread()->currentThreadId();
    delete thread;
    delete meas1;
    
    delete ui;
}
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
void Dialog::processFrameAndUpdateGUI()
{
    // matOriginal_main = meas1->get_threadImage();
    // QImage qimgOriginal((uchar*)matOriginal_main.data, matOriginal_main.cols, matOriginal_main.rows, matOriginal_main.step, QImage::Format_RGB888);
    // ui->processed->setPixmap(QPixmap::fromImage(qimgOriginal)); // show the Image
//     capWebcam.read(matOriginal);
//     // 按下按键
//     if (state == rec && digitalRead(key_pin)==LOW)
//     {
//         while(digitalRead(key_pin)==LOW) ;
//         if(matOriginal.empty())return;
//         clock_t start_time,end_time;
//         start_time = clock();
//         show_reco();
//         end_time = clock();
// //        QString qstring = "ncnn recog time: " + QString::number((end_time - start_time)*1000.0 / CLOCKS_PER_SEC)+"ms\n";
// //        ui->textBrowser->insertPlainText(qstring);
//     }
//     if (state == reg && digitalRead(key_pin) == LOW)
//     {
//         while(digitalRead(key_pin)==LOW) ;
//         // clock_t start_time,end_time;
//         // start_time = clock();
//         register_id();
//         // end_time = clock();
//         // QString qstring = "ncnn time: " + QString::number((end_time - start_time)*1000.0 / CLOCKS_PER_SEC)+"ms\n";
//         // ui->textBrowser->insertPlainText(qstring);
        
//     }

//     cv::Mat matOriginal2;
//     matOriginal2 = matOriginal.clone();
//     cv::Rect rect(60,20,520,50);
//     cv::rectangle(matOriginal2,rect,cv::Scalar(0,255,0),1,8,0);

//     QImage qimgOriginal((uchar*)matOriginal2.data, matOriginal2.cols, matOriginal2.rows, matOriginal2.step, QImage::Format_RGB888);
// //    QImage qimgroi((uchar*)final_roi_image.data, final_roi_image.cols, final_roi_image.rows, final_roi_image.step, QImage::Format_RGB888);

//     ui->processed->setPixmap(QPixmap::fromImage(qimgOriginal));

//    ui->processed_2->setPixmap(QPixmap::fromImage(qimgroi));
    // if(digitalRead(6) == LOW)
    // {
    //     register_id();
    // }
    return;
}

void Dialog::get_palmImg() // 读取子线程的图片
{
    // matOriginal_main = meas1->matOriginal.clone();
    
    // cout << "GRAY Image" << endl;
    // for(int i = 240; i < 243; i++)
    // {
    //     for(int j = 320; j < 321; j++)
    //     {
    //         cout << i << "," << j << "," << (int)matOriginal_main.at<uchar>(i,j) << endl;
    //     }
    // }
	mutex_main.lock();
    bool rec_reg_finished_temp = rec_reg_finished;
    mutex_main.unlock();
    cout << "rec_reg_finished_temp is now: " << rec_reg_finished_temp << endl;
    string roipp_path =  "../run_file/roi_pp/";
	string roipv_path =  "../run_file/roi_pv/";
    


	if(rec_reg_finished_temp){
		roi_count++;
		mutex_main.lock();
		rec_reg_finished = false;
		//vector<Mat> mainImg_vector(meas1->Img_vector);
		//取相机图片
		std::vector<uint8_t> buffer1_shibie;
		std::vector<uint8_t> buffer2_shibie;
		cv::Mat img0_shibie = cv::Mat::zeros(height, width, CV_8UC3);
		cv::Mat img1_shibie = cv::Mat::zeros(height, width, CV_8UC3);
		cam0.get_img(buffer1_shibie);
		cam1.get_img(buffer2_shibie);
		img_pp=cv::Mat::zeros(height, width, CV_8UC3);
		img_pv=cv::Mat::zeros(height, width, CV_8UC3);
		memcpy(img_pp.data, buffer1_shibie.data(), buffer1_shibie.size());
		memcpy(img_pv.data, buffer2_shibie.data(), buffer2_shibie.size());
	
        
        cv::imwrite(roipp_path+std::to_string(roi_count)+".jpg",img_pp);
        cv::imwrite(roipv_path+std::to_string(roi_count)+".jpg",img_pv);


		mutex_main.unlock();

		// for(int i = mainImg_vector.size() - 1; i >= 0; i--){ //后往前
		// 	matOriginal_main = mainImg_vector[i];
		// 	if(matOriginal_main.empty()){
		// 		continue;
		// 	}
		// cvtColor(matOriginal_main, matOriginal_main, COLOR_GRAY2BGR);
		QImage qimgOriginal((uchar*)img_pv.data, img_pv.cols, img_pv.rows, img_pv.step, QImage::Format_RGB888);
		ui->processed->setPixmap(QPixmap::fromImage(qimgOriginal)); // show the Image
		QImage qimgOriginal2((uchar*)img_pp.data, img_pp.cols, img_pp.rows, img_pp.step, QImage::Format_RGB888);
		ui->processed_2->setPixmap(QPixmap::fromImage(qimgOriginal2)); // show the Image


		if (state == rec)
		{
			show_reco();
		}
		else if (state == reg)
		{
			register_id();
		}

		// 	meas1->complete_oneImg();
		// }
		mutex_main.lock();
		rec_reg_finished = true;
		mutex_main.unlock();
		// cout << "BGR Image" << endl;
		// for(int i = 240; i < 243; i++)
		// {
		//     for(int j = 320; j < 321; j++)
		//     {
		//         cout << i << "," << j << "," << (int)matOriginal_main.at<Vec3b>(i,j)[0] << "," << (int)matOriginal_main.at<Vec3b>(i,j)[1] << "," << (int)matOriginal_main.at<Vec3b>(i,j)[2] << endl;
		//     }
		// }		
	}
	

}

void Dialog::del_oneuser()
{
	ui->textBrowser->clear();
	int info_cnt = 0;
	QString id;
	id = ui->textEdit->toPlainText();
	if(QSqlDatabase::contains("qt_sql_default_connection"))
		db = QSqlDatabase::database("qt_sql_default_connection");
	else
		db = QSqlDatabase::addDatabase("QSQLITE");
	#ifdef NCNN
	db.setDatabaseName(QApplication::applicationDirPath()+"/database.dat");
	#else
	db.setDatabaseName(QApplication::applicationDirPath()+"/databaseMCM.dat");
	#endif
	db.open();

	QSqlQuery query_del;
	QString del = QString("delete from featuretables where user='%1'").arg(id);
	QString s = QString("select count(*) from featuretables where user='%1';").arg(id);

	if(!query_del.exec(s)){
		qDebug() << query_del.lastError().databaseText();
		db.close();
		return;
	}
	while(query_del.next()){
		if(query_del.isActive()){
			info_cnt = query_del.value(0).toInt();
		}
	}
	if(query_del.exec(del) && info_cnt > 0){
		
		// ui->textBrowser->setFontPointSize(20);
        // ui->textBrowser->setFontWeight(QFont::Normal);
		QString qstring = "成功删除用户ID : " + id + "\n";
        ui->textBrowser->insertPlainText(qstring);
        ui->textBrowser->moveCursor(QTextCursor::End);
		ui->textEdit->clear();
		ui->textEdit->clearFocus();
		db.close();
		return;
	}
	else{
		qDebug()<<"删除失败:"<<query_del.lastError().text();
		// ui->textBrowser->setFontPointSize(20);
        // ui->textBrowser->setFontWeight(QFont::Normal);
        ui->textBrowser->insertPlainText("未找到待删除用户ID !\n");
        ui->textBrowser->moveCursor(QTextCursor::End);
		db.close();
		return;
	}

}

#ifdef NCNN
//ncnn提取特征的识别函数
void Dialog::show_reco()
{
    // clock_t start_time,end_time;
    int e_rror = 0;
    
    ui->textBrowser->clear();
	string hardOriginal = "../run_file/hard_Original/";
	string hardRoi = "../run_file/hard_Roi/";
	
	string timeStamp = getCurrentTime();
//    static int pwm_val = 20;
//    pwm_val += 5;
//    if (pwm_val>99) pwm_val = 20;
//    softPwmWrite(pwm_pin, pwm_val);
//    QString qstring = "now pwm_val: " + QString::number(pwm_val) + "\n";
//    ui->textBrowser->insertPlainText(qstring);
//    ui->textBrowser->moveCursor(QTextCursor::End);
    Mat matOriginalPV =img_pv.clone();
	Mat matOriginalPP =img_pp.clone();

    cv::Mat final_roi_imagePV, final_roi_imageEnhancePV, reco_showImgPV, \
		final_roi_imagePP, final_roi_imageEnhancePP, reco_showImgPP;

	cv::imwrite("../run_file/reco_originalPV.bmp",matOriginalPV);
	cv::imwrite("../run_file/reco_originalPP.bmp",matOriginalPP);
    final_roi_imagePV = getRoiImg(matOriginalPV, true, "PV", e_rror, reco_showImgPV);
	final_roi_imagePP = matOriginalPP.clone()//先这么写着
//    normalize2(final_roi_image);2
    cv::imwrite("../run_file/reco_roipv.bmp",final_roi_imagePV);
    cv::imwrite("../run_file/valleypoint_recopv.bmp",reco_showImgPV);
	cv::imwrite("../run_file/reco_roipp.bmp",final_roi_imagePP);
    // cv::imwrite("../run_file/valleypoint_recopp.bmp",reco_showImgPP);
	if(e_rror == 1){
        qDebug() << "Can not get ROI !!!";
        ui->textBrowser->setFontPointSize(20);
        ui->textBrowser->setFontWeight(QFont::Normal);
        ui->textBrowser->insertPlainText("Can not get ROI !!!\n");
        ui->textBrowser->moveCursor(QTextCursor::End);
        // mutex_main.lock();
        // rec_reg_finished = true;
        // mutex_main.unlock();
        return;
    }

    Enhancement(final_roi_image, final_roi_imageEnhancePV);
    cv::imwrite("../run_file/Enhance_recopv.bmp",final_roi_imageEnhancePV);
	Enhancement(final_roi_imagePP, final_roi_imageEnhancePP);
    cv::imwrite("../run_file/Enhance_recopp.bmp",final_roi_imageEnhancePP);
	//掌脉
    ncnn::Mat feature2 = net.extract_feature(final_roi_imageEnhancePV);
    ncnn::Mat feature2_flatten = feature2.reshape(feature2.w * feature2.h * feature2.c);
    vector<float> feature;
    feature.resize(feature2_flatten.w);
    cout<<"feature size: "<<feature.size()<<" feature2_flat: "<<feature2_flatten.w<<endl;
    for (int j=0; j<feature2_flatten.w; j++)
    {
        feature[j] = feature2_flatten[j]; // 99
    }
    float feature_arrayPV[feature.size()];
    memcpy(feature_arrayPV,&feature[0],feature.size()*sizeof(feature[0]));
	//掌纹
	feature2 = net.extract_feature(final_roi_imageEnhancePP);
    feature2_flatten = feature2.reshape(feature2.w * feature2.h * feature2.c);
    for (int j=0; j<feature2_flatten.w; j++)
    {
        feature[j] = feature2_flatten[j]; // 99
    }
    float feature_arrayPP[feature.size()];
    memcpy(feature_arrayPP,&feature[0],feature.size()*sizeof(feature[0]));

    result final_result;
	// clock_t start_time, end_time;
	// start_time = clock();
    final_result = recognition(feature_arrayPV, feature_arrayPP);
	// end_time = clock();
	// qDebug() << "ncnn recog time : " << ((end_time - start_time) * 1000.0 / CLOCKS_PER_SEC) << "ms\n";
//    ui->textBrowser->setFontPointSize(24);
//    ui->textBrowser->setFontWeight(75);
//    std::string string= "result:id="+final_result.id+"\n";
    cout<<"id:"<<final_result.id<<" score:"<<final_result.score<<endl;
    if (final_result.score < 0.25f && final_result.score > 0)
    {
        ui->textBrowser->setFontPointSize(20);
        ui->textBrowser->setFontWeight(QFont::Normal);
        // QString qstring = "认证结果 : "+QString::number(final_result.id)+"\n";
		QString qstring = "认证结果 : "+QString::fromStdString(final_result.id)+"\n";
        ui->textBrowser->insertPlainText(qstring);
        qstring = "度量距离 : " + QString::number(final_result.score) + "\n";
        ui->textBrowser->insertPlainText(qstring);
        ui->textBrowser->moveCursor(QTextCursor::End);
    }
	else if(final_result.score == 3)
    {
        ui->textBrowser->setFontPointSize(20);
        ui->textBrowser->setFontWeight(QFont::Normal);
        QString qstring = "数据库为空 \n";
        ui->textBrowser->insertPlainText(qstring);
        ui->textBrowser->moveCursor(QTextCursor::End);
    }
    else
    {
        ui->textBrowser->setFontPointSize(20);
        ui->textBrowser->setFontWeight(QFont::Normal);
        QString qstring = "认证结果 : 未注册用户 \n";
        ui->textBrowser->insertPlainText(qstring);
        qstring = "度量距离 : " + QString::number(final_result.score) + "\n";
        ui->textBrowser->insertPlainText(qstring);
        ui->textBrowser->moveCursor(QTextCursor::End);
		if(final_result.score < 0.3f){
			char score[10];
			std::string str(score);
			sprintf(score, "%.3f", final_result.score);
			string ImgPath = hardOriginal + final_result.id + score + "_" + timeStamp + ".bmp";
			cv::imwrite(ImgPath, matOriginalPV);
			ImgPath = hardRoi + final_result.id + score + "_" + timeStamp + ".bmp";
			cv::imwrite(ImgPath, final_roi_image);
		}
    }
    // mutex_main.lock();
    // rec_reg_finished = true;
    // mutex_main.unlock();

}

// NCNN版本
void Dialog::change_state_reg()
{
	ui->textBrowser->clear();
	QString id;
	id = ui->textEdit->toPlainText();
	int length = id.length();
	if(length < 2 || length > 18){
		ui->textBrowser->setFontPointSize(20);
        ui->textBrowser->setFontWeight(QFont::Normal);
        ui->textBrowser->insertPlainText("请正确输入注册用户ID !!! \n");
        ui->textBrowser->moveCursor(QTextCursor::End);
		return;
	}
	else{

		// sql
		if(QSqlDatabase::contains("qt_sql_default_connection"))
			db = QSqlDatabase::database("qt_sql_default_connection");
		else
			db = QSqlDatabase::addDatabase("QSQLITE");
		db.setDatabaseName(QApplication::applicationDirPath()+"/database.dat");
		db.open();

		QSqlQuery query_first;
    //    query_first.exec("CREATE TABLE IF NOT EXISTS featuretables(user int primary key, features1 blob, features2 blob,features3 blob,features4 blob,features5 blob,\
    //                     features6 blob,features7 blob, features8 blob, features9 blob, features10 blob)");
		query_first.exec("CREATE TABLE IF NOT EXISTS featuretables(user VARCHAR(20) primary key, features1 blob, features2 blob,features3 blob,features4 blob,features5 blob,\
                        features6 blob,features7 blob, features8 blob, features9 blob, features10 blob)");
		// QString c = QString("SELECT * from featuretables where user=%1").arg(id.toInt());
		QString c = QString("SELECT * from featuretables where user='%1'").arg(id); // 要加''才正常
	
		query_first.exec(c);
		if(query_first.next()){
			// qDebug()<<"error";
			ui->textBrowser->setFontPointSize(20);
			ui->textBrowser->setFontWeight(QFont::Normal);
			ui->textBrowser->insertPlainText("用户已存在!\n");
			ui->textBrowser->moveCursor(QTextCursor::End);
			// ui->textEdit->clear();
			// ui->textEdit->clearFocus();
			state = rec;
			reg_num = 0;
			db.close();
			// mutex_main.lock();
			// rec_reg_finished = true;
			// mutex_main.unlock();
			return;
		}
	}
	db.close();
    state = reg;
    reg_num = 0;
	ui->textBrowser->insertPlainText("开始采集\n");
	ui->textBrowser->moveCursor(QTextCursor::End);
}

// NCNN版本
void Dialog::register_id()
{
    ui->textBrowser->clear();
    int e_rror = 0;

    // mutex_main.lock();
    // rec_reg_finished = false;
    // mutex_main.unlock();

    QString id;
    // 读取id
    id = ui->textEdit->toPlainText();
    string string_id = id.toStdString();

    //ROI image
    cv::Mat register_matPV;
    cv::Mat register_roiPV, register_showImgPV;
	cv::Mat register_matPP, register_roiPP; 
    
    // capWebcam.read(register_mat);
    register_matPV = img_pv.clone();
	register_matPP = img_pp.clone();

    string defaultPath = "../run_file/qt_register/";
    string showPath = "../run_file/qt_register_valleypoint/";
	//保存注册时期拍到的图片
    string PPRoiPath = "../run_file/qt_register_roiPP/";
	string PVRoiPath = "../run_file/qt_register_roiPV/";
	//创建文件夹
    if (0 != access(PPRoiPath.c_str(), 0))
    {
        mkdir(PPRoiPath.c_str(),0777);   // 返回 0 表示创建成功，-1 表示失败
        //换成 ::_mkdir  ::_access 也行，不知道什么意思
    }
	if (0 != access(PVRoiPath.c_str(), 0))
    {
        mkdir(PVRoiPath.c_str(),0777);   // 返回 0 表示创建成功，-1 表示失败
        //换成 ::_mkdir  ::_access 也行，不知道什么意思
    }

	// sql
    if(QSqlDatabase::contains("qt_sql_default_connection"))
        db = QSqlDatabase::database("qt_sql_default_connection");
    else
        db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(QApplication::applicationDirPath()+"/database.dat");
    db.open();
	
    register_roiPV = getRoiImg(register_matPV, true, "1", e_rror, register_showImgPV); // BGR
	register_roiPP = register_matPP.clone(); //暂时这么写着
    if(e_rror == 1){
        qDebug() << "Can not get ROI !!!";
        ui->textBrowser->setFontPointSize(20);
        ui->textBrowser->setFontWeight(QFont::Normal);
        ui->textBrowser->insertPlainText("Can not get ROI !!!\n");
        ui->textBrowser->moveCursor(QTextCursor::End);
		db.close();
        // mutex_main.lock();
        // rec_reg_finished = true;
        // mutex_main.unlock();
        return;
    }
    reg_num += 1;
    qDebug() << reg_num;
    string save_dir = defaultPath + string_id + "_" + to_string(reg_num) + ".bmp";
    string show_dir = showPath + string_id + "_" + to_string(reg_num) + ".bmp";
    string savePVroi_dir = PVRoiPath + string_id + "_" + to_string(reg_num) + ".bmp";
	string savePProi_dir = PPRoiPath + string_id + "_" + to_string(reg_num) + ".bmp";
//    normalize2(register_roi);
    //   cv::imwrite("/home/pi/Downloads/roi.jpg",register_roi);
    // normalize(register_roi);
    cv::imwrite(show_dir,register_showImgPV);
    cv::imwrite(savePVroi_dir,register_roiPV);
	cv::imwrite(savePProi_dir,register_roiPP);
//   cv::Mat register_roi2 = img_normalize(register_roi);
    // ncnn_net net;
    // Image_normalize(register_roi, register_roi);
	//掌脉特征
    Enhancement(register_roiPV, register_roiPV);
    ncnn::Mat feature2 = net.extract_feature(register_roiPV);
    ncnn::Mat feature2_flatten = feature2.reshape(feature2.w * feature2.h * feature2.c);
	//待存储的特征向量
    vector<float> feature;
    feature.resize(feature2_flatten.w * 2);
    for (int j=0; j<feature2_flatten.w; j++)
    {
        feature[j] = feature2_flatten[j]; // 99
    }
	//掌纹特征
    Enhancement(register_roiPP, register_roiPP);
    feature2 = net.extract_feature(register_roiPP);
    feature2_flatten = feature2.reshape(feature2.w * feature2.h * feature2.c);
	for (int j=feature2_flatten.w; j<feature2_flatten.w*2; j++)
    {
        feature[j] = feature2_flatten[j-feature2_flatten.w]; // 99
    }
	//feature2_flatten.w * 2长度的特征向量，先是掌脉，再掌纹特征
    float feature_array[feature.size()];
    memcpy(feature_array,&feature[0],feature.size()*sizeof(feature[0]));
    
    QByteArray array;
    int len_feature = sizeof(feature_array);
    array.resize(len_feature);
    memcpy(array.data(),feature_array,len_feature);
    if(reg_num ==1)
    {
		QSqlQuery query_first;
		QString d = QString("INSERT INTO featuretables  VALUES(?,?,?,?,?,?,?,?,?,?,?)");
		query_first.prepare(d);
		// query_first.addBindValue(id.toInt());
		query_first.addBindValue(id);
		query_first.addBindValue(array);
		query_first.addBindValue(array);
		query_first.addBindValue(array);
		query_first.addBindValue(array);
		query_first.addBindValue(array);
		query_first.addBindValue(array);
		query_first.addBindValue(array);
		query_first.addBindValue(array);
		query_first.addBindValue(array);
		query_first.addBindValue(array);
		query_first.exec();
		db.close();
		ui->textBrowser->setFontPointSize(20);
		ui->textBrowser->setFontWeight(QFont::Normal);
		ui->textBrowser->insertPlainText("成功采集 : 1 / 10\n");
		ui->textBrowser->moveCursor(QTextCursor::End);
		cv::imwrite(save_dir,register_mat); // 原图
		// mutex_main.lock();
		// rec_reg_finished = true;
		// mutex_main.unlock();
		return;
		
	}

    QSqlQuery query;
    // QString d = QString("UPDATE featuretables SET features%1=:features where user=%2").arg(reg_num).arg(id.toInt());
	QString d = QString("UPDATE featuretables SET features%1=:features where user='%2'").arg(reg_num).arg(id);
    query.prepare(d);
    query.bindValue(":features",array);
    query.exec();
    ui->textBrowser->setFontPointSize(20);
    ui->textBrowser->setFontWeight(QFont::Normal);
    char reg_temp[50];
    sprintf(reg_temp,"成功采集 : %d / 10\n",reg_num);
    ui->textBrowser->insertPlainText(reg_temp);
    ui->textBrowser->moveCursor(QTextCursor::End);
    cv::imwrite(save_dir,register_mat);
    // mutex_main.lock();
    // rec_reg_finished = true;
    // mutex_main.unlock();
   if(reg_num == register_num)
   {
    //    ui->textBrowser->setFontPointSize(20);
    //    ui->textBrowser->setFontWeight(QFont::Normal);
       ui->textBrowser->insertPlainText("成功注册用户!\n");
       ui->textBrowser->moveCursor(QTextCursor::End);
       ui->textEdit->clear();
       ui->textEdit->clearFocus();
       state = rec;
       reg_num = 0;
       db.close();
    //    mutex_main.lock();
    //    rec_reg_finished = true;
    //    mutex_main.unlock();
       return;
   }
   db.close();
}

//NCNN版本
Dialog::result Dialog::recognition(float featurePV[], float featurePP[]){
    // float score;
    // int id;
	string id;
    float cur_score = 10;
    float max_dis = 10;
    float y = 0;
    vector<float> dist_listPV, dist_listPP, dist_list;
    vector<string> name_list;
    if(QSqlDatabase::contains("qt_sql_default_connection"))
                  db = QSqlDatabase::database("qt_sql_default_connection");
    else
                  db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(QApplication::applicationDirPath()+"/database.dat");
    db.open();

    QSqlQuery query;
    query.exec("SELECT * from featuretables");
    //bool ret = query.exec("select count(*) from featuretables");
    if(!query.next())
    {
        qDebug()<<"error";
        result final;
        final.id = "0";
        final.score = 3;
		db.close();
        return final;
    }
	clock_t start_time, end_time;
	start_time = clock();
   	query.first();
	do
    {
        //当前ID号
		QString cur_id1 = query.value(0).toString();
		string cur_id = cur_id1.toStdString();

//        qDebug()<<cur_id;
//        char curr_id[20];
//        sprintf(curr_id,"cur_id:%d",cur_id);
//        printf(curr_id);
		//读出10组array，包含10个掌脉+掌纹特征
        QByteArray out_array1 = query.value(1).toByteArray();
        QByteArray out_array2 = query.value(2).toByteArray();
        QByteArray out_array3 = query.value(3).toByteArray();
        QByteArray out_array4 = query.value(4).toByteArray();
        QByteArray out_array5 = query.value(5).toByteArray();
        QByteArray out_array6 = query.value(6).toByteArray();
        QByteArray out_array7 = query.value(7).toByteArray();
        QByteArray out_array8 = query.value(8).toByteArray();
		QByteArray out_array9 = query.value(9).toByteArray();
        QByteArray out_array10 = query.value(10).toByteArray();
        float out_featurePV1[feature_dim];
        float out_featurePV2[feature_dim];
        float out_featurePV3[feature_dim];
        float out_featurePV4[feature_dim];
        float out_featurePV5[feature_dim];
        float out_featurePV6[feature_dim];
        float out_featurePV7[feature_dim];
        float out_featurePV8[feature_dim];
		float out_featurePV9[feature_dim];
        float out_featurePV10[feature_dim];
		float out_featurePP1[feature_dim];
        float out_featurePP2[feature_dim];
        float out_featurePP3[feature_dim];
        float out_featurePP4[feature_dim];
        float out_featurePP5[feature_dim];
        float out_featurePP6[feature_dim];
        float out_featurePP7[feature_dim];
        float out_featurePP8[feature_dim];
		float out_featurePP9[feature_dim];
        float out_featurePP10[feature_dim];
//        float out_feature[feature_dim];
		//掌脉
		int HalfArrayLen = out_array1.size() / 2;
        memcpy(out_featurePV1,out_array1.data(),sizeof(out_featurePV1));
        memcpy(out_featurePV2,out_array2.data(),sizeof(out_featurePV2));
        memcpy(out_featurePV3,out_array3.data(),sizeof(out_featurePV3));
        memcpy(out_featurePV4,out_array4.data(),sizeof(out_featurePV4));
        memcpy(out_featurePV5,out_array5.data(),sizeof(out_featurePV5));
        memcpy(out_featurePV6,out_array6.data(),sizeof(out_featurePV6));
        memcpy(out_featurePV7,out_array7.data(),sizeof(out_featurePV7));
        memcpy(out_featurePV8,out_array8.data(),sizeof(out_featurePV8));
		memcpy(out_featurePV9,out_array9.data(),sizeof(out_featurePV9));
        memcpy(out_featurePV10,out_array10.data(),sizeof(out_featurePV10));
		//掌纹
		memcpy(out_featurePP1,out_array1.right(HalfArrayLen).data(),sizeof(out_featurePP1));
        memcpy(out_featurePP2,out_array2.right(HalfArrayLen).data(),sizeof(out_featurePP2));
        memcpy(out_featurePP3,out_array3.right(HalfArrayLen).data(),sizeof(out_featurePP3));
        memcpy(out_featurePP4,out_array4.right(HalfArrayLen).data(),sizeof(out_featurePP4));
        memcpy(out_featurePP5,out_array5.right(HalfArrayLen).data(),sizeof(out_featurePP5));
        memcpy(out_featurePP6,out_array6.right(HalfArrayLen).data(),sizeof(out_featurePP6));
        memcpy(out_featurePP7,out_array7.right(HalfArrayLen).data(),sizeof(out_featurePP7));
        memcpy(out_featurePP8,out_array8.right(HalfArrayLen).data(),sizeof(out_featurePP8));
		memcpy(out_featurePP9,out_array9.right(HalfArrayLen).data(),sizeof(out_featurePP9));
        memcpy(out_featurePP10,out_array10.right(HalfArrayLen).data(),sizeof(out_featurePP10));

        vector <float*> out_featurePV; // 一个人的PVfeature
        out_featurePV.push_back(out_featurePV1);
        out_featurePV.push_back(out_featurePV2);
        out_featurePV.push_back(out_featurePV3);
        out_featurePV.push_back(out_featurePV4);
        out_featurePV.push_back(out_featurePV5);
        out_featurePV.push_back(out_featurePV6);
        out_featurePV.push_back(out_featurePV7);
        out_featurePV.push_back(out_featurePV8);
		out_featurePV.push_back(out_featurePV9);
        out_featurePV.push_back(out_featurePV10);

		vector <float*> out_featurePP; // 一个人的PPfeature
        out_featurePP.push_back(out_featurePP1);
        out_featurePP.push_back(out_featurePP2);
        out_featurePP.push_back(out_featurePP3);
        out_featurePP.push_back(out_featurePP4);
        out_featurePP.push_back(out_featurePP5);
        out_featurePP.push_back(out_featurePP6);
        out_featurePP.push_back(out_featurePP7);
        out_featurePP.push_back(out_featurePP8);
		out_featurePP.push_back(out_featurePP9);
        out_featurePP.push_back(out_featurePP10);
    
        float temp_score = 10;
		float AlphaPV = 0.5;
    //  cur_score = 10;
	// 一个人的掌纹和掌脉的匹配分数
        for(unsigned int i = 0; i < out_featurePV.size(); i++)
        {
            temp_score = cal_sim(featurePV,out_featurePV[i]); // 返回的是距离
            dist_listPV.push_back(temp_score);
            // name_list.push_back(int2str(cur_id));
			name_list.push_back(cur_id);
        }
		for(unsigned int i = 0; i < out_featurePP.size(); i++)
        {
            temp_score = cal_sim(featurePP,out_featurePP[i]); // 返回的是距离
            dist_listPP.push_back(temp_score);
        }
		
    //    for(int i=0;i<feature_dim;i++)
    //    {
    //        out_feature[i] = (out_feature1[i]+out_feature2[i]+out_feature3[i]+out_feature4[i]+out_feature5[i]+out_feature6[i]+out_feature7[i]+out_feature8[i])*1.0/8;
    //    }

    //    cur_score = cal_sim(feature,out_feature);
    //     if (cur_score<max_dis)
    //     {
    //         id =cur_id;
    //         score = cur_score;
    //         max_dis=cur_score;

    //     }
    }while (query.next());
	//融合之后的距离
	for(unsigned int i = 0; i < dist_listPV.size(); i++){
		dist_list.at(i) = dist_listPV.at(i)*AlphaPV + dist_listPP.at(i)*(1-AlphaPV);
	}
	end_time = clock();
	qDebug() << "ncnn recog time: " << (end_time - start_time) * 1000 / CLOCKS_PER_SEC << "ms\n";
    vector<size_t> idx_list = sort_indexes<float>(dist_list); // 升序的，距离越小越接近
    float dist_temp;
    string name_temp;
//    cout << "idx_list situation:" << endl;
//    for(size_t i = 0; i < 5; i++)// 升序的，距离越小越接近
//    {
//        dist_temp = dist_list[idx_list[i]];
//        name_temp = name_list[idx_list[i]];
//        cout << 'name:' << name_temp << ",dist:" << setprecision(6) << dist_temp << endl;
//    }
    int not_found = 0;
    vector<pair<string, int>> vote_list;
    for(size_t i = 0; i < 5; i++) //最接近的5人
    {
        name_temp = name_list[idx_list[i]];
        not_found = 0;

        for(size_t j = 0; j < vote_list.size(); j++)
        {
            if(vote_list[j].first == name_temp) //同样id，数量++
            {
                // y = (-0.07f) * (i + 1) + 1.1f;
                vote_list[j].second ++;
            }
            else
            {
                not_found++;
            }
        }
        if(not_found == vote_list.size()) //找遍了，没有同id的
        {
            // y = -0.07f * (i + 1) + 1.1f;
            pair<string, int> vote_new;
            vote_new.first = name_temp;
            vote_new.second = 1;
            vote_list.push_back(vote_new);
        }
    }
    cout << "vote_list situation:" << endl;
    for(size_t i = 0; i < vote_list.size(); i++)
    {
        cout << "person:" << vote_list[i].first << ",times:" << setprecision(2) << vote_list[i].second << endl;
    }

    int max_vote = 0; // 出现次数
    string max_id;
    for(size_t i = 0; i < vote_list.size(); i++)
    {
        if(max_vote < vote_list[i].second)
        {
            max_vote = vote_list[i].second;
            max_id = vote_list[i].first; // find the man appears most
        }
    }
	cout << "output situation:" << endl;
    for(size_t i = 0; i < 5; i++)
    {
        name_temp = name_list[idx_list[i]];
        dist_temp = dist_list[idx_list[i]];
		cout << "person:" << name_temp << ",dis:" << setprecision(2) << dist_temp << endl;

        if(name_temp == max_id)
        {
            if(max_dis > dist_temp)
            {
                max_dis = dist_temp;
                // id = str2int(name_temp);
				id = name_temp;
            }
        }
    }
    if(max_vote == 5 && max_dis < 0.28f && max_dis >= 0.2f)
    {
        max_dis = 0.199f;
    }

    query.first();
    result final;
    final.id = id;
    final.score = max_dis;

    db.close();
    return final;
}

// NCNN
void Dialog::Enhancement(Mat input, Mat &output){

    cv::resize(input, input, Size(320, 320), 0, 0);
    Mat input2 = input.clone();
    cvtColor(input2, input2, COLOR_BGR2GRAY);
    Ptr<CLAHE> clahe = createCLAHE();
    clahe->setClipLimit(2.0);
    clahe->setTilesGridSize(Size(8, 8));
    clahe->apply(input2, input2);
    cvtColor(input2, input2, COLOR_GRAY2BGR);
    output = input2.clone();
}

#else

// MCM提取特征的识别函数
void Dialog::show_reco()
{
    int e_rror = 0;
    
    ui->textBrowser->clear();

    Mat matOriginal = matOriginal_main.clone();

    cv::Mat final_roi_image, reco_showImg;
	cv::imwrite("../run_file/reco_original.bmp",matOriginal);
    final_roi_image = getRoiImg(matOriginal, true, "reco.bmp", e_rror, reco_showImg);
//    normalize2(final_roi_image);
    cv::imwrite("../run_file/reco_roi.bmp",final_roi_image);
    cv::imwrite("../run_file/valleypoint_reco.bmp",reco_showImg);
	if(e_rror == 1){
        qDebug() << "Can not get ROI !!!";
        // ui->textBrowser->setFontPointSize(20);
        // ui->textBrowser->setFontWeight(QFont::Normal);
        ui->textBrowser->insertPlainText("Can not get ROI !!!\n");
        ui->textBrowser->moveCursor(QTextCursor::End);
        
        return;
    }

    // Enhancement(final_roi_image, final_roi_image); // resize roi
    // cv::imwrite("../run_file/Enhance_reco.bmp",final_roi_image);
    
	vector<Point> PointVector_rec;
	vector<vector<float>> FeatureDescriptor_rec;
	featurePointAndDescriptorExtract(final_roi_image, PointVector_rec, FeatureDescriptor_rec); //提取特征点和描述子

	// QString str = "识别中...";
	// ui->textEdit->setPlainText(str);

    result final_result; 
	clock_t start_time, end_time;
	start_time = clock();
    final_result = recognition(PointVector_rec, FeatureDescriptor_rec);
	end_time = clock();
	qDebug() << "ncnn recog time : " << (end_time - start_time) * 1000 / CLOCKS_PER_SEC << "ms\n";

    cout<<"id:"<<final_result.id<<" score:"<<final_result.score<<endl;
    if (final_result.score > 0.35f)
    {
		ui->textBrowser->clear();
        // ui->textBrowser->setFontPointSize(20);
        // ui->textBrowser->setFontWeight(QFont::Normal);
        // QString qstring = "认证结果 : "+QString::number(final_result.id)+"\n";
		QString qstring = "认证结果 : "+QString::fromStdString(final_result.id)+"\n";
        ui->textBrowser->insertPlainText(qstring);
        qstring = "相似程度 : " + QString::number(final_result.score) + "\n";
        ui->textBrowser->insertPlainText(qstring);
        ui->textBrowser->moveCursor(QTextCursor::End);
    }
	else if(final_result.score == 0)
    {
		ui->textBrowser->clear();
        // ui->textBrowser->setFontPointSize(20);
        // ui->textBrowser->setFontWeight(QFont::Normal);
        QString qstring = "数据库为空 \n";
        ui->textBrowser->insertPlainText(qstring);
        ui->textBrowser->moveCursor(QTextCursor::End);
    }
    else
    {
		ui->textBrowser->clear();
        // ui->textBrowser->setFontPointSize(20);
        // ui->textBrowser->setFontWeight(QFont::Normal);
        QString qstring = "认证结果 : 未注册用户 \n";
        ui->textBrowser->insertPlainText(qstring);
        qstring = "相似程度 : " + QString::number(final_result.score) + "\n";
        ui->textBrowser->insertPlainText(qstring);
        ui->textBrowser->moveCursor(QTextCursor::End);
    }
	// ui->textEdit->clear();
	// ui->textEdit->clearFocus();
}


// MCM版本
void Dialog::change_state_reg()
{
	ui->textBrowser->clear();
	QString id;
	id = ui->textEdit->toPlainText();
	int length = id.length();
	if(length < 2 || length > 18){
		// ui->textBrowser->setFontPointSize(20);
        // ui->textBrowser->setFontWeight(QFont::Normal);
        ui->textBrowser->insertPlainText("请正确输入注册用户ID !!! \n");
        ui->textBrowser->moveCursor(QTextCursor::End);
		return;
	}
	else{

		// sql
		if(QSqlDatabase::contains("qt_sql_default_connection"))
			db = QSqlDatabase::database("qt_sql_default_connection");
		else
			db = QSqlDatabase::addDatabase("QSQLITE");
		db.setDatabaseName(QApplication::applicationDirPath()+"/databaseMCM.dat");
		db.open();

		QSqlQuery query_first;
    //    query_first.exec("CREATE TABLE IF NOT EXISTS featuretables(user int primary key, features1 blob, features2 blob,features3 blob,features4 blob,features5 blob,\
    //                     features6 blob,features7 blob, features8 blob, features9 blob, features10 blob)");
		query_first.exec("CREATE TABLE IF NOT EXISTS featuretables(user VARCHAR(20) primary key, Pointset1 blob, Descriptor1 blob, Pointnum1 int, Pointset2 blob, Descriptor2 blob, Pointnum2 int,\
                        Pointset3 blob, Descriptor3 blob, Pointnum3 int, Pointset4 blob, Descriptor4 blob, Pointnum4 int, Pointset5 blob, Descriptor5 blob, Pointnum5 int, Pointset6 blob, Descriptor6 blob, Pointnum6 int)");
		// QString c = QString("SELECT * from featuretables where user=%1").arg(id.toInt());
		QString c = QString("SELECT * from featuretables where user='%1'").arg(id); // 要加''才正常
	
		query_first.exec(c);
		if(query_first.next()){
			// qDebug()<<"error";
			// ui->textBrowser->setFontPointSize(20);
			// ui->textBrowser->setFontWeight(QFont::Normal);
			ui->textBrowser->insertPlainText("用户已存在!\n");
			ui->textBrowser->moveCursor(QTextCursor::End);
			// ui->textEdit->clear();
			// ui->textEdit->clearFocus();
			state = rec;
			reg_num = 0;
			db.close();
			// mutex_main.lock();
			// rec_reg_finished = true;
			// mutex_main.unlock();
			return;
		}
	}
	db.close();
    state = reg;
    reg_num = 0;
	ui->textBrowser->insertPlainText("开始采集\n");
	ui->textBrowser->moveCursor(QTextCursor::End);
}



// MCM版本
void Dialog::register_id()
{
    ui->textBrowser->clear();
    int e_rror = 0;

    QString id;
    // 读取id
    id = ui->textEdit->toPlainText();
    string string_id = id.toStdString();
    
    cv::Mat register_mat;
    cv::Mat register_roi, register_showImg;
    
    // capWebcam.read(register_mat);
    register_mat = matOriginal_main.clone();

    string defaultPath = "../run_file/qt_register/";
    string showPath = "../run_file/qt_register_valleypoint/";
    string RoiPath = "../run_file/qt_register_roi/";
    
	// sql
    if(QSqlDatabase::contains("qt_sql_default_connection"))
        db = QSqlDatabase::database("qt_sql_default_connection");
    else
        db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(QApplication::applicationDirPath()+"/databaseMCM.dat");
    db.open();
	
    register_roi = getRoiImg(register_mat, true, "1", e_rror, register_showImg); // BGR
    if(e_rror == 1){
        qDebug() << "Can not get ROI !!!";
        // ui->textBrowser->setFontPointSize(20);
        // ui->textBrowser->setFontWeight(QFont::Normal);
        ui->textBrowser->insertPlainText("Can not get ROI !!!\n");
        ui->textBrowser->moveCursor(QTextCursor::End);
		db.close();
        // mutex_main.lock();
        // rec_reg_finished = true;
        // mutex_main.unlock();
        return;
    }
    reg_num += 1;
    qDebug() << reg_num;
    string save_dir = defaultPath + string_id + "_" + to_string(reg_num) + ".bmp";
    string show_dir = showPath + string_id + "_" + to_string(reg_num) + ".bmp";
    string saveroi_dir = RoiPath + string_id + "_" + to_string(reg_num) + ".bmp";

    cv::imwrite(show_dir,register_showImg);
    cv::imwrite(saveroi_dir,register_roi);

    // Enhancement(register_roi, register_roi); // resize roi
    vector<Point> PointVector1;
	vector<vector<float>>FeatureDescriptor1;
	featurePointAndDescriptorExtract(register_roi, PointVector1, FeatureDescriptor1); // 点集和描述子

    QByteArray array1; // point set
	int len_point = sizeof(Point) * PointVector1.size();
    int point_num = PointVector1.size();
    array1.resize(len_point);
    memcpy(array1.data(), &PointVector1[0], len_point);

	QByteArray array2; // descriptor
    vector<float> Des_tmp(point_num * FEATURE_POINT_DIM);
    int len_Des = point_num * FEATURE_POINT_DIM * sizeof(float);
    array2.resize(len_Des);
	for(int i = 0; i < point_num; i++){ // change to 1 dim
        for(int j = 0; j < FEATURE_POINT_DIM; j++){
            Des_tmp[i * FEATURE_POINT_DIM + j] = FeatureDescriptor1[i][j];
        }
    }
    memcpy(array2.data(), &Des_tmp[0], len_Des);

    if(reg_num ==1)
    {
		QSqlQuery query_first;
		QString d = QString("INSERT INTO featuretables  VALUES(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)");
		query_first.prepare(d);
		
		query_first.addBindValue(id);
		query_first.addBindValue(array1);
		query_first.addBindValue(array2);
		query_first.addBindValue(point_num);
		query_first.addBindValue(array1);
		query_first.addBindValue(array2);
		query_first.addBindValue(point_num);
		query_first.addBindValue(array1);
		query_first.addBindValue(array2);
		query_first.addBindValue(point_num);
		query_first.addBindValue(array1);
		query_first.addBindValue(array2);
		query_first.addBindValue(point_num);
		query_first.addBindValue(array1);
		query_first.addBindValue(array2);
		query_first.addBindValue(point_num);
		query_first.addBindValue(array1);
		query_first.addBindValue(array2);
		query_first.addBindValue(point_num);
		query_first.exec();
		db.close();
		// ui->textBrowser->setFontPointSize(20);
		// ui->textBrowser->setFontWeight(QFont::Normal);
		ui->textBrowser->insertPlainText("成功采集 : 1 / 6\n");
		ui->textBrowser->moveCursor(QTextCursor::End);
		cv::imwrite(save_dir,register_mat); // 原图
		return;
		
	}

    QSqlQuery query;
    
	QString d = QString("UPDATE featuretables SET Pointset%1=:pointset, Descriptor%1=:descriptor, Pointnum%1=:pointnum where user='%2'").arg(reg_num).arg(id);
    query.prepare(d);
    query.bindValue(":pointset",array1); // update the database
	query.bindValue(":descriptor",array2);
	query.bindValue(":pointnum",point_num);
    query.exec();
    // ui->textBrowser->setFontPointSize(20);
    // ui->textBrowser->setFontWeight(QFont::Normal);
    char reg_temp[50];
    sprintf(reg_temp, "成功采集 : %d / 6\n", reg_num);
    ui->textBrowser->insertPlainText(reg_temp);
    ui->textBrowser->moveCursor(QTextCursor::End);
    cv::imwrite(save_dir,register_mat);
    
	if(reg_num == register_num)
	{
		ui->textBrowser->insertPlainText("成功注册用户!\n");
		ui->textBrowser->moveCursor(QTextCursor::End);
		ui->textEdit->clear();
		ui->textEdit->clearFocus();
		state = rec;
		reg_num = 0;
		db.close();
		
		return;
	}
	db.close();
}


// MCM版本
Dialog::result Dialog::recognition(vector<Point> PointVector_rec, vector<vector<float>> FeatureDescriptor_rec){
    // float score;
    // int id;
	string id;
    // float cur_score = 10;
    double max_score = 0;
    // float y = 0;
    vector<double> dist_list;
    vector<string> name_list;
    if(QSqlDatabase::contains("qt_sql_default_connection"))
                  db = QSqlDatabase::database("qt_sql_default_connection");
    else
                  db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(QApplication::applicationDirPath()+"/databaseMCM.dat");
    db.open();

    QSqlQuery query;
    query.exec("SELECT * from featuretables");
    //bool ret = query.exec("select count(*) from featuretables");
    if(!query.next())
    {
        qDebug()<<"error";
        result final;
        final.id = "0";
        final.score = 0;
		db.close();
        return final;
    }
    
	// ui->textBrowser->clear();
	// ui->textBrowser->setFontPointSize(20);
	// ui->textBrowser->setFontWeight(QFont::Normal);
	// QString qstring = "识别中... \n";
	// ui->textBrowser->insertPlainText(qstring);
	// ui->textBrowser->moveCursor(QTextCursor::End);
   	query.first();
	do
    {
		clock_t start, end;
		start = clock();
        // int cur_id = query.value(0).toInt();
		QString cur_id1 = query.value(0).toString();
		string cur_id = cur_id1.toStdString();
		for(unsigned int num = 0; num < register_num; num++){ // 一个ID的6组数据
			// 读取数据
			QByteArray arrayPointset = query.value(1 + num * 3).toByteArray();
			QByteArray arrayDescriptor = query.value(2 + num * 3).toByteArray();
			int Pointnum = query.value(3 + num * 3).toInt();
			// 处理数据
			vector<Point> Pointset_tmp(Pointnum);
			int len_point = sizeof(Point) * Pointnum;
			memcpy((char*)&Pointset_tmp[0], arrayPointset.data(), len_point);

			int len_Des = Pointnum * FEATURE_POINT_DIM * sizeof(float);
			vector<vector<float>> Des1_tmp(Pointnum, vector<float>(FEATURE_POINT_DIM)); //二维用于计算相似性
    		vector<float> Des2_tmp(Pointnum * FEATURE_POINT_DIM); //一维用于读出数据
			memcpy((char*)&Des2_tmp[0], arrayDescriptor.data(), len_Des);
			
			for(int i = 0; i < Pointnum; i++){ // 二维化
				for(int j = 0; j < FEATURE_POINT_DIM; j++){
					Des1_tmp[i][j] = Des2_tmp[i * FEATURE_POINT_DIM + j];
				}
    		}
			
			// 计算距离
			// clock_t start2, end2;
			// start2 = clock();
			vector<PointMATCH> matchList;
			SearchCorrespondingPoint(PointVector_rec, FeatureDescriptor_rec, Pointset_tmp, Des1_tmp, matchList);
			int imgW = NORM_WIDTH;
			int imgH = NORM_HEIGHT;
			// clock_t start2, end2;
			// start2 = clock();
			MatchPointGlobalFilter(matchList, imgW, imgH);
			double temp_score = double(2 * matchList.size()) / (FeatureDescriptor_rec.size() + Des1_tmp.size());
			// end2 = clock();
			// qDebug() << "one hand match time: " << (end2 - start2) * 1000 / CLOCKS_PER_SEC << "ms\n";
			dist_list.push_back(temp_score);
			name_list.push_back(cur_id);
		}
		end = clock();
		qDebug() << "one user time: " << (end - start) * 1000 / CLOCKS_PER_SEC << "ms\n";
    
    }while (query.next());
    vector<size_t> idx_list = sort_indexes<double>(dist_list); // 降序的，相似性
    double dist_temp;
    string name_temp;

    int not_found = 0;
    vector<pair<string, int>> vote_list;
    for(size_t i = 0; i < 5; i++) //最接近的5人
    {
        name_temp = name_list[idx_list[i]];
        not_found = 0;

        for(size_t j = 0; j < vote_list.size(); j++)
        {
            if(vote_list[j].first == name_temp) //同样id，数量++
            {
                // y = (-0.07f) * (i + 1) + 1.1f;
                vote_list[j].second ++;
            }
            else
            {
                not_found++;
            }
        }
        if(not_found == vote_list.size()) //找遍了，没有同id的
        {
            // y = -0.07f * (i + 1) + 1.1f;
            pair<string, int> vote_new;
            vote_new.first = name_temp;
            vote_new.second = 1;
            vote_list.push_back(vote_new);
        }
    }
    cout << "vote_list situation:" << endl;
    for(size_t i = 0; i < vote_list.size(); i++)
    {
        cout << "person:" << vote_list[i].first << ",times:" << setprecision(2) << vote_list[i].second << endl;
    }

    int max_vote = 0; // find the man appears most
    string max_id;
    for(size_t i = 0; i < vote_list.size(); i++)
    {
        if(max_vote < vote_list[i].second)
        {
            max_vote = vote_list[i].second;
            max_id = vote_list[i].first; // find the man appears most
        }
    }
	// cout << "output situation:" << endl;
    for(size_t i = 0; i < 5; i++)
    {
        name_temp = name_list[idx_list[i]];
        dist_temp = dist_list[idx_list[i]]; // similarity
		// cout << "person:" << name_temp << ",dis:" << setprecision(2) << dist_temp << endl;

        if(name_temp == max_id)
        {
            if(max_score < dist_temp)
            {
                max_score = dist_temp;
                // id = str2int(name_temp);
				id = name_temp;
            }
        }
    }
    // if(max_vote >= 4)
    // {
    //     float y = (-0.075f) * max_vote + 1.0f;
    //     max_score *= y;
    // }

    query.first();
    result final;
    final.id = id;
    final.score = max_score;

    db.close();
    return final;
}


// MCM
void Dialog::Enhancement(Mat input, Mat &output){

    cv::resize(input, input, Size(128, 128), 0, 0);
    Mat input2 = input.clone();
    // cvtColor(input2, input2, COLOR_BGR2GRAY);
    // Ptr<CLAHE> clahe = createCLAHE();
    // clahe->setClipLimit(2.0);
    // clahe->setTilesGridSize(Size(8, 8));
    // clahe->apply(input2, input2);
    // cvtColor(input2, input2, COLOR_GRAY2BGR);
    output = input2.clone();
}
#endif // NCNN

void Dialog::registered_num()
{
	/*
	if(QSqlDatabase::contains("qt_sql_default_connection"))
        db = QSqlDatabase::database("qt_sql_default_connection");
    else
        db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(QApplication::applicationDirPath()+"/database.dat");
	db.open();
	
	QSqlQuery query;
	if(db.driver()->hasFeature(QSqlDriver::QuerySize)){
		register_num = query.size();
	}
	else{
		query.last();
        register_num = query.at() + 1;
	}
	qDebug() << "------the registered_num is : " << register_num << "------";
	*/
	return;
}



void Dialog::show_register_ok()
{
    QString reg_ok = QString::fromStdString("register ok! \n");
}
void Dialog::show_running()
{
    QString sim = QString::fromStdString("similarity:");
    QString usr = QString::fromStdString("usr:");
}

// 统计灰度信息的特征提取
vector<float> Dialog::extract_feature(cv::Mat roi){
//    QImage q_roi_image = ui->roi->pixmap()->toImage();
//    qDebug()<<q_roi_image.format();
    cv::Mat feature_frame;
    vector<float> temp_feature;
    vector<float> feature;
    feature_frame = roi;

    cv::Size size = cv::Size(150,150);
    cv::resize(feature_frame,feature_frame,size,0,0,cv::INTER_AREA);

    float sum =0;
    for (int i=0;i<=9;i++)
    {
        for(int j=0;j<=9;j++)
        {
            sum=0;
            for(int m=0;m<=14;m++)
            {
                for(int n=0;n<=14;n++)
                {
                    sum+=feature_frame.at<uchar>(i*15+m,j*15+n);
                }
            }
            temp_feature.push_back(sum);

        }
    }
    feature.insert(feature.begin(),temp_feature[1]-temp_feature[0]);
    for(int k=1;k<=98;k++)
    {
        feature.push_back(temp_feature[k+1]-temp_feature[k]);
    }
    return feature;


}

void Dialog::detectAndDisplay(Mat frame) //截取roi，版本2, opencv自带检测器
{
    return;
    // vector<Rect> palms;
    // // vector<vector<Rect>> palms_bank;
    // Mat frame_gray, frame_draw;
    // frame_draw = frame.clone();
    // cvtColor(frame, frame_gray, COLOR_BGR2GRAY);
    // equalizeHist(frame_gray, frame_gray);

    // //palm detection
    // // palm_cascade.detectMultiScale(frame_gray, palms, 1.1, 2, 0|CASCADE_SCALE_IMAGE, Size(24,24));
    // palm_cascade.detectMultiScale(frame_gray, palms, 1.2, 2, 0 | CASCADE_DO_CANNY_PRUNING | CASCADE_SCALE_IMAGE \
    //                                 | CASCADE_FIND_BIGGEST_OBJECT, Size(19, 19), Size(480,640));
    // // palms_bank.push_back(palms);

    // // vector<Rect> palmAll;
    // // RestoreVectors(palms_bank, palmAll);
    // groupRectangles(palms, 1);

    // cout << "Rect location:" << endl;
    // for(size_t j = 0; j < palms.size(); j++)
    // {
    //     rectangle(frame_draw, palms[j], Scalar(255, 0, 0), 2);
    //     cout << palms[j] << " ";
    // }
    // cout << endl;
    // cv::imwrite("/home/pi/Downloads/palm_detect.jpg", frame_draw);
}

void Dialog::RestoreVectors(vector<vector<Rect>>& vecs_bank, vector<Rect>& vecAll)
{
    for(size_t i = 0; i < vecs_bank.size(); i++)
    {
        vecAll.insert(vecAll.end(), vecs_bank[i].begin(), vecs_bank[i].end());
    }
}


//*******************************
cv::Mat Dialog::run_roi(cv::Mat matOriginal){ //截取roi，版本1
    float sum=0;
    cv::Mat roi_frame;
    cv::Mat roi_img;
    cv::Mat final_roi_image;
//    roi_img = cv::imread("/home/pi/Downloads/1-0.jpg");
    roi_frame = matOriginal.clone();
    roi_img = matOriginal;
//    roi_frame = cv::imread("/home/pi/Downloads/1-0.jpg");
    cv::Mat close;
    cv::cvtColor(roi_frame, roi_frame, CV_BGR2GRAY);
    QImage gray((uchar*)roi_frame.data, roi_frame.cols, roi_frame.rows, roi_frame.step, QImage::Format_Grayscale8);
    gray.save("/home/pi/Downloads/gray.jpg","JPG",100);
    //  cv::threshold(roi_frame, roi_frame, 127, 255, cv::THRESH_OTSU);
   cv::threshold(roi_frame, roi_frame, OTSU(roi_frame), 255, cv::THRESH_BINARY);
//    adaptiveThreshold(roi_frame, roi_frame, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, 7, 2);
//    QImage qthresh((uchar*)roi_frame.data, roi_frame.cols, roi_frame.rows, roi_frame.step, QImage::Format_Grayscale8);
//    qthresh.save("/home/pi/Downloads/thresh.jpg","JPG",100);
//    std::vector<std::vector<cv::Point> > contours;
//    std::vector<cv::Vec4i> hierarchy;
    //Find contours
//    cv::findContours(roi_frame, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0,0));
    cv::morphologyEx(roi_frame, close, CV_MOP_CLOSE, cv::Mat1b(3,3,1));
    cv::threshold(close, close, 127, 255, cv::THRESH_BINARY_INV);
    QImage qclose((uchar*)close.data, close.cols, close.rows, close.step, QImage::Format_Grayscale8);
    qclose.save("/home/pi/Downloads/close.jpg","JPG",100);
    int lx = 0,rx = close.cols - 1;
    cv::Size close_size = cv::Size(close.cols,close.rows);
//    cout<<"close_size.width: "<<close_size.width<<"  close_size.height: "<<close_size.height<<endl;

    for(int i=0;i<(close_size.width)/2;i++)
    {
        sum=0;
        for(int j=(close_size.height)/2-150;j<(close_size.height)/2+150;j++)
        {
            sum+=close.at<uchar>(j,i);
        }
        if (sum<20)
        {
            lx=i;
            break;
        }
        
    }

    for(int i=close_size.width-1;i>(close_size.width)/2;i=i-1)
    {
        sum=0;
        for(int j=(close_size.height)/2-150;j<(close_size.height)/2+150;j++)
        {
            sum+=close.at<uchar>(j,i);
        }
        if (sum<20)
        {
            rx=i;
            break;
        }
        
    }

    int range_l = lx;
    int range_r = rx;

    int ry = close.rows - 1;
    int ly=0;

    for(int i=close_size.height-1;i>(close_size.height)/2;i--)
    {
        sum=0;
        for(int j=range_l;j<range_r;j++)
        {
            sum+=close.at<uchar>(i,j);
        }
        if (sum<15)
        {
            ry=i;
            break;
        }
        
    }
    for(int i=0;i<(close_size.height)/2;i++)
    {
        sum=0;
        for(int j=range_l;j<range_r;j++)
        {
            sum+=close.at<uchar>(i,j);
        }
        if (sum<15)
        {
            ly=i;
            break;
        }
        
    }

    cv::Rect rect = cv::Rect(lx,ly,rx-lx,ry-ly);
    if (rect.area()>0 and(ry<roi_img.rows)and(rx<roi_img.cols))
    {
        std::cout<<"ry:"<<ry<<endl;
        std::cout<<"rx:"<<rx<<endl;
        std::cout<<"ly:"<<ly<<endl;
        std::cout<<"lx:"<<lx<<endl;
        std::cout<<"cols:"<<roi_img.cols<<endl;
        std::cout<<"rows:"<<roi_img.rows<<endl;
        roi_img(rect).copyTo(final_roi_image);
        return final_roi_image;
    }
    else
    {
        final_roi_image = matOriginal;
        return final_roi_image ;
    }

}

void Dialog::database(){
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(QApplication::applicationDirPath()+"/features.dat");
    bool ret = db.open();
    int feature[4]={1,2,3,4};
    QByteArray array;
    int len_feature = sizeof(feature);
    array.resize(len_feature);
    memcpy(array.data(),feature,len_feature);


   QSqlQuery query;

   if(!query.exec("SELECT features from feature"))
       qDebug()<<"error";
   query.first();
   QByteArray out_array = query.value(0).toByteArray();
   int out_feature[4];
   memcpy(out_feature,out_array.data(),len_feature);


}

float Dialog::cal_sim(float feature1[],float feature2[])
{
    float sum=0;
    float dist=0;
    for(int i=0;i<feature_dim;i++)
    {
        sum+=feature1[i]*feature2[i];
    }
    float norm1=0;
    for(int i=0;i<feature_dim;i++)
    {
        norm1+=feature1[i]*feature1[i];
    }
    norm1=sqrt(norm1);
    float norm2=0;
    for(int i=0;i<feature_dim;i++)
    {
        norm2+=feature2[i]*feature2[i];
    }
    norm2=sqrt(norm2);

    dist=1-(sum/(norm1*norm2));
    return dist;
}

// cv::Mat Dialog::img_normalize(cv::Mat roi_img)
// {
//     cv::Mat roi_img2 = roi_img.clone();
//     roi_img2.at<cv::Vec3b>(0,0)[0] = 0;
//     roi_img2.at<cv::Vec3b>(0,0)[1] = 0;
//     roi_img2.at<cv::Vec3b>(0,0)[2] = 0;
//     roi_img2.at<cv::Vec3b>(0,1)[0] = 255;
//     roi_img2.at<cv::Vec3b>(0,1)[1] = 255;
//     roi_img2.at<cv::Vec3b>(0,1)[2] = 255;
//     cv::normalize(roi_img2,roi_img2,-1,1,cv::NORM_MINMAX);
//     return roi_img2;
// }

// void Dialog::normalize2(cv::Mat& roi_img)
// {
//     normalize(roi_img, roi_img, 1, 254, NORM_MINMAX);
//     // vector<Mat> channels;
//     // cv::split(roi_img, channels);

//     // cv::equalizeHist(channels[0], channels[0]);
//     // cv::equalizeHist(channels[1], channels[1]);
//     // cv::equalizeHist(channels[2], channels[2]);
//     // cv::merge(channels, roi_img);
// }

void Dialog::Image_normalize(Mat input, Mat &output)
{
    Mat input2 = input.clone();
    input2.convertTo(output, CV_32FC3, 1.0 / 255);
}

void Dialog::adjust_cam()
{
    // capWebcam.set(cv::CAP_PROP_GAIN,15);
    // capWebcam.set(cv::CAP_PROP_AUTO_EXPOSURE,0.25);
    // capWebcam.set(cv::CAP_PROP_EXPOSURE,-5);
    // capWebcam.set(cv::CAP_PROP_EXPOSUREPROGRAM, 17009);
    // capWebcam.set(cv::CAP_PROP_IRIS,10240);
    // printf("BRIGHTNESS:%.2f\n",capWebcam.get(cv::CAP_PROP_BRIGHTNESS));
    // printf("CONTRAST:%.2f\n",capWebcam.get(cv::CAP_PROP_CONTRAST));
    // printf("SATURATION:%.2f\n",capWebcam.get(cv::CAP_PROP_SATURATION));
    // printf("HUE:%.2f\n",capWebcam.get(cv::CAP_PROP_HUE));
    // printf("GAIN:%.2f\n",capWebcam.get(cv::CAP_PROP_GAIN));
    // printf("AUTO_EXPOSURE:%.2f\n",capWebcam.get(cv::CAP_PROP_AUTO_EXPOSURE));
    // printf("EXPOSURE:%.2f\n",capWebcam.get(cv::CAP_PROP_EXPOSURE));
    // printf("IRIS:%.2f\n",capWebcam.get(cv::CAP_PROP_IRIS));
    // printf("EXPOSUREPROGRAM:%.2f\n",capWebcam.get(cv::CAP_PROP_EXPOSUREPROGRAM));
    return;
}

// 索引排序（升序排列）
template < typename T>
vector< size_t> sort_indexes(const vector< T>  & v) {

    // initialize original index locations
    vector< size_t>  idx(v.size());
    for (size_t i = 0; i != idx.size(); ++i) idx[i] = i;

    // sort indexes based on comparing values in v
	#ifdef NCNN
    sort(idx.begin(), idx.end(),
         [& v](size_t i1, size_t i2) {return v[i1] < v[i2];});
	#else
	sort(idx.begin(), idx.end(),
         [& v](size_t i1, size_t i2) {return v[i1] > v[i2];});
	#endif

    return idx;
}

//*******************掌纹roi函数，参考王浩师兄*******************
int OTSU(Mat image)
{
//    Mat image2 = image.clone();
//    cvtColor(image2, image2, COLOR_BGR2GRAY);
    const Mat gray = image.clone();
    if(gray.channels() != 1)
    {
        cout << "please input gray image" << endl;
        return 0;
    }
    int T = 0;
    double varValue = 0;
    double w0 = 0;
    double w1 = 0;
    double u0 = 0;
    double u1 = 0;
    double Histogram[256] = {0};
    uchar* data = gray.data;
    double totalNum = gray.rows * gray.cols;
    // 计算灰度直方图
    for(int i = 0; i < gray.rows; i++)
    {
        for(int j = 0; j < gray.cols; j++)
        {
            Histogram[data[i * image.step + j]]++;
        }
    }
    for(int i = 0; i < 255; i++) // 遍历阈值
    {
        w1 = 0; u1 = 0; w0 = 0; u0 = 0;

        for(int j = 0; j <= i; j++) // background
        {
            w1 += Histogram[j];
            u1 += j * Histogram[j];
        }
        if(w1 == 0) continue;
        u1 = u1 / w1;
        w1 = w1 / totalNum;

        for(int k = i+1; k < 255; k++)
        {
            w0 += Histogram[k];
            u0 += k * Histogram[k];
        }
        if(w0 == 0) break;
        u0 = u0 / w0;
        w0 = w0 / totalNum;

        double varValue_now = w0 * w1 * (u1 - u0) * (u1 - u0);
        if(varValue_now > varValue)
        {
            varValue = varValue_now;
            T = i;
        }
    }
    return static_cast<int>(T * 0.85f);
}

// cv::Mat Dialog::getRoiImg(cv::Mat PalmveinImg, bool flag_showImg) // 输入bgr图片 得到roi图片
// {
//     /*********************************************
//      * 掌纹ROI截取
//      * PalmveinImg : BGR图片
//      * flag_showImg : 保存fingerpoitns.jpg图片
//      * ROI_Palmvein : BGR图
//     *********************************************/
//     Mat PalmveinImg_gray = PalmveinImg.clone();
//     cvtColor(PalmveinImg_gray, PalmveinImg_gray, COLOR_BGR2GRAY);
//     // cout << "PalmveinImg_gray.channels() == " << PalmveinImg_gray.channels() << endl;
//     CV_Assert(PalmveinImg_gray.channels() == 1);
//     transpose(PalmveinImg_gray, PalmveinImg_gray);
//     flip(PalmveinImg_gray, PalmveinImg_gray, 0); // 逆时针转90度
//     // Point2f center = Point2f(static_cast<float>(PalmveinImg_gray.cols / 2), static_cast<float>(PalmveinImg_gray.rows / 2));
//     // int angle = 90;
//     // Mat affineTrans = getRotationMatrix2D(center, angle, 1.0);
//     // int len = max(PalmveinImg_gray.rows, PalmveinImg_gray.cols);
//     // warpAffine(PalmveinImg_gray, PalmveinImg_gray, affineTrans, Size(len, len)); // 逆时针转angle度
//     // cout << "rotate the row img for angle \n";
//     // cout << "size = " << PalmveinImg_gray.size() << endl;

//     Mat showImg = Mat(PalmveinImg_gray.rows, PalmveinImg_gray.cols, PalmveinImg_gray.type());
//     Mat BackImg = Mat(PalmveinImg_gray.rows, PalmveinImg_gray.cols, PalmveinImg_gray.type());
//     Rect rect(0, 0, PalmveinImg_gray.cols, PalmveinImg_gray.rows);
//     BackImg = PalmveinImg_gray(rect).clone(); // 灰度图
//     imwrite("/home/pi/Downloads/rotate.jpg", BackImg);
//     threshold(BackImg, BackImg, OTSU(BackImg), 255, THRESH_BINARY); // 二值化
//     for(int row = 0; row < BackImg.rows; row++)
//     {
//         BackImg.at<uchar>(row, BackImg.cols - 1) = 0;
//     }
//     for(int row = 0; row < BackImg.rows; row++)
//     {
//         BackImg.at<uchar>(row, 0) = 0;
//     }
//     for(int col = 0; col < BackImg.cols; col++)
//     {
//         BackImg.at<uchar>(BackImg.rows - 1, col) = 0;
//     }
//     // cout << "binary the row img \n";
//     imwrite("/home/pi/Downloads/OTSU.jpg", BackImg);
//     // return PalmveinImg;
//     Point* maxarea_point = new Point[5000];
//     int totalnum = 0;
//     getMaxRegion(BackImg, BackImg, maxarea_point, totalnum);
//     // cout << "getMaxRegion \n";

//     int upRowValue = 0; int downRowValue = 0;
//     for(int row = 1; row < BackImg.rows; row++)
//     {
//         if(BackImg.at<uchar>(row, static_cast<int>(BackImg.cols -2)) == 255)
//         {
//             upRowValue = row;
//             break;
//         }
//     }
//     for(int row = BackImg.rows - 1; row > 0; row--)
//     {
//         if(BackImg.at<uchar>(row, static_cast<int>(BackImg.cols -2)) == 255)
//         {
//             downRowValue = row;
//             break;
//         }
//     }
//     Point centroidpoint;
//     centroidpoint.y = upRowValue + (downRowValue - upRowValue) / 2;
//     centroidpoint.x = static_cast<int>(BackImg.cols -2);
//     // cout << "get centroidpoint \n";

//     Point* squarepoint = new Point[2];
//     Point* fingerpoint = new Point[9];
//     int step_result = get_square_twopoints(maxarea_point, totalnum, centroidpoint.x, centroidpoint, squarepoint, fingerpoint);
//     if(!step_result)
//     {
//         return PalmveinImg;
//     }
//     cout << "get_square_twopoints \n"; //

//     if(flag_showImg) //保存描点的图
//     {
//         showImg = BackImg.clone();
//         cvtColor(showImg, showImg, COLOR_GRAY2BGR);
//         circle(showImg, centroidpoint, 3, Scalar(255,0,0), 2);
//         for(int i = 0; i < 9; i++)
//         {
//             if(i % 2 == 0)
//             {
//                 circle(showImg, fingerpoint[i], 3, Scalar(255,0,0), 2);
//             }
//             else
//             {
//                 circle(showImg, fingerpoint[i], 3, Scalar(0,255,0), 2);
//             }
            
//         }
//         // for(int i = 0; i < 2; i++)
//         // {
//         //     circle(showImg, squarepoint[i], 3, Scalar(255,0,0), 2);
//         // }
//         circle(showImg, squarepoint[0], 3, Scalar(0,0,255), 2);
//         circle(showImg, squarepoint[1], 3, Scalar(255,255,0), 2);
//         imwrite("/home/pi/Downloads/fingerpoitns.jpg", showImg);
//     }
//     Mat ROI_Palmvein = get_palm_ROI(PalmveinImg_gray, squarepoint); // 灰度图
//     cout << "get final ROI img \n";

//     delete[]maxarea_point;
//     delete[]squarepoint;
//     delete[]fingerpoint;

//     cvtColor(ROI_Palmvein, ROI_Palmvein, COLOR_GRAY2BGR);

//     return ROI_Palmvein;
// }

// Mat Dialog::getRoiImg(cv::Mat PalmveinImg, bool flag_showImg, string imgName, int & e_rror) // ����bgrͼƬ �õ�roiͼƬ
// {
// 	/*********************************************
// 	 * ����ROI��ȡ
// 	 * PalmveinImg : BGRͼƬ
// 	 * flag_showImg : ����fingerpoitns.jpgͼƬ
// 	 * ROI_Palmvein : BGRͼ
// 	*********************************************/
// 	e_rror = 0;

// 	transpose(PalmveinImg, PalmveinImg);
// 	flip(PalmveinImg, PalmveinImg, 0); // ��ʱ��ת90��

// 	Mat PalmveinImg_gray = PalmveinImg.clone();
// 	cvtColor(PalmveinImg_gray, PalmveinImg_gray, COLOR_BGR2GRAY);
// 	// cout << "PalmveinImg_gray.channels() == " << PalmveinImg_gray.channels() << endl;
// 	CV_Assert(PalmveinImg_gray.channels() == 1);

// 	// Point2f center = Point2f(static_cast<float>(PalmveinImg_gray.cols / 2), static_cast<float>(PalmveinImg_gray.rows / 2));
// 	// int angle = 90;
// 	// Mat affineTrans = getRotationMatrix2D(center, angle, 1.0);
// 	// int len = max(PalmveinImg_gray.rows, PalmveinImg_gray.cols);
// 	// warpAffine(PalmveinImg_gray, PalmveinImg_gray, affineTrans, Size(len, len)); // ��ʱ��תangle��
// 	// cout << "rotate the row img for angle \n";
// 	// cout << "size = " << PalmveinImg_gray.size() << endl;

// 	Mat showImg = Mat(PalmveinImg_gray.rows, PalmveinImg_gray.cols, PalmveinImg_gray.type());
// 	Mat BackImg = Mat(PalmveinImg_gray.rows, PalmveinImg_gray.cols, PalmveinImg_gray.type());
// 	//Rect rect(0, 0, PalmveinImg_gray.cols, PalmveinImg_gray.rows);
// 	//BackImg = PalmveinImg_gray(rect).clone(); // �Ҷ�ͼ
// 	//imwrite("./rotate/rotate_" + imgName, BackImg);
// 	//threshold(BackImg, BackImg, OTSU(BackImg), 255, THRESH_BINARY); // ��ֵ��
// 	//threshold(BackImg, BackImg, 180, 255, CV_THRESH_OTSU | CV_THRESH_BINARY);
// 	/*imshow("mask", BackImg);
// 	waitKey();*/

// 	BackImg = finger_segmentation(PalmveinImg);

// 	Mat backImg = BackImg.clone();
// 	for (int row = 0; row < BackImg.rows; row++)
// 	{
// 		BackImg.at<uchar>(row, BackImg.cols - 1) = 0;
// 	}
// 	for (int row = 0; row < BackImg.rows; row++)
// 	{
// 		BackImg.at<uchar>(row, 0) = 0;
// 	}
// 	for (int col = 0; col < BackImg.cols; col++)
// 	{
// 		BackImg.at<uchar>(BackImg.rows - 1, col) = 0;
// 	}
// 	for (int col = 0; col < BackImg.cols; col++)
// 	{
// 		BackImg.at<uchar>(0, col) = 0;
// 	}
// 	// cout << "binary the row img \n";
// 	//imwrite("./OTSU/OTSU_" + imgName, BackImg);
// 	// return PalmveinImg;
// 	Point* maxarea_point = new Point[7000];
// 	int totalnum = 0;
// 	getMaxRegion(BackImg, BackImg, maxarea_point, totalnum);
// 	// cout << "getMaxRegion \n";
// 	/*imshow("MaxRegion", BackImg);
// 	waitKey();*/

// 	int upRowValue = 0; int downRowValue = 0;
// 	for (int row = 1; row < BackImg.rows; row++)
// 	{
// 		if (BackImg.at<uchar>(row, static_cast<int>(BackImg.cols - 2)) == 255)
// 		{
// 			upRowValue = row;
// 			break;
// 		}
// 	}
// 	for (int row = BackImg.rows - 1; row > 0; row--)
// 	{
// 		if (BackImg.at<uchar>(row, static_cast<int>(BackImg.cols - 2)) == 255)
// 		{
// 			downRowValue = row;
// 			break;
// 		}
// 	}
// 	Point centroidpoint;
// 	int edge_y = upRowValue + (downRowValue - upRowValue) / 2;
// 	//centroidpoint.y = static_cast<int>((BackImg.rows - 2) / 2);
// 	int point_min_y = BackImg.rows - 1;
// 	int point_max_y = 0;
// 	for (int i = 0; i < totalnum; i++)
// 	{
// 		if (maxarea_point[i].y < point_min_y)
// 		{
// 			point_min_y = maxarea_point[i].y;
// 		}
// 		if (maxarea_point[i].y > point_max_y)
// 		{
// 			point_max_y = maxarea_point[i].y;
// 		}
// 	}
// 	int palm_y = (point_min_y + point_max_y) / 2;
// 	if (edge_y == 0)
// 	{
// 		centroidpoint.y = palm_y;
// 	}
// 	else
// 	{
// 		centroidpoint.y = (palm_y + edge_y) / 2;
// 	}
// 	centroidpoint.x = static_cast<int>(BackImg.cols - 2);
// 	// cout << "get centroidpoint \n";

// 	Point* squarepoint = new Point[2];
// 	//Point* fingerpoint = new Point[100];
// 	vector<Point> fingerpoint;
// 	int step_result = get_square_twopoints(maxarea_point, totalnum, centroidpoint.x, centroidpoint, squarepoint, fingerpoint, backImg, e_rror);
// 	if (!step_result || e_rror == 1)
// 	{
// 		cout << "get error points!!!! \n";
// 		return PalmveinImg;
// 	}
// 	cout << "get_square_twopoints \n"; 

// 	if (flag_showImg) //��������ͼ
// 	{
// 		showImg = BackImg.clone();
// 		cvtColor(showImg, showImg, COLOR_GRAY2BGR);
// 		circle(showImg, centroidpoint, 3, Scalar(255, 0, 0), 2);
// 		for (int i = 0; i < fingerpoint.size(); i++)
// 		{
// 			if (i < 3)
// 			{
// 				circle(showImg, fingerpoint[i], 3, Scalar(0, 255, 0), 2);
// 			}
// 			else
// 			{
// 				circle(showImg, fingerpoint[i], 3, Scalar(0, 0, 255), 2);
// 			}	
// 		}
// 		/*for (int i = 0; i < 2; i++)
// 		{
// 			circle(showImg, squarepoint[i], 3, Scalar(255, 0, 0), 2);
// 		}*/
// 		//imwrite("./fingerpoitns/fingerpoitns_" + imgName, showImg);
// 	}
// 	Mat ROI_Palmvein = get_palm_ROI(PalmveinImg_gray, squarepoint, e_rror); // �Ҷ�ͼ
// 	if (e_rror == 1)
// 	{
// 		cout << "ROI out of range!!!! \n";
// 		return PalmveinImg;
// 	}
// 	cout << "get final ROI img \n";

// 	delete[]maxarea_point;
// 	delete[]squarepoint;
// 	//delete[]fingerpoint;

// 	cvtColor(ROI_Palmvein, ROI_Palmvein, COLOR_GRAY2BGR);
// 	//imwrite("./ROI_Palmvein/" + imgName, ROI_Palmvein);

// 	return ROI_Palmvein;
// }


cv::Mat Dialog::getRoiImg(cv::Mat PalmveinImg, bool flag_showImg, string PPorPV, int &e_rror, Mat &showImg2) // 输入bgr图片 得到roi图片
{
	/*********************************************
	 * 掌纹ROI截取
	 * PalmveinImg : BGR图片
	 * flag_showImg : 保存fingerpoitns.jpg图片
	 * ROI_Palmvein : BGR图
	*********************************************/
	e_rror = 0;

	transpose(PalmveinImg, PalmveinImg);
	flip(PalmveinImg, PalmveinImg, 0); // 逆时针转90度

	/*imshow("PalmveinImg", PalmveinImg);
	waitKey();*/

	Mat PalmveinImg_gray = PalmveinImg.clone();
	cvtColor(PalmveinImg_gray, PalmveinImg_gray, COLOR_BGR2GRAY);
	// cout << "PalmveinImg_gray.channels() == " << PalmveinImg_gray.channels() << endl;
	CV_Assert(PalmveinImg_gray.channels() == 1);

	// Point2f center = Point2f(static_cast<float>(PalmveinImg_gray.cols / 2), static_cast<float>(PalmveinImg_gray.rows / 2));
	// int angle = 90;
	// Mat affineTrans = getRotationMatrix2D(center, angle, 1.0);
	// int len = max(PalmveinImg_gray.rows, PalmveinImg_gray.cols);
	// warpAffine(PalmveinImg_gray, PalmveinImg_gray, affineTrans, Size(len, len)); // 逆时针转angle度
	// cout << "rotate the row img for angle \n";
	// cout << "size = " << PalmveinImg_gray.size() << endl;

	Mat showImg = Mat(PalmveinImg_gray.rows, PalmveinImg_gray.cols, PalmveinImg_gray.type());
	Mat BackImg = Mat(PalmveinImg_gray.rows, PalmveinImg_gray.cols, PalmveinImg_gray.type());
	//Rect rect(0, 0, PalmveinImg_gray.cols, PalmveinImg_gray.rows);
	//BackImg = PalmveinImg_gray(rect).clone(); // 灰度图
	//imwrite("./rotate/rotate_" + imgName, BackImg);
	//threshold(BackImg, BackImg, OTSU(BackImg), 255, THRESH_BINARY); // 二值化
	//threshold(BackImg, BackImg, 180, 255, CV_THRESH_OTSU | CV_THRESH_BINARY);
	/*imshow("mask", BackImg);
	waitKey();*/

	/*Mat prewitt_mask = prewitt_segmentation(PalmveinImg_gray);
	BackImg = finger_segmentation(PalmveinImg);*/
	//return 二值化图
	BackImg = prewitt_segmentation(PalmveinImg_gray, PPorPV);

	Mat backImg = BackImg.clone();
	for (int row = 0; row < BackImg.rows; row++)
	{
		BackImg.at<uchar>(row, BackImg.cols - 1) = 0;
	}
	for (int row = 0; row < BackImg.rows; row++)
	{
		BackImg.at<uchar>(row, 0) = 0;
	}
	for (int col = 0; col < BackImg.cols; col++)
	{
		BackImg.at<uchar>(BackImg.rows - 1, col) = 0;
	}
	for (int col = 0; col < BackImg.cols; col++)
	{
		BackImg.at<uchar>(0, col) = 0;
	}
	// cout << "binary the row img \n";
	// imwrite("./OTSU/OTSU_" + imgName, BackImg);
	// return PalmveinImg;
	Point* maxarea_point = new Point[7000];
	int totalnum = 0;
	//return 画出了轮廓的图
	getMaxRegion(BackImg, BackImg, maxarea_point, totalnum, e_rror);
	if (e_rror == 1)
	{
		cout << "get wrong thre!!!! \n";
		return PalmveinImg;
	}
	// cout << "getMaxRegion \n";
	/*imshow("MaxRegion", BackImg);
	waitKey();*/

	int upRowValue = 0; int downRowValue = 0;
	for (int row = 1; row < BackImg.rows; row++)
	{
		if (BackImg.at<uchar>(row, static_cast<int>(BackImg.cols - 2)) == 255)
		{
			upRowValue = row;
			break;
		}
	}
	for (int row = BackImg.rows - 1; row > 0; row--)
	{
		if (BackImg.at<uchar>(row, static_cast<int>(BackImg.cols - 2)) == 255)
		{
			downRowValue = row;
			break;
		}
	}
	Point centroidpoint;
	int edge_y = upRowValue + (downRowValue - upRowValue) / 2;
	//centroidpoint.y = static_cast<int>((BackImg.rows - 2) / 2);
	int point_min_y = BackImg.rows - 1;
	int point_max_y = 0;
	for (int i = 0; i < totalnum; i++)
	{
		if (maxarea_point[i].y < point_min_y)
		{
			point_min_y = maxarea_point[i].y;
		}
		if (maxarea_point[i].y > point_max_y)
		{
			point_max_y = maxarea_point[i].y;
		}
	}
	int palm_y = (point_min_y + point_max_y) / 2;
	if (edge_y == 0)
	{
		centroidpoint.y = palm_y;
	}
	else
	{
		centroidpoint.y = (palm_y + edge_y) / 2;
	}
	centroidpoint.x = static_cast<int>(BackImg.cols - 2);
	// cout << "get centroidpoint \n";

	Point* squarepoint = new Point[2];
	//Point* fingerpoint = new Point[100];
	vector<Point> fingerpoint;
	int step_result = get_square_twopoints(maxarea_point, totalnum, centroidpoint.x, centroidpoint, squarepoint, fingerpoint, backImg, e_rror);
	/*if (!step_result || e_rror == 1)
	{
		cout << "get wrong points!!!! \n";
		return PalmveinImg;
	}
	cout << "get_square_twopoints \n"; */

	if (flag_showImg) //保存描点的图
	{
		showImg = BackImg.clone();
		cvtColor(showImg, showImg, COLOR_GRAY2BGR);
		circle(showImg, centroidpoint, 3, Scalar(255, 0, 0), 2);
		for (int i = 0; i < fingerpoint.size(); i++)
		{
			if (i < 3)
			{
				circle(showImg, fingerpoint[i], 3, Scalar(0, 255, 0), 2);
			}
			else
			{
				circle(showImg, fingerpoint[i], 3, Scalar(0, 0, 255), 2);
			}	
		}
		showImg2 = showImg.clone();
		/*for (int i = 0; i < 2; i++)
		{
			circle(showImg, squarepoint[i], 3, Scalar(255, 0, 0), 2);
		}*/
		// imwrite("./fingerpoitns/fingerpoitns_" + imgName, showImg);
	}
	if (!step_result || e_rror == 1)
	{
		cout << "get wrong points!!!! \n";
		return PalmveinImg;
	}
	cout << "get_square_twopoints \n";

	Mat ROI_Palmvein = get_palm_ROI(PalmveinImg_gray, squarepoint, e_rror); // 灰度图
	if (e_rror == 1)
	{
		cout << "get wrong ROI!!!! \n";
		return PalmveinImg;
	}
	cout << "get final ROI img \n";

	delete[]maxarea_point;
	delete[]squarepoint;
	//delete[]fingerpoint;

	cvtColor(ROI_Palmvein, ROI_Palmvein, COLOR_GRAY2BGR);
	// imwrite("./ROI_Palmvein/" + imgName, ROI_Palmvein);

	return ROI_Palmvein;
}

Mat Dialog::prewitt_segmentation(Mat img, string PPorPV)
{
	//cvtColor(img, img, COLOR_BGR2GRAY);

	/*Mat kernel_up;
	Mat kernel_down;*/
	Mat kernel_bias;
	/*kernel_up = (cv::Mat_<char>(5, 5) <<
		-1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1,
		0, 0, 0, 0, 0,
		1, 1, 1, 1, 1,
		1, 1, 1, 1, 1);
	kernel_down = (cv::Mat_<char>(5, 5) <<
		-1, -1, 0, 1, 1,
		-1, -1, 0, 1, 1,
		-1, -1, 0, 1, 1,
		-1, -1, 0, 1, 1,
		-1, -1, 0, 1, 1);*/
	kernel_bias = (cv::Mat_<char>(5, 5) <<
		0, 0, 0, 0, 1,
		0, 0, 0, 1, 0,
		0, 0, 1, 0, 0,
		0, 1, 0, 0, 0,
		1, 0, 0, 0, 0);
	/*kernel_down = (cv::Mat_<char>(3, 3) <<
		1, 1, 1,
		1, 1, 1,
		1, 1, 1);*/
	/*cout << "kernel_up = " << endl << kernel_up << endl;
	cout << "kernel_down = " << endl << kernel_down << endl;*/

	/*Mat tmp_x;
	Mat tmp_y;*/
	Mat tmp_bias;
	/*filter2D(img, tmp_x, CV_16S, kernel_up);
	filter2D(img, tmp_y, CV_16S, kernel_down);*/
	filter2D(img, tmp_bias, CV_16S, kernel_bias);
	/*imshow("tmp_x", tmp_x);
	waitKey();
	imshow("tmp_y", tmp_y);
	waitKey();*/
	imwrite("../run_file/tmp_bias"+PPorPV+".bmp", tmp_bias);
	/*Mat absX;
	Mat absY;*/
	Mat absBIAS;
	/*convertScaleAbs(tmp_x, absX);
	convertScaleAbs(tmp_y, absY);*/
	convertScaleAbs(tmp_bias, absBIAS);
	/*imshow("absX", absX);
	waitKey();
	imshow("absY", absY);
	waitKey();*/
	
	/*threshold(absX, absX, 127, 255, THRESH_BINARY);
	threshold(absY, absY, 127, 255, THRESH_BINARY);*/
	threshold(absBIAS, absBIAS, 127, 255, THRESH_BINARY);
	/*imshow("absX", absX);
	waitKey();
	imshow("absY", absY);
	waitKey();*/
	imwrite("../run_file/absBIAS"+PPorPV+".bmp", absBIAS);
	//imshow("absBIAS.bmp", absBIAS);
	/*morphologyEx(absX, absX, MORPH_OPEN, Mat(3, 3, CV_8U), Point(-1, -1), 5);
	morphologyEx(absY, absY, MORPH_OPEN, Mat(3, 3, CV_8U), Point(-1, -1), 5);*/
	Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3));
	erode(absBIAS, absBIAS, kernel);
	dilate(absBIAS, absBIAS, kernel);
	//morphologyEx(absBIAS, absBIAS, MORPH_OPEN, Mat(3, 3, CV_8U), Point(-1, -1), 5);
	/*imshow("absX", absX);
	waitKey();
	imshow("absY", absY);
	waitKey();*/
	imwrite("../run_file/MORPH_OPEN"+PPorPV+".bmp", absBIAS);
	//imshow("MORPH_OPEN.bmp", absBIAS);
	/*Mat addxy = (absX + absY) / 2;
	imshow("prewitt_mask", addxy);
	waitKey();*/
	// imwrite("../run_file/absBIAS.bmp", absBIAS);
	//waitKey(0);

	Mat auto_mask;
	threshold(img, auto_mask, OTSU(img), 255, THRESH_BINARY);
	//morphologyEx(thre_mask, thre_mask, MORPH_OPEN, Mat(3, 3, CV_8U), Point(-1, -1), 3);
	/*imshow("thre_mask", auto_mask);
	waitKey();*/

	Mat mask;
	Scalar auto_sum = sum(auto_mask);
	Scalar bias_sum = sum(absBIAS);
	double diff = bias_sum[0] / auto_sum[0];
	cout << "diff = " << diff << " --- ";
	if (diff > 1.5)
	{
		auto_mask.copyTo(mask);
		cout << "auto_mask" << endl;
	}
	else if (diff < 0.85)
	{
		mask = absBIAS + auto_mask;
		cout << "bias_mask + auto_mask" << endl;
	}
	else
	{
		absBIAS.copyTo(mask);
		cout << "bias_mask" << endl;
	}

	return mask;
}


Mat Dialog::finger_segmentation(Mat img)
{
    Mat mask = Mat::zeros(img.size(), CV_8U);
	Mat gabor_img = gabor(img);
	gabor_img.convertTo(gabor_img, CV_8U);
	equalizeHist(gabor_img, gabor_img); //ֱ��ͼ���⻯
	morphologyEx(gabor_img, gabor_img, MORPH_CLOSE, Mat(3, 3, CV_8U), Point(-1, -1), 4);
	debug_imshow("gabor", gabor_img);
	/*imshow("gabor", gabor_img);
	waitKey();*/

	threshold(gabor_img, gabor_img, 150, 255, CV_THRESH_OTSU | CV_THRESH_BINARY); //CV_THRESH_OTSU | CV_THRESH_BINARY_INV
	morphologyEx(gabor_img, gabor_img, MORPH_CLOSE, Mat(3, 3, CV_8U), Point(-1, -1), 3);
	debug_imshow("thre", gabor_img);
	/*imshow("thre", gabor_img);
	waitKey();*/
	//vector<Mat> area; //���������������ͨ��
	//bwareaopen(gabor_img, area, 100000);  //��ָ���40000����
	//dilate(gabor_img, gabor_img, Mat());
	//morphologyEx(gabor_img, gabor_img, MORPH_CLOSE, Mat(3, 3, CV_8U), Point(-1, -1), 3);
	//debug_imshow("remove", gabor_img);
	/*imshow("remove", gabor_img);
	waitKey();*/

	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	findContours(gabor_img, contours, hierarchy, CV_RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point());
	fillPoly(gabor_img, contours, Scalar(255), 8, 0);
	debug_imshow("fill", gabor_img);
	/*imshow("fill", gabor_img);
	waitKey();*/

	vector<Mat> area; //���������������ͨ��
	bwareaopen(gabor_img, area, 80000);  //��ָ���40000����

	Mat auto_mask;
	//threshold(img, auto_mask, 180, 255, CV_THRESH_OTSU | CV_THRESH_BINARY);  //����㷨ʧЧ��û���ᵽmask����ֱ��������Ӧ��ֵ��
	Mat img_gray;
	cvtColor(img, img_gray, CV_BGR2GRAY);
	threshold(img_gray, auto_mask, OTSU(img_gray), 255, CV_THRESH_BINARY); //CV_THRESH_OTSU | 
	bwareaopen2(auto_mask);//ȥ�������飬һ��Ϊ��ֵ����ı�����������ָ�����ԼΪ100*400��Ч���Ϻ�
	/*transpose(auto_mask, auto_mask);
	flip(auto_mask, auto_mask, 1);
	imshow("auto_mask", auto_mask);
	waitKey();
	transpose(auto_mask, auto_mask);
	flip(auto_mask, auto_mask, 0);*/
	//Mat lap_mask = auto_mask.mul(gabor_img);

	cout << area.size() << endl;
	int probable_i = -1;
	float max_diff = 0;
	for (int i = 0; i < area.size(); i++)
	{
		Scalar auto_sum = sum(auto_mask);
		Scalar gabor_auto_sum = sum(auto_mask + area[i]);
		float diff = auto_sum[0] / gabor_auto_sum[0];
		if (diff > 0.7 && diff > max_diff)
		{
			max_diff = diff;
			probable_i = i;
		}
	}
	//Scalar auto_sum = sum(auto_mask);
	//Scalar gabor_sum = sum(mask);
	//float diff = gabor_sum[0] / auto_sum[0];
	//float diff = 0;
	/*cout << diff << endl;
	debug_imshow("auto", auto_mask);*/

	if (probable_i == -1)
	{
		cout << "auto mask" << endl;
		//threshold(img, mask, 180, 255, CV_THRESH_OTSU | CV_THRESH_BINARY);  //����㷨ʧЧ��û���ᵽmask����ֱ��������Ӧ��ֵ��
		//bwareaopen(mask, 30000);//ȥ�������飬һ��Ϊ��ֵ����ı�����������ָ�����ԼΪ100*400��Ч���Ϻ�
		auto_mask.copyTo(mask);
		imageblur(mask, mask, Size(9, 9), 100);//��Ե�ữ
	}
	else
	{
		cout << "auto mask + gabor mask" << endl;
		gabor_img = area[probable_i] + auto_mask;
		/*transpose(area[probable_i], area[probable_i]);
		flip(area[probable_i], area[probable_i], 1);
		imshow("gabor_mask", area[probable_i]);
		waitKey();*/
		//findContours(gabor_img, contours, hierarchy, CV_RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point());
		////fillPoly(gabor_img, contours, Scalar(255), 8, 0);
		//for (int i = 0; i < contours.size(); i++)
		//{
		//	convexHull(Mat(contours[i]), hull[i], false);
		//}
		//for (int i = 0; i < contours.size(); i++)
		//{
		//	Scalar color = Scalar(255);
		//	//drawContours(drawing, contours, i, color, 1, 8, vector<Vec4i>(), 0, Point());
		//	drawContours(mask, hull, i, color, 1, 8, vector<Vec4i>(), 0, Point());
		//}
		//fillPoly(mask, hull, Scalar(255), 8, 0);
		gabor_img.copyTo(mask);
	}

	/*dilate(mask, mask, Mat(), Point(-1, -1), 3);
	erode(mask, mask, Mat(), Point(-1, -1), 3);*/
	//morphologyEx(mask, mask, MORPH_CLOSE, Mat(3, 3, CV_8U), Point(-1, -1), 3);
	debug_imshow("mask", mask);

	//Mat imageContours = Mat::zeros(gabor_img.size(), CV_8UC1);
	//drawContours(imageContours, contours, 0, Scalar(255), 1, 8, hierarchy);
	//fillPoly(imageContours, contours, Scalar(255), 8, 0);


	//����Σ�light���ݼ���һ�����Ե�ǽ����
	//for (int i = 0; i < 10; i++)
	//{
	//	for (int j = 0; j < mask.cols; j++)
	//	{
	//		gabor_img.at<uchar>(i, j) = 0;
	//	}
	//}

	//Mat gabor_invers;
	//bitwise_not(gabor_img, gabor_invers);
	//bwareaopen(gabor_invers, 120000);  //��ָ���90000-100000
	//debug_imshow("gabor_invers", gabor_invers);

	//mask = gabor_img + gabor_invers;
	//bitwise_not(mask, mask);
	//bwareaopen(mask, 70000);

	//������
	//vector<vector<Point>> contours;
	//vector<Vec4i> hierarchy;
	//findContours(mask, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point());
	//if (contours.size() == 0)
	//{
	//	threshold(img, mask, 180, 255, CV_THRESH_OTSU | CV_THRESH_BINARY);  //����㷨ʧЧ��û���ᵽmask����ֱ��������Ӧ��ֵ��
	//	bwareaopen(mask, 30000);//ȥ�������飬һ��Ϊ��ֵ����ı�����������ָ�����ԼΪ100*400��Ч���Ϻ�
	//	imageblur(mask, mask, Size(9, 9), 100);//��Ե�ữ
	//}

	//Mat final_mask = Mat(mask.rows, mask.cols + shrink, CV_8U, Scalar(0));
	Mat final_mask = Mat(mask.rows, mask.cols, CV_8U, Scalar(0));
	for (int i = 0; i < mask.rows; i++)
	{
		for (int j = 0; j < mask.cols; j++)
		{
			if (mask.at<uchar>(i, j) != 0)
				//final_mask.at<uchar>(i, j + shrink) = 255;
				final_mask.at<uchar>(i, j) = 255;
		}

	}
	//morphologyEx(final_mask, final_mask, MORPH_CLOSE, Mat(3, 3, CV_8U), Point(-1, -1), 1);
	debug_imshow("mask", final_mask);
	/*transpose(final_mask, final_mask);
	flip(final_mask, final_mask, 1);*/
	/*imshow("final_mask", final_mask);
	waitKey();*/

	//for (int i = 0; i < mask.rows; i++)
	//{
	//	for (int j = 0; j < mask.cols; j++)
	//	{
	//		if (mask.at<uchar>(i, j) == 0)
	//			img.at<uchar>(i, j) = 0;
	//	}
	//}

	//debug_imshow("final", img);
	return final_mask;
}

void Dialog::debug_imshow(const char * name, Mat img)
{
#ifdef DEBUG
	imshow(name, img);
	waitKey(0);
#endif
}


void Dialog::imageblur(Mat& src, Mat& dst, Size size, int threshold)
{
	int height = src.rows;
	int width = src.cols;
	blur(src, dst, size);
	for (int i = 0; i < height; i++)
	{
		uchar* p = dst.ptr<uchar>(i);
		for (int j = 0; j < width; j++)
		{
			if (p[j] < threshold)
				p[j] = 0;
			else p[j] = 255;
		}
	}
}

/************************************************************************
FunName: bwareaopen
Funtion: ȥ�����С��n�Ĺ�����
Parspecification:
--input
src --�����ֵͼ��
n--�������pix
*************************************************************************/
void Dialog::bwareaopen(Mat &src, vector<Mat> &area, int n)
{
	//����ԭͼ���ڵ���
	Mat temp = Mat::zeros(src.size(), CV_8U);
	src.copyTo(temp);

	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	//	cout << "findContours"<< endl;
		//�������� CV_RETR_EXTERNAL���Ե
	findContours(temp, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	//	cout << "contours.size() is :" << contours.size() << endl;
	
	for (int i = 0; i < contours.size(); i++)
	{
		double dArea = contourArea(contours[i]);
		if (dArea > n)
		{
			Mat temp_area = Mat::zeros(src.size(), CV_8U);
			cv::drawContours(temp_area, contours, i, Scalar(255), CV_FILLED);
			area.push_back(temp_area);
		}
	}
	//cout << area.size() << endl;
	/*for (int i = 0; i < area.size(); i++)
	{
		imshow("area", area[i]);
		waitKey();
	}*/
}


void Dialog::bwareaopen2(Mat &src)
{
	//����ԭͼ���ڵ���
	Mat temp = Mat::zeros(src.size(), CV_8U);
	src.copyTo(temp);

	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	//	cout << "findContours"<< endl;
		//�������� CV_RETR_EXTERNAL���Ե
	findContours(temp, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	//	cout << "contours.size() is :" << contours.size() << endl;
	double max_area = 0;
	int max_i = 0;
	for (int i = 0; i < contours.size(); i++)
	{
		double dArea = contourArea(contours[i]);
		//if (dArea <= n)
		//{
		//	cv::drawContours(src, contours, i, Scalar(0), CV_FILLED);  //������С��n����ͨ��
		//}
		if (dArea > max_area)
		{
			max_area = dArea;
			max_i = i;
		}
	}

	for (int i = 0; i < contours.size(); i++)
	{
		if (i != max_i)
		{
			cv::drawContours(src, contours, i, Scalar(0), CV_FILLED);
		}
	}
}


void Dialog::getMaxRegion(Mat srcImg, Mat& dstImg, Point* maxarea_point, int& totalnum, int& e_rror)
{
    /************************************
     * 找出最大面积的轮廓点
     * srcImg : 二值化图
     * dstImg : 轮廓高亮的图
    ************************************/
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    findContours(srcImg, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0,0));
    Mat tempImg = Mat(srcImg.rows, srcImg.cols, srcImg.type(), Scalar::all(0));
    // cout << "find Contours \n";
    int area = 0; int max_area = 100;
    for(int i = 0; i < contours.size(); i++)
    {
        area = static_cast<int>(contourArea(contours[i]));
        if(area > max_area)
        {
            max_area = area;
        }
    }
    // cout << "max_area = " << max_area << endl;
    for(int i = 0; i < contours.size(); i++)
    {
        area = static_cast<int>(contourArea(contours[i]));
        if(area == max_area) // 最大连通域
        {
            totalnum = static_cast<int>(contours[i].size()); //最大联通域的轮廓点数量
            cout << "totalnum = " << totalnum << endl;
			if (totalnum >= 7000)
			{
				e_rror = 1;
				return;
			}
            for(int j = 0; j < totalnum; j++)
            {
                maxarea_point[j] = contours[i][j]; //轮廓复制给maxarea_point
            }
            drawContours(tempImg, contours, i, Scalar(255), 1);
            // Mat pKernel = getStructuringElement(MORPH_ELLIPSE, Size(7,7));
            // dilate(tempImg, tempImg, pKernel);
            // erode(tempImg, tempImg, pKernel);
            
            // imwrite("/home/pi/Downloads/contours.jpg", tempImg);
            break;
        }
        
    }
    dstImg = tempImg.clone();

}


// int Dialog::get_square_twopoints(Point* areapoint, int& totalnum, int& beginpoint, Point& centroidpoint, Point* square_point, Point* fingerpoint)
// {
//     /************************************
//      * 寻找两个角点
//      * areapoint : 轮廓点
//      * totalnum : 轮廓点数量
//      * beginpoint : 开始点的x坐标
//      * centroidpoint : 质心
//      * square_point : 角点
//      * fingerpoint : 手指关键点
//     ************************************/
//     int num = 0;
//     int startpoint = 0, startpoint_y = 0;
//     double* Dis = new double[totalnum];
//     for(int i = totalnum - 1; i >= 0; i--)
//     {
//         if(areapoint[i].x == beginpoint)
//         {
//             if(areapoint[i].y > startpoint_y)
//             {
//                 startpoint_y = areapoint[i].y; //找与质心同样x情况下的最大的y
//                 startpoint = i; // 轮廓点的idx
//             }
//         }
//     }
//     for(int i = startpoint; i < startpoint + totalnum; i++)
//     {
//         Dis[(i - startpoint) % totalnum] = Dis_of_twoPoint(areapoint[i % totalnum], centroidpoint); // 计算边缘点到Pref的距离 Dis从0开始
//     }
//     int num_seq[9];
//     for(int i = 0; i < totalnum; i++)
//     {
//         if(num == 9)
//             break;
//         if(num % 2 == 0) //指尖
//         {
//             if(find_extremum(Dis, totalnum, i, 40, true))
//             {
//                 fingerpoint[num] = areapoint[(i + startpoint) % totalnum];
//                 num_seq[num] = (i + startpoint) % totalnum;
//                 num++;
//                 continue;
//             }
//         }
//         else if(num % 2 != 0) //指缝
//         {
//             if(find_extremum(Dis, totalnum, i, 40, false))
//             {
//                 fingerpoint[num] = areapoint[(i + startpoint) % totalnum];
//                 num_seq[num] = (i + startpoint) % totalnum;
//                 num++;
//             }
//         }

//     }
//     delete[]Dis;
//     if(num < 9)
//     {
//         cout << "can not find nine fingerpoints!!!!" << endl;
//         return 0;
//     }
    
//     Point point_else; 
//     int fingerlength;
//     if(Dis_of_twoPoint(fingerpoint[1], fingerpoint[3]) > Dis_of_twoPoint(fingerpoint[5], fingerpoint[7])) // 左手
//     {
//         fingerlength = (num_seq[3] - num_seq[2] + totalnum) % totalnum; // 食指轮廓点的数量
// 		point_else = areapoint[(num_seq[2] - fingerlength + totalnum) % totalnum]; // 食指另一边的谷点
// 		square_point[1].x = (fingerpoint[3].x + point_else.x) / 2;
// 		square_point[1].y = (fingerpoint[3].y + point_else.y) / 2; // 食指中心点
// 		fingerlength = (num_seq[8] - num_seq[7] + totalnum) % totalnum;
// 		point_else = areapoint[(num_seq[8] + fingerlength + totalnum) % totalnum];
// 		square_point[0].x = (fingerpoint[7].x + point_else.x) / 2;
// 		square_point[0].y = (fingerpoint[7].y + point_else.y) / 2; // 小指中心点
//     }
//     else 		//右手
// 	{
// 		fingerlength = (num_seq[1] - num_seq[0] + totalnum) % totalnum;
// 		point_else = areapoint[(num_seq[0] - fingerlength + totalnum) % totalnum];
// 		square_point[1].x = (fingerpoint[1].x + point_else.x) / 2;
// 		square_point[1].y = (fingerpoint[1].y + point_else.y) / 2;
// 		fingerlength = (num_seq[6] - num_seq[5] + totalnum) % totalnum;
// 		point_else = areapoint[(num_seq[6] + fingerlength + totalnum) % totalnum];
// 		square_point[0].x = (fingerpoint[5].x + point_else.x) / 2;
// 		square_point[0].y = (fingerpoint[5].y + point_else.y) / 2; // 图中偏下面的点
// 	}
//     return 1;
// }

int Dialog::get_square_twopoints(Point* areapoint, int& totalnum, int& beginpoint, Point& centroidpoint, Point* square_point, vector<Point>& fingerpoint, Mat& backImg, int& e_rror)
{
	/************************************
	 * Ѱ�������ǵ�
	 * areapoint : ������
	 * totalnum : ����������
	 * beginpoint : ��ʼ���x����
	 * centroidpoint : ����
	 * square_point : �ǵ�
	 * fingerpoint : ��ָ�ؼ���
	************************************/
	int num = 0;
	int startpoint = 0, startpoint_y = 0, max_x = 0;
	double* Dis = new double[totalnum];
	for (int i = totalnum - 1; i >= 0; i--)
	{
		/*if (areapoint[i].x == beginpoint)
		{
			if (areapoint[i].y > startpoint_y)
			{
				startpoint_y = areapoint[i].y; //��������ͬ��x����µ�����y
				startpoint = i; // �������idx
			}
		}*/
		if (areapoint[i].x > max_x)
		{
			max_x = areapoint[i].x;
			startpoint = i;
			startpoint_y = areapoint[i].y;
		}
		else if (areapoint[i].x == max_x)
		{
			if (areapoint[i].y < startpoint_y)
			{
				startpoint = i;
				startpoint_y = areapoint[i].y;
			}
		}
	}
	for (int i = startpoint; i < startpoint + totalnum; i++)
	{
		Dis[(i - startpoint) % totalnum] = Dis_of_twoPoint(areapoint[i % totalnum], centroidpoint); // �����Ե�㵽Pref�ľ��� Dis��0��ʼ
	}
	int num_seq[100];
	double disValley[100];
	/*for (int i = 0; i < 9; i++)
	{
		cout << disValley[i] << endl;
	}*/
	for (int i = 0; i < totalnum; i++)
	{
		Point curPoint = areapoint[(i + startpoint) % totalnum];
		if (curPoint.x < backImg.cols - 15 && find_extremum(Dis, totalnum, i, 40, false, disValley, num, backImg, areapoint, startpoint)) //65 30for18
		//if (find_extremum(Dis, totalnum, i, 150, false, disValley, num, backImg, areapoint, startpoint))
		{
			fingerpoint.push_back(areapoint[(i + startpoint) % totalnum]);
			num_seq[num] = (i + startpoint) % totalnum;
			num++;
			i += 20; //35
		}
		if (num > 100)
		{
			cout << "find too many fingerpoints!!!!" << endl;
			e_rror = 1;
			return 0;
		}

	}
	cout << "num = " << num << endl;
	while (num > 4)
	{
		double minDis = disValley[0];
		int minX = backImg.cols - fingerpoint[0].x;
		int minNum = 0;
		double minL = 0.1 * minX + 0.9 * minDis;
		for (int i = 1; i < num; i++)
		{
			double tmpL = 0.1 * (backImg.cols - fingerpoint[i].x) + 0.9 * disValley[i];
			if (tmpL < minL)
			{
				minL = tmpL;
				minNum = i;
			}
		}

		for (int i = minNum; i < num - 1; i++)
		{
			Point tmp = fingerpoint[i];
			fingerpoint[i] = fingerpoint[i + 1];
			fingerpoint[i + 1] = tmp;
			disValley[i] = disValley[i + 1];
		}
		num--;
	}

	//cout << "point" << fingerpoint[0] << endl << fingerpoint[1] << endl << fingerpoint[2] << endl;
	//cout << disValley[0] << ", " << disValley[1] << ", " << disValley[2] << endl;

	delete[]Dis;
	double dis0_1 = 0;
	double dis1_2 = 0;
	double dis2_3 = 0;
	if (num <= 3)
	{
		cout << "can not find three fingerpoints!!!!" << endl;
		e_rror = 1;
		return 0;
	}
	//else if (num == 3)
	//{
	//	dis0_1 = Dis_of_twoPoint(fingerpoint[0], fingerpoint[1]);
	//	dis1_2 = Dis_of_twoPoint(fingerpoint[1], fingerpoint[2]);
	//}
	else
	{
		dis0_1 = Dis_of_twoPoint(fingerpoint[0], fingerpoint[1]);
		dis1_2 = Dis_of_twoPoint(fingerpoint[1], fingerpoint[2]);
		dis2_3 = Dis_of_twoPoint(fingerpoint[2], fingerpoint[3]);
		//cout << dis0_1 << endl << dis1_2 << endl << dis2_3 << endl;
		if (dis0_1 > dis1_2 && dis0_1 > dis2_3)
		{
			for (int i = 0; i < num - 1; i++)
			{
				Point tmp = fingerpoint[i];
				fingerpoint[i] = fingerpoint[i + 1];
				fingerpoint[i + 1] = tmp;
			}
			num--;
		}
		else if (dis2_3 > dis0_1 && dis2_3 > dis1_2)
		{
			num--;
		}
	}

	if (num == 4)
	{
		e_rror = 1;
		return 0;
	}

	square_point[0] = fingerpoint[2];
	square_point[1] = fingerpoint[0];

	return 1;
}

// Mat Dialog::get_palm_ROI(Mat srcImg, const Point* square_points)  //也会报错 无返回？
// {
//     /************************************
//      * 寻找两个角点
//      * srcImg : 灰度图片
//      * square_points : 两个角点
//      * ROIImg : 灰度图片
//     ************************************/
//     bool out_range = false;
//     Mat rotateImg = Mat(srcImg.rows, srcImg.cols, srcImg.type());
//     if(square_points[0].x == square_points[1].x) // 图片没有旋转
//     {
//         rotateImg = srcImg.clone();
//     }
//     else
//     {
//         Point2f rotatecenter; //旋转中心
//         rotatecenter.x = static_cast<float>(square_points[0].x);
// 		rotatecenter.y = static_cast<float>(square_points[0].y);
//         int rotateangle;
//         rotateangle = static_cast<int>(floor(atan(static_cast<float>(square_points[0].x - square_points[1].x) / (square_points[1].y - square_points[0].y)) * 180 / 3.1416 + 0.5));
// 		rotateangle = (rotateangle + 360) % 360;
// 		rotateImage(srcImg, rotateImg, rotatecenter, rotateangle); // 旋转灰度图
//     }
//     int dis = static_cast<int>(Dis_of_twoPoint(square_points[0], square_points[1]));
//     int dis1 = 0; int dis2 = 0;
//     if(square_points[0].x + dis >= rotateImg.cols)
//     {
//         dis1 = rotateImg.cols - 1 - square_points[0].x;
//         out_range = true;
//     }
//     if(square_points[0].y - dis < 0)
//     {
//         dis2 = square_points[0].y;
//         out_range = true;
//     }
//     if(out_range)
//     {
//         dis1 < dis2 ? dis = dis1 : dis = dis2; // dis取小的
//         // dis = dis1;
//     }
//     cout << "dis = " << dis << endl;
//     Rect square(square_points[0].x, square_points[0].y - dis, dis, dis);
//     if(square.area() > 0)
//     {
//         Mat ROIImg = Mat(dis, dis, rotateImg.type());
//         ROIImg = rotateImg(square).clone();
//         return ROIImg;
//     }
//     else
//         return rotateImg;
    
// }

Mat Dialog::get_palm_ROI(Mat srcImg, const Point* square_points, int& e_rror)  //Ҳ�ᱨ�� �޷��أ�
{
	/************************************
	 * Ѱ�������ǵ�
	 * srcImg : �Ҷ�ͼƬ
	 * square_points : �����ǵ�
	 * ROIImg : �Ҷ�ͼƬ
	************************************/
	bool out_range = false;
	Mat rotateImg = Mat(srcImg.rows, srcImg.cols, srcImg.type());
	if (square_points[0].x == square_points[1].x) // 图片没有旋转
	{
		rotateImg = srcImg.clone();
	}
	else
	{
		Point2f rotatecenter; //旋转中心
		rotatecenter.x = static_cast<float>(square_points[0].x);
		rotatecenter.y = static_cast<float>(square_points[0].y);
		if (square_points[1].y - square_points[0].y >= 0)
		{
			cout << "ratate error!!!! \n";
			e_rror = 1;
			return srcImg;
		}
		int rotateangle;
		rotateangle = static_cast<int>(floor(atan(static_cast<float>(square_points[0].x - square_points[1].x) / (square_points[1].y - square_points[0].y)) * 180 / 3.1416 + 0.5));
		rotateangle = (rotateangle + 360) % 360;
		rotateImage(srcImg, rotateImg, rotatecenter, rotateangle); // 旋转灰度图
	}
	/*imshow("img", rotateImg);
	waitKey();*/
	int dis = static_cast<int>(Dis_of_twoPoint(square_points[0], square_points[1]));
	//cout << "dis = " << dis << endl;
	int addL = (int)(dis / 1.5);
	int addX = dis / 5;
	int divX = dis / 4;
	
	int downL = rotateImg.rows - 1 - square_points[0].y;
	int upL = square_points[0].y - dis;
	int add_downL = addL;
	int add_upL = addL;
	if (add_downL > downL)
	{
		add_downL = downL;                              //2020.12.3
		/*if (downL * 2 >= addL)
		{
			addL = downL;
		}
		else
		{
			cout << "ROI out of range!!!! \n";
			e_rror = 1;
			return srcImg;
		}*/
	}
	if (add_upL > upL)
	{
		add_upL = upL;                              //2020.12.3
		/*if (upL * 2 >= addL)
		{
			addL = upL;
		}
		else
		{
			cout << "ROI out of range!!!! \n";
			e_rror = 1;
			return srcImg;
		}*/
	}
	dis = dis + add_downL + add_upL;
	int dis_y = dis;
	int dis_x;
	//if (square_points[0].x + addX + (dis * ratio_wh) >= rotateImg.cols)
	if (square_points[0].x - divX < 0)
	{
		divX = square_points[0].x;
	}
	if (square_points[0].x - divX + (dis * 1) >= rotateImg.cols)
	{
		//if (square_points[0].x + addX + (dis * 0.85) <= rotateImg.cols)
		if (square_points[0].x - divX + (dis * 0.6) <= rotateImg.cols)
		{
			//dis_x = rotateImg.cols - 1 - square_points[0].x - addX;
			dis_x = rotateImg.cols - 1 - square_points[0].x + divX;
		}
		else
		{
			cout << "ROI out of range!!!! \n";
			e_rror = 1;
			return srcImg;
		}
		
	}
	else
	{
		dis_x = dis * 1;
	}
	// if(square_points[0].y + dis >= rotateImg.rows)
	// {
	//     dis2 = rotateImg.rows - 1 - square_points[0].y;
	//     out_range = true;
	// }
	//if (out_range)
	//{
	//	// dis1 < dis2 ? dis = dis1 : dis = dis2; // dis取小的
	//	dis = dis1;
	//}
	cout << "dis_x = " << dis_x << endl;
	cout << "dis_y = " << dis_y << endl;
	if (dis_x <= 0 || dis_y <= 0)
	{
		e_rror = 1;
		return rotateImg;
	}
	//Rect square(square_points[0].x + addX, square_points[0].y + addL - dis_y, dis_x, dis_y);
	Rect square(square_points[0].x - divX, square_points[0].y + add_downL - dis_y, dis_x, dis_y);
	/*rectangle(rotateImg, square, Scalar(0, 0, 255), 2);
	imshow("rect", rotateImg);
	waitKey();*/
	if (square.area() > 0)
	{
		Mat ROIImg = Mat(dis, dis, rotateImg.type());
		ROIImg = rotateImg(square).clone();
		/*imshow("ROIImg", ROIImg);
		waitKey(); */
		return ROIImg;
	}
	else
	{
		e_rror = 1;
		return rotateImg;
	}

}


double Dialog::Dis_of_twoPoint(Point point1, Point point2)
{
	int x1 = point1.x;
	int y1 = point1.y;
	int x2 = point2.x;
	int y2 = point2.y;
	return sqrt(static_cast<double>((x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2)));
}

// int Dialog::find_extremum(double* Dis, int totalnum, int position, int length_limit, bool isMax)
// {
// 	// if (position-length_limit < 0 || position+length_limit > totalnum-1)
// 	// {
// 	// return -1;
// 	// }
// 	for (int i = position - length_limit; i<position + length_limit; i++)
// 	{
// 		if (isMax)
// 		{
// 			if (Dis[(i + totalnum) % totalnum] > Dis[position])
// 			{
// 				return 0; // 不是极值返回0
// 			}
// 		}
// 		if (!isMax)
// 		{
// 			if (Dis[(i + totalnum) % totalnum] < Dis[position])
// 			{
// 				return 0;
// 			}
// 		}
// 	}
// 	return 1; // 是极值返回1
// }

int Dialog::find_extremum(double* Dis, int totalnum, int position, int length_limit, bool isMax, double* disValley, int num, Mat& backImg, Point* areapoint, int startpoint)
{
	 for (int i = position; i <= position + length_limit; i++)
	{
		if (isMax)
		{
			if (Dis[(i + totalnum) % totalnum] > Dis[position])
			{
				return 0; // 不是极值返回0
			}
		}
		if (!isMax)
		{
			if (Dis[(i + totalnum) % totalnum] + 1.0 < Dis[position])
			{
				return 0;
			}
			else
			{
				//if ((areapoint[(i + startpoint) % totalnum].x == 1 || areapoint[(i + startpoint) % totalnum].y == backImg.rows - 1) && i > position + 10) //15
				if ((areapoint[(i + startpoint) % totalnum].x == 1 && areapoint[(i + startpoint) % totalnum].x < areapoint[(position + startpoint) % totalnum].x - 15) || (areapoint[(i + startpoint) % totalnum].y == backImg.rows - 1 && areapoint[(i + startpoint) % totalnum].y > areapoint[(position + startpoint) % totalnum].y + 8))
				{
					break;
				}
			}
		}

	}

	for (int i = position; i >= position - length_limit; i--)
	{
		if (isMax)
		{
			if (Dis[(i + totalnum) % totalnum] > Dis[position])
			{
				return 0; // 不是极值返回0
			}
		}
		if (!isMax)
		{
			if (Dis[(i + totalnum) % totalnum] + 1.0 < Dis[position])
			{
				return 0;
			}
			else
			{
				//if ((areapoint[(i + startpoint) % totalnum].x == 1 || areapoint[(i + startpoint) % totalnum].y == 1) && i < position - 10)
				if ((areapoint[(i + startpoint) % totalnum].x == 1 && areapoint[(i + startpoint) % totalnum].x < areapoint[(position + startpoint) % totalnum].x - 15) || (areapoint[(i + startpoint) % totalnum].y == 1 && areapoint[(i + startpoint) % totalnum].y < areapoint[(position + startpoint) % totalnum].y - 8))
				{
					break;
				}
			}
		}

	}

	/***********************
	�ж�4����8����16����
	***********************/
	//int totalPoint = 4;
	//int bgPoint = 0;
	//if (curPoint.x - 1 > 0)
	//{
	//	//totalPoint++;
	//	if (backImg.at<uchar>(curPoint.y, curPoint.x - 1) == 255)
	//	{
	//		bgPoint++;
	//	}
	//}
	//if (curPoint.x + 1 < backImg.cols - 1)
	//{
	//	//totalPoint++;
	//	if (backImg.at<uchar>(curPoint.y, curPoint.x + 1) == 255)
	//	{
	//		bgPoint++;
	//	}
	//}
	//if (curPoint.y - 1 > 0)
	//{
	//	//totalPoint++;
	//	if (backImg.at<uchar>(curPoint.y - 1, curPoint.x) == 255)
	//	{
	//		bgPoint++;
	//	}
	//}
	//if (curPoint.y + 1 < backImg.rows - 1)
	//{
	//	//totalPoint++;
	//	if (backImg.at<uchar>(curPoint.y + 1, curPoint.x) == 255)
	//	{
	//		bgPoint++;
	//	}
	//}
	//if (double(totalPoint - bgPoint) / totalPoint > 0.4)
	//{
	//	return 0;
	//}

	//totalPoint = 8;
	//bgPoint = 0;
	//if (curPoint.y - 2 > 0)
	//{
	//	//totalPoint++;
	//	if (backImg.at<uchar>(curPoint.y - 2, curPoint.x) == 255)
	//	{
	//		bgPoint++;
	//	}
	//	if (curPoint.x - 2 > 0)
	//	{
	//		//totalPoint++;
	//		if (backImg.at<uchar>(curPoint.y - 2, curPoint.x - 2) == 255)
	//		{
	//			bgPoint++;
	//		}
	//	}
	//	if (curPoint.x + 2 < backImg.cols - 1)
	//	{
	//		//totalPoint++;
	//		if (backImg.at<uchar>(curPoint.y - 2, curPoint.x + 2) == 255)
	//		{
	//			bgPoint++;
	//		}
	//	}
	//}
	//if (curPoint.y + 2 < backImg.rows - 1)
	//{
	//	//totalPoint++;
	//	if (backImg.at<uchar>(curPoint.y + 2, curPoint.x) == 255)
	//	{
	//		bgPoint++;
	//	}
	//	if (curPoint.x - 2 > 0)
	//	{
	//		//totalPoint++;
	//		if (backImg.at<uchar>(curPoint.y - 2, curPoint.x - 2) == 255)
	//		{
	//			bgPoint++;
	//		}
	//	}
	//	if (curPoint.x + 2 < backImg.cols - 1)
	//	{
	//		//totalPoint++;
	//		if (backImg.at<uchar>(curPoint.y - 2, curPoint.x + 2) == 255)
	//		{
	//			bgPoint++;
	//		}
	//	}
	//}
	//if (curPoint.x - 2 > 0)
	//{
	//	//totalPoint++;
	//	if (backImg.at<uchar>(curPoint.y, curPoint.x - 2) == 255)
	//	{
	//		bgPoint++;
	//	}
	//}
	//if (curPoint.x + 2 < backImg.cols - 1)
	//{
	//	//totalPoint++;
	//	if (backImg.at<uchar>(curPoint.y, curPoint.x + 2) == 255)
	//	{
	//		bgPoint++;
	//	}
	//}
	//if ((totalPoint - bgPoint) / totalPoint > 0.4)
	//{
	//	return 0;
	//}


	//cout<< Dis[position]<<endl;
	disValley[num] = Dis[position];
	return 1; // �Ǽ�ֵ����1
}


void Dialog::rotateImage(Mat srcImg, Mat& rotateImg, Point2f rotatecenter, int rotateangle)
{
    /******************************
     * srcImg : 灰度图
     * rotateImg : 灰度图
     * rotatecenter : 旋转中心
     * rotateangle : 旋转角度
    ******************************/
	// float m[6];
	// CvMat M = cvMat(2, 3, CV_32F, m);
    Mat M = getRotationMatrix2D(rotatecenter, rotateangle, 1.0);
	// cv2DRotationMatrix(rotatecenter, rotateangle, 1, &M);
	// cvWarpAffine(srcImg, rotateImg, &M, CV_INTER_LINEAR + CV_WARP_FILL_OUTLIERS, cvScalarAll(0));
    warpAffine(srcImg, rotateImg, M, srcImg.size(), INTER_CUBIC, BORDER_REPLICATE);
}
