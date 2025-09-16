#include "block.h"
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
#include <dlfcn.h>
#include <cstring>
#include <array>
#include "Substrate/And64InlineHook.hpp"
// Daftar domain yang ingin diblok
static const char* BLACKLIST[] = {
    "analytics.bloodstrike.netease.com",
    "bamreport.bloodstrike.netease.com",
    "reportban.netease.com",
    "ban.netease.com",
    "matchreport.bloodstrike.netease.com",
    "securitycheck.netease.com",
    "accountsecurity.netease.com",
    "userlog.bloodstrike.netease.com",
    "easebar.com",
    "monitor.easebar.com",
    "data.easebar.com",
    "report.easebar.com",
    "analytics.easebar.com",
    "track.easebar.com",
    "stat.easebar.com",
    "mgbsdknaeast.matrix.easebar.com",
    "android.googleapis.com",
    "prod-lt-playstoregatewayadapter-pa.googleapis.com",
    "anticheat.bloodstrike.net",
    "anticheat-api.bloodstrike.com",
    "qsec.netease.com",
    "tinfo.netease.com",
    "log.qos.netease.com",
    "report.bloodstrike.net",
    "report-api.bloodstrike.io",
    "report-api.bloodstrike.net",
    "appeal-api.bloodstrike.io",
    "log.bloodstrike.net",
    "track.bloodstrike.io",
    "log-collector.bloodstrike.com",
    "stat-api.bloodstrike.net",
    "update-api.bloodstrike.io",
    "config.bloodstrike.io",
    "crashlyticsreports-pa.googleapis.com",
    "protocol.unisdk.easebar.com",
    "translate.mpsdk.easebar.com",
    "g0.gsf.easebar.com",
    "sdkgate.pushv3.easebar.com",
    "who.easebar.com",
    "drpf-g83naxx1ena.proxima.nie.easebar.com",
    "data-detect.nie.easebar.com",
    "s.q.easebar.com",
    "optsdk.gameyw.easebar.com",
    "nos.gameyw.easebar.com",
    "t.appsflyer.com",
    "api.access.umeng.com",
    "crashlytics.com",
    "reports.crashlytics.com",
    "app-measurement.com",
    "events.appsflyer.com",
    "log-api.dragonflygames",
    "log-tracker.dragonflygames.cn",
    "tracking.netease.com"
};

static constexpr size_t BLACKLIST_SIZE = sizeof(BLACKLIST)/sizeof(BLACKLIST[0]);

// Fungsi asli
static int (*orig_getaddrinfo)(const char *node, const char *service,
                               const struct addrinfo *hints,
                               struct addrinfo **res) = nullptr;

static bool is_blacklisted(const char *node) {
    if (!node) return false;
    for (size_t i = 0; i < BLACKLIST_SIZE; ++i) {
        if (strcasecmp(node, BLACKLIST[i]) == 0)
            return true;
    }
    return false;
}

// Hook-nya
int hooked_getaddrinfo(const char *node, const char *service,
                       const struct addrinfo *hints,
                       struct addrinfo **res) {
    if (is_blacklisted(node)) {
        return EAI_NONAME; // domain not found -> putuskan koneksi
    }
    return orig_getaddrinfo(node, service, hints, res);
}

void install_domain_blocker() {
    void *libc = dlopen("libc.so", RTLD_NOW);
    if (!libc) return;
    void *sym = dlsym(libc, "getaddrinfo");
    if (sym) {
        A64HookFunction(sym, (void*)hooked_getaddrinfo, (void**)&orig_getaddrinfo);
    }
}