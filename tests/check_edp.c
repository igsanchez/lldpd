#define _GNU_SOURCE 1
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <check.h>
#include "../src/lldpd.h"
#include "common.h"

char filenameprefix[] = "edp_send";

#ifdef ENABLE_EDP

START_TEST (test_send_basic)
{
	int n;
	/* Packet we should build:
Extreme Discovery Protocol
    Version: 1
    Reserved: 0
    Data length: 74
    Checksum: 0xde22 [correct]
        [Good: True]
        [Bad : False]
    Sequence number: 0
    Machine ID type: MAC (0)
    Machine MAC: 5e:10:8e:e7:84:ad (5e:10:8e:e7:84:ad)
    Display: "First chassis"
        Marker 0x99, length 18, type 1 = Display
            TLV Marker: 0x99
            TLV type: Display (1)
            TLV length: 18
        Name: First chassis
    Info: Slot/Port: 1/4, Version: 7.6.4.99
        Marker 0x99, length 36, type 2 = Info
            TLV Marker: 0x99
            TLV type: Info (2)
            TLV length: 36
        Slot: 1
        Port: 4
        Virt chassis: 0
        Reserved: 000000000000
        Version: 7.6.4 Internal: 99
            Version: 0x07060463
            Version (major1): 7
            Version (major2): 6
            Version (sustaining): 4
            Version (internal): 99
        Connections: FFFFFFFF000000000000000000000000
    Null
        Marker 0x99, length 4, type 0 = Null
            TLV Marker: 0x99
            TLV type: Null (0)
            TLV length: 4
	 */
	char pkt1[] = {
		0x00, 0xe0, 0x2b, 0x00, 0x00, 0x00, 0x5e, 0x10,
		0x8e, 0xe7, 0x84, 0xad, 0x00, 0x52, 0xaa, 0xaa,
		0x03, 0x00, 0xe0, 0x2b, 0x00, 0xbb, 0x01, 0x00,
		0x00, 0x4a, 0xde, 0x22, 0x00, 0x00, 0x00, 0x00,
		0x5e, 0x10, 0x8e, 0xe7, 0x84, 0xad, 0x99, 0x01,
		0x00, 0x12, 0x46, 0x69, 0x72, 0x73, 0x74, 0x20,
		0x63, 0x68, 0x61, 0x73, 0x73, 0x69, 0x73, 0x00,
		0x99, 0x02, 0x00, 0x24, 0x00, 0x00, 0x00, 0x03,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x07, 0x06, 0x04, 0x63, 0xff, 0xff, 0xff, 0xff,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x99, 0x00, 0x00, 0x04 };
	struct packet *pkt;

	/* Populate port and chassis */
	hardware.h_lport.p_id_subtype = LLDP_PORTID_SUBTYPE_IFNAME;
	hardware.h_lport.p_id = "Not used";
	hardware.h_lport.p_id_len = strlen(hardware.h_lport.p_id);
	hardware.h_lport.p_descr = "Not used";
	strcpy(hardware.h_ifname, "eth3");
	hardware.h_ifindex = 4;
	chassis.c_id_subtype = LLDP_CHASSISID_SUBTYPE_LLADDR;
	chassis.c_id = macaddress;
	chassis.c_id_len = ETH_ALEN;
	chassis.c_name = "First chassis";
	/* Build packet */
	n = edp_send(NULL, &hardware);
	if (n != 0) {
		fail("unable to build packet");
		return;
	}
	if (TAILQ_EMPTY(&pkts)) {
		fail("no packets sent");
		return;
	}
	pkt = TAILQ_FIRST(&pkts);
	ck_assert_int_eq(pkt->size, sizeof(pkt1));
	fail_unless(memcmp(pkt->data, pkt1, sizeof(pkt1)) == 0);
	fail_unless(TAILQ_NEXT(pkt, next) == NULL, "more than one packet sent");
}
END_TEST

