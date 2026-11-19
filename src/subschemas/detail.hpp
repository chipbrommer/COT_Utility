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
            fillColor fillColor = fillColor(),
            strokeColor strokeColor = strokeColor(),
            strokeStyle strokeStyle = strokeStyle(),
            strokeWeight strokeWeight = strokeWeight(),
            image image = image(),
            inclination inclination = inclination(),
            labels_on labels_on = labels_on(),
            usericon usericon = usericon(),
            remarks remarks = remarks(),
            link_attr link_attr = link_attr(),
            milicon milicon = milicon(),
            milsym milsym = milsym(),
            creator creator = creator(),
            display display = display(),
            emergency emergency = emergency(),
            address address = address(),
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
            fillColor_(std::move(fillColor)),
            strokeColor_(std::move(strokeColor)),
            strokeStyle_(std::move(strokeStyle)),
            strokeWeight_(std::move(strokeWeight)),
            image_(std::move(image)),
            inclination_(std::move(inclination)),
            labels_on_(std::move(labels_on)),
            usericon_(std::move(usericon)),
            remarks_(std::move(remarks)),
            link_attr_(std::move(link_attr)),
            milicon_(std::move(milicon)),
            milsym_(std::move(milsym)),
            creator_(std::move(creator)),
            display_(std::move(display)),
            emergency_(std::move(emergency)),
            address_(std::move(address)),
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
                fillColor_ == other.fillColor_ &&
                strokeColor_ == other.strokeColor_ &&
                strokeStyle_ == other.strokeStyle_ &&
                strokeWeight_ == other.strokeWeight_ &&
                image_ == other.image_ &&
                inclination_ == other.inclination_ &&
                labels_on_ == other.labels_on_ &&
                usericon_ == other.usericon_ &&
                remarks_ == other.remarks_ &&
                link_attr_ == other.link_attr_ &&
                milicon_ == other.milicon_ &&
                milsym_ == other.milsym_ &&
                creator_ == other.creator_ &&
                display_ == other.display_ &&
                emergency_ == other.emergency_ &&
                address_ == other.address_ &&
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
            if (!remarks_.is_valid()) {
                auto res = result{ result::error_code::InvalidDetail, "Invalid remarks data" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            if (has_fillColor_data() && !fillColor_.is_valid()) {
                auto res = result{ result::error_code::InvalidDetail, "Invalid fillColor data" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            if (has_strokeColor_data() && !strokeColor_.is_valid()) {
                auto res = result{ result::error_code::InvalidDetail, "Invalid strokeColor data" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            if (has_strokeStyle_data() && !strokeStyle_.is_valid()) {
                auto res = result{ result::error_code::InvalidDetail, "Invalid strokeStyle data" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            if (has_strokeWeight_data() && !strokeWeight_.is_valid()) {
                auto res = result{ result::error_code::InvalidDetail, "Invalid strokeWeight data" };
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
            if (has_fillColor_data() && fillColor_.is_valid()) {
                auto res = fillColor_.to_xml(node);
                if (!res.is_success()) return res;
            }
            if (has_strokeColor_data() && strokeColor_.is_valid()) {
                auto res = strokeColor_.to_xml(node);
                if (!res.is_success()) return res;
            }
            if (has_strokeStyle_data() && strokeStyle_.is_valid()) {
                auto res = strokeStyle_.to_xml(node);
                if (!res.is_success()) return res;
            }
            if (has_strokeWeight_data() && strokeWeight_.is_valid()) {
                auto res = strokeWeight_.to_xml(node);
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
                    else if (name == "fillColor") d.fillColor_ = fillColor::from_xml(child, rslt);
                    else if (name == "strokeColor") d.strokeColor_ = strokeColor::from_xml(child, rslt);
                    else if (name == "strokeStyle") d.strokeStyle_ = strokeStyle::from_xml(child, rslt);
                    else if (name == "strokeWeight") d.strokeWeight_ = strokeWeight::from_xml(child, rslt);
                    else if (name == "image") d.image_ = image::from_xml(child, rslt);
                    else if (name == "inclination") d.inclination_ = inclination::from_xml(child, rslt);
                    else if (name == "labels_on") d.labels_on_ = labels_on::from_xml(child, rslt);
                    else if (name == "usericon") d.usericon_ = usericon::from_xml(child, rslt);
                    else if (name == "remarks") d.remarks_ = remarks::from_xml(child, rslt);
                    else if (name == "link_attr") d.link_attr_ = link_attr::from_xml(child, rslt);
                    else if (name == "__milicon") d.milicon_ = milicon::from_xml(child, rslt);
                    else if (name == "__milsym") d.milsym_ = milsym::from_xml(child, rslt);
                    else if (name == "creator") d.creator_ = creator::from_xml(child, rslt);
                    else if (name == "display") d.display_ = display::from_xml(child, rslt);
                    else if (name == "emergency") d.emergency_ = emergency::from_xml(child, rslt);
                    else if (name == "__address") d.address_ = address::from_xml(child, rslt);
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
                << d.color_;
            if (d.has_fillColor_data()) os << d.fillColor_;
            if (d.has_strokeColor_data()) os << d.strokeColor_;
            if (d.has_strokeStyle_data()) os << d.strokeStyle_;
            if (d.has_strokeWeight_data()) os << d.strokeWeight_;
            if (d.has_image_data()) os << d.image_;
            if (d.has_inclination_data()) os << d.inclination_;
            if (d.has_labels_on_data()) os << d.labels_on_;
            os << d.usericon_
                << d.remarks_;
            if (d.has_link_attr_data()) os << d.link_attr_;
            if (!d.milicon_.id.empty()) os << d.milicon_;
            if (!d.milsym_.id.empty() || !d.milsym_.modifiers.empty()) os << d.milsym_;
            if (d.has_creator_data()) os << d.creator_;
            if (d.has_display_data()) os << d.display_;
            if (d.has_emergency_data()) os << d.emergency_;
            if (d.has_address_data()) os << d.address_;
            os << "\tlinks:\n";
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
        fillColor fillColor_;
        strokeColor strokeColor_;
        strokeStyle strokeStyle_;
        strokeWeight strokeWeight_;
        image image_;
        inclination inclination_;
        labels_on labels_on_;
        usericon usericon_;
        remarks remarks_;
        link_attr link_attr_;
        milicon milicon_;
        milsym milsym_;
        creator creator_;
        display display_;
        emergency emergency_;
        address address_;
        std::vector<link> links;
        std::vector<customdetail> custom_details;

    private:
        /// @brief Checks if fillColor has a non-default value
        [[nodiscard]] bool has_fillColor_data() const noexcept {
            return fillColor_.value != std::numeric_limits<int>::min();
        }

        /// @brief Checks if strokeColor has a non-default value
        [[nodiscard]] bool has_strokeColor_data() const noexcept {
            return strokeColor_.value != std::numeric_limits<int>::min();
        }

        /// @brief Checks if strokeStyle has a non-empty value
        [[nodiscard]] bool has_strokeStyle_data() const noexcept {
            return !strokeStyle_.value.empty();
        }

        /// @brief Checks if strokeWeight has a non-NaN value
        [[nodiscard]] bool has_strokeWeight_data() const noexcept {
            return !std::isnan(strokeWeight_.value);
        }

        /// @brief Checks if any image fields are populated
        [[nodiscard]] bool has_image_data() const noexcept {
            return !image_.mime.empty() || !image_.text.empty();
        }

        /// @brief Checks if inclination has a non-NaN value
        [[nodiscard]] bool has_inclination_data() const noexcept {
            return !std::isnan(inclination_.value);
        }

        /// @brief Checks if labels_on has an explicit value set
        [[nodiscard]] bool has_labels_on_data() const noexcept {
            return labels_on_.has_value;
        }

        /// @brief Checks if any link_attr fields are populated
        [[nodiscard]] bool has_link_attr_data() const noexcept {
            return !link_attr_.color.empty() || !link_attr_.method.empty() ||
                !link_attr_.direction.empty() || !link_attr_.routetype.empty() ||
                !link_attr_.order.empty();
        }

        /// @brief Checks if any creator fields are populated
        [[nodiscard]] bool has_creator_data() const noexcept {
            return !creator_.callsign.empty() || !creator_.time.empty() ||
                !creator_.type.empty() || !creator_.uid.empty();
        }

        /// @brief Checks if any display fields are populated
        [[nodiscard]] bool has_display_data() const noexcept {
            return !std::isnan(display_.maxzoom) || !std::isnan(display_.minzoom) ||
                display_.has_rotate;
        }

        /// @brief Checks if any emergency fields are populated
        [[nodiscard]] bool has_emergency_data() const noexcept {
            return !emergency_.text.empty() || !emergency_.type.empty() ||
                emergency_.has_cancel;
        }

        /// @brief Checks if any address fields are populated
        [[nodiscard]] bool has_address_data() const noexcept {
            return !address_.text.empty() || !address_.geocoder.empty() ||
                !address_.time.empty();
        }
    };
}