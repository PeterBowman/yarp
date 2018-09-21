/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_DEV_IMPLEMENTCURRENTCONTROL_H
#define YARP_DEV_IMPLEMENTCURRENTCONTROL_H

#include <yarp/dev/ICurrentControl.h>
#include <yarp/dev/api.h>

namespace yarp {
    namespace dev {
        class ImplementCurrentControl;
        class StubImplCurrentControlRaw;
    }
}

namespace yarp {
namespace dev {
namespace impl {

template <typename T>
class FixedSizeBuffersManager;

} // namespace impl
} // namespace dev
} // namespace yarp

class YARP_dev_API yarp::dev::ImplementCurrentControl: public ICurrentControl
{
protected:
    yarp::dev::ICurrentControlRaw *iCurrentRaw;
    void *helper;
    yarp::dev::impl::FixedSizeBuffersManager<int> *intBuffManager;
    yarp::dev::impl::FixedSizeBuffersManager<double> *doubleBuffManager;

    /**
     * Initialize the internal data and alloc memory.
     * @param size is the number of controlled axes the driver deals with.
     * @param amap is a lookup table mapping axes onto physical drivers.
     * @return true if initialized succeeded, false if it wasn't executed, or assert.
     */
    bool initialize(int size, const int *amap, const double* ampsToSens);

    /**
     * Clean up internal data and memory.
     * @return true if uninitialization is executed, false otherwise.
     */
    bool uninitialize();

public:
    /* Constructor.
     * @param y is the pointer to the class instance inheriting from this
     *  implementation.
     */
    ImplementCurrentControl(yarp::dev::ICurrentControlRaw *y);

    /**
     * Destructor. Perform uninitialize if needed.
     */
    virtual ~ImplementCurrentControl();

    virtual bool getNumberOfMotors(int *ax) override;
    virtual bool getRefCurrent(int j, double *) override;
    virtual bool getRefCurrents(double *t) override;
    virtual bool setRefCurrents(const double *t) override;
    virtual bool setRefCurrent(int j, double t) override;
    virtual bool setRefCurrents(const int n_joint, const int *joints, const double *t) override;
    virtual bool getCurrents(double *t) override;
    virtual bool getCurrent(int j, double *t) override;
    virtual bool getCurrentRange(int j, double *min, double *max) override;
    virtual bool getCurrentRanges(double *min, double *max) override;
};

/**
 * Stub implementation of ICurrentControlRaw interface.
 * Inherit from this class if you want a stub implementation
 * of methods in ICurrentControlRaw. This class allows to
 * gradually implement an interface; you just have to implement
 * functions that are useful for the underlying device.
 * Another way to see this class is as a means to convert
 * compile time errors in runtime errors.
 *
 * If you use this class please be aware that the device
 * you are wrapping might not function properly because you
 * missed to implement useful functionalities.
 */
class YARP_dev_API yarp::dev::StubImplCurrentControlRaw : public ICurrentControlRaw
{
    /**
     * Helper for printing error message, see below.
     * Implemented in ImplementCurrentControl.cpp.
     */
    bool NOT_YET_IMPLEMENTED(const char *func=0);

public:
    virtual ~StubImplCurrentControlRaw() {}

    /**
    * Retrieves the number of controlled motors from the current physical interface.
    * @param number returns the number of controlled motors.
    * @return true/false on success/failure
    */
    virtual bool getNumberOfMotorsRaw(int *number) override
    { return NOT_YET_IMPLEMENTED("getNumberOfMotorsRaw"); }

    /** Get the instantaneous current measurement for a single motor.
    * @param m motor number
    * @param curr pointer to the result value
    * @return true/false on success/failure
    */
    virtual bool getCurrentRaw(int m, double *curr) override
    { return NOT_YET_IMPLEMENTED("getCurrentRaw"); }

    /** Get the instantaneous current measurement for all motors.
    * @param currs pointer to the array that will store the output
    * @return true/false on success/failure
    */
    virtual bool getCurrentsRaw(double *currs) override
    { return NOT_YET_IMPLEMENTED("getCurrentsRaw"); }

    /** Get the full scale of the current measurement for a given motor (e.g. -20A +20A)
    * Reference values set by user with methods such as setRefCurrent() should be in this range.
    * This method is not related to the current overload protection methods belonging to the iAmplifierControl interface.
    * @param m motor number
    * @param min minimum current of the motor m
    * @param max maximum current of the motor m
    * @return true/false on success/failure
    */
    virtual bool getCurrentRangeRaw(int m, double *min, double *max) override
    { return NOT_YET_IMPLEMENTED("getCurrentRangeRaw"); }

    /** Get the full scale of the current measurements for all motors motor (e.g. -20A +20A)
    * Reference values set by user with methods such as setRefCurrent() should be in this range.
    * This method is not related to the current overload protection methods belonging to the iAmplifierControl interface.
    * @param min pointer to the array that will store minimum currents
    * @param max pointer to the array that will store maximum currents
    * @return true/false on success/failure
    */
    virtual bool getCurrentRangesRaw(double *min, double *max) override
    { return NOT_YET_IMPLEMENTED("getCurrentRangesRaw"); }

    /** Set the reference value of the currents for all motors.
    * @param currs the array containt the reference current values
    * @return true/false on success/failure
    */
    virtual bool setRefCurrentsRaw(const double *currs) override
    { return NOT_YET_IMPLEMENTED("setRefCurrentsRaw"); }

    /** Set the reference value of the current for a single motor.
    * @param m motor number
    * @param curr the current reference value for motor m
    * @return true/false on success/failure
    */
    virtual bool setRefCurrentRaw(int m, double curr) override
    { return NOT_YET_IMPLEMENTED("setRefCurrentRaw"); }

    /**  Set the reference value of the current for a group of motors.
     * @param n_motor size of motors ans currs arrays
     * @param motors pointer to the array of motor numbers
     * @param refs   pointer to the array specifying the new current reference
     * @return true/false on success/failure
     */
    virtual bool setRefCurrentsRaw(const int n_motor, const int *motors, const double *currs) override
    { return NOT_YET_IMPLEMENTED("setRefCurrentsRaw"); }

    /** Get the reference value of the currents for all motors.
    * @param currs pointer to the array to be filled with reference current values.
    * @return true/false on success/failure
    */
    virtual bool getRefCurrentsRaw(double *currs) override
    { return NOT_YET_IMPLEMENTED("getRefCurrentsRaw"); }

    /** Get the reference value of the current for a single motor.
    * @param m motor number
    * @param curr the current reference value for motor m
    * @return true/false on success/failure
    */
    virtual bool getRefCurrentRaw(int m, double *curr) override
    { return NOT_YET_IMPLEMENTED("getRefCurrentRaw"); }
};

#endif // YARP_DEV_IMPLEMENTCURRENTCONTROL_H
