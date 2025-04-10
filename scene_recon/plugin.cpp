#include "plugin.hpp"
#include "illixr/phonebook.hpp"
#include "illixr/switchboard.hpp"
#include "open3d/Open3D.h"
#include <thread>
#include <iostream>
#include "plugin.hpp"
#include <eigen3/Eigen/Dense>
#include <memory>
#include "illixr/plugin.hpp"
#include "illixr/data_format/scene_recon_type.hpp"
#include "open3d/Open3D.h"
#include <deque>
#include <fstream>
#include <sys/stat.h>
using namespace open3d;
using namespace open3d::core;
using namespace ILLIXR;
using namespace ILLIXR::data_format;

scene_recon::scene_recon(std::string name, phonebook* pb)
    : plugin{name, pb}
    , switchboard_{phonebook_->lookup_impl<switchboard>()}
    , scene_data_{switchboard_->get_reader<scene_recon_type>("scene_recon_data")}
{
    const char* illixr_data_c_str = std::getenv("ILLIXR_DATA");
    std::cout << "here!!!!!!!!!!"<< std::endl;

    // device = core::Device(core::Device::DeviceType::CUDA, 0);

    //INTRINSICS
    camera::PinholeCameraIntrinsic intrinsic = camera::PinholeCameraIntrinsic(
            camera::PinholeCameraIntrinsicParameters::PrimeSenseDefault);
    if (intrinsic_path.empty()) {
        std::cout <<"empty"<<"\n";
    } else if (!io::ReadIJsonConvertible(intrinsic_path, intrinsic)) {
        std::cout <<"cant convert"<<"\n";
    }

    // auto focal_length = intrinsic.GetFocalLength();
    // auto principal_point = intrinsic.GetPrincipalPoint();
    // Tensor intrinsic_t = Tensor::Init<double>(
    //         {{focal_length.first, 0, principal_point.first},
    //         {0, focal_length.second, principal_point.second},
    //         {0, 0, 1}});

    int count = 0;
    pb->lookup_impl<switchboard>()->schedule<data_format::scene_recon_type>(
        id_,
        "scene_recon_data",
        [&](const switchboard::ptr<const data_format::scene_recon_type> datum, size_t) {
            callback(datum, count);
        }
    );
}

void scene_recon::callback(const switchboard::ptr<const data_format::scene_recon_type>& datum, int count){
    std::cout<<"Hello"<<"\n";
}


PLUGIN_MAIN(scene_recon);