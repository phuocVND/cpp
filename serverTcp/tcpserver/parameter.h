#pragma once
#include <iostream>

struct parameterAngle
{
    float angle1;
    float angle2;
    float angle3;
    float angle4;
    float angle5;
    float angle6;
};

class Parameter
{
public:
    Parameter();
    ~Parameter();
    void set_parameter();
    bool checkChangedPara();
private:
    bool isChanged;
    struct parameterAngle *parameterAngle;
    struct parameterAngle *parameterSaved;
};

