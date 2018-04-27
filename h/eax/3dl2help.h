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


// 3DL2HELP.H
//
#ifndef _3DL2HELP_H_INCLUDED
#define _3DL2HELP_H_INCLUDED
// Example listener environment presets
// for use with DSPROPERTY_I3DL2LISTENER_ALL.
// Each array contains 12 values whose order and types are defined
// by structure I3DL2_LISTENERPROPERTIES (defined in 3DL2.H).
#define I3DL2_ENVIRONMENT_PRESET_DEFAULT \
-10000, 0,0.0f, 1.00f,0.50f,-10000,0.020f,-10000,0.040f,100.0f,100.0f,5000.0f
#define I3DL2_ENVIRONMENT_PRESET_GENERIC \
-1000, -100,0.0f, 1.49f,0.83f, -2602,0.007f, 200,0.011f,100.0f,100.0f,5000.0f
#define I3DL2_ENVIRONMENT_PRESET_PADDEDCELL \
-1000,-6000,0.0f, 0.17f,0.10f, -1204,0.001f, 207,0.002f,100.0f,100.0f,5000.0f
#define I3DL2_ENVIRONMENT_PRESET_ROOM \
-1000, -454,0.0f, 0.40f,0.83f, -1646,0.002f, 53,0.003f,100.0f,100.0f,5000.0f
#define I3DL2_ENVIRONMENT_PRESET_BATHROOM \
-1000,-1200,0.0f, 1.49f,0.54f, -370,0.007f, 1030,0.011f,100.0f, 60.0f,5000.0f
#define I3DL2_ENVIRONMENT_PRESET_LIVINGROOM \
-1000,-6000,0.0f, 0.50f,0.10f, -1376,0.003f, -1104,0.004f,100.0f,100.0f,5000.0f
#define I3DL2_ENVIRONMENT_PRESET_STONEROOM \
-1000, -300,0.0f, 2.31f,0.64f, -711,0.012f, 83,0.017f,100.0f,100.0f,5000.0f
#define I3DL2_ENVIRONMENT_PRESET_AUDITORIUM \
-1000, -476,0.0f, 4.32f,0.59f, -789,0.020f, -289,0.030f,100.0f,100.0f,5000.0f
#define I3DL2_ENVIRONMENT_PRESET_CONCERTHALL \
-1000, -500,0.0f, 3.92f,0.70f, -1230,0.020f, -2,0.029f,100.0f,100.0f,5000.0f
#define I3DL2_ENVIRONMENT_PRESET_CAVE \
-1000, 0,0.0f, 2.91f,1.30f, -602,0.015f, -302,0.022f,100.0f,100.0f,5000.0f
#define I3DL2_ENVIRONMENT_PRESET_ARENA \
-1000, -698,0.0f, 7.24f,0.33f, -1166,0.020f, 16,0.030f,100.0f,100.0f,5000.0f
#define I3DL2_ENVIRONMENT_PRESET_HANGAR \
-1000,-1000,0.0f,10.05f,0.23f, -602,0.020f, 198,0.030f,100.0f,100.0f,5000.0f
#define I3DL2_ENVIRONMENT_PRESET_CARPETEDHALLWAY \
-1000,-4000,0.0f, 0.30f,0.10f, -1831,0.002f, -1630,0.030f,100.0f,100.0f,5000.0f
#define I3DL2_ENVIRONMENT_PRESET_HALLWAY \
-1000, -300,0.0f, 1.49f,0.59f, -1219,0.007f, 441,0.011f,100.0f,100.0f,5000.0f
#define I3DL2_ENVIRONMENT_PRESET_STONECORRIDOR \
-1000, -237,0.0f, 2.70f,0.79f, -1214,0.013f, 395,0.020f,100.0f,100.0f,5000.0f
#define I3DL2_ENVIRONMENT_PRESET_ALLEY \
-1000, -270,0.0f, 1.49f,0.86f, -1204,0.007f, -4,0.011f,100.0f,100.0f,5000.0f
#define I3DL2_ENVIRONMENT_PRESET_FOREST \
-1000,-3300,0.0f, 1.49f,0.54f, -2560,0.162f, -613,0.088f, 79.0f,100.0f,5000.0f
#define I3DL2_ENVIRONMENT_PRESET_CITY \
-1000, -800,0.0f, 1.49f,0.67f, -2273,0.007f, -2217,0.011f, 50.0f,100.0f,5000.0f
#define I3DL2_ENVIRONMENT_PRESET_MOUNTAINS \
-1000,-2500,0.0f, 1.49f,0.21f, -2780,0.300f, -2014,0.100f, 27.0f,100.0f,5000.0f
#define I3DL2_ENVIRONMENT_PRESET_QUARRY \
-1000,-1000,0.0f, 1.49f,0.83f,-10000,0.061f, 500,0.025f,100.0f,100.0f,5000.0f
#define I3DL2_ENVIRONMENT_PRESET_PLAIN \
-1000,-2000,0.0f, 1.49f,0.50f, -2466,0.179f, -2514,0.100f, 21.0f,100.0f,5000.0f
#define I3DL2_ENVIRONMENT_PRESET_PARKINGLOT \
-1000, 0,0.0f, 1.65f,1.50f, -1363,0.008f, -1153,0.012f,100.0f,100.0f,5000.0f
#define I3DL2_ENVIRONMENT_PRESET_SEWERPIPE \
-1000,-1000,0.0f, 2.81f,0.14f, 429,0.014f, 648,0.021f, 80.0f, 60.0f,5000.0f
#define I3DL2_ENVIRONMENT_PRESET_UNDERWATER \
-1000,-4000,0.0f, 1.49f,0.10f, -449,0.007f, 1700,0.011f,100.0f,100.0f,5000.0f
// Examples simulating General MIDI 2 reverb presets
//
// Name (Decay time) Description
//
// Small Room (1.1s) A small size room with a length of 5m or so.
// Medium Room (1.3s) A medium size room with a length of 10m or so.
// Large Room (1.5s) A large size room suitable for live performances.
// Medium Hall (1.8s) A medium size concert hall.
// Large Hall (1.8s) A large size concert hall suitable for a full orchestra.
// Plate (1.3s) A plate reverb simulation.
#define I3DL2_ENVIRONMENT_PRESET_SMALLROOM \
-1000, -600,0.0f, 1.10f,0.83f, -400,0.005f, 500,0.010f,100.0f,100.0f,5000.0f
#define I3DL2_ENVIRONMENT_PRESET_MEDIUMROOM \
-1000, -600,0.0f, 1.30f,0.83f, -1000,0.010f, -200,0.020f,100.0f,100.0f,5000.0f
#define I3DL2_ENVIRONMENT_PRESET_LARGEROOM \
-1000, -600,0.0f, 1.50f,0.83f, -1600,0.020f, -1000,0.040f,100.0f,100.0f,5000.0f
#define I3DL2_ENVIRONMENT_PRESET_MEDIUMHALL \
-1000, -600,0.0f, 1.80f,0.70f, -1300,0.015f, -800,0.030f,100.0f,100.0f,5000.0f
#define I3DL2_ENVIRONMENT_PRESET_LARGEHALL \
-1000, -600,0.0f, 1.80f,0.70f, -2000,0.030f, -1400,0.060f,100.0f,100.0f,5000.0f
#define I3DL2_ENVIRONMENT_PRESET_PLATE \
-1000, -200,0.0f, 1.30f,0.90f, 0,0.002f, 0,0.010f,100.0f, 75.0f,5000.0f
// Example material transmission presets
// for use with DSPROPERTY_I3DL2BUFFER_OCCLUSIONALL
// or DSPROPERTY_I3DL2BUFFER_OBSTRUCTIONALL.
// Each array contains 2 values whose order and types are defined
// by structure I3DL2_OCCLUSIONPROPERTIES
// or I3DL2_OBSTRUCTIONPROPERTIES (defined in 3DL2.H).
#define I3DL2_MATERIAL_PRESET_SINGLEWINDOW -2800,0.71f
#define I3DL2_MATERIAL_PRESET_DOUBLEWINDOW -5000,0.40f
#define I3DL2_MATERIAL_PRESET_THINDOOR -1800,0.66f
#define I3DL2_MATERIAL_PRESET_THICKDOOR -4400,0.64f
#define I3DL2_MATERIAL_PRESET_WOODWALL -4000,0.50f
#define I3DL2_MATERIAL_PRESET_BRICKWALL -5000,0.60f
#define I3DL2_MATERIAL_PRESET_STONEWALL -6000,0.68f
#define I3DL2_MATERIAL_PRESET_CURTAIN -1200,0.15f
#endif
