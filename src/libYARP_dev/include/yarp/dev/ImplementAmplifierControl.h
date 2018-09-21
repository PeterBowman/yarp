/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_DEV_IMPLEMENTAMPLIFIERCONTROL_H
#define YARP_DEV_IMPLEMENTAMPLIFIERCONTROL_H

#include <yarp/dev/api.h>
#include <yarp/dev/IAmplifierControl.h>

namespace yarp
{
    namespace dev
    {
        class ImplementAmplifierControl;
        class StubImplAmplifierControlRaw;
    }
}

class YARP_dev_API yarp::dev::ImplementAmplifierControl : public IAmplifierControl
{
protected:
    IAmplifierControlRaw *iAmplifier;
    void *helper;
    double *dTemp;
    int *iTemp;

    /**
     * Initialize the internal data and alloc memory.
     * @param size is the number of controlled axes the driver deals with.
     * @param amap is a lookup table mapping axes onto physical drivers.
     * @param enc is an array containing the encoder to angles conversion factors.
     * @param zos is an array containing the zeros of the encoders.
     * @return true if initialized succeeded, false if it wasn't executed, or assert.
     */
    bool initialize (int size, const int *amap, const double *enc, const double *zos, const double *ampereFactor=NULL, const double *voltFactor=NULL);

    /**
     * Clean up internal data and memory.
     * @return true if uninitialization is executed, false otherwise.
     */
    bool uninitialize ();

public:
    /* Constructor.
     * @param y is the pointer to the class instance inheriting from this
     *  implementation.
     */
    ImplementAmplifierControl(yarp::dev::IAmplifierControlRaw  *y);

    /**
     * Destructor. Perform uninitialize if needed.
     */
    virtual ~ImplementAmplifierControl();

    /** Enable the amplifier on a specific joint. Be careful, check that the output
     * of the controller is appropriate (usually zero), to avoid
     * generating abrupt movements.
     * @return true/false on success/failure
     */
    virtual bool enableAmp(int j) override;

    /** Disable the amplifier on a specific joint. All computations within the board
     * will be carried out normally, but the output will be disabled.
     * @return true/false on success/failure
     */
    virtual bool disableAmp(int j) override;

    /* Get the status of the amplifiers, coded in a 32 bits integer for
     * each amplifier (at the moment contains only the fault, it will be
     * expanded in the future).
     * @param st pointer to storage
     * @return true in good luck, false otherwise.
     */
    virtual bool getAmpStatus(int *st) override;

    virtual bool getAmpStatus(int j, int *st) override;

    /* Read the electric current going to all motors.
     * @param vals pointer to storage for the output values
     * @return hopefully true, false in bad luck.
     */
    virtual bool getCurrents(double *vals) override;

    /* Read the electric current going to a given motor.
     * @param j motor number
     * @param val pointer to storage for the output value
     * @return probably true, might return false in bad times
     */
    virtual bool getCurrent(int j, double *val) override;

    /* Set the maximum electric current going to a given motor. The behavior
     * of the board/amplifier when this limit is reached depends on the
     * implementation.
     * @param j motor number
     * @param v the new value
     * @return probably true, might return false in bad times
     */
    virtual bool setMaxCurrent(int j, double v) override;

    /**
    * Returns the maximum electric current allowed for a given motor. The behavior
    * of the board/amplifier when this limit is reached depends on the
    * implementation.
    * @param j motor number
    * @param v the return value
    * @return probably true, might return false in bad times
    */
    virtual bool getMaxCurrent(int j, double *v) override;

    /* Get the the nominal current which can be kept for an indefinite amount of time
     * without harming the motor. This value is specific for each motor and it is typically
     * found in its datasheet. The units are Ampere.
     * This value and the peak current may be used by the firmware to configure
     * an I2T filter.
     * @param j joint number
     * @param val storage for return value. [Ampere]
     * @return true/false success failure.
     */
    virtual bool getNominalCurrent(int m, double *val) override;

    /* Set the the nominal current which can be kept for an indefinite amount of time
    * without harming the motor. This value is specific for each motor and it is typically
    * found in its datasheet. The units are Ampere.
    * This value and the peak current may be used by the firmware to configure
    * an I2T filter.
    * @param j joint number
    * @param val storage for return value. [Ampere]
    * @return true/false success failure.
    */
    virtual bool setNominalCurrent(int m, const double val) override;

    /* Get the the peak current which causes damage to the motor if maintained
     * for a long amount of time.
     * The value is often found in the motor datasheet, units are Ampere.
     * This value and the nominal current may be used by the firmware to configure
     * an I2T filter.
     * @param j joint number
     * @param val storage for return value. [Ampere]
     * @return true/false success failure.
     */
    virtual bool getPeakCurrent(int m, double *val) override;

