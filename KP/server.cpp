#include <bits/stdc++.h>
#include <unistd.h>
#include <sys/wait.h>

using matrix=std::vector<std::vector<int>>;
const int MAX_COUNT_SHIPS=10;

void arrangement_ships(matrix &map_player, const matrix input_ships)
{
    for (size_t i = 0; i <input_ships.size()-1; ++i)
    {
        int size_ship = input_ships[i][0];
        switch (size_ship)
        {
        case 1:
            if (input_ships[i][1] == input_ships[i][3] && input_ships[i][2] == input_ships[i][4])
            {
                int placeX = input_ships[i][1] - 1;
                int placeY = input_ships[i][2] - 1;
                map_player[placeX][placeY] = 1;
            }

            break;
        case 2:
        case 3:
        case 4:
            if (input_ships[i][1] == input_ships[i][3])
            {
                int min_place = std::min(input_ships[i][2], input_ships[i][4]) - 1;
                int max_place = std::max(input_ships[i][2], input_ships[i][4]) - 1;
                int placeX = input_ships[i][1] - 1;
                for (size_t j = min_place; j <= max_place; ++j)
                {
                    map_player[placeX][j] = 1;
                }
            }
            if (input_ships[i][2] == input_ships[i][4])
            {
                int min_place = std::min(input_ships[i][1], input_ships[i][3]) - 1;
                int max_place = std::max(input_ships[i][1], input_ships[i][3]) - 1;
                int placeY = input_ships[i][2] - 1;
                for (size_t j = min_place; j <= max_place; ++j)
                {
                    map_player[j][placeY] = 1;
                }
            }
            break;
        default:
            break;
        }
    }
}

void print_matrix(matrix matrix)
{
    for (int row_index = 0; row_index < 10; ++row_index)
    {
        for (int column_index = 0; column_index < 10; ++column_index)
        {

            if (matrix[row_index][column_index] == 3)
            {
                std::cout << "\u2715"
                          << " ";
            }
            if (matrix[row_index][column_index] == 2)
            {
                std::cout << "\u25A1\u0338"
                          << " ";
            }
            if (matrix[row_index][column_index] == 1)
            {
                std::cout << "\u25A1"
                          << " ";
            }
            if (matrix[row_index][column_index] == 0)
            {
                std::cout << "\u00B7"
                          << " ";
            }
        }
        std::cout << "\n";
    }
}

void clear(int (&pipe_first)[2])
{
    pid_t process_id_first = fork();

        if (process_id_first == 0)
        {
            close(pipe_first[0]);
            dup2(pipe_first[1], STDOUT_FILENO);
            close(pipe_first[1]);
            execl("./client", "./client", "3", NULL);
            perror("exec");
            exit(1);
        }
        else
        {
            wait(NULL);
            char result_child_first[200];
            int bytes_read_first = read(pipe_first[0], result_child_first, sizeof(result_child_first));
            if (bytes_read_first > 0)
            {
                std::string status(result_child_first, bytes_read_first);
                if (status == "ok")
                {
                    std::cout << "\x1B[2J\x1B[H";
                }
            }
        }
   
}

std::string startMenu(int (&pipe_first)[2]){
    std::cout<<"1 - посмотреть статистику"<<std::endl;
    pid_t process_id = fork();

        if (process_id == 0)
        {
            close(pipe_first[0]);
            dup2(pipe_first[1], STDOUT_FILENO);
            close(pipe_first[1]);
            execl("./client", "./client", "5", NULL);
            perror("exec");
            exit(1);
        }
        else
        {
            wait(NULL);
            char result_child_first[200];

            int bytes_read_first = read(pipe_first[0], result_child_first, sizeof(result_child_first));
            if (bytes_read_first > 0)
            {
                std::string request(result_child_first, bytes_read_first);
                return request;
            }
        }
    return "";
}

