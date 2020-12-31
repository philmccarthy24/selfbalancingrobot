#pragma once

#include <utility>

namespace sbr 
{

    class ISpacialSensor 
    {
    public:
        ISpacialSensor() {}
        virtual ~ISpacialSensor() {}
        virtual std::pair<double, double> ReadOrientation() = 0;
    };
}