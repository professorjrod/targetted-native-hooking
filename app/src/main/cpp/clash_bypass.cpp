#include <jni.h>
#include <dlfcn.h>
#include <cstring>
#include <string>
#include <android/log.h>
#include <cstdarg>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
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
    // Critical boot properties - MUST BE FIRST
    {"ro.boot.qemu", "0"},
    {"ro.boot.hardware", "samsungexynos2100"},
    {"ro.boot.hardware.vulkan", "mali"},
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
    {"ro.boot.qemu.settings.system.screen_off_timeout", ""},
    {"ro.boot.qemu.virtiowifi", ""},
    {"ro.boot.qemu.vsync", ""},
    
    // Kernel properties
    {"ro.kernel.qemu", "0"},
    {"ro.kernel.qemu.gles", "0"},
    
    // Hardware - hide ranchu
    {"ro.hardware", "samsungexynos2100"},
    {"ro.hardware.gralloc", "mali"},
    {"ro.hardware.vulkan", "mali"},
    {"ro.hardware.egl", "mali"},
    {"ro.hardware.power", "exynos"},
    
    // Serial
    {"ro.serialno", "R5CR8348495"},
    
    // Product - base properties
    {"ro.product.board", "universal2100"},
    {"ro.product.device", "o1s"},
    {"ro.product.model", "SM-G998B"},
    {"ro.product.name", "p3sxxx"},
    {"ro.product.manufacturer", "samsung"},
    
    // Product - vendor/odm/system partitions (CRITICAL!)
    {"ro.product.vendor.model", "SM-G998B"},
    {"ro.product.vendor.device", "o1s"},
    {"ro.product.vendor.name", "p3sxxx"},
    {"ro.product.odm.device", "o1s"},
    {"ro.product.odm.name", "p3sxxx"},
    {"ro.product.product.device", "o1s"},
    {"ro.product.product.name", "p3sxxx"},
    {"ro.product.system.device", "o1s"},
    {"ro.product.system.name", "p3sxxx"},
    {"ro.product.system_ext.device", "o1s"},
    {"ro.product.system_ext.name", "p3sxxx"},
    {"ro.product.vendor_dlkm.device", "o1s"},
    {"ro.product.vendor_dlkm.name", "p3sxxx"},
    
    // Build fingerprints
    {"ro.build.characteristics", "default"},
    {"ro.product.build.fingerprint", "samsung/p3sxxx/o1s:14/UP1A.231005.007/S998BXXU1AVC1:user/release-keys"},
    {"ro.bootimage.build.fingerprint", "samsung/p3sxxx/o1s:14/UP1A.231005.007/S998BXXU1AVC1:user/release-keys"},
    
    // Hide QEMU properties
    {"qemu.hw.mainkeys", ""},
    {"qemu.sf.lcd_density", ""},
    {"qemu.sf.fake_camera", ""},
    {"vendor.qemu.sf.fake_camera", ""},
    {"vendor.qemu.timezone", ""},
    {"ro.adb.qemud", ""},
    
    // Hide QEMU services
    {"init.svc.qemu-props", "stopped"},
    {"init.svc.qemu-adb-setup", "stopped"},
    {"init.svc.qemu-adb-keys", "stopped"},
    {"init.svc.qemu-device-state", "stopped"}
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
    int result = backup_system_property_get(name, value);
    
    // Log ALL property requests to see what we're missing
    if (strstr(name, "qemu") || strstr(name, "goldfish") || 
        strstr(name, "ro.boot") || strstr(name, "ro.kernel") ||
        strstr(name, "ro.hardware") || strstr(name, "ro.product")) {
        LOGI("Unhooked property check: %s = %s", name, value);
    }
    
    return result;
}

// Backup function pointers for file operations
static FILE* (*backup_fopen)(const char *pathname, const char *mode) = nullptr;
static int (*backup_open)(const char *pathname, int flags, ...) = nullptr;
static int (*backup_openat)(int dirfd, const char *pathname, int flags, ...) = nullptr;
static int (*backup_access)(const char *pathname, int mode) = nullptr;
static int (*backup_stat)(const char *pathname, struct stat *buf) = nullptr;
static int (*backup_lstat)(const char *pathname, struct stat *buf) = nullptr;
static ssize_t (*backup_readlink)(const char *pathname, char *buf, size_t bufsiz) = nullptr;
static char* (*backup_fgets)(char *s, int size, FILE *stream) = nullptr;
static ssize_t (*backup_read)(int fd, void *buf, size_t count) = nullptr;
static void* (*backup_dlopen)(const char *filename, int flag) = nullptr;
static void* (*backup_dlsym)(void *handle, const char *symbol) = nullptr;

