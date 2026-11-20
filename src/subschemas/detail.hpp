#pragma once
/////////////////////////////////////////////////////////////////////////////////
// @file            detail.hpp
// @brief           A CoT Message subschema class for detail data.
// @author          Chip Brommer
/////////////////////////////////////////////////////////////////////////////////

#include <algorithm>
#include <cmath>
#include <exception>
#include <limits>
#include <map>
#include <ostream>
#include <string>
#include <utility>
#include <vector>

#include <pugixml.hpp>

#include "../handlers/error_handler.hpp"
#include "../result/result.hpp"

#include "ackrequest.hpp"
#include "address.hpp"
#include "chat.hpp"
#include "color.hpp"
#include "contact.hpp"
#include "creator.hpp"
#include "customdetail.hpp"
#include "display.hpp"
#include "emergency.hpp"
#include "fillcolor.hpp"
#include "group.hpp"
#include "image.hpp"
#include "inclination.hpp"
#include "labels_on.hpp"
#include "link.hpp"
#include "link_attr.hpp"
#include "milicon.hpp"
#include "milsym.hpp"
#include "model.hpp"
#include "precisionlocation.hpp"
#include "range.hpp"
#include "rangeunits.hpp"
#include "remarks.hpp"
#include "spatial.hpp"
#include "status.hpp"
#include "strokecolor.hpp"
#include "strokestyle.hpp"
#include "strokeweight.hpp"
#include "takcontrol.hpp"
#include "takv.hpp"
#include "tog.hpp"
#include "track.hpp"
#include "uid.hpp"
#include "usericon.hpp"

namespace cot::subschemas
{
    /// @brief A CoT Message subschema class for detail data
    class detail {
    public:
        /// @brief Constructor - Initializes everything
        detail(takv takv = takv(),
            contact contact = contact(),
            uid uid = uid(),
            model model = model(),
            precisionlocation precisionlocation = precisionlocation(),
            group group = group(),
            status status = status(),
            track track = track(),
            color color = color(),
            fillcolor fillcolor = fillcolor(),
            strokecolor strokecolor = strokecolor(),
            strokestyle strokestyle = strokestyle(),
            strokeweight strokeweight = strokeweight(),
            image image = image(),
            inclination inclination = inclination(),
            labels_on labels_on = labels_on(),
            tog tog = tog(),
            range range = range(),
            rangeunits rangeunits = rangeunits(),
            usericon usericon = usericon(),
            remarks remarks = remarks(),
            link_attr link_attr = link_attr(),
            milicon milicon = milicon(),
            milsym milsym = milsym(),
            creator creator = creator(),
            display display = display(),
            emergency emergency = emergency(),
            address address = address(),
            takcontrol takcontrol = takcontrol(),
            ackrequest ackrequest = ackrequest(),
            chat chat = chat(),
            chat chatreceipt = chat(),
            spatial spatial = spatial(),
            std::vector<link> links = {},
            std::vector<customdetail> custom_details = {})
            : takv_(std::move(takv)),
            contact_(std::move(contact)),
            uid_(std::move(uid)),
            model_(std::move(model)),
            precisionlocation_(std::move(precisionlocation)),
            group_(std::move(group)),
            status_(std::move(status)),
            track_(std::move(track)),
            color_(std::move(color)),
            fillcolor_(std::move(fillcolor)),
            strokecolor_(std::move(strokecolor)),
            strokestyle_(std::move(strokestyle)),
            strokeweight_(std::move(strokeweight)),
            image_(std::move(image)),
            inclination_(std::move(inclination)),
            labels_on_(std::move(labels_on)),
            tog_(std::move(tog)),
            range_(std::move(range)),
            rangeunits_(std::move(rangeunits)),
            usericon_(std::move(usericon)),
            remarks_(std::move(remarks)),
            link_attr_(std::move(link_attr)),
            milicon_(std::move(milicon)),
            milsym_(std::move(milsym)),
            creator_(std::move(creator)),
            display_(std::move(display)),
            emergency_(std::move(emergency)),
            address_(std::move(address)),
            takcontrol_(std::move(takcontrol)),
            ackrequest_(std::move(ackrequest)),
            chat_(std::move(chat)),
            chatreceipt_(std::move(chatreceipt)),
            spatial_(std::move(spatial)),
            links(std::move(links)),
            custom_details(std::move(custom_details)) {
        }