    /* Set the the peak current. This value  which causes damage to the motor if maintained
     * for a long amount of time.
     * The value is often found in the motor datasheet, units are Ampere.
     * This value and the nominal current may be used by the firmware to configure
     * an I2T filter.
     * @param j joint number
     * @param val storage for return value. [Ampere]
     * @return true/false success failure.
     */
    virtual bool setPeakCurrent(int m, const double val) override;

    /* Get the the current PWM value used to control the motor.
     * The units are firmware dependent, either machine units or percentage.
     * @param j joint number
     * @param val filled with PWM value.
     * @return true/false success failure.
     */
    virtual bool getPWM(int j, double* val) override;

    /* Get the PWM limit for the given motor.
     * The units are firmware dependent, either machine units or percentage.
     * @param j joint number
     * @param val filled with PWM limit value.
     * @return true/false success failure.
     */
    virtual bool getPWMLimit(int j, double* val) override;

    /* Set the PWM limit for the given motor.
     * The units are firmware dependent, either machine units or percentage.
     * @param j joint number
     * @param val new value for the PWM limit.
     * @return true/false success failure.
     */
    virtual bool setPWMLimit(int j, const double val) override;

    /* Get the power source voltage for the given motor in Volt.
     * @param j joint number
     * @param val filled with return value.
     * @return true/false success failure.
     */
    virtual bool getPowerSupplyVoltage(int j, double* val) override;
};

/**
 * Stub implementation of IAmplifierControlRaw interface.
 * Inherit from this class if you want a stub implementation
 * of methods in IAmplifierControlRaw. This class allows to
 * gradually implement an interface; you just have to implement
 * functions that are useful for the underlying device.
 * Another way to see this class is as a means to convert
 * compile time errors in runtime errors.
 *
 * If you use this class please be aware that the device
 * you are wrapping might not function properly because you
 * missed to implement useful functionalities.
 *
 */
class YARP_dev_API yarp::dev::StubImplAmplifierControlRaw : public IAmplifierControlRaw
{
private:
    /**
     * Helper for printing error message, see below.
     * Implemented in ImplementAmplifierControl.cpp.
     */
    bool NOT_YET_IMPLEMENTED(const char *func=0);

public:
    virtual ~StubImplAmplifierControlRaw() {}

    virtual bool enableAmpRaw(int j) override
    { return NOT_YET_IMPLEMENTED("enableAmpRaw"); }

    virtual bool disableAmpRaw(int j) override
    { return NOT_YET_IMPLEMENTED("disableAmpRaw"); }

    virtual bool getAmpStatusRaw(int *st) override
    { return NOT_YET_IMPLEMENTED("getAmpStatusRaw"); }

    virtual bool getAmpStatusRaw(int j, int *st) override
    { return NOT_YET_IMPLEMENTED("getAmpStatusRaw"); }

    virtual bool getCurrentsRaw(double *vals) override
    { return NOT_YET_IMPLEMENTED("getCurrentsRaw"); }

    virtual bool getCurrentRaw(int j, double *val) override
    { return NOT_YET_IMPLEMENTED("getCurrentRaw"); }

    virtual bool setMaxCurrentRaw(int j, double v) override
    { return NOT_YET_IMPLEMENTED("setMaxCurrentRaw"); }

    virtual bool getMaxCurrentRaw(int j, double *v) override
    { return NOT_YET_IMPLEMENTED("getMaxCurrentRaw"); }

    virtual bool getNominalCurrentRaw(int m, double *val) override
    { return NOT_YET_IMPLEMENTED("getNominalCurrentRaw"); }

    virtual bool setNominalCurrentRaw(int m, const double val) override
    { return NOT_YET_IMPLEMENTED("setNominalCurrentRaw"); }

    virtual bool getPeakCurrentRaw(int m, double *val) override
    { return NOT_YET_IMPLEMENTED("getPeakCurrentRaw"); }

    virtual bool setPeakCurrentRaw(int m, const double val) override
    { return NOT_YET_IMPLEMENTED("setPeakCurrentRaw"); }

    virtual bool getPWMRaw(int j, double* val) override
    { return NOT_YET_IMPLEMENTED("getPWMRaw"); }

    virtual bool getPWMLimitRaw(int j, double* val) override
    { return NOT_YET_IMPLEMENTED("getPWMLimitRaw"); }

    virtual bool setPWMLimitRaw(int j, const double val) override
    { return NOT_YET_IMPLEMENTED("setPWMLimitRaw"); }

    virtual bool getPowerSupplyVoltageRaw(int j, double* val) override
    { return NOT_YET_IMPLEMENTED("getPowerSupplyVoltageRaw"); }
};

#endif // YARP_DEV_IMPLEMENTAMPLIFIERCONTROL_H
