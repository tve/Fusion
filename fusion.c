#include "xsmc.h"
#include "xsHost.h"
#include "Fusion.h"

#define xsmcVar(x) xsVar(x)

void xs_FusionAhrsConstruct(xsMachine *the)
{
	int argc = xsmcArgc;
  // the settings are internally copied in FusionAhrsSetSettings, so stack alloc is OK
  FusionAhrsSettings settings;
  settings.convention = argc > 0 ? xsmcToInteger(xsArg(0)) : 0;
  settings.gain = argc > 1 ? xsmcToNumber(xsArg(1)) : 0.5;
  settings.accelerationRejection = argc > 2 ? xsmcToNumber(xsArg(2)) : 10;
  settings.magneticRejection = argc > 3 ? xsmcToNumber(xsArg(3)) : 20;
  settings.rejectionTimeout = argc > 4 ? xsmcToInteger(xsArg(4)) : 0;

  // allocate FusionAhrs internal data structure
	FusionAhrs *ahrs = xsmcSetHostChunk(xsThis, NULL, sizeof(FusionAhrs));

  // initialise FusionAhrs
  FusionAhrsInitialise(ahrs);
  FusionAhrsSetSettings(ahrs, &settings);
}

void xs_FusionAhrsDestroy(void *data)
{
}

void xs_FusionAhrsReset(xsMachine *the)
{
  FusionAhrs *ahrs = xsmcGetHostChunk(xsThis);
  FusionAhrsReset(ahrs);
}

void extractVector(xsMachine *the, xsSlot slot, FusionVector *vector)
{
  xsmcVars(1);
  xsmcGet(xsmcVar(0), slot, xsID("x"));
  vector->axis.x = xsmcToNumber(xsVar(0));
  xsmcGet(xsmcVar(0), slot, xsID("y"));
  vector->axis.y = xsmcToNumber(xsVar(0));
  xsmcGet(xsmcVar(0), slot, xsID("z"));
  vector->axis.z = xsmcToNumber(xsVar(0));
}

void xs_FusionAhrsUpdate(xsMachine *the)
{
  FusionAhrs *ahrs = xsmcGetHostChunk(xsThis);
  FusionVector gyroscope;
  extractVector(the, xsArg(0), &gyroscope);
  FusionVector accelerometer;
  extractVector(the, xsArg(1), &accelerometer);
  FusionVector magnetometer;
  extractVector(the, xsArg(2), &magnetometer);
  unsigned int deltaTicks = xsmcToInteger(xsArg(3));
  float deltaTime = deltaTicks / 1000.0; // milliseconds -> seconds
  FusionAhrsUpdate(ahrs, gyroscope, accelerometer, magnetometer, deltaTime);
}

void xs_FusionAhrsUpdateNoMagnetometer(xsMachine *the)
{
  FusionAhrs *ahrs = xsmcGetHostChunk(xsThis);
  FusionVector gyroscope;
  extractVector(the, xsArg(0), &gyroscope);
  FusionVector accelerometer;
  extractVector(the, xsArg(1), &accelerometer);
  unsigned int deltaTicks = xsmcToInteger(xsArg(2));
  float deltaTime = deltaTicks / 1000.0; // milliseconds -> seconds
  // xsLog("acc=[%d,%d,%d] gyr=[%d,%d,%d] dt=%dms ",
  //   (int)(accelerometer.axis.x*10), (int)(10*accelerometer.axis.y), (int)(10*accelerometer.axis.z),
  //   (int)(10*gyroscope.axis.x), (int)(10*gyroscope.axis.y), (int)(10*gyroscope.axis.z),
  //   (int)(1000*deltaTime));
  FusionAhrsUpdateNoMagnetometer(ahrs, gyroscope, accelerometer, deltaTime);
}

void xs_FusionAhrsUpdateExternalHeading(xsMachine *the)
{
  xsUnknownError("not implemented");
}

void defineProperty(xsMachine *the, xsSlot obj, const char *name, float value)
{
  xsmcVars(1);
  xsmcSetNumber(xsmcVar(0), value);
  xsmcSet(obj, xsID(name), xsmcVar(0));
}