        /// @brief Equality comparison
        bool operator==(const detail& other) const noexcept {
            return takv_ == other.takv_ &&
                contact_ == other.contact_ &&
                uid_ == other.uid_ &&
                model_ == other.model_ &&
                precisionlocation_ == other.precisionlocation_ &&
                group_ == other.group_ &&
                status_ == other.status_ &&
                track_ == other.track_ &&
                color_ == other.color_ &&
                fillcolor_ == other.fillcolor_ &&
                strokecolor_ == other.strokecolor_ &&
                strokestyle_ == other.strokestyle_ &&
                strokeweight_ == other.strokeweight_ &&
                image_ == other.image_ &&
                inclination_ == other.inclination_ &&
                labels_on_ == other.labels_on_ &&
                tog_ == other.tog_ &&
                range_ == other.range_ &&
                rangeunits_ == other.rangeunits_ &&
                usericon_ == other.usericon_ &&
                remarks_ == other.remarks_ &&
                link_attr_ == other.link_attr_ &&
                milicon_ == other.milicon_ &&
                milsym_ == other.milsym_ &&
                creator_ == other.creator_ &&
                display_ == other.display_ &&
                emergency_ == other.emergency_ &&
                address_ == other.address_ &&
                takcontrol_ == other.takcontrol_ &&
                ackrequest_ == other.ackrequest_ &&
                chat_ == other.chat_ &&
                chatreceipt_ == other.chatreceipt_ &&
                spatial_ == other.spatial_ &&
                links == other.links &&
                custom_details == other.custom_details;
        }

        /// @brief Inequality comparison
        bool operator!=(const detail& other) const noexcept { return !(*this == other); }

        [[nodiscard]] bool has_fillcolor_data() const noexcept {
            return fillcolor_ != fillcolor{};
        }
        [[nodiscard]] bool has_strokecolor_data() const noexcept {
            return strokecolor_ != strokecolor{};
        }
        [[nodiscard]] bool has_strokestyle_data() const noexcept {
            return strokestyle_ != strokestyle{};
        }
        [[nodiscard]] bool has_strokeweight_data() const noexcept {
            return strokeweight_ != strokeweight{};
        }
        [[nodiscard]] bool has_image_data() const noexcept {
            return image_ != image{};
        }
        [[nodiscard]] bool has_inclination_data() const noexcept {
            return inclination_ != inclination{};
        }
        [[nodiscard]] bool has_labels_on_data() const noexcept {
            return labels_on_ != labels_on{};
        }
        [[nodiscard]] bool has_tog_data() const noexcept {
            return tog_ != tog{};
        }
        [[nodiscard]] bool has_range_data() const noexcept {
            return range_ != range{};
        }
        [[nodiscard]] bool has_rangeunits_data() const noexcept {
            return rangeunits_ != rangeunits{};
        }
        [[nodiscard]] bool has_link_attr_data() const noexcept {
            return link_attr_ != link_attr{};
        }
        [[nodiscard]] bool has_creator_data() const noexcept {
            return creator_ != creator{};
        }
        [[nodiscard]] bool has_display_data() const noexcept {
            return display_ != display{};
        }
        [[nodiscard]] bool has_emergency_data() const noexcept {
            return emergency_ != emergency{};
        }
        [[nodiscard]] bool has_address_data() const noexcept {
            return address_ != address{};
        }
        [[nodiscard]] bool has_takcontrol_data() const noexcept {
            return takcontrol_.has_data();
        }
        [[nodiscard]] bool has_ackrequest_data() const noexcept {
            return ackrequest_ != ackrequest{};
        }
        [[nodiscard]] bool has_chat_data() const noexcept {
            return chat_ != chat{};
        }
        [[nodiscard]] bool has_chatreceipt_data() const noexcept {
            return chatreceipt_ != chat{};
        }
        [[nodiscard]] bool has_spatial_data() const noexcept {
            return spatial_ != spatial{};
        }

