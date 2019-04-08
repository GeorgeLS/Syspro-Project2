#ifndef EXERCISE_II_CLIENTPROCESSINFO_HPP
#define EXERCISE_II_CLIENTPROCESSINFO_HPP

struct ClientProcessInfo {
    enum Profile {
        Sender,
        Receiver
    };
    char *clientID_;
    Profile profile_;
};


#endif //EXERCISE_II_CLIENTPROCESSINFO_HPP
