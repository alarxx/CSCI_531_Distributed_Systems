#include <condition_variable>
#include <mutex>
#include <iostream>
#include <string>
#include <thread>


class Friend{
private:
    std::condition_variable m_cv;
    std::mutex m_mutex;
    std::string name;
public:
    Friend(std::string friendName):name{friendName}{}

    void bow(Friend &bower){
        {
            // std::unique_lock dataLock1{m_mutex};
            // std::unique_lock dataLock2{bower.m_mutex};

            std::scoped_lock lock(m_mutex, bower.m_mutex);

            std::cout<<name<<": "<<bower.getName()<<" bowed to me!\n";
            bower.bowBack(*this);
        }
        //m_cv.notify_one();
    }

    void bowBack(Friend &bower){
        {
            // std::unique_lock dataLock{m_mutex};
            std::cout<<name<<": "<<bower.getName()<<" bowed back to me!\n";
        }
        //m_cv.notify_one();
    }

    std::string getName(){
        return name;
    }

    void setName(std::string friendName){
        name = friendName;
    }
};


int main(){
    std::cout<<"Main thread start !\n";
    Friend alphonse{"Alphonse"};
    Friend gaston{"Gaston"};
    std::jthread thread_1(&Friend::bow, &alphonse, std::ref(gaston));
    std::jthread thread_2(&Friend::bow, &gaston, std::ref(alphonse));
    std::cout<<"Main thread end !\n";
}
