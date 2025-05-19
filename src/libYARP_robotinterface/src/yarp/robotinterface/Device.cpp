/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/robotinterface/Device.h>

#include <yarp/robotinterface/Action.h>
#include <yarp/robotinterface/Param.h>
#include <yarp/robotinterface/impl/CalibratorThread.h>

#include <yarp/os/LogStream.h>
#include <yarp/os/Property.h>
#include <yarp/os/Semaphore.h>

#include <yarp/dev/CalibratorInterfaces.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/IMultipleWrapper.h>
#include <yarp/dev/IWrapper.h>
#include <yarp/dev/PolyDriver.h>

#include <string>

namespace {
YARP_LOG_COMPONENT(YRI_DEVICE, "yarp.yri.Device")
}

class yarp::robotinterface::Device::Private
{

public:
    struct Driver
    {
        Driver(yarp::dev::PolyDriver* d) :
                driver(d),
                ref(1)
        {
        }
        yarp::dev::PolyDriver* driver;
        ThreadList runningThreads;
        yarp::os::Semaphore registerThreadSemaphore;
        yarp::os::Semaphore threadListSemaphore;
        int ref;
    };

    Private(Device* /*parent*/) :
            driver(new Driver(new yarp::dev::PolyDriver()))
    {
    }

    Private(const Private& other) :
            driver(other.driver)
    {
        ++driver->ref;
    }

    Private& operator=(const Private& other)
    {
        YARP_UNUSED(other);
        YARP_FIXME_NOTIMPLEMENTED("operator=")
        return *this;
    }

    ~Private()
    {
        if (!--driver->ref) {
            stopThreads();

            if (driver->driver->isValid()) {
                if (!driver->driver->close()) {
                    yCWarning(YRI_DEVICE) << "Cannot close device" << name;
                }
            }

            delete driver->driver;
            delete driver;
            driver = nullptr;
        }
    }

    inline yarp::dev::PolyDriver* drv() const
    {
        return driver->driver;
    }
    inline yarp::robotinterface::ThreadList* thr() const
    {
        return &driver->runningThreads;
    }
    inline yarp::os::Semaphore* reg_sem() const
    {
        return &driver->registerThreadSemaphore;
    }
    inline yarp::os::Semaphore* lst_sem() const
    {
        return &driver->threadListSemaphore;
    }

    inline bool isValid() const
    {
        return drv()->isValid();
    }
    inline bool open()
    {
        yarp::os::Property p = paramsAsProperty();
        return drv()->open(p);
    }
    inline bool close()
    {
        return drv()->close();
    }

    inline void registerThread(yarp::os::Thread* thread) const
    {
        reg_sem()->wait();
        lst_sem()->wait();
        thr()->push_back(thread);
        reg_sem()->post();
        lst_sem()->post();
    }

    inline void joinThreads() const
    {
        // The semafore will not allow other thread to be registered while
        // joining threads.
        // Other calls to joinThread() will stop at the semaphore and will
        // be restarted as soon as all the threads are joined and the list
        // is empty. This will avoid calling delete twice.
        // stopThreads() must pass this semaphore, but in order to avoid to
        // stop an already deleted thread we need a second semaphore.
        reg_sem()->wait();
        auto tit = thr()->begin();
        while (tit != thr()->end()) {
            yarp::os::Thread* thread = *tit;
            thread->join();
            lst_sem()->wait();
            thr()->erase(tit++);
            delete thread;
            lst_sem()->post();
        }
        reg_sem()->post();
    }

    inline void stopThreads() const
    {
        lst_sem()->wait();
        for (auto* thread : *thr()) {
            thread->stop();
        }
        lst_sem()->post();
    }

