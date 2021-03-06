/*** BEGIN file-header ***/
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <glib-object.h>

#include "spice-server-enums.h"
#include "spice-server.h"

/*** END file-header ***/

/*** BEGIN value-header ***/
static const G@Type@Value _@enum_name@_values[] = {
/*** END value-header ***/

/*** BEGIN value-production ***/
    { @VALUENAME@, "@VALUENAME@", "@valuenick@" },
/*** END value-production ***/

/*** BEGIN value-tail ***/
    { 0, NULL, NULL }
};

GType
@enum_name@_get_type (void)
{
    static GType type = 0;
    static volatile gsize type_volatile = 0;

    if (g_once_init_enter(&type_volatile)) {
        type = g_@type@_register_static ("@EnumName@", _@enum_name@_values);
        g_once_init_leave(&type_volatile, type);
    }

    return type;
}

/*** END value-tail ***/
