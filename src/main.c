#include <kernel.h>
#include <stdio.h>
#include <stdint.h>

typedef enum
{
        CANIOT_DEVICE_CLASS0 = 0,
        CANIOT_DEVICE_CLASS1,
        CANIOT_DEVICE_CLASS2,
        CANIOT_DEVICE_CLASS3,
        CANIOT_DEVICE_CLASS4,
        CANIOT_DEVICE_CLASS5,
        CANIOT_DEVICE_CLASS6,
        CANIOT_DEVICE_CLASS7,
} caniot_device_class_t;

typedef enum
{
        CANIOT_DEVICE_SID0 = 0,
        CANIOT_DEVICE_SID1,
        CANIOT_DEVICE_SID2,
        CANIOT_DEVICE_SID3,
        CANIOT_DEVICE_SID4,
        CANIOT_DEVICE_SID5,
        CANIOT_DEVICE_SID6,
        CANIOT_DEVICE_SID7,
} caniot_device_subid_t;

typedef enum
{
        CANIOT_FRAME_TYPE_COMMAND = 0,
        CANIOT_FRAME_TYPE_TELEMETRY = 1,
        CANIOT_FRAME_TYPE_WRITE_ATTRIBUTE = 2,
        CANIOT_FRAME_TYPE_READ_ATTRIBUTE = 3,
} caniot_frame_type_t;

typedef enum
{
        CANIOT_QUERY = 0,
        CANIOT_RESPONSE = 1,
} caniot_frame_dir_t;

typedef enum {
        CANIOT_ENDPOINT_APP = 0,
        CANIOT_ENDPOINT_1 = 1,
        CANIOT_ENDPOINT_2 = 2,
        CANIOT_ENDPOINT_BOARD_CONTROL = 3,
} caniot_endpoint_t;

union caniot_id {
        struct __attribute__((packed)) {
                caniot_frame_type_t type : 2;
                caniot_frame_dir_t query : 1;
                caniot_device_class_t cls : 3;
                caniot_device_subid_t sid : 3;
                caniot_endpoint_t endpoint : 2;
        };
        uint16_t raw;
};

union caniot_id cid = {
        .type = CANIOT_FRAME_TYPE_TELEMETRY,
        .query = CANIOT_RESPONSE,
        .cls = CANIOT_DEVICE_CLASS0,
        .sid = CANIOT_DEVICE_SID4,
        .endpoint = CANIOT_ENDPOINT_BOARD_CONTROL
};

int main(void)
{
        printk("%hx: type=%u, query=%u, cls=%u, sid=%u, endpoint=%u",
                cid.raw, cid.type, cid.query, cid.cls, cid.sid, cid.endpoint);
	
	return 0;
}