    yarp::os::Property paramsAsProperty() const
    {
        ParamList p = yarp::robotinterface::mergeDuplicateGroups(params);

        yarp::os::Property prop;
        prop.put("device", type);
        prop.put("id", name);

        for (yarp::robotinterface::ParamList::const_iterator it = p.begin(); it != p.end(); ++it) {
            const yarp::robotinterface::Param& param = *it;

            // check if parentheses are balanced
            std::string stringFormatValue = param.value();
            int counter = 0;
            for (size_t i = 0; i < stringFormatValue.size() && counter >= 0; i++) {
                if (stringFormatValue[i] == '(') {
                    counter++;
                } else if (stringFormatValue[i] == ')') {
                    counter--;
                }
            }
            if (counter != 0) {
                yCWarning(YRI_DEVICE) << "Parentheses not balanced for param " << param.name();
            }

            std::string s = "(" + param.name() + " " + param.value() + ")";
            prop.fromString(s, false);
        }

        return prop;
    }

    std::string name;
    std::string type;
    ParamList params;
    ActionList actions;
    Driver* driver;
};

yarp::os::LogStream operator<<(yarp::os::LogStream dbg, const yarp::robotinterface::Device& t)
{
    dbg << "(name = \"" << t.name() << "\", type = \"" << t.type() << "\"";
    if (!t.params().empty()) {
        dbg << ", params = [";
        dbg << t.params();
        dbg << "]";
    }
    if (!t.actions().empty()) {
        dbg << ", actions = [";
        dbg << t.actions();
        dbg << "]";
    }
    dbg << ")";
    return dbg;
}

yarp::robotinterface::Device::Device() :
        mPriv(new Private(this))
{
    mPriv->close();
}

yarp::robotinterface::Device::Device(const std::string& name,
                                                   const std::string& type,
                                                   const yarp::robotinterface::ParamList& params,
                                                   const yarp::robotinterface::ActionList& actions) :
        mPriv(new Private(this))
{
    mPriv->name = name;
    mPriv->type = type;
    mPriv->params = params;
    mPriv->actions = actions;
}

yarp::robotinterface::Device::Device(const yarp::robotinterface::Device& other) :
        mPriv(new Private(*other.mPriv))
{
    mPriv->name = other.mPriv->name;
    mPriv->type = other.mPriv->type;
    mPriv->params = other.mPriv->params;
    mPriv->actions = other.mPriv->actions;
    *mPriv->driver = *other.mPriv->driver;
}

yarp::robotinterface::Device& yarp::robotinterface::Device::operator=(const yarp::robotinterface::Device& other)
{
    if (&other != this) {
        mPriv->name = other.mPriv->name;
        mPriv->type = other.mPriv->type;

        mPriv->params.clear();
        mPriv->params = other.mPriv->params;

        mPriv->actions.clear();
        mPriv->actions = other.mPriv->actions;

        *mPriv->driver = *other.mPriv->driver;
    }
    return *this;
}

yarp::robotinterface::Device::~Device()
{
    delete mPriv;
}

std::string& yarp::robotinterface::Device::name()
{
    return mPriv->name;
}

std::string& yarp::robotinterface::Device::type()
{
    return mPriv->type;
}

yarp::robotinterface::ParamList& yarp::robotinterface::Device::params()
{
    return mPriv->params;
}

yarp::robotinterface::ActionList& yarp::robotinterface::Device::actions()
{
    return mPriv->actions;
}

const std::string& yarp::robotinterface::Device::name() const
{
    return mPriv->name;
}

const std::string& yarp::robotinterface::Device::type() const
{
    return mPriv->type;
}

const yarp::robotinterface::ParamList& yarp::robotinterface::Device::params() const
{
    return mPriv->params;
}

const yarp::robotinterface::ActionList& yarp::robotinterface::Device::actions() const
{
    return mPriv->actions;
}

bool yarp::robotinterface::Device::open()
{
    if (mPriv->isValid()) {
        yCError(YRI_DEVICE) << "Trying to open an already opened device.";
        return false;
    }

    if (!mPriv->open()) {
        yCWarning(YRI_DEVICE) << "Cannot open device" << mPriv->name;
        return false;
    }

    return true;
}

bool yarp::robotinterface::Device::close()
{
    if (!mPriv->isValid()) {
        // Trying to close an already closed device. Perhaps open()
        // Failed... Nothing to do and not worth sending an error.
        return true;
    }

    if (!mPriv->close()) {
        yCWarning(YRI_DEVICE) << "Cannot close device" << mPriv->name;
        return false;
    }

    return true;
}

