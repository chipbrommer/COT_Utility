#pragma once
namespace cot::subschemas
{
    /// @brief A CoT Message subschema class for detail data
    class detail {
    public:
        /// @brief Constructor - Initializes everything
        explicit detail(takv takv = takv(),
            contact contact = contact(),
            uid uid = uid(),
            model model = model(),
            precisionlocation precisionlocation = precisionlocation(),
            group group = group(),
            status status = status(),
            track track = track(),
            color strokecolor = color(),
            color fillcolor = color(),
            color color = color(),
            usericon usericon = usericon(),
            std::string remarks = "",
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
            strokecolor_(std::move(strokecolor)),
            fillcolor_(std::move(fillcolor)),
            color_(std::move(color)),
            usericon_(std::move(usericon)),
            remarks(std::move(remarks)),
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
                strokecolor_ == other.strokecolor_ &&
                fillcolor_ == other.fillcolor_ &&
                color_ == other.color_ &&
                usericon_ == other.usericon_ &&
                remarks == other.remarks &&
                links == other.links &&
                custom_details == other.custom_details;
        }

        /// @brief Inequality comparison
        bool operator!=(const detail& other) const noexcept { return !(*this == other); }

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
            if (strokecolor_.is_valid()) {
                auto res = strokecolor_.to_xml(node, "strokecolor");
                if (!res.is_success()) return res;
            }
            if (fillcolor_.is_valid()) {
                auto res = fillcolor_.to_xml(node, "fillcolor");
                if (!res.is_success()) return res;
            }
            if (color_.is_valid()) {
                auto res = color_.to_xml(node, "color");
                if (!res.is_success()) return res;
            }
            if (usericon_.is_valid()) {
                auto res = usericon_.to_xml(node);
                if (!res.is_success()) return res;
            }
            if (!remarks.empty()) {
                node.append_child("remarks").append_child(pugi::node_pcdata).set_value(remarks.c_str());
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
                    else if (name == "strokecolor") d.strokecolor_ = color::from_xml(child, rslt, "strokecolor");
                    else if (name == "fillcolor") d.fillcolor_ = color::from_xml(child, rslt, "fillcolor");
                    else if (name == "color") d.color_ = color::from_xml(child, rslt, "color");
                    else if (name == "usericon") d.usericon_ = usericon::from_xml(child, rslt);
                    else if (name == "remarks") d.remarks = child.child_value();
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
            if (!d.is_valid(rslt) && rslt && rslt->is_success()) {
                *rslt = result{ result::error_code::InvalidDetail, "Invalid detail data after parsing" };
                if (error_handler) error_handler(*rslt);
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
                << d.strokecolor_
                << d.fillcolor_
                << d.color_
                << d.usericon_
                << "\tremarks: " << (d.remarks.empty() ? "None" : d.remarks) << "\n"
                << "\tlinks:\n";
            if (d.links.empty()) os << "\t\tNone\n";
            else for (const auto& link : d.links) os << link;
            os << "\tcustom_details:\n";
            if (d.custom_details.empty()) os << "\t\tNone\n";
            else for (const auto& custom : d.custom_details) os << custom;
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
        color strokecolor_;
        color fillcolor_;
        color color_;
        usericon usericon_;
        std::string remarks;
        std::vector<link> links;
        std::vector<customdetail> custom_details;
    };
}