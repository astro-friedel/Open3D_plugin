// #include "plugin.hpp"
// #include "illixr/phonebook.hpp"
// #include "illixr/switchboard.hpp"
// #include "open3d/Open3D.h"
// #include <thread>
// #include <iostream>
// #include "plugin.hpp"
// #include <eigen3/Eigen/Dense>
// #include <memory>
// #include "illixr/plugin.hpp"
// #include "illixr/data_format/scene_recon_type.hpp"
// #include <open3d/core/Tensor.h>
// #include <opencv2/opencv.hpp>
// #include <cstring>
// #include "open3d/Open3D.h"
// #include <deque>
// #include <fstream>
// #include <sys/stat.h>
// using namespace open3d;
// using namespace open3d::core;

// using namespace ILLIXR;
// using namespace ILLIXR::data_format;

// scene_recon::scene_recon(std::string name, phonebook* pb)
//     : plugin{name, pb}
//     , switchboard_{phonebook_->lookup_impl<switchboard>()}
//     , _m_scene_recon_data{switchboard_->get_reader<scene_recon_type>("scene_recon_data")}
// {
//     const char* illixr_data_c_str = std::getenv("ILLIXR_DATA");
//     std::cout << "here!!!!!!!!!!"<< std::endl;

//     //INTRINSICS
//     camera::PinholeCameraIntrinsic intrinsic = camera::PinholeCameraIntrinsic(
//             camera::PinholeCameraIntrinsicParameters::PrimeSenseDefault);
//     if (intrinsic_path.empty()) {
//         utility::LogWarning("Using default Primesense depth intrinsics");
//     } else if (!io::ReadIJsonConvertible(intrinsic_path, intrinsic)) {
//         utility::LogError("Unable to convert json to depth intrinsics");
//     }

//     auto focal_length = intrinsic.GetFocalLength();
//     auto principal_point = intrinsic.GetPrincipalPoint();
//     Tensor intrinsic_t = Tensor::Init<double>(
//             {{focal_length.first, 0, principal_point.first},
//             {0, focal_length.second, principal_point.second},
//             {0, 0, 1}});

//     // Load color intrinsics
//     // std::string color_intrinsic_path = utility::GetProgramOptionAsString(argc, argv, "--color_intrinsic_path", "");
//     camera::PinholeCameraIntrinsic color_intrinsic = camera::PinholeCameraIntrinsic(
//             camera::PinholeCameraIntrinsicParameters::PrimeSenseDefault);
//     if (color_intrinsic_path.empty()) {
//         utility::LogWarning("Using default Primesense color intrinsics");
//     } else if (!io::ReadIJsonConvertible(color_intrinsic_path, color_intrinsic)) {
//         utility::LogError("Unable to convert json to color intrinsics");
//     }

//     auto color_focal = color_intrinsic.GetFocalLength();
//     auto color_principal = color_intrinsic.GetPrincipalPoint();
//     Tensor color_intrinsic_t = Tensor::Init<double>(
//         {{color_focal.first, 0, color_principal.first},
//         {0, color_focal.second, color_principal.second},
//         {0, 0, 1}});
//     //END INTRINSICS

//     //Voxel
//     t::geometry::VoxelBlockGrid voxel_grid(
//             {"tsdf", "weight", "color"},
//             {core::Dtype::Float32, core::Dtype::UInt16, core::Dtype::UInt16},
//             {{1}, {1}, {3}}, voxel_size, 16, 50000, device);

//     // Schedule a callback for the "scene_recon_data" topic.
//     int count = 0;
//     pb->lookup_impl<switchboard>()->schedule<scene_recon_type>(
//         id_,
//         "scene_recon_data",
//         [&](const switchboard::ptr<const scene_recon_type>& datum, size_t) {
//             callback(datum, count);
//         }
//     );
//     std::cout << "Done with callback" << "\n";
//     auto mesh = voxel_grid.ExtractTriangleMesh(0.0f);
//     auto mesh_legacy = std::make_shared<geometry::TriangleMesh>(mesh.ToLegacy());