bool yarp::robotinterface::Device::hasParam(const std::string& name) const
{
    return yarp::robotinterface::hasParam(mPriv->params, name);
}

std::string yarp::robotinterface::Device::findParam(const std::string& name) const
{
    return yarp::robotinterface::findParam(mPriv->params, name);
}

yarp::dev::PolyDriver* yarp::robotinterface::Device::driver() const
{
    return mPriv->drv();
}

void yarp::robotinterface::Device::registerThread(yarp::os::Thread* thread) const
{
    mPriv->registerThread(thread);
}

void yarp::robotinterface::Device::joinThreads() const
{
    mPriv->joinThreads();
}

void yarp::robotinterface::Device::stopThreads() const
{
    mPriv->stopThreads();
}

bool yarp::robotinterface::Device::calibrate(const yarp::dev::PolyDriverDescriptor& target) const
{
    if (!driver()) {
        yCDebug(YRI_DEVICE) << "Device do not exists, cannot do " << ActionTypeToString(ActionTypeDetach) << "action";
        return false;
    }

    yarp::dev::ICalibrator* calibrator;
    if (!driver()->view(calibrator)) {
        yCError(YRI_DEVICE) << name() << "is not a yarp::dev::ICalibrator, therefore it cannot have" << ActionTypeToString(ActionTypeCalibrate) << "actions";
        return false;
    }

    yarp::dev::IControlCalibration* controlCalibrator;
    if (!target.poly->view(controlCalibrator)) {
        yCError(YRI_DEVICE) << target.key << "is not a yarp::dev::IControlCalibration2, therefore it cannot have" << ActionTypeToString(ActionTypeCalibrate) << "actions";
        return false;
    }

    controlCalibrator->setCalibrator(calibrator);

    // Saving pointer to Calibrator device into Wrapper to later use
    // (NOTE this make sense if the target device is a controlBoard_nws_yarp, as it should be)
    yarp::dev::IRemoteCalibrator* rem_calibrator_wrap;
    yarp::dev::IRemoteCalibrator* rem_calibrator_calib;
    bool rem_calibrator_available = true;

    if (!target.poly->view(rem_calibrator_wrap)) {
        yCWarning(YRI_DEVICE) << "Device " << target.key << "is not implementing a yarp::dev::IRemoteCalibrator, therefore it cannot attach to a Calibrator device. \n \
                                                     Please verify that the target of calibrate action is a controlBoard_nws_yarp device.  \
                                                     This doesn't prevent the yarprobotinterface to correctly operate, but no remote calibration and homing will be available";
        rem_calibrator_available = false;
    }

    if ((rem_calibrator_available) && (!driver()->view(rem_calibrator_calib))) {
        yCWarning(YRI_DEVICE) << "Device " << name() << "is not implementing a yarp::dev::IRemoteCalibrator, therefore it cannot be used as a remote calibration device. \n \
                                               This doesn't prevent the yarprobotinterface to correctly operate, but no remote calibration and homing will be available";
        rem_calibrator_available = false;
    }

    if (rem_calibrator_available) {
        rem_calibrator_wrap->setCalibratorDevice(rem_calibrator_calib);
    }

    // Start the calibrator thread
    yarp::os::Thread* calibratorThread = new yarp::robotinterface::impl::CalibratorThread(calibrator,
                                                                                          name(),
                                                                                          target.poly,
                                                                                          target.key,
                                                                                          yarp::robotinterface::impl::CalibratorThread::ActionCalibrate);
    registerThread(calibratorThread);

    if (!calibratorThread->start()) {
        yCError(YRI_DEVICE) << "Device" << name() << "cannot execute" << ActionTypeToString(ActionTypeCalibrate) << "on device" << target.key;
        return false;
    }

    return true;
}

