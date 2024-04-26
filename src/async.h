#include "CommandReceiverSafe.h"

namespace async
{
    namespace internal
    {
        /**
         * @brief Returns a reference to the singleton instance of CommandReceiverSafe.
         * @return A reference to the CommandReceiverSafe instance.
         */
        CommandReceiverSafe& GetInstance() noexcept;
    }

    /**
     * @brief A type alias for an unsigned integer representing a handle.
     */
    using handle_t = unsigned;

    /**
     * @brief Connects to the command receiver with a specified bulk size.
     * @param bulk The bulk size.
     * @return A handle representing the connection.
     */
    handle_t connect(std::size_t bulk);

    /**
     * @brief Receives data and processes it as a command.
     * @param handle The handle representing the connection.
     * @param data The data to receive.
     * @param size The size of the data.
     */
    void receive(handle_t handle, const char* data, std::size_t size);

    /**
     * @brief Disconnects from the command receiver.
     * @param handle The handle representing the connection.
     */
    void disconnect(handle_t handle);
}