// Hook fopen to log file access and filter /proc/self/maps
FILE* hooked_fopen(const char *pathname, const char *mode) {
    if (pathname) {
        // Block emulator files
        if (strstr(pathname, "qemu") || strstr(pathname, "goldfish") || 
            strstr(pathname, "ranchu")) {
            LOGI("fopen BLOCKED: %s", pathname);
            errno = ENOENT;
            return nullptr;
        }
        
        // Intercept /proc/self/maps to hide LSPosed/Xposed
        if (strcmp(pathname, "/proc/self/maps") == 0 || 
            strstr(pathname, "/proc/") && strstr(pathname, "/maps")) {
            LOGI("fopen /proc/self/maps - filtering LSPosed references");
            // Return the original file - we'll filter it in read operations
            // For now, just log it
        }
        
        // Log all other accesses
        LOGI("fopen: %s", pathname);
    }
    return backup_fopen(pathname, mode);
}

// Hook open to log/block file access
int hooked_open(const char *pathname, int flags, ...) {
    if (pathname && (strstr(pathname, "qemu") || strstr(pathname, "goldfish") || 
                     strstr(pathname, "cpuinfo") || strstr(pathname, "ranchu"))) {
        LOGI("open attempt: %s - BLOCKED", pathname);
        errno = ENOENT;
        return -1;
    }
    
    // Handle optional mode parameter for O_CREAT
    mode_t mode = 0;
    if (flags & O_CREAT) {
        va_list args;
        va_start(args, flags);
        mode = va_arg(args, mode_t);
        va_end(args);
        return backup_open(pathname, flags, mode);
    }
    return backup_open(pathname, flags);
}

// Hook openat to log/block file access (modern Android API)
int hooked_openat(int dirfd, const char *pathname, int flags, ...) {
    if (pathname && (strstr(pathname, "qemu") || strstr(pathname, "goldfish") || 
                     strstr(pathname, "cpuinfo") || strstr(pathname, "ranchu"))) {
        LOGI("openat attempt: %s - BLOCKED", pathname);
        errno = ENOENT;
        return -1;
    }
    
    // Handle optional mode parameter for O_CREAT
    mode_t mode = 0;
    if (flags & O_CREAT) {
        va_list args;
        va_start(args, flags);
        mode = va_arg(args, mode_t);
        va_end(args);
        return backup_openat(dirfd, pathname, flags, mode);
    }
    return backup_openat(dirfd, pathname, flags);
}

// Hook access to log file checks
int hooked_access(const char *pathname, int mode) {
    if (pathname && (strstr(pathname, "qemu") || strstr(pathname, "goldfish") || 
                     strstr(pathname, "cpuinfo") || strstr(pathname, "ranchu"))) {
        LOGI("access attempt: %s - BLOCKED", pathname);
        errno = ENOENT;  // File not found
        return -1;
    }
    return backup_access(pathname, mode);
}

// Hook stat to hide emulator files
int hooked_stat(const char *pathname, struct stat *buf) {
    if (pathname && (strstr(pathname, "goldfish") || strstr(pathname, "qemu") || 
                     strstr(pathname, "ranchu"))) {
        LOGI("stat attempt: %s - BLOCKED", pathname);
        errno = ENOENT;  // File not found
        return -1;
    }
    return backup_stat(pathname, buf);
}

// Hook lstat to hide emulator files
int hooked_lstat(const char *pathname, struct stat *buf) {
    if (pathname && (strstr(pathname, "goldfish") || strstr(pathname, "qemu") || 
                     strstr(pathname, "ranchu"))) {
        LOGI("lstat attempt: %s - BLOCKED", pathname);
        errno = ENOENT;  // File not found
        return -1;
    }
    return backup_lstat(pathname, buf);
}

// Hook readlink to log symlink checks
ssize_t hooked_readlink(const char *pathname, char *buf, size_t bufsiz) {
    if (pathname) {
        LOGI("readlink: %s", pathname);
        
        // Block emulator-related symlinks
        if (strstr(pathname, "goldfish") || strstr(pathname, "qemu") || 
            strstr(pathname, "ranchu")) {
            LOGI("readlink BLOCKED: %s", pathname);
            errno = ENOENT;
            return -1;
        }
    }
    return backup_readlink(pathname, buf, bufsiz);
}

// Hook fgets to filter /proc/self/maps line by line
char* hooked_fgets(char *s, int size, FILE *stream) {
    char *result = backup_fgets(s, size, stream);
    
    if (result && s) {
        // Check if this line contains LSPosed/Xposed references
        if (strstr(s, "lsposed") || strstr(s, "LSPosed") || 
            strstr(s, "xposed") || strstr(s, "Xposed") ||
            strstr(s, "lspd") || strstr(s, "magisk")) {
            LOGI("Filtering maps line containing: %s", 
                 strstr(s, "lsposed") ? "lsposed" : 
                 strstr(s, "xposed") ? "xposed" : "framework");
            // Skip this line by reading the next one
            return hooked_fgets(s, size, stream);
        }
    }
    
    return result;
}