//     cleanMesh(mesh_legacy);
//     // std::string mesh_dir = utility::GetProgramOptionAsString(argc, argv, "--mesh_dir", "mesh");
//     std::string mesh_path = mesh_dir + "/" + seq_name + ".ply";

//     struct stat info;
//     if (stat(mesh_dir.c_str(), &info) != 0) {
//             // Directory does not exist; create it.
//             mkdir(mesh_dir.c_str(), 0755);
//     }
//     open3d::io::WriteTriangleMesh(mesh_path, *mesh_legacy);
//     std::cout << "Mesh saved to " + mesh_path + "\n";
// }


// void scene_recon::callback(const switchboard::ptr<const scene_recon_type>& datum, int count) {
//     if (datum->last_frame == 1) {
//         std::cout<< "LAST FRAME" << "\n";
//     }
//     if (datum->id % divisor == 0){
//         std::cout << "Processing frame {:d}" + datum -> id << "\n";

//         //Looad images
//         // t::geometry::Image depth = datum->depth;
//         // t::geometry::Image color = datum->rgb;

//         //Converting cv::Mat to Image
//         // Is this fine or is there a way to keep using  cv::mat?
//         open3d::t::geometry::Image depth = ConvertCvMatToImage(datum->depth);
//         open3d::t::geometry::Image color = ConvertCvMatToImage(datum->rgb);

//         //Move to CPU or CUDA
//         depth = depth.To(device);
//         color = color.To(device);

//         // Trajectory(poses)
//         Eigen::Matrix4d extrinsic = ConvertPoseToEigen(datum->pose);
//         // double check if this is right and maybe remove the extra function
//         Tensor extrinsic_t = core::eigen_converter::EigenMatrixToTensor(extrinsic);

//         // // Important to specify depth scale, depth max, and trunc_mul here.
//         core::Tensor frustum_block_coords =
//             voxel_grid->GetUniqueBlockCoordinates(depth, intrinsic_t, extrinsic_t, depth_scale, depth_max, trunc_multiplier);

//         // // Scene Reconstruction
//         voxel_grid->Integrate(frustum_block_coords, depth, color, intrinsic_t, color_intrinsic_t,
//                                 extrinsic_t, depth_scale, depth_max, trunc_multiplier);
//     }
// }

// open3d::t::geometry::Image ConvertCvMatToImage(const cv::Mat& mat) {
//     cv::Mat continuousMat = mat.isContinuous() ? mat : mat.clone();

//     // Determine tensor shape based on the cv::Mat dimensions.
//     std::vector<int64_t> shape;
//     if (continuousMat.channels() == 1) {
//         shape = {continuousMat.rows, continuousMat.cols};
//     } else {
//         shape = {continuousMat.rows, continuousMat.cols, continuousMat.channels()};
//     }

//     // Map cv::Mat depth to Open3D dtype.
//     open3d::core::Dtype dtype;
//     if (continuousMat.depth() == CV_8U) {
//         dtype = open3d::core::UInt8;
//     } else if (continuousMat.depth() == CV_16U) {
//         dtype = open3d::core::UInt16;
//     } else if (continuousMat.depth() == CV_32F) {
//         dtype = open3d::core::Float32;
//     } else {
//         throw std::runtime_error("Unsupported cv::Mat type for conversion.");
//     }

//     // Create an uninitialized tensor.
//     auto tensor = open3d::core::Tensor(shape, dtype, open3d::core::Device("CPU:0"));

//     // Copy cv::Mat data into the tensor.
//     size_t total_bytes = continuousMat.total() * continuousMat.elemSize();
//     std::memcpy(tensor.GetDataPtr(), continuousMat.data, total_bytes);

//     return open3d::t::geometry::Image(tensor);
// }



// // Suppose pose_type contains a 2D array or similar:
// Eigen::Matrix4d ConvertPoseToEigen(const ILLIXR::data_format::pose_type& pose) {
//     Eigen::Matrix4d transform = Eigen::Matrix4d::Identity();

