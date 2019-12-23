#ifndef ASIO_HTTPD_PATH
#define ASIO_HTTPD_PATH

#include <string>
#include <vector>

#include <stdexcept>

namespace asio_httpd {

// Supports Unix-style absolute paths only
// Always normalizes the given path

class Path
    {
    public:
    Path();
    Path(const std::string& path);
    ~Path();

    void set(const std::string& path);
    void append(const Path& path);

    bool operator==(const Path& path) const;
    bool operator<(const Path& path) const;

    std::string str() const;
    bool is_in_subtree(const Path& path) const;
    unsigned int num_elements() const;
    bool is_absolute() const;
    Path difference(const Path& path) const;

    class MalformedPath : public std::runtime_error
        {
        public: MalformedPath(const std::string& msg) : runtime_error(msg) {}
        };

    class NotInSubtree : public std::runtime_error
        {
        public: NotInSubtree(const std::string& msg) : runtime_error(msg) {}
        };

    private:
    std::vector<std::string> m_components;
    };

std::ostream& operator<<(std::ostream& out, const Path& p);

}

#endif
