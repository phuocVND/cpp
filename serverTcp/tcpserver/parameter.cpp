#include "parameter.h"

Parameter::Parameter()
{
    this->parameterAngle->angle1 = 0;
    this->parameterAngle->angle2 = 0;
    this->parameterAngle->angle3 = 0;
    this->parameterAngle->angle4 = 0;
    this->parameterAngle->angle5 = 0;
    this->parameterAngle->angle6 = 0;
    this->isChanged = false;
}

bool Parameter::checkChangedPara()
{
    this->isChanged =
        this->parameterAngle->angle1 != this->parameterSaved->angle1 ||
        this->parameterAngle->angle2 != this->parameterSaved->angle2 ||
        this->parameterAngle->angle3 != this->parameterSaved->angle3 ||
        this->parameterAngle->angle4 != this->parameterSaved->angle4 ||
        this->parameterAngle->angle5 != this->parameterSaved->angle5 ||
        this->parameterAngle->angle6 != this->parameterSaved->angle6;

    return this->isChanged;
}

void Parameter::set_parameter()
{
    if(checkChangedPara())
    {
        this->parameterAngle->angle1 = this->parameterSaved->angle1;
        this->parameterAngle->angle2 = this->parameterSaved->angle2;
        this->parameterAngle->angle3 = this->parameterSaved->angle3;
        this->parameterAngle->angle4 = this->parameterSaved->angle4;
        this->parameterAngle->angle5 = this->parameterSaved->angle5;
        this->parameterAngle->angle6 = this->parameterSaved->angle6;
    }
}
