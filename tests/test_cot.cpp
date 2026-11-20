#include <cmath>
#include <sstream>
#include <string>
#include <string_view>

#include <gtest/gtest.h>

#include "cot_utility.hpp"

namespace {

    constexpr double kDefaultEps = 1e-6;

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

    TEST(CotUtilityTest, VersionVerifyPrepValidateAndAck) {
        EXPECT_FALSE(cot::get_version().empty());

        cot::result r;
        EXPECT_TRUE(cot::verify_xml("<root><child/></root>", &r).is_success());
        EXPECT_TRUE(cot::verify_xml("", &r).is_failed());
        EXPECT_TRUE(r.is_failed());

        std::string noisy =
            "GARBAGE_PREFIX_123<event version=\"2.0\" uid=\"U1\" type=\"a-f-G\" "
            "time=\"2026-11-19T23:55:00Z\" start=\"2026-11-19T23:55:00Z\" "
            "stale=\"2026-11-20T00:05:00Z\" how=\"m-g\">"
            "<point lat=\"30.0\" lon=\"-86.0\" hae=\"0.0\" ce=\"1.0\" le=\"1.0\"/></event>";
        EXPECT_TRUE(cot::prep_xml_message(noisy, &r).is_success());
        ASSERT_FALSE(noisy.empty());
        EXPECT_EQ(noisy.front(), '<');

        EXPECT_TRUE(cot::validate_cot_structure(noisy, &r));

        // Multiple <event> elements rejected
        const std::string dup_event = noisy + noisy;
        EXPECT_FALSE(cot::validate_cot_structure(dup_event, &r));

        // Acknowledgment helper
        const std::string with_status = make_event(R"(<status readiness="true"/>)");
        std::string ack_xml;
        EXPECT_TRUE(cot::acknowledge_received_cot_message(with_status, ack_xml, &r).is_success());
        EXPECT_NE(ack_xml.find("acknowledgment=\"ack\""), std::string::npos);

        // Second ack on already-acknowledged message should report NoModificationMade
        std::string ack_again;
        EXPECT_TRUE(cot::acknowledge_received_cot_message(ack_xml, ack_again, &r).is_failed());
    }

    TEST(CotLinkSubschemaTest, ParseValidateAndRoundTrip) {
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

        cot::subschemas::message msg;
        ASSERT_TRUE(cot::parse_cot(make_event(detail_xml), msg).is_success());
        ASSERT_EQ(msg.event_.detail_.links.size(), 2u);

        const auto& l1 = msg.event_.detail_.links[0];
        EXPECT_EQ(l1.uid, "LNK-1");
        EXPECT_EQ(l1.type, "a-f-G-E-V");
        EXPECT_EQ(l1.relation, "p-p");
        EXPECT_EQ(l1.callsign, "CALL-1");
        EXPECT_EQ(l1.parent_callsign, "PARENT-1");
        EXPECT_EQ(l1.device, "TAK-PC");
        EXPECT_EQ(l1.event_ref, "EV-99");
        EXPECT_EQ(l1.mime, "application/octet-stream");
        EXPECT_EQ(l1.mission, "MISSION-ALPHA");
        EXPECT_EQ(l1.production_time, "2026-11-19T22:00:00Z");
        EXPECT_EQ(l1.remarks, "Full link test");
        EXPECT_EQ(l1.url, "https://example.invalid/item");
        EXPECT_TRUE(l1.has_point());
        EXPECT_NEAR(l1.latitude, 30.123456, kDefaultEps);
        EXPECT_NEAR(l1.longitude, -86.654321, kDefaultEps);

        const auto& l2 = msg.event_.detail_.links[1];
        EXPECT_EQ(l2.uid, "LNK-2");
        EXPECT_EQ(l2.relation, "r-f-h-c");
        EXPECT_FALSE(l2.has_point());
        EXPECT_TRUE(std::isnan(l2.latitude));
        EXPECT_TRUE(std::isnan(l2.longitude));

        std::string xml_out;
        ASSERT_TRUE(cot::generate_xml_cot(msg, xml_out).is_success());

        cot::subschemas::message msg_roundtrip;
        ASSERT_TRUE(cot::parse_cot(xml_out, msg_roundtrip).is_success());
        EXPECT_EQ(msg_roundtrip, msg);

        // Stream output sanity check
        std::ostringstream oss;
        oss << l1;
        EXPECT_FALSE(oss.str().empty());
    }

