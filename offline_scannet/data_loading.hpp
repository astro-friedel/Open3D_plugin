#include "illixr/error_util.hpp"
#include "illixr/data_format/pose.hpp"
#include "csv_iterator.hpp"
#include <cassert>
#include <eigen3/Eigen/Dense>
#include <fstream>
#include <map> // Includes all OpenCV core modules
#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <optional>
#include <string>

typedef unsigned long long ullong;

//pyh for convinence I will use the combined imu_cam for scannet
using namespace ILLIXR;
using namespace ILLIXR::data_format;

class lazy_load_image {
public:
    lazy_load_image(const std::string& path)
        : _m_path(path) { }

    lazy_load_image(){}
    std::unique_ptr<cv::Mat> load() const {
        //printf("depth image path: %s\n", _m_path.c_str());
        auto img = std::unique_ptr<cv::Mat>{new cv::Mat{cv::imread(_m_path, cv::IMREAD_UNCHANGED)}};
        assert(!img->empty());
        return img;
    }
   std::unique_ptr<cv::Mat> color_load() const {
        //printf("color image path: %s\n", _m_path.c_str());
        cv::Mat original_mat = cv::imread(_m_path, cv::IMREAD_UNCHANGED);
        cv::Mat *converted_mat = new cv::Mat(original_mat.size(), CV_8UC4, cv::Scalar(0, 0, 0, 255));
        cv::cvtColor(original_mat,*converted_mat,cv::COLOR_BGR2RGBA,0);
        auto img = std::unique_ptr<cv::Mat>{converted_mat};
        
        assert(!img->empty());
        return img;
    }

private:
    std::string _m_path;
};

typedef struct {
    //pyh since we are using groundtruth pose change the datatype
    pose_type    pose;
    lazy_load_image depth_cam;
    lazy_load_image color_cam;
    bool last_frame;
    unsigned id;

} sensor_types;

static std::map<ullong, sensor_types> load_data() {
    // const char* illixr_data_c_str = std::getenv("ILLIXR_DATA");
    // if (!illixr_data_c_str) {
    //     std::cerr << "Please define ILLIXR_DATA" << std::endl;
    //     ILLIXR::abort();
    // }
    // std::string illixr_data = std::string{illixr_data_c_str};
    //pyh gldemo test
    std::string illixr_data = "/home/anhiti/Open3DScripts/original/0005";
    std::cout<<"data loading"<<"\n";
    std::map<ullong, sensor_types> data;
    const std::string groundtruth_subpath = "/poses/groundtruth.txt";
    //const std::string groundtruth_subpath = "/poses/groundtruth_rgb.txt";
    std::ifstream groundtruth_file{illixr_data + groundtruth_subpath};
    printf("groundtruth pose path: %s\n", (illixr_data+groundtruth_subpath).c_str());    
    if (!groundtruth_file.good()) {
        std::cerr << "${ILLIXR_DATA}" << groundtruth_subpath << " (" << illixr_data << groundtruth_subpath << ") is not a good path"
                  << std::endl;
        ILLIXR::abort();
    }
    ullong t;
    unsigned count = 0;
    for (CSVIterator row{groundtruth_file, 0}; row != CSVIterator{}; ++row) {
        //pyh I don't want to use ns scale so I will use ms scale for now
        t = std::stoull(row[0]) * 33.33;
        Eigen::Vector3f gt_position{std::stof(row[1]), std::stof(row[2]), std::stof(row[3])};
        Eigen::Quaternionf gt_orientation{std::stof(row[7]), std::stof(row[4]), std::stof(row[5]), std::stof(row[6])};
        //printf("x: %s, y: %s, z: %s, qx: %s, qy: %s, qz: %s, qw: %s, depth: %s, color: %s\n", row[1].c_str(), row[2].c_str(), row[3].c_str(), row[4].c_str(), row[5].c_str(), row[6].c_str(), row[7].c_str(), row[9].c_str(), row[11].c_str());        
        data[t].pose = {time_point{},gt_position,gt_orientation};
        data[t].depth_cam = {illixr_data + "/" + row[9]};
        // std::cout<< illixr_data + "/" + row[11]<<"\n";
        data[t].color_cam = {illixr_data + "/" + row[11]};
        data[t].id = count++;
    }
    //check if it the last line
    data[t].last_frame=true;
    //std::cout<<"last frame time: "<<t<<std::endl;
   
    return data;
}
