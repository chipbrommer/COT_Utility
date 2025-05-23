COMING SOON - CoT Utility v0.5.0.
Currently in work: 
    Free function style for a more modern and flexible usecase with a single file include. 

___________________________________________________________________________

CoT Utility v0.4.2.
Update: 
    Significantly updated the abilities of each subschema to parse into and out of XML format directly. 
    Added ability to add custom fields in the Detail subschema with attributes and content.
    Added ability to set timestamps from system clock
    Further expend the return result from function to include error code and description

___________________________________________________________________________

CoT Utility v0.3.3.
Updates: 
  Fix compile issues for Linux;
  Add support for dedicated returned error types
  Add support for new sub schemas and additional information: 
    Color
    StrokeColor
    FillColor
    UserIcon
    Remarks
    Model
    Links (Multiple links in single a detail supported)
___________________________________________________________________________

CoT Utility v0.2.0. 

This project was originally created to parse XML CoT messages being received from an external source into my C++11 and legacy C applications under the name COT_Parser.

Since, I have decided to expand upon my initial creation to transition this into more of a full CoT utility that will parse and generate XML CoT messages with the hope that other users can find use for this library to implement their applications with WinTAK, ATAK, or other CoT related communication items.

This project has a good base and I would like to expand more on it if outside users have needs / requests for addition support of desired fields. 
Feel free to submit issues for enhancements. 

Examples:
Please see the 'examples.cpp' for a list of use case scenarios I have created. 
