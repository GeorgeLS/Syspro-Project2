#ifndef EXERCISE_II_CLIENTPROCESSINFO_HPP
#define EXERCISE_II_CLIENTPROCESSINFO_HPP

struct ClientProcessInfo {
    enum Profile {
        Sender,
        Receiver,
        Cleaner
    };
    
    char *clientID_;
    Profile profile_;
    uint reties_;
};


#endif //EXERCISE_II_CLIENTPROCESSINFO_HPP
