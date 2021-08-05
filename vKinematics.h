/*  This file is part of the Maslow Control Software.

    The Maslow Control Software is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Maslow Control Software is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with the Maslow Control Software.  If not, see <http://www.gnu.org/licenses/>.

    Copyright 2014-2017 Bar Smith
*/

#pragma once


#include <cstdint>


extern int prh_debug_forward;
    // see prh debugging code Report.cpp
extern bool in_homing;
    // in vMachine.cpp

// #define WITH_QUAD_KINEMATICS

class Kinematics{
    public:
        Kinematics();
        void init  ();

        void inverse(
            float xTarget,
            float yTarget,
            float* aChainLength,
            float* bChainLength,
            bool from_forward = false);

        void forward(
            float chainALength,
            float chainBLength,
            float* xPos,
            float* yPos);


    private:
        void _constrainTarget(float* xTarget,float* yTarget);

        // cached preference constants (used in triangle calcs)

        float sprocketRadius;
        float dist_between_motors;
        float halfWidth;            // Half the machine width
        float halfHeight;           // Half the machine height
        float _xCordOfMotor;        // center relative positive x position of the motor (dist_between_motors/2)
        float _yCordOfMotor;        // cached halfHeight + vMachine::getOffsetY()
        float safe_offset;
        int   max_forward_guesses;
        float forward_guess_tolerance;
        float max_guess_chain_length;



        #ifdef WITH_QUAD_KINEMATICS
            void  _triangularInverse(float xTarget, float yTarget, float* aChainLength, float* bChainLength, bool from_forward);
            void  _quadrilateralInverse(float xTarget, float yTarget, float* aChainLength, float* bChainLength, bool from_forward);

            // constants (everything else only used in quad calcs)

            float Phi;
            float Theta;
            float Psi1;
            float Psi2;
            float h;                // distance between sled attach point and bit

            bool Mirror;

            float Chain1;
            float Chain2;
            float x = 0;
            float y = 0;
            float Motor1Distance;   // left motor axis distance to sled
            float Motor2Distance;   // right motor axis distance to sled

            float TanGamma;
            float TanLambda;
            float Y1Plus ;
            float Y2Plus;
            float Jac[9];
            float Solution[3];
            float Crit[3];
            float Offsetx1;
            float Offsetx2;
            float Offsety1;
            float Offsety2;
            float SinPsi1;
            float CosPsi1;
            float SinPsi2;
            float CosPsi2;
            float SinPsi1D;
            float CosPsi1D;
            float SinPsi2D;
            float CosPsi2D;
            float MySinPhi;
            float MySinPhiDelta;

            float Lambda;
            float Gamma;

            float _moment(
                float Y1Plus,
                float Y2Plus,
                float MSinPhi,
                float MSinPsi1,
                float MCosPsi1,
                float MSinPsi2,
                float MCosPsi2);
            float _YOffsetEqn(
                float YPlus,
                float Denominator,
                float Psi);
            void _MatSolv();
            void _MyTrig();
        #endif

};
