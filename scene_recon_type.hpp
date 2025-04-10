#ifndef ILLIXR_DATA_FORMAT_SCENE_RECON_TYPE_HPP
#define ILLIXR_DATA_FORMAT_SCENE_RECON_TYPE_HPP

#include <opencv4/opencv2/core/mat.hpp>
#include "illixr/data_format/pose.hpp"
#include "illixr/switchboard.hpp"
#include "illixr/phonebook.hpp"
#include "illixr/switchboard.hpp"

namespace ILLIXR::data_format {
  struct scene_recon_type:public switchboard::event {
      [[maybe_unused]] time_point time;
      pose_type pose;
      cv::Mat depth;
      cv::Mat rgb;
      bool last_frame;
      unsigned id;
      scene_recon_type(time_point camera_time, pose_type pose_, cv::Mat depth_, cv::Mat rgb_, bool is_last_frame, unsigned id_)
        : time{camera_time}
      , pose{pose_}
      , depth{depth_}
      , rgb{rgb_}
      , last_frame{is_last_frame}
      , id{id_}{}
      // scene_recon_type(time_point camera_time, pose_type pose_, cv::Mat depth_, cv::Mat rgb_, bool is_last_frame, unsigned id_)
      //   : time{camera_time}
      // , pose{pose_}
      // , depth{depth_}
      // , rgb{rgb_}
      // , last_frame{is_last_frame}
      // , id{id_}{}
      // //~scene_recon_type() {
      // //   std::cout << "scene_recon_type id: "<<id<<" is being destroyed." << std::endl;
      // //   std::cout.flush();
      // //}
    };
}
#endif