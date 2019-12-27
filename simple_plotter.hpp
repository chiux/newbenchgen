#ifndef PLOTTER_HPP
#define PLOTTER_HPP

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

namespace simple_plotter {

class Plotter {
 public:
  Plotter() {
    type_to_ids_.insert(
        make_pair("point", std::unordered_map<int, std::vector<int>>()));
    type_to_ids_.insert(
        make_pair("segment", std::unordered_map<int, std::vector<int>>()));
    type_to_ids_.insert(
        make_pair("arrow", std::unordered_map<int, std::vector<int>>()));
  }

  void Plot(std::ostream& os, const std::string& title) const {
    os << "set title '" << title << "'" << std::endl;
    os << std::endl;

    for (int i = 0; i < styles_.size(); ++i) {
      const auto& style = styles_[i];
      const int style_id = i + 1;
      os << "set style " << style.type_ << " " << style_id << " "
         << style.value_ << std::endl;
    }
    os << std::endl;

    os << "set size ratio -1" << std::endl;
    os << "set clip two" << std::endl;
    os << "unset key" << std::endl;

    os << std::endl;
    os << "plot ";
    for (const auto& pair : type_to_ids_) {
      const std::string type = pair.first;
      const auto& style_id_to_ids = pair.second;
      if (type == "point") {
        for (const auto& ppair : style_id_to_ids) {
          const int style_id = ppair.first;
          os << "'-' w p ls " << style_id << ", ";
        }
      } else if (type == "segment") {
        for (const auto& ppair : style_id_to_ids) {
          const int style_id = ppair.first;
          os << "'-' w l ls " << style_id << ", ";
        }
      } else if (type == "arrow") {
        for (const auto& ppair : style_id_to_ids) {
          const int style_id = ppair.first;
          os << "'-' w vectors arrowstyle " << style_id << ", ";
        }
      }
    }
    os << std::endl;

    for (const auto& pair : type_to_ids_) {
      const std::string type = pair.first;
      const auto& style_id_to_ids = pair.second;
      if (type == "point") {
        for (const auto& ppair : style_id_to_ids) {
          const auto& point_ids = ppair.second;
          os << std::endl;
          for (const int point_id : point_ids) {
            const auto& point = points_.at(point_id);
            os << point.x_ << " " << point.y_ << std::endl;
            os << std::endl;
          }
          os << "e" << std::endl;
        }
      } else if (type == "segment") {
        for (const auto& ppair : style_id_to_ids) {
          const auto& segment_ids = ppair.second;
          os << std::endl;
          for (const int segment_id : segment_ids) {
            const auto& segment = segments_.at(segment_id);
            os << segment.from_point_x_ << " " << segment.from_point_y_
               << std::endl;
            os << segment.to_point_x_ << " " << segment.to_point_y_
               << std::endl;
            os << std::endl;
          }
          os << "e" << std::endl;
        }
      } else if (type == "arrow") {
        for (const auto& ppair : style_id_to_ids) {
          const auto& arrow_ids = ppair.second;
          os << std::endl;
          for (const int arrow_id : arrow_ids) {
            const auto& arrow = arrows_.at(arrow_id);
            os << arrow.from_point_x_ << " " << arrow.from_point_y_ << " "
               << arrow.to_point_x_ - arrow.from_point_x_ << " "
               << arrow.to_point_y_ - arrow.from_point_y_ << std::endl;
            os << std::endl;
          }
          os << "e" << std::endl;
        }
      }
    }

    os << std::endl;
    os << "pause -1 'Press enter to exit.'" << std::endl;
  }

  // Setters

  int AddStyle(const std::string& type, const std::string& value) {
    styles_.push_back(Style(type, value));
    const int style_id = styles_.size();
    return style_id;
  }

  void AddPoint(double point_x, double point_y, int style_id) {
    const int id = points_.size();
    points_.push_back(Point(point_x, point_y));
    if (type_to_ids_.at("point").count(style_id) == 0) {
      type_to_ids_.at("point").insert(make_pair(style_id, std::vector<int>()));
    }
    type_to_ids_.at("point").at(style_id).push_back(id);
  }

  void AddSegment(double from_point_x, double from_point_y, double to_point_x,
                  double to_point_y, int style_id) {
    const int id = segments_.size();
    segments_.push_back(
        Segment(from_point_x, from_point_y, to_point_x, to_point_y));
    if (type_to_ids_.at("segment").count(style_id) == 0) {
      type_to_ids_.at("segment").insert(
          make_pair(style_id, std::vector<int>()));
    }
    type_to_ids_.at("segment").at(style_id).push_back(id);
  }

  void AddArrow(double from_point_x, double from_point_y, double to_point_x,
                double to_point_y, int style_id) {
    const int id = arrows_.size();
    arrows_.push_back(
        Arrow(from_point_x, from_point_y, to_point_x, to_point_y));
    if (type_to_ids_.at("arrow").count(style_id) == 0) {
      type_to_ids_.at("arrow").insert(make_pair(style_id, std::vector<int>()));
    }
    type_to_ids_.at("arrow").at(style_id).push_back(id);
  }

  void AddRect(double lower_left_point_x, double lower_left_point_y,
               double upper_right_point_x, double upper_right_point_y,
               int style_id) {
    AddSegment(lower_left_point_x, lower_left_point_y, lower_left_point_x,
               upper_right_point_y, style_id);
    AddSegment(lower_left_point_x, upper_right_point_y, upper_right_point_x,
               upper_right_point_y, style_id);
    AddSegment(upper_right_point_x, upper_right_point_y, upper_right_point_x,
               lower_left_point_y, style_id);
    AddSegment(upper_right_point_x, lower_left_point_y, lower_left_point_x,
               lower_left_point_y, style_id);
  }

 private:
  class Style {
   public:
    Style(const std::string& type, const std::string& value)
        : type_(type), value_(value) {
    }

    std::string type_;
    std::string value_;
  };

  class Point {
   public:
    Point(double x, double y) : x_(x), y_(y) {
    }

    double x_;
    double y_;
  };

  class Segment {
   public:
    Segment(double from_point_x, double from_point_y, double to_point_x,
            double to_point_y)
        : from_point_x_(from_point_x),
          from_point_y_(from_point_y),
          to_point_x_(to_point_x),
          to_point_y_(to_point_y) {
    }

    double from_point_x_;
    double from_point_y_;
    double to_point_x_;
    double to_point_y_;
  };

  class Arrow {
   public:
    Arrow(double from_point_x, double from_point_y, double to_point_x,
          double to_point_y)
        : from_point_x_(from_point_x),
          from_point_y_(from_point_y),
          to_point_x_(to_point_x),
          to_point_y_(to_point_y) {
    }

    double from_point_x_;
    double from_point_y_;
    double to_point_x_;
    double to_point_y_;
  };

  std::vector<Style> styles_;
  std::vector<Point> points_;
  std::vector<Segment> segments_;
  std::vector<Arrow> arrows_;
  std::unordered_map<std::string, std::unordered_map<int, std::vector<int>>>
      type_to_ids_;
};
}

#endif