void startGame(int (&pipe_first)[2], matrix &map_player,matrix& input_ships,auto iterator_players)
{
    
    std::string request=startMenu(pipe_first);
    if (request=="1")
    {
        std::cout<<iterator_players->first + ": Выиграл "<<iterator_players->second.first<<".";
        std::cout<<"Проиграл "<<iterator_players->second.second<<"\n";
    }
    std::cout << "Введите размер корабля и две точки "
              << "\n";
        pid_t process_id = fork();

        if (process_id == 0)
        {
            close(pipe_first[0]);
            dup2(pipe_first[1], STDOUT_FILENO);
            close(pipe_first[1]);
            execl("./client", "./client", "2", NULL);
            perror("exec");
            exit(1);
        }
        else
        {
            wait(NULL);
            char result_child_first[200];

            int bytes_read_first = read(pipe_first[0], result_child_first, sizeof(result_child_first));
            if (bytes_read_first > 0)
            {

                std::stringstream ss(result_child_first);
                std::string line;

                while (std::getline(ss, line, '\n'))
                {
                    std::vector<int> row;
                    std::istringstream iss(line);
                    int number;

                    while (iss >> number)
                    {
                        row.push_back(number);
                    }

                    input_ships.push_back(row);
                }
                arrangement_ships(map_player, input_ships);
                print_matrix(map_player);
            }
        }
}

bool killShip(matrix &map_player,matrix& input_ships){
    for (size_t i = 0; i < input_ships.size()-1; ++i)
    {
        bool isKill=true;
        int size_ship = input_ships[i][0];
        int placeX, placeY; 
        switch (size_ship)
        {
        case 1:
            placeX = input_ships[i][1] - 1; 
            placeY = input_ships[i][2] - 1;
            if (map_player[placeX][placeY] == 2)
            {
                input_ships.erase(input_ships.begin() + i);
                return true;
            }
            break;
        case 2:
        case 3:
        case 4:
            if (input_ships[i][1] == input_ships[i][3])
            {
                int min_place = std::min(input_ships[i][2], input_ships[i][4]) - 1;
                int max_place = std::max(input_ships[i][2], input_ships[i][4]) - 1;
                int placeX = input_ships[i][1] - 1;
                size_t j;
                for (j = min_place; j <= max_place; ++j)
                {
                    if(map_player[placeX][j] !=2){
                        isKill=false;
                        break;
                    }
                    
                }
                if(isKill){
                    input_ships.erase(input_ships.begin() + i);
                    return true;
                }
 
                
            }
            if (input_ships[i][2] == input_ships[i][4])
            {
                int min_place = std::min(input_ships[i][1], input_ships[i][3]) - 1;
                int max_place = std::max(input_ships[i][1], input_ships[i][3]) - 1;
                int placeY = input_ships[i][2] - 1;
                size_t j;
                 for (j = min_place; j <= max_place; ++j)
                {
                    if(map_player[j][placeY] !=2){
                        isKill=false;
                        break;
                    }
                    
                }
                if(isKill){
                    input_ships.erase(input_ships.begin() + i);
                    return true;
                }
            }
            break;
        default:
            return false;
            break;
        }
    }
    
    return false;
}


void giveInfo(int x, int y, matrix &map_player, matrix &map_enemy,matrix& input_ships,int& count_kill_ships)
{
    if (map_player[x][y] == 1)
    {
        map_player[x][y] = 2;
        map_enemy[x][y] = 2;
        if (killShip(map_player,input_ships))
        {
            ++count_kill_ships;
            std::cout<<"убил"<<std::endl;
        }
        else{   
        std::cout << "ранил" << std::endl;
        }
    }
    else if (map_player[x][y] == 0)
    {
        map_player[x][y] = 3;
        map_enemy[x][y] = 3;
        std::cout << "мимо" << std::endl;
    }
    else
    {
        std::cout << "ход в молоко" << std::endl;
    }
}

