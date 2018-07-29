#include "gearvr.h"

const uint16_t opt_mtu = 62; // NOTE 59 bytes of the actual packet data & 3 bytes of GATT header.

static void exchange_mtu_cb(guint8 status, const guint8 *pdu, guint16 plen, gpointer user_data) {
  gattlib_context_t* conn_context = (gattlib_context_t*) user_data;
  uint16_t mtu;

  if (status != 0) {
    printf("Exchange MTU Request failed.\n");
    return;
  }

  if (!dec_mtu_resp(pdu, plen, &mtu)) {
    printf("Protocol error.\n");
    return;
  }

  mtu = MIN(mtu, opt_mtu);
  if (g_attrib_set_mtu(conn_context->attrib, mtu))
    printf("MTU was exchanged successfully: %d\n", mtu);
  else
    printf("Error exchanging MTU\n");
}

void notification_handler(const uuid_t* uuid, const uint8_t* data, size_t data_length, void* user_data) {
  UNUSED(uuid);
  UNUSED(user_data);

  printf("Device status: ");

  for (size_t i = 0; i < data_length; i++) {
    printf("%02x ", data[i]);
  }
  printf("\n");
}

static void usage(char *argv[]) {
  printf("%s <device_address>\n", argv[0]);
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    usage(argv);
    return 1;
  }

  // Service UUIDs
  uuid_t battery_level_uuid = CREATE_UUID16(BATTERY_UUID);
  uuid_t gearvr_updates_uuid;
  uuid_t gearvr_cmd_uuid;

  if (gattlib_string_to_uuid(GEARVR_UPDATES_UUID, strlen(GEARVR_UPDATES_UUID) + 1, &gearvr_updates_uuid) < 0) {
    return 1;
  }

  if (gattlib_string_to_uuid(GEARVR_CMD_UUID, strlen(GEARVR_CMD_UUID) + 1, &gearvr_cmd_uuid) < 0) {
    return 1;
  }

  printf("Connecting to %s.\n", argv[1]);
  gatt_connection_t *connection = gattlib_connect(NULL, argv[1], BDADDR_LE_PUBLIC, BT_SEC_MEDIUM, 0, 0);
  if (connection == NULL) {
    fprintf(stderr, "Fail to connect to the bluetooth device.\n");
    return 1;
  }

  printf("Registering notification handler.\n");
  gattlib_register_notification(connection, notification_handler, NULL);

  printf("Setting the desired MTU size.\n");
  gattlib_context_t *conn_context = connection->context;
  gatt_exchange_mtu(conn_context->attrib, opt_mtu, exchange_mtu_cb, (void*) connection->context);

  uint8_t buffer[100];
  size_t len = sizeof(buffer);

  printf("Reading battery level.\n");
  size_t ret = gattlib_read_char_by_uuid(connection, &battery_level_uuid, buffer, &len);
  if (ret) {
    fprintf(stderr, "Failed to retrieve battery status.\n");
    return 1;
  }

  printf("Battery level: %d%%\n", buffer[0]);

  printf("Enabling notifications for the sensor service.\n");
  ret = gattlib_notification_start(connection, &gearvr_updates_uuid);
  if (ret) {
    fprintf(stderr, "Fail to start notification.\n");
    return 1;
  }

  printf("Reading device status.\n");
  ret = gattlib_read_char_by_uuid(connection, &gearvr_cmd_uuid, buffer, &len);
  if (ret) {
    fprintf(stderr, "Failed to retrieve device.\n");
    return 1;
  }

  printf("Device status: ");
  for (size_t i = 0; i < len; i++) {
    printf("%02x ", buffer[i]);
  }
  printf("\n");

  printf("Resetting the device.\n");
  ret = gattlib_write_char_by_handle(connection, GEARVR_CMD_HANDLE, CMD_RESET, sizeof(CMD_RESET));
  if (ret) {
    fprintf(stderr, "Failed to reset device.\n");
    return 1;
  }

  printf("Turning on the VR mode.\n");
  ret = gattlib_write_char_by_handle(connection, GEARVR_CMD_HANDLE, CMD_VR_MODE, sizeof(CMD_VR_MODE));
  if (ret) {
    fprintf(stderr, "Failed to enable the VR mode on the device.\n");
    return 1;
  }

  // NOTE Gotta wait for the device to actually enable the VR mode.
  sleep(2);

  printf("Turning on the sensor.\n");
  ret = gattlib_write_char_by_handle(connection, GEARVR_CMD_HANDLE, CMD_SENSOR, sizeof(CMD_SENSOR));
  if (ret) {
    fprintf(stderr, "Failed to enable the sensor on the device.\n");
    return 1;
  }

  GMainLoop *loop = g_main_loop_new(NULL, 0);
  g_main_loop_run(loop);

  g_main_loop_unref(loop);
  gattlib_disconnect(connection);
  puts("Done");
  return 0;
}
