#include <bits/stdc++.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    const int max_count_ships=10;
    int state = std::stoi(argv[1]);
    if (state == 1)
    {
        std::string login;
        std::cin >> login;
        write(STDOUT_FILENO, login.c_str(), login.length());
        return 0;
    }
    else if (state==2)
    {
        std::string request,temp;
        for (size_t i = 0; i < max_count_ships; ++i)
        {
            std::getline(std::cin, temp); 
            request+=(temp+"\n");
        }
        write(STDOUT_FILENO, request.c_str(), request.length());
        return 0;
    }
    else if(state==3){
        std::string status;
        std::cin >> status;
        write(STDOUT_FILENO, status.c_str(), status.length());
        return 0;
    }
    else if (state==4)
    {
        int x,y;
        std::cin>>x>>y;
        --x;
        --y;
        std::string coords=std::to_string(x)+" "+std::to_string(y);
        write(STDOUT_FILENO, coords.c_str(), coords.length());
    }
    else if (state==5)
    {
        std::string request;
        std::cin>>request;
        write(STDOUT_FILENO, request.c_str(), request.length());
    }
    return 0;

}