void movePlayer(int (&pipe_first)[2],matrix &map_current,matrix &map_player, matrix &map_enemy, std::string player,matrix &input_ships,int& count_kill_ships)
{
    std::cout << player + " ходит"
              << "\n";
    pid_t process_id_first = fork();

    if (process_id_first == 0)
    {
        close(pipe_first[0]);
        dup2(pipe_first[1], STDOUT_FILENO);
        close(pipe_first[1]);
        execl("./client", "./client", "4", NULL);
        perror("exec");
        exit(1);
    }
    else
    {
        wait(NULL);
        char result_child_first[200];

        std::vector<int> coords;
        int bytes_read_first = read(pipe_first[0], result_child_first, sizeof(result_child_first));
        if (bytes_read_first > 0)
        {

            std::stringstream ss(result_child_first);
            std::string line;

            std::getline(ss, line, '\n');
            std::istringstream iss(line);
            int number;

            while (iss >> number)
            {
                coords.push_back(number);
            }
        }
        giveInfo(coords[0], coords[1], map_player, map_enemy,input_ships,count_kill_ships);
        std::cout << "Ваше поле:" << std::endl;
        print_matrix(map_current);
        std::cout << std::endl;
        std::cout << "Поле противника:" << std::endl;
        print_matrix(map_enemy);
    }
}



int main()
{
    int pipe_first[2];
    int pipe_second[2];
    int count_kill_ships_first=0;
    int count_kill_ships_second=0;
    matrix input_ships_first;
    matrix input_ships_second;
    matrix map_player1(10, std::vector<int>(10, 0));
    matrix map_player2(10, std::vector<int>(10, 0));
    matrix map_enemy1(10, std::vector<int>(10, 0));
    matrix map_enemy2(10, std::vector<int>(10, 0));
    std::unordered_map<std::string, std::pair<int, int>> players;
    if (pipe(pipe_first) < 0 || pipe(pipe_second) < 0)
    {
        perror("create pipe");
        exit(1);
    }
    {
        pid_t process_id_first = fork();

        if (process_id_first == 0)
        {
            std::cout << "Введите логины: " << std::endl;
            close(pipe_first[0]);
            dup2(pipe_first[1], STDOUT_FILENO);
            close(pipe_first[1]);
            execl("./client", "./client", "1", NULL);
            perror("exec");
            exit(1);
        }
        else
        {
            pid_t process_id_second = fork();

            if (process_id_second == 0)
            {
                close(pipe_second[0]);
                dup2(pipe_second[1], STDOUT_FILENO);
                close(pipe_second[1]);
                execl("./client", "./client", "1", NULL);
                perror("exec");
                exit(1);
            }
            else
            {
                wait(NULL);
                wait(NULL);
                char result_child_first[200];
                char result_child_second[200];

                int bytes_read_first = read(pipe_first[0], result_child_first, sizeof(result_child_first));
                int bytes_read_second = read(pipe_second[0], result_child_second, sizeof(result_child_second));
                if (bytes_read_first > 0)
                {
                    std::string player(result_child_first, bytes_read_first);
                    players[player] = std::make_pair(0, 0);
                    std::cout << player + " is loggined" << std::endl;
                }
                if (bytes_read_second > 0)
                {
                    std::string player2(result_child_second, bytes_read_second);

                    players[player2] = std::make_pair(0, 0);

                    std::cout << player2 + " is loggined" << std::endl;
                }
            }
        }
    }
    auto it1 = players.begin();
    auto it2 = players.begin();
    ++it2;
    while (true)
    {       
        startGame(pipe_first, map_player1,input_ships_first,it1);
        clear(pipe_first);
        startGame(pipe_second, map_player2,input_ships_second,it2);
        clear(pipe_second);

        while (true)
        {
            movePlayer(pipe_first,map_player1,map_player2, map_enemy1, it1->first,input_ships_first,count_kill_ships_first);
            if (count_kill_ships_first==MAX_COUNT_SHIPS)
            {
                std::cout<<it1->first+" победил"<<std::endl;
                std::cout<<it2->first+" проиграл"<<std::endl;
                it1->second.first++;
                it2->second.second++;
                break;
            }
            // clear(pipe_first);
            movePlayer(pipe_second,map_player2, map_player1, map_enemy2, it2->first,input_ships_second,count_kill_ships_second);
            if (count_kill_ships_second==MAX_COUNT_SHIPS)
            {
                std::cout<<it2->first+" победил"<<std::endl;
                std::cout<<it1->first+" проиграл"<<std::endl;
                it2->second.first++;
                it1->second.second++;
                break;
            }
            // clear(pipe_second);

        }
        std::cout<<"\n";
    }
    return 0;
}
