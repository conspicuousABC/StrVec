#include <iostream>
#include <string>
#include <memory>
#include <utility>

using namespace std;

class StrVec
{
public:
    size_t size() const
    {
        return first_free - element;
    }
    size_t capacity() const
    {
        return cap - element;
    }
    string* begin() const
    {
        return element;
    }
    string* end() const
    {
        return first_free;
    }

    StrVec() : element(nullptr), first_free(nullptr), cap(nullptr){}
    StrVec(const StrVec&);
    StrVec& operator=(const StrVec&);
    ~StrVec();

    void push_back(const string&);

    string& operator[](size_t n)
    {
        return element[n];
    }
    const string& operator[](size_t n) const
    {
        return element[n];
    }

private:
    static allocator<string> alloc;
    string* element;
    string* first_free;
    string* cap;

    pair<string*, string*> alloc_n_copy(const string*, const string*);
    void free();

    void reallocate();
    void chk_n_alloc();
};

allocator<string> StrVec::alloc;
  
pair<string*, string*> StrVec::alloc_n_copy(const string* b, const string* e)
{
    auto data = alloc.allocate(e - b);
    return {data, uninitialized_copy(b, e, data)};
}

void StrVec::free()
{
    if (element)
    {
        for (auto p = first_free; p != element;)
        {
            alloc.destroy(--p);
        }
        alloc.deallocate(element, cap - element);
    }
}

StrVec::StrVec(const StrVec& s)
{
    auto data = alloc_n_copy(s.begin(), s.end());
    element = data.first;
    first_free = cap = data.second;
}

StrVec& StrVec::operator=(const StrVec& rhs)
{
    auto data = alloc_n_copy(rhs.begin(), rhs.end());
    free();
    element = data.first;
    first_free = cap = data.second;

    return *this;
}

StrVec::~StrVec()
{
    free();
}

void StrVec::reallocate()
{
    auto newcap = size() ? 2 * size() : 1;

    auto data = alloc.allocate(newcap);
    auto dest = data;
    auto origin = element;
    for (size_t i = 0; i != size(); ++i)
    {
        alloc.construct(dest++, std::move(*origin++));
    }

    free();

    element = data;
    first_free = dest;
    cap = element + newcap;
}

void StrVec::chk_n_alloc()
{
    if (size() == capacity())
    {
        reallocate();
    }
}

void StrVec::push_back(const string& s)
{
    chk_n_alloc();
    alloc.construct(first_free++, s);
}

int main()
{
    StrVec sv1;
    sv1.push_back("Chen Zhangquan");
    sv1.push_back("China");
    sv1.push_back("Fuzhou");

    StrVec sv2(sv1);
    StrVec sv3 = sv1;

    for (size_t i = 0; i != sv2.size(); ++i)
    {
        cout << sv2[i] << " ";
    }
    cout << endl;

    for (size_t i = 0; i != sv3.size(); ++i)
    {
        cout << sv3[i] << " ";
    }
    cout << endl;

    return 0;
}

