/*
 * Copyright (C) 2013 Bastian Bloessl <bloessl@ccs-labs.org>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <fullduplex/packet_encap2.h>

#include <gnuradio/io_signature.h>
#include <volk/volk.h>
/*
#include <sys/time.h>
#ifdef FOO_UHD
#include <uhd/types/time_spec.hpp>
#endif
*/

using namespace gr::fullduplex;


class packet_encap2_impl : public packet_encap2 {

public:
packet_encap2_impl(bool debug,
                   bool delay,
                   double delay_sec,
                   unsigned int pad_front,
                   unsigned int pad_tail,
                   unsigned int sig_pilot_pos,
                   double premultiplier,
                   unsigned int sync_word) : tagged_stream_block("fullduplex_packet_encap2",
      gr::io_signature::make(1, 1, sizeof(gr_complex)),
      gr::io_signature::make(1, 1, sizeof(gr_complex)),
      "packet_len"),
      d_debug(debug),
      d_delay(delay),
      d_delay_sec(delay_sec),
      d_pad_front(pad_front),
      d_pad_tail(pad_tail),
      d_sig_pilot_pos(sig_pilot_pos),
      d_sig_pilot_len(80*2),
      d_sig_pilot_scale(0.75),
      d_premultiplier(premultiplier) {
      set_tag_propagation_policy(block::TPP_DONT);

      // d_sig_pilot
      switch (sync_word) {
        case 4: {
          d_sig_pilot[0] = gr_complex(0.2415301, 0.2260483);
          d_sig_pilot[1] = gr_complex(0.2362651, 0.2448738);
          d_sig_pilot[2] = gr_complex(0.2294977, 0.2328913);
          d_sig_pilot[3] = gr_complex(0.2469668, 0.2359375);
          d_sig_pilot[4] = gr_complex(0.2266778, 0.2383683);
          d_sig_pilot[5] = gr_complex(0.2061197, 0.2414872);
          d_sig_pilot[6] = gr_complex(0.2997628, 0.2237712);
          d_sig_pilot[7] = gr_complex(0.2841261, 0.2129913);
          d_sig_pilot[8] = gr_complex(-0.06339657, 0.2949414);
          d_sig_pilot[9] = gr_complex(-0.2966726, 0.2741778);
          d_sig_pilot[10] = gr_complex(-0.002312267, -0.04418950);
          d_sig_pilot[11] = gr_complex(0.2996568, -0.2846775);
          d_sig_pilot[12] = gr_complex(0.06442316, -0.05345832);
          d_sig_pilot[13] = gr_complex(-0.2925742, 0.2873699);
          d_sig_pilot[14] = gr_complex(-0.2882507, 0.2860050);
          d_sig_pilot[15] = gr_complex(-0.1892664, 0.1848816);
          d_sig_pilot[16] = gr_complex(-0.2752704, 0.2856798);
          d_sig_pilot[17] = gr_complex(-0.2717815, 0.2861970);
          d_sig_pilot[18] = gr_complex(0.006807338, -0.05156344);
          d_sig_pilot[19] = gr_complex(0.2665204, -0.2855588);
          d_sig_pilot[20] = gr_complex(0.2675541, -0.04496950);
          d_sig_pilot[21] = gr_complex(0.2081669, 0.2820880);
          d_sig_pilot[22] = gr_complex(0.2716217, 0.2837466);
          d_sig_pilot[23] = gr_complex(0.2605682, 0.1966480);
          d_sig_pilot[24] = gr_complex(0.01265812, 0.2718523);
          d_sig_pilot[25] = gr_complex(-0.2487035, 0.2683555);
          d_sig_pilot[26] = gr_complex(-0.3282138, 0.0005040909);
          d_sig_pilot[27] = gr_complex(-0.2421811, -0.2604103);
          d_sig_pilot[28] = gr_complex(0.005069729, -0.2852476);
          d_sig_pilot[29] = gr_complex(0.2653926, -0.2160861);
          d_sig_pilot[30] = gr_complex(0.2735989, -0.2267712);
          d_sig_pilot[31] = gr_complex(0.1907513, -0.2531302);
          d_sig_pilot[32] = gr_complex(0.2886056, -0.2191226);
          d_sig_pilot[33] = gr_complex(0.2846102, -0.2205545);
          d_sig_pilot[34] = gr_complex(-0.05311486, -0.2880961);
          d_sig_pilot[35] = gr_complex(-0.2726818, -0.2470663);
          d_sig_pilot[36] = gr_complex(-0.06083876, -0.01463879);
          d_sig_pilot[37] = gr_complex(0.2497119, 0.2417358);
          d_sig_pilot[38] = gr_complex(0.3604469, 0.3383162);
          d_sig_pilot[39] = gr_complex(0.2476456, 0.2339509);
          d_sig_pilot[40] = gr_complex(-0.05856207, -0.002824778);
          d_sig_pilot[41] = gr_complex(-0.2698561, -0.2309523);
          d_sig_pilot[42] = gr_complex(-0.05873690, -0.3357959);
          d_sig_pilot[43] = gr_complex(0.2854967, -0.2470277);
          d_sig_pilot[44] = gr_complex(0.2933628, 0.01821897);
          d_sig_pilot[45] = gr_complex(0.1791395, 0.2554515);
          d_sig_pilot[46] = gr_complex(0.2849427, 0.2719296);
          d_sig_pilot[47] = gr_complex(0.2922719, 0.2106134);
          d_sig_pilot[48] = gr_complex(-0.05788809, 0.2645695);
          d_sig_pilot[49] = gr_complex(-0.2855059, 0.2676133);
          d_sig_pilot[50] = gr_complex(-0.03860194, 0.008550827);
          d_sig_pilot[51] = gr_complex(0.2692111, -0.2739787);
          d_sig_pilot[52] = gr_complex(0.2947738, -0.2746450);
          d_sig_pilot[53] = gr_complex(0.2248881, -0.1889465);
          d_sig_pilot[54] = gr_complex(0.2075375, -0.2892225);
          d_sig_pilot[55] = gr_complex(0.2236260, -0.2924972);
          d_sig_pilot[56] = gr_complex(0.2960655, 0.06529105);
          d_sig_pilot[57] = gr_complex(0.2706344, 0.2988375);
          d_sig_pilot[58] = gr_complex(-0.04156519, -0.002312247);
          d_sig_pilot[59] = gr_complex(-0.2855397, -0.2974918);
          d_sig_pilot[60] = gr_complex(-0.05456954, -0.06252863);
          d_sig_pilot[61] = gr_complex(0.2885890, 0.2842031);
          d_sig_pilot[62] = gr_complex(0.2859116, 0.2987909);
          d_sig_pilot[63] = gr_complex(0.1893616, 0.2072588);
          d_sig_pilot[64] = gr_complex(0.2776349, 0.2264353);
          d_sig_pilot[65] = gr_complex(0.2698198, 0.2455117);
          d_sig_pilot[66] = gr_complex(-0.007173449, 0.2313452);
          d_sig_pilot[67] = gr_complex(-0.2564558, 0.2353229);
          d_sig_pilot[68] = gr_complex(-0.2818089, 0.2406278);
          d_sig_pilot[69] = gr_complex(-0.2293922, 0.2347062);
          d_sig_pilot[70] = gr_complex(-0.2122173, 0.2324805);
          d_sig_pilot[71] = gr_complex(-0.2275259, 0.2367725);
          d_sig_pilot[72] = gr_complex(-0.2839042, 0.2383511);
          d_sig_pilot[73] = gr_complex(-0.2584940, 0.2324972);
          d_sig_pilot[74] = gr_complex(-0.002667191, 0.2369672);
          d_sig_pilot[75] = gr_complex(0.2687763, 0.2446251);
          d_sig_pilot[76] = gr_complex(0.2744262, 0.2216781);
          d_sig_pilot[77] = gr_complex(0.2000531, 0.2188707);
          d_sig_pilot[78] = gr_complex(0.2742395, 0.2874470);
          d_sig_pilot[79] = gr_complex(0.2675741, 0.2556854);
          std::cout << "QPSK pilot" << std::endl;
          break;
        }
        case 3: {
          d_sig_pilot[0] = gr_complex(-0.125000000000000, 0.0625000000000000);
          d_sig_pilot[1] = gr_complex(0.105394756677054, 0.0878555783066188);
          d_sig_pilot[2] = gr_complex(-0.00737027342239904, -0.0966425138032576);
          d_sig_pilot[3] = gr_complex(-0.348206785428921, 0.0432568283664788);
          d_sig_pilot[4] = gr_complex(-0.306726427265922, 0.309813334387990);
          d_sig_pilot[5] = gr_complex(-0.0488555723557256, -0.105744015744401);
          d_sig_pilot[6] = gr_complex(-0.0941327917604179, -0.101820914942547);
          d_sig_pilot[7] = gr_complex(-0.177351248468494, 0.170844567046037);
          d_sig_pilot[8] = gr_complex(0.0388325214724777, 0.0129441738241592);
          d_sig_pilot[9] = gr_complex(-0.0350906596181919, 0.0425799529170058);
          d_sig_pilot[10] = gr_complex(0.218936491841036, -0.0816331390336931);
          d_sig_pilot[11] = gr_complex(0.277179654820419, -0.178890283239925);
          d_sig_pilot[12] = gr_complex(-0.356724011776221, 0.0748317924806947);
          d_sig_pilot[13] = gr_complex(-0.121019298472119, -0.0142235362871961);
          d_sig_pilot[14] = gr_complex(0.0739162194461587, -0.230699357452330);
          d_sig_pilot[15] = gr_complex(-0.0501506182661379, -0.214717908809369);
          d_sig_pilot[16] = gr_complex(0.125000000000000, -0.187500000000000);
          d_sig_pilot[17] = gr_complex(-0.0501506182661380, -0.214717908809369);
          d_sig_pilot[18] = gr_complex(0.0739162194461587, -0.230699357452330);
          d_sig_pilot[19] = gr_complex(-0.121019298472119, -0.0142235362871961);
          d_sig_pilot[20] = gr_complex(-0.356724011776221, 0.0748317924806947);
          d_sig_pilot[21] = gr_complex(0.277179654820419, -0.178890283239925);
          d_sig_pilot[22] = gr_complex(0.218936491841036, -0.0816331390336930);
          d_sig_pilot[23] = gr_complex(-0.0350906596181919, 0.0425799529170058);
          d_sig_pilot[24] = gr_complex(0.0388325214724777, 0.0129441738241592);
          d_sig_pilot[25] = gr_complex(-0.177351248468494, 0.170844567046037);
          d_sig_pilot[26] = gr_complex(-0.0941327917604179, -0.101820914942547);
          d_sig_pilot[27] = gr_complex(-0.0488555723557256, -0.105744015744401);
          d_sig_pilot[28] = gr_complex(-0.306726427265922, 0.309813334387990);
          d_sig_pilot[29] = gr_complex(-0.348206785428921, 0.0432568283664788);
          d_sig_pilot[30] = gr_complex(-0.00737027342239904, -0.0966425138032576);
          d_sig_pilot[31] = gr_complex(0.105394756677054, 0.0878555783066188);
          d_sig_pilot[32] = gr_complex(-0.125000000000000, 0.0625000000000000);
          d_sig_pilot[33] = gr_complex(0.0574520747571235, -0.0108560828136444);
          d_sig_pilot[34] = gr_complex(0.141623416990853, -0.0191043426282881);
          d_sig_pilot[35] = gr_complex(0.126529190000493, 0.227803787646502);
          d_sig_pilot[36] = gr_complex(0.163420601090081, 0.0986575347328064);
          d_sig_pilot[37] = gr_complex(-0.173155377467864, -0.293335891969060);
          d_sig_pilot[38] = gr_complex(-0.0165946205854483, -0.258906497403319);
          d_sig_pilot[39] = gr_complex(0.114615531315137, -0.0793133234540199);
          d_sig_pilot[40] = gr_complex(-0.226332521472478, -0.0754441738241592);
          d_sig_pilot[41] = gr_complex(-0.0271736232284508, -0.244546175623114);
          d_sig_pilot[42] = gr_complex(0.0167909205048302, 0.0673605513795593);
          d_sig_pilot[43] = gr_complex(-0.180168704996829, 0.167807119994380);
          d_sig_pilot[44] = gr_complex(0.125029837952061, -0.108302661601491);
          d_sig_pilot[45] = gr_complex(0.217696893900546, 0.256879381826496);
          d_sig_pilot[46] = gr_complex(-0.0831693630146130, -0.0285537861161242);
          d_sig_pilot[47] = gr_complex(-0.237696213168040, -0.355399998162787);
          d_sig_pilot[48] = gr_complex(-0.250000000000000, -0.0625000000000000);
          d_sig_pilot[49] = gr_complex(-0.237696213168040, -0.355399998162788);
          d_sig_pilot[50] = gr_complex(-0.0831693630146130, -0.0285537861161242);
          d_sig_pilot[51] = gr_complex(0.217696893900546, 0.256879381826496);
          d_sig_pilot[52] = gr_complex(0.125029837952061, -0.108302661601491);
          d_sig_pilot[53] = gr_complex(-0.180168704996829, 0.167807119994380);
          d_sig_pilot[54] = gr_complex(0.0167909205048302, 0.0673605513795593);
          d_sig_pilot[55] = gr_complex(-0.0271736232284508, -0.244546175623114);
          d_sig_pilot[56] = gr_complex(-0.226332521472478, -0.0754441738241592);
          d_sig_pilot[57] = gr_complex(0.114615531315137, -0.0793133234540199);
          d_sig_pilot[58] = gr_complex(-0.0165946205854483, -0.258906497403319);
          d_sig_pilot[59] = gr_complex(-0.173155377467864, -0.293335891969060);
          d_sig_pilot[60] = gr_complex(0.163420601090081, 0.0986575347328064);
          d_sig_pilot[61] = gr_complex(0.126529190000493, 0.227803787646502);
          d_sig_pilot[62] = gr_complex(0.141623416990853, -0.0191043426282880);
          d_sig_pilot[63] = gr_complex(0.0574520747571235, -0.0108560828136444);
          d_sig_pilot[64] = gr_complex(-0.125000000000000, 0.0625000000000000);
          d_sig_pilot[65] = gr_complex(0.105394756677054, 0.0878555783066188);
          d_sig_pilot[66] = gr_complex(-0.00737027342239904, -0.0966425138032576);
          d_sig_pilot[67] = gr_complex(-0.348206785428921, 0.0432568283664788);
          d_sig_pilot[68] = gr_complex(-0.306726427265922, 0.309813334387990);
          d_sig_pilot[69] = gr_complex(-0.0488555723557256, -0.105744015744401);
          d_sig_pilot[70] = gr_complex(-0.0941327917604179, -0.101820914942547);
          d_sig_pilot[71] = gr_complex(-0.177351248468494, 0.170844567046037);
          d_sig_pilot[72] = gr_complex(0.0388325214724777, 0.0129441738241592);
          d_sig_pilot[73] = gr_complex(-0.0350906596181919, 0.0425799529170058);
          d_sig_pilot[74] = gr_complex(0.218936491841036, -0.0816331390336931);
          d_sig_pilot[75] = gr_complex(0.277179654820419, -0.178890283239925);
          d_sig_pilot[76] = gr_complex(-0.356724011776221, 0.0748317924806947);
          d_sig_pilot[77] = gr_complex(-0.121019298472119, -0.0142235362871961);
          d_sig_pilot[78] = gr_complex(0.0739162194461587, -0.230699357452330);
          d_sig_pilot[79] = gr_complex(-0.0501506182661379, -0.214717908809369);
          std::cout << "Symmetric OFDM pilot" << std::endl;
          break;
        }
        case 2: {
          d_sig_pilot[0] = gr_complex(0.186025000000000, 0.0930125000000000);
          d_sig_pilot[1] = gr_complex(0.321146430470934, 0.245711291356193);
          d_sig_pilot[2] = gr_complex(0.00541891602060207, 0.0993353200279574); 
          d_sig_pilot[3] = gr_complex(-0.285318228340574, 0.229518079506709);
          d_sig_pilot[4] = gr_complex(0.204526336570900, 0.165862424467408);
          d_sig_pilot[5] = gr_complex(-0.0110874127106207, -0.126700474986762);
          d_sig_pilot[6] = gr_complex(-0.106094920550298, -0.114687722577076);
          d_sig_pilot[7] = gr_complex(0.422432004720176, -0.0542106075797569);
          d_sig_pilot[8] = gr_complex(-0.449104077940455, -0.104296808455341);
          d_sig_pilot[9] = gr_complex(-0.431334563663399, 0.0343342477095727);
          d_sig_pilot[10] = gr_complex(0.237634459520526, 0.172572873877521);
          d_sig_pilot[11] = gr_complex(0.194773625816765, 0.0715132110883542);
          d_sig_pilot[12] = gr_complex(0.325228043499924, -0.155849573267744);
          d_sig_pilot[13] = gr_complex(-0.425724045609008, -0.383327169591475);
          d_sig_pilot[14] = gr_complex(-0.136958454990830, -0.387026621223704);
          d_sig_pilot[15] = gr_complex(0.345087674600714, -0.162027241244462);
          d_sig_pilot[16] = gr_complex(-0.186025000000000, 0.00000000000000);
          d_sig_pilot[17] = gr_complex(0.345087674600714, 0.162027241244462);
          d_sig_pilot[18] = gr_complex(-0.136958454990830, 0.387026621223704);
          d_sig_pilot[19] = gr_complex(-0.425724045609008, 0.383327169591475);
          d_sig_pilot[20] = gr_complex(0.325228043499924, 0.155849573267744);
          d_sig_pilot[21] = gr_complex(0.194773625816765, -0.0715132110883542);
          d_sig_pilot[22] = gr_complex(0.237634459520526, -0.172572873877521);
          d_sig_pilot[23] = gr_complex(-0.431334563663399, -0.0343342477095727);
          d_sig_pilot[24] = gr_complex(-0.449104077940455, 0.104296808455341);
          d_sig_pilot[25] = gr_complex(0.422432004720176, 0.0542106075797570);
          d_sig_pilot[26] = gr_complex(-0.106094920550298, 0.114687722577076);
          d_sig_pilot[27] = gr_complex(-0.0110874127106206, 0.126700474986762);
          d_sig_pilot[28] = gr_complex(0.204526336570900, -0.165862424467408);
          d_sig_pilot[29] = gr_complex(-0.285318228340574, -0.229518079506709);
          d_sig_pilot[30] = gr_complex(0.00541891602060209, -0.0993353200279574);
          d_sig_pilot[31] = gr_complex(0.321146430470934, -0.245711291356193);
          d_sig_pilot[32] = gr_complex(0.186025000000000, -0.0930125000000000);
          d_sig_pilot[33] = gr_complex(0.0156983448938515, 0.351198300308213);
          d_sig_pilot[34] = gr_complex(0.00541891602060207, 0.157878365477758);
          d_sig_pilot[35] = gr_complex(-0.176287259044145, -0.0799775834093137);
          d_sig_pilot[36] = gr_complex(-0.281580414511355, 0.337727114502820);
          d_sig_pilot[37] = gr_complex(-0.185352351175256, 0.436536075174812);
          d_sig_pilot[38] = gr_complex(-0.106094920550298, 0.100527412612488);
          d_sig_pilot[39] = gr_complex(0.312168281575520, 0.162150025937668);
          d_sig_pilot[40] = gr_complex(0.0770540779404550, 0.290321808455341);
          d_sig_pilot[41] = gr_complex(-0.182564015703273, 0.222627497506538);
          d_sig_pilot[42] = gr_complex(0.237634459520526, 0.185316816157891);
          d_sig_pilot[43] = gr_complex(0.0823164401994809, -0.0720005420192420);
          d_sig_pilot[44] = gr_complex(0.123876034440531, -0.0846608877620284);
          d_sig_pilot[45] = gr_complex(0.280521074982447, 0.440486578689023);
          d_sig_pilot[46] = gr_complex(-0.136958454990830, 0.272190306729420);
          d_sig_pilot[47] = gr_complex(-0.276476001013613, -0.362298996113843);
          d_sig_pilot[48] = gr_complex(-0.186025000000000, 0.00000000000000);
          d_sig_pilot[49] = gr_complex(-0.276476001013613, 0.362298996113843);
          d_sig_pilot[50] = gr_complex(-0.136958454990830, -0.272190306729420);
          d_sig_pilot[51] = gr_complex(0.280521074982447, -0.440486578689023);
          d_sig_pilot[52] = gr_complex(0.123876034440531, 0.0846608877620283);
          d_sig_pilot[53] = gr_complex(0.0823164401994809, 0.0720005420192421);
          d_sig_pilot[54] = gr_complex(0.237634459520526, -0.185316816157891);
          d_sig_pilot[55] = gr_complex(-0.182564015703273, -0.222627497506538);
          d_sig_pilot[56] = gr_complex(0.0770540779404550, -0.290321808455341);
          d_sig_pilot[57] = gr_complex(0.312168281575520, -0.162150025937668);
          d_sig_pilot[58] = gr_complex(-0.106094920550298, -0.100527412612488);
          d_sig_pilot[59] = gr_complex(-0.185352351175256, -0.436536075174812);
          d_sig_pilot[60] = gr_complex(-0.281580414511355, -0.337727114502820);
          d_sig_pilot[61] = gr_complex(-0.176287259044145, 0.0799775834093136);
          d_sig_pilot[62] = gr_complex(0.00541891602060209, -0.157878365477758);
          d_sig_pilot[63] = gr_complex(0.0156983448938515, -0.351198300308213);
          d_sig_pilot[64] = gr_complex(0.186025000000000, 0.0930125000000000);
          d_sig_pilot[65] = gr_complex(0.321146430470934, 0.245711291356193);
          d_sig_pilot[66] = gr_complex(0.00541891602060207, 0.0993353200279574);
          d_sig_pilot[67] = gr_complex(-0.285318228340574, 0.229518079506709);
          d_sig_pilot[68] = gr_complex(0.204526336570900, 0.165862424467408);
          d_sig_pilot[69] = gr_complex(-0.0110874127106207, -0.126700474986762);
          d_sig_pilot[70] = gr_complex(-0.106094920550298, -0.114687722577076);
          d_sig_pilot[71] = gr_complex(0.422432004720176, -0.0542106075797569);
          d_sig_pilot[72] = gr_complex(-0.449104077940455, -0.104296808455341);
          d_sig_pilot[73] = gr_complex(-0.431334563663399, 0.0343342477095727);
          d_sig_pilot[74] = gr_complex(0.237634459520526, 0.172572873877521);
          d_sig_pilot[75] = gr_complex(0.194773625816765, 0.0715132110883542);
          d_sig_pilot[76] = gr_complex(0.325228043499924, -0.155849573267744);
          d_sig_pilot[77] = gr_complex(-0.425724045609008, -0.383327169591475);
          d_sig_pilot[78] = gr_complex(-0.136958454990830, -0.387026621223704);
          d_sig_pilot[79] = gr_complex(0.345087674600714, -0.162027241244462);
          std::cout << "Sync word 2 OFDM pilot" << std::endl;
          break;
        }
        case 1 : {
          d_sig_pilot[ 0] = gr_complex( 0.0000,  0.0000);
          d_sig_pilot[ 1] = gr_complex( 0.5000, -0.1435);
          d_sig_pilot[ 2] = gr_complex( 0.0517, -0.1429);
          d_sig_pilot[ 3] = gr_complex( 0.0615, -0.1139);
          d_sig_pilot[ 4] = gr_complex( 0.0147, -0.2222);
          d_sig_pilot[ 5] = gr_complex(-0.1490,  0.0156);
          d_sig_pilot[ 6] = gr_complex( 0.0912,  0.0139);
          d_sig_pilot[ 7] = gr_complex( 0.2461, -0.1844);
          d_sig_pilot[ 8] = gr_complex( 0.1973,  0.1315);
          d_sig_pilot[ 9] = gr_complex(-0.0908,  0.2135);
          d_sig_pilot[10] = gr_complex( 0.0181, -0.0698);
          d_sig_pilot[11] = gr_complex( 0.1200, -0.1509);
          d_sig_pilot[12] = gr_complex(-0.2075, -0.1927);
          d_sig_pilot[13] = gr_complex(-0.0559, -0.0674);
          d_sig_pilot[14] = gr_complex(-0.0773, -0.0955);
          d_sig_pilot[15] = gr_complex(-0.1356, -0.3690);
          d_sig_pilot[16] = gr_complex( 0.0930,  0.0000);
          d_sig_pilot[17] = gr_complex(-0.1356,  0.3690);
          d_sig_pilot[18] = gr_complex(-0.0773,  0.0955);
          d_sig_pilot[19] = gr_complex(-0.0559,  0.0674);
          d_sig_pilot[20] = gr_complex(-0.2075,  0.1927);
          d_sig_pilot[21] = gr_complex( 0.1200,  0.1509);
          d_sig_pilot[22] = gr_complex( 0.0181,  0.0698);
          d_sig_pilot[23] = gr_complex(-0.0908, -0.2135);
          d_sig_pilot[24] = gr_complex( 0.1973, -0.1315);
          d_sig_pilot[25] = gr_complex( 0.2461,  0.1844);
          d_sig_pilot[26] = gr_complex( 0.0912, -0.0139);
          d_sig_pilot[27] = gr_complex(-0.1490, -0.0156);
          d_sig_pilot[28] = gr_complex( 0.0147,  0.2222);
          d_sig_pilot[29] = gr_complex( 0.0615,  0.1139);
          d_sig_pilot[30] = gr_complex( 0.0517,  0.1429);
          d_sig_pilot[31] = gr_complex( 0.5000,  0.1435);
          d_sig_pilot[32] = gr_complex( 0.0000,  0.0000);
          d_sig_pilot[33] = gr_complex(-0.5000,  0.1435);
          d_sig_pilot[34] = gr_complex(-0.0517,  0.1429);
          d_sig_pilot[35] = gr_complex(-0.0615,  0.1139);
          d_sig_pilot[36] = gr_complex(-0.0147,  0.2222);
          d_sig_pilot[37] = gr_complex( 0.1490, -0.0156);
          d_sig_pilot[38] = gr_complex(-0.0912, -0.0139);
          d_sig_pilot[39] = gr_complex(-0.2461,  0.1844);
          d_sig_pilot[40] = gr_complex(-0.1973, -0.1315);
          d_sig_pilot[41] = gr_complex( 0.0908, -0.2135);
          d_sig_pilot[42] = gr_complex(-0.0181,  0.0698);
          d_sig_pilot[43] = gr_complex(-0.1200,  0.1509);
          d_sig_pilot[44] = gr_complex( 0.2075,  0.1927);
          d_sig_pilot[45] = gr_complex( 0.0559,  0.0674);
          d_sig_pilot[46] = gr_complex( 0.0773,  0.0955);
          d_sig_pilot[47] = gr_complex( 0.1356,  0.3690);
          d_sig_pilot[48] = gr_complex(-0.0930,  0.0000);
          d_sig_pilot[49] = gr_complex( 0.1356, -0.3690);
          d_sig_pilot[50] = gr_complex( 0.0773, -0.0955);
          d_sig_pilot[51] = gr_complex( 0.0559, -0.0674);
          d_sig_pilot[52] = gr_complex( 0.2075, -0.1927);
          d_sig_pilot[53] = gr_complex(-0.1200, -0.1509);
          d_sig_pilot[54] = gr_complex(-0.0181, -0.0698);
          d_sig_pilot[55] = gr_complex( 0.0908,  0.2135);
          d_sig_pilot[56] = gr_complex(-0.1973,  0.1315);
          d_sig_pilot[57] = gr_complex(-0.2461, -0.1844);
          d_sig_pilot[58] = gr_complex(-0.0912,  0.0139);
          d_sig_pilot[59] = gr_complex( 0.1490,  0.0156);
          d_sig_pilot[60] = gr_complex(-0.0147, -0.2222);
          d_sig_pilot[61] = gr_complex(-0.0615, -0.1139);
          d_sig_pilot[62] = gr_complex(-0.0517, -0.1429);
          d_sig_pilot[63] = gr_complex(-0.5000, -0.1435);
          d_sig_pilot[64] = gr_complex( 0.0000,  0.0000);
          d_sig_pilot[65] = gr_complex( 0.5000, -0.1435);
          d_sig_pilot[66] = gr_complex( 0.0517, -0.1429);
          d_sig_pilot[67] = gr_complex( 0.0615, -0.1139);
          d_sig_pilot[68] = gr_complex( 0.0147, -0.2222);
          d_sig_pilot[69] = gr_complex(-0.1490,  0.0156);
          d_sig_pilot[70] = gr_complex( 0.0912,  0.0139);
          d_sig_pilot[71] = gr_complex( 0.2461, -0.1844);
          d_sig_pilot[72] = gr_complex( 0.1973,  0.1315);
          d_sig_pilot[73] = gr_complex(-0.0908,  0.2135);
          d_sig_pilot[74] = gr_complex( 0.0181, -0.0698);
          d_sig_pilot[75] = gr_complex( 0.1200, -0.1509);
          d_sig_pilot[76] = gr_complex(-0.2075, -0.1927);
          d_sig_pilot[77] = gr_complex(-0.0559, -0.0674);
          d_sig_pilot[78] = gr_complex(-0.0773, -0.0955);
          d_sig_pilot[79] = gr_complex(-0.1356, -0.3690);
          std::cout << "Sync word 1 OFDM pilot" << std::endl;
          break;
        }
        case 0: {
          for (int idx = 0; idx < 80; idx++) {
            d_sig_pilot[idx] = gr_complex(0.0, 0.0);
          }
          std::cout << "No pilot";
          break;
        }
      }
      // end of d_sig_pilot

      for (int idx_sig_pilot = 0; idx_sig_pilot<80; idx_sig_pilot++) {
        d_sig_pilot[idx_sig_pilot+80] = d_sig_pilot[idx_sig_pilot];
      }

      for (int idx_sig_pilot = 0; idx_sig_pilot<80*2; idx_sig_pilot++) {
        d_sig_pilot[idx_sig_pilot] *= d_premultiplier;
      }

}

~packet_encap2_impl(){
}

void
set_premultiplier(float premultiplier)
{
  gr::thread::scoped_lock lock(d_mutex);
  // // undo old premultiplier
  // for (int idx_sig_pilot = 0; idx_sig_pilot<80*2; idx_sig_pilot++) {
  //   d_sig_pilot[idx_sig_pilot] /= d_premultiplier;
  // }
  // d_premultiplier = premultiplier;
  // // apply new premultiplier
  // for (int idx_sig_pilot = 0; idx_sig_pilot<80*2; idx_sig_pilot++) {
  //   d_sig_pilot[idx_sig_pilot] *= d_premultiplier;
  // }
}

int calculate_output_stream_length(const gr_vector_int &ninput_items) {
  return ninput_items[0] + d_pad_front + d_pad_tail;
}

int work (int noutput_items, gr_vector_int& ninput_items,
    gr_vector_const_void_star& input_items,
    gr_vector_void_star& output_items) {

  const gr_complex *in = (const gr_complex*)input_items[0];
  gr_complex *out = (gr_complex*)output_items[0];

  int ninput = ninput_items[0];
  int noutput = noutput_items;

  /*
  std::memset(out, 0x00, sizeof(gr_complex) * (ninput_items[0] + d_pad_front + d_pad_tail));

  std::memcpy(out + d_pad_front, in, sizeof(gr_complex) * ninput_items[0]);
  */
  std::memset(out, 0x00, sizeof(gr_complex)*(ninput_items[0] + 4*d_sig_pilot_len + d_pad_front + d_pad_tail));
  std::memcpy(out + d_pad_front + d_sig_pilot_pos*d_sig_pilot_len, d_sig_pilot, sizeof(gr_complex)*d_sig_pilot_len);
  std::memcpy(out + d_pad_front + 2*d_sig_pilot_len, in, sizeof(gr_complex)*ninput_items[0]);

  // std::cout << "Pad position: " << d_sig_pilot_pos*d_sig_pilot_len << std::endl;

  int produced = ninput_items[0] + 4*d_sig_pilot_len + d_pad_front + d_pad_tail;
  const pmt::pmt_t src = pmt::string_to_symbol(alias());

  volk_32fc_s32fc_multiply_32fc(out, out, 0.316227766, produced);

  /*
  #ifdef FOO_UHD
  if(d_delay) {
    static const pmt::pmt_t time_key = pmt::string_to_symbol("tx_time");
    struct timeval t;
    gettimeofday(&t, NULL);
    uhd::time_spec_t now = uhd::time_spec_t(t.tv_sec + t.tv_usec / 1000000.0)
      + uhd::time_spec_t(d_delay_sec);

    const pmt::pmt_t time_value = pmt::make_tuple(
      pmt::from_uint64(now.get_full_secs()),
      pmt::from_double(now.get_frac_secs())
    );
    add_item_tag(0, nitems_written(0), time_key, time_value, src);
  }
  #endif
  */

  std::vector<gr::tag_t> tags;
  get_tags_in_range(tags, 0, nitems_read(0), nitems_read(0) + ninput_items[0]);
  for (size_t i = 0; i < tags.size(); i++) {
    add_item_tag(0, nitems_written(0),
    tags[i].key,
    tags[i].value);
  }

  return produced;
}

private:
  bool   d_debug;
  bool   d_delay;
  double d_delay_sec;
  unsigned int d_pad_front;
  unsigned int d_pad_tail;
  unsigned int d_sig_pilot_pos;
  int d_sig_pilot_len;
  double d_sig_pilot_scale;
  double d_premultiplier;
};

packet_encap2::sptr
packet_encap2::make(bool debug,
                    bool delay,
                    double delay_sec,
                    unsigned int pad_front,
                    unsigned int pad_tail,
                    unsigned int sig_pilot_pos,
                    double premultiplier,
                    unsigned int sync_word) {
  return gnuradio::get_initial_sptr(new packet_encap2_impl(debug, delay, delay_sec, pad_front, pad_tail, sig_pilot_pos, premultiplier, sync_word));
}
