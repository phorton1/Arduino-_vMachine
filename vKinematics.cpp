/*  Copied and modified from the Maslow Control Software.

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
    2021 - Patrick Horton
*/


#include "vKinematics.h"
#include "vMachine.h"
#include <Report.h>

#define DEBUG_INVERSE  0
#define DEBUG_FORWARD  0
    // DEBUG_FORWARD can be upto 3 for more detail

#if DEBUG_INVERSE
    bool suppress_inverse_debugging = 0;
        // inverse debugging is, by default, disabled from forward calls
#endif

int prh_debug_forward = 0;
    // by default forward debugging is only enabled from the serial port ? command
    // see Report.cpp for prh comments


#define CHAIN_OVER_SPROCKET    1
    // versus under (I only use OVER) sysSettings.chainOverSprocket
#define USE_SAG_CALCS          1
    // whether they're compiled in or not


//------------------------------------
// note on machine dimensions
//------------------------------------
// the center of the motor is 24.3mm above the board
// the top of the "machine area" is 5" (127mm) below the top of the board
//
// the most critical dimensions are the distance between motor centers,
//    and the distance to the top left and right corners of the
//    "machine area", as those points represent the "zero" of the
//    X and Y chain lengths (motor positions), respectively.
//
// Everything else is calculated from those zero points.
//
// These chain lengths are stored in the global int32 "sys_position"
//    as the number of steps on each stepper motor.
//
//    Given the grbl "steps/mm" setting (50) the chain lengths
//    in steps can be converted to mm's and vice versa.
//
//    When in mm's the chain lengths are refered to as the
//    motor_positions.
//
//    The method cartesian_to_motors() is used to calculate
//    the needed chain lengths to arrive at a given X,Y location.
//    It is only called when a move is to take place, so it ALSO
//    calls mc_line() which (calls the planner, etc to actually)
//    effect the movements which all take place in stepper motor
//    positions. This is the "inverse" kinematics.
//
// The "machine" postition is in cartesian coordinates from the
// bottom left of the "machine area" in mm's.  It can be
// approximated from the chain lengths via the motors_to_cartesian()
// method that is ONLY called for reporting .. never for machine
// movements.  There are also storable "work" coordinate systems,
// that are managed by GRBL (see %54, $#, etc) but they are not
// important for this discussion.


//---------------------------------
// only used in quad calcs
//---------------------------------

#ifdef WITH_QUAD_KINEMATICS
    #define USE_QUAD_KINEMATICS     0                  // sysSettings.kinematicsType == 1
    #define SLED_WIDTH              50    // 310.0     // sysSettings.sledWidth
    #define SLED_HEIGHT             50    // 139.0     // sysSettings.sledHeight
    #define SLED_CG                 0     // 79.0      // sysSettings.sledCG (don't understand)
    #define DELTAPHI                0.001
    #define DELTAY                  0.01
    #define KINEMATICSMAXERROR      0.001
    #define KINEMATICSMAXINVERSE    10
#endif


//======================================================================
// my Kinematics
//======================================================================


Kinematics::Kinematics()
{
}


void Kinematics::init()
{
    info_serial("Kinematics::init()");

    // used in triangle calcs

    dist_between_motors = v_machine.getDistBetweenMotors();     // 730.25
    safe_offset = v_machine.getSafeAreaOffset();                // 5.0
    sprocketRadius = v_machine.getSprocketRadius();             // 4.8
    halfWidth = v_machine.getMachineWidth() / 2.0;              // 200
    halfHeight = v_machine.getMachineHeight() / 2.0;            // 150
    _xCordOfMotor = dist_between_motors/2;                      // 730.25 / 2 = 365.125
    _yCordOfMotor = halfHeight + v_machine.getMotorOffsetY();   // 150 + 151.30 = 301.30

    max_forward_guesses = v_machine.getMaxForwardGuesses();
    forward_guess_tolerance = v_machine.getForwardGuessTolerance();
    max_guess_chain_length = v_machine.getGuessMaxChainLength();

    #ifdef WITH_QUAD_KINEMATICS
        Phi = -0.2;
        Theta = atan(2*SLED_HEIGHT/SLED_WIDTH);
        Psi1 = Theta - Phi;
        Psi2 = Theta + Phi;
        h = sqrt((SLED_WIDTH/2)*(SLED_WIDTH/2) + SLED_HEIGHT * SLED_HEIGHT);
    #endif
}



