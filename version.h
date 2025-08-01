#define ZBITXD_VERSION_MAJOR 0
#define ZBITXD_VERSION_MINOR 2
#define ZBITXD_VERSION_PATCH 0

#define STRINGIFY(s) #s
#define VERSIONIZE(x, y, z)                                                    \
  "zbitxd v" STRINGIFY(x) "." STRINGIFY(y) "." STRINGIFY(z)

#define VER_STR                                                                \
  VERSIONIZE(ZBITXD_VERSION_MAJOR, ZBITXD_VERSION_MINOR, ZBITXD_VERSION_PATCH)
