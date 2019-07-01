#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include "Graphics.h"
#include "Intersection.h"

void graphics::simulate()
{
    this->load_background_img();
    while (true)
    {
        /* Sleep at every iteration to reduce CPU usage */
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        /* Update graphics */
        this->draw_traffic_objects();
    }
}

void graphics::load_background_img()
{
    /* Create window */
    window_name_ = "Concurrency Traffic Simulation";
    cv::namedWindow(window_name_);

    /* Load image and create copy to be used for semi-transparent overlay */
    cv::Mat background = cv::imread(bg_filename_);
    images_.push_back(background);              //First element is the original background
    images_.push_back(background.clone()); //Second element will be the transparent overlay
    images_.push_back(background.clone()); //Third element will be the result image for display
}

void graphics::draw_traffic_objects()
{
    /* Reset images */
    images_.at(1) = images_.at(0).clone();
    images_.at(2) = images_.at(0).clone();

    /* Create overlay from all traffic objects */
    for (const auto& it : traffic_objects_)
    {
        double posx, posy;
        it->get_position(posx, posy);

        if (it->get_type() == object_intersection)
        {
            /* Cast object type from TrafficObject to Intersection */
            auto intersect = std::dynamic_pointer_cast<intersection>(it);

            /* Set color according to traffic light and draw the intersection as a circle */
            cv::Scalar traffic_light_color = intersect->traffic_light_is_green() ? cv::Scalar(0, 255, 0) : cv::Scalar(0, 0, 255);
            circle(images_.at(1), cv::Point2d(posx, posy), 25, traffic_light_color, -1);
        }
        else if (it->get_type() == object_vehicle)
        {
            cv::RNG rng(it->get_id());
            int b = rng.uniform(0, 255);
            int g = rng.uniform(0, 255);
            int r = static_cast<int>(sqrt(255*255 - g*g)); //Ensure that length of color vector is always 255
            cv::Scalar vehicle_color = cv::Scalar(b,g,r);
            circle(images_.at(1), cv::Point2d(posx, posy), 50, vehicle_color, -1);
        }
    }

    float opacity = 0.85;
    addWeighted(images_.at(1), opacity, images_.at(0), 1.0 - opacity, 0, images_.at(2));

    // display background and overlay image
	resize(images_.at(2), images_.at(2), cv::Size(images_.at(2).cols / 4, images_.at(2).rows / 4));
    imshow(window_name_, images_.at(2));
    cv::waitKey(33);
}