void xs_FusionAhrsGetQuaternion(xsMachine *the)
{
  FusionAhrs *ahrs = xsmcGetHostChunk(xsThis);
#if 1
  xsResult = xsmcNewArray(4);
  xsmcVars(2);
  xsmcSetNumber(xsmcVar(0), ahrs->quaternion.element.w);
  xsmcSetNumber(xsmcVar(1), 0);
  xsmcSetAt(xsResult, xsmcVar(1), xsmcVar(0));
  xsmcSetNumber(xsmcVar(0), ahrs->quaternion.element.x);
  xsmcSetNumber(xsmcVar(1), 1);
  xsmcSetAt(xsResult, xsmcVar(1), xsmcVar(0));
  xsmcSetNumber(xsmcVar(0), ahrs->quaternion.element.y);
  xsmcSetNumber(xsmcVar(1), 2);
  xsmcSetAt(xsResult, xsmcVar(1), xsmcVar(0));
  xsmcSetNumber(xsmcVar(0), ahrs->quaternion.element.z);
  xsmcSetNumber(xsmcVar(1), 3);
  xsmcSetAt(xsResult, xsmcVar(1), xsmcVar(0));
#else
#if 0
  xsmcVars(1);
  xsmcSetNumber(xsmcVar(0), 0);
  xsmcNew(xsResult, xsGlobal, xsID("Quaternion"), xsmcVar(0));
#else
  xsResult = xsNewObject();
#endif
  // this code cheats a bit and reaches into the internal data structure...
  defineProperty(the, xsResult, "w", ahrs->quaternion.element.w);
  defineProperty(the, xsResult, "x", ahrs->quaternion.element.x);
  defineProperty(the, xsResult, "y", ahrs->quaternion.element.y);
  defineProperty(the, xsResult, "z", ahrs->quaternion.element.z);
  #endif
}

void xs_FusionAhrsGetLinearAcceleration(xsMachine *the)
{
  FusionAhrs *ahrs = xsmcGetHostChunk(xsThis);
  FusionVector linearAcceleration = FusionAhrsGetLinearAcceleration(ahrs);
  xsResult = xsNewObject();
  defineProperty(the, xsResult, "x", linearAcceleration.axis.x);
  defineProperty(the, xsResult, "y", linearAcceleration.axis.y);
  defineProperty(the, xsResult, "z", linearAcceleration.axis.z);
}

void xs_FusionAhrsGetEarthAcceleration(xsMachine *the)
{
  FusionAhrs *ahrs = xsmcGetHostChunk(xsThis);
  FusionVector earthAcceleration = FusionAhrsGetEarthAcceleration(ahrs);
  xsResult = xsNewObject();
  defineProperty(the, xsResult, "x", earthAcceleration.axis.x);
  defineProperty(the, xsResult, "y", earthAcceleration.axis.y);
  defineProperty(the, xsResult, "z", earthAcceleration.axis.z);
}

void xs_FusionAhrsGetInternalStates(xsMachine *the)
{
  FusionAhrs *ahrs = xsmcGetHostChunk(xsThis);
  FusionAhrsInternalStates states = FusionAhrsGetInternalStates(ahrs);
  xsResult = xsNewObject();
  defineProperty(the, xsResult, "accelerationError", states.accelerationError);
  defineProperty(the, xsResult, "accelerometerIgnored", states.accelerometerIgnored);
  defineProperty(the, xsResult, "accelerationRejectionTimer", states.accelerationRejectionTimer);
  defineProperty(the, xsResult, "magneticError", states.magneticError);
  defineProperty(the, xsResult, "magnetometerIgnored", states.magnetometerIgnored);
  defineProperty(the, xsResult, "magneticRejectionTimer", states.magneticRejectionTimer);
}

