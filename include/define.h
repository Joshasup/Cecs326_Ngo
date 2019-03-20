//
// Created by christopher on 2/26/19.
//
#include <type_traits>

struct message_buffer {
    long message_type;
    char message[20];
};

// constexpr auto alpha = 274471;
// constexpr auto beta = 80107;
// constexpr auto rho = 75479;
constexpr auto shared_mtype = 111;
constexpr auto msg_size = sizeof(message_buffer) - sizeof(long);

template <typename T1, typename T2>
bool valid_reading(T1 random_number, T2 magic_number) {
    return random_number != 0 && random_number % magic_number == 0;
}
