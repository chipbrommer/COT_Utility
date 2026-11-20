#pragma once
/////////////////////////////////////////////////////////////////////////////////
// @file            error_handler.hpp
// @brief           Global error handler callback for CoT operations.
// @author          Chip Brommer
/////////////////////////////////////////////////////////////////////////////////

#include <functional>
#include <utility>

#include "../result/result.hpp"

namespace cot
{
    /// @brief Error handler for custom error logging
    using ErrorHandler = std::function<void(const result&)>;
    inline ErrorHandler error_handler;

    /// @brief Sets the error handler
    /// @param handler Custom callback invoked when errors are reported
    inline void set_error_handler(ErrorHandler handler) noexcept {
        error_handler = std::move(handler);
    }
}