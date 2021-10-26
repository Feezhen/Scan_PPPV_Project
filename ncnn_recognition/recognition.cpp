#include "recognition.h"



ncnn::Mat ncnn_net::extract_feature(cv::Mat bgr)
{

    ncnn::Mat in = ncnn::Mat::from_pixels(bgr.data, ncnn::Mat::PIXEL_BGR, bgr.cols, bgr.rows);
    ncnn::Mat feat;

    const float mean_value[3] = {127.5f, 127.5f, 127.5f};
    const float norm_value[3] = {1 / 127.5f, 1 / 127.5f, 1 / 127.5f};
    in.substract_mean_normalize(mean_value, norm_value);

    ncnn::Extractor ex = net.create_extractor();
    ex.set_light_mode(true);
    ex.input(MobileNet_v2_20201217new_param_id::LAYER_input, in);
    ex.extract(MobileNet_v2_20201217new_param_id::BLOB_feat, feat);
    //ex.extract("Addmm_2", out);
    return feat;
}

void ncnn_net::net_init()
{
    net.load_param(MobileNet_v2_20201217new_param_bin);
    net.load_model(MobileNet_v2_20201217new_bin);
}

ncnn_net::ncnn_net()
{
    net_init();
}
