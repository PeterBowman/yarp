/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_DEV_IMPLEMENTAXISINFO_H
#define YARP_DEV_IMPLEMENTAXISINFO_H

#include <yarp/dev/api.h>
#include <yarp/dev/IAxisInfo.h>

namespace yarp {
    namespace dev {
        class ImplementAxisInfo;
        class StubImplAxisInfoRaw;
    }
}

class YARP_dev_API yarp::dev::ImplementAxisInfo : public IAxisInfo
{
protected:
    IAxisInfoRaw *iinfo;
    void *helper;
    double *temp1;
    double *temp2;


    /**
     * Initialize the internal data and alloc memory.
     * @param size is the number of controlled axes the driver deals with.
     * @param amap is a lookup table mapping axes onto physical drivers.
     * @return true if initialized succeeded, false if it wasn't executed, or assert.
     */
    bool initialize (int size, const int *amap);

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
    ImplementAxisInfo(yarp::dev::IAxisInfoRaw *y);

    virtual ~ImplementAxisInfo();

    virtual bool getAxisName(int axis, std::string& name) override;
    virtual bool getJointType(int axis, yarp::dev::JointTypeEnum& type) override;
};

/**
 * Stub implementation of IAxisInfoRaw interface.
 * Inherit from this class if you want a stub implementation
 * of methods in IAxisInfoRaw. This class allows to
 * gradually implement an interface; you just have to implement
 * functions that are useful for the underlying device.
 * Another way to see this class is as a means to convert
 * compile time errors in runtime errors.
 *
 * If you use this class please be aware that the device
 * you are wrapping might not function properly because you
 * missed to implement useful functionalities.
 */
class YARP_dev_API yarp::dev::StubImplAxisInfoRaw : public IAxisInfoRaw
{
private:
    /**
     * Helper for printing error message, see below.
     * Implemented in ImplementAxisInfo.cpp.
     */
    bool NOT_YET_IMPLEMENTED(const char *func=0);

public:
    virtual ~StubImplAxisInfoRaw() {}

    /* Get the name for a particular axis.
    * @param axis joint number
    * @param name the axis name
    * @return true if everything goes fine, false otherwise.
    */
    virtual bool getAxisNameRaw(int axis, std::string& name) override
    { return NOT_YET_IMPLEMENTED("getAxisNameRaw"); }

    /* Get the joint type (e.g. revolute/prismatic) for a particular axis.
    * @param axis joint number
    * @param type the joint type
    * @return true if everything goes fine, false otherwise.
    */
    virtual bool getJointTypeRaw(int axis, yarp::dev::JointTypeEnum& type) override
    { return NOT_YET_IMPLEMENTED("getJointTypeRaw"); }
};

#endif // YARP_DEV_IMPLEMENTAXISINFO_H
