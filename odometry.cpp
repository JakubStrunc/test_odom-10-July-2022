#include "main.h"

using namespace pros;

// encoders values
int L_ENC_value = 0.0;
int M_ENC_value = 0.0;

// change in encoders
int lDelta = 0.0;
int mDelta = 0.0;

// previous encoders values
int last_lDelta = 0.0;
int last_mDelta = 0.0;

// distance in inches
float lDist = 0.0;
float mDist = 0.0;

// The current angle of the bot (RADIANS)
float currentAbsoluteOrientation = 0;
// The previous angle of the bot (RADIANS)
float last_Theta = 0;

// change in the angle(RADIANS)
float deltaTheta = 0;

// average theta for the arc
float avgThetaForArc = 0;

// local position
float x_pos_local = 0;
float y_pos_local = 0;

// the global postion in arc
float delta_x_pos_global = 0;
float delta_y_pos_global = 0;

// global position of the robot
float x_pos_global = 0;
float y_pos_global = 0;

// total local position
float t_x_pos_local = 0;
float t_y_pos_local = 0;

int odomtrack()
{
  while (true)
  {

    // getting values from encoders
    L_ENC_value = L_Encoder.get_value();
    M_ENC_value = M_Encoder.get_value();

    // get the change of the values
    lDelta = L_ENC_value - last_lDelta;
    mDelta = M_ENC_value - last_mDelta;

    // trasform values to inches
    lDist = lDelta / degrees_side_per_inch;
    mDist = mDelta / degrees_side_per_inch;

    // calculate the current angle of the robot in radians
    currentAbsoluteOrientation = (int)inertial.get_heading() * drive_pi / 180;

    // get change in the angle
    deltaTheta = currentAbsoluteOrientation - last_Theta;

    // transform my current values to previous values
    last_lDelta = L_ENC_value;
    last_mDelta = M_ENC_value;
    last_Theta = currentAbsoluteOrientation;

    // if there is no angle change, then it is only change in encoders
    if (deltaTheta == 0)
    { 
      y_pos_local = lDist;
      x_pos_local = mDist;
      
    }
    // calculate the x-position and y-position
    else
    {
      y_pos_local = (lDist / deltaTheta + dRobotSide) * (2 * sinf(deltaTheta / 2.0));
      x_pos_local = (mDist / deltaTheta + dRobotMiddle) * (2 * sinf(deltaTheta / 2.0));
    }

    t_x_pos_local += x_pos_local;
    t_y_pos_local += y_pos_local;
    // the average angle in the arc
    avgThetaForArc = (currentAbsoluteOrientation + (deltaTheta / 2)) * -1.0f  ;

    // calculate the global change in the position
    delta_y_pos_global = (x_pos_local * sinf(avgThetaForArc)) + (y_pos_local * cosf(avgThetaForArc));
    delta_x_pos_global = (x_pos_local * cosf(avgThetaForArc)) - (y_pos_local * sinf(avgThetaForArc));

    // uptade my global position of the robot
    y_pos_global += delta_y_pos_global;
    x_pos_global += delta_x_pos_global;



    int currentAbsoluteOrientation_deg = inertial.get_heading();
    // printing the values
    lcd::print(3, "x:%f, y:%f, ang:%d", x_pos_global, y_pos_global, currentAbsoluteOrientation_deg);// global x, global y, the robot heading in deg
    // lcd::print(4, "pl:%d, pm:%d, pt:%d", last_lDelta, last_mDelta, deltaTheta);//left encoder delta, middle encoder delta, theate delte(radians)
    // lcd::print(5, "ty:%f,tx:%f",t_y_pos_local, t_x_pos_local);// total y local, total, x local
    // lcd::print(6, "lenc:%d, menc:%d",L_ENC_value, M_ENC_value);// values from left encoder, values from middle encoder

    delay(20);
  }

  return 1;
}