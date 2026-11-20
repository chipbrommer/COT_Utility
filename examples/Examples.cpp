#include <iostream>
#include <string>

#include "cot.hpp"

int main() {
    // Optional centralized error observer for subschema & utility failures
    cot::error_handler = [](const cot::result& res) {
        if (res.is_failed()) {
            std::cerr << "[cot error] " << res.to_string() << "\n";
        }
        };

    const std::string incoming_cot = R"(<?xml version="1.0" encoding="UTF-8"?>
<event version="2.0" uid="UAV-ALPHA-07" type="a-f-A-M-F-Q"
       time="2026-11-19T23:55:00Z" start="2026-11-19T23:55:00Z" stale="2026-11-20T00:05:00Z"
       how="m-g">
    <point lat="30.451500" lon="-86.489700" hae="450.0" ce="3.5" le="4.0"/>
    <detail>
        <spatial version="1">
            <attitude pitch="2.5" roll="-1.0" yaw="135.0" ePitch="0.1" eRoll="0.1" eYaw="0.4"/>
            <spin pitch="0.0" roll="0.0" yaw="0.25"/>
        </spatial>
        <link uid="BASE-STATION-01" type="a-f-G-E-V" relation="p-p"
              callsign="OPS-BASE" mission="GRID-07" point="30.420000,-86.510000"/>
        <link uid="DOC-MISSION-BRIEF" relation="r-f-h-c"
              url="https://example.invalid/briefings/grid-07.pdf"/>
        <__chat chatroom="AirOps" senderCallsign="UAV-ALPHA-07"
                id="room-airops" messageId="msg-771" parent="RootContactGroup">
            <chatgrp id="room-airops" uid0="UAV-ALPHA-07" uid1="AirOps"/>
        </__chat>
        <__chatreceipt chatroom="AirOps" senderCallsign="OPS-BASE"
                       id="room-airops" messageId="msg-771" parent="RootContactGroup">
            <chatgrp id="room-airops" uid0="OPS-BASE" uid1="UAV-ALPHA-07"/>
        </__chatreceipt>
    </detail>
</event>)";

    cot::message msg;
    cot::result parse_res = cot::parse_cot(incoming_cot, msg);
    if (parse_res.is_failed()) {
        std::cerr << "Failed to parse CoT message: " << parse_res.to_string() << "\n";
        return 1;
    }

    std::cout << "Parsed message:\n" << msg << "\n";

    // Example modification: add a follow-up link and generate outbound XML
    cot::subschemas::link waypoint_link;
    waypoint_link.uid = "WP-08";
    waypoint_link.type = "b-m-p-w";
    waypoint_link.relation = "c-f-t";
    waypoint_link.callsign = "TURN-POINT-8";
    waypoint_link.latitude = 30.475000;
    waypoint_link.longitude = -86.460000;
    msg.event_.detail_.links.push_back(waypoint_link);

    std::string outbound_xml;
    cot::result gen_res = cot::generate_xml_cot(msg, outbound_xml);
    if (gen_res.is_failed()) {
        std::cerr << "Failed to generate XML: " << gen_res.to_string() << "\n";
        return 1;
    }

    std::cout << "Generated outbound XML:\n" << outbound_xml << "\n";
    return 0;
}