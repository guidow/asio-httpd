#ifndef ASIO_HTTPD_PATH
#define ASIO_HTTPD_PATH

#include <string>
#include <vector>

#include <stdexcept>

namespace asio_httpd {

// Supports Unix-style absolute paths only
// Always normalizes the given path
// We do not use std::filesystem::path because that has slightly different
// semantics from what we need for HTTP.
// TODO: Elaborate on those differences.

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

    /* Determines if this path is in a subtree of path */
    bool is_in_subtree(const Path& path) const;

    /* If this is a subtree of other_path, return the path components that
       are not part of other_path.
       Example: Path("/a/b/c").difference("/a/") returns Path("b/c")
       Throws NotInSubtree if this is not in a subtree of other_path.*/
    Path difference(const Path& path) const;
    unsigned int num_elements() const;
    bool is_absolute() const;

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