//     // Convert the orientation (quaternion) to a 3x3 rotation matrix.
//     // Note: The quaternion in pose is a float type (Eigen::Quaternionf). Cast it to double.
//     Eigen::Matrix3d rotation = pose.orientation.toRotationMatrix().cast<double>();

//     // Convert the position (Vector3f) to a Vector3d.
//     Eigen::Vector3d translation = pose.position.cast<double>();

//     // Fill in the transformation matrix.
//     transform.block<3, 3>(0, 0) = rotation;
//     transform.block<3, 1>(0, 3) = translation;

//     return transform;
// }


// void cleanMesh(std::shared_ptr<geometry::TriangleMesh> &mesh)
// {
//         std::tuple<std::vector<int>, std::vector<size_t>, std::vector<double>> clusters;
//         clusters = mesh->ClusterConnectedTriangles();

//         std::vector<bool> remove_cluster;
//         for (int idx=0; idx<std::get<1>(clusters).size(); idx++)
//         {
//             if (std::get<1>(clusters)[idx] < 200)
//                 remove_cluster.push_back(true);
//             else
//                 remove_cluster.push_back(false);
//         }
//         std::vector<bool> remove_mask;
//         for (int idx=0; idx<std::get<0>(clusters).size(); idx++)
//         {

//             if (remove_cluster[std::get<0>(clusters)[idx]])
//                 remove_mask.push_back(true);
//             else
//                 remove_mask.push_back(false);
//         }
//         mesh->RemoveTrianglesByMask(remove_mask);
//         mesh->RemoveDuplicatedVertices();
//         mesh->RemoveDuplicatedTriangles();
//         mesh->RemoveDegenerateTriangles();
//         mesh->RemoveUnreferencedVertices();
// }


// // t::geometry::Image LoadScanNetDepthPGM_uint16(const std::string &path,
// //                                               float depth_min = 0.2f,
// //                                               float depth_max = 4.0f,
// //                                               bool depth_in_mm = true,
// //                                               bool do_byte_swap = true) {
// //     // Open file and parse header
// //     std::ifstream file(path, std::ios::binary);
// //     if (!file.is_open()) {
// //         throw std::runtime_error("Could not open file: " + path);
// //     }

// //     // Read and verify magic number (must be "P5")
// //     std::string magic;
// //     std::getline(file, magic);
// //     if (magic != "P5") {
// //         throw std::runtime_error("File " + path + " is not a raw PGM (P5) file: " + magic);
// //     }

// //     // skip comment lines
// //     auto SkipCommentsAndGetLine = [&file]() -> std::string {
// //         std::string line;
// //         while (std::getline(file, line)) {
// //             if (!line.empty() && line[0] != '#') {
// //                 return line;
// //             }
// //         }
// //         return "";
// //     };

// //     // Get width and height
// //     std::string dims_line = SkipCommentsAndGetLine();
// //     std::istringstream dims_stream(dims_line);
// //     int width, height;
// //     if (!(dims_stream >> width >> height)) {
// //         throw std::runtime_error("Could not parse width and height from line: " + dims_line);
// //     }

// //     // Get maxval
// //     std::string maxval_line = SkipCommentsAndGetLine();
// //     int maxval = std::stoi(maxval_line);
// //     if (maxval > 65535) {
// //         throw std::runtime_error("Unsupported maxval=" + std::to_string(maxval) + ", must be <= 65535.");
// //     }

// //     // Read the binary raw image data as 16-bit values
// //     size_t num_pixels = static_cast<size_t>(width * height);
// //     std::vector<uint16_t> depth_raw(num_pixels);
// //     file.read(reinterpret_cast<char*>(depth_raw.data()), num_pixels * sizeof(uint16_t));
// //     if (static_cast<size_t>(file.gcount()) != num_pixels * sizeof(uint16_t)) {
// //         throw std::runtime_error("File ended before reading all depth pixels.");
// //     }
// //     file.close();

