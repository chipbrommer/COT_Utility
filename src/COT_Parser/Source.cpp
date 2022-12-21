#include "../PugiXML/pugixml.hpp"
#include <iostream>
#include "COTParser/cotParser.hpp"

int main()
{
    std::string test = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    test += "<event version=\"2.0\" uid=\"ANDROID - bae3f2313cda7004\" type=\"a - f -G-U-C\" time=\"2022 - 11 - 18T19:40 : 47.443Z\" start=\"2022 - 11 - 18T19 : 40 : 47.443Z\" stale=\"2022 - 11 - 18T19 : 42 : 02.443Z\" how=\"h - e\">";
    test += "<point lat=\"37.1040299\" lon=\"-76.481972\" hae=\"-27.576\" ce=\"9999999.0\" le=\"9999999.0\"/>";
    //test += "<point lat=\"3.19\" lon=\"6.492\" hae=\"-7.576\" ce=\"1.9510\" le=\"0.241654\"/>";
    test += "<detail>";
    test += "<takv os=\"30\" version=\"4.6.1.3 (eeda83c0).1652809158 - CIV\" device=\"SAMSUNG SM - T870\" platform=\"ATAK - CIV\"/>";
    test += "<contact endpoint=\"tcpsrcreply:4242:srctcp\" callsign=\"HOOPER\"/>";
    test += "<uid Droid=\"HOOPER\"/>";
    test += "<precisionlocation altsrc=\"DTED2\" geopointsrc=\"USER\"/>";
    test += "<__group role=\"Team Member\" name=\"Cyan\"/>";
    test += "<status battery=\"92\"/>";
    test += "<track course=\"342.6135387858228\" speed=\"10.0\"/>";
    test += "</detail>";
    test += "</event>";

    COTSchema cot;
    COTParser c;
    int num = c.ParseCOT(test, cot);
    std::cout << cot;
    return 0;
}