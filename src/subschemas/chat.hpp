#pragma once
#include <map>
#include <ostream>
#include <string>
#include <string_view>
#include <utility>
#include <pugixml.hpp>

namespace cot::subschemas
{
    /// @brief Represents the <chatgrp> child element inside __chat or __chatreceipt
    struct chat_group {
        std::string id;                                     ///< Group identifier attribute
        std::string uid0;                                   ///< Primary participant or sender UID
        std::string uid1;                                   ///< Secondary participant or recipient UID
        std::map<std::string, std::string> extra_attributes; ///< Additional unrecognized XML attributes on <chatgrp>

        /// @brief Equality comparison for chat_group
        /// @param other Another chat_group instance to compare against
        /// @return True if all fields and extra attributes match
        bool operator==(const chat_group& other) const noexcept {
            return id == other.id &&
                uid0 == other.uid0 &&
                uid1 == other.uid1 &&
                extra_attributes == other.extra_attributes;
        }

        /// @brief Inequality comparison for chat_group
        /// @param other Another chat_group instance to compare against
        /// @return True if any field or extra attribute differs
        bool operator!=(const chat_group& other) const noexcept { return !(*this == other); }

        /// @brief Checks whether any <chatgrp> fields or extra attributes are populated
        /// @return True if at least one attribute is non-empty, false otherwise
        [[nodiscard]] bool has_data() const noexcept {
            return !id.empty() || !uid0.empty() || !uid1.empty() || !extra_attributes.empty();
        }
    };

    /// @brief A CoT Message subschema class for <__chat> and <__chatreceipt> elements
    class chat {
    public:
        /// @brief Constructor - Initializes everything
        explicit chat(std::string chatroom = INVALID_STRING,
            std::string sender_callsign = INVALID_STRING,
            std::string group_owner = INVALID_STRING,
            std::string id = INVALID_STRING,
            std::string message_id = INVALID_STRING,
            std::string parent = INVALID_STRING,
            chat_group grp = chat_group{}) noexcept
            : chatroom(std::move(chatroom)),
            senderCallsign(std::move(sender_callsign)),
            groupOwner(std::move(group_owner)),
            id(std::move(id)),
            messageId(std::move(message_id)),
            parent(std::move(parent)),
            chatgrp(std::move(grp)) {
        }

        /// @brief Equality comparison
        bool operator==(const chat& other) const noexcept {
            return chatroom == other.chatroom &&
                senderCallsign == other.senderCallsign &&
                groupOwner == other.groupOwner &&
                id == other.id &&
                messageId == other.messageId &&
                parent == other.parent &&
                chatgrp == other.chatgrp;
        }

        /// @brief Inequality comparison
        bool operator!=(const chat& other) const noexcept { return !(*this == other); }

        /// @brief Checks if any chat fields are populated
        [[nodiscard]] bool has_data() const noexcept {
            return !chatroom.empty() || !senderCallsign.empty() || !groupOwner.empty() ||
                !id.empty() || !messageId.empty() || !parent.empty() || chatgrp.has_data();
        }

