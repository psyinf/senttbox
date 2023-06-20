#pragma once
//TODO: enum class with operators/magic_enum
enum ShowInterface
{
    DEFAULT     = 0,
    DEBUG_MOUSE = 1 << 0,
    DEBUG_TIME = 1 << 1,


};
/*Global scene properties*/
struct SceneProperties
{
    ShowInterface showInterface{DEFAULT};

    double        timestep_scale{1.0}; // 1.0 means real time time passage for now
    bool          paused{false};
};