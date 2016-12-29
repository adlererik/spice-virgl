
/* Generated data (by glib-mkenums) */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <glib-object.h>

#include "spice-server-enums.h"
#include "spice-server.h"

static const GEnumValue _spice_compat_version_t_spice_compat_version_t_values[] = {
    { SPICE_COMPAT_VERSION_0_4, "SPICE_COMPAT_VERSION_0_4", "4" },
    { SPICE_COMPAT_VERSION_0_6, "SPICE_COMPAT_VERSION_0_6", "6" },
    { 0, NULL, NULL }
};

GType
spice_compat_version_t_spice_compat_version_t_get_type (void)
{
    static GType type = 0;
    static volatile gsize type_volatile = 0;

    if (g_once_init_enter(&type_volatile)) {
        type = g_enum_register_static ("spice_compat_version_t", _spice_compat_version_t_spice_compat_version_t_values);
        g_once_init_leave(&type_volatile, type);
    }

    return type;
}

static const GEnumValue _spice_image_compression_t_spice_image_compression_t_values[] = {
    { SPICE_IMAGE_COMPRESSION_INVALID, "SPICE_IMAGE_COMPRESSION_INVALID", "invalid" },
    { SPICE_IMAGE_COMPRESSION_OFF, "SPICE_IMAGE_COMPRESSION_OFF", "off" },
    { SPICE_IMAGE_COMPRESSION_AUTO_GLZ, "SPICE_IMAGE_COMPRESSION_AUTO_GLZ", "auto-glz" },
    { SPICE_IMAGE_COMPRESSION_AUTO_LZ, "SPICE_IMAGE_COMPRESSION_AUTO_LZ", "auto-lz" },
    { SPICE_IMAGE_COMPRESSION_QUIC, "SPICE_IMAGE_COMPRESSION_QUIC", "quic" },
    { SPICE_IMAGE_COMPRESSION_GLZ, "SPICE_IMAGE_COMPRESSION_GLZ", "glz" },
    { SPICE_IMAGE_COMPRESSION_LZ, "SPICE_IMAGE_COMPRESSION_LZ", "lz" },
    { SPICE_IMAGE_COMPRESSION_LZ4, "SPICE_IMAGE_COMPRESSION_LZ4", "lz4" },
    { 0, NULL, NULL }
};

GType
spice_image_compression_t_spice_image_compression_t_get_type (void)
{
    static GType type = 0;
    static volatile gsize type_volatile = 0;

    if (g_once_init_enter(&type_volatile)) {
        type = g_enum_register_static ("spice_image_compression_t", _spice_image_compression_t_spice_image_compression_t_values);
        g_once_init_leave(&type_volatile, type);
    }

    return type;
}

static const GEnumValue _spice_wan_compression_t_spice_wan_compression_t_values[] = {
    { SPICE_WAN_COMPRESSION_INVALID, "SPICE_WAN_COMPRESSION_INVALID", "invalid" },
    { SPICE_WAN_COMPRESSION_AUTO, "SPICE_WAN_COMPRESSION_AUTO", "auto" },
    { SPICE_WAN_COMPRESSION_ALWAYS, "SPICE_WAN_COMPRESSION_ALWAYS", "always" },
    { SPICE_WAN_COMPRESSION_NEVER, "SPICE_WAN_COMPRESSION_NEVER", "never" },
    { 0, NULL, NULL }
};

GType
spice_wan_compression_t_spice_wan_compression_t_get_type (void)
{
    static GType type = 0;
    static volatile gsize type_volatile = 0;

    if (g_once_init_enter(&type_volatile)) {
        type = g_enum_register_static ("spice_wan_compression_t", _spice_wan_compression_t_spice_wan_compression_t_values);
        g_once_init_leave(&type_volatile, type);
    }

    return type;
}


/* Generated data ends here */

