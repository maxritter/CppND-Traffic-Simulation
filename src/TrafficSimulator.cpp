#include <iostream>
#include <thread>
#include <vector>

#include "Vehicle.h"
#include "Street.h"
#include "Intersection.h"
#include "Graphics.h"
#include "TrafficObject.h"

/* Create traffic objects for 1st scenario, which is Paris */
void create_traffic_objects_paris(std::vector<std::shared_ptr<street>> &streets, std::vector<std::shared_ptr<intersection>> &intersections, 
	std::vector<std::shared_ptr<vehicle>> &vehicles, std::string &filename, const int n_vehicles)
{
    /* Assign filename of corresponding city map*/
    filename = "./data/paris.jpg";

    /* Init traffic objects */
    int n_intersections = 9;
    for (auto ni = 0; ni < n_intersections; ni++)
    {
        intersections.push_back(std::make_shared<intersection>());
    }

    /* Position intersections in pixel coordinates (counter-clockwise) */
    intersections.at(0)->set_position(385, 270);
    intersections.at(1)->set_position(1240, 80);
    intersections.at(2)->set_position(1625, 75);
    intersections.at(3)->set_position(2110, 75);
    intersections.at(4)->set_position(2840, 175);
    intersections.at(5)->set_position(3070, 680);
    intersections.at(6)->set_position(2800, 1400);
    intersections.at(7)->set_position(400, 1100);
    intersections.at(8)->set_position(1700, 900); //Central plaza

    /* Create streetsand connect traffic objects */
    int n_streets = 8;
    for (auto ns = 0; ns < n_streets; ns++)
    {
        streets.push_back(std::make_shared<street>());
        streets.at(ns)->set_in_intersection(intersections.at(ns));
        streets.at(ns)->set_out_intersection(intersections.at(8));
    }

    /* Add vehicles to streets, starting from central plaza outwards */
    for (auto nv = 0; nv < n_vehicles; nv++)
    {
        vehicles.push_back(std::make_shared<vehicle>());
        vehicles.at(nv)->set_current_street(streets.at(nv));
        vehicles.at(nv)->set_current_destination(intersections.at(8));
    }
}

/* Create traffic objects for 1st scenario, which is Paris */
void create_traffic_objects_nyc(std::vector<std::shared_ptr<street>> &streets, std::vector<std::shared_ptr<intersection>> &intersections, 
	std::vector<std::shared_ptr<vehicle>> &vehicles, std::string &filename, int nVehicles)
{
    /* Assign filename of corresponding city map */
    filename = "./data/nyc.jpg";

    /* Init traffic objects */
    int n_intersections = 6;
    for (auto ni = 0; ni < n_intersections; ni++)
    {
        intersections.push_back(std::make_shared<intersection>());
    }

    /* Position intersections in pixel coordinates */
    intersections.at(0)->set_position(1430, 625);
    intersections.at(1)->set_position(2575, 1260);
    intersections.at(2)->set_position(2200, 1950);
    intersections.at(3)->set_position(1000, 1350);
    intersections.at(4)->set_position(400, 1000);
    intersections.at(5)->set_position(750, 250);

    /* Create streets and connect traffic objects */
    int n_streets = 7;
    for (auto ns = 0; ns < n_streets; ns++)
    {
        streets.push_back(std::make_shared<street>());
    }
    streets.at(0)->set_in_intersection(intersections.at(0));
    streets.at(0)->set_out_intersection(intersections.at(1));
    streets.at(1)->set_in_intersection(intersections.at(1));
    streets.at(1)->set_out_intersection(intersections.at(2));
    streets.at(2)->set_in_intersection(intersections.at(2));
    streets.at(2)->set_out_intersection(intersections.at(3));
    streets.at(3)->set_in_intersection(intersections.at(3));
    streets.at(3)->set_out_intersection(intersections.at(4));
    streets.at(4)->set_in_intersection(intersections.at(4));
    streets.at(4)->set_out_intersection(intersections.at(5));
    streets.at(5)->set_in_intersection(intersections.at(5));
    streets.at(5)->set_out_intersection(intersections.at(0));
    streets.at(6)->set_in_intersection(intersections.at(0));
    streets.at(6)->set_out_intersection(intersections.at(3));

    /* Add vehicles to streets */
    for (auto nv = 0; nv < nVehicles; nv++)
    {
        vehicles.push_back(std::make_shared<vehicle>());
        vehicles.at(nv)->set_current_street(streets.at(nv));
        vehicles.at(nv)->set_current_destination(intersections.at(nv));
    }
}

/* Main function */
int main()
{
    /* Set up traffic objects */
	int n_vehicles = 6;
    std::vector<std::shared_ptr<street>> streets;
    std::vector<std::shared_ptr<intersection>> intersections;
    std::vector<std::shared_ptr<vehicle>> vehicles;
    std::string background_img;
    //create_traffic_objects_paris(streets, intersections, vehicles, background_img, n_vehicles);
	create_traffic_objects_nyc(streets, intersections, vehicles, background_img, n_vehicles);

    /* Simulate traffic objects */
    std::for_each(intersections.begin(), intersections.end(), [](std::shared_ptr<intersection> &i) {
        i->simulate();
    });
    std::for_each(vehicles.begin(), vehicles.end(), [](std::shared_ptr<vehicle> &v) {
        v->simulate();
    });

    /* Launch visualization */
    std::vector<std::shared_ptr<traffic_object>> traffic_objects;
    std::for_each(intersections.begin(), intersections.end(), [&traffic_objects](std::shared_ptr<intersection> &intersection) {
	    auto traffic_obj = std::dynamic_pointer_cast<traffic_object>(intersection);
        traffic_objects.push_back(traffic_obj);
    });
    std::for_each(vehicles.begin(), vehicles.end(), [&traffic_objects](std::shared_ptr<vehicle> &vehicles) {
	    auto traffic_obj = std::dynamic_pointer_cast<traffic_object>(vehicles);
        traffic_objects.push_back(traffic_obj);
    });

    auto graphic = new graphics();
    graphic->set_bg_filename(background_img);
    graphic->set_traffic_objects(traffic_objects);
    graphic->simulate();
}
