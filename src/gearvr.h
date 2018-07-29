#ifndef __GEAR_VR_H__
#define __GEAR_VR_H__

#include <assert.h>
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <gattlib.h>

#define UNUSED(x) (void)(x)

#define BATTERY_UUID 0x2A19
#define GEARVR_UPDATES_UUID "c8c51726-81bc-483b-a052-f7a14ea3d281"
#define GEARVR_CMD_UUID "c8c51726-81bc-483b-a052-f7a14ea3d282"
#define GEARVR_CMD_HANDLE 0x35

const uint8_t CMD_RESET[] = { 0x00, 0x00 };
const uint8_t CMD_SENSOR[] = { 0x01, 0x00 };
const uint8_t CMD_KEEPALIVE[] = { 0x04, 0x00 };
const uint8_t CMD_VR_MODE[] = { 0x08, 0x00 };

// FIXME These are an internal abstraction that is leaking out.
typedef struct {
  GIOChannel* io;
  GAttrib* attrib;

  // We keep a list of characteristics to make the correspondence handle/UUID.
  void* characteristics;
  int characteristic_count;
} gattlib_context_t;

void gatt_exchange_mtu(void*, guint16, void (*)(guint8, const guint8*, guint16, gpointer), void*);

int dec_mtu_resp(const guint8*, guint16, guint16*);

int g_attrib_set_mtu(void*, guint16);

#endif