    TEST(CotChatSubschemaTest, ChatAndChatReceiptRoundTripAndValidation) {
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

        cot::subschemas::message msg;
        ASSERT_TRUE(
            cot::parse_cot(make_event(detail_xml, "GeoChat.UID-ALPHA.OpsRoom.m-101", "b-t-f"), msg).is_success());
        EXPECT_TRUE(msg.event_.detail_.chat_.has_data());
        EXPECT_TRUE(msg.event_.detail_.chatreceipt_.has_data());
        EXPECT_EQ(msg.event_.detail_.chat_.chatroom, "OpsRoom");
        EXPECT_EQ(msg.event_.detail_.chat_.senderCallsign, "ALPHA-6");
        EXPECT_EQ(msg.event_.detail_.chat_.chatgrp.uid0, "UID-ALPHA");
        EXPECT_EQ(msg.event_.detail_.chat_.chatgrp.extra_attributes.at("role"), "sender");
        EXPECT_EQ(msg.event_.detail_.chatreceipt_.senderCallsign, "BRAVO-1");
        EXPECT_EQ(msg.event_.detail_.chatreceipt_.messageId, "m-101");

        std::string xml_out;
        ASSERT_TRUE(cot::generate_xml_cot(msg, xml_out).is_success());

        cot::subschemas::message msg_roundtrip;
        ASSERT_TRUE(cot::parse_cot(xml_out, msg_roundtrip).is_success());
        EXPECT_EQ(msg_roundtrip.event_.detail_.chat_, msg.event_.detail_.chat_);
        EXPECT_EQ(msg_roundtrip.event_.detail_.chatreceipt_, msg.event_.detail_.chatreceipt_);

        // Missing required senderCallsign should fail
        cot::subschemas::message bad_msg;
        EXPECT_TRUE(cot::parse_cot(make_event(R"(<__chat chatroom="OpsRoom"/>)"), bad_msg).is_failed());
    }

    TEST(CotSpatialSubschemaTest, SpatialFullPartialAndInvalidCases) {
        const std::string detail_xml = R"(
        <spatial version="1">
            <attitude pitch="8.25" roll="-3.5" yaw="210.0" ePitch="0.05" eRoll="0.05" eYaw="0.2"/>
            <spin pitch="0.1" roll="-0.1" yaw="2.0" ePitch="0.01" eRoll="0.01" eYaw="0.05"/>
        </spatial>
    )";

        cot::subschemas::message msg;
        ASSERT_TRUE(cot::parse_cot(make_event(detail_xml), msg).is_success());
        EXPECT_TRUE(msg.event_.detail_.spatial_.has_data());
        EXPECT_NEAR(msg.event_.detail_.spatial_.version, 1.0, kDefaultEps);
        EXPECT_NEAR(msg.event_.detail_.spatial_.attitude.pitch, 8.25, kDefaultEps);
        EXPECT_NEAR(msg.event_.detail_.spatial_.attitude.roll, -3.5, kDefaultEps);
        EXPECT_NEAR(msg.event_.detail_.spatial_.attitude.yaw, 210.0, kDefaultEps);
        EXPECT_NEAR(msg.event_.detail_.spatial_.spin.yaw, 2.0, kDefaultEps);

        std::string xml_out;
        ASSERT_TRUE(cot::generate_xml_cot(msg, xml_out).is_success());

        cot::subschemas::message msg_roundtrip;
        ASSERT_TRUE(cot::parse_cot(xml_out, msg_roundtrip).is_success());
        EXPECT_EQ(msg_roundtrip.event_.detail_.spatial_, msg.event_.detail_.spatial_);

        // Partial <spatial> with only <attitude> and pitch/roll
        const std::string partial_xml = R"(
        <spatial>
            <attitude pitch="0.0" roll="0.0"/>
        </spatial>
    )";
        cot::subschemas::message partial_msg;
        ASSERT_TRUE(cot::parse_cot(make_event(partial_xml), partial_msg).is_success());
        EXPECT_TRUE(partial_msg.event_.detail_.spatial_.attitude.has_data());
        EXPECT_FALSE(partial_msg.event_.detail_.spatial_.spin.has_data());

        // <attitude> missing required roll should fail
        cot::subschemas::message bad_spatial;
        EXPECT_TRUE(cot::parse_cot(make_event(R"(<spatial><attitude pitch="1.0"/></spatial>)"), bad_spatial).is_failed());
    }

} // namespace