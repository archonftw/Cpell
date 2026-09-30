#include <iostream>
#include <unistd.h>
#include <pwd.h>

int main() {
    uid_t uid = getuid();

    struct passwd *pw = getpwuid(uid);

    if (pw != nullptr) {
        std::cout << "Username: " << pw->pw_name << '\n';
        std::cout << "UID: " << pw->pw_uid << '\n';
        std::cout << "Home: " << pw->pw_dir << '\n';
        std::cout << "Shell: " << pw->pw_shell << '\n';
    }

    return 0;
}