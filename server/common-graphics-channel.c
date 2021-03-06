/*
   Copyright (C) 2009-2016 Red Hat, Inc.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, see <http://www.gnu.org/licenses/>.
*/
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

#include "common-graphics-channel.h"
#include "dcc.h"
#include "red-client.h"

#define CHANNEL_RECEIVE_BUF_SIZE 1024

G_DEFINE_ABSTRACT_TYPE(CommonGraphicsChannel, common_graphics_channel, RED_TYPE_CHANNEL)

#define GRAPHICS_CHANNEL_PRIVATE(o) \
    (G_TYPE_INSTANCE_GET_PRIVATE((o), TYPE_COMMON_GRAPHICS_CHANNEL, CommonGraphicsChannelPrivate))

struct CommonGraphicsChannelPrivate
{
    QXLInstance *qxl;
    uint8_t recv_buf[CHANNEL_RECEIVE_BUF_SIZE];
    int during_target_migrate; /* TRUE when the client that is associated with the channel
                                  is during migration. Turned off when the vm is started.
                                  The flag is used to avoid sending messages that are artifacts
                                  of the transition from stopped vm to loaded vm (e.g., recreation
                                  of the primary surface) */
};

static uint8_t *common_alloc_recv_buf(RedChannelClient *rcc, uint16_t type, uint32_t size)
{
    RedChannel *channel = red_channel_client_get_channel(rcc);
    CommonGraphicsChannel *common = COMMON_GRAPHICS_CHANNEL(channel);

    /* SPICE_MSGC_MIGRATE_DATA is the only client message whose size is dynamic */
    if (type == SPICE_MSGC_MIGRATE_DATA) {
        return spice_malloc(size);
    }

    if (size > CHANNEL_RECEIVE_BUF_SIZE) {
        spice_critical("unexpected message size %u (max is %d)", size, CHANNEL_RECEIVE_BUF_SIZE);
        return NULL;
    }
    return common->priv->recv_buf;
}

static void common_release_recv_buf(RedChannelClient *rcc, uint16_t type, uint32_t size,
                                    uint8_t* msg)
{
    if (type == SPICE_MSGC_MIGRATE_DATA) {
        free(msg);
    }
}


enum {
    PROP0,
    PROP_QXL
};

static void
common_graphics_channel_get_property(GObject *object,
                                   guint property_id,
                                   GValue *value,
                                   GParamSpec *pspec)
{
    CommonGraphicsChannel *self = COMMON_GRAPHICS_CHANNEL(object);

    switch (property_id)
    {
        case PROP_QXL:
            g_value_set_pointer(value, self->priv->qxl);
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
    }
}

static void
common_graphics_channel_set_property(GObject *object,
                                   guint property_id,
                                   const GValue *value,
                                   GParamSpec *pspec)
{
    CommonGraphicsChannel *self = COMMON_GRAPHICS_CHANNEL(object);

    switch (property_id)
    {
        case PROP_QXL:
            self->priv->qxl = g_value_get_pointer(value);
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
    }
}

int common_channel_config_socket(RedChannelClient *rcc)
{
    RedClient *client = red_channel_client_get_client(rcc);
    MainChannelClient *mcc = red_client_get_main(client);
    RedsStream *stream = red_channel_client_get_stream(rcc);
    int flags;
    int delay_val;
    gboolean is_low_bandwidth;

    if ((flags = fcntl(stream->socket, F_GETFL)) == -1) {
        spice_warning("accept failed, %s", strerror(errno));
        return FALSE;
    }

    if (fcntl(stream->socket, F_SETFL, flags | O_NONBLOCK) == -1) {
        spice_warning("accept failed, %s", strerror(errno));
        return FALSE;
    }

    // TODO - this should be dynamic, not one time at channel creation
    is_low_bandwidth = main_channel_client_is_low_bandwidth(mcc);
    delay_val = is_low_bandwidth ? 0 : 1;
    /* FIXME: Using Nagle's Algorithm can lead to apparent delays, depending
     * on the delayed ack timeout on the other side.
     * Instead of using Nagle's, we need to implement message buffering on
     * the application level.
     * see: http://www.stuartcheshire.org/papers/NagleDelayedAck/
     */
    if (setsockopt(stream->socket, IPPROTO_TCP, TCP_NODELAY, &delay_val,
                   sizeof(delay_val)) == -1) {
        if (errno != ENOTSUP) {
            spice_warning("setsockopt failed, %s", strerror(errno));
        }
    }
    // TODO: move wide/narrow ack setting to red_channel.
    red_channel_client_ack_set_client_window(rcc,
        is_low_bandwidth ?
        WIDE_CLIENT_ACK_WINDOW : NARROW_CLIENT_ACK_WINDOW);
    return TRUE;
}


static void
common_graphics_channel_class_init(CommonGraphicsChannelClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS(klass);
    RedChannelClass *channel_class = RED_CHANNEL_CLASS(klass);

    g_type_class_add_private(klass, sizeof(CommonGraphicsChannelPrivate));

    object_class->get_property = common_graphics_channel_get_property;
    object_class->set_property = common_graphics_channel_set_property;

    channel_class->config_socket = common_channel_config_socket;
    channel_class->alloc_recv_buf = common_alloc_recv_buf;
    channel_class->release_recv_buf = common_release_recv_buf;

    g_object_class_install_property(object_class,
                                    PROP_QXL,
                                    g_param_spec_pointer("qxl",
                                                         "qxl",
                                                         "QXLInstance for this channel",
                                                         G_PARAM_READWRITE |
                                                         G_PARAM_CONSTRUCT_ONLY |
                                                         G_PARAM_STATIC_STRINGS));
}

static void
common_graphics_channel_init(CommonGraphicsChannel *self)
{
    self->priv = GRAPHICS_CHANNEL_PRIVATE(self);
}

void common_graphics_channel_set_during_target_migrate(CommonGraphicsChannel *self, gboolean value)
{
    self->priv->during_target_migrate = value;
}

gboolean common_graphics_channel_get_during_target_migrate(CommonGraphicsChannel *self)
{
    return self->priv->during_target_migrate;
}

QXLInstance* common_graphics_channel_get_qxl(CommonGraphicsChannel *self)
{
    return self->priv->qxl;
}