// //     // Optional byte swapping (if needed for big-endian files)
// //     if (do_byte_swap) {
// //         for (size_t i = 0; i < num_pixels; ++i) {
// //             uint16_t val = depth_raw[i];
// //             depth_raw[i] = (val >> 8) | (val << 8);
// //         }
// //     }

// //     // Clamp out-of-range depth values
// //     // If the raw values are in mm, convert thresholds from meters to mm.
// //     uint16_t min_val, max_val_threshold;
// //     if (depth_in_mm) {
// //         min_val = static_cast<uint16_t>(depth_min * 1000.0f);
// //         max_val_threshold = static_cast<uint16_t>(depth_max * 1000.0f);
// //     } else {
// //         min_val = static_cast<uint16_t>(depth_min);
// //         max_val_threshold = static_cast<uint16_t>(depth_max);
// //     }
// //     for (size_t i = 0; i < num_pixels; ++i) {
// //         if (depth_raw[i] < min_val || depth_raw[i] > max_val_threshold) {
// //             depth_raw[i] = 0;
// //         }
// //     }

// //     // Create an Open3D Tensor and then a t::geometry::Image
// //     core::Tensor depth_tensor(depth_raw, {height, width}, core::Dtype::UInt16);
// //     t::geometry::Image depth_o3d(depth_tensor);
// //     return depth_o3d;
// // }




// // open3d::camera::PinholeCameraIntrinsic LoadCalibration(const std::string filename)
// // {
// //     if (filename == "")
// //     {
// //         std::cout << "[Error] Please specify calibration file. \n";
// //         return open3d::camera::PinholeCameraIntrinsic(); // Exits the function early
// //     }
// //     std::vector<double> intrinsics;


// //     std::ifstream input;
// //     input.open(filename.c_str());
// //     while (!input.eof())
// //     {
// //         std::string line;
// //         getline(input, line);
// //         std::stringstream ss;
// //         ss << line;
// //         double value;
// //         while (ss >> value)
// //         {
// //             intrinsics.push_back(value);
// //         }
// //     }
// //     double width, height, fx, fy, cx, cy;
// //     width   = intrinsics[6];
// //     height  = intrinsics[7];
// //     fx      = intrinsics[8];
// //     fy      = intrinsics[9];
// //     cx      = intrinsics[10];
// //     cy      = intrinsics[11];

// //     open3d::camera::PinholeCameraIntrinsic intrinsic (width, height, fx, fy, cx, cy);
// //     return intrinsic;
// // }


// // int main() {
// //     using namespace open3d;

// //     // if (argc < 4) {
// //     //     utility::LogError("Not enough arguments");
// //     //     return 1;
// //     // }
    

// //     std::string color_folder = "/home/anhiti/Open3DScripts/ScanNet_Dataset/images/color_new";
// //     std::string depth_folder = "/home/anhiti/Open3DScripts/ScanNet_Dataset/images/depth";
// //     std::string trajectory_path = "/home/anhiti/Open3DScripts/ScanNet_Dataset/poses/groundtruth_rgb.txt";
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


// //     // float depth_scale = static_cast<float>(utility::GetProgramOptionAsDouble(
// //     //         argc, argv, "--depth_scale", 1000.0f));
// //     // float voxel_size = static_cast<float>(utility::GetProgramOptionAsDouble(
// //     //         argc, argv, "--voxel_size", 0.01f));
// //     // float depth_max = static_cast<float>(utility::GetProgramOptionAsDouble(
// //     //         argc, argv, "--depth_max", 4.0f));
// //     // float trunc_multiplier = static_cast<float>(utility::GetProgramOptionAsDouble(
// //     //         argc, argv, "--trunc_mul", 10.0f)); // enforce truncation band to be 0.01 * 10 = 0.1
// //     // int divisor = static_cast<int>(utility::GetProgramOptionAsInt(
// //     //         argc, argv, "--divisor", 1)); // default divisor = 1 means no skipping