bool yarp::robotinterface::Device::attach(const yarp::dev::PolyDriverList& drivers) const
{
    if (!driver()) {
        yCDebug(YRI_DEVICE) << "Device do not exists, cannot do " << ActionTypeToString(ActionTypeDetach) << "action";
        return false;
    }

    yarp::dev::IMultipleWrapper* multiplewrapper;
    driver()->view(multiplewrapper);

    if (drivers.size() == 1) {
        yarp::dev::IWrapper* wrapper;
        if (!driver()->view(wrapper)) {
            yCInfo(YRI_DEVICE) << name() << "is not an IWrapper. Trying IMultipleWrapper";
        } else if (wrapper->attach(drivers[0]->poly)) {
            return true;
        } else if (!multiplewrapper) {
            yCError(YRI_DEVICE) << "Device" << name() << "cannot execute" << ActionTypeToString(ActionTypeAttach);
            return false;
        } else {
            yCInfo(YRI_DEVICE) << name() << "IWrapper::attach() failed. Trying IMultipleWrapper::attach().";
        }
    }

    if (!multiplewrapper) {
        yCError(YRI_DEVICE) << name() << "is not a multiplewrapper, therefore it cannot have" << ActionTypeToString(ActionTypeAttach) << "actions";
        return false;
    }

    if (!multiplewrapper->attachAll(drivers)) {
        yCError(YRI_DEVICE) << "Device" << name() << "cannot execute" << ActionTypeToString(ActionTypeAttach);
        return false;
    }

    return true;
}

bool yarp::robotinterface::Device::detach() const
{
    if (!driver()) {
        yCDebug(YRI_DEVICE) << "Device do not exists, cannot do " << ActionTypeToString(ActionTypeDetach) << "action";
        return false;
    }

    yarp::dev::IWrapper* wrapper;
    yarp::dev::IMultipleWrapper* multiplewrapper;
    driver()->view(wrapper);
    driver()->view(multiplewrapper);

    if (!wrapper && !multiplewrapper) {
        yCError(YRI_DEVICE) << name() << "is neither a wrapper nor a multiplewrapper, therefore it cannot have" << ActionTypeToString(ActionTypeDetach) << "actions";
        return false;
    }

    if (multiplewrapper) {
        if (multiplewrapper->detachAll()) {
            return true;
        }
        if (!wrapper) {
            yCError(YRI_DEVICE) << "Device" << name() << "cannot execute" << ActionTypeToString(ActionTypeDetach);
            return false;
        }
        yCInfo(YRI_DEVICE) << name() << "IMultipleWrapper::detachAll() failed. Trying IWrapper::detach().";
    }

    if (wrapper && !wrapper->detach()) {
        yCError(YRI_DEVICE) << "Device" << name() << "cannot execute" << ActionTypeToString(ActionTypeDetach);
        return false;
    }

    return true;
}

bool yarp::robotinterface::Device::park(const yarp::dev::PolyDriverDescriptor& target) const
{
    yarp::dev::ICalibrator* calibrator;
    if (!driver()) {
        yCDebug(YRI_DEVICE) << "Device do not exists, cannot do " << ActionTypeToString(ActionTypeDetach) << "action";
        return false;
    }

    if (!driver()->isValid()) {
        yCError(YRI_DEVICE) << "park device do not exists";
        return false;
    }

    if (!driver()->view(calibrator)) {
        yCError(YRI_DEVICE) << name() << "is not a yarp::dev::ICalibrator, therefore it cannot have" << ActionTypeToString(ActionTypePark) << "actions";
        return false;
    }

    yarp::dev::IControlCalibration* controlCalibrator;
    if (!target.poly->view(controlCalibrator)) {
        yCError(YRI_DEVICE) << target.key << "is not a yarp::dev::IControlCalibration2, therefore it cannot have" << ActionTypeToString(ActionTypePark) << "actions";
        return false;
    }

    controlCalibrator->setCalibrator(calibrator); // TODO Check if this should be removed

    yarp::os::Thread* parkerThread = new yarp::robotinterface::impl::CalibratorThread(calibrator,
                                                                                      name(),
                                                                                      target.poly,
                                                                                      target.key,
                                                                                      yarp::robotinterface::impl::CalibratorThread::ActionPark);
    registerThread(parkerThread);

    if (!parkerThread->start()) {
        yCError(YRI_DEVICE) << "Device" << name() << "cannot execute" << ActionTypeToString(ActionTypePark) << "on device" << target.key;
        return false;
    }

    return true;
}
