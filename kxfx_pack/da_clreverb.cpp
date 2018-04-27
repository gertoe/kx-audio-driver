// kX Driver / kX Driver Interface / kX Driver Effects Library
// Copyright (c) Eugene Gavrilov, 2001-2005.
// All rights reserved

/*
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 */

// 10kX microcode
// Patch name: 'CL Reverb'

char *clreverb_copyright="Reverb: (c) E-mu Systems/Creative Technology, Ltd., 2000. Default parameters and Dane Source (c) Eugene Gavrilov, 2002-2004";
char *clreverb_engine="kX";
char *clreverb_comment="You should have original Creative Labs .DLL in order to use this effect legally; $nobypass";
char *clreverb_created="Feb 8 2002";
char *clreverb_guid="3EF83B67-66A6-4e30-90D6-8D11A6BD3D3A";

char *clreverb_name="CL Reverb";
int clreverb_itramsize=3760,clreverb_xtramsize=78154;

dsp_register_info clreverb_info[]={
	{ "in0",0x4000,0x7,0xffff,0x0 },
	{ "out0",0x8000,0x8,0xffff,0x0 },
	{ "out1",0x8001,0x8,0xffff,0x0 },
	{ "out2",0x8002,0x8,0xffff,0x0 },
	{ "out3",0x8003,0x8,0xffff,0x0 },
	{ "sti8004",0x8004,0x1,0xffff,0x800 },
	{ "sti8028",0x8005,0x1,0xffff,0x3e1f081 },
	{ "sti8029",0x8006,0x1,0xffff,0xd2cfa93 },
	{ "sti802a",0x8007,0x1,0xffff,0x33333333 },
	{ "sti802b",0x8008,0x1,0xffff,0x554fdf3a },
	{ "sti802c",0x8009,0x1,0xffff,0x554fdf3a },
	{ "sti802d",0x800a,0x1,0xffff,0x554fdf3a },
	{ "sti802e",0x800b,0x1,0xffff,0x554fdf3a },
	{ "sti802f",0x800c,0x1,0xffff,0x554fdf3a },
	{ "sti8030",0x800d,0x1,0xffff,0x554fdf3a },
	{ "sti8031",0x800e,0x1,0xffff,0x554fdf3a },
	{ "sti8032",0x800f,0x1,0xffff,0x554fdf3a },
	{ "sti8033",0x8010,0x1,0xffff,0x7e978d4f },
	{ "sti8034",0x8011,0x1,0xffff,0x3eb60f69 },
	{ "sti8035",0x8012,0x1,0xffff,0x54679c04 },
	{ "dyn8005",0x8013,0x1,0xffff,0x0 },
	{ "dyn8006",0x8014,0x1,0xffff,0x2794efdc },
	{ "dyn8007",0x8015,0x1,0xffff,0x2794f036 },
	{ "dyn8008",0x8016,0x1,0xffff,0x0 },
	{ "dyn8009",0x8017,0x1,0xffff,0x0 },
	{ "dyn800a",0x8018,0x1,0xffff,0x3324ef50 },
	{ "dyn800b",0x8019,0x1,0xffff,0x1c31a0c7 },
	{ "dyn800c",0x801a,0x1,0xffff,0x2eec134a },
	{ "dyn800d",0x801b,0x1,0xffff,0x147fff68 },
	{ "dyn800e",0x801c,0x1,0xffff,0x2380739c },
	{ "dyn800f",0x801d,0x1,0xffff,0x1a6aa06d },
	{ "dyn8010",0x801e,0x1,0xffff,0x1cbd7881 },
	{ "dyn8011",0x801f,0x1,0xffff,0x18975850 },
	{ "dyn8012",0x8020,0x1,0xffff,0x0 },
	{ "dyn8013",0x8021,0x1,0xffff,0x1d25b034 },
	{ "dyn8014",0x8022,0x1,0xffff,0x2674dde1 },
	{ "dyn8015",0x8023,0x1,0xffff,0x1f24711e },
	{ "dyn8016",0x8024,0x1,0xffff,0x28c226c9 },
	{ "dyn8017",0x8025,0x1,0xffff,0x23e6db29 },
	{ "dyn8018",0x8026,0x1,0xffff,0x2705b9fb },
	{ "dyn8019",0x8027,0x1,0xffff,0x26474104 },
	{ "dyn801a",0x8028,0x1,0xffff,0x2794f036 },
	{ "dyn801b",0x8029,0x1,0xffff,0xffffffff },
	{ "dyn801c",0x802a,0x1,0xffff,0xffffffff },
	{ "dyn801d",0x802b,0x1,0xffff,0xffffffff },
	{ "dyn801e",0x802c,0x1,0xffff,0xffffffff },
	{ "dyn801f",0x802d,0x1,0xffff,0xffffffff },
	{ "dyn8020",0x802e,0x1,0xffff,0xffffffff },
	{ "dyn8021",0x802f,0x1,0xffff,0xffffffff },
	{ "dyn8022",0x8030,0x1,0xffff,0xffffffff },
	{ "dyn8023",0x8031,0x1,0xffff,0x112843 },
	{ "dyn8024",0x8032,0x1,0xffff,0x0 },
	{ "dyn8025",0x8033,0x1,0xffff,0x601e0dcf },
	{ "dyn8026",0x8034,0x1,0xffff,0x7fffffff },
	{ "dyn8027",0x8035,0x1,0xffff,0x7ffffffe },
	{ "tmp8094",0x8036,0x3,0xffff,0x0 },
	{ "tmp8095",0x8037,0x3,0xffff,0x0 },
	{ "tmp8096",0x8038,0x3,0xffff,0x0 },
	{ "tmp8097",0x8039,0x3,0xffff,0x0 },
	{ "tmp8098",0x803a,0x3,0xffff,0x0 },
	{ "tmp8099",0x803b,0x3,0xffff,0x0 },
	{ "tmp809a",0x803c,0x3,0xffff,0x0 },
	{ "tmp809b",0x803d,0x3,0xffff,0x0 },
	{ "tmp809c",0x803e,0x3,0xffff,0x0 },
	{ "itr8036",0x803f,0x49,0xffff,0x0 },
	{ "&itr8036",0x8040,0xb,0xffff,0x0 },
	{ "itr8038",0x8041,0x29,0xffff,0xf0 },
	{ "&itr8038",0x8042,0xb,0xffff,0xf0 },
	{ "itr803a",0x8043,0x29,0xffff,0x1df },
	{ "&itr803a",0x8044,0xb,0xffff,0x1df },
	{ "itr803c",0x8045,0x29,0xffff,0x1df },
	{ "&itr803c",0x8046,0xb,0xffff,0x1df },
	{ "itr803e",0x8047,0x49,0xffff,0x1e0 },
	{ "&itr803e",0x8048,0xb,0xffff,0x1e0 },
	{ "itr8040",0x8049,0x29,0xffff,0x520 },
	{ "&itr8040",0x804a,0xb,0xffff,0x520 },
	{ "itr8042",0x804b,0x29,0xffff,0x3d5 },
	{ "&itr8042",0x804c,0xb,0xffff,0x3d5 },
	{ "itr8044",0x804d,0x29,0xffff,0x65f },
	{ "&itr8044",0x804e,0xb,0xffff,0x65f },
	{ "itr8046",0x804f,0x49,0xffff,0x660 },
	{ "&itr8046",0x8050,0xb,0xffff,0x660 },
	{ "itr8048",0x8051,0x29,0xffff,0xde0 },
	{ "&itr8048",0x8052,0xb,0xffff,0xde0 },
	{ "itr804a",0x8053,0x29,0xffff,0xbe7 },
	{ "&itr804a",0x8054,0xb,0xffff,0xbe7 },
	{ "itr804c",0x8055,0x29,0xffff,0xa5a },
	{ "&itr804c",0x8056,0xb,0xffff,0xa5a },
	{ "itr804e",0x8057,0x29,0xffff,0x923 },
	{ "&itr804e",0x8058,0xb,0xffff,0x923 },
	{ "itr8050",0x8059,0x29,0xffff,0x82e },
	{ "&itr8050",0x805a,0xb,0xffff,0x82e },
	{ "itr8052",0x805b,0x29,0xffff,0x76e },
	{ "&itr8052",0x805c,0xb,0xffff,0x76e },
	{ "itr8054",0x805d,0x29,0xffff,0x6d6 },
	{ "&itr8054",0x805e,0xb,0xffff,0x6d6 },
	{ "itr8056",0x805f,0x49,0xffff,0xdf1 },
	{ "&itr8056",0x8060,0xb,0xffff,0xdf1 },
	{ "itr8058",0x8061,0x49,0xffff,0xe45 },
	{ "&itr8058",0x8062,0xb,0xffff,0xe45 },
	{ "itr805a",0x8063,0x29,0xffff,0xeaf },
	{ "&itr805a",0x8064,0xb,0xffff,0xeaf },
	{ "itr805c",0x8065,0x29,0xffff,0xe44 },
	{ "&itr805c",0x8066,0xb,0xffff,0xe44 },
	{ "xtr805e",0x8067,0x4a,0xffff,0x0 },
	{ "&xtr805e",0x8068,0xb,0xffff,0x0 },
	{ "xtr8060",0x8069,0x2a,0xffff,0x552 },
	{ "&xtr8060",0x806a,0xb,0xffff,0x552 },
	{ "xtr8062",0x806b,0x4a,0xffff,0x553 },
	{ "&xtr8062",0x806c,0xb,0xffff,0x553 },
	{ "xtr8064",0x806d,0x2a,0xffff,0xa55 },
	{ "&xtr8064",0x806e,0xb,0xffff,0xa55 },
	{ "xtr8066",0x806f,0x4a,0xffff,0xa56 },
	{ "&xtr8066",0x8070,0xb,0xffff,0xa56 },
	{ "xtr8068",0x8071,0x2a,0xffff,0xf21 },
	{ "&xtr8068",0x8072,0xb,0xffff,0xf21 },
	{ "xtr806a",0x8073,0x4a,0xffff,0xf22 },
	{ "&xtr806a",0x8074,0xb,0xffff,0xf22 },
	{ "xtr806c",0x8075,0x2a,0xffff,0x14c1 },
	{ "&xtr806c",0x8076,0xb,0xffff,0x14c1 },
	{ "xtr806e",0x8077,0x4a,0xffff,0x14d2 },
	{ "&xtr806e",0x8078,0xb,0xffff,0x14d2 },
	{ "xtr8070",0x8079,0x2a,0xffff,0x4d12 },
	{ "&xtr8070",0x807a,0xb,0xffff,0x4d12 },
	{ "xtr8072",0x807b,0x2a,0xffff,0x5fd2 },
	{ "&xtr8072",0x807c,0xb,0xffff,0x5fd2 },
	{ "xtr8074",0x807d,0x4a,0xffff,0x5fe2 },
	{ "&xtr8074",0x807e,0xb,0xffff,0x5fe2 },
	{ "xtr8076",0x807f,0x4a,0xffff,0x8f03 },
	{ "&xtr8076",0x8080,0xb,0xffff,0x8f03 },
	{ "xtr8078",0x8081,0x4a,0xffff,0xa084 },
	{ "&xtr8078",0x8082,0xb,0xffff,0xa084 },
	{ "xtr807a",0x8083,0x4a,0xffff,0xc885 },
	{ "&xtr807a",0x8084,0xb,0xffff,0xc885 },
	{ "xtr807c",0x8085,0x4a,0xffff,0xd3d6 },
	{ "&xtr807c",0x8086,0xb,0xffff,0xd3d6 },
	{ "xtr807e",0x8087,0x4a,0xffff,0xf067 },
	{ "&xtr807e",0x8088,0xb,0xffff,0xf067 },
	{ "xtr8080",0x8089,0x4a,0xffff,0x102c0 },
	{ "&xtr8080",0x808a,0xb,0xffff,0x102c0 },
	{ "xtr8082",0x808b,0x4a,0xffff,0x11c99 },
	{ "&xtr8082",0x808c,0xb,0xffff,0x11c99 },
	{ "xtr8084",0x808d,0x2a,0xffff,0x8ef2 },
	{ "&xtr8084",0x808e,0xb,0xffff,0x8ef2 },
	{ "xtr8086",0x808f,0x2a,0xffff,0xa073 },
	{ "&xtr8086",0x8090,0xb,0xffff,0xa073 },
	{ "xtr8088",0x8091,0x2a,0xffff,0xc874 },
	{ "&xtr8088",0x8092,0xb,0xffff,0xc874 },
	{ "xtr808a",0x8093,0x2a,0xffff,0xd3c5 },
	{ "&xtr808a",0x8094,0xb,0xffff,0xd3c5 },
	{ "xtr808c",0x8095,0x2a,0xffff,0xf056 },
	{ "&xtr808c",0x8096,0xb,0xffff,0xf056 },
	{ "xtr808e",0x8097,0x2a,0xffff,0x102af },
	{ "&xtr808e",0x8098,0xb,0xffff,0x102af },
	{ "xtr8090",0x8099,0x2a,0xffff,0x11c88 },
	{ "&xtr8090",0x809a,0xb,0xffff,0x11c88 },
	{ "xtr8092",0x809b,0x2a,0xffff,0x13139 },
	{ "&xtr8092",0x809c,0xb,0xffff,0x13139 },
};

