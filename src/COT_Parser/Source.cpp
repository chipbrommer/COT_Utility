#include "../PugiXML/pugixml.hpp"
#include <iostream>
#include "COTParser/cotParser.hpp"

int main()
{
    // Testing string
    std::string test = "ÆÊ½8(Û'BEÈÚü¼À¨ÒÀ¨Ò¡Êh´û<?xml version=\"1.0\" encoding=\"utf - 8\" standalone=\"yes\"?>";
    test += "<event version=\"2.0\" uid=\"S-1-5-21-2515255310-331139352-785488330-3297\" type=\"a-f-G-E-V-A\" time=\"2022-12-22T18:06:59.36Z\" start=\"2022-12-22T18:06:59.36Z\" stale=\"2022-12-22T18:08:14.36Z\" how=\"h-e\">";
    test += "<point lat=\"31.5990919461411\" lon=\"-81.7768698985248\" hae=\"9999999\" ce=\"9999999\" le=\"9999999\"/>";
    test += "<detail><takv version=\"4.1.0.231\" platform=\"WinTAK-CIV\" os=\"Microsoft Windows 10 Pro\" device=\"Dell Inc. XPS 15 9510\"/>";
    test += "<contact callsign=\"ASEIRS\" endpoint=\"tcpsrcreply:4242:srctcp\" xmppUsername=\"\"/><uid Droid=\"ASEIRS\"/><__group name=\"Blue\" role=\"HQ\"/><status battery=\"100\"/>";
    test += "<track course=\"0.00000000\" speed=\"0.00000000\"/></detail></event>";

    COTSchema cot;
    COTParser c;
    int num = c.ParseCOT(test, cot);
    std::cout << cot;
    return 0;
}