        /// @brief Checks if the class has valid data
        /// @param rslt Optional result structure for invalid reason
        /// @return True if valid, false otherwise
        bool is_valid(result* rslt = nullptr) const {
            if (!contact_.is_valid()) {
                auto res = result{ result::error_code::InvalidDetail, "Invalid contact data" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            if (!group_.is_valid()) {
                auto res = result{ result::error_code::InvalidDetail, "Invalid group data" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            if (!remarks_.is_valid()) {
                auto res = result{ result::error_code::InvalidDetail, "Invalid remarks data" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            if (has_fillcolor_data() && !fillcolor_.is_valid()) {
                auto res = result{ result::error_code::InvalidDetail, "Invalid fillcolor data" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            if (has_strokecolor_data() && !strokecolor_.is_valid()) {
                auto res = result{ result::error_code::InvalidDetail, "Invalid strokecolor data" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            if (has_strokestyle_data() && !strokestyle_.is_valid()) {
                auto res = result{ result::error_code::InvalidDetail, "Invalid strokestyle data" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            if (has_strokeweight_data() && !strokeweight_.is_valid()) {
                auto res = result{ result::error_code::InvalidDetail, "Invalid strokeweight data" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            if (has_image_data() && !image_.is_valid()) {
                auto res = result{ result::error_code::InvalidDetail, "Invalid image data" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            if (has_inclination_data() && !inclination_.is_valid()) {
                auto res = result{ result::error_code::InvalidDetail, "Invalid inclination data" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            if (!labels_on_.is_valid()) {
                auto res = result{ result::error_code::InvalidDetail, "Invalid labels_on data" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            if (has_tog_data() && !tog_.is_valid()) {
                auto res = result{ result::error_code::InvalidDetail, "Invalid tog data" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            if (has_range_data() && !range_.is_valid()) {
                auto res = result{ result::error_code::InvalidDetail, "Invalid range data" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            if (has_rangeunits_data() && !rangeunits_.is_valid()) {
                auto res = result{ result::error_code::InvalidDetail, "Invalid rangeunits data" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            if (has_link_attr_data() && !link_attr_.is_valid()) {
                auto res = result{ result::error_code::InvalidDetail, "Invalid link_attr data" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            if (!milicon_.id.empty() && !milicon_.is_valid()) {
                auto res = result{ result::error_code::InvalidDetail, "Invalid milicon data" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            if ((!milsym_.id.empty() || !milsym_.modifiers.empty()) && !milsym_.is_valid()) {
                auto res = result{ result::error_code::InvalidDetail, "Invalid milsym data" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            if (has_creator_data() && !creator_.is_valid()) {
                auto res = result{ result::error_code::InvalidDetail, "Invalid creator data" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            if (!display_.is_valid()) {
                auto res = result{ result::error_code::InvalidDetail, "Invalid display data" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            if (has_emergency_data() && !emergency_.is_valid()) {
                auto res = result{ result::error_code::InvalidDetail, "Invalid emergency data" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            if (has_address_data() && !address_.is_valid()) {
                auto res = result{ result::error_code::InvalidDetail, "Invalid address data" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            if (has_takcontrol_data() && !takcontrol_.is_valid()) {
                auto res = result{ result::error_code::InvalidDetail, "Invalid TakControl data" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            if (has_ackrequest_data() && !ackrequest_.is_valid()) {
                auto res = result{ result::error_code::InvalidDetail, "Invalid ackrequest data" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            if (has_chat_data() && !chat_.is_valid()) {
                auto res = result{ result::error_code::InvalidDetail, "Invalid __chat data" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            if (has_chatreceipt_data() && !chatreceipt_.is_valid()) {
                auto res = result{ result::error_code::InvalidDetail, "Invalid __chatreceipt data" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            if (has_spatial_data() && !spatial_.is_valid()) {
                auto res = result{ result::error_code::InvalidDetail, "Invalid spatial data" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            for (const auto& link : links) {
                if (!link.is_valid()) {
                    auto res = result{ result::error_code::InvalidDetail, "Invalid link data" };
                    if (rslt) *rslt = res;
                    if (error_handler) error_handler(res);
                    return false;
                }
            }
            for (const auto& custom : custom_details) {
                if (!custom.is_valid()) {
                    auto res = result{ result::error_code::InvalidDetail, "Invalid custom detail data" };
                    if (rslt) *rslt = res;
                    if (error_handler) error_handler(res);
                    return false;
                }
            }
            return true;
        }

        /// @brief Serializes to XML
        /// @param parent The parent XML node to append to
        /// @return Result indicating success or failure
        result to_xml(pugi::xml_node& parent) const {
            if (!is_valid()) {
                auto res = result{ result::error_code::InvalidDetail, "Invalid detail data" };
                if (error_handler) error_handler(res);
                return res;
            }
            auto node = parent.append_child("detail");
            if (takv_.is_valid()) {
                auto res = takv_.to_xml(node);
                if (!res.is_success()) return res;
            }
            if (contact_.is_valid()) {
                auto res = contact_.to_xml(node);
                if (!res.is_success()) return res;
            }
            if (uid_.is_valid()) {
                auto res = uid_.to_xml(node);
                if (!res.is_success()) return res;
            }
            if (model_.is_valid()) {
                auto res = model_.to_xml(node);
                if (!res.is_success()) return res;
            }
            if (precisionlocation_.is_valid()) {
                auto res = precisionlocation_.to_xml(node);
                if (!res.is_success()) return res;
            }
            if (group_.is_valid()) {
                auto res = group_.to_xml(node);
                if (!res.is_success()) return res;
            }
            if (status_.is_valid()) {
                auto res = status_.to_xml(node);
                if (!res.is_success()) return res;
            }
            if (track_.is_valid()) {
                auto res = track_.to_xml(node);
                if (!res.is_success()) return res;
            }
            if (color_.is_valid()) {
                auto res = color_.to_xml(node, "color");
                if (!res.is_success()) return res;
            }
            if (has_fillcolor_data() && fillcolor_.is_valid()) {
                auto res = fillcolor_.to_xml(node);
                if (!res.is_success()) return res;
            }
            if (has_strokecolor_data() && strokecolor_.is_valid()) {
                auto res = strokecolor_.to_xml(node);
                if (!res.is_success()) return res;
            }
            if (has_strokestyle_data() && strokestyle_.is_valid()) {
                auto res = strokestyle_.to_xml(node);
                if (!res.is_success()) return res;
            }
            if (has_strokeweight_data() && strokeweight_.is_valid()) {
                auto res = strokeweight_.to_xml(node);
                if (!res.is_success()) return res;
            }
            if (has_image_data() && image_.is_valid()) {
                auto res = image_.to_xml(node);
                if (!res.is_success()) return res;
            }
            if (has_inclination_data() && inclination_.is_valid()) {
                auto res = inclination_.to_xml(node);
                if (!res.is_success()) return res;
            }
            if (has_labels_on_data() && labels_on_.is_valid()) {
                auto res = labels_on_.to_xml(node);
                if (!res.is_success()) return res;
            }
            if (has_tog_data() && tog_.is_valid()) {
                auto res = tog_.to_xml(node);
                if (!res.is_success()) return res;
            }
            if (has_range_data() && range_.is_valid()) {
                auto res = range_.to_xml(node);
                if (!res.is_success()) return res;
            }
            if (has_rangeunits_data() && rangeunits_.is_valid()) {
                auto res = rangeunits_.to_xml(node);
                if (!res.is_success()) return res;
            }
            if (usericon_.is_valid()) {
                auto res = usericon_.to_xml(node);
                if (!res.is_success()) return res;
            }
            if (remarks_.is_valid() && (!remarks_.text.empty() || !remarks_.source.empty() ||
                !remarks_.to.empty() || remarks_.time.has_value())) {
                auto res = remarks_.to_xml(node);
                if (!res.is_success()) return res;
            }
            if (has_link_attr_data() && link_attr_.is_valid()) {
                auto res = link_attr_.to_xml(node);
                if (!res.is_success()) return res;
            }
            if (milicon_.is_valid()) {
                auto res = milicon_.to_xml(node);
                if (!res.is_success()) return res;
            }
            if (milsym_.is_valid()) {
                auto res = milsym_.to_xml(node);
                if (!res.is_success()) return res;
            }
            if (has_creator_data() && creator_.is_valid()) {
                auto res = creator_.to_xml(node);
                if (!res.is_success()) return res;
            }
            if (has_display_data() && display_.is_valid()) {
                auto res = display_.to_xml(node);
                if (!res.is_success()) return res;
            }
            if (has_emergency_data() && emergency_.is_valid()) {
                auto res = emergency_.to_xml(node);
                if (!res.is_success()) return res;
            }
            if (has_address_data() && address_.is_valid()) {
                auto res = address_.to_xml(node);
                if (!res.is_success()) return res;
            }
            if (has_takcontrol_data() && takcontrol_.is_valid()) {
                auto res = takcontrol_.to_xml(node);
                if (!res.is_success()) return res;
            }
            if (has_ackrequest_data() && ackrequest_.is_valid()) {
                auto res = ackrequest_.to_xml(node);
                if (!res.is_success()) return res;
            }
            if (has_chat_data() && chat_.is_valid()) {
                auto res = chat_.to_xml(node, "__chat");
                if (!res.is_success()) return res;
            }
            if (has_chatreceipt_data() && chatreceipt_.is_valid()) {
                auto res = chatreceipt_.to_xml(node, "__chatreceipt");
                if (!res.is_success()) return res;
            }
            if (has_spatial_data() && spatial_.is_valid()) {
                auto res = spatial_.to_xml(node);
                if (!res.is_success()) return res;
            }
            for (const auto& link : links) {
                auto res = link.to_xml(node);
                if (!res.is_success()) return res;
            }
            for (const auto& custom : custom_details) {
                auto res = custom.to_xml(node);
                if (!res.is_success()) return res;
            }
            return result{};
        }

        /// @brief Deserializes from an XML node
        /// @param node The XML node to parse
        /// @param rslt Optional result for error details
        /// @return Deserialized detail
        static detail from_xml(const pugi::xml_node& node, result* rslt = nullptr) {
            detail d;
            if (node.name() != std::string("detail")) {
                auto res = result{ result::error_code::InvalidDetail, "Expected <detail> node" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return d;
            }
            for (auto child : node.children()) {
                std::string name = child.name();
                try {
                    if (name == "takv") d.takv_ = takv::from_xml(child, rslt);
                    else if (name == "contact") d.contact_ = contact::from_xml(child, rslt);
                    else if (name == "uid") d.uid_ = uid::from_xml(child, rslt);
                    else if (name == "model") d.model_ = model::from_xml(child, rslt);
                    else if (name == "precisionlocation") d.precisionlocation_ = precisionlocation::from_xml(child, rslt);
                    else if (name == "__group") d.group_ = group::from_xml(child, rslt);
                    else if (name == "status") d.status_ = status::from_xml(child, rslt);
                    else if (name == "track") d.track_ = track::from_xml(child, rslt);
                    else if (name == "color") d.color_ = color::from_xml(child, rslt, "color");
                    else if (name == "fillcolor") d.fillcolor_ = fillcolor::from_xml(child, rslt);
                    else if (name == "strokecolor") d.strokecolor_ = strokecolor::from_xml(child, rslt);
                    else if (name == "strokestyle") d.strokestyle_ = strokestyle::from_xml(child, rslt);
                    else if (name == "strokeweight") d.strokeweight_ = strokeweight::from_xml(child, rslt);
                    else if (name == "image") d.image_ = image::from_xml(child, rslt);
                    else if (name == "inclination") d.inclination_ = inclination::from_xml(child, rslt);
                    else if (name == "labels_on") d.labels_on_ = labels_on::from_xml(child, rslt);
                    else if (name == "tog") d.tog_ = tog::from_xml(child, rslt);
                    else if (name == "range") d.range_ = range::from_xml(child, rslt);
                    else if (name == "rangeunits") d.rangeunits_ = rangeunits::from_xml(child, rslt);
                    else if (name == "usericon") d.usericon_ = usericon::from_xml(child, rslt);
                    else if (name == "remarks") d.remarks_ = remarks::from_xml(child, rslt);
                    else if (name == "link_attr") d.link_attr_ = link_attr::from_xml(child, rslt);
                    else if (name == "__milicon") d.milicon_ = milicon::from_xml(child, rslt);
                    else if (name == "__milsym") d.milsym_ = milsym::from_xml(child, rslt);
                    else if (name == "creator") d.creator_ = creator::from_xml(child, rslt);
                    else if (name == "display") d.display_ = display::from_xml(child, rslt);
                    else if (name == "emergency") d.emergency_ = emergency::from_xml(child, rslt);
                    else if (name == "__address") d.address_ = address::from_xml(child, rslt);
                    else if (name == "TakControl") d.takcontrol_ = takcontrol::from_xml(child, rslt);
                    else if (name == "ackrequest") d.ackrequest_ = ackrequest::from_xml(child, rslt);
                    else if (name == "__chat") d.chat_ = chat::from_xml(child, rslt, "__chat");
                    else if (name == "__chatreceipt" || name == "__chatReceipt") {
                        d.chatreceipt_ = chat::from_xml(child, rslt, name);
                    }
                    else if (name == "spatial") d.spatial_ = spatial::from_xml(child, rslt);
                    else if (name == "link") d.links.push_back(link::from_xml(child, rslt));
                    else if (!name.empty()) d.custom_details.push_back(customdetail::from_xml(child, rslt));
                }
                catch (const std::exception& e) {
                    auto res = result{ result::error_code::InvalidDetail,
                                      "Error parsing <" + name + ">: " + std::string(e.what()) };
                    if (rslt) *rslt = res;
                    if (error_handler) error_handler(res);
                }
            }
            if (!d.is_valid(rslt)) {
                auto res = result{ result::error_code::InvalidDetail,
                                  "Invalid detail data after parsing" };
                if (rslt && rslt->is_success()) *rslt = res;
                if (error_handler) error_handler(res);
            }
            return d;
        }

        /// @brief Adds a new custom detail
        /// @param name XML tag name
        /// @param attributes XML attributes
        /// @param content Inner text or CDATA
        /// @return Result indicating success or failure
        result add_custom_detail(std::string name,
            std::map<std::string, std::string> attributes = {},
            std::string content = "") {
            if (name.empty()) {
                auto res = result{ result::error_code::InvalidDetail, "Empty custom detail name" };
                if (error_handler) error_handler(res);
                return res;
            }
            customdetail cd(std::move(name), std::move(attributes), std::move(content));
            if (!cd.is_valid()) {
                auto res = result{ result::error_code::InvalidDetail, "Invalid custom detail data" };
                if (error_handler) error_handler(res);
                return res;
            }
            if (std::any_of(custom_details.begin(), custom_details.end(),
                [&cd](const auto& d) { return d.name == cd.name; })) {
                auto res = result{ result::error_code::InvalidDetail, "Custom detail name already exists" };
                if (error_handler) error_handler(res);
                return res;
            }
            custom_details.push_back(std::move(cd));
            return result{};
        }

        /// @brief Modifies an existing custom detail
        /// @param name XML tag name to match
        /// @param matchAttributes Attributes to match
        /// @param matchContent Content to match
        /// @param newAttributes New attributes
        /// @param newContent New content
        /// @return Result indicating success or failure
        result modify_custom_detail(std::string name,
            const std::map<std::string, std::string>& matchAttributes,
            std::string matchContent,
            std::map<std::string, std::string> newAttributes,
            std::string newContent) {
            if (name.empty()) {
                auto res = result{ result::error_code::InvalidDetail, "Empty custom detail name" };
                if (error_handler) error_handler(res);
                return res;
            }
            auto it = std::find_if(custom_details.begin(), custom_details.end(),
                [&name, &matchAttributes, &matchContent](const customdetail& detail) {
                    bool nameMatch = detail.name == name;
                    bool attrMatch = matchAttributes.empty() || detail.attributes == matchAttributes;
                    bool contentMatch = matchContent.empty() || detail.content == matchContent;
                    return nameMatch && attrMatch && contentMatch;
                });
            if (it == custom_details.end()) {
                auto res = result{ result::error_code::InvalidDetail, "Custom detail not found" };
                if (error_handler) error_handler(res);
                return res;
            }
            it->attributes = std::move(newAttributes);
            it->content = std::move(newContent);
            return result{};
        }

        /// @brief Adds or modifies a custom detail
        /// @param name XML tag name
        /// @param attributes XML attributes
        /// @param content Inner text or CDATA
        /// @param matchAttributes Attributes to match for modification
        /// @param matchContent Content to match for modification
        /// @return Result indicating success or failure
        result add_or_modify_custom_detail(std::string name,
            std::map<std::string, std::string> attributes,
            std::string content,
            const std::map<std::string, std::string>& matchAttributes = {},
            std::string matchContent = "") {
            auto mod_res = modify_custom_detail(name, matchAttributes, matchContent,
                attributes, content);
            if (mod_res.is_success()) return mod_res;
            return add_custom_detail(std::move(name), std::move(attributes), std::move(content));
        }

        /// @brief Removes a custom detail
        /// @param name XML tag name to match
        /// @param attributes Attributes to match
        /// @param content Content to match
        /// @return Result indicating success or failure
        result remove_custom_detail(std::string name,
            const std::map<std::string, std::string>& attributes = {},
            std::string content = "") {
            if (name.empty()) {
                auto res = result{ result::error_code::InvalidDetail, "Empty custom detail name" };
                if (error_handler) error_handler(res);
                return res;
            }
            auto it = std::find_if(custom_details.begin(), custom_details.end(),
                [&name, &attributes, &content](const customdetail& detail) {
                    bool nameMatch = detail.name == name;
                    bool attrMatch = attributes.empty() || detail.attributes == attributes;
                    bool contentMatch = content.empty() || detail.content == content;
                    return nameMatch && attrMatch && contentMatch;
                });
            if (it == custom_details.end()) {
                auto res = result{ result::error_code::InvalidDetail, "Custom detail not found" };
                if (error_handler) error_handler(res);
                return res;
            }
            custom_details.erase(it);
            return result{};
        }

        /// @brief Stream output for printing
        friend std::ostream& operator<<(std::ostream& os, const detail& d) {
            os << "detail: ";
            if (!d.is_valid()) os << " -NOT VALID- ";
            os << "\n"
                << d.takv_
                << d.contact_
                << d.uid_
                << d.model_
                << d.precisionlocation_
                << d.group_
                << d.status_
                << d.track_
                << d.color_;
            if (d.has_fillcolor_data()) os << d.fillcolor_;
            if (d.has_strokecolor_data()) os << d.strokecolor_;
            if (d.has_strokestyle_data()) os << d.strokestyle_;
            if (d.has_strokeweight_data()) os << d.strokeweight_;
            if (d.has_image_data()) os << d.image_;
            if (d.has_inclination_data()) os << d.inclination_;
            if (d.has_labels_on_data()) os << d.labels_on_;
            if (d.has_tog_data()) os << d.tog_;
            if (d.has_range_data()) os << d.range_;
            if (d.has_rangeunits_data()) os << d.rangeunits_;
            os << d.usericon_
                << d.remarks_;
            if (d.has_link_attr_data()) os << d.link_attr_;
            os << d.milicon_
                << d.milsym_;
            if (d.has_creator_data()) os << d.creator_;
            if (d.has_display_data()) os << d.display_;
            if (d.has_emergency_data()) os << d.emergency_;
            if (d.has_address_data()) os << d.address_;
            if (d.has_takcontrol_data()) os << d.takcontrol_;
            if (d.has_ackrequest_data()) os << d.ackrequest_;
            if (d.has_chat_data()) os << d.chat_;
            if (d.has_chatreceipt_data()) os << d.chatreceipt_;
            if (d.has_spatial_data()) os << d.spatial_;
            for (const auto& link : d.links) os << link;
            for (const auto& custom : d.custom_details) os << custom;
            return os;
        }

        takv takv_;
        contact contact_;
        uid uid_;
        model model_;
        precisionlocation precisionlocation_;
        group group_;
        status status_;
        track track_;
        color color_;
        fillcolor fillcolor_;
        strokecolor strokecolor_;
        strokestyle strokestyle_;
        strokeweight strokeweight_;
        image image_;
        inclination inclination_;
        labels_on labels_on_;
        tog tog_;
        range range_;
        rangeunits rangeunits_;
        usericon usericon_;
        remarks remarks_;
        link_attr link_attr_;
        milicon milicon_;
        milsym milsym_;
        creator creator_;
        display display_;
        emergency emergency_;
        address address_;
        takcontrol takcontrol_;
        ackrequest ackrequest_;
        chat chat_;
        chat chatreceipt_;
        spatial spatial_;
        std::vector<link> links;
        std::vector<customdetail> custom_details;
    };
}