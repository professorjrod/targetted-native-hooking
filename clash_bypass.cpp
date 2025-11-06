#include <jni.h>
#include <dlfcn.h>
#include <cstring>
#include <string>
#include <android/log.h>
#include "native_hook.h"

#define LOG_TAG "ClashBypass"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

static HookFunType hook_func = nullptr;

// Backup pointer for original __system_property_get
static int (*backup_system_property_get)(const char *name, char *value) = nullptr;

// Fake property values
struct PropertyMapping {
    const char *key;
    const char *value;
};

static const PropertyMapping fake_props[] = {
    // Critical boot properties
    {"ro.boot.qemu", "0"},
    {"ro.boot.hardware", "exynos2100"},
    {"ro.boot.serialno", "R5CR8348495"},
    
    // Hide ALL ro.boot.qemu.* properties
    {"ro.boot.qemu.avd_name", ""},
    {"ro.boot.qemu.camera_hq_edge_processing", ""},
    {"ro.boot.qemu.camera_protocol_ver", ""},
    {"ro.boot.qemu.cpuvulkan.version", ""},
    {"ro.boot.qemu.gltransport.drawFlushInterval", ""},
    {"ro.boot.qemu.gltransport.name", ""},
    {"ro.boot.qemu.hwcodec.avcdec", ""},
    {"ro.boot.qemu.hwcodec.hevcdec", ""},
    {"ro.boot.qemu.hwcodec.vpxdec", ""},
    
    // Kernel properties
    {"ro.kernel.qemu", "0"},
    {"ro.kernel.qemu.gles", "0"},
    
    // Hardware
    {"ro.hardware", "exynos2100"},
    {"ro.hardware.gralloc", "exynos2100"},
    {"ro.hardware.vulkan", "mali"},
    {"ro.hardware.egl", "mali"},
    
    // Serial
    {"ro.serialno", "R5CR8348495"},
    
    // Product
    {"ro.product.board", "universal2100"},
    {"ro.product.device", "o1s"},
    {"ro.product.model", "SM-G998B"},
    
    // Build
    {"ro.build.characteristics", "default"},
    
    // Hide QEMU properties
    {"qemu.hw.mainkeys", ""},
    {"qemu.sf.lcd_density", ""}
};

// Hooked __system_property_get function
int fake_system_property_get(const char *name, char *value) {
    // Check if this property should be faked
    for (const auto &prop : fake_props) {
        if (strcmp(name, prop.key) == 0) {
            // Copy fake value to output buffer
            strcpy(value, prop.value);
            int len = strlen(prop.value);
            
            LOGI("Hooked property: %s = %s", name, prop.value);
            return len;
        }
    }
    
    // Call original function for non-faked properties
    return backup_system_property_get(name, value);
}

// Callback when a library is loaded
void on_library_loaded(const char *name, void *handle) {
    LOGD("Library loaded: %s", name);
    
    // We can hook additional libraries here if needed
    // For now, we only hook libc.so which is done in native_init
}

// Entry point for native hook
extern "C" [[gnu::visibility("default")]] [[gnu::used]]
NativeOnModuleLoaded native_init(const NativeAPIEntries *entries) {
    LOGI("ClashBypass native_init called");
    
    if (entries == nullptr || entries->hook_func == nullptr) {
        LOGE("Invalid NativeAPIEntries");
        return nullptr;
    }
    
    hook_func = entries->hook_func;
    
    // Hook __system_property_get from libc.so
    void *libc_handle = dlopen("libc.so", RTLD_NOW);
    if (libc_handle == nullptr) {
        LOGE("Failed to dlopen libc.so: %s", dlerror());
        return on_library_loaded;
    }
    
    void *original_func = dlsym(libc_handle, "__system_property_get");
    if (original_func == nullptr) {
        LOGE("Failed to find __system_property_get: %s", dlerror());
        dlclose(libc_handle);
        return on_library_loaded;
    }
    
    LOGI("Found __system_property_get at %p", original_func);
    
    // Perform the hook
    int result = hook_func(
        original_func,
        (void *)fake_system_property_get,
        (void **)&backup_system_property_get
    );
    
    if (result == 0) {
        LOGI("Successfully hooked __system_property_get");
    } else {
        LOGE("Failed to hook __system_property_get, error code: %d", result);
    }
    
    dlclose(libc_handle);
    
    return on_library_loaded;
}