// Hook read() to filter /proc/self/maps content
ssize_t hooked_read(int fd, void *buf, size_t count) {
    ssize_t result = backup_read(fd, buf, count);
    
    if (result > 0 && buf) {
        char *content = (char *)buf;
        // Check if this looks like /proc/self/maps content and contains LSPosed
        if (strstr(content, "lsposed") || strstr(content, "LSPosed") ||
            strstr(content, "xposed") || strstr(content, "Xposed") ||
            strstr(content, "lspd")) {
            LOGI("read() detected LSPosed in content, filtering...");
            
            // Filter out lines containing LSPosed/Xposed
            char *filtered = (char *)malloc(count);
            if (filtered) {
                size_t filtered_len = 0;
                char *line = content;
                char *next_line;
                
                while (line < content + result) {
                    next_line = strchr(line, '\n');
                    if (!next_line) next_line = content + result;
                    else next_line++;
                    
                    size_t line_len = next_line - line;
                    
                    // Copy line if it doesn't contain framework references
                    if (!strstr(line, "lsposed") && !strstr(line, "xposed") &&
                        !strstr(line, "LSPosed") && !strstr(line, "Xposed") &&
                        !strstr(line, "lspd") && !strstr(line, "magisk")) {
                        memcpy(filtered + filtered_len, line, line_len);
                        filtered_len += line_len;
                    }
                    
                    line = next_line;
                }
                
                memcpy(buf, filtered, filtered_len);
                free(filtered);
                return filtered_len;
            }
        }
    }
    
    return result;
}

// Hook dlopen to detect library loading checks
void* hooked_dlopen(const char *filename, int flag) {
    if (filename) {
        LOGI("dlopen: %s", filename);
        
        // Block loading of LSPosed detection libraries
        if (strstr(filename, "lsposed") || strstr(filename, "xposed") || 
            strstr(filename, "substrate")) {
            LOGI("dlopen BLOCKED: %s", filename);
            return nullptr;
        }
    }
    return backup_dlopen(filename, flag);
}

// Hook dlsym to detect symbol lookups
void* hooked_dlsym(void *handle, const char *symbol) {
    if (symbol) {
        // Log suspicious symbol lookups
        if (strstr(symbol, "xposed") || strstr(symbol, "hook") ||
            strstr(symbol, "Xposed") || strstr(symbol, "Hook")) {
            LOGI("dlsym looking for: %s", symbol);
        }
    }
    return backup_dlsym(handle, symbol);
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
    
    // Hook file access functions to detect emulator file checks
    void *fopen_func = dlsym(libc_handle, "fopen");
    if (fopen_func) {
        hook_func(fopen_func, (void *)hooked_fopen, (void **)&backup_fopen);
        LOGI("Hooked fopen");
    }
    
    void *open_func = dlsym(libc_handle, "open");
    if (open_func) {
        hook_func(open_func, (void *)hooked_open, (void **)&backup_open);
        LOGI("Hooked open");
    }
    
    void *openat_func = dlsym(libc_handle, "openat");
    if (openat_func) {
        hook_func(openat_func, (void *)hooked_openat, (void **)&backup_openat);
        LOGI("Hooked openat");
    }
    
    void *access_func = dlsym(libc_handle, "access");
    if (access_func) {
        hook_func(access_func, (void *)hooked_access, (void **)&backup_access);
        LOGI("Hooked access");
    }
    
    void *stat_func = dlsym(libc_handle, "stat");
    if (stat_func) {
        hook_func(stat_func, (void *)hooked_stat, (void **)&backup_stat);
        LOGI("Hooked stat");
    }
    
    void *lstat_func = dlsym(libc_handle, "lstat");
    if (lstat_func) {
        hook_func(lstat_func, (void *)hooked_lstat, (void **)&backup_lstat);
        LOGI("Hooked lstat");
    }
    
    void *readlink_func = dlsym(libc_handle, "readlink");
    if (readlink_func) {
        hook_func(readlink_func, (void *)hooked_readlink, (void **)&backup_readlink);
        LOGI("Hooked readlink");
    }
    
    void *fgets_func = dlsym(libc_handle, "fgets");
    if (fgets_func) {
        hook_func(fgets_func, (void *)hooked_fgets, (void **)&backup_fgets);
        LOGI("Hooked fgets");
    }
    
    void *read_func = dlsym(libc_handle, "read");
    if (read_func) {
        hook_func(read_func, (void *)hooked_read, (void **)&backup_read);
        LOGI("Hooked read");
    }
    
    // Hook dlopen/dlsym to detect library inspection
    void *dlopen_func = dlsym(RTLD_DEFAULT, "dlopen");
    if (dlopen_func) {
        hook_func(dlopen_func, (void *)hooked_dlopen, (void **)&backup_dlopen);
        LOGI("Hooked dlopen");
    }
    
    void *dlsym_func = dlsym(RTLD_DEFAULT, "dlsym");
    if (dlsym_func) {
        hook_func(dlsym_func, (void *)hooked_dlsym, (void **)&backup_dlsym);
        LOGI("Hooked dlsym");
    }
    
    dlclose(libc_handle);
    
    return on_library_loaded;
}
