#include <chrono>
#include <cctype>
#include <fstream>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#endif

std::mutex mmutex;

std::string ТекущееВремя()
{
    static const auto programStart = std::chrono::steady_clock::now(); // зафиксируем время запуска программы
    auto now = std::chrono::steady_clock::now(); 
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now - programStart).count();
    return std::to_string(ms) + "ms";
}


// печатаем 
void Печать(const std::string& text)
{
    mmutex.lock();
    std::cout << text << '\n';
    mmutex.unlock();
}

class Команда {
public:
    virtual ~Команда() = default;
    virtual void execute() = 0; // чисто виртуальный метод
};

class КомандаПечать : public Команда {
public:
    КомандаПечать(const std::string& text)
    {
        text_ = text;
    }
    void execute() 
    { Печать(text_);
    }

private:
    std::string text_;
};

class КомандаФайл : public Команда
{
public:
    КомандаФайл(const std::string& fileName, const std::string& text)
    {
        fileName_ = fileName;
        text_ = text;
    }

    void execute()
    {
        mmutex.lock();
        std::ofstream file(fileName_, std::ios::app);
        mmutex.unlock();
        if (!file) {
            Печать("ошибка при открытии файла");
            return;
        }
        file << text_ << '\n';
    }

private:
    std::string fileName_;
    std::string text_;
};

class КомандаСложить : public Команда {
public:
    КомандаСложить(long long a, long long b)
    {
        a_ = a;
        b_ = b;
    }
    void execute() 
    {
    Печать("результат: " + std::to_string(a_ + b_));
    }

private:
    long long a_;
    long long b_;
};

class КомандаВычесть : public Команда {
public:
    КомандаВычесть(long long a, long long b) 
    {
        a_ = a;
        b_ = b;
    }
    void execute() { Печать("результат: " + std::to_string(a_ - b_)); }

private:
    long long a_;
    long long b_;
};

class КомандаУмножить : public Команда {
public:
    КомандаУмножить(long long a, long long b)
    {
        a_ = a;
        b_ = b;
    }
    void execute() { Печать("результат: " + std::to_string(a_ * b_)); }

private:
    long long a_;
    long long b_;
};

class КомандаДелить : public Команда {
public:
    КомандаДелить(long long a, long long b) 
    {
        a_ = a;
        b_ = b;
    }
    void execute()
    {
        if (b_ == 0) {
            Печать("делить на ноль нельзя");
            return;
        }  
        Печать("результат: " + std::to_string(a_ / b_));
    }

private:
    long long a_;
    long long b_;
};

class КомандаДля : public Команда {
public:
    КомандаДля(int count, Команда* p) 
    {
        count_ = count;
        p_ = p;
    }

    ~КомандаДля() { delete p_; }

    void execute() {
        for (int i = 0; i < count_; ++i) {
            p_->execute();
        }
    }

private:
    int count_;
    Команда* p_;
};

bool Число(const std::string& sim, long long& value)
{
    std::istringstream in(sim);
    in >> value;
    return in && in.eof(); // + проверка на то что дочитали до коцна строки 
}

std::vector<std::string> Токены(const std::string& line)
{
    std::vector<std::string> slovo;
    std::string current;
    bool inCavs = false;

    for (char ch : line)
    {
        if (ch == '"') // если кавычки
        {
            if (inCavs) // уже в кавычках => конец слова
            {
                slovo.push_back(current);
                current.clear();
            }
            inCavs = !inCavs;
            continue;
        }

        if (!inCavs && std::isspace(static_cast<unsigned char>(ch))) // не в кавычках и пробел => конец слова 
        {
            if (!current.empty())
            {
                slovo.push_back(current);
                current.clear();
            }
        } 
        else
            current.push_back(ch); //   в кавычках или не пробел (ПЕЧАТЬ например) => добавим к слову символ
    }

    if (!current.empty()) 
        slovo.push_back(current); // на случай если последнее слово не закончится пробелом или кавычкой


    return slovo;
}

Команда* parseCommand(const std::vector<std::string>& slovo, size_t& i) // unsigned int, парсинг команд
{
    if (i >= slovo.size()) return nullptr;

    std::string op = slovo[i++];

    if (op == "ПЕЧАТЬ")
    {
        if (i >= slovo.size()) return nullptr;
        return new КомандаПечать(slovo[i++]);
    }

    if (op == "ФАЙЛ")
    {
        if (i + 1 >= slovo.size()) return nullptr;
        std::string name = slovo[i++];
        std::string text = slovo[i++];
        return new КомандаФайл(name, text);
    }

    if (op == "СЛОЖИТЬ" || op == "ВЫЧЕСТЬ" || op == "УМНОЖИТЬ" || op == "ДЕЛИТЬ")
    {
        if (i + 1 >= slovo.size()) return nullptr;
        long long a = 0, b = 0;
        if (!Число(slovo[i], a) || !Число(slovo[i + 1], b)) return nullptr;
        i += 2;

        if (op == "СЛОЖИТЬ") return new КомандаСложить(a, b);
        if (op == "ВЫЧЕСТЬ") return new КомандаВычесть(a, b);
        if (op == "УМНОЖИТЬ") return new КомандаУмножить(a, b);
        return new КомандаДелить(a, b);
    }

    if (op == "ДЛЯ") {
        if (i >= slovo.size()) return nullptr;
        long long count = 0;
        if (!Число(slovo[i++], count) || count < 0) return nullptr;
        Команда* p = parseCommand(slovo, i);
        if (!p) return nullptr; 
        return new КомандаДля(static_cast<int>(count), p);
    }

    return nullptr;
}

std::vector<Команда*> parseLine(const std::string& line) // парсим строку и возвращаем вектор команд, если ошибка - пустой вектор
{
    std::vector<Команда*> commands;
    std::vector<std::string> slovo = Токены(line); // массив слов из строки
    size_t i = 0;

    while (i < slovo.size())
    {
        Команда* cmd = parseCommand(slovo, i);
        if (!cmd)
         {
            for (Команда* c : commands) delete c; // удалим все
            return {};
        }
        commands.push_back(cmd);
    }

    return commands;
}

void ВыполнитьСтроку(const std::string& line, int threadId) {
    Печать("[Поток " + std::to_string(threadId) + "] старт: " + ТекущееВремя());

    std::vector<Команда*> commands = parseLine(line);
    if (commands.empty()) {
        Печать("Ошибка парсинга");
    }

    for (Команда* cmd : commands) {
        cmd->execute();
        delete cmd;
    }

    Печать("[Поток " + std::to_string(threadId) + "] финиш: " + ТекущееВремя());
}

int main()
{
#ifdef _WIN32
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
#endif

    std::vector<std::string> lines; // сначала собираем строки
    std::string line;

    while (std::getline(std::cin, line)) {
        if (line == "СТОП") break;
        if (line.empty()) continue;
        lines.push_back(line);
    }

    std::vector<std::thread> threads; // потом запускаем все сразу
    int id = 1;
    for (const std::string& l : lines) {
        threads.emplace_back(ВыполнитьСтроку, l, id++);
    }

    for (std::thread& t : threads) {
        t.join();
    }


    std::cin.get();
    return 0;
}