        /// @brief Checks if the class has valid data
        /// @param rslt Optional result structure for invalid reason
        /// @return True if valid, false otherwise
        bool is_valid(result* rslt = nullptr) const {
            if (!has_data()) {
                auto res = result{ result::error_code::InvalidChat, "Empty chat data" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            if (chatroom.empty()) {
                auto res = result{ result::error_code::InvalidChat, "Missing required chatroom attribute" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            if (senderCallsign.empty()) {
                auto res = result{ result::error_code::InvalidChat, "Missing required senderCallsign attribute" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            return true;
        }

        /// @brief Serializes to XML under the specified tag name ("__chat" or "__chatreceipt")
        /// @param parent_node The parent XML node to append to
        /// @param tag_name XML element tag name (defaults to "__chat")
        /// @return Result indicating success or failure
        result to_xml(pugi::xml_node& parent_node, std::string_view tag_name = "__chat") const {
            if (tag_name != "__chat" && tag_name != "__chatreceipt" && tag_name != "__chatReceipt") {
                auto res = result{ result::error_code::InvalidChat,
                                  "Unsupported chat tag name: " + std::string(tag_name) };
                if (error_handler) error_handler(res);
                return res;
            }

            result val_res;
            if (!is_valid(&val_res)) {
                auto res = result{ result::error_code::InvalidChat,
                                  val_res.description().empty()
                                      ? std::string("Invalid chat data")
                                      : val_res.description() };
                if (error_handler) error_handler(res);
                return res;
            }

            std::string tag_str(tag_name);
            auto node = parent_node.append_child(tag_str.c_str());
            node.append_attribute("chatroom") = chatroom.c_str();
            node.append_attribute("senderCallsign") = senderCallsign.c_str();
            if (!groupOwner.empty()) node.append_attribute("groupOwner") = groupOwner.c_str();
            if (!id.empty()) node.append_attribute("id") = id.c_str();
            if (!messageId.empty()) node.append_attribute("messageId") = messageId.c_str();
            if (!parent.empty()) node.append_attribute("parent") = parent.c_str();

            if (chatgrp.has_data()) {
                auto grp_node = node.append_child("chatgrp");
                if (!chatgrp.id.empty()) grp_node.append_attribute("id") = chatgrp.id.c_str();
                if (!chatgrp.uid0.empty()) grp_node.append_attribute("uid0") = chatgrp.uid0.c_str();
                if (!chatgrp.uid1.empty()) grp_node.append_attribute("uid1") = chatgrp.uid1.c_str();
                for (const auto& [k, v] : chatgrp.extra_attributes) {
                    if (!k.empty()) grp_node.append_attribute(k.c_str()) = v.c_str();
                }
            }

            return result{};
        }

        /// @brief Deserializes from an XML node
        /// @param node The XML node to parse
        /// @param rslt Optional result for error details
        /// @param expected_tag Optional expected tag name; accepts "__chat", "__chatreceipt", and "__chatReceipt" when empty
        /// @return Deserialized chat
        static chat from_xml(const pugi::xml_node& node,
            result* rslt = nullptr,
            std::string_view expected_tag = {}) {
            chat c;
            const std::string_view actual_name = node.name();
            const bool is_known_chat_tag = (actual_name == "__chat" ||
                actual_name == "__chatreceipt" ||
                actual_name == "__chatReceipt");

            if (!expected_tag.empty()) {
                if (actual_name != expected_tag) {
                    auto res = result{ result::error_code::InvalidChat,
                                      "Expected <" + std::string(expected_tag) + "> node" };
                    if (rslt) *rslt = res;
                    if (error_handler) error_handler(res);
                    return c;
                }
            }
            else if (!is_known_chat_tag) {
                auto res = result{ result::error_code::InvalidChat,
                                  "Expected <__chat> or <__chatreceipt> node" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return c;
            }

            try {
                if (auto attr = node.attribute("chatroom")) c.chatroom = attr.as_string();
                if (auto attr = node.attribute("senderCallsign")) c.senderCallsign = attr.as_string();
                if (auto attr = node.attribute("groupOwner")) c.groupOwner = attr.as_string();
                if (auto attr = node.attribute("id")) c.id = attr.as_string();
                if (auto attr = node.attribute("messageId")) c.messageId = attr.as_string();
                if (auto attr = node.attribute("parent")) c.parent = attr.as_string();

                if (auto grp_node = node.child("chatgrp")) {
                    for (auto attr : grp_node.attributes()) {
                        const std::string_view attr_name = attr.name();
                        if (attr_name == "id") c.chatgrp.id = attr.as_string();
                        else if (attr_name == "uid0") c.chatgrp.uid0 = attr.as_string();
                        else if (attr_name == "uid1") c.chatgrp.uid1 = attr.as_string();
                        else if (!attr_name.empty()) {
                            c.chatgrp.extra_attributes[std::string(attr_name)] = attr.as_string();
                        }
                    }
                }

                result val_res;
                if (!c.is_valid(&val_res) && rslt && rslt->is_success()) {
                    *rslt = val_res;
                }
            }
            catch (const std::exception& ex) {
                auto res = result{ result::error_code::InvalidChat,
                                  "Error parsing chat element: " + std::string(ex.what()) };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
            }

            return c;
        }

        /// @brief Stream output for printing
        friend std::ostream& operator<<(std::ostream& os, const chat& c) {
            os << "chat: ";
            if (!c.is_valid()) os << " -NOT VALID- ";
            os << "\n\tchatroom: " << (c.chatroom.empty() ? "None" : c.chatroom)
                << "\n\tsenderCallsign: " << (c.senderCallsign.empty() ? "None" : c.senderCallsign)
                << "\n\tgroupOwner: " << (c.groupOwner.empty() ? "None" : c.groupOwner)
                << "\n\tid: " << (c.id.empty() ? "None" : c.id)
                << "\n\tmessageId: " << (c.messageId.empty() ? "None" : c.messageId)
                << "\n\tparent: " << (c.parent.empty() ? "None" : c.parent)
                << "\n\tchatgrp: " << (c.chatgrp.has_data() ? "present" : "None")
                << "\n";
            return os;
        }

        std::string chatroom{ INVALID_STRING };
        std::string senderCallsign{ INVALID_STRING };
        std::string groupOwner{ INVALID_STRING };
        std::string id{ INVALID_STRING };
        std::string messageId{ INVALID_STRING };
        std::string parent{ INVALID_STRING };
        chat_group chatgrp{};

    private:
        inline static constexpr const char* INVALID_STRING = "";
    };
}