// //     // std::string device_code =
// //     //         utility::GetProgramOptionAsString(argc, argv, "--device", "CUDA:0");
// //     core::Device device(device_code);
// //     utility::LogInfo("\n");
// //     utility::LogInfo("*** Execution Configuration ***");
// //     utility::LogInfo("Using device:     \t{}", device.ToString());
// //     utility::LogInfo("Using divisor:    \t{}", divisor);
// //     utility::LogInfo("Using voxel size: \t{}", voxel_size);
// //     utility::LogInfo("Using depth scale:\t{}", depth_scale);
// //     utility::LogInfo("Using depth max:  \t{}", depth_max);
// //     utility::LogInfo("Using trunc mul:  \t{}", trunc_multiplier);
// //     utility::LogInfo("\n");

// //     // Color and depth
// //     // std::string color_folder = std::string(argv[1]);
// //     // std::string depth_folder = std::string(argv[2]);

// //     std::vector<std::string> color_filenames;
// //     utility::filesystem::ListFilesInDirectory(color_folder, color_filenames);
// //     std::sort(color_filenames.begin(), color_filenames.end());

// //     std::vector<std::string> depth_filenames;
// //     utility::filesystem::ListFilesInDirectory(depth_folder, depth_filenames);
// //     std::sort(depth_filenames.begin(), depth_filenames.end());

// //     if (color_filenames.size() != depth_filenames.size()) {
// //         utility::LogError(
// //                 "Different number of color and depth files");
// //         return 0;
// //     }

// //     // Trajectory
// //     // std::string trajectory_path = std::string(argv[3]);
// //     auto trajectory =
// //             io::CreatePinholeCameraTrajectoryFromFile(trajectory_path);

// //     // Intrinsics Depth
// //     // std::string intrinsic_path = utility::GetProgramOptionAsString(argc, argv, "--intrinsic_path", "");
// //     camera::PinholeCameraIntrinsic intrinsic = camera::PinholeCameraIntrinsic(
// //             camera::PinholeCameraIntrinsicParameters::PrimeSenseDefault);
// //     if (intrinsic_path.empty()) {
// //         utility::LogWarning("Using default Primesense depth intrinsics");
// //     } else if (!io::ReadIJsonConvertible(intrinsic_path, intrinsic)) {
// //         utility::LogError("Unable to convert json to depth intrinsics");
// //     }

// //     auto focal_length = intrinsic.GetFocalLength();
// //     auto principal_point = intrinsic.GetPrincipalPoint();
// //     Tensor intrinsic_t = Tensor::Init<double>(
// //             {{focal_length.first, 0, principal_point.first},
// //             {0, focal_length.second, principal_point.second},
// //             {0, 0, 1}});

// //     // Load color intrinsics
// //     // std::string color_intrinsic_path = utility::GetProgramOptionAsString(argc, argv, "--color_intrinsic_path", "");
// //     camera::PinholeCameraIntrinsic color_intrinsic = camera::PinholeCameraIntrinsic(
// //             camera::PinholeCameraIntrinsicParameters::PrimeSenseDefault);
// //     if (color_intrinsic_path.empty()) {
// //         utility::LogWarning("Using default Primesense color intrinsics");
// //     } else if (!io::ReadIJsonConvertible(color_intrinsic_path, color_intrinsic)) {
// //         utility::LogError("Unable to convert json to color intrinsics");
// //     }

// //     auto color_focal = color_intrinsic.GetFocalLength();
// //     auto color_principal = color_intrinsic.GetPrincipalPoint();
// //     Tensor color_intrinsic_t = Tensor::Init<double>(
// //         {{color_focal.first, 0, color_principal.first},
// //         {0, color_focal.second, color_principal.second},
// //         {0, 0, 1}});
    


    
// //     // Log intrinsics
// //     utility::LogInfo("Depth Intrinsics: {}x{} f=({}, {}), c=({}, {})",
// //                      intrinsic.width_, intrinsic.height_,
// //                      focal_length.first, focal_length.second,
// //                      principal_point.first, principal_point.second);

