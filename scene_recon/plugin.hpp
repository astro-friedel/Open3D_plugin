#ifndef SCENE_RECON_HPP
#define SCENE_RECON_HPP

#include "illixr/plugin.hpp"
#include "illixr/phonebook.hpp"
#include "illixr/switchboard.hpp"
#include "illixr/data_format/scene_recon_type.hpp"
#include "illixr/data_format/pose.hpp"

#include <open3d/Open3D.h>
#include <open3d/core/Tensor.h>
#include <open3d/t/geometry/VoxelBlockGrid.h>
#include <open3d/t/geometry/Image.h>
#include <open3d/t/io/ImageIO.h>
#include <string>
#include <memory>
#include <mutex>

namespace ILLIXR {

class scene_recon : public plugin {
public:
    scene_recon(std::string name, phonebook* pb);
    // virtual ~scene_recon() override = default;

    void callback(const switchboard::ptr<const data_format::scene_recon_type>& datum, int count);

private:
    const std::shared_ptr<switchboard> switchboard_;
    switchboard::reader<data_format::scene_recon_type> scene_data_;

    // Paths
    std::string intrinsic_path       = "/home/anhiti/Open3DScripts/cpp_code/intrinsics.json";
    std::string color_intrinsic_path = "/home/anhiti/Open3DScripts/cpp_code/color_intrinsics.json";
    std::string mesh_dir             = "/home/anhiti/Open3DScripts/cpp_code/mesh_output";
    std::string seq_name             = "my_sequence";
    std::string device_code          = "CUDA:0";

    // // TSDF config
    float depth_scale       = 1000.0f;
    float voxel_size        = 0.01f;
    float depth_max         = 4.0f;
    float trunc_multiplier  = 10.0f;
    int divisor             = 1;
    bool save_mesh          = true;

    // // Open3D runtime objects
    open3d::core::Device device;
    // std::unique_ptr<open3d::t::geometry::VoxelBlockGrid> voxel_grid;
    // open3d::core::Tensor intrinsic_t;
    // open3d::core::Tensor color_intrinsic_t;

    unsigned frame_count = 0;
    std::mutex recon_mutex;

    // // Helpers (optional, used in callback)
    // open3d::t::geometry::Image ConvertCvMatToImage(const cv::Mat& mat);
    // Eigen::Matrix4d ConvertPoseToEigen(const ILLIXR::data_format::pose_type& pose);
    // void cleanMesh(std::shared_ptr<open3d::geometry::TriangleMesh>& mesh);
};

} // namespace ILLIXR

#endif // SCENE_RECON_HPP
