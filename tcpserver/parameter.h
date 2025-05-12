#pragma once

#include <QtCore/qlist.h>
#include <QtCore/qobject.h>

struct ParameterAngle {
    float angle1;
    float angle2;
    float angle3;
    float angle4;
    float angle5;
    float angle6;
};

class Parameter : public QObject {
    Q_OBJECT
public:
    Parameter();
    ~Parameter();
    Q_INVOKABLE void set_parameter(const QList<double> &values);
    ParameterAngle* get_parameter();
    bool checkChangedPara();
    bool isChanged;
signals:
    void valueChanged();

private:

    void set_parameter_save();
    ParameterAngle *parameterAngle;
    ParameterAngle *parameterSaved;
};
