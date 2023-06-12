// Moddable JavaScript shim for xio Fusion library
// Copyright © 2023 by Thorsten von Eicken.

const ConventionNWU = 0 // North-West-Up
const ConventionENU = 1 // East-North-Up
const ConventionNED = 2 // North-East-Down

export class AHRS @ "xs_FusionAhrsDestroy" {
  constructor (convention, gain, accelRej, magRej, rejTimeout) @ "xs_FusionAhrsConstruct";
  reset () @ "xs_FusionAhrsReset";
  update(gyroscope, accelerometer, magnetometer, deltaTicks) @ "xs_FusionAhrsUpdate";
  updateNoMagnetometer(gyroscope, accelerometer, deltaTicks) @ "xs_FusionAhrsUpdateNoMagnetometer";
  updateExternalHeading(gyroscope, accelerometer, heading, deltaTicks) @ "xs_FusionAhrsUpdateExternalHeading";
  _quaternion () @ "xs_FusionAhrsGetQuaternion";
  get quaternion () { return new Quaternion(...this._quaternion()) };
  get linearAcceleration () @ "xs_FusionAhrsGetLinearAcceleration";
  get earthAcceleration () @ "xs_FusionAhrsGetEarthAcceleration";
  get states () @ "xs_FusionAhrsGetInternalStates";
  get flags () @ "xs_FusionAhrsGetFlags";
  set heading (heading) @ "xs_FusionAhrsSetHeading";
}

export class Quaternion {
  constructor (w, x, y, z) @ "xs_FusionQuaternionConstruct";
  toEuler () @ "xs_FusionQuaternionToEuler";
  _toMatrix4 () @ "xs_FusionQuaternionToMatrix4";
  toMatrix4() { return new Float64Array(this._toMatrix4()) };
}

export class GyroOffset @ "xs_FusionGyroOffsetDestroy" {
  constructor (sampleRate) @ "xs_FusionGyroOffsetConstruct";
  update(gyroscope) @ "xs_FusionGyroOffsetUpdate";
}

export { AHRS as default }
