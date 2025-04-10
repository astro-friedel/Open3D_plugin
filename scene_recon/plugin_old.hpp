// // #ifndef SCENE_RECON_HPP
// // #define SCENE_RECON_HPP

// // #include "illixr/plugin.hpp"
// // #include "illixr/phonebook.hpp"
// // #include "illixr/switchboard.hpp"
// // #include "illixr/data_format/scene_recon_type.hpp"

// // #include <string>

// // namespace ILLIXR {

// // /// A plugin that subscribes to "scene_recon_data" and processes scene reconstruction events.
// // class scene_recon : public plugin {
// // public:
// //     scene_recon(std::string name, phonebook* pb);
// //     virtual ~scene_recon() override = default;
// //     int callback(const switchboard::ptr<const data_format::scene_recon_type>& datum, int count);

// // private:
// //     const std::shared_ptr<switchboard> sb;
// // 	switchboard::reader<scene_recon_type> _m_scene_recon_data;
// //     std::string intrinsic_path = "/home/anhiti/Open3DScripts/cpp_code/intrinsics.json";
// //     std::string color_intrinsic_path = "/home/anhiti/Open3DScripts/cpp_code/color_intrinsics.json";
// //     std::string seq_name = "my_sequence";
// //     std::string mesh_dir = "/home/anhiti/Open3DScripts/cpp_code/mesh_output";
// //     float depth_scale = 1000.0f;
// //     float voxel_size = 0.01f;
// //     float depth_max = 4.0f;
// //     float trunc_multiplier = 10.0f;
// //     int divisor = 1;
// //     std::string device_code = "CUDA:0";
// //     bool save_mesh = true;
// //         // ITMUChar4Image *inputRGBImage;
// //         // ITMShortImage *inputRawDepthImage;
// //         // ITMLib::ITMMainEngine *mainEngine;
// //         // ITMLib::ITMRGBDCalib *calib;
// //     unsigned frame_count=0;
// //         // std::ofstream adjusted_file;
// //         // std::vector<std::vector<float>> gt_array;
// //         std::string output_mesh_name;
// //     };
// // } // namespace ILLIXR

// // #endif // SCENE_RECON_HPP


// #ifndef SCENE_RECON_HPP
// #define SCENE_RECON_HPP

// #include "illixr/plugin.hpp"
// #include "illixr/phonebook.hpp"
// #include "illixr/switchboard.hpp"
// #include "illixr/data_format/scene_recon_type.hpp"
// #include "illixr/data_format/pose.hpp"

// #include <open3d/Open3D.h>
// #include <open3d/core/Tensor.h>
// #include <open3d/t/geometry/VoxelBlockGrid.h>
// #include <open3d/t/geometry/Image.h>
// #include <open3d/t/io/ImageIO.h>
// // #include <open3d/t/pipelines/integration/UniformTSDFVolume.h>
// // #include <open3d/core/eigen_converter.h>

// #include <string>
// #include <memory>
// #include <mutex>

// namespace ILLIXR {

// class scene_recon : public plugin {
// public:
//     scene_recon(std::string name, phonebook* pb);
//     virtual ~scene_recon() override;

// private:
//     void callback(const switchboard::ptr<const data_format::scene_recon_type>& datum, int count);
//     open3d::camera::PinholeCameraIntrinsic LoadCalibration(const std::string filename);
//     void cleanMesh(std::shared_ptr<open3d::geometry::TriangleMesh> &mesh);
//     open3d::t::geometry::Image ConvertCvMatToImage(const cv::Mat& mat);
//     Eigen::Matrix4d ConvertPoseToEigen(const ILLIXR::data_format::pose_type& pose);
//     /// Converts a pose_type (position + quaternion) to a 4x4 extrinsic matrix.
//     // Eigen::Matrix4d pose_to_extrinsic(const pose_type pose);

//     const std::shared_ptr<switchboard> switchboard_;
//     switchboard::reader<data_format::scene_recon_type> _m_scene_recon_data;

//     // Voxel integration
//     std::unique_ptr<open3d::t::geometry::VoxelBlockGrid> voxel_grid;
//     open3d::core::Tensor intrinsic_t;
//     open3d::core::Tensor color_intrinsic_t;

//     // Reconstruction configuration
//     std::string intrinsic_path       = "/home/anhiti/Open3DScripts/cpp_code/intrinsics.json";
//     std::string color_intrinsic_path = "/home/anhiti/Open3DScripts/cpp_code/color_intrinsics.json";
//     std::string mesh_dir             = "/home/anhiti/Open3DScripts/cpp_code/mesh_output";
//     std::string seq_name             = "my_sequence";
//     std::string device_code          = "CUDA:0";
//     // Define parameters (adjust as necessary) new
//     // double voxel_length = 0.004;       // size of each voxel
//     // double sdf_trunc = 0.04;           // truncation value for TSDF
//     // auto tsdf_volume = std::make_unique<open3d::t::pipelines::integration::UniformTSDFVolume>(
//     //     voxel_length, sdf_trunc, device);
//     float depth_scale       = 1000.0f;
//     float voxel_size        = 0.01f;
//     float depth_max         = 4.0f;
//     float trunc_multiplier  = 10.0f;
//     int divisor             = 1;
//     bool save_mesh          = true;

//     open3d::core::Device device;
//     unsigned frame_count = 0;
//     std::mutex recon_mutex;
// };

// } // namespace ILLIXR

// #endif // SCENE_RECON_HPP
