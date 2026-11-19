#pragma once
namespace cot
{
    /// @brief Error handler for custom error logging
    using ErrorHandler = std::function<void(const result&)>;
    inline ErrorHandler error_handler;

    /// @brief Sets the error handler
    inline void set_error_handler(ErrorHandler handler) noexcept {
        error_handler = std::move(handler);
    }
}