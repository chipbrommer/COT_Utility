#include "../PugiXML/pugixml.hpp"
#include <iostream>
#include "COTParser/cotParser.hpp"

int main()
{
    // Testing string
    std::string test = "ÆÊ½8(Û'BEÈÚü¼À¨ÒÀ¨Ò¡Êh´û<?xml version=\"1.0\" encoding=\"utf - 8\" standalone=\"yes\"?>";
    test += "<event version=\"2.0\" uid=\"S-1-5-21-2515255310-331139352-785488330-3297\" type=\"a-f-G-E-V-A\" time=\"2022-12-22T18:06:59.36Z\" start=\"2022-12-22T18:06:59.36Z\" stale=\"2022-12-22T18:08:14.36Z\" how=\"h-e\">";
    test += "<point lat=\"31.5990919461411\" lon=\"-81.7768698985248\" hae=\"9999999\" ce=\"9999999\" le=\"9999999\"/>";
    test += "<detail>";
    test += "<takv version=\"4.1.0.231\" platform=\"WinTAK-CIV\" os=\"Microsoft Windows 10 Pro\" device=\"Dell Inc. XPS 15 9510\"/>";
    test += "<contact callsign=\"ASEIRS\" endpoint=\"tcpsrcreply:4242:srctcp\" xmppUsername=\"\"/>";
    test += "<uid Droid=\"BIDDLE\"/>";
    test += "<precisionlocation altsrc=\" ? ? ? \" geopointsrc=\"USER\"/>";
    test += "<uid Droid=\"ASEIRS\"/><__group name=\"Blue\" role=\"HQ\"/><status battery=\"100\"/>";
    test += "<track course=\"0.00000000\" speed=\"0.00000000\"/></detail></event>";

    // Testing char aray (unit8_t buffer)
    const char* test2 = "ÆÊ½8(Û'BEÈÚü¼À¨ÒÀ¨Ò¡Êh´û<?xml version=\"1.0\" encoding=\"utf - 8\" standalone=\"yes\"?><event version=\"2.0\" uid=\"S-1-5-21-2515255310-331139352-785488330-3297\" type=\"a-f-G-E-V-A\" time=\"2022-11-02T18:06:59.36Z\" start=\"2022-10-02T18:06:59.36Z\" stale=\"2022-09-22T18:08:14.36Z\" how=\"m-t\"><point lat=\"31.5990919461411\" lon=\"-81.7768698985248\"/></event>";
    const uint8_t* t2 = reinterpret_cast<const uint8_t*>(test2);

    // EXAMPLE: How to parse from a string into a COT structure with return value. 
    COTSchema cot;
    COTParser c;
    int num = c.ParseCOT(test, cot);
    if (num > 0)
    {
        std::cout << cot;
    }

    // EXAMPLE: How to parse from a uint8_t buffer into a COTSchema with return value. 
    std::cout << "\n\n";
    COTSchema cot2;
    num = c.ParseCOT(t2, cot2);
    if (num > 0)
    {
        std::cout << cot2;
    }

    // EXAMPLE: How to parse from a uint8_t buffer with returned COTSchema. 
    std::cout << "\n\n";
    COTSchema cot3 = c.ParseBufferToCOT(t2);
    if (cot2.Valid())
    {
        std::cout << cot3;
    }

    return 0;
}