dsp_code clreverb_code[]={
	{ 0xe,0x8014,0x8015,0x8010,0x8014 },
	{ 0xe,0x8016,0x4000,0x8011,0x8016 },
	{ 0xe,0x8017,0x8016,0x8011,0x8017 },
	{ 0x0,0x8077,0x8017,0x8017,0x204d },
	{ 0x0,0x8036,0x2040,0x8079,0x8005 },
	{ 0x4,0x8036,0x2040,0x8036,0x2044 },
	{ 0x0,0x8047,0x8036,0x2040,0x2040 },
	{ 0x0,0x8037,0x2040,0x8069,0x204f },
	{ 0x0,0x8038,0x2040,0x8036,0x8008 },
	{ 0x0,0x8000,0x8037,0x8038,0x8007 },
	{ 0x1,0x8067,0x8038,0x8000,0x8007 },
	{ 0x0,0x8037,0x2040,0x806d,0x204f },
	{ 0x0,0x8038,0x2040,0x8049,0x8009 },
	{ 0x0,0x8001,0x8037,0x8038,0x8007 },
	{ 0x1,0x806b,0x8038,0x8001,0x8007 },
	{ 0x0,0x8037,0x2040,0x8071,0x204f },
	{ 0x0,0x8038,0x2040,0x804b,0x800a },
	{ 0x0,0x8002,0x8037,0x8038,0x8007 },
	{ 0x1,0x806f,0x8038,0x8002,0x8007 },
	{ 0x0,0x8037,0x2040,0x8075,0x204f },
	{ 0x0,0x8038,0x2040,0x804d,0x800b },
	{ 0x0,0x8003,0x8037,0x8038,0x8007 },
	{ 0x1,0x8073,0x8038,0x8003,0x8007 },
	{ 0x0,0x803d,0x2040,0x807b,0x8006 },
	{ 0x0,0x804f,0x803d,0x2040,0x2040 },
	{ 0x0,0x8033,0x8033,0x8031,0x8012 },
	{ 0x1,0x8012,0x8012,0x8031,0x8033 },
	{ 0x0,0x8037,0x2040,0x8051,0x8034 },
	{ 0x0,0x8039,0x2040,0x8053,0x8034 },
	{ 0x0,0x803b,0x2040,0x8055,0x8034 },
	{ 0x0,0x803e,0x2040,0x8057,0x8034 },
	{ 0x0,0x8036,0x8059,0x808d,0x8021 },
	{ 0x0,0x8037,0x8037,0x808f,0x8022 },
	{ 0x0,0x8038,0x805b,0x8091,0x8023 },
	{ 0x0,0x8039,0x8039,0x8093,0x8024 },
	{ 0x0,0x803a,0x805d,0x8095,0x8025 },
	{ 0x0,0x803b,0x803b,0x8097,0x8026 },
	{ 0x0,0x803f,0x803d,0x8099,0x8027 },
	{ 0xe,0x8013,0x8043,0x8013,0x8045 },
	{ 0x0,0x803d,0x803e,0x809b,0x8028 },
	{ 0xe,0x8029,0x8036,0x8018,0x8029 },
	{ 0xe,0x802a,0x8037,0x8019,0x802a },
	{ 0xe,0x802b,0x8038,0x801a,0x802b },
	{ 0xe,0x802c,0x8039,0x801b,0x802c },
	{ 0xe,0x802d,0x803a,0x801c,0x802d },
	{ 0xe,0x802e,0x803b,0x801d,0x802e },
	{ 0xe,0x802f,0x8013,0x801e,0x802f },
	{ 0xe,0x8030,0x803d,0x801f,0x8030 },
	{ 0x0,0x8036,0x8029,0x802a,0x8035 },
	{ 0x0,0x8038,0x802b,0x802c,0x8035 },
	{ 0x0,0x803a,0x802d,0x802e,0x8035 },
	{ 0x0,0x803c,0x802f,0x8030,0x8035 },
	{ 0x0,0x8037,0x8036,0x8038,0x8035 },
	{ 0x1,0x8039,0x8038,0x8036,0x8035 },
	{ 0x0,0x803b,0x803a,0x803c,0x8035 },
	{ 0x1,0x803d,0x803c,0x803a,0x8035 },
	{ 0x0,0x807f,0x8037,0x803b,0x8035 },
	{ 0x0,0x803e,0x2056,0x2040,0x2040 },
	{ 0x0,0x8000,0x8000,0x803e,0x800c },
	{ 0x1,0x8087,0x803b,0x8037,0x8035 },
	{ 0x0,0x8061,0x2056,0x2040,0x2040 },
	{ 0x0,0x8002,0x8002,0x8063,0x800e },
	{ 0x0,0x8083,0x8039,0x803d,0x8035 },
	{ 0x0,0x803e,0x2056,0x2040,0x2040 },
	{ 0x0,0x8001,0x8001,0x803e,0x800d },
	{ 0x1,0x808b,0x803d,0x8039,0x8035 },
	{ 0x0,0x805f,0x2056,0x2040,0x2040 },
	{ 0x0,0x8003,0x8003,0x8065,0x800f },
	{ 0x1,0x8037,0x802a,0x8029,0x8035 },
	{ 0x1,0x8039,0x802c,0x802b,0x8035 },
	{ 0x1,0x803b,0x802e,0x802d,0x8035 },
	{ 0x1,0x803d,0x8030,0x802f,0x8035 },
	{ 0x0,0x8036,0x8037,0x8039,0x8035 },
	{ 0x1,0x8038,0x8039,0x8037,0x8035 },
	{ 0x0,0x803a,0x803b,0x803d,0x8035 },
	{ 0x1,0x803c,0x803d,0x803b,0x8035 },
	{ 0x0,0x8081,0x8036,0x803a,0x8035 },
	{ 0x1,0x8089,0x803a,0x8036,0x8035 },
	{ 0x0,0x8085,0x8038,0x803c,0x8035 },
	{ 0x1,0x807d,0x803c,0x8038,0x8035 },
	{ 0x0,0x8013,0x8042,0x8033,0x8032 },
	{ 0x0,0x8044,0x8013,0x2040,0x2040 },
	{ 0x1,0x8046,0x8013,0x8004,0x204e },
	{ 0x5,0x8013,0x2040,0x8013,0x2055 },
};

