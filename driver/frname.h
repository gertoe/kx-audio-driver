// inlined

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

#include "kx.h"

//i don't really get why he used all those references while he just needed a reference to the kx object - ITzTravelIntime
/*int kx_get_friendly_name(const unsigned int device, const unsigned int subsys, const unsigned int chiprev,
						 char *tmp_str,
						 unsigned char &is_51,
						 unsigned char &has_surdac,
						 unsigned char &is_aps,
						 unsigned char &is_10k2,
						 unsigned char &is_a2,
						 unsigned char &is_a2ex,
						 unsigned char &is_k8,
						 unsigned char &is_a4,
						 unsigned char &is_edsp,
						 char &have_ac97,
						 char &lack_ac97,
						 unsigned char &is_zsnb,
						 unsigned char &is_cardbus);
*/
static int kx_get_friendly_name(const unsigned int device, const unsigned int subsys, const unsigned int chiprev,
		char *tmp_str,
        char *tmp_model_str,
		unsigned char &is_51,
		unsigned char &has_surdac,
 		unsigned char &is_aps,
 		unsigned char &is_10k2,
 		unsigned char &is_a2,
 		unsigned char &is_a2ex,
 		unsigned char &is_k8,
        unsigned char &is_a4,
 		unsigned char &is_edsp,
 		char &have_ac97,
 		char &lack_ac97,
 		unsigned char &is_zsnb,
        unsigned char &is_cardbus)
{
 int ret=0;

 lack_ac97=0;
 have_ac97=0; // autodetect

 if(device==0x00041102) // 10k2
   is_10k2=1;

 if(device==0x00041102 && chiprev==4)  // a2
   is_a2=1;

 if(device==0x00081102) // a2value, ... with p17v
 {
   is_10k2=1;
   is_a2=1;
   is_k8=1;

   // this is assumed:
   // if((subsys&0xf0000000)==0x10000000) // any ca0108
   // is_k8=1;

   if((subsys&0xf0000000)==0x20000000) // any zsnb
    is_cardbus=1;

   if((subsys&0xf0000000)==0x40000000) // any e-dsp v2
    is_edsp=1;

   if((subsys&0xff000000)==0x42000000) // any e-dsp v2 cardbus
    is_cardbus=1;
 }

 if(is_edsp)
   lack_ac97=1;

 if(is_10k2)
   is_51=1; // assume 10k2 is always at least '5.1'

 switch(subsys)
 {
      case 0x40011102:
       if(is_10k2==0) // [1]
       {
        //this card is rare, be proud of owning one
           _kx_strcpy(tmp_str,"E-MU APS",KX_MAX_STRING);
           _kx_strcpy(tmp_model_str,"KXProject_EMU_APS",KX_MAX_STRING);
        
        is_aps=1;
        lack_ac97=1;
       }
       else
       {
        //strncpy(tmp_str,"EM8810 10k2",KX_MAX_STRING); // [110] - 1010 v1
           _kx_strcpy(tmp_str,"E-MU E-1010 10k2",KX_MAX_STRING);
           _kx_strcpy(tmp_model_str,"KXProject_EMU_E-1010_10k2",KX_MAX_STRING);
        is_edsp=1;
        lack_ac97=1;
       }
       break;

     case 0x00201102: // [2]
       //strncpy(tmp_str,"CT4670 10k1",KX_MAX_STRING); // live! value
         _kx_strcpy(tmp_str,"SB Live!",KX_MAX_STRING);
         _kx_strcpy(tmp_model_str,"KXProject_CT4670",KX_MAX_STRING);
       break;
     case 0x00211102: // [3]
       //strncpy(tmp_str,"CT4620 10k1",KX_MAX_STRING); // live! standard
         _kx_strcpy(tmp_str,"SB Live!",KX_MAX_STRING);
         _kx_strcpy(tmp_model_str,"KXProject_CT4620",KX_MAX_STRING);
       break;
     case 0x80221102: // [4]
       //strncpy(tmp_str,"CT4780 10k1",KX_MAX_STRING); // value digital
         _kx_strcpy(tmp_str,"SB Live!",KX_MAX_STRING);
         _kx_strcpy(tmp_model_str,"KXProject_CT4680",KX_MAX_STRING);
       break;
     case 0x80231102: // [5]
       //strncpy(tmp_str,"CT4790 10k1",KX_MAX_STRING); // 512
         _kx_strcpy(tmp_str,"SB PCI 512",KX_MAX_STRING);
         _kx_strcpy(tmp_model_str,"KXProject_CT4970",KX_MAX_STRING);
       break;
     case 0x80251102: // sb0040 ? too // [6]
//       strncpy(tmp_str,"CT1140 10k1",KX_MAX_STRING); // MB
         _kx_strcpy(tmp_str,"SB Live!",KX_MAX_STRING);
         _kx_strcpy(tmp_model_str,"KXProject_CT1140",KX_MAX_STRING);
       break;
     case 0x80261102: // [7]
//       strncpy(tmp_str,"CT4830 10k1",KX_MAX_STRING); // value digital
         _kx_strcpy(tmp_str,"SB Live!",KX_MAX_STRING);
         _kx_strcpy(tmp_model_str,"KXProject_CT4830",KX_MAX_STRING);
       break;
     case 0x80271102: // [8]
       //strncpy(tmp_str,"CT4832 10k1",KX_MAX_STRING); // value digital
         _kx_strcpy(tmp_str,"SB Live!",KX_MAX_STRING);
         _kx_strcpy(tmp_model_str,"KXProject_CT4832",KX_MAX_STRING);
       break;
     case 0x80281102: // [9]
       //strncpy(tmp_str,"CT4870 10k1",KX_MAX_STRING); // value
         _kx_strcpy(tmp_str,"SB Live!",KX_MAX_STRING);
         _kx_strcpy(tmp_model_str,"KXProject_CT4870",KX_MAX_STRING);
       break;
     case 0x80291102: // [10]
       //strncpy(tmp_str,"CT4872 10k1",KX_MAX_STRING); // value
         _kx_strcpy(tmp_str,"SB Live!",KX_MAX_STRING);
         _kx_strcpy(tmp_model_str,"KXProject_CT4872",KX_MAX_STRING);
       break;
     case 0x802a1102: // [11]
       //strncpy(tmp_str,"CT4890 10k1",KX_MAX_STRING); // k256
         _kx_strcpy(tmp_str,"SB PCI",KX_MAX_STRING);
         _kx_strcpy(tmp_model_str,"KXProject_CT4890",KX_MAX_STRING);
       break;
     case 0x802b1102: // [12]
       //strncpy(tmp_str,"CT4891 10k1",KX_MAX_STRING); // 256
         
         _kx_strcpy(tmp_str,"SB PCI",KX_MAX_STRING);
         _kx_strcpy(tmp_model_str,"KXProject_CT4891",KX_MAX_STRING);
       break;
     case 0x002F1102: // M002/M003 // [13]
         _kx_strcpy(tmp_str,"CTM002 10k1",KX_MAX_STRING); // MB
         _kx_strcpy(tmp_model_str,"KXProject_M002_M003",KX_MAX_STRING);
       break;
     case 0x80311102: // [14]
       //strncpy(tmp_str,"CT4831 10k1",KX_MAX_STRING); // value
         _kx_strcpy(tmp_str,"SB Live!",KX_MAX_STRING);
         _kx_strcpy(tmp_model_str,"KXProject_CT4831",KX_MAX_STRING);
       break;
     case 0x80321102: // [15]
       //strncpy(tmp_str,"CT4871 10k1",KX_MAX_STRING); // value
         _kx_strcpy(tmp_str,"SB Live!",KX_MAX_STRING);
         _kx_strcpy(tmp_model_str,"KXProject_CT4871",KX_MAX_STRING);
       break;
     case 0x80331102: // [16]
       //strncpy(tmp_str,"CT4893 10k1",KX_MAX_STRING); // k256
         _kx_strcpy(tmp_str,"SB PCI",KX_MAX_STRING);
         _kx_strcpy(tmp_model_str,"KXProject_CT4893",KX_MAX_STRING);
       break;
     case 0x80351102: // [17]
       //strncpy(tmp_str,"CT0060 10k1",KX_MAX_STRING);
         
         _kx_strcpy(tmp_str,"SB Live! 5.1",KX_MAX_STRING);
         _kx_strcpy(tmp_model_str,"KXProject_SB0060",KX_MAX_STRING);
         
       is_51=1;
       has_surdac=1;
       break;
     case 0x80401102: // [18]
       //strncpy(tmp_str,"CT4760 10k1",KX_MAX_STRING); // live digital entertainment/platinum
         _kx_strcpy(tmp_str,"SB Live! Platinum",KX_MAX_STRING);
         _kx_strcpy(tmp_model_str,"KXProject_CT4760",KX_MAX_STRING);
       break;

     case 0x80501102: // [19]
       //strncpy(tmp_str,"CT4750 10k1",KX_MAX_STRING); // was 4820?! k512
         _kx_strcpy(tmp_str,"SB PCI",KX_MAX_STRING);
         _kx_strcpy(tmp_model_str,"KXProject_CT4760",KX_MAX_STRING);
       break;
     case 0x80511102: // [20]
       //strncpy(tmp_str,"CT4850 10k1",KX_MAX_STRING);
         _kx_strcpy(tmp_str,"SB Live!",KX_MAX_STRING);
         _kx_strcpy(tmp_model_str,"KXProject_CT4850",KX_MAX_STRING);
       break;

     case 0x80611102: // [21]
       //strncpy(tmp_str,"SB0060 10k1",KX_MAX_STRING);  // live 5.1
         _kx_strcpy(tmp_str,"SB Live! 5.1",KX_MAX_STRING);
         _kx_strcpy(tmp_model_str,"KXProject_SB0060",KX_MAX_STRING);
       is_51=1;
       has_surdac=1;
       break;
     case 0x80621102: // [22]
       //strncpy(tmp_str,"SB0100 10k1",KX_MAX_STRING);  // live 5.1
         _kx_strcpy(tmp_str,"SB Live! 5.1",KX_MAX_STRING);
         _kx_strcpy(tmp_model_str,"KXProject_SB0100",KX_MAX_STRING);
       is_51=1;
       has_surdac=1;
       break;
     case 0x80631102: // [23]
         _kx_strcpy(tmp_str,"DXW MB",KX_MAX_STRING);
         _kx_strcpy(tmp_model_str,"KXProject_DXW_MB",KX_MAX_STRING);
       break;
     case 0x80641102: // [24]
       //strncpy(tmp_str,"SB0102 10k1",KX_MAX_STRING);  // live 5.1
         _kx_strcpy(tmp_str,"SB Live! 5.1",KX_MAX_STRING);
         _kx_strcpy(tmp_model_str,"KXProject_SB0102",KX_MAX_STRING);
       is_51=1;
       has_surdac=1;
       break;
     case 0x80651102: // [25]
       //strncpy(tmp_str,"SB0222 10k1",KX_MAX_STRING);
         _kx_strcpy(tmp_str,"SB Live! 5.1",KX_MAX_STRING);
         _kx_strcpy(tmp_model_str,"KXProject_SB0222",KX_MAX_STRING);
       is_51=1;
       has_surdac=1;
       break;
     case 0x80661102: // [33]
       //strncpy(tmp_str,"SB0228 10k1",KX_MAX_STRING); // Russ's
         _kx_strcpy(tmp_str,"SB Live! 5.1",KX_MAX_STRING);
         _kx_strcpy(tmp_model_str,"KXProject_SB0228",KX_MAX_STRING);
       is_51=1;
       has_surdac=1;
       break;
     case 0x100a1102:
       if(!is_10k2)
       {
           if(chiprev==0xa){
               //strncpy(tmp_str,"SB0229 10k1",KX_MAX_STRING); // [34] - LS
               _kx_strcpy(tmp_str,"SB Live! 5.1",KX_MAX_STRING);
               _kx_strcpy(tmp_model_str,"KXProject_SB0229",KX_MAX_STRING);
           }else{
               //strncpy(tmp_str,"SB022b 10k1",KX_MAX_STRING); // [33]
               _kx_strcpy(tmp_str,"SB Live! 5.1",KX_MAX_STRING);
               _kx_strcpy(tmp_model_str,"KXProject_SB022b",KX_MAX_STRING);
           }

        is_51=1;
        has_surdac=1;
       }
       else
       {
         //strncpy(tmp_str,"SB0246 10k2",KX_MAX_STRING); // Audigy2 gateway oem [88]
           _kx_strcpy(tmp_str,"SB Audigy 2",KX_MAX_STRING);
           _kx_strcpy(tmp_model_str,"KXProject_SB0246",KX_MAX_STRING);
       }
       break;
     case 0x80671102: // [32]
       //strncpy(tmp_str,"SB0220 10k1",KX_MAX_STRING);
         _kx_strcpy(tmp_str,"SB Live! 5.1",KX_MAX_STRING);
         _kx_strcpy(tmp_model_str,"KXProject_SB0220",KX_MAX_STRING);
       is_51=1;
       has_surdac=1;
       break;
     case 0x80681102: // [26]
       //strncpy(tmp_str,"CT0061 10k1",KX_MAX_STRING);
         _kx_strcpy(tmp_str,"SB Live! 5.1",KX_MAX_STRING);
         _kx_strcpy(tmp_model_str,"KXProject_CT0061",KX_MAX_STRING);
       is_51=1;
       has_surdac=1;
       break;
     case 0x80691102: // [27]
       //strncpy(tmp_str,"SB0101 10k1",KX_MAX_STRING); // value
         _kx_strcpy(tmp_str,"SB Live! 5.1",KX_MAX_STRING);
         _kx_strcpy(tmp_model_str,"KXProject_SB0101",KX_MAX_STRING);
       is_51=1;
       has_surdac=1;
       break;
     case 0x806a1102: // [28]
       //strncpy(tmp_str,"SB0103 10k1",KX_MAX_STRING);
         _kx_strcpy(tmp_str,"SB Live! 5.1",KX_MAX_STRING);
         _kx_strcpy(tmp_model_str,"KXProject_SB0103",KX_MAX_STRING);
       is_51=1;
       has_surdac=1;
       break;
     case 0x806b1102: // [29]
       //strncpy(tmp_str,"SB0105 10k1",KX_MAX_STRING);
         _kx_strcpy(tmp_str,"SB Live! 5.1",KX_MAX_STRING);
         _kx_strcpy(tmp_model_str,"KXProject_SB0105",KX_MAX_STRING);
       is_51=1;
       has_surdac=1;
       break;
     case 0x806C1102: // [30]
       //strncpy(tmp_str,"SB0221 10k1",KX_MAX_STRING);
         _kx_strcpy(tmp_str,"SB Live! 5.1",KX_MAX_STRING);
         _kx_strcpy(tmp_model_str,"KXProject_SB0221",KX_MAX_STRING);
       is_51=1;
       has_surdac=1;
       break;
     case 0x80711102: // [31]
       //strncpy(tmp_str,"SB0150 10k1",KX_MAX_STRING); // PCI512
         _kx_strcpy(tmp_str,"SB PCI 512",KX_MAX_STRING);
         _kx_strcpy(tmp_model_str,"KXProject_SB0150",KX_MAX_STRING);
       is_51=1;
       has_surdac=1;
       break;

     case 0x00401102: // ?.. dead audigy [50]
       //strncpy(tmp_str,"SB0090 10k2",KX_MAX_STRING); // audigy
         _kx_strcpy(tmp_str,"SB Audigy",KX_MAX_STRING);
         _kx_strcpy(tmp_model_str,"KXProject_SB0090",KX_MAX_STRING);
       break;
     case 0x00411102: // PCI512
       //strncpy(tmp_str,"CT4820 10k2",KX_MAX_STRING); // [51]
         _kx_strcpy(tmp_str,"SB Live! 2",KX_MAX_STRING);
         _kx_strcpy(tmp_model_str,"KXProject_CT4820",KX_MAX_STRING);
       break;
     case 0x00421102:
       //strncpy(tmp_str,"CT0070 10k2",KX_MAX_STRING); // [52]
         _kx_strcpy(tmp_str,"SB Audigy",KX_MAX_STRING);
         _kx_strcpy(tmp_model_str,"KXProject_CT0070",KX_MAX_STRING);
       break;
     case 0x00431102:
       //strncpy(tmp_str,"CT0072 10k2",KX_MAX_STRING); // ct0073? // [53]
         _kx_strcpy(tmp_str,"SB Audigy",KX_MAX_STRING);
         _kx_strcpy(tmp_model_str,"KXProject_CT0072",KX_MAX_STRING);
       break;
     case 0x00511102: // [54]
       //strncpy(tmp_str,"SB0090 10k2",KX_MAX_STRING); // audigy with golden connectors
         _kx_strcpy(tmp_str,"SB Audigy",KX_MAX_STRING);
         _kx_strcpy(tmp_model_str,"KXProject_SB0090",KX_MAX_STRING);
       break;
     case 0x00521102: // [55]
       //strncpy(tmp_str,"SB0162 10k2",KX_MAX_STRING); // audigy: plastic + gameport (no 1394)
       				      // unknown: sb0162 -- audigy w/midi jack + plastic jacks
         _kx_strcpy(tmp_str,"SB Audigy",KX_MAX_STRING);
         _kx_strcpy(tmp_model_str,"KXProject_SB0162",KX_MAX_STRING);
       break;
     case 0x00531102: // [56]
       //strncpy(tmp_str,"SB0092 10k2",KX_MAX_STRING); // audigy OEM / Player SB0092
         
         _kx_strcpy(tmp_str,"SB Audigy",KX_MAX_STRING);
         _kx_strcpy(tmp_model_str,"KXProject_SB0092",KX_MAX_STRING);
         
       break;
     case 0x00541102: // [57]
       //strncpy(tmp_str,"SB0161 10k2",KX_MAX_STRING); // audigy
         
         _kx_strcpy(tmp_str,"SB Audigy",KX_MAX_STRING);
         _kx_strcpy(tmp_model_str,"KXProject_SB0161",KX_MAX_STRING);
         
       break;
     case 0x00551102: // [58]
       //strncpy(tmp_str,"SB0192 10k2",KX_MAX_STRING); // audigy
         
         _kx_strcpy(tmp_str,"SB Audigy",KX_MAX_STRING);
         _kx_strcpy(tmp_model_str,"KXProject_SB0192",KX_MAX_STRING);
         
       break;
     case 0x00561102: // [59]
       //strncpy(tmp_str,"SB0191 10k2",KX_MAX_STRING); // audigy
         
         _kx_strcpy(tmp_str,"SB Audigy",KX_MAX_STRING);
         _kx_strcpy(tmp_model_str,"KXProject_SB0191",KX_MAX_STRING);
         
       break;
     case 0x00571102: // [60]
       //strncpy(tmp_str,"SB0091 10k2",KX_MAX_STRING); // audigy
         
         _kx_strcpy(tmp_str,"SB Audigy",KX_MAX_STRING);
         _kx_strcpy(tmp_model_str,"KXProject_SB0091",KX_MAX_STRING);
         
       break;
     case 0x00581102: // [61]
//       strncpy(tmp_str,"SB0095 10k2",KX_MAX_STRING); // audigy
         
         _kx_strcpy(tmp_str,"SB Audigy",KX_MAX_STRING);
         _kx_strcpy(tmp_model_str,"KXProject_SB0095",KX_MAX_STRING);
         
       break;
     case 0x00591102: // [62]
//       strncpy(tmp_str,"SB0230 10k2",KX_MAX_STRING); // audigy
         
         _kx_strcpy(tmp_str,"SB Audigy",KX_MAX_STRING);
         _kx_strcpy(tmp_model_str,"KXProject_SB0230",KX_MAX_STRING);
         
       break;
     case 0x005A1102: // [63]
//       strncpy(tmp_str,"SB0231 10k2",KX_MAX_STRING); // audigy
         _kx_strcpy(tmp_str,"SB Audigy",KX_MAX_STRING);
         _kx_strcpy(tmp_model_str,"KXProject_SB0231",KX_MAX_STRING);
       break;
     case 0x005B1102: // [64]
//       strncpy(tmp_str,"SB0232 10k2",KX_MAX_STRING); // audigy
         _kx_strcpy(tmp_str,"SB Audigy",KX_MAX_STRING);
         _kx_strcpy(tmp_model_str,"KXProject_SB0232",KX_MAX_STRING);
       break;
     case 0x005C1102: // [65]
//       strncpy(tmp_str,"SB0238 10k2",KX_MAX_STRING); // audigy
         
         _kx_strcpy(tmp_str,"SB Audigy",KX_MAX_STRING);
         _kx_strcpy(tmp_model_str,"KXProject_SB0238",KX_MAX_STRING);
       break;

     case 0x10021102: // [80]
       //strncpy(tmp_str,"SB0240 10k2",KX_MAX_STRING); // Audigy2
         
         _kx_strcpy(tmp_str,"SB Audigy 2",KX_MAX_STRING);
         _kx_strcpy(tmp_model_str,"KXProject_SB0240",KX_MAX_STRING);
         
       break;
     case 0x10031102: // [81]
       //strncpy(tmp_str,"SB0243 10k2",KX_MAX_STRING); // Audigy2
         
         _kx_strcpy(tmp_str,"SB Audigy 2",KX_MAX_STRING);
         _kx_strcpy(tmp_model_str,"KXProject_SB0243",KX_MAX_STRING);
         
       break;
     case 0x10041102: // [82]
       //strncpy(tmp_str,"SB0242 10k2",KX_MAX_STRING); // Audigy2
         
         _kx_strcpy(tmp_str,"SB Audigy 2",KX_MAX_STRING);
         _kx_strcpy(tmp_model_str,"KXProject_SB0242",KX_MAX_STRING);
         
       break;
     case 0x10051102: // [83]
       //strncpy(tmp_str,"SB0280 10k2",KX_MAX_STRING); // Audigy2 Platinum Ex; sb244, sb250, sb290
         
         _kx_strcpy(tmp_str,"SB Audigy 2",KX_MAX_STRING);
         _kx_strcpy(tmp_model_str,"KXProject_SB0280",KX_MAX_STRING);
         
       is_a2ex=1;
       lack_ac97=1;
       break;
     case 0x10061102: // [86]
       //strncpy(tmp_str,"SB0245 10k2",KX_MAX_STRING); // audigy2 gateway oem
         
         _kx_strcpy(tmp_str,"SB Audigy 2",KX_MAX_STRING);
         _kx_strcpy(tmp_model_str,"KXProject_SB0245",KX_MAX_STRING);
         
       break;
     case 0x10071102: // [84]
       //strncpy(tmp_str,"SB0244 10k2",KX_MAX_STRING); // audigy2 oem [riva's]
         
         _kx_strcpy(tmp_str,"SB Audigy 2",KX_MAX_STRING);
         _kx_strcpy(tmp_model_str,"KXProject_SB0244",KX_MAX_STRING);
         
       break;
     case 0x10081102: // [85]
       //strncpy(tmp_str,"SB0320 10k2",KX_MAX_STRING); // Audigy2 Platinum Ex (with ac97 codec!) + sb290
         
         _kx_strcpy(tmp_str,"SB Audigy 2 Platinum",KX_MAX_STRING);
         _kx_strcpy(tmp_model_str,"KXProject_SB0320",KX_MAX_STRING);
         
       is_a2ex=1;
       break;
     case 0x10091102: // [87]
       //strncpy(tmp_str,"SB0249 10k2",KX_MAX_STRING); // Audigy2 unk
         
         _kx_strcpy(tmp_str,"SB Audigy 2",KX_MAX_STRING);
         _kx_strcpy(tmp_model_str,"KXProject_SB0249",KX_MAX_STRING);
         
       break;

     case 0x20011102: // [101]
       if(is_k8)
       {
        //strncpy(tmp_str,"SB0530 10k8",KX_MAX_STRING); // audigy2 ZS Notebook
           
           _kx_strcpy(tmp_str,"SB Audigy 2 ZS",KX_MAX_STRING);
           _kx_strcpy(tmp_model_str,"KXProject_SB0530",KX_MAX_STRING);
           
        is_zsnb=1;
        is_cardbus=1;
        lack_ac97=1;
       }
       else // [90]
       {
        //strncpy(tmp_str,"SB0360 10k2",KX_MAX_STRING); // audigy2 ZS Platinum Pro + sb290
           //expensive stuff
           _kx_strcpy(tmp_str,"SB Audigy 2 ZS Platinum Pro",KX_MAX_STRING);
           _kx_strcpy(tmp_model_str,"KXProject_SB0360",KX_MAX_STRING);
           
        is_a2ex=1;
       }
       break;
     case 0x20211102: // [105]
       if(is_k8)
       {
        //strncpy(tmp_str,"SB2021 10k8",KX_MAX_STRING); // audigy2 zs notebook [?]
           
           _kx_strcpy(tmp_str,"SB Audigy 2 ZS",KX_MAX_STRING);
           _kx_strcpy(tmp_model_str,"KXProject_SB2021",KX_MAX_STRING);
           
        is_zsnb=1;
        is_cardbus=1;
        lack_ac97=1;
       }
       break;
     case 0x20021102: // [91]
       //strncpy(tmp_str,"SB0350 10k2",KX_MAX_STRING); // audigy2 ZS retail + sb250
         
         _kx_strcpy(tmp_str,"SB Audigy 2 ZS",KX_MAX_STRING);
         _kx_strcpy(tmp_model_str,"KXProject_SB0350",KX_MAX_STRING);
         
       break;
     case 0x20031102: // [92]
       //strncpy(tmp_str,"SB0352 10k2",KX_MAX_STRING); // audigy2 ZS ?
         
         _kx_strcpy(tmp_str,"SB Audigy 2 ZS",KX_MAX_STRING);
         _kx_strcpy(tmp_model_str,"KXProject_SB0352",KX_MAX_STRING);
         
       break;
     case 0x20041102: // [95]
       //strncpy(tmp_str,"SB0355 10k2",KX_MAX_STRING); // Audigy2 zs sb0350/sb0355 / Gateway OEM
         
         _kx_strcpy(tmp_str,"SB Audigy 2 ZS",KX_MAX_STRING);
         _kx_strcpy(tmp_model_str,"KXProject_SB0355",KX_MAX_STRING);
         
       break;
     case 0x20051102: // [96]
       //strncpy(tmp_str,"SB0350 10k2",KX_MAX_STRING); // Audigy2 zs SB0350/359
         
         _kx_strcpy(tmp_str,"SB Audigy 2 ZS",KX_MAX_STRING);
         _kx_strcpy(tmp_model_str,"KXProject_SB0350",KX_MAX_STRING);
         
       break;
     case 0x20061102: // [94]
       //strncpy(tmp_str,"SB035x 10k2",KX_MAX_STRING);
         
         _kx_strcpy(tmp_str,"SB Audigy 2 ZS",KX_MAX_STRING);
         _kx_strcpy(tmp_model_str,"KXProject_SB035x",KX_MAX_STRING);
         
       break;
     case 0x20071102: // [93]
       //strncpy(tmp_str,"SB0380 10k2",KX_MAX_STRING); // audigy4 PRO
         
         //even more expensive stuff
         _kx_strcpy(tmp_str,"SB Audigy 4 Pro",KX_MAX_STRING);
         _kx_strcpy(tmp_model_str,"KXProject_SB0380",KX_MAX_STRING);
         
       break;

     case 0x10001102: // [103]
       //strncpy(tmp_str,"SB1000 10k2",KX_MAX_STRING); // audigy2 value [?]
         
         _kx_strcpy(tmp_str,"SB Audigy 2",KX_MAX_STRING);
         _kx_strcpy(tmp_model_str,"KXProject_SB0100",KX_MAX_STRING);
         
       break;
     case 0x10011102: // [100]
       //strncpy(tmp_str,"SB0400 10k2",KX_MAX_STRING); // audigy2 value
         
         _kx_strcpy(tmp_str,"SB Audigy 2",KX_MAX_STRING);
         _kx_strcpy(tmp_model_str,"KXProject_SB0400",KX_MAX_STRING);
         
       // has ac97 codec
       break;
     case 0x10211102: // [102]
     case 0x10231102: // audigy 4 variant
        //strncpy(tmp_str,"SB0610 10k2",KX_MAX_STRING);
           
         _kx_strcpy(tmp_str,"SB Audigy 4",KX_MAX_STRING);
         _kx_strcpy(tmp_model_str,"KXProject_SB0610",KX_MAX_STRING);
           
        is_a4=1;
        // has ac97
       break;
     case 0x10241102:
         //strncpy(tmp_str,"SB1550 10k2",KX_MAX_STRING); // audigy rx experimental
         
         //get this card if you want to use this driver on a modern system
         _kx_strcpy(tmp_str,"SB Audigy RX",KX_MAX_STRING);
         _kx_strcpy(tmp_model_str,"KXProject_SB1550",KX_MAX_STRING);
         
         is_a4=1;
         //has ac97, since it's just an audigy 4 with a pcie bridge chip
         break;
     case 0x10221102: // [104]
       //strncpy(tmp_str,"SB1022 10k2",KX_MAX_STRING); // Audigy2 value ?
         
         _kx_strcpy(tmp_str,"SB Audigy 2",KX_MAX_STRING);
         _kx_strcpy(tmp_model_str,"KXProject_SB1022",KX_MAX_STRING);
         
       break;
     case 0x11011102: // [106]
       //strncpy(tmp_str,"SB1101 10k2",KX_MAX_STRING); // Audigy SA ??
         
         _kx_strcpy(tmp_str,"SB Audigy",KX_MAX_STRING);
         _kx_strcpy(tmp_model_str,"KXProject_SB1101",KX_MAX_STRING);
       break;

     case 0x40021102:
         if(!is_k8){
             _kx_strcpy(tmp_str,"E-MU E-0404",KX_MAX_STRING); // [111]
             _kx_strcpy(tmp_model_str,"KXProject_EMU_E-0404_10k2",KX_MAX_STRING);
         }else{
             _kx_strcpy(tmp_str,"E-MU E-0404",KX_MAX_STRING); // [120]
             _kx_strcpy(tmp_model_str,"KXProject_EMU_E-0404_10k8",KX_MAX_STRING);
         }
       is_edsp=1;
       lack_ac97=1;
       break;
     case 0x40031102:
         if(!is_k8){
             _kx_strcpy(tmp_str,"E-MU E-1010",KX_MAX_STRING); // [112]
             _kx_strcpy(tmp_model_str,"KXProject_EMU_E-1010_10k2",KX_MAX_STRING);
         }else{
             _kx_strcpy(tmp_str,"E-MU E-1010",KX_MAX_STRING); // [121]
             _kx_strcpy(tmp_model_str,"KXProject_EMU_E-1010_10k8",KX_MAX_STRING);
         }
       is_edsp=1;
       lack_ac97=1;
       break;
     case 0x40041102: // [123]
       if(is_k8)
       {
           _kx_strcpy(tmp_str,"E-MU EM8960",KX_MAX_STRING); // EM8960: 1010 PCI 10k8
           _kx_strcpy(tmp_model_str,"KXProject_EMU_EM8960",KX_MAX_STRING);
        is_edsp=1;
        lack_ac97=1;
       }
       else
       {
           _kx_strcpy(tmp_str,"E-MU UN4004",KX_MAX_STRING); // ?? 10k2
           _kx_strcpy(tmp_model_str,"KXProject_EMU_UN4004",KX_MAX_STRING);
       }
       break;
     case 0x40051102:
         if(is_k8)
         {
             //strncpy(tmp_str,"EM8984 10k8",KX_MAX_STRING); // EM8984: 0404 PCIe 10k8
             
             _kx_strcpy(tmp_str,"E-MU E-0404 PCIe",KX_MAX_STRING);
             _kx_strcpy(tmp_model_str,"KXProject_EMU_E-0404_PCIe",KX_MAX_STRING);
             
             is_edsp=1;
             lack_ac97=1;
         }
         else
         {
             _kx_strcpy(tmp_str,"E-MU Un4005",KX_MAX_STRING); // ?? 10k2
             _kx_strcpy(tmp_model_str,"KXProject_EMU_UN4005",KX_MAX_STRING);
         }
         break;
     case 0x40071102: // EM8982 - 1010 ('PCIe' series)
     {
         if(is_k8)
         {
             _kx_strcpy(tmp_str,"E-MU EM8982",KX_MAX_STRING); //10k8
             _kx_strcpy(tmp_model_str,"KXProject_EMU_EM8982",KX_MAX_STRING);
             is_edsp=1;
             lack_ac97=1;
         }else{
             _kx_strcpy(tmp_str,"E-MU Un4007",KX_MAX_STRING); //10k2
             _kx_strcpy(tmp_model_str,"KXProject_EMU_UN4007",KX_MAX_STRING);
         }
     }
         break;

     case 0x42011102: // [122]
         _kx_strcpy(tmp_str,"E-MU EM8950",KX_MAX_STRING); // 1616 CardBus v2 10k2
         _kx_strcpy(tmp_model_str,"KXProject_EMU_EM8950",KX_MAX_STRING);
       is_edsp=1;
       lack_ac97=1;
       is_cardbus=1;
       break;

     default:
         //if you are in this case you own a really wired sound card
         _kx_strcpy(tmp_str,"kX audio driver generic",KX_MAX_STRING);
         _kx_strcpy(tmp_model_str,"KXProject_generic",KX_MAX_STRING);
       ret=-1;
       break;
 }
 return ret;
}
