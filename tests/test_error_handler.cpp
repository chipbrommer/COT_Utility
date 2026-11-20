#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

#include <gtest/gtest.h>

#include "cot_utility.hpp"

namespace {

    std::string_view identify_error_source(const cot::result& res) noexcept {
        const std::string desc = res.to_string();

        if (desc.find("takv") != std::string::npos) return "takv";
        if (desc.find("contact") != std::string::npos) return "contact";
        if (desc.find("uid") != std::string::npos) return "uid";
        if (desc.find("model") != std::string::npos) return "model";
        if (desc.find("precisionlocation") != std::string::npos) return "precisionlocation";
        if (desc.find("group") != std::string::npos) return "group";
        if (desc.find("status") != std::string::npos) return "status";
        if (desc.find("track") != std::string::npos) return "track";
        if (desc.find("color") != std::string::npos) return "color";
        if (desc.find("fillcolor") != std::string::npos) return "fillcolor";
        if (desc.find("strokecolor") != std::string::npos) return "strokecolor";
        if (desc.find("strokestyle") != std::string::npos) return "strokestyle";
        if (desc.find("strokeweight") != std::string::npos) return "strokeweight";
        if (desc.find("image") != std::string::npos) return "image";
        if (desc.find("inclination") != std::string::npos) return "inclination";
        if (desc.find("labels_on") != std::string::npos) return "labels_on";
        if (desc.find("tog") != std::string::npos) return "tog";
        if (desc.find("rangeunits") != std::string::npos) return "rangeunits";
        if (desc.find("range") != std::string::npos) return "range";
        if (desc.find("usericon") != std::string::npos) return "usericon";
        if (desc.find("remarks") != std::string::npos) return "remarks";
        if (desc.find("link_attr") != std::string::npos) return "link_attr";
        if (desc.find("milicon") != std::string::npos) return "milicon";
        if (desc.find("milsym") != std::string::npos) return "milsym";
        if (desc.find("creator") != std::string::npos) return "creator";
        if (desc.find("display") != std::string::npos) return "display";
        if (desc.find("emergency") != std::string::npos) return "emergency";
        if (desc.find("address") != std::string::npos) return "address";
        if (desc.find("takcontrol") != std::string::npos || desc.find("TakControl") != std::string::npos) {
            return "takcontrol";
        }
        if (desc.find("ackrequest") != std::string::npos) return "ackrequest";
        if (desc.find("chatreceipt") != std::string::npos || desc.find("__chatreceipt") != std::string::npos) {
            return "chatreceipt";
        }
        if (desc.find("chat") != std::string::npos || desc.find("__chat") != std::string::npos) {
            return "chat";
        }
        if (desc.find("spatial") != std::string::npos) return "spatial";
        if (desc.find("link") != std::string::npos) return "link";
        if (desc.find("point") != std::string::npos) return "point";
        if (desc.find("event") != std::string::npos) return "event";
        if (desc.find("detail") != std::string::npos) return "detail";

        return "unknown / top-level";
    }

    class CotErrorHandlerTest : public ::testing::Test {
    protected:
        std::vector<cot::result> captured_errors;

        void SetUp() override {
            captured_errors.clear();
            cot::set_error_handler([this](const cot::result& res) {
                if (res.is_failed()) {
                    captured_errors.push_back(res);
                }
                });
        }

        void TearDown() override {
            cot::set_error_handler(nullptr);
            captured_errors.clear();
        }
    };

    TEST_F(CotErrorHandlerTest, DefaultNullHandlerDoesNotCrashOnFailure) {
        cot::set_error_handler(nullptr);

        cot::result r;
        EXPECT_TRUE(cot::verify_xml("", &r).is_failed());
        EXPECT_TRUE(r.is_failed());
    }

    TEST_F(CotErrorHandlerTest, SetErrorHandlerReplacesExistingCallback) {
        std::vector<std::string> first_log;
        std::vector<std::string> second_log;

        cot::set_error_handler([&first_log](const cot::result& res) {
            if (res.is_failed()) {
                first_log.push_back(res.to_string());
            }
            });

        cot::result r;
        EXPECT_TRUE(cot::verify_xml("", &r).is_failed());
        EXPECT_EQ(first_log.size(), 1u);
        EXPECT_TRUE(second_log.empty());

        cot::set_error_handler([&second_log](const cot::result& res) {
            if (res.is_failed()) {
                second_log.push_back(res.to_string());
            }
            });

        EXPECT_TRUE(cot::verify_xml("<unclosed>", &r).is_failed());
        EXPECT_EQ(first_log.size(), 1u);
        EXPECT_EQ(second_log.size(), 1u);
    }

    TEST_F(CotErrorHandlerTest, InvokedOnInvalidXmlAndParseFailures) {
        cot::result r;
        EXPECT_TRUE(cot::verify_xml("", &r).is_failed());
        EXPECT_FALSE(captured_errors.empty());

        const std::size_t count_after_verify = captured_errors.size();
        cot::message msg;
        EXPECT_TRUE(cot::parse_cot(R"(<event version="2.0"><__chat chatroom="OpsRoom"/></event>)", msg).is_failed());
        EXPECT_GT(captured_errors.size(), count_after_verify);
    }

    TEST_F(CotErrorHandlerTest, OmittedOptionalDetailSubschemasDoNotTriggerErrors) {
        const std::string valid_without_optional_subschemas = R"(<?xml version="1.0" encoding="UTF-8"?>
<event version="2.0" uid="UAV-ALPHA-07" type="a-f-A-M-F-Q"
       time="2026-11-19T23:55:00Z" start="2026-11-19T23:55:00Z"
       stale="2026-11-20T00:05:00Z" how="m-g">
    <point lat="30.451500" lon="-86.489700" hae="450.0" ce="3.5" le="4.0"/>
    <detail>
        <spatial version="1">
            <attitude pitch="2.5" roll="-1.0" yaw="135.0"/>
        </spatial>
    </detail>
</event>)";

        cot::message msg;
        ASSERT_TRUE(cot::parse_cot(valid_without_optional_subschemas, msg).is_success());

        std::string xml_out;
        ASSERT_TRUE(cot::generate_xml_cot(msg, xml_out).is_success());

        for (std::size_t i = 0; i < captured_errors.size(); ++i) {
            const auto& err = captured_errors[i];
            ADD_FAILURE() << "Captured error [" << i << "] "
                << "(suspected source: " << identify_error_source(err) << "): "
                << err.to_string();
        }

        EXPECT_TRUE(captured_errors.empty());
    }

    TEST_F(CotErrorHandlerTest, DirectAssignmentToErrorHandlerWorks) {
        int invocations = 0;
        cot::error_handler = [&invocations](const cot::result& res) {
            if (res.is_failed()) {
                ++invocations;
            }
            };

        cot::result r;
        EXPECT_TRUE(cot::verify_xml("", &r).is_failed());
        EXPECT_EQ(invocations, 1);
    }

} // namespace