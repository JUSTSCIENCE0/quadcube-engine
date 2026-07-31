// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#include <qce/mjson/generated_objects.hpp>

using namespace QCE;

int main(int argc, char* argv[]) {
    auto schema = macrojson::get_json_schema_str<TransformAnimation>();
    std::cout << "TransformAnimation" << std::endl;
    std::cout << "Generated JSON Schema:" << std::endl;
    std::cout << schema << std::endl;
    std::cout << std::endl;

    QCE::TransformAnimation square_path{};
    square_path.id = "square_path";
    square_path.position_channel = {
        {
            /*value*/ { 1.0f, 0.0f, 1.0f },
            /*start_time*/ 0.0f
        },
        {
            /*value*/ { 1.0f, 0.0f, -1.0f },
            /*start_time*/ 1.0f
        },
        {
            /*value*/ { 1.0f, 1.0f, -1.0f },
            /*start_time*/ 2.0f
        },
        {
            /*value*/ { 1.0f, 1.0f, 1.0f },
            /*start_time*/ 3.0f
        },
        {
            /*value*/ { 1.0f, 0.0f, 1.0f },
            /*start_time*/ 4.0f
        }
    };
    square_path.scale_channel = {
        {
            /*value*/ { 1.0f, 1.0f, 1.0f },
            /*start_time*/ 1.0f
        },
        {
            /*value*/ { 1.0f, 3.0f, 1.0f },
            /*start_time*/ 2.0f
        },
        {
            /*value*/ { 3.0f, 1.0f, 1.0f },
            /*start_time*/ 3.0f
        },
        {
            /*value*/ { 1.0f, 1.0f, 1.0f },
            /*start_time*/ 4.0f
        }
    };
    square_path.rotation_channel = {
        {
            /*value*/ QCE::euler_deg_to_quaternion(0.0f, 0.0f, 0.0f),
            /*start_time*/ 0.0f
        },
        {
            /*value*/ QCE::euler_deg_to_quaternion(0.0f, 90.0f, 0.0f),
            /*start_time*/ 1.0f
        },
        {
            /*value*/ QCE::euler_deg_to_quaternion(0.0f, 180.0f, 0.0f),
            /*start_time*/ 2.0f
        },
        {
            /*value*/ QCE::euler_deg_to_quaternion(0.0f, 270.0f, 0.0f),
            /*start_time*/ 3.0f
        },
        {
            /*value*/ QCE::euler_deg_to_quaternion(0.0f, 0.0f, 0.0f),
            /*start_time*/ 4.0f
        }
    };

    std::string json{};
    macrojson::object_to_json_str(square_path, json);

    std::cout << "Serialized JSON:" << std::endl;
    std::cout << json << std::endl;
    std::cout << std::endl;

    std::string error_descr;
    TransformAnimation out_pos{};
    macrojson::json_str_to_object(json, out_pos, error_descr);

    return 0;
}