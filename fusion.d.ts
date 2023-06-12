declare module "embedded:lib/IMU/fusion" {
  export interface Vector {
    x: number
    y: number
    z: number
  }

  export interface Euler {
    roll: number
    pitch: number
    yaw: number
  }

  export class Quaternion {
    w: number
    x: number
    y: number
    z: number
    constructor(w: number, x: number, y: number, z: number)
    toEuler(): Euler
  }

  export class AHRS {
    constructor(
      convention?: number,
      gain?: number,
      accelRej?: number,
      magRej?: number,
      rejTimeout?: number
    )
    reset(): void
    update(gyroscope: Vector, accelerometer: Vector, magnetometer: Vector, deltaTicks: number): void
    updateNoMagnetometer(gyroscope: Vector, accelerometer: Vector, deltaTicks: number): void
    updateExternalHeading(
      gyroscope: Vector,
      accelerometer: Vector,
      heading: number,
      deltaTicks: number
    ): void
    get quaternion(): Quaternion
    get linearAcceleration(): Vector
    get earthAcceleration(): Vector
    get states(): Record<string, any>
    get flags(): Record<string, any>
    set heading(heading: number)
  }
  export { AHRS as default }

  export class GyroOffset {
    constructor(sampleRate: number)
    update(gyroscope: Vector): Vector
  }
}