void Kinematics::_constrainTarget(float* xTarget,float* yTarget)
    // constrain to -half <= p <= +half +/- SAFE_AREA_OFFSET
{

    *xTarget =
        (*xTarget < -halfWidth-safe_offset) ? -halfWidth-safe_offset :
        (*xTarget > halfWidth+safe_offset) ? halfWidth+safe_offset :
        *xTarget;
    *yTarget =
        (*yTarget < -halfHeight-safe_offset) ? -halfHeight-safe_offset :
        (*yTarget > halfHeight+safe_offset) ? halfHeight+safe_offset :
        *yTarget;
}


//----------------------------------
// inverse kinematics
//----------------------------------

void Kinematics::inverse(
        float xTarget,
        float yTarget,
        float* aChainLength,
        float* bChainLength,
        bool from_forward /*= false*/)
#ifdef WITH_QUAD_KINEMATICS
    {
        if (USE_QUAD_KINEMATICS)
        {
            _quadrilateralInverse(xTarget, yTarget, aChainLength, bChainLength, from_forward);
        }
        else
        {
            _triangularInverse(xTarget, yTarget, aChainLength, bChainLength, from_forward);
        }
    }
    void Kinematics::_triangularInverse(
            float xTarget,
            float yTarget,
            float* aChainLength,
            float* bChainLength,
            bool from_forward)
#endif

    // The inverse kinematics (relating an xy coordinate pair to the required chain lengths to hit that point)
    // function for a triangular set up where the chains meet at a point, or are arranged so that they simulate
    // meeting at a point.
    //
    // The result *should* be the chain length (from the point it departs the sprocket)
    // to the point, given that 0,0 is the bottom left corner of the WORK_AREA, which is
    // (-half_width,-half_height) relative to the center of the work_area

