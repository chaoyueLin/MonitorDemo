#include <jni.h>
#include <string>

#include "dlopen.h"
#include <android/log.h>

#define LOG_TAG "DeadLockMonitor_native"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

extern "C" JNIEXPORT jstring JNICALL
Java_com_example_monitordemo_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

jint api_level;

void *get_contended_monitor;
void *get_lock_owner_thread;


const char *get_lock_owner_symbol_name(jint level);

extern "C"
JNIEXPORT jint JNICALL
Java_com_example_monitordemo_DeadLockMonitor_nativeInit(JNIEnv *env, jobject thiz,
                                                        jint sdk_version) {
    api_level = sdk_version;
    //dlopen libart.so
    //1.初始化
    ndk_init(env);
    //2.加载libart.so

    void *so_addr = ndk_dlopen("libart.so", RTLD_NOLOAD);
    if (so_addr == NULL) {
        return 1;
    }

    //获取Monitor::GetContendMonitor 函数符号地址
    get_contended_monitor = ndk_dlsym(so_addr, "_ZN3art7Monitor19GetContendedMonitorEPNS_6ThreadE");
    if (get_contended_monitor == NULL) {
        return 2;
    }

    //获取Monitor的持有者的线程
    get_lock_owner_thread = ndk_dlsym(so_addr, get_lock_owner_symbol_name(api_level));
    if (get_lock_owner_thread == NULL) {
        return 3;
    }
    return 0;
}

const char *get_lock_owner_symbol_name(jint level) {
    if (level < 29) {
        //android 9.0 之前
        //http://androidxref.com/9.0.0_r3/xref/system/core/libbacktrace/testdata/arm/libart.so 搜索 GetLockOwnerThreadId
        return "_ZN3art7Monitor20GetLockOwnerThreadIdEPNS_6mirror6ObjectE";
    } else if (level <= 30) {
        //android 10.0
        return "_ZN3art7Monitor20GetLockOwnerThreadIdENS_6ObjPtrINS_6mirror6ObjectEEE";
    }
}


extern "C"
JNIEXPORT jint JNICALL
Java_com_example_monitordemo_DeadLockMonitor_getContentThreadIdArt(JNIEnv *env, jobject thiz,
                                                                   jlong thread_address) {

    LOGI("getContentThreadIdArt");
    int monitor_thread_id = 0;
    if (get_contended_monitor != NULL && get_lock_owner_thread != NULL) {
        int monitorObj = ((int (*)(long)) get_contended_monitor)(thread_address);
        if (monitorObj != 0) {
            LOGI("monitorObj !=0");
            monitor_thread_id = ((int (*)(int)) get_lock_owner_thread)(monitorObj);
        } else {
            monitor_thread_id = 0;
            LOGE("GetContendedMonitor return null");
        }
    } else {
        LOGE("get_contended_monitor == NULL || get_lock_owner_thread == NULL");
    }
    return monitor_thread_id;

}

extern "C"
JNIEXPORT jint JNICALL
Java_com_example_monitordemo_DeadLockMonitor_getThreadIdFromThreadPtr(JNIEnv *env, jobject thiz,
                                                                      jlong thread_address) {

    LOGI("suspendThreadGetThreadIdFromThreadPtrArt nativeThread");

    if (thread_address != 0) {
        LOGI("nativeThread !=0");
        if (api_level > 20) {
            int *pInt = reinterpret_cast<int *>(thread_address);
            pInt = pInt + 3;
            return *pInt;
        }
    } else {
        LOGE("suspendThreadArt failed");
    }

}