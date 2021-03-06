/*
   Copyright (C) 2009 Red Hat, Inc.

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

#if defined(CLIENT_CURSOR_CACHE)

#define CACHE_NAME cursor_cache
#define CACHE_HASH_KEY CURSOR_CACHE_HASH_KEY
#define CACHE_HASH_SIZE CURSOR_CACHE_HASH_SIZE
#define FUNC_NAME(name) red_cursor_cache_##name
#define VAR_NAME(name) cursor_cache_##name
#define CHANNEL CursorChannel
#define CHANNELCLIENT CursorChannelClient

#elif defined(CLIENT_PALETTE_CACHE)

#define CACHE_NAME palette_cache
#define CACHE_HASH_KEY PALETTE_CACHE_HASH_KEY
#define CACHE_HASH_SIZE PALETTE_CACHE_HASH_SIZE
#define FUNC_NAME(name) red_palette_cache_##name
#define VAR_NAME(name) palette_cache_##name
#define CHANNEL DisplayChannel
#define CHANNELCLIENT DisplayChannelClient
#else

#error "no cache type."

#endif

static RedCacheItem *FUNC_NAME(find)(CHANNELCLIENT *channel_client, uint64_t id)
{
    RedCacheItem *item = channel_client->priv->CACHE_NAME[CACHE_HASH_KEY(id)];

    while (item) {
        if (item->id == id) {
            ring_remove(&item->u.cache_data.lru_link);
            ring_add(&channel_client->priv->VAR_NAME(lru), &item->u.cache_data.lru_link);
            break;
        }
        item = item->u.cache_data.next;
    }
    return item;
}

static void FUNC_NAME(remove)(CHANNELCLIENT *channel_client, RedCacheItem *item)
{
    RedCacheItem **now;
    spice_assert(item);

    now = &channel_client->priv->CACHE_NAME[CACHE_HASH_KEY(item->id)];
    for (;;) {
        spice_assert(*now);
        if (*now == item) {
            *now = item->u.cache_data.next;
            break;
        }
        now = &(*now)->u.cache_data.next;
    }
    ring_remove(&item->u.cache_data.lru_link);
    channel_client->priv->VAR_NAME(items)--;
    channel_client->priv->VAR_NAME(available) += item->u.cache_data.size;

    red_pipe_item_init(&item->u.pipe_data, RED_PIPE_ITEM_TYPE_INVAL_ONE);
    red_channel_client_pipe_add_tail_and_push(RED_CHANNEL_CLIENT(channel_client), &item->u.pipe_data); // for now
}

static int FUNC_NAME(add)(CHANNELCLIENT *channel_client, uint64_t id, size_t size)
{
    RedCacheItem *item;
    int key;

    item = spice_new(RedCacheItem, 1);

    channel_client->priv->VAR_NAME(available) -= size;
    SPICE_VERIFY(SPICE_OFFSETOF(RedCacheItem, u.cache_data.lru_link) == 0);
    while (channel_client->priv->VAR_NAME(available) < 0) {
        RedCacheItem *tail = (RedCacheItem *)ring_get_tail(&channel_client->priv->VAR_NAME(lru));
        if (!tail) {
            channel_client->priv->VAR_NAME(available) += size;
            free(item);
            return FALSE;
        }
        FUNC_NAME(remove)(channel_client, tail);
    }
    ++channel_client->priv->VAR_NAME(items);
    item->u.cache_data.next = channel_client->priv->CACHE_NAME[(key = CACHE_HASH_KEY(id))];
    channel_client->priv->CACHE_NAME[key] = item;
    ring_item_init(&item->u.cache_data.lru_link);
    ring_add(&channel_client->priv->VAR_NAME(lru), &item->u.cache_data.lru_link);
    item->id = id;
    item->u.cache_data.size = size;
    return TRUE;
}

static void FUNC_NAME(reset)(CHANNELCLIENT *channel_client, long size)
{
    int i;

    for (i = 0; i < CACHE_HASH_SIZE; i++) {
        while (channel_client->priv->CACHE_NAME[i]) {
            RedCacheItem *item = channel_client->priv->CACHE_NAME[i];
            channel_client->priv->CACHE_NAME[i] = item->u.cache_data.next;
            free(item);
        }
    }
    ring_init(&channel_client->priv->VAR_NAME(lru));
    channel_client->priv->VAR_NAME(available) = size;
    channel_client->priv->VAR_NAME(items) = 0;
}


#undef CACHE_NAME
#undef CACHE_HASH_KEY
#undef CACHE_HASH_SIZE
#undef FUNC_NAME
#undef VAR_NAME
#undef CHANNEL
#undef CHANNELCLIENT
