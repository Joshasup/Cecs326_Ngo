//
// Created by christopher on 2/26/19.
//
#include <type_traits>

struct message_buffer {
    long message_type;
    char message[50];
};

constexpr auto alpha = 997;
constexpr auto beta = 257;
constexpr auto rho = 251;
constexpr auto msg_size = sizeof(message_buffer) - sizeof(long);

template <typename T1, typename T2>
bool valid_reading(T1 random_number, T2 magic_number) {
    return random_number % magic_number == 0;
}

static_assert(alpha > 3 * beta, "Alpha must be 3 times greater than beta");
static_assert(beta > rho, "Beta must be greater than rho");
