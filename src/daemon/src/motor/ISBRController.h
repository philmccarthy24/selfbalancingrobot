#pragma once

namespace sbrcontroller {
    namespace motor {

    /**
     * Top level control of self balancing state, and motion
     * control while self-balanced
     **/
    class ISBRController
    {
    public:
        ISBRController();
        virtual ~ISBRController();

        virtual void BeginControl() = 0;
        virtual void EndControl() = 0;

        // TODO: more control for balanced velocity and turning.
        // decide whether motion is created through change of target angle
        // or simply set.
    };

    }
}