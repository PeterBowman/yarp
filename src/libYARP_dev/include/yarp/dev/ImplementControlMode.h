/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_DEV_IMPLEMENTCONTROLMODE_H
#define YARP_DEV_IMPLEMENTCONTROLMODE_H

#include <yarp/dev/IControlMode.h>
#include <yarp/dev/api.h>

namespace yarp {
    namespace dev {
        class ImplementControlMode;
        class StubImplControlModeRaw;
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

class YARP_dev_API yarp::dev::ImplementControlMode: public IControlMode
{
    void *helper;
    yarp::dev::IControlModeRaw *raw;
    yarp::dev::impl::FixedSizeBuffersManager<int> *buffManager;

public:
    bool initialize(int k, const int *amap);
    bool uninitialize();
    ImplementControlMode(IControlModeRaw *v);
    ~ImplementControlMode();
    bool getControlMode(int j, int *f) override;
    bool getControlModes(int *modes) override;
    bool getControlModes(const int n_joint, const int *joints, int *modes) override;
    bool setControlMode(const int j, const int mode) override;
    bool setControlModes(const int n_joint, const int *joints, int *modes) override;
    bool setControlModes(int *modes) override;
};

/**
 * Stub implementation of IControlModeRaw interface.
 * Inherit from this class if you want a stub implementation
 * of methods in IControlModeRaw. This class allows to
 * gradually implement an interface; you just have to implement
 * functions that are useful for the underlying device.
 * Another way to see this class is as a means to convert
 * compile time errors in runtime errors.
 *
 * If you use this class please be aware that the device
 * you are wrapping might not function properly because you
 * missed to implement useful functionalities.
 */
class YARP_dev_API yarp::dev::StubImplControlModeRaw : public IControlModeRaw
{
private:
    /**
     * Helper for printing error message, see below.
     * Implemented in ImplementControlMode.cpp.
     */
    bool NOT_YET_IMPLEMENTED(const char *func=0);

public:
    virtual ~StubImplControlModeRaw() {}

    virtual bool getControlModeRaw(int j, int *mode) override
    { return NOT_YET_IMPLEMENTED("getControlModeRaw"); }

    virtual bool getControlModesRaw(int* modes) override
    { return NOT_YET_IMPLEMENTED("getControlModesRaw"); }

    virtual bool getControlModesRaw(const int n_joint, const int *joints, int *modes) override
    { return NOT_YET_IMPLEMENTED("getControlModesRaw"); }

    virtual bool setControlModeRaw(const int j, const int mode) override
    { return NOT_YET_IMPLEMENTED("setControlModeRaw"); }

    virtual bool setControlModesRaw(const int n_joint, const int *joints, int *modes) override
    { return NOT_YET_IMPLEMENTED("setControlModesRaw"); }

    virtual bool setControlModesRaw(int *modes) override
    { return NOT_YET_IMPLEMENTED("setControlModesRaw"); }
};

#endif // YARP_DEV_IMPLEMENTCONTROLMODE_H
