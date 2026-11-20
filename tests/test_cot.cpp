#include <cassert>
#include <cmath>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>

#include "cot.hpp"

namespace
{
    int g_failures = 0;

    void check(bool condition, const char* expr, const char* file, int line) {
        if (!condition) {
            ++g_failures;
            std::cerr << "FAIL [" << file << ":" << line << "]: " << expr << "\n";
        }
    }

#define CHECK(expr) check((expr), #expr, __FILE__, __LINE__)

    inline bool nearly_equal(double a, double b, double eps = 1e-6) noexcept {
        if (std::isnan(a) && std::isnan(b)) return true;
        if (!std::isfinite(a) || !std::isfinite(b)) return false;
        return std::abs(a - b) <= eps;
    }

    std::string make_event(std::string_view detail_xml,
        std::string_view uid = "UID-001",
        std::string_view type = "a-f-G-U-C",
        std::string_view how = "m-g") {
        std::string xml;
        xml += R"(<?xml version="1.0" encoding="UTF-8"?>)";
        xml += R"(<event version="2.0" uid=")" + std::string(uid) +
            R"(" type=")" + std::string(type) +
            R"(" time="2026-11-19T23:55:00Z" start="2026-11-19T23:55:00Z" stale="2026-11-20T00:05:00Z" how=")" +
            std::string(how) + R"(">)";
        xml += R"(<point lat="30.451500" lon="-86.489700" hae="10.0" ce="5.0" le="5.0"/>)";
        xml += R"(<detail>)";
        xml += detail_xml;
        xml += R"(</detail></event>)";
        return xml;
    }

    void test_utility_functions() {
        CHECK(!cot::get_version().empty());

        cot::result r;
        CHECK(cot::verify_xml("<root><child/></root>", &r).is_success());
        CHECK(cot::verify_xml("", &r).is_failed());
        CHECK(r.is_failed());

        std::string noisy = "GARBAGE_PREFIX_123<event version=\"2.0\" uid=\"U1\" type=\"a-f-G\" time=\"2026-11-19T23:55:00Z\" start=\"2026-11-19T23:55:00Z\" stale=\"2026-11-20T00:05:00Z\" how=\"m-g\"><point lat=\"30.0\" lon=\"-86.0\" hae=\"0.0\" ce=\"1.0\" le=\"1.0\"/></event>";
        CHECK(cot::prep_xml_message(noisy, &r).is_success());
        CHECK(noisy.front() == '<');

        CHECK(cot::validate_cot_structure(noisy, &r));

        // Multiple <event> elements rejected
        const std::string dup_event = noisy + noisy;
        CHECK(!cot::validate_cot_structure(dup_event, &r));

        // Acknowledgment helper
        const std::string with_status = make_event(R"(<status readiness="true"/>)");
        std::string ack_xml;
        CHECK(cot::acknowledge_received_cot_message(with_status, ack_xml, &r).is_success());
        CHECK(ack_xml.find("acknowledgment=\"ack\"") != std::string::npos);

        // Second ack on already-acknowledged message should report NoModificationMade
        std::string ack_again;
        CHECK(cot::acknowledge_received_cot_message(ack_xml, ack_again, &r).is_failed());
    }

    void test_link_subschema_and_round_trip() {
        const std::string detail_xml = R"(
            <link uid="LNK-1"
                  type="a-f-G-E-V"
                  relation="p-p"
                  callsign="CALL-1"
                  parent_callsign="PARENT-1"
                  device="TAK-PC"
                  event="EV-99"
                  mime="application/octet-stream"
                  mission="MISSION-ALPHA"
                  production_time="2026-11-19T22:00:00Z"
                  remarks="Full link test"
                  url="https://example.invalid/item"
                  point="30.123456,-86.654321"/>
            <link uid="LNK-2"
                  relation="r-f-h-c"
                  mission="MISSION-ALPHA"/>
        )";

        cot::message msg;
        CHECK(cot::parse_cot(make_event(detail_xml), msg).is_success());
        CHECK(msg.event_.detail_.links.size() == 2);

        const auto& l1 = msg.event_.detail_.links[0];
        CHECK(l1.uid == "LNK-1");
        CHECK(l1.type == "a-f-G-E-V");
        CHECK(l1.relation == "p-p");
        CHECK(l1.callsign == "CALL-1");
        CHECK(l1.parent_callsign == "PARENT-1");
        CHECK(l1.device == "TAK-PC");
        CHECK(l1.event_ref == "EV-99");
        CHECK(l1.mime == "application/octet-stream");
        CHECK(l1.mission == "MISSION-ALPHA");
        CHECK(l1.production_time == "2026-11-19T22:00:00Z");
        CHECK(l1.remarks == "Full link test");
        CHECK(l1.url == "https://example.invalid/item");
        CHECK(l1.has_point());
        CHECK(nearly_equal(l1.latitude, 30.123456));
        CHECK(nearly_equal(l1.longitude, -86.654321));

        const auto& l2 = msg.event_.detail_.links[1];
        CHECK(l2.uid == "LNK-2");
        CHECK(l2.relation == "r-f-h-c");
        CHECK(!l2.has_point());
        CHECK(std::isnan(l2.latitude));
        CHECK(std::isnan(l2.longitude));

        std::string xml_out;
        CHECK(cot::generate_xml_cot(msg, xml_out).is_success());

        cot::message msg_roundtrip;
        CHECK(cot::parse_cot(xml_out, msg_roundtrip).is_success());
        CHECK(msg_roundtrip == msg);

        // Stream output sanity check
        std::ostringstream oss;
        oss << l1;
        CHECK(!oss.str().empty());
    }

    void test_chat_and_chatreceipt_round_trip() {
        const std::string detail_xml = R"(
            <__chat chatroom="OpsRoom"
                    senderCallsign="ALPHA-6"
                    groupOwner="true"
                    id="chat-id-1"
                    messageId="m-101"
                    parent="RootContactGroup">
                <chatgrp id="chat-id-1" uid0="UID-ALPHA" uid1="OpsRoom" role="sender"/>
            </__chat>
            <__chatreceipt chatroom="OpsRoom"
                           senderCallsign="BRAVO-1"
                           groupOwner="false"
                           id="chat-id-1"
                           messageId="m-101"
                           parent="RootContactGroup">
                <chatgrp id="chat-id-1" uid0="UID-BRAVO" uid1="UID-ALPHA"/>
            </__chatreceipt>
        )";

        cot::message msg;
        CHECK(cot::parse_cot(make_event(detail_xml, "GeoChat.UID-ALPHA.OpsRoom.m-101", "b-t-f"), msg).is_success());
        CHECK(msg.event_.detail_.chat_.has_data());
        CHECK(msg.event_.detail_.chatreceipt_.has_data());
        CHECK(msg.event_.detail_.chat_.chatroom == "OpsRoom");
        CHECK(msg.event_.detail_.chat_.senderCallsign == "ALPHA-6");
        CHECK(msg.event_.detail_.chat_.chatgrp.uid0 == "UID-ALPHA");
        CHECK(msg.event_.detail_.chat_.chatgrp.extra_attributes.at("role") == "sender");
        CHECK(msg.event_.detail_.chatreceipt_.senderCallsign == "BRAVO-1");
        CHECK(msg.event_.detail_.chatreceipt_.messageId == "m-101");

        std::string xml_out;
        CHECK(cot::generate_xml_cot(msg, xml_out).is_success());

        cot::message msg_roundtrip;
        CHECK(cot::parse_cot(xml_out, msg_roundtrip).is_success());
        CHECK(msg_roundtrip.event_.detail_.chat_ == msg.event_.detail_.chat_);
        CHECK(msg_roundtrip.event_.detail_.chatreceipt_ == msg.event_.detail_.chatreceipt_);

        // Missing required senderCallsign should fail
        cot::message bad_msg;
        CHECK(cot::parse_cot(make_event(R"(<__chat chatroom="OpsRoom"/>)"), bad_msg).is_failed());
    }

    void test_spatial_round_trip() {
        const std::string detail_xml = R"(
            <spatial version="1">
                <attitude pitch="8.25" roll="-3.5" yaw="210.0" ePitch="0.05" eRoll="0.05" eYaw="0.2"/>
                <spin pitch="0.1" roll="-0.1" yaw="2.0" ePitch="0.01" eRoll="0.01" eYaw="0.05"/>
            </spatial>
        )";

        cot::message msg;
        CHECK(cot::parse_cot(make_event(detail_xml), msg).is_success());
        CHECK(msg.event_.detail_.spatial_.has_data());
        CHECK(nearly_equal(msg.event_.detail_.spatial_.version, 1.0));
        CHECK(nearly_equal(msg.event_.detail_.spatial_.attitude.pitch, 8.25));
        CHECK(nearly_equal(msg.event_.detail_.spatial_.attitude.roll, -3.5));
        CHECK(nearly_equal(msg.event_.detail_.spatial_.attitude.yaw, 210.0));
        CHECK(nearly_equal(msg.event_.detail_.spatial_.spin.yaw, 2.0));

        std::string xml_out;
        CHECK(cot::generate_xml_cot(msg, xml_out).is_success());

        cot::message msg_roundtrip;
        CHECK(cot::parse_cot(xml_out, msg_roundtrip).is_success());
        CHECK(msg_roundtrip.event_.detail_.spatial_ == msg.event_.detail_.spatial_);

        // Partial <spatial> with only <attitude> and pitch/roll
        const std::string partial_xml = R"(
            <spatial>
                <attitude pitch="0.0" roll="0.0"/>
            </spatial>
        )";
        cot::message partial_msg;
        CHECK(cot::parse_cot(make_event(partial_xml), partial_msg).is_success());
        CHECK(partial_msg.event_.detail_.spatial_.attitude.has_data());
        CHECK(!partial_msg.event_.detail_.spatial_.spin.has_data());

        // <attitude> missing required roll should fail
        cot::message bad_spatial;
        CHECK(cot::parse_cot(make_event(R"(<spatial><attitude pitch="1.0"/></spatial>)"), bad_spatial).is_failed());
    }
} // namespace

int main() {
    test_utility_functions();
    test_link_subschema_and_round_trip();
    test_chat_and_chatreceipt_round_trip();
    test_spatial_round_trip();

    if (g_failures > 0) {
        std::cerr << g_failures << " test check(s) failed.\n";
        return 1;
    }

    std::cout << "All cot tests passed.\n";
    return 0;
}