{
    #if DEBUG_INVERSE
        if (!suppress_inverse_debugging)
        {
            debug_serial("inverse   start target(%f,%f)",
                xTarget,
                yTarget);
        }
    #endif

    // the passed in coordinates are from 0,0 at left bottom of work area
    // this code works from the center point (with positive numbers in the
    // bottom right hand quadrant) so we offset the coordinates
    // to the center of the piece ..

    xTarget -= halfWidth;
    yTarget -= halfHeight;

    #if DEBUG_INVERSE
        if (!suppress_inverse_debugging)
        {
            debug_serial("  inverse offset target(%f,%f)",
                xTarget,
                yTarget);
        }
    #endif


    // constrain to the work area
    // This clipping causes the forward to not work near edges of work area
    // so I have commented it out. Perhaps a margin of safety is called for.
    //

    if (!in_homing && !from_forward)
        _constrainTarget(&xTarget, &yTarget);


    // Calculate straight length from center of motor to the center of the pen
    // so, for 0,0 the calculation goes like this
    //
    //   Motor1Distance = sqrt( (-343.625 - 0)^2 + (301.50 - 0)*2 )
    //       457.1437   calculated = 457.03216
    //   Motor2Distance = sqrt( (343.625 - 0)^2 + (301.50 - 0)*2 )
    //       457.1437   calculated = 457.03216

    float Motor1Distance = sqrt(pow((-1*_xCordOfMotor - xTarget),2)+pow((_yCordOfMotor - yTarget),2));
    float Motor2Distance = sqrt(pow((_xCordOfMotor - xTarget),2)+pow((_yCordOfMotor - yTarget),2));

    // Calculate the belt angles from horizontal,
    // we ONLY use CHAIN_OVER_SPROCKET, so the angle is from
    // top dead center to the tangent point the belt departs

    #if CHAIN_OVER_SPROCKET
        float Chain1Angle = asin((_yCordOfMotor - yTarget)/Motor1Distance) + asin(sprocketRadius/Motor1Distance);
        float Chain2Angle = asin((_yCordOfMotor - yTarget)/Motor2Distance) + asin(sprocketRadius/Motor2Distance);
        float Chain1AroundSprocket = sprocketRadius * Chain1Angle;
        float Chain2AroundSprocket = sprocketRadius * Chain2Angle;
    #else
        float Chain1Angle = asin((_yCordOfMotor - yTarget)/Motor1Distance) - asin(sprocketRadius/Motor1Distance);
        float Chain2Angle = asin((_yCordOfMotor - yTarget)/Motor2Distance) - asin(sprocketRadius/Motor2Distance);
        float Chain1AroundSprocket = sprocketRadius * (3.14159 - Chain1Angle);
        float Chain2AroundSprocket = sprocketRadius * (3.14159 - Chain2Angle);
    #endif

    #if DEBUG_INVERSE
        if (!suppress_inverse_debugging)
        {
            debug_serial("  inverse distance(%f,%f) angle(%f,%f) around(%f,%f)",
                Motor1Distance,
                Motor2Distance,
                Chain1Angle,
                Chain2Angle,
                Chain1AroundSprocket,
                Chain2AroundSprocket);
        }
    #endif

    // Calculate the straight chain length from where the belt leaves the sprocket
    // to the center of the pen.  The belt leaves at a tangent point, forming a right
    // triangle with a radius of the sprocket. MotorDistance is the hypotenuse of
    // that right triangle, so below we solve for the other side (center of pen to
    // tangent point)

    float Chain1Straight = sqrt(pow(Motor1Distance,2)-pow(sprocketRadius,2));
    float Chain2Straight = sqrt(pow(Motor2Distance,2)-pow(sprocketRadius,2));

    // This code mimics exactly what is found at
    // https://github.com/MaslowCNC/Firmware/blob/master/cnc_ctrl_v1/Kinematics.cpp
    // except that it partitions out the values for debugging and sticks them
    // in "Chain1Straight" instead of Chain 1

    #if USE_SAG_CALCS

        float Chain1Sag = Chain1Straight *
            (v_machine.getSagCorrection()/1000000000000) *
            pow(cos(Chain1Angle),2) *
            pow(Chain1Straight,2) *
            pow((tan(Chain2Angle)*cos(Chain1Angle))+ sin(Chain1Angle),2);
        float Chain2Sag = Chain2Straight *
            (v_machine.getSagCorrection()/1000000000000) *
            pow(cos(Chain2Angle),2) *
            pow(Chain2Straight,2) *
            pow((tan(Chain1Angle)*cos(Chain2Angle))+ sin(Chain2Angle),2);

        #if DEBUG_INVERSE
            if (!suppress_inverse_debugging)
            {
                debug_serial("  inverse straight(%f,%f) += sag(%f,%f)",
                    Chain1Straight,
                    Chain2Straight,
                    Chain1Sag,
                    Chain2Sag);
            }
        #endif

        Chain1Straight += Chain1Sag;
        Chain2Straight += Chain2Sag;

        float leftToleranceFactor = (1.0f + v_machine.getChainLeftTolerance() / 100.0f);
        float rightToleranceFactor = (1.0f + v_machine.getChainRightTolerance() / 100.0f);

        #if DEBUG_INVERSE
            if (!suppress_inverse_debugging)
            {
                debug_serial("  inverse straight(%f,%f) /= tolerance_factor(%f,%f)",
                    Chain1Straight,
                    Chain2Straight,
                    leftToleranceFactor,
                    rightToleranceFactor);
            }
        #endif

        Chain1Straight /= leftToleranceFactor;
        Chain2Straight /= rightToleranceFactor;

        #if DEBUG_INVERSE
            if (!suppress_inverse_debugging)
            {
                debug_serial("  inverse straight(%f,%f) -= rotation_disk_radius(%f)",
                    Chain1Straight,
                    Chain2Straight,
                    v_machine.getSledRadius());
            }
        #endif

        Chain1Straight -= v_machine.getSledRadius();
        Chain2Straight -= v_machine.getSledRadius();

    #endif


    #if DEBUG_INVERSE
        if (!suppress_inverse_debugging)
        {
            debug_serial("  inverse chain = straight(%f,%f) + aroundSprocket(%f,%f)",
                Chain1Straight,
                Chain2Straight,
                Chain1AroundSprocket,
                Chain2AroundSprocket);
        }
    #endif


    float Chain1 = Chain1Straight + Chain1AroundSprocket;
    float Chain2 = Chain2Straight + Chain2AroundSprocket;


    #if DEBUG_INVERSE
        if (!suppress_inverse_debugging)
        {
            debug_serial("inverse  finish target(%f,%f) final(%f,%f)]",
                xTarget + halfWidth,
                yTarget + halfHeight,
                Chain1,
                Chain2);
        }
    #endif

    *aChainLength = Chain1;
    *bChainLength = Chain2;

}   // Kinematics::triangularInverse()



//-------------------------------------
// forward kinematics
//-------------------------------------