#ifdef ENABLE_DOT1
START_TEST (test_send_vlans)
{
	int n;
	/* Packets we should build:
Extreme Discovery Protocol
    Version: 1
    Reserved: 0
    Data length: 74
    Checksum: 0xde20 [correct]
        [Good: True]
        [Bad : False]
    Sequence number: 2
    Machine ID type: MAC (0)
    Machine MAC: 5e:10:8e:e7:84:ad (5e:10:8e:e7:84:ad)
    Display: "First chassis"
        Marker 0x99, length 18, type 1 = Display
            TLV Marker: 0x99
            TLV type: Display (1)
            TLV length: 18
        Name: First chassis
    Info: Slot/Port: 1/4, Version: 7.6.4.99
        Marker 0x99, length 36, type 2 = Info
            TLV Marker: 0x99
            TLV type: Info (2)
            TLV length: 36
        Slot: 1
        Port: 4
        Virt chassis: 0
        Reserved: 000000000000
        Version: 7.6.4 Internal: 99
            Version: 0x07060463
            Version (major1): 7
            Version (major2): 6
            Version (sustaining): 4
            Version (internal): 99
        Connections: FFFFFFFF000000000000000000000000
    Null
        Marker 0x99, length 4, type 0 = Null
            TLV Marker: 0x99
            TLV type: Null (0)
            TLV length: 4

Extreme Discovery Protocol
    Version: 1
    Reserved: 0
    Data length: 102
    Checksum: 0x28c4 [correct]
        [Good: True]
        [Bad : False]
    Sequence number: 3
    Machine ID type: MAC (0)
    Machine MAC: 5e:10:8e:e7:84:ad (5e:10:8e:e7:84:ad)
    Vlan: ID 157, Name "First VLAN"
        Marker 0x99, length 27, type 5 = VL
            TLV Marker: 0x99
            TLV type: VL (5)
            TLV length: 27
        Flags: 0x00
            0... .... = Flags-IP: Not set
            .000 000. = Flags-reserved: 0x00
            .... ...0 = Flags-Unknown: Not set
        Reserved1: 00
        Vlan ID: 157
        Reserved2: 00000000
        IP addr: 0.0.0.0 (0.0.0.0)
        Name: First VLAN
    Vlan: ID 1247, Name "Second VLAN"
        Marker 0x99, length 28, type 5 = VL
            TLV Marker: 0x99
            TLV type: VL (5)
            TLV length: 28
        Flags: 0x00
            0... .... = Flags-IP: Not set
            .000 000. = Flags-reserved: 0x00
            .... ...0 = Flags-Unknown: Not set
        Reserved1: 00
        Vlan ID: 1247
        Reserved2: 00000000
        IP addr: 0.0.0.0 (0.0.0.0)
        Name: Second VLAN
    Vlan: ID 741, Name "Third VLAN"
        Marker 0x99, length 27, type 5 = VL
            TLV Marker: 0x99
            TLV type: VL (5)
            TLV length: 27
        Flags: 0x00
            0... .... = Flags-IP: Not set
            .000 000. = Flags-reserved: 0x00
            .... ...0 = Flags-Unknown: Not set
        Reserved1: 00
        Vlan ID: 741
        Reserved2: 00000000
        IP addr: 0.0.0.0 (0.0.0.0)
        Name: Third VLAN
    Null
        Marker 0x99, length 4, type 0 = Null
            TLV Marker: 0x99
            TLV type: Null (0)
            TLV length: 4
	*/
	char pkt1[] = {
		0x00, 0xe0, 0x2b, 0x00, 0x00, 0x00, 0x5e, 0x10,
		0x8e, 0xe7, 0x84, 0xad, 0x00, 0x52, 0xaa, 0xaa,
		0x03, 0x00, 0xe0, 0x2b, 0x00, 0xbb, 0x01, 0x00,
		0x00, 0x4a, 0xde, 0x20, 0x00, 0x02, 0x00, 0x00,
		0x5e, 0x10, 0x8e, 0xe7, 0x84, 0xad, 0x99, 0x01,
		0x00, 0x12, 0x46, 0x69, 0x72, 0x73, 0x74, 0x20,
		0x63, 0x68, 0x61, 0x73, 0x73, 0x69, 0x73, 0x00,
		0x99, 0x02, 0x00, 0x24, 0x00, 0x00, 0x00, 0x03,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x07, 0x06, 0x04, 0x63, 0xff, 0xff, 0xff, 0xff,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x99, 0x00, 0x00, 0x04 };

	char pkt2[] = {
		0x00, 0xe0, 0x2b, 0x00, 0x00, 0x00, 0x5e, 0x10,
		0x8e, 0xe7, 0x84, 0xad, 0x00, 0x6e, 0xaa, 0xaa,
		0x03, 0x00, 0xe0, 0x2b, 0x00, 0xbb, 0x01, 0x00,
		0x00, 0x66, 0x28, 0xc4, 0x00, 0x03, 0x00, 0x00,
		0x5e, 0x10, 0x8e, 0xe7, 0x84, 0xad, 0x99, 0x05,
		0x00, 0x1b, 0x00, 0x00, 0x00, 0x9d, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46, 0x69,
		0x72, 0x73, 0x74, 0x20, 0x56, 0x4c, 0x41, 0x4e,
		0x00, 0x99, 0x05, 0x00, 0x1c, 0x00, 0x00, 0x04,
		0xdf, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x53, 0x65, 0x63, 0x6f, 0x6e, 0x64, 0x20,
		0x56, 0x4c, 0x41, 0x4e, 0x00, 0x99, 0x05, 0x00,
		0x1b, 0x00, 0x00, 0x02, 0xe5, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x54, 0x68, 0x69,
		0x72, 0x64, 0x20, 0x56, 0x4c, 0x41, 0x4e, 0x00,
		0x99, 0x00, 0x00, 0x04 };

	struct packet *pkt;
	struct lldpd_vlan vlan1, vlan2, vlan3;

	/* Populate port and chassis */
	hardware.h_lport.p_id_subtype = LLDP_PORTID_SUBTYPE_IFNAME;
	hardware.h_lport.p_id = "Not used";
	hardware.h_lport.p_id_len = strlen(hardware.h_lport.p_id);
	hardware.h_lport.p_descr = "Not used";
	strcpy(hardware.h_ifname, "eth3");
	hardware.h_ifindex = 4;
	chassis.c_id_subtype = LLDP_CHASSISID_SUBTYPE_LLADDR;
	chassis.c_id = macaddress;
	chassis.c_id_len = ETH_ALEN;
	chassis.c_name = "First chassis";
	vlan1.v_name = "First VLAN"; vlan1.v_vid = 157;
	vlan2.v_name = "Second VLAN"; vlan2.v_vid = 1247;
	vlan3.v_name = "Third VLAN"; vlan3.v_vid = 741;
	TAILQ_INSERT_TAIL(&hardware.h_lport.p_vlans, &vlan1, v_entries);
	TAILQ_INSERT_TAIL(&hardware.h_lport.p_vlans, &vlan2, v_entries);
	TAILQ_INSERT_TAIL(&hardware.h_lport.p_vlans, &vlan3, v_entries);

	/* Build packet */
	n = edp_send(NULL, &hardware);
	if (n != 0) {
		fail("unable to build packet");
		return;
	}
	if (TAILQ_EMPTY(&pkts)) {
		fail("no packets sent");
		return;
	}
	pkt = TAILQ_FIRST(&pkts);
	ck_assert_int_eq(pkt->size, sizeof(pkt1));
	fail_unless(memcmp(pkt->data, pkt1, sizeof(pkt1)) == 0);
	pkt = TAILQ_NEXT(pkt, next);
	if (!pkt) {
		fail("need one more packet");
		return;
	}
	ck_assert_int_eq(pkt->size, sizeof(pkt2));
	fail_unless(memcmp(pkt->data, pkt2, sizeof(pkt2)) == 0);
	fail_unless(TAILQ_NEXT(pkt, next) == NULL, "more than two packets sent");
}
END_TEST
#endif

