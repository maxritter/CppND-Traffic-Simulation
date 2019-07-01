#pragma once

#include <string>
#include <utility>
#include <vector>
#include <opencv2/core.hpp>

#include "TrafficObject.h"

class graphics
{
public:
    void set_bg_filename(std::string filename) { bg_filename_ = std::move(filename); }
    void set_traffic_objects(std::vector<std::shared_ptr<traffic_object>> &traffic_objects) { traffic_objects_ = traffic_objects; }

    void simulate();

private:
    void load_background_img();
    void draw_traffic_objects();

    std::vector<std::shared_ptr<traffic_object>> traffic_objects_;
    std::string bg_filename_;
    std::string window_name_;
    std::vector<cv::Mat> images_;
};