void  Kinematics::forward(
        float chainALength,
        float chainBLength,
        float* xPos,
        float* yPos)
    // The iterative part of this method has problems.
    // prh - note this probably will not work well with (untested) quad kinematics
{
    #if DEBUG_FORWARD
        if (prh_debug_forward)
            debug_serial("forward   start lengths(%f,%f)",
                chainALength,
                chainBLength);
    #endif

    // If the sum of the chain lengths is less than DIST_BETWEEN_MOTORS
    // THERE IS NO INTERSECTION !!

    if (chainALength + chainBLength < dist_between_motors)
    {
        // if (!in_homing)
        //     info_serial(
        //         "sum of chainLengths(%f,%f) less than (%f) - the chains do not interssect!!",
        //         chainALength,
        //         chainBLength,
        //         dist_between_motors);
        *xPos = 0;
        *yPos = 0;
        return;
    }

    //------------------------
    // initial guess
    //------------------------
    // First we find a first approximation of the position based on the given chain lengths
    // as if they came from the center of the motor, THEN using that we reverse the process
    // from the inverse() method to subtract out the "chain over sprocket", go from the
    // hypotenuse to the calculated center distance, an then a second calculation of the
    // intersection point, to provide our initial guess to the iterative routine that follows.

    // from https://math.stackexchange.com/questions/256100/how-can-i-find-the-points-at-which-two-circles-intersect
    // answer 11, also from https://mathworld.wolfram.com/Circle-CircleIntersection.html
    //
    //  let d == DIST_BETWEEN_MOTORS
    //  and define 0,0 as the center of the top left motor
    //
    //  given two circles of radii r1 and r2 centered on the motors,
    //  the equations for the point(s) x,y where the circles intersect are
    //
    //      x = (r1^2 - r2^2 + d^2) / 2d
    //      y = sqrt( r1^2 - x^2 )
    //
    //  note that r2^2 - x^2 is likely negative and there are two solutions
    //  so we negate it if it is negative
    //
    //      y = sqrt( x^2 - r1^2  )

    // Calculate the first x,y approximation

    float xGuess = (
        chainALength*chainALength - chainBLength*chainBLength +
        dist_between_motors*dist_between_motors) /
        (2*dist_between_motors);
    float yGuess = chainALength*chainALength - xGuess*xGuess;
    if (yGuess < 0) yGuess = -yGuess;
    yGuess = sqrt(yGuess);

    #if DEBUG_FORWARD > 1
        if (prh_debug_forward)
            debug_serial("   forward guess intersection xy(%f,%f)",
                xGuess,
                yGuess);
    #endif

    // Offset the xGuess from the motor to the work area

    xGuess -= v_machine.getMotorOffsetX();

    // Offset yGuess from the motor to the work area.
    // The intersection is currently ABOVE the motor axes.
    // To offset them to our coordinate system, we reverse it
    // into our space (as a negative number from the top)
    // and subtract that from the Y coordinate in our system
    // of the motor line (MACHINE_HEIGHT + MOTOR_OFFSET_Y)

    yGuess = -yGuess + v_machine.getMachineHeight() + v_machine.getMotorOffsetY();

    #if DEBUG_FORWARD > 1
        if (prh_debug_forward)
            debug_serial("   forward guess offset xy(%f,%f)",
                xGuess,
                yGuess);
    #endif

    // The above gave us the x,y coordinates of the intersection of the chains
    // assuming they are coming from the center of the motor.
    //
    // When we do the forward kinematics, however, the chain length is to the
    // top dead center of the sprocket, based on the tangent distance to the
    // point from where the chain leaves the sprocket, and the amount that goes
    // from there to TDC of the sprocket.
    //
    // Following, we use the above solution to get the approximate angle of the chains,
    // calculate the tangent point and the amount over the sprocket at that angle.
    //
    // (0) convert to center relative calculations
    // (1) calculate (approximate)  chain angle and amount over the sprocket
    // (2) subtract the amount over the sprocket to get hypotenuse of triangle
    // (3) solve for the shorter edge of the triangle to get the straight chain length and
    // (4) do the intersection calculations again based on new straight chain lengths

    // (0) convert to center relative calculations

    float xTarget = xGuess - halfWidth;
    float yTarget = yGuess - halfHeight;

    // (1) calculate (approximate) the chain angle and  the amount over the sprocket

    float Motor1Distance = sqrt(pow((-1*_xCordOfMotor - xTarget),2)+pow((_yCordOfMotor - yTarget),2));
    float Motor2Distance = sqrt(pow((_xCordOfMotor - xTarget),2)+pow((_yCordOfMotor - yTarget),2));

    float Chain1Angle = asin((_yCordOfMotor - yTarget)/Motor1Distance) + asin(sprocketRadius/Motor1Distance);
    float Chain2Angle = asin((_yCordOfMotor - yTarget)/Motor2Distance) + asin(sprocketRadius/Motor2Distance);
    float Chain1AroundSprocket = sprocketRadius * Chain1Angle;
    float Chain2AroundSprocket = sprocketRadius * Chain2Angle;

    // (2) subtract the amount over the sprocket to get hypotenuse of triangle

    float hypA = chainALength - Chain1AroundSprocket;
    float hypB = chainBLength - Chain2AroundSprocket;

    // (3) solve for the straight chain length
    //          a^2 + b^2 = c^2
    //  so      a = sqrt(c^2 - b^2)

    float straightA = sqrt(hypA*hypA - sprocketRadius*sprocketRadius);
    float straightB = sqrt(hypB*hypB - sprocketRadius*sprocketRadius);

    #if DEBUG_FORWARD > 1
        if (prh_debug_forward)
            debug_serial("   forward guess distance(%f,%f) angle(%f,%f) over(%f,%f) hyp(%f,%f)",
                Motor1Distance,
                Motor2Distance,
                Chain1Angle,
                Chain2Angle,
                Chain1AroundSprocket,
                Chain2AroundSprocket,
                hypA,
                hypB);
    #endif

    // (4) do the circle intersection calculations again

    xGuess = (straightA*straightA -
              straightB*straightB +
              dist_between_motors*dist_between_motors) /
              (2*dist_between_motors);
    yGuess = straightA*straightA - xGuess*xGuess;
    if (yGuess < 0) yGuess = -yGuess;
    yGuess = sqrt(yGuess);
    xGuess -= v_machine.getMotorOffsetX();
    yGuess = -yGuess + v_machine.getMachineHeight() + v_machine.getMotorOffsetY();

    #if DEBUG_FORWARD > 1
        if (prh_debug_forward)
            debug_serial("   forward guess final xy(%f,%f) from straight(%f,%f)",
                xGuess,
                yGuess,
                straightA,
                straightB);
    #endif

    // This guess is very close in most cases.
    // In some cases (i.e. 200,150) it is within 1/10th of a millimeter.


    #if DEBUG_INVERSE
        bool supress_first = 1;
        #if DEBUG_FORWARD > 1
            if (prh_debug_forward)
                supress_first = 0;
        #endif
    #endif


    //------------------------------------
    // iterative loop
    //------------------------------------

    int guessCount = 0;
    float initial_xGuess = xGuess;
    float initial_yGuess = yGuess;
    float guessLengthA;
    float guessLengthB;

    while (1)
    {
        //check our guess

        #if DEBUG_INVERSE
            bool save_suppress = suppress_inverse_debugging;
            suppress_inverse_debugging = supress_first;
        #endif

        inverse(xGuess, yGuess, &guessLengthA, &guessLengthB, true);

        #if DEBUG_INVERSE
            suppress_inverse_debugging = save_suppress;
            supress_first = 1;
        #endif


        float aChainError = chainALength - guessLengthA;
        float bChainError = chainBLength - guessLengthB;

        guessCount++;

        #if DEBUG_FORWARD > 2
            if (prh_debug_forward)
                debug_serial("   forward(%d) guess_xy(%f,%f) lenAB(%f,%f) err(%f,%f)]",
                    guessCount,
                    xGuess,
                    yGuess,
                    guessLengthA,
                    guessLengthB,
                    aChainError,
                    bChainError);
        #endif

        //if we've converged on the point...or it's time to give up, exit the loop
        // prh - i changed the tolerance from 0.1 to 0.5 with addition of initial guess code

        if ((abs(aChainError) < forward_guess_tolerance &&
             abs(bChainError) < forward_guess_tolerance) ||
            guessCount   > max_forward_guesses ||
            guessLengthA > max_guess_chain_length  ||
            guessLengthB > max_guess_chain_length)
        {
            if ((guessCount > max_forward_guesses ) ||
                guessLengthA > max_guess_chain_length ||
                guessLengthB > max_guess_chain_length)
            {
                if (!in_homing)
                    info_serial("forward count:%d chainLength(%f,%f) guessLength(%f,%f) returning(%f,%f) Unable to find valid machine position for chain lengths",
                        guessCount,
                        chainALength,
                        chainBLength,
                        guessLengthA,
                        guessLengthB);
                *xPos = initial_xGuess;
                *yPos = initial_yGuess;
                return;
            }
            else
            {
                #if DEBUG_FORWARD
                    if (prh_debug_forward)
                        debug_serial("forward final(%f,%f)] guesses(%d)",
                           xGuess,
                           yGuess,
                           guessCount);
                #endif
                *xPos = xGuess;
                *yPos = yGuess;
            }
            break;
        }

        // adjust the next guess based on the result
        // this original formula has problem converging on a result
        // I'm not sure why the pluses and minuses are used.

        xGuess = xGuess + .1*aChainError - .1*bChainError;
        yGuess = yGuess - .1*aChainError - .1*bChainError;

    }   // while (1)
}   // Kinematics::forward()




