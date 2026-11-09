// Diagnostic client for the matching ishtar .307 read-only CWB refresh command.
#include <hidl/HidlBinderSupport.h>
#include <hidl/ServiceManagement.h>
#include <hidl/Status.h>
#include <hwbinder/IBinder.h>
#include <hwbinder/Parcel.h>
#include <array>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <thread>

int main(int argc, char** argv) {
    using namespace android;
    using namespace android::hardware;
    const int count = argc > 1 ? std::atoi(argv[1]) : 1;
    if (count < 1 || count > 400) return 2;
    constexpr char descriptor[] = "vendor.xiaomi.sensor.citsensorservice@2.0::ICitSensorService";
    auto service = details::getRawServiceInternal(descriptor, "default", false, false);
    if (service == nullptr || !service->isRemote()) return 3;
    auto binder = getOrCreateCachedBinder(service.get());
    if (binder == nullptr) return 4;
    std::array<unsigned char, 264> msg{};
    const float command = -9.0f;
    std::memcpy(msg.data() + 12, &command, sizeof(command));
    for (int i = 0; i < count; i++) {
        Parcel data, reply;
        size_t handle = 0;
        status_t rc = data.writeInterfaceToken(descriptor);
        if (rc == OK) rc = data.writeBuffer(msg.data(), msg.size(), &handle);
        bool callback = false;
        status_t response = UNKNOWN_ERROR;
        if (rc == OK) {
            rc = binder->transact(4, data, &reply, 0, [&](Parcel& p) {
                callback = true;
                Status status;
                response = readFromParcel(&status, p);
                if (response == OK && !status.isOk()) response = UNKNOWN_ERROR;
                if (response == OK) {
                    const void* returned = nullptr;
                    size_t outHandle;
                    response = p.readBuffer(msg.size(), &outHandle, &returned);
                    if (response == OK && std::memcmp(returned, msg.data(), msg.size()) != 0)
                        response = BAD_VALUE;
                }
            });
        }
        std::printf("{\"sample\":%d,\"transport\":%d,\"callback\":%s,\"echo_status\":%d}\n",
                    i, rc, callback ? "true" : "false", response);
        std::fflush(stdout);
        if (rc != OK || !callback || response != OK) return 5;
        if (i + 1 < count) std::this_thread::sleep_for(std::chrono::milliseconds(270));
    }
    return 0;
}