void xs_FusionAhrsGetFlags(xsMachine *the)
{
  FusionAhrs *ahrs = xsmcGetHostChunk(xsThis);
  FusionAhrsFlags flags = FusionAhrsGetFlags(ahrs);
  xsResult = xsNewObject();
  defineProperty(the, xsResult, "initialising", flags.initialising);
  defineProperty(the, xsResult, "accelerationRejectionWarning", flags.accelerationRejectionWarning);
  defineProperty(the, xsResult, "accelerationRejectionTimeout", flags.accelerationRejectionTimeout);
  defineProperty(the, xsResult, "magneticRejectionWarning", flags.magneticRejectionWarning);
  defineProperty(the, xsResult, "magneticRejectionTimeout", flags.magneticRejectionTimeout);
}

void xs_FusionAhrsSetHeading(xsMachine *the)
{
  FusionAhrs *ahrs = xsmcGetHostChunk(xsThis);
  float heading = xsmcToNumber(xsArg(0));
  FusionAhrsSetHeading(ahrs, heading);
}

// ===== Quaternion

void xs_FusionQuaternionConstruct(xsMachine *the)
{
  xsmcVars(1);
  // xsmcSetHostChunk(xsThis, NULL, 0); // Necessary?
  if (xsmcArgc == 4) {
    xsmcSet(xsThis, xsID("w"), xsArg(0));
    xsmcSet(xsThis, xsID("x"), xsArg(1));
    xsmcSet(xsThis, xsID("y"), xsArg(2));
    xsmcSet(xsThis, xsID("z"), xsArg(3));
  }
}

void extractQuaternion(xsMachine *the, xsSlot obj, FusionQuaternion *quaternion)
{
  xsmcVars(1);
  xsmcGet(xsmcVar(0), obj, xsID("w"));
  quaternion->element.w = xsmcToNumber(xsmcVar(0));
  xsmcGet(xsmcVar(0), obj, xsID("x"));
  quaternion->element.x = xsmcToNumber(xsmcVar(0));
  xsmcGet(xsmcVar(0), obj, xsID("y"));
  quaternion->element.y = xsmcToNumber(xsmcVar(0));
  xsmcGet(xsmcVar(0), obj, xsID("z"));
  quaternion->element.z = xsmcToNumber(xsmcVar(0));
}

void xs_FusionQuaternionToEuler(xsMachine *the)
{
  FusionQuaternion quaternion;
  extractQuaternion(the, xsThis, &quaternion);
  FusionEuler angles = FusionQuaternionToEuler(quaternion);
  xsResult = xsNewObject();
  defineProperty(the, xsResult, "pitch", angles.angle.pitch);
  defineProperty(the, xsResult, "roll", angles.angle.roll);
  defineProperty(the, xsResult, "yaw", angles.angle.yaw);
}

void xs_FusionQuaternionToMatrix4(xsMachine *the)
{
  FusionQuaternion quaternion;
  extractQuaternion(the, xsThis, &quaternion);
  FusionMatrix matrix = FusionQuaternionToMatrix(quaternion);
  xsResult = xsNewObject();
  // defineProperty(the, xsResult, "pitch", matrix.angle.pitch);
  // defineProperty(the, xsResult, "roll", matrix.angle.roll);
  // defineProperty(the, xsResult, "yaw", matrix.angle.yaw);
}

// ===== GyroOffset

void xs_FusionGyroOffsetConstruct(xsMachine *the)
{
  FusionOffset fo;
  FusionOffsetInitialise(&fo, xsmcToInteger(xsArg(0)));
  xsmcSetHostChunk(xsThis, &fo, sizeof(FusionOffset));
}

void xs_FusionGyroOffsetDestroy(xsMachine *the)
{
}

void xs_FusionGyroOffsetUpdate(xsMachine *the)
{
  FusionOffset *fo = xsmcGetHostChunk(xsThis);
  FusionVector gyroscope;
  extractVector(the, xsArg(0), &gyroscope);
  gyroscope = FusionOffsetUpdate(fo, gyroscope);
  xsResult = xsNewObject();
  defineProperty(the, xsResult, "x", gyroscope.axis.x);
  defineProperty(the, xsResult, "y", gyroscope.axis.y);
  defineProperty(the, xsResult, "z", gyroscope.axis.z);
}