//==================================================================================
// unused quad stuff
//==================================================================================
// has not been compiled with new preferences

#ifdef WITH_QUAD_KINEMATICS
    void  Kinematics::_quadrilateralInverse(
            float xTarget,
            float yTarget,
            float* aChainLength,
            float* bChainLength,
            bool from_forward)
    {
        //Confirm that the coordinates are on the wood
        _verifyValidTarget(&xTarget, &yTarget);

        //coordinate shift to put (0,0) in the center of the plywood from the left sprocket
        y = (halfHeight) + MOTOR_OFFSET_Y  - yTarget;
        x = (DIST_BETWEEN_MOTORS/2.0) + xTarget;

        //Coordinates definition:
        //         x -->, y |
        //                  v
        // (0,0) at center of left sprocket
        // upper left corner of plywood (270, 270)

        byte Tries = 0;                                  //initialize
        if(x > DIST_BETWEEN_MOTORS/2.0){                              //the right half of the board mirrors the left half so all computations are done  using left half coordinates.
          x = DIST_BETWEEN_MOTORS-x;                                  //Chain lengths are swapped at exit if the x,y is on the right half
          Mirror = true;
        }
        else{
            Mirror = false;
        }

        TanGamma = y/x;
        TanLambda = y/(DIST_BETWEEN_MOTORS-x);
        Y1Plus = sprocketRadius * sqrt(1 + TanGamma * TanGamma);
        Y2Plus = sprocketRadius * sqrt(1 + TanLambda * TanLambda);


        while (Tries <= KINEMATICSMAXINVERSE) {

            _MyTrig();
                                                 //These criteria will be zero when the correct values are reached
                                                 //They are negated here as a numerical efficiency expedient

            Crit[0]=  - _moment(Y1Plus, Y2Plus, MySinPhi, SinPsi1, CosPsi1, SinPsi2, CosPsi2);
            Crit[1] = - _YOffsetEqn(Y1Plus, x - h * CosPsi1, SinPsi1);
            Crit[2] = - _YOffsetEqn(Y2Plus, DIST_BETWEEN_MOTORS - (x + h * CosPsi2), SinPsi2);

            if (abs(Crit[0]) < KINEMATICSMAXERROR) {
                if (abs(Crit[1]) < KINEMATICSMAXERROR) {
                    if (abs(Crit[2]) < KINEMATICSMAXERROR){
                        break;
                    }
                }
            }

                       //estimate the tilt angle that results in zero net _moment about the pen
                       //and refine the estimate until the error is acceptable or time runs out

                              //Estimate the Jacobian components

            Jac[0] = (_moment( Y1Plus, Y2Plus, MySinPhiDelta, SinPsi1D, CosPsi1D, SinPsi2D, CosPsi2D) + Crit[0])/DELTAPHI;
            Jac[1] = (_moment( Y1Plus + DELTAY, Y2Plus, MySinPhi, SinPsi1, CosPsi1, SinPsi2, CosPsi2) + Crit[0])/DELTAY;
            Jac[2] = (_moment(Y1Plus, Y2Plus + DELTAY, MySinPhi, SinPsi1, CosPsi1, SinPsi2, CosPsi2) + Crit[0])/DELTAY;
            Jac[3] = (_YOffsetEqn(Y1Plus, x - h * CosPsi1D, SinPsi1D) + Crit[1])/DELTAPHI;
            Jac[4] = (_YOffsetEqn(Y1Plus + DELTAY, x - h * CosPsi1,SinPsi1) + Crit[1])/DELTAY;
            Jac[5] = 0.0;
            Jac[6] = (_YOffsetEqn(Y2Plus, DIST_BETWEEN_MOTORS - (x + h * CosPsi2D), SinPsi2D) + Crit[2])/DELTAPHI;
            Jac[7] = 0.0;
            Jac[8] = (_YOffsetEqn(Y2Plus + DELTAY, DIST_BETWEEN_MOTORS - (x + h * CosPsi2D), SinPsi2) + Crit[2])/DELTAY;


            //solve for the next guess
            _MatSolv();     // solves the matrix equation Jx=-Criterion

            // update the variables with the new estimate

            Phi = Phi + Solution[0];
            Y1Plus = Y1Plus + Solution[1];                         //don't allow the anchor points to be inside a sprocket
            Y1Plus = (Y1Plus < sprocketRadius) ? sprocketRadius : Y1Plus;

            Y2Plus = Y2Plus + Solution[2];                         //don't allow the anchor points to be inside a sprocke
            Y2Plus = (Y2Plus < sprocketRadius) ? sprocketRadius : Y2Plus;

            Psi1 = Theta - Phi;
            Psi2 = Theta + Phi;

        Tries++;                                       // increment itteration count

        }

        //Variables are within accuracy limits
        //  perform output computation

        Offsetx1 = h * CosPsi1;
        Offsetx2 = h * CosPsi2;
        Offsety1 = h *  SinPsi1;
        Offsety2 = h * SinPsi2;
        TanGamma = (y - Offsety1 + Y1Plus)/(x - Offsetx1);
        TanLambda = (y - Offsety2 + Y2Plus)/(DIST_BETWEEN_MOTORS -(x + Offsetx2));
        Gamma = atan(TanGamma);
        Lambda =atan(TanLambda);

        //compute the chain lengths

        if(Mirror){
            Chain2 = sqrt((x - Offsetx1)*(x - Offsetx1) + (y + Y1Plus - Offsety1)*(y + Y1Plus - Offsety1)) - sprocketRadius * TanGamma + sprocketRadius * Gamma;   //right chain length
            Chain1 = sqrt((DIST_BETWEEN_MOTORS - (x + Offsetx2))*(DIST_BETWEEN_MOTORS - (x + Offsetx2))+(y + Y2Plus - Offsety2)*(y + Y2Plus - Offsety2)) - sprocketRadius * TanLambda + sprocketRadius * Lambda;   //left chain length
        }
        else{
            Chain1 = sqrt((x - Offsetx1)*(x - Offsetx1) + (y + Y1Plus - Offsety1)*(y + Y1Plus - Offsety1)) - sprocketRadius * TanGamma + sprocketRadius * Gamma;   //left chain length
            Chain2 = sqrt((DIST_BETWEEN_MOTORS - (x + Offsetx2))*(DIST_BETWEEN_MOTORS - (x + Offsetx2))+(y + Y2Plus - Offsety2)*(y + Y2Plus - Offsety2)) - sprocketRadius * TanLambda + sprocketRadius * Lambda;   //right chain length
        }

        *aChainLength = Chain1;
        *bChainLength = Chain2;

    }

    void  Kinematics::_MatSolv(){
        float Sum;
        int NN;
        int i;
        int ii;
        int J;
        int JJ;
        int K;
        int KK;
        int L;
        int M;
        int N;

        float fact;

        // gaus elimination, no pivot

        N = 3;
        NN = N-1;
        for (i=1;i<=NN;i++){
            J = (N+1-i);
            JJ = (J-1) * N-1;
            L = J-1;
            KK = -1;
            for (K=0;K<L;K++){
                fact = Jac[KK+J]/Jac[JJ+J];
                for (M=1;M<=J;M++){
                    Jac[KK + M]= Jac[KK + M] -fact * Jac[JJ+M];
                }
            KK = KK + N;
            Crit[K] = Crit[K] - fact * Crit[J-1];
            }
        }

    //Lower triangular matrix solver

        Solution[0] =  Crit[0]/Jac[0];
        ii = N-1;
        for (i=2;i<=N;i++){
            M = i -1;
            Sum = Crit[i-1];
            for (J=1;J<=M;J++){
                Sum = Sum-Jac[ii+J]*Solution[J-1];
            }
        Solution[i-1] = Sum/Jac[ii+i];
        ii = ii + N;
        }
    }


    float Kinematics::_moment(
            float Y1Plus,
            float Y2Plus,
            float MSinPhi,
            float MSinPsi1,
            float MCosPsi1,
            float MSinPsi2,
            float MCosPsi2)
        //computes net moment about center of mass
    {
        float Offsetx1;
        float Offsetx2;
        float Offsety1;
        float Offsety2;
        float TanGamma;
        float TanLambda;

        Offsetx1 = h * MCosPsi1;
        Offsetx2 = h * MCosPsi2;
        Offsety1 = h * MSinPsi1;
        Offsety2 = h * MSinPsi2;
        TanGamma = (y - Offsety1 + Y1Plus)/(x - Offsetx1);
        TanLambda = (y - Offsety2 + Y2Plus)/(DIST_BETWEEN_MOTORS -(x + Offsetx2));

        return SLED_CG *MSinPhi + (h/(TanLambda+TanGamma))*(MSinPsi2 - MSinPsi1 + (TanGamma*MCosPsi1 - TanLambda * MCosPsi2));
    }


    void Kinematics::_MyTrig()
    {
        float Phisq = Phi * Phi;
        float Phicu = Phi * Phisq;
        float Phidel = Phi + DELTAPHI;
        float Phidelsq = Phidel * Phidel;
        float Phidelcu = Phidel * Phidelsq;
        float Psi1sq = Psi1 * Psi1;
        float Psi1cu = Psi1sq * Psi1;
        float Psi2sq = Psi2 * Psi2;
        float Psi2cu = Psi2 * Psi2sq;
        float Psi1del = Psi1 - DELTAPHI;
        float Psi1delsq = Psi1del * Psi1del;
        float Psi1delcu = Psi1del * Psi1delsq;
        float Psi2del = Psi2 + DELTAPHI;
        float Psi2delsq = Psi2del * Psi2del;
        float Psi2delcu = Psi2del * Psi2delsq;

        // Phirange is 0 to -27 degrees
        // sin -0.1616   -0.0021    1.0002   -0.0000 (error < 6e-6)
        // cos(phi): 0.0388   -0.5117    0.0012    1.0000 (error < 3e-5)
        // Psi1 range is 42 to  69 degrees,
        // sin(Psi1):  -0.0942   -0.1368    1.0965   -0.0241 (error < 2.5 e-5)
        // cos(Psi1):  0.1369   -0.6799    0.1077    0.9756  (error < 1.75e-5)
        // Psi2 range is 15 to 42 degrees
        // sin(Psi2): -0.1460   -0.0197    1.0068   -0.0008 (error < 1.5e-5)
        // cos(Psi2):  0.0792   -0.5559    0.0171    0.9981 (error < 2.5e-5)

        MySinPhi = -0.1616*Phicu - 0.0021*Phisq + 1.0002*Phi;
        MySinPhiDelta = -0.1616*Phidelcu - 0.0021*Phidelsq + 1.0002*Phidel;

        SinPsi1 = -0.0942*Psi1cu - 0.1368*Psi1sq + 1.0965*Psi1 - 0.0241;//sinPsi1
        CosPsi1 = 0.1369*Psi1cu - 0.6799*Psi1sq + 0.1077*Psi1 + 0.9756;//cosPsi1
        SinPsi2 = -0.1460*Psi2cu - 0.0197*Psi2sq + 1.0068*Psi2 - 0.0008;//sinPsi2
        CosPsi2 = 0.0792*Psi2cu - 0.5559*Psi2sq + 0.0171*Psi2 + 0.9981;//cosPsi2

        SinPsi1D = -0.0942*Psi1delcu - 0.1368*Psi1delsq + 1.0965*Psi1del - 0.0241;//sinPsi1
        CosPsi1D = 0.1369*Psi1delcu - 0.6799*Psi1delsq + 0.1077*Psi1del + 0.9756;//cosPsi1
        SinPsi2D = -0.1460*Psi2delcu - 0.0197*Psi2delsq + 1.0068*Psi2del - 0.0008;//sinPsi2
        CosPsi2D = 0.0792*Psi2delcu - 0.5559*Psi2delsq + 0.0171*Psi2del +0.9981;//cosPsi2
    }


    float Kinematics::_YOffsetEqn(
            float YPlus,
            float Denominator,
            float Psi)
    {
        float Temp;
        Temp = ((sqrt(YPlus * YPlus - sprocketRadius * sprocketRadius)/sprocketRadius) - (y + YPlus - h * sin(Psi))/Denominator);
        return Temp;
    }

#endif  // WITH_QUAD_KINEMATICS
