#include "../PugiXML/pugixml.hpp"
#include <iostream>
#include "COT_Utility/cot_utility.h"

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

    // Testing char aray
    const char* test2 = test.c_str();

    // Get Version and display
    COT_Utility c;
    std::cout << c.GetVersion() << "\n\n";

    // EXAMPLE: How to parse from a string into a COT structure with return value. 
    COTSchema cot;
    int num = c.ParseCOT(test, cot);
    if (num > 0)
    {
        std::cout << cot;
    }

    // EXAMPLE: How to parse from a char buffer into a COTSchema with return value. 
    std::cout << "\n\n";
    COTSchema cot2;
    num = c.ParseCOT(test2, cot2);
    if (num > 0)
    {
        std::cout << cot2;
    }

    // EXAMPLE: How to parse from a uint8_t buffer with returned COTSchema. 
    std::cout << "\n\n";
    COTSchema cot3 = c.ParseBufferToCOT(test2);
    if (cot3.Valid())
    {
        std::cout << cot3;
    }

    // EXAMPLE: How to create an output message from a COT Schema using cot3 from prior example
    std::string out = c.GenerateXMLCOTMessage(cot3);
    std::cout << out;


    // EXAMPLE: How to append an 'ack' status to a received message. 
    std::cout << "\n\n";
    std::string mod;
    bool rtn = c.AcknowledgeReceivedCOTMessage(test, mod);

    if (rtn)
    {
        std::cout << mod;
    }

    // EXAMPLE: How to edit fields within a received message from schema. 
    std::cout << "\n\n";
    std::string mod2;
    cot3.point.latitude = 180.01234; // here we are updating the latitude of cot3 example
    rtn = c.UpdateReceivedCOTMessage(test, cot3, mod2);

    if (rtn)
    {
        std::cout << mod2;
    }

    return 0;
}