START_TEST (test_recv_edp)
{
	char pkt1[] = {
		0x00, 0xe0, 0x2b, 0x00, 0x00, 0x00, 0x00, 0x04,
		0x96, 0x05, 0x44, 0x6f, 0x01, 0x44, 0xaa, 0xaa,
		0x03, 0x00, 0xe0, 0x2b, 0x00, 0xbb, 0x01, 0x00,
		0x01, 0x3c, 0x05, 0xdf, 0x03, 0x0f, 0x00, 0x00,
		0x00, 0x04, 0x96, 0x05, 0x44, 0x6f, 0x99, 0x02,
		0x00, 0x24, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x06,
		0x04, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x99, 0x01, 0x01, 0x04, 0x6e, 0x65,
		0x30, 0x35, 0x30, 0x31, 0x73, 0x77, 0x2e, 0x58,
		0x58, 0x58, 0x58, 0x58, 0x58, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x03, 0x7b, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x80, 0x79, 0x0d, 0xec, 0xff, 0xff,
		0xff, 0xff, 0x80, 0xa7, 0x8b, 0x24, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x17, 0x08, 0x7e, 0xe5, 0xe2,
		0x00, 0x00, 0xee, 0xee, 0xee, 0xee, 0x00, 0x00,
		0x00, 0x02, 0x81, 0xb2, 0x19, 0xf0, 0x00, 0x00,
		0x00, 0x02, 0x80, 0xa5, 0x67, 0x20, 0xee, 0xee,
		0xee, 0xee, 0x80, 0xea, 0x8c, 0xac, 0x00, 0x00,
		0x00, 0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00,
		0x00, 0x00, 0x00, 0xe0, 0x2b, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00,
		0x00, 0x00, 0x00, 0xe0, 0x2b, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x80, 0xa4, 0x86, 0x2c, 0xee, 0xee,
		0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee,
		0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0x00, 0x00,
		0x00, 0x00, 0xee, 0xee, 0xee, 0xee, 0x00, 0xe0,
		0x2b, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00,
		0x00, 0x00, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee,
		0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0x99, 0x00,
		0x00, 0x04 };
	/* This is:
IEEE 802.3 Ethernet 
    Destination: Extreme-EDP (00:e0:2b:00:00:00)
    Source: ExtremeN_05:44:6f (00:04:96:05:44:6f)
    Length: 324
Logical-Link Control
    DSAP: SNAP (0xaa)
    IG Bit: Individual
    SSAP: SNAP (0xaa)
    CR Bit: Command
    Control field: U, func=UI (0x03)
        000. 00.. = Command: Unnumbered Information (0x00)
        .... ..11 = Frame type: Unnumbered frame (0x03)
    Organization Code: Extreme Networks (0x00e02b)
    PID: EDP (0x00bb)
Extreme Discovery Protocol
    Version: 1
    Reserved: 0
    Data length: 316
    Checksum: 0xdf05 [correct]
        [Good: True]
        [Bad : False]
    Sequence number: 783
    Machine ID type: MAC (0)
    Machine MAC: ExtremeN_05:44:6f (00:04:96:05:44:6f)
    Info: Slot/Port: 1/1, Version: 7.6.4.0
        Marker 0x99, length 36, type 2 = Info
            TLV Marker: 0x99
            TLV type: Info (2)
            TLV length: 36
        Slot: 1
        Port: 1
        Virt chassis: 0
        Reserved: 000000000000
        Version: 7.6.4 Internal: 0
            Version: 0x07060400
            Version (major1): 7
            Version (major2): 6
            Version (sustaining): 4
            Version (internal): 0
        Connections: FFFF0000000000000000000000000000
    Display: "ne0501sw.XXXXXX"
        Marker 0x99, length 260, type 1 = Display
            TLV Marker: 0x99
            TLV type: Display (1)
            TLV length: 260
        Name: ne0501sw.XXXXXX
    Null
        Marker 0x99, length 4, type 0 = Null
            TLV Marker: 0x99
            TLV type: Null (0)
            TLV length: 4
	*/

#ifdef ENABLE_DOT1
	char pkt2[] = {
		0x00, 0xe0, 0x2b, 0x00, 0x00, 0x00, 0x00, 0x04,
		0x96, 0x05, 0x44, 0x6f, 0x01, 0x48, 0xaa, 0xaa,
		0x03, 0x00, 0xe0, 0x2b, 0x00, 0xbb, 0x01, 0x00,
		0x01, 0x40, 0x73, 0x04, 0x03, 0x10, 0x00, 0x00,
		0x00, 0x04, 0x96, 0x05, 0x44, 0x6f, 0x99, 0x05,
		0x00, 0x64, 0x80, 0x00, 0x00, 0x01, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x44, 0x65,
		0x66, 0x61, 0x75, 0x6c, 0x74, 0x00, 0x43, 0x61,
		0x6e, 0x6e, 0x6f, 0x74, 0x20, 0x73, 0x61, 0x76,
		0x65, 0x20, 0x62, 0x72, 0x69, 0x64, 0x67, 0x65,
		0x20, 0x64, 0x65, 0x66, 0x61, 0x75, 0x6c, 0x74,
		0x20, 0x74, 0x6f, 0x20, 0x6e, 0x76, 0x20, 0x28,
		0x25, 0x64, 0x29, 0x0a, 0x00, 0x00, 0x4e, 0x6f,
		0x20, 0x62, 0x72, 0x69, 0x64, 0x67, 0x65, 0x20,
		0x66, 0x6f, 0x72, 0x20, 0x73, 0x75, 0x70, 0x65,
		0x72, 0x42, 0x72, 0x69, 0x64, 0x67, 0x65, 0x49,
		0x6e, 0x73, 0x74, 0x20, 0x25, 0x64, 0x00, 0x00,
		0x00, 0x00, 0x99, 0x05, 0x00, 0x64, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x4d, 0x61, 0x63, 0x56, 0x6c, 0x61,
		0x6e, 0x44, 0x69, 0x73, 0x63, 0x6f, 0x76, 0x65,
		0x72, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x99, 0x05,
		0x00, 0x64, 0x80, 0x00, 0x00, 0x32, 0x00, 0x00,
		0x00, 0x00, 0x0a, 0x32, 0x00, 0x3f, 0x41, 0x64,
		0x6d, 0x69, 0x6e, 0x42, 0x32, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x99, 0x00, 0x00, 0x04 };
	/* This is:
IEEE 802.3 Ethernet 
    Destination: Extreme-EDP (00:e0:2b:00:00:00)
    Source: ExtremeN_05:44:6f (00:04:96:05:44:6f)
    Length: 328
Logical-Link Control
    DSAP: SNAP (0xaa)
    IG Bit: Individual
    SSAP: SNAP (0xaa)
    CR Bit: Command
    Control field: U, func=UI (0x03)
        000. 00.. = Command: Unnumbered Information (0x00)
        .... ..11 = Frame type: Unnumbered frame (0x03)
    Organization Code: Extreme Networks (0x00e02b)
    PID: EDP (0x00bb)
Extreme Discovery Protocol
    Version: 1
    Reserved: 0
    Data length: 320
    Checksum: 0x7304 [correct]
        [Good: True]
        [Bad : False]
    Sequence number: 784
    Machine ID type: MAC (0)
    Machine MAC: ExtremeN_05:44:6f (00:04:96:05:44:6f)
    Vlan: ID 1, Name "Default"
        Marker 0x99, length 100, type 5 = VL
            TLV Marker: 0x99
            TLV type: VL (5)
            TLV length: 100
        Flags: 0x80
            1... .... = Flags-IP: Set
            .000 000. = Flags-reserved: 0x00
            .... ...0 = Flags-Unknown: Not set
        Reserved1: 00
        Vlan ID: 1
        Reserved2: 00000000
        IP addr: 0.0.0.0 (0.0.0.0)
        Name: Default
    Vlan: ID 0, Name "MacVlanDiscover"
        Marker 0x99, length 100, type 5 = VL
            TLV Marker: 0x99
            TLV type: VL (5)
            TLV length: 100
        Flags: 0x00
            0... .... = Flags-IP: Not set
            .000 000. = Flags-reserved: 0x00
            .... ...0 = Flags-Unknown: Not set
        Reserved1: 00
        Vlan ID: 0
        Reserved2: 00000000
        IP addr: 0.0.0.0 (0.0.0.0)
        Name: MacVlanDiscover
    Vlan: ID 50, Name "AdminB2"
        Marker 0x99, length 100, type 5 = VL
            TLV Marker: 0x99
            TLV type: VL (5)
            TLV length: 100
        Flags: 0x80
            1... .... = Flags-IP: Set
            .000 000. = Flags-reserved: 0x00
            .... ...0 = Flags-Unknown: Not set
        Reserved1: 00
        Vlan ID: 50
        Reserved2: 00000000
        IP addr: 10.50.0.63 (10.50.0.63)
        Name: AdminB2
    Null
        Marker 0x99, length 4, type 0 = Null
            TLV Marker: 0x99
            TLV type: Null (0)
            TLV length: 4
	*/
	struct lldpd_vlan *vlan;
#endif
	struct lldpd_chassis *nchassis = NULL;
	struct lldpd_port *nport = NULL;
	struct lldpd cfg;
	char mac1[] = { 0x00, 0x04, 0x96, 0x05, 0x44, 0x6f };

	cfg.g_mgmt_pattern = NULL;
	fail_unless(edp_decode(&cfg, pkt1, sizeof(pkt1), &hardware,
		&nchassis, &nport) != -1);
	if (!nchassis || !nport) {
		fail("unable to decode packet");
		return;
	}
	ck_assert_int_eq(nchassis->c_id_subtype,
	    LLDP_CHASSISID_SUBTYPE_LLADDR);
	ck_assert_int_eq(nchassis->c_id_len, ETH_ALEN);
	fail_unless(memcmp(nchassis->c_id, mac1, ETH_ALEN) == 0);
	ck_assert_int_eq(nport->p_id_subtype,
	    LLDP_PORTID_SUBTYPE_IFNAME);
	ck_assert_int_eq(nport->p_id_len, strlen("1/1"));
	fail_unless(memcmp(nport->p_id,
		"1/1", strlen("1/1")) == 0);
	ck_assert_str_eq(nport->p_descr, "Slot 1 / Port 1");
	ck_assert_str_eq(nchassis->c_name, "ne0501sw.XXXXXX");
	ck_assert_str_eq(nchassis->c_descr, "EDP enabled device, version 7.6.4.0");
	ck_assert_int_eq(nchassis->c_cap_enabled, 0);

#ifdef ENABLE_DOT1
	/* Add this port to list of remote port for hardware port */
	TAILQ_INSERT_TAIL(&hardware.h_rports, nport, p_entries);
	nport->p_chassis = nchassis;
	nport->p_protocol = LLDPD_MODE_EDP;

	/* Recept second packet */
	nchassis = NULL; nport = NULL;
	fail_unless(edp_decode(&cfg, pkt2, sizeof(pkt2), &hardware,
		&nchassis, &nport) == -1);
	nport = TAILQ_FIRST(&hardware.h_rports);
	if (!nport) {
		fail("unable to find our previous port?");
		return;
	}
	ck_assert_int_eq(TAILQ_FIRST(&nport->p_chassis->c_mgmt)->m_addr.inet.s_addr,
	    (u_int32_t)inet_addr("10.50.0.63"));
	if (TAILQ_EMPTY(&nport->p_vlans)) {
		fail("no VLAN");
		return;
	}
	vlan = TAILQ_FIRST(&nport->p_vlans);
	ck_assert_int_eq(vlan->v_vid, 1);
	ck_assert_str_eq(vlan->v_name, "Default");
	vlan = TAILQ_NEXT(vlan, v_entries);
	if (!vlan) {
		fail("no more VLAN");
		return;
	}
	ck_assert_int_eq(vlan->v_vid, 0);
	ck_assert_str_eq(vlan->v_name, "MacVlanDiscover");
	vlan = TAILQ_NEXT(vlan, v_entries);
	if (!vlan) {
		fail("no more VLAN");
		return;
	}
	ck_assert_int_eq(vlan->v_vid, 50);
	ck_assert_str_eq(vlan->v_name, "AdminB2");
	vlan = TAILQ_NEXT(vlan, v_entries);
	fail_unless(vlan == NULL);
#endif
}
END_TEST

#endif

Suite *
edp_suite(void)
{
	Suite *s = suite_create("EDP");

#ifdef ENABLE_EDP
	TCase *tc_send = tcase_create("Send EDP packets");
	TCase *tc_receive = tcase_create("Receive EDP packets");

	tcase_add_checked_fixture(tc_send, pcap_setup, pcap_teardown);
	tcase_add_test(tc_send, test_send_basic);
#ifdef ENABLE_DOT1
	tcase_add_test(tc_send, test_send_vlans);
#endif
	suite_add_tcase(s, tc_send);

	tcase_add_test(tc_receive, test_recv_edp);
	suite_add_tcase(s, tc_receive);
#endif

	return s;
}

int
main()
{
	int number_failed;
	Suite *s = edp_suite ();
	SRunner *sr = srunner_create (s);
	srunner_set_fork_status (sr, CK_NOFORK); /* Can't fork because
						    we need to write
						    files */
	srunner_run_all (sr, CK_ENV);
	number_failed = srunner_ntests_failed (sr);
	srunner_free (sr);
	return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