// //     utility::LogInfo("Color Intrinsics: {}x{} f=({}, {}), c=({}, {})",
// //                      color_intrinsic.width_, color_intrinsic.height_,
// //                      color_focal.first, color_focal.second,
// //                      color_principal.first, color_principal.second);


// //     t::geometry::VoxelBlockGrid voxel_grid(
// //             {"tsdf", "weight", "color"},
// //             {core::Dtype::Float32, core::Dtype::UInt16, core::Dtype::UInt16},
// //             {{1}, {1}, {3}}, voxel_size, 16, 50000, device);

// //     utility::LogInfo("\n");
    
    
// //     //Processing Frames
// //     for (size_t index = 0; index < color_filenames.size(); index++) 
// //     {
// //         if (index % divisor != 0) continue;
// //         utility::LogInfo("Processing frame {:d}", index);

// //         std::string color_path = color_filenames[index];
// //         std::string depth_path = depth_filenames[index];

// //         //Looad images
// //         t::geometry::Image depth = LoadScanNetDepthPGM_uint16(depth_path, 0.2f, 4.0f, true, true);
// //         t::geometry::Image color =
// //                 (*t::io::CreateImageFromFile(color_path));

// //         //Move to CPU or CUDA
// //         depth = depth.To(device);
// //         color = color.To(device);

// //         // Trajectory(poses)
// //         Eigen::Matrix4d extrinsic = trajectory->parameters_[index].extrinsic_;
// //         Tensor extrinsic_t = core::eigen_converter::EigenMatrixToTensor(extrinsic);

// //         // Important to specify depth scale, depth max, and trunc_mul here.
// //         core::Tensor frustum_block_coords =
// //                 voxel_grid.GetUniqueBlockCoordinates(depth, intrinsic_t, extrinsic_t,
// //                                                      depth_scale, depth_max, trunc_multiplier);
        
// //         //Scene Reconstruction
// //         voxel_grid.Integrate(frustum_block_coords, depth, color, intrinsic_t, color_intrinsic_t,
// //                              extrinsic_t, depth_scale, depth_max, trunc_multiplier);
// //     }

// //     // std::string seq_name =
// //     //         utility::GetProgramOptionAsString(argc, argv, "--seq");

// //     //Saving output
// //     // if (utility::ProgramOptionExists(argc, argv, "--save_mesh")) {
// //     auto mesh = voxel_grid.ExtractTriangleMesh(0.0f);
// //     auto mesh_legacy = std::make_shared<geometry::TriangleMesh>(mesh.ToLegacy());

// //     cleanMesh(mesh_legacy);
// //     // std::string mesh_dir = utility::GetProgramOptionAsString(argc, argv, "--mesh_dir", "mesh");
// //     std::string mesh_path = mesh_dir + "/" + seq_name + ".ply";

// //     struct stat info;
// //     if (stat(mesh_dir.c_str(), &info) != 0) {
// //             // Directory does not exist; create it.
// //             mkdir(mesh_dir.c_str(), 0755);
// //     }
// //     open3d::io::WriteTriangleMesh(mesh_path, *mesh_legacy);
// //     std::cout << "Mesh saved to " + mesh_path + "\n";
// //     // }

// //     // if (utility::ProgramOptionExists(argc, argv, "--save_pcd")) {
// //     //     auto pcd = voxel_grid.ExtractPointCloud(0.0f);
// //     //     auto pcd_legacy =
// //     //             std::make_shared<open3d::geometry::PointCloud>(pcd.ToLegacy());
// //     //     open3d::io::WritePointCloud("pcd_" + device.ToString() + ".ply",
// //     //                                 *pcd_legacy);
// //     // }

// //     // if (utility::ProgramOptionExists(argc, argv, "--tsdf")) {
// //     //     voxel_grid.Save("tsdf.npz");
// //     // }

// //     return 0;
// // }

// // Register the plugin with ILLIXR.
// PLUGIN_MAIN(scene_recon);