
#include <iostream>
#include "cot_utility.h"

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
    CoT_Utility c;
    std::cout << c.GetVersion() << "\n\n";

    // EXAMPLE: How to parse from a string into a COT structure with return value. 
    CoT_Schema cot;
    CoT_UtilityResult res = c.ParseCOT(test, cot);
    if (res == CoT_UtilityResult::Success)
    {
        std::cout << cot;
    }

    // EXAMPLE: How to parse from a char buffer into a CoT_Schema with return value. 
    std::cout << "\n\n";
    CoT_Schema cot2;
    res = c.ParseCOT(test2, cot2);
    if (res == CoT_UtilityResult::Success)
    {
        std::cout << cot2;
    }

    // EXAMPLE: How to parse from a uint8_t buffer with returned CoT_Schema. 
    std::cout << "\n\n";
    CoT_Schema cot3 = c.ParseBufferToCOT(test2);
    if (cot3.IsValid())
    {
        std::cout << cot3;
    }

    // EXAMPLE: How to create an output message from a COT Schema using cot3 from prior example
    std::string out = c.GenerateXMLCOTMessage(cot3);
    std::cout << out;


    // EXAMPLE: How to append an 'ack' status to a received message. 
    std::cout << "\n\n";
    std::string mod;
    res = c.AcknowledgeReceivedCOTMessage(test, mod);

    if (res == CoT_UtilityResult::Success)
    {
        std::cout << mod;
    }

    // EXAMPLE: How to edit fields within a received message from schema. 
    std::cout << "\n\n";
    std::string mod2;
    cot3.point.latitude = 180.01234; // here we are updating the latitude of cot3 example
    res = c.UpdateReceivedCOTMessage(test, cot3, mod2);

    if (res == CoT_UtilityResult::Success)
    {
        std::cout << mod2;
    }

    // EXAMPLE: Handling an bad or malformed XML.
    std::string badTest = "<?xml version=\"1.0\" encoding=\"utf-16\" standalone=\"yes\"?>";
        badTest += "<event version=\"2.0\" uid=\"7bacac5f-c590-492c-a628-88622048c531\" type=\"u-d-v\" time=\"2025-05-10T19:59:09.00Z\" start=\"2025-05-10T19:59:09.00Z\" stale=\"2026-05-10T19:59:09.00Z\" how=\"h-g-i-g-o\">";
        badTest += "<point lat=\"34.2570122\" lon=\"-82.5056726\" hae=\"9999999\" ce=\"9999999\" le=\"9999999\" />";
        badTest += "<detail>";

    std::cout << "\n\n";
    CoT_Schema cot4;
    res = c.ParseCOT(badTest, cot4);
    if (res == CoT_UtilityResult::Success)
    {
        std::cout << cot4;
    }
    else
    {
        std::cerr << c.UtilityResultToString(res) << "\n";
    }

    // EXAMPLE: Links.
    std::string linkTest = R"("<?xml version="1.0" encoding="utf-16"?>
        <event version="2.0" uid="5d062bca-8828-49fe-8351-fdac40f0f8e2" type="b-m-r" how="h-e" time="2025-05-11T19:05:24.000Z" start="2025-05-11T19:05:24.000Z" stale="2025-06-10T19:05:24.000Z">
            <point lat="0" lon="0" hae="0" ce="9999999" le="9999999" />
            <detail>
                <contact callsign="Route 1" />
                <link uid="b7d81d74-8eb2-4c95-a273-3bd1bb658dcf" remarks="" relation="" callsign="Route 1 SP" type="b-m-p-w" point="34.911072,-85.754034" />
                <link uid="c28233bd-0fba-452d-b09a-8c89a9355ed3" remarks="" relation="" callsign="" type="b-m-p-c" point="36.100849,-81.596748" />
                <link uid="132310d0-fffa-406a-b607-11c0ec48f32d" remarks="" relation="" callsign="" type="b-m-p-c" point="34.529618,-81.338185" />
                <link uid="a1488d2d-e2b1-414e-89d7-028f96c2949c" remarks="" relation="" callsign="" type="b-m-p-c" point="32.489615,-81.703468" />
                <link uid="c2a287b2-59ac-4acf-a48a-02ca3d27dfce" remarks="" relation="" callsign="" type="b-m-p-c" point="31.324285,-84.254756" />
                <link uid="aa6dc12c-f5b1-4e6e-ad03-03cb32397a67" remarks="" relation="" callsign="" type="b-m-p-c" point="32.410202,-86.241709" />
                <link uid="0caa1cec-6f39-43c7-84bd-58adcce769c9" remarks="" relation="" callsign="TGT" type="b-m-p-c" point="34.005914,-86.152126" />
                <link_attr color="-16777089" method="Walking" direction="Infil" routetype="Primary" order="Ascending Check Points" />
                <remarks />
                <archive />
                <__routeinfo>
                    <__navcues />
                </__routeinfo>
            </detail>
        </event>)";

    std::cout << "\n\n";
    CoT_Schema cot5;
    res = c.ParseCOT(linkTest, cot5);
    if (res == CoT_UtilityResult::Success)
    {
        std::cout << cot5;
    }
    else
    {
        std::cerr << c.UtilityResultToString(res) << "\n";
    }

